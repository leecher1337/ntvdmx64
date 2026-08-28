/*++

Copyright (c) 2026  leecher@dose.0wnz.at  (Hyper-V / WHP port)

Module Name:

    Monitor.c

Abstract:

    User-mode portion of the NTVDM monitor, Windows Hypervisor Platform
    backend.  This file mirrors haxm/x86/monitor.c one-for-one.  The
    register get/set helpers and the BOP/exit dispatch logic are unchanged;
    only the hypervisor primitives (partition / VCPU creation, the RunVp
    loop, exception bitmap, interrupt injection) have been retargeted from
    HAXM IOCTLs to WHvPlatform APIs.

Environment:

    User mode only

--*/

#include "monitorp.h"
#include "insignia.h"
#include "host_def.h"

/* WHP_DEBUG_VERBOSE gates per-call OutputDebugStringA logs across the
 * IRQ injection, ICA, timer, and keyboard paths.  Default OFF.
 */
#ifndef WHP_DEBUG_VERBOSE
#define WHP_DEBUG_VERBOSE 0
#endif

#include "gvi.h"
#include "xt.h"
#include "../../../softpc.new/base/cvidc/gdpvar.h"
#include "sas4gen.h"
#include "yoda.h"
/* Pull in the softpc disassembler entry points: decode() gives us
 * instruction length without having to maintain a separate length table.
 * DASM_INTERNAL gates the dasm_internal/decode prototypes in dasm.h. */
#define DASM_INTERNAL
#include "dasm.h"
/* Decoded-operand types (A_M*, A_R*, A_I, DCD_* accessors) and instruction
 * ids (I_MOV*, I_MOVS*, I_STOS*, I_LODS*) used by hyperv_emulate_vram_access
 * to replay A0000 EGA accesses.  decode.h (via dasm.h) does not pull these in. */
#include "d_oper.h"
#include "d_inst.h"
#undef DBG

extern host_addr Start_of_M_area;
extern host_addr Length_of_M_area;
extern const char *name_vmx_exit(int value);
#undef pNtVDMState
#define pNtVDMState   ((ULONG *)(Start_of_M_area + FIXED_NTVDMSTATE_LINEAR))

struct hax_debug_t haxdbg = {0};

BOOL show_exceptions = FALSE;
BOOL trap_exceptions = FALSE;

extern int  ErrorDialogBox(char *message, char *Edit, DWORD dwOptions);
extern GLOBAL VOID haxm_sync_vram(VOID);
extern BOOL host_hwint_hook IPT1(IS32, int_no);
extern BOOL host_swint_hook IPT1(IS32, int_no);
extern BOOL host_exint_hook IPT2(IS32, exp_no, IS32, error_code);
extern VOID data_debug_exception(VOID);            /* yoda.c: DR data-BP handler */
extern int  yoda_data_break_pending(unsigned long dr6); /* yoda.c: DR6 hit vs active data BPs (IU32) */
extern BOOL show_exceptions;                       /* yoda `vex` toggle (c_xcptn.c) */
extern BOOL trap_exceptions;                       /* yoda `bex` toggle (c_xcptn.c) */
extern IU8  sas_hw_at IPT1(IU32, addr);            /* sas4gen byte read (returns IU8) */
ULONG getPE(VOID);                                 /* defined below; forward-declare */
ULONG getVM(VOID);                                 /* so early callers don't assume int */
extern VOID host_pit_loop_tick(VOID);              /* nt_timer.c: loop-paced PIT time_tick (raises IRQ0) */
extern unsigned long GetTimer0InitialCount(void);  /* timer.c: current timer-0 reload */

/* I/O exit fast-path master switch (the ~2-3x win that skips register
 * marshalling for simple IN/OUT).  Set 0 to force every I/O through the full
 * slow path -- use to bisect whether the fast-path's tight burst (which skips
 * DispatchInterrupts/host_timer_event between exits) is starving time-based
 * device state, e.g. SKYROADS' OPL-timer status poll hanging. */
#define WHP_IO_FASTPATH 1

/* Consecutive (IRQ-pending && guest-IF=0) loop iterations before we arm WHP's
 * interrupt-window notification (see hyperv_arm_interrupt_window).  High enough
 * that a normally-delivering guest's brief transient IF=0 windows never trip it
 * (only a genuine sustained disabled-interrupt spin like Keen 4's "Loading"
 * loop does), so we don't multiply intack/inject events on healthy guests.
 * Keen's loop exits ~13000x/s -> ~20ms to recover; DOOM never reaches it. */
#define INTWIN_STUCK_ITERS 256

/* PIT-rate timer NUDGE thread (sound timing).  The 18 Hz heartbeat already
 * GENERATES the right tick count; the problem (PERF-TMR) is DELIVERY: the timer
 * IRQ is injected at the top of cpu_simulate, so when the guest goes compute-
 * bound (SKYROADS free-runs ~2.9 ms/exit -> only ~310 loop iters/s) the loop
 * rarely reaches the injection point -> ticks pile up in EoiIntsPending and get
 * dropped (eoi collapses 162->72).  This thread CANCELS the run at the PIT rate
 * to force loop iterations so the pending timer gets injected promptly --
 * decoupling delivery from the guest's exit rate.  It does NOT call time_tick
 * (that over-generated, since timer-0 updateCount returns a fixed 9/call).
 * RE-ENABLED (1): the apparent idle/input-wait "regression" was actually
 * NTVDM's normal cursor-out background CPU throttling (not the nudge) -- in the
 * foreground the nudge kept delivery up (SKYROADS eoi 72->155).  The separate
 * ~9% generation truncation is now fixed in timer.c (remainder-carry). */
#define WHP_PIT_TIMER_THREAD 1

VOID EventVdmIo(VOID);
VOID EventVdmMemAccess(VOID);
VOID EventVdmBop(VOID);
VOID EventVdmError(VOID);
VOID EventVdmIrq13(VOID);

void hax_handle_fastmmio(struct hax_fastmmio *hft);
void hax_handle_fastmmio_op(struct hax_fastmmio *hft);

/* ---- WHP backend state ----
 *
 * g_Partition / g_VpIndex are the only handles needed; WHP has no "system"
 * device handle equivalent to HAXM's \\.\HAX.  Visible to sas.c and int.c.
 */
WHV_PARTITION_HANDLE g_Partition = 0;
UINT32               g_VpIndex   = 0;

/* Compatibility aliases so existing references in sas.c (extern hVM/hVCPU)
 * still link.  They are not used as device handles, just as truthy markers.
 */
HANDLE hVM    = (HANDLE)(LONG_PTR)-2;
HANDLE hVCPU  = (HANDLE)(LONG_PTR)-2;

struct hax_tunnel *tunnel = NULL;
PBYTE              iobuf  = NULL;

/* A static tunnel + io buffer instead of the HAXM shared-memory mailbox. */
static struct hax_tunnel s_tunnel;
static BYTE              s_iobuf[16];

struct vcpu_state_t state = {0};

/* The WHP exception-exit bitmask we asked for; mirrored so we can panic
 * with a helpful message if WHP refused the setup. */
static UINT64 s_exception_exit_bitmap = 0;

VDM_TIB VdmTib;

ULONG   IntelBase;
ULONG   VdmSize;
ULONG   VdmDebugLevel;
ULONG   InitialBreakpoint = FALSE;
ULONG   InitialVdmTibFlags = 0;
CONTEXT InitialContext;
BOOLEAN DebugContextActive = FALSE;
ULONG   VdmFeatureBits = 0;

extern void (*BIOS[])();

BOOLEAN ContinueExecution;

BOOLEAN fShowBop = TRUE;
#if DBG
BOOLEAN fBreakInDebugger = FALSE;
#endif
LONG NumTasks = -1;

void haxmvm_panic(const char *fmt, ...)
{
    char ErrMsg[512];
    va_list arg;

    va_start(arg, fmt);
    _vsnprintf(ErrMsg, sizeof(ErrMsg), fmt, arg);
    va_end(arg);
    ErrMsg[sizeof(ErrMsg) - 1] = '\0';
    ErrorDialogBox(ErrMsg, NULL, RMB_ICON_STOP | RMB_ABORT | RMB_IGNORE);
    EventVdmError();
}

/* ------------------------------------------------------------------------ */
/*  WHP register marshalling (per-group, dirty-tracked).
 *
 *  Registers are partitioned into five groups (see REG_GROUP_* in
 *  hypervvm.h).  Each group has its own WHV_REGISTER_NAME array and its
 *  own push/pull helper.  The cpu_simulate loop:
 *
 *    - after RunVp: calls hyperv_fetch(<mask for this exit reason>) which
 *      pulls only the groups the handler is going to read;
 *    - on its way back into RunVp: calls hyperv_flush() which pushes only
 *      the groups that get/set helpers (or direct writes that called
 *      hyperv_mark_dirty()) actually touched since the last entry, and
 *      then clears the dirty mask.
 *
 *  The full-state convenience wrappers hyperv_load_state() and
 *  hyperv_store_state() route through the same machinery.
 * ------------------------------------------------------------------------ */

static uint32_t g_dirty_mask = 0;

void hyperv_mark_dirty(uint32_t groups) { g_dirty_mask |= groups; }

static void segment_to_whv(const segment_desc_t *s, WHV_X64_SEGMENT_REGISTER *out)
{
    out->Base       = s->base;
    out->Limit      = s->limit;
    out->Selector   = s->selector;
    out->Attributes = (UINT16)(s->ar & 0xF0FF);
}

static void whv_to_segment(const WHV_X64_SEGMENT_REGISTER *in, segment_desc_t *s)
{
    s->base     = in->Base;
    s->limit    = in->Limit;
    s->selector = in->Selector;
    s->ar       = in->Attributes;
}

static void table_to_whv(const segment_desc_t *s, WHV_X64_TABLE_REGISTER *out)
{
    out->Base  = s->base;
    out->Limit = (UINT16)s->limit;
}

static void whv_to_table(const WHV_X64_TABLE_REGISTER *in, segment_desc_t *s)
{
    s->base  = in->Base;
    s->limit = in->Limit;
}

/* ---------------- GPRs (RAX..R15, RIP, RFLAGS) ---------------- */

static const WHV_REGISTER_NAME k_GprNames[] = {
    WHvX64RegisterRax, WHvX64RegisterRcx, WHvX64RegisterRdx, WHvX64RegisterRbx,
    WHvX64RegisterRsp, WHvX64RegisterRbp, WHvX64RegisterRsi, WHvX64RegisterRdi,
    WHvX64RegisterR8,  WHvX64RegisterR9,  WHvX64RegisterR10, WHvX64RegisterR11,
    WHvX64RegisterR12, WHvX64RegisterR13, WHvX64RegisterR14, WHvX64RegisterR15,
    WHvX64RegisterRip, WHvX64RegisterRflags
};
#define K_GPR_COUNT (sizeof(k_GprNames) / sizeof(k_GprNames[0]))

static void push_gprs(void)
{
    WHV_REGISTER_VALUE v[K_GPR_COUNT] = {0};
    HRESULT hr;
    int i = 0;
    v[i++].Reg64 = state._rax;
    v[i++].Reg64 = state._rcx;
    v[i++].Reg64 = state._rdx;
    v[i++].Reg64 = state._rbx;
    v[i++].Reg64 = state._rsp;
    v[i++].Reg64 = state._rbp;
    v[i++].Reg64 = state._rsi;
    v[i++].Reg64 = state._rdi;
    v[i++].Reg64 = state._r8;
    v[i++].Reg64 = state._r9;
    v[i++].Reg64 = state._r10;
    v[i++].Reg64 = state._r11;
    v[i++].Reg64 = state._r12;
    v[i++].Reg64 = state._r13;
    v[i++].Reg64 = state._r14;
    v[i++].Reg64 = state._r15;
    v[i++].Reg64 = state._rip;
    v[i++].Reg64 = state._rflags;
    hr = WHvSetVirtualProcessorRegisters(g_Partition, g_VpIndex,
                                         k_GprNames, (UINT32)K_GPR_COUNT, v);
    if (FAILED(hr))
        haxmvm_panic("WHvSetVirtualProcessorRegisters(GPRs) failed (0x%08X)", hr);
}

static void pull_gprs(void)
{
    WHV_REGISTER_VALUE v[K_GPR_COUNT] = {0};
    HRESULT hr;
    int i = 0;
    hr = WHvGetVirtualProcessorRegisters(g_Partition, g_VpIndex,
                                         k_GprNames, (UINT32)K_GPR_COUNT, v);
    if (FAILED(hr))
        haxmvm_panic("WHvGetVirtualProcessorRegisters(GPRs) failed (0x%08X)", hr);
    state._rax = v[i++].Reg64;
    state._rcx = v[i++].Reg64;
    state._rdx = v[i++].Reg64;
    state._rbx = v[i++].Reg64;
    state._rsp = v[i++].Reg64;
    state._rbp = v[i++].Reg64;
    state._rsi = v[i++].Reg64;
    state._rdi = v[i++].Reg64;
    state._r8  = v[i++].Reg64;
    state._r9  = v[i++].Reg64;
    state._r10 = v[i++].Reg64;
    state._r11 = v[i++].Reg64;
    state._r12 = v[i++].Reg64;
    state._r13 = v[i++].Reg64;
    state._r14 = v[i++].Reg64;
    state._r15 = v[i++].Reg64;
    state._rip = v[i++].Reg64;
    state._rflags = v[i++].Reg64;
}

/* ---------------- Segments (CS,SS,DS,ES,FS,GS,LDTR,TR) ---------------- */

static const WHV_REGISTER_NAME k_SegNames[] = {
    WHvX64RegisterCs,  WHvX64RegisterSs,  WHvX64RegisterDs,  WHvX64RegisterEs,
    WHvX64RegisterFs,  WHvX64RegisterGs,  WHvX64RegisterLdtr, WHvX64RegisterTr
};
#define K_SEG_COUNT (sizeof(k_SegNames) / sizeof(k_SegNames[0]))

static void push_segs(void)
{
    WHV_REGISTER_VALUE v[K_SEG_COUNT] = {0};
    HRESULT hr;
    int i = 0;
    segment_to_whv(&state._cs,  &v[i++].Segment);
    segment_to_whv(&state._ss,  &v[i++].Segment);
    segment_to_whv(&state._ds,  &v[i++].Segment);
    segment_to_whv(&state._es,  &v[i++].Segment);
    segment_to_whv(&state._fs,  &v[i++].Segment);
    segment_to_whv(&state._gs,  &v[i++].Segment);
    segment_to_whv(&state._ldt, &v[i++].Segment);
    segment_to_whv(&state._tr,  &v[i++].Segment);
    hr = WHvSetVirtualProcessorRegisters(g_Partition, g_VpIndex,
                                         k_SegNames, (UINT32)K_SEG_COUNT, v);
    if (FAILED(hr))
        haxmvm_panic("WHvSetVirtualProcessorRegisters(SEGS) failed (0x%08X)", hr);
}

static void pull_segs(void)
{
    WHV_REGISTER_VALUE v[K_SEG_COUNT] = {0};
    HRESULT hr;
    int i = 0;
    hr = WHvGetVirtualProcessorRegisters(g_Partition, g_VpIndex,
                                         k_SegNames, (UINT32)K_SEG_COUNT, v);
    if (FAILED(hr))
        haxmvm_panic("WHvGetVirtualProcessorRegisters(SEGS) failed (0x%08X)", hr);
    whv_to_segment(&v[i++].Segment, &state._cs);
    whv_to_segment(&v[i++].Segment, &state._ss);
    whv_to_segment(&v[i++].Segment, &state._ds);
    whv_to_segment(&v[i++].Segment, &state._es);
    whv_to_segment(&v[i++].Segment, &state._fs);
    whv_to_segment(&v[i++].Segment, &state._gs);
    whv_to_segment(&v[i++].Segment, &state._ldt);
    whv_to_segment(&v[i++].Segment, &state._tr);
}

/* ---------------- Minimal fetch for the trapped-VRAM path ----------------
 * Only the 13 registers hyperv_emulate_vram_access reads: the low-8 GPRs +
 * RIP + RFLAGS + CS/DS/ES, in ONE hypercall.  PERF-VRAM showed fetch cost is
 * register-count-bound, so 13 vs the 26 of GPRS|SEGS ~halves it.
 *
 * Safe ONLY because a trapped-VRAM exit now does a GPRS-ONLY push (the
 * g_vram_only_gprs_dirty path): the un-fetched SS/FS/GS/LDTR/TR are never
 * written back, so their staleness cannot corrupt the guest.  R8-R15 ARE
 * written by push_gprs but are always 0 here (the guest is real-mode whenever
 * a planar EGA aperture is trapped -- they're architecturally inaccessible),
 * and state starts zeroed, so writing them back is a no-op.  CR0 is left
 * stale -> code_32 uses the real-mode assumption (valid for the trapped
 * modes), exactly as the old GPRS|SEGS path did (it didn't fetch CR0 either). */
static const WHV_REGISTER_NAME k_VramNames[] = {
    WHvX64RegisterRax, WHvX64RegisterRcx, WHvX64RegisterRdx, WHvX64RegisterRbx,
    WHvX64RegisterRsp, WHvX64RegisterRbp, WHvX64RegisterRsi, WHvX64RegisterRdi,
    WHvX64RegisterRip, WHvX64RegisterRflags,
    WHvX64RegisterCs,  WHvX64RegisterDs,  WHvX64RegisterEs
};
#define K_VRAM_COUNT (sizeof(k_VramNames) / sizeof(k_VramNames[0]))

static void pull_vram(void)
{
    WHV_REGISTER_VALUE v[K_VRAM_COUNT] = {0};
    HRESULT hr;
    int i = 0;
    hr = WHvGetVirtualProcessorRegisters(g_Partition, g_VpIndex,
                                         k_VramNames, (UINT32)K_VRAM_COUNT, v);
    if (FAILED(hr))
        haxmvm_panic("WHvGetVirtualProcessorRegisters(VRAM) failed (0x%08X)", hr);
    state._rax = v[i++].Reg64;
    state._rcx = v[i++].Reg64;
    state._rdx = v[i++].Reg64;
    state._rbx = v[i++].Reg64;
    state._rsp = v[i++].Reg64;
    state._rbp = v[i++].Reg64;
    state._rsi = v[i++].Reg64;
    state._rdi = v[i++].Reg64;
    state._rip = v[i++].Reg64;
    state._rflags = v[i++].Reg64;
    whv_to_segment(&v[i++].Segment, &state._cs);
    whv_to_segment(&v[i++].Segment, &state._ds);
    whv_to_segment(&v[i++].Segment, &state._es);
}

/* ---------------- Tables (GDTR, IDTR) ---------------- */

static const WHV_REGISTER_NAME k_TableNames[] = {
    WHvX64RegisterGdtr, WHvX64RegisterIdtr
};
#define K_TABLE_COUNT (sizeof(k_TableNames) / sizeof(k_TableNames[0]))

static void push_tables(void)
{
    WHV_REGISTER_VALUE v[K_TABLE_COUNT] = {0};
    HRESULT hr;
    int i = 0;
    table_to_whv(&state._gdt, &v[i++].Table);
    table_to_whv(&state._idt, &v[i++].Table);
    hr = WHvSetVirtualProcessorRegisters(g_Partition, g_VpIndex,
                                         k_TableNames, (UINT32)K_TABLE_COUNT, v);
    if (FAILED(hr))
        haxmvm_panic("WHvSetVirtualProcessorRegisters(TABLES) failed (0x%08X)", hr);
}

static void pull_tables(void)
{
    WHV_REGISTER_VALUE v[K_TABLE_COUNT] = {0};
    HRESULT hr;
    int i = 0;
    hr = WHvGetVirtualProcessorRegisters(g_Partition, g_VpIndex,
                                         k_TableNames, (UINT32)K_TABLE_COUNT, v);
    if (FAILED(hr))
        haxmvm_panic("WHvGetVirtualProcessorRegisters(TABLES) failed (0x%08X)", hr);
    whv_to_table(&v[i++].Table, &state._gdt);
    whv_to_table(&v[i++].Table, &state._idt);
}

/* ---------------- Control regs (CR0/CR2/CR3/CR4 + EFER) ---------------- */

static const WHV_REGISTER_NAME k_CtrlNames[] = {
    WHvX64RegisterCr0, WHvX64RegisterCr2, WHvX64RegisterCr3, WHvX64RegisterCr4,
    WHvX64RegisterEfer
};
#define K_CTRL_COUNT (sizeof(k_CtrlNames) / sizeof(k_CtrlNames[0]))

static void push_ctrl(void)
{
    WHV_REGISTER_VALUE v[K_CTRL_COUNT] = {0};
    HRESULT hr;
    int i = 0;
    v[i++].Reg64 = state._cr0;
    v[i++].Reg64 = state._cr2;
    v[i++].Reg64 = state._cr3;
    v[i++].Reg64 = state._cr4;
    v[i++].Reg64 = state._efer;
    hr = WHvSetVirtualProcessorRegisters(g_Partition, g_VpIndex,
                                         k_CtrlNames, (UINT32)K_CTRL_COUNT, v);
    if (FAILED(hr))
        haxmvm_panic("WHvSetVirtualProcessorRegisters(CTRL) failed (0x%08X)", hr);
}

static void pull_ctrl(void)
{
    WHV_REGISTER_VALUE v[K_CTRL_COUNT] = {0};
    HRESULT hr;
    int i = 0;
    hr = WHvGetVirtualProcessorRegisters(g_Partition, g_VpIndex,
                                         k_CtrlNames, (UINT32)K_CTRL_COUNT, v);
    if (FAILED(hr))
        haxmvm_panic("WHvGetVirtualProcessorRegisters(CTRL) failed (0x%08X)", hr);
    state._cr0  = v[i++].Reg64;
    state._cr2  = v[i++].Reg64;
    state._cr3  = v[i++].Reg64;
    state._cr4  = v[i++].Reg64;
    state._efer = (uint32)v[i++].Reg64;
}

/* ---------------- Debug regs (DR0-DR3, DR6, DR7) ---------------- */

static const WHV_REGISTER_NAME k_DebugNames[] = {
    WHvX64RegisterDr0, WHvX64RegisterDr1, WHvX64RegisterDr2, WHvX64RegisterDr3,
    WHvX64RegisterDr6, WHvX64RegisterDr7
};
#define K_DEBUG_COUNT (sizeof(k_DebugNames) / sizeof(k_DebugNames[0]))

static void push_debug(void)
{
    WHV_REGISTER_VALUE v[K_DEBUG_COUNT] = {0};
    HRESULT hr;
    int i = 0;
    v[i++].Reg64 = state._dr0;
    v[i++].Reg64 = state._dr1;
    v[i++].Reg64 = state._dr2;
    v[i++].Reg64 = state._dr3;
    v[i++].Reg64 = state._dr6;
    v[i++].Reg64 = state._dr7;
    hr = WHvSetVirtualProcessorRegisters(g_Partition, g_VpIndex,
                                         k_DebugNames, (UINT32)K_DEBUG_COUNT, v);
    if (FAILED(hr))
        haxmvm_panic("WHvSetVirtualProcessorRegisters(DEBUG) failed (0x%08X)", hr);
}

static void pull_debug(void)
{
    WHV_REGISTER_VALUE v[K_DEBUG_COUNT] = {0};
    HRESULT hr;
    int i = 0;
    hr = WHvGetVirtualProcessorRegisters(g_Partition, g_VpIndex,
                                         k_DebugNames, (UINT32)K_DEBUG_COUNT, v);
    if (FAILED(hr))
        haxmvm_panic("WHvGetVirtualProcessorRegisters(DEBUG) failed (0x%08X)", hr);
    state._dr0 = v[i++].Reg64;
    state._dr1 = v[i++].Reg64;
    state._dr2 = v[i++].Reg64;
    state._dr3 = v[i++].Reg64;
    state._dr6 = v[i++].Reg64;
    state._dr7 = v[i++].Reg64;
}

/* ---------------- Combined ALL-group fast path ----------------
 *
 * The original per-group push/pull pattern does 5 separate hypercalls
 * (one per group: GPRs/SEGS/TABLES/CTRL/DEBUG) per full snapshot.  When
 * cpu_simulate's main loop does mark-all-dirty + flush + WHvRunVp +
 * fetch-all every iteration, that's 10 hypercalls of marshalling
 * overhead per iteration.
 *
 * WHvGet/SetVirtualProcessorRegisters happily takes an arbitrary list
 * of register names in one call, so we can collapse all 39 names into
 * one hypercall per direction.  Same data moved, 5x fewer hypercalls.
 *
 * The per-group push_xxx / pull_xxx functions are kept intact for the
 * partial-fetch cases inside the exit dispatcher (e.g. IO exits only
 * need GPRs+SEGS).  Only the REG_GROUP_ALL path is routed through here.
 *
 * Layout of k_AllNames must exactly match the gather/scatter order in
 * push_all / pull_all below -- changes to one MUST update the other. */

#define K_ALL_COUNT (K_GPR_COUNT + K_SEG_COUNT + K_TABLE_COUNT \
                   + K_CTRL_COUNT + K_DEBUG_COUNT + 1 /* InterruptState */)

static const WHV_REGISTER_NAME k_AllNames[K_ALL_COUNT] = {
    /* [0..17] GPRs */
    WHvX64RegisterRax, WHvX64RegisterRcx, WHvX64RegisterRdx, WHvX64RegisterRbx,
    WHvX64RegisterRsp, WHvX64RegisterRbp, WHvX64RegisterRsi, WHvX64RegisterRdi,
    WHvX64RegisterR8,  WHvX64RegisterR9,  WHvX64RegisterR10, WHvX64RegisterR11,
    WHvX64RegisterR12, WHvX64RegisterR13, WHvX64RegisterR14, WHvX64RegisterR15,
    WHvX64RegisterRip, WHvX64RegisterRflags,
    /* [18..25] Segments */
    WHvX64RegisterCs,  WHvX64RegisterSs,  WHvX64RegisterDs,  WHvX64RegisterEs,
    WHvX64RegisterFs,  WHvX64RegisterGs,  WHvX64RegisterLdtr, WHvX64RegisterTr,
    /* [26..27] Tables */
    WHvX64RegisterGdtr, WHvX64RegisterIdtr,
    /* [28..32] Control */
    WHvX64RegisterCr0, WHvX64RegisterCr2, WHvX64RegisterCr3, WHvX64RegisterCr4,
    WHvX64RegisterEfer,
    /* [33..38] Debug */
    WHvX64RegisterDr0, WHvX64RegisterDr1, WHvX64RegisterDr2, WHvX64RegisterDr3,
    WHvX64RegisterDr6, WHvX64RegisterDr7,
    /* [39] Interrupt state (STI / MOV-SS interrupt shadow + NMI mask). */
    WHvRegisterInterruptState
};

static void push_all(void)
{
    WHV_REGISTER_VALUE v[K_ALL_COUNT] = {0};
    HRESULT hr;
    int i = 0;

    /* GPRs */
    v[i++].Reg64 = state._rax;
    v[i++].Reg64 = state._rcx;
    v[i++].Reg64 = state._rdx;
    v[i++].Reg64 = state._rbx;
    v[i++].Reg64 = state._rsp;
    v[i++].Reg64 = state._rbp;
    v[i++].Reg64 = state._rsi;
    v[i++].Reg64 = state._rdi;
    v[i++].Reg64 = state._r8;
    v[i++].Reg64 = state._r9;
    v[i++].Reg64 = state._r10;
    v[i++].Reg64 = state._r11;
    v[i++].Reg64 = state._r12;
    v[i++].Reg64 = state._r13;
    v[i++].Reg64 = state._r14;
    v[i++].Reg64 = state._r15;
    v[i++].Reg64 = state._rip;
    v[i++].Reg64 = state._rflags;
    /* Segments */
    segment_to_whv(&state._cs,  &v[i++].Segment);
    segment_to_whv(&state._ss,  &v[i++].Segment);
    segment_to_whv(&state._ds,  &v[i++].Segment);
    segment_to_whv(&state._es,  &v[i++].Segment);
    segment_to_whv(&state._fs,  &v[i++].Segment);
    segment_to_whv(&state._gs,  &v[i++].Segment);
    segment_to_whv(&state._ldt, &v[i++].Segment);
    segment_to_whv(&state._tr,  &v[i++].Segment);
    /* Tables */
    table_to_whv(&state._gdt, &v[i++].Table);
    table_to_whv(&state._idt, &v[i++].Table);
    /* Control */
    v[i++].Reg64 = state._cr0;
    v[i++].Reg64 = state._cr2;
    v[i++].Reg64 = state._cr3;
    v[i++].Reg64 = state._cr4;
    v[i++].Reg64 = state._efer;
    /* Debug */
    v[i++].Reg64 = state._dr0;
    v[i++].Reg64 = state._dr1;
    v[i++].Reg64 = state._dr2;
    v[i++].Reg64 = state._dr3;
    v[i++].Reg64 = state._dr6;
    v[i++].Reg64 = state._dr7;
    /* Interrupt state (STI / MOV-SS shadow + NMI mask).
     *
     * This MUST ride in the same WHvSetVirtualProcessorRegisters call as
     * RFLAGS / RIP / SS above: on WHP, writing those architectural registers
     * resets the VP's interrupt shadow.  Because cpu_simulate re-pushes the
     * full register set every iteration, a run that exited *inside* a MOV SS /
     * POP SS / STI shadow -- e.g. a worker-thread WHvCancelRunVirtualProcessor
     * landing right after a `mov ss,[]` (the timer fires ~18x/sec, so this is
     * a routine race, not an edge case) -- would otherwise resume with the
     * shadow silently cleared.  WHP then delivers a pending interrupt between
     * the MOV SS and the following SP load, pushing onto the half-loaded
     * SS:SP and derailing the guest (chkcpu's stack-switch epilogue faults
     * and spins forever in the unexp_nt handler).  Restoring the shadow we
     * read back in pull_all keeps the SS-load + next instruction atomic, as
     * the hardware requires.  WHP collapses STI- and MOV-SS-blocking into one
     * InterruptShadow bit. */
    {
        WHV_X64_INTERRUPT_STATE_REGISTER is;
        is.AsUINT64        = 0;
        is.InterruptShadow = (state._interruptibility_state.sti_blocking |
                              state._interruptibility_state.movss_blocking) ? 1 : 0;
        is.NmiMasked       = state._interruptibility_state.nmi_blocking ? 1 : 0;
        v[i++].InterruptState = is;
    }

    hr = WHvSetVirtualProcessorRegisters(g_Partition, g_VpIndex,
                                         k_AllNames, (UINT32)K_ALL_COUNT, v);
    if (FAILED(hr))
        haxmvm_panic("WHvSetVirtualProcessorRegisters(ALL) failed (0x%08X)", hr);
}

static void pull_all(void)
{
    WHV_REGISTER_VALUE v[K_ALL_COUNT] = {0};
    HRESULT hr;
    int i = 0;

    hr = WHvGetVirtualProcessorRegisters(g_Partition, g_VpIndex,
                                         k_AllNames, (UINT32)K_ALL_COUNT, v);
    if (FAILED(hr))
        haxmvm_panic("WHvGetVirtualProcessorRegisters(ALL) failed (0x%08X)", hr);

    /* GPRs */
    state._rax = v[i++].Reg64;
    state._rcx = v[i++].Reg64;
    state._rdx = v[i++].Reg64;
    state._rbx = v[i++].Reg64;
    state._rsp = v[i++].Reg64;
    state._rbp = v[i++].Reg64;
    state._rsi = v[i++].Reg64;
    state._rdi = v[i++].Reg64;
    state._r8  = v[i++].Reg64;
    state._r9  = v[i++].Reg64;
    state._r10 = v[i++].Reg64;
    state._r11 = v[i++].Reg64;
    state._r12 = v[i++].Reg64;
    state._r13 = v[i++].Reg64;
    state._r14 = v[i++].Reg64;
    state._r15 = v[i++].Reg64;
    state._rip = v[i++].Reg64;
    state._rflags = v[i++].Reg64;
    /* Segments */
    whv_to_segment(&v[i++].Segment, &state._cs);
    whv_to_segment(&v[i++].Segment, &state._ss);
    whv_to_segment(&v[i++].Segment, &state._ds);
    whv_to_segment(&v[i++].Segment, &state._es);
    whv_to_segment(&v[i++].Segment, &state._fs);
    whv_to_segment(&v[i++].Segment, &state._gs);
    whv_to_segment(&v[i++].Segment, &state._ldt);
    whv_to_segment(&v[i++].Segment, &state._tr);
    /* Tables */
    whv_to_table(&v[i++].Table, &state._gdt);
    whv_to_table(&v[i++].Table, &state._idt);
    /* Control */
    state._cr0  = v[i++].Reg64;
    state._cr2  = v[i++].Reg64;
    state._cr3  = v[i++].Reg64;
    state._cr4  = v[i++].Reg64;
    state._efer = (uint32)v[i++].Reg64;
    /* Debug */
    state._dr0 = v[i++].Reg64;
    state._dr1 = v[i++].Reg64;
    state._dr2 = v[i++].Reg64;
    state._dr3 = v[i++].Reg64;
    state._dr6 = v[i++].Reg64;
    state._dr7 = v[i++].Reg64;
    /* Interrupt state -- preserve the STI / MOV-SS shadow across the
     * push/pull cycle (see push_all for why this matters).  WHP exposes a
     * single InterruptShadow bit covering both STI- and MOV-SS-blocking;
     * stash it in movss_blocking so push_all can OR it straight back. */
    {
        WHV_X64_INTERRUPT_STATE_REGISTER is = v[i++].InterruptState;
        state._interruptibility_state.raw            = 0;
        state._interruptibility_state.movss_blocking = is.InterruptShadow ? 1 : 0;
        state._interruptibility_state.nmi_blocking   = is.NmiMasked ? 1 : 0;
    }
}

/* ---------------- Public marshal API ---------------- */

void hyperv_fetch(uint32_t groups)
{
    /* Fast path: all-groups fetch goes through a single hypercall.
     * This is hot -- it's called every cpu_simulate iteration. */
    if ((groups & REG_GROUP_ALL) == REG_GROUP_ALL) {
        pull_all();
        return;
    }
    if (groups & REG_GROUP_GPRS)    pull_gprs();
    if (groups & REG_GROUP_SEGS)    pull_segs();
    if (groups & REG_GROUP_TABLES)  pull_tables();
    if (groups & REG_GROUP_CONTROL) pull_ctrl();
    if (groups & REG_GROUP_DEBUG)   pull_debug();
}

void hyperv_flush(void)
{
    uint32_t dirty = g_dirty_mask;
    g_dirty_mask = 0;
    /* Fast path: when every group is dirty (the common case after
     * mark-all-dirty), push everything in one hypercall instead of five. */
    if ((dirty & REG_GROUP_ALL) == REG_GROUP_ALL) {
        push_all();
        return;
    }
    if (dirty & REG_GROUP_GPRS)    push_gprs();
    if (dirty & REG_GROUP_SEGS)    push_segs();
    if (dirty & REG_GROUP_TABLES)  push_tables();
    if (dirty & REG_GROUP_CONTROL) push_ctrl();
    if (dirty & REG_GROUP_DEBUG)   push_debug();
}

void hyperv_load_state(void)  { hyperv_mark_dirty(REG_GROUP_ALL); hyperv_flush(); }
void hyperv_store_state(void) { hyperv_fetch(REG_GROUP_ALL); }

/* MARK is the convenience macro the get/set helpers use.  It is also used
 * from the exit dispatch in cpu_simulate when we mutate `state` directly
 * (e.g. `state._rip += 3`). */
#define MARK(g) hyperv_mark_dirty(g)

/* ------------------------------------------------------------------------ */
/*  Memory mapping wrappers used by sas.c.
 * ------------------------------------------------------------------------ */

/* Returns S_OK on success, otherwise the raw HRESULT from WHvMapGpaRange.
 * Callers want to see the actual error code; a BOOL with GetLastError()
 * doesn't help because WHv* APIs don't set the Win32 last-error.  The
 * most useful failure codes here are:
 *   0x80370301  WHV_E_INSUFFICIENT_BUFFER
 *   0x80070057  E_INVALIDARG   - gpa or size not 4-KB aligned, or overlaps
 *                                an existing mapping
 *   0x8007000E  E_OUTOFMEMORY  - the partition manager ran out of slots
 *   0x80370104  HV_STATUS_INVALID_PARTITION_STATE  - call after teardown */
HRESULT hyperv_map_gpa_range(uint64_t gpa, uint64_t size, void *host_va,
						   WHV_MAP_GPA_RANGE_FLAGS flags)
{
	/* WHP requires page-aligned size and gpa.  Callers from sas.c sometimes
	* pass byte-granular sizes (e.g. the actual size of a BIOS extension
	* ROM file).  Round up to the next 4 KB to keep them working without
	* touching every call site. */
	size = (size + 0xFFFu) & ~(uint64_t)0xFFFu;
	return WHvMapGpaRange(g_Partition, host_va, gpa, size, flags);
}

HRESULT hyperv_unmap_gpa_range(uint64_t gpa, uint64_t size)
{
    return WHvUnmapGpaRange(g_Partition, gpa, size);
}

BOOL hyperv_translate_gva(uint64_t gva, uint64_t *gpa)
{
    WHV_TRANSLATE_GVA_RESULT res = {0};
    UINT64 out_gpa = 0;
    HRESULT hr;

    hr = WHvTranslateGva(g_Partition, g_VpIndex, gva,
                         WHvTranslateGvaFlagValidateRead, &res, &out_gpa);
    if (FAILED(hr) || res.ResultCode != WHvTranslateGvaResultSuccess)
        return FALSE;
    *gpa = out_gpa;
    return TRUE;
}

BOOL hyperv_query_dirty(uint64_t gpa, uint64_t size,
                        UINT64 *bitmap, UINT32 bitmap_qwords)
{
    /* WHP returns one bit per 4 KB page, packed into UINT64 words; the
     * call clears the bits as a side effect so the next query reports
     * only newly-written pages.  Returns WHV_E_GPA_RANGE_NOT_FOUND when
     * the range wasn't mapped with WHvMapGpaRangeFlagTrackDirtyPages. */
    HRESULT hr;
    if (!bitmap || bitmap_qwords == 0) return FALSE;
    hr = WHvQueryGpaRangeDirtyBitmap(g_Partition, gpa, size, bitmap,
                                     (UINT32)(bitmap_qwords * sizeof(UINT64)));
    return SUCCEEDED(hr);
}

/* Inject an external interrupt at the next interrupt-window opportunity.
 * If IF=0 we ask WHP to wake us on the next window; otherwise we deliver
 * immediately by setting the pending interruption.
 */
/*
 * Wake a stuck WHvRunVirtualProcessor on the VCPU thread from another
 * thread.  WHP guarantees the running WHvRunVirtualProcessor returns
 * with ExitReason=WHvRunVpExitReasonCanceled.
 *
 * Called by:
 *   - cpu_interrupt() in int.c when a non-VCPU thread queues an IRQ.
 *     Without this, the main thread sits inside WHvRunVirtualProcessor
 *     forever if the VCPU is in HLT (IF=0) or in a tight CPU-bound
 *     loop with no I/O / BOPs / exceptions.
 *   - the CTRL+C handler, so YODA can break in.
 *
 * Safe to call from any thread.  Idempotent: extra cancels are dropped
 * once the cancellation has been delivered.
 */
DWORD g_cpu_simulate_tid = 0;   /* set the first time cpu_simulate enters,
                                 * read by int.c::cpu_interrupt to decide
                                 * whether to cancel. */

/* Coordination flags between cpu_simulate and worker-thread cpu_interrupt()
 * calls.  Worker bursts (esp. timer at high rates) used to issue dozens of
 * WHvCancelRunVirtualProcessor hypercalls per drain cycle -- each cancel
 * arriving while the main thread was already out of WHvRunVp processing the
 * previous exit was pure overhead, and even worse, made the next WHvRunVp
 * exit immediately with Canceled before running any guest instructions.
 *
 * g_in_whvrunvp: set to 1 just before WHvRunVp, 0 just after.  Read by the
 * cancel path -- if the VCPU isn't currently inside the hypervisor there's
 * no point cancelling, the pending bit in pNtVDMState will be picked up
 * the next iteration anyway.
 *
 * g_cancel_pending: set atomically by the first worker to call cancel for
 * a given WHvRunVp invocation; cleared by cpu_simulate after the exit.
 * Subsequent workers see it set and skip the redundant hypercall. */
volatile LONG g_in_whvrunvp   = 0;
volatile LONG g_cancel_pending = 0;

void hyperv_cancel_run(void)
{
    if (g_Partition)
        WHvCancelRunVirtualProcessor(g_Partition, g_VpIndex, 0);
}

/* Called from int.c::cpu_interrupt() on worker threads.  Gates the actual
 * hypercall behind "VCPU is in WHvRunVp" + "no other worker has already
 * issued a cancel for this iteration". */
void hyperv_cancel_run_if_running(void)
{
    if (!g_in_whvrunvp) return;
    if (InterlockedExchange(&g_cancel_pending, 1) != 0) return;
    hyperv_cancel_run();
}

#if WHP_PIT_TIMER_THREAD
/* ------------------------------------------------------------------------
 *  PIT-rate timer NUDGE thread (WHP only).
 *
 *  The 18 Hz heartbeat (nt_timer.c) already generates the correct number of
 *  timer ticks (PERF-TMR gen=162/s for SKYROADS' 180 Hz).  The failure is on
 *  DELIVERY: the timer IRQ is injected at the top of cpu_simulate, so its rate
 *  is gated by the loop iteration rate.  When the guest goes compute-bound
 *  (SKYROADS free-runs ~2.9 ms/exit -> ~310 loop iters/s, runvp ~90%) the loop
 *  rarely reaches the injection point, ticks pile into EoiIntsPending and get
 *  dropped, and `eoi` collapses 162 -> 72 -> the timer "jumps".
 *
 *  This thread CANCELS the VCPU run at the PIT rate, forcing the loop to
 *  iterate (and thus inject the pending timer) even mid free-run -- decoupling
 *  delivery from the guest's natural exit rate.  It deliberately does NOT
 *  generate (no time_tick): timer-0 updateCount returns a FIXED 65536/init
 *  ticks per call, so calling it faster over-generates.  No starvation: cancels
 *  just give the loop injection opportunities; the PIC still arbitrates IRQ0
 *  vs the keyboard's IRQ1 normally (and with delivery kept up the backlog --
 *  the thing that caused the synchronous-re-assert starvation -- never builds).
 *  The cancel is deduped (g_cancel_pending), so in fast windows it's ~free.
 *
 *  STATUS: ENABLED (1).  PERF-TMR showed it keeps delivery up in active
 *  gameplay (SKYROADS eoi 72->155).  The earlier "idle starvation" (eoi ~32)
 *  was NOT this thread -- it was NTVDM's normal cursor-out background CPU
 *  throttling (which legitimately slows the whole VM, music included), so it's
 *  out of scope here.  The separate ~9% slowness (DOOM gen=eoi=126 vs 140) was
 *  the 65536/initialCount truncation in clocksSinceCounterUpdate, now fixed
 *  with remainder-carry in timer.c. */

extern long __stdcall NtSetTimerResolution(unsigned long DesiredResolution,
                                           unsigned char  SetResolution,
                                           unsigned long *CurrentResolution);

#define PIT_TIMER_HZ_CAP   500   /* don't cancel faster than this */
#define PIT_TIMER_HZ_FLOOR 20    /* at/below this leave it to the 18 Hz heartbeat */

/* Win10 1803+ flag; define if the SDK headers are older. */
#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif

static volatile LONG g_pit_thread_stop = 0;
static HANDLE        g_pit_thread       = NULL;

static DWORD WINAPI pit_timer_thread_proc(LPVOID arg)
{
    unsigned long cur_res = 0;
    HANDLE        hTimer;
    (void)arg;
    /* High-res waitable timer for evenly-spaced cancels (~1 ms res via
     * NtSetTimerResolution backs the Sleep fallback). */
    NtSetTimerResolution(10000, 1, &cur_res);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
    hTimer = CreateWaitableTimerExW(NULL, NULL,
                 CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);

    while (!g_pit_thread_stop) {
        unsigned long pit0 = GetTimer0InitialCount();
        unsigned long pit_hz;
        unsigned long nudge_hz;

        if (pit0 == 0) pit0 = 65536;
        pit_hz = 1193180UL / pit0;

        if (pit_hz < PIT_TIMER_HZ_FLOOR) {   /* default/slow PIT -> heartbeat handles it */
            Sleep(20);
            continue;
        }
        /* Cancel FASTER than the guest's compute-bound exit rate (~300/s),
         * else we add no injection opportunities.  Use the cap rate; the
         * cancel is deduped (g_cancel_pending) so in fast windows it's ~free. */
        nudge_hz = PIT_TIMER_HZ_CAP;

        if (hTimer) {
            LARGE_INTEGER due;
            due.QuadPart = -(LONGLONG)(10000000UL / nudge_hz);
            SetWaitableTimer(hTimer, &due, 0, NULL, NULL, FALSE);
            WaitForSingleObject(hTimer, 100);
        } else {
            DWORD ms = 1000UL / nudge_hz;
            Sleep(ms ? ms : 1);
        }

        /* NUDGE only: force a loop iteration so a pending timer IRQ (generated
         * by the heartbeat) gets injected even while the guest free-runs.  No
         * time_tick() here -- that would over-generate. */
        hyperv_cancel_run_if_running();
    }
    if (hTimer) CloseHandle(hTimer);
    NtSetTimerResolution(10000, 0, &cur_res);
    return 0;
}

static void pit_timer_start_if_needed(void)
{
    DWORD tid;
    if (g_pit_thread != NULL) return;
    g_pit_thread = CreateThread(NULL, 0, pit_timer_thread_proc, NULL, 0, &tid);
    /* On failure leave NULL; sound just falls back to the 18 Hz heartbeat. */
}
#endif /* WHP_PIT_TIMER_THREAD */

/*
 * Returns TRUE if WHP currently has no pending interruption (slot is free),
 * FALSE if one is already pending and waiting to be delivered.
 *
 * Unlike HAXM (which queued pending interrupts in the kernel driver), WHP
 * has exactly one WHvRegisterPendingInterruption slot.  If we set it while
 * a previous injection is still un-delivered (VCPU in CLI / interrupt
 * shadow), the previous one is silently lost.  Since we already
 * ica_intack()'d that vector, the PIC keeps it marked in-service, no EOI
 * ever happens, and DOS keyboard handlers (BIOS or KB16) sit polling
 * forever.  Always check this before injecting.
 */
BOOL hyperv_pending_interruption_slot_free(void)
{
    WHV_REGISTER_NAME  names[2] = { WHvRegisterPendingInterruption,
                                    WHvX64RegisterRflags };
    WHV_REGISTER_VALUE vals[2]  = {0};
    WHV_X64_PENDING_INTERRUPTION_REGISTER pi;
    HRESULT hr = WHvGetVirtualProcessorRegisters(g_Partition, g_VpIndex,
                                                 names, 2, vals);
    if (FAILED(hr)) return TRUE;  /* fail open -- worst case we stomp */
    pi.AsUINT64 = vals[0].PendingInterruption.AsUINT64;

    return !pi.InterruptionPending;
}

/* ------------------------------------------------------------------------
 * YODA "trace into IRQ handlers" support.
 *
 * The CPU's automatic IRQ delivery atomically pushes EFLAGS (with TF=1)
 * onto the stack, clears live EFLAGS.TF, and vectors to the IDT entry.
 * The result: every instruction inside the handler runs with TF=0, so
 * the single-step #DB stays silent until IRET restores TF.  That makes
 * the IRQ path invisible in the trace, unlike CCPU which interprets the
 * delivery itself and check_I()s every step.
 *
 * Bridge that gap by arming DR0 at the handler's first instruction the
 * moment we inject an IRQ.  When the CPU delivers, DR0 fires at handler
 * entry, we re-arm RFLAGS.TF, and the rest of the handler steps via the
 * existing TF path.  The eventual IRET pops EFLAGS (TF=1) and -- per
 * SDM 17.3.1.1 -- raises #DB immediately after IRET, returning the
 * trace to the original interrupted RIP.
 *
 * Vector resolution covers all three modes the guest can be in when an
 * IRQ fires:
 *   - Real mode (CR0.PE=0): IVT at linear 0, 4 bytes/entry.
 *   - V86      (PE=1,VM=1): handler runs in PM ring 0; use IDTR + GDT walk.
 *   - PM       (PE=1,VM=0): same as V86 -- IDTR + GDT walk.
 * The IDT walk skips task gates and non-present entries (returns 0 to
 * indicate "punt -- can't trace this one, deliver normally").
 *
 * g_db_armed_vec is the vector for the IRQ currently in flight (-1 when
 * idle); kept non-static so YODA / debug.c can display it in the trace.
 * ------------------------------------------------------------------------ */

int   g_db_armed_vec = -1;
static DWORD g_saved_dr7    = 0;
static int   g_dr7_was_saved = 0;

/* IRET-return tracking.  When DR0 fires at handler entry we record the
 * RIP we were about to execute; the handler's IRET eventually pops that
 * same RIP and -- per SDM 17.3.1.1, IRET-with-TF=1 -- raises #DB right
 * after IRET completes.  We detect that trap (RIP matches the saved
 * value) and arm g_skip_next_dispatch so the *next* WHvRunVp iteration
 * does NOT inject another IRQ at the same boundary.  Without this skip,
 * a queued timer IRQ would be delivered before the now-resumed main-code
 * instruction can execute, livelocking the trace at one RIP. */
static DWORD g_irq_return_rip   = 0;
static int   g_in_irq_handler   = 0;
static int   g_skip_next_dispatch = 0;

/* WHP TF-stuck recovery state.  WHP's #DB-via-exception-bitmap intercept
 * can latch into a state where it re-delivers the same #DB at the same
 * RIP without ever running the instruction.  Once latched, no
 * combination of clearing PendingDebugException / InterruptState / DR6 /
 * setting RF unsticks it.  Workaround: temporarily clear EFLAGS.TF so
 * the CPU runs free, then re-arm TF on the next natural exit (BOP /
 * IRQ window / IO / MMIO -- anything that isn't another #DB at the same
 * RIP).  Trace fidelity drops between the stuck point and the next
 * natural exit, but the system progresses instead of livelocking. */
static int   g_retf_on_next_exit = 0;

/* The forward declarations are intentional: getPE() / NANO_ENABLED are
 * defined further down in this file (after cpu_simulate), but our IDT
 * walker and inject helper are above cpu_simulate.  Rather than reshuffle
 * the file, inline the underlying bit checks.  CR0.PE is bit 0 (MSW_PE,
 * value 1); NANO_ENABLED is the YODA "Nano slow mode" bit (0x20) from
 * GLOBAL_nanoDebugControl. */
#define HV_NANO_ENABLED 0x20

static DWORD idt_handler_lin(uint32_t vec)
{
    if (!(state._cr0 & 1u)) {
        /* Real mode: IVT entry is (offset_lo16 | seg_lo16). */
        DWORD ent = sas_dw_at((DWORD)vec * 4);
        return (((ent >> 16) & 0xffff) << 4) + (ent & 0xffff);
    } else {
        /* Protected mode (V86 source counts as PM here -- the CPU still
         * walks IDTR, not the IVT).  IDT entry layout:
         *   ent_lo : offset[15:0]  | selector
         *   ent_hi : reserved(8)   | type/attr(8) | offset[31:16] */
        DWORD idt_base = (DWORD)state._idt.base;
        DWORD ent_lo, ent_hi, offset;
        WORD  sel;
        BYTE  present, gate_type;
        LIN_ADDR descr_addr;
        DESCR entry;

        if ((DWORD)vec * 8u + 7u > state._idt.limit) return 0;

        ent_lo = sas_dw_at(idt_base + (DWORD)vec * 8u);
        ent_hi = sas_dw_at(idt_base + (DWORD)vec * 8u + 4u);
        present   = (BYTE)((ent_hi >> 15) & 1);
        gate_type = (BYTE)((ent_hi >> 8)  & 0xf);

        /* P=0 (not present) or task gate (0x5) -- punt; the IRQ will be
         * delivered normally and the handler runs untraced. */
        if (!present || gate_type == 0x5) return 0;

        offset = (ent_lo & 0xffffu) | (ent_hi & 0xffff0000u);
        sel    = (WORD)((ent_lo >> 16) & 0xffffu);

        if (selector_outside_table(sel, &descr_addr)) return 0;
        read_descriptor(descr_addr, &entry);
        return (DWORD)entry.base + offset;
    }
}

static int hyperv_arm_idt_trap(uint32_t vec)
{
    DWORD lin = idt_handler_lin(vec);
    if (!lin) return 0;

    /* Save the guest's DR7 once per arming session so we can restore it
     * cleanly when YODA stops -- otherwise guest software that itself
     * touches DR0..DR3 (rare in DOS, common in DOSX/SoftICE) would see
     * our breakpoint stick around. */
    if (!g_dr7_was_saved) {
        g_saved_dr7      = (DWORD)state._dr7;
        g_dr7_was_saved  = 1;
    }

    state._dr0 = lin;
    /* L0=1 (local enable), RW0=00 (execute), LEN0=00 (1-byte). */
    state._dr7 = (state._dr7 & ~0x000F0003u) | 0x00000001u;
    state._dr6 = 0;
    MARK(REG_GROUP_DEBUG);
    g_db_armed_vec   = (int)vec;
    g_irq_return_rip = (DWORD)state._rip;
    {
        char dbg[128];
        sprintf(dbg, "YODA: arm DR0=%08X for INT 0x%02X, return@%04X:%08X\n",
                (unsigned)lin, (unsigned)vec,
                (unsigned)state._cs.selector,
                (unsigned)state._eip);
        OutputDebugStringA(dbg);
    }
    return 1;
}

static void hyperv_disarm_idt_trap(void)
{
    /* Restore ONLY DR0's enable/RW/LEN bits (mask 0x000F0003) -- the arm side
     * only ever touched those.  Do NOT wholesale-restore g_saved_dr7: that
     * wiped any data watchpoint (blwd, in DR1-DR3) armed after the snapshot,
     * which is why a freshly-set data BP only "stuck" on the 2nd try (the
     * timer IRQ's IDT-trap disarm clobbered the 1st).  DR1-DR3 (software-step
     * + data BPs) are left untouched. */
    if (g_dr7_was_saved) {
        state._dr7 = (state._dr7 & ~0x000F0003u) | (g_saved_dr7 & 0x000F0003u);
        g_dr7_was_saved = 0;
    } else {
        state._dr7 &= ~0x000F0003u;
    }
    state._dr6 = 0;
    MARK(REG_GROUP_DEBUG);
    g_db_armed_vec = -1;
}

/* Called from int.c::DispatchInterrupts before it acks the PIC.  Refuse
 * only when a DR0 arming is already in flight (would otherwise stomp
 * the address WHP is going to trap on for the previously-injected IRQ).
 *
 * We deliberately do NOT gate on g_in_irq_handler: in single-step mode
 * each IRQ handler takes many milliseconds to trace, and during that
 * window worker threads keep firing -- the PIC's `ica_cpu_int` would
 * accumulate stuck-TRUE for every subsequent IRQ that arrives before
 * the current handler's IRET, blocking timer + keyboard at the ICA
 * layer.  Allowing INTACK while in a handler is the right trade: it
 * permits nested IRQ delivery (which only happens when the handler
 * itself enables interrupts via STI, just like real hardware), keeps
 * `ica_cpu_int` flowing, and avoids the stuck-TRUE deadlock. */
BOOL hyperv_yoda_inject_allowed(void)
{
    if (!(GLOBAL_nanoDebugControl & HV_NANO_ENABLED)) return TRUE;
    return (g_db_armed_vec < 0);
}

/* ------------------------------------------------------------------------
 *  Software single-step via DR1/DR2.
 *
 *  WHP's TF #DB intercept latches into "re-deliver same #DB without
 *  advancing RIP" after certain CPU operations (post-STI, post-IRQ-
 *  delivery -- see project_hyperv_tf_intercept_bug memory).  Substitute:
 *  use DR1 (sequential next-RIP) and DR2 (taken-branch target, for
 *  JCC/INTO) as the trapping mechanism.  DR0 stays reserved for IRQ-
 *  handler-entry tracing.
 *
 *  Each #DB exit ends with predict_next_lin() + hyperv_arm_step_bp() so
 *  the next instruction's address is breakpointed.  The CPU executes the
 *  current instruction and traps on whichever DR matches the resulting
 *  RIP (DR1 or DR2 for direct flow; DR0 if an IRQ vectored to a handler).
 * ------------------------------------------------------------------------ */

struct insn_predict {
    int      length;       /* bytes the current instruction consumes */
    int      n_targets;    /* 1 or 2 plausible next-RIPs */
    DWORD    target[2];    /* target[0] = sequential/not-taken;
                              target[1] = taken branch target */
    int      indirect;     /* 1 if target was guessed (JMP/CALL r/m etc.) */
};

static IS32 sw_step_read_byte(LIN_ADDR a)
{
    return (IS32)sas_hw_at(a);
}

/* SS:(SP+offset) linear address.  Honors the SS descriptor's B bit:
 * 32-bit stack uses the full ESP, 16-bit stack masks to 16. */
static DWORD sw_step_stack_lin(DWORD sp_offset)
{
    if (state._ss.operand_size)
        return (DWORD)state._ss.base + (DWORD)state._esp + sp_offset;
    return (DWORD)state._ss.base
         + (((DWORD)state._esp + sp_offset) & 0xFFFFu);
}

static WORD sw_step_stack_word(DWORD sp_offset)
{
    DWORD lin = sw_step_stack_lin(sp_offset);
    return (WORD)((WORD)sas_hw_at(lin) | ((WORD)sas_hw_at(lin + 1) << 8));
}

static DWORD sw_step_stack_dword(DWORD sp_offset)
{
    DWORD lin = sw_step_stack_lin(sp_offset);
    return (DWORD)sas_hw_at(lin)
         | ((DWORD)sas_hw_at(lin + 1) << 8)
         | ((DWORD)sas_hw_at(lin + 2) << 16)
         | ((DWORD)sas_hw_at(lin + 3) << 24);
}

/* Effective operand size for IRET/RETF/RET decoding.
 * Real mode / V86: default 16-bit, 0x66 prefix toggles to 32-bit.
 * Protected mode: CS.D bit is the default, 0x66 prefix toggles. */
static int sw_step_op_size_32(int prefix_66)
{
    int pe = !!(state._cr0 & 1u);
    int vm = !!(state._eflags & 0x20000u);
    if (!pe || vm) return prefix_66;
    return (!!state._cs.operand_size) ^ prefix_66;
}

/* Compute linear from (popped CS, popped IP/EIP).  Walks GDT/LDT for the
 * CS descriptor when in PM; uses real-mode CS*16 + IP for RM/V86. */
static DWORD sw_step_resolve_far(WORD cs_sel, DWORD eip, int op_32,
                                  int *indirect_out)
{
    int pe = !!(state._cr0 & 1u);
    int vm = !!(state._eflags & 0x20000u);
    if (pe && !vm) {
        LIN_ADDR descr_addr;
        DESCR entry;
        if (!selector_outside_table(cs_sel, &descr_addr)) {
            read_descriptor(descr_addr, &entry);
            return (DWORD)entry.base + (op_32 ? eip : (eip & 0xFFFFu));
        }
        if (indirect_out) *indirect_out = 1;
        return 0;  /* caller fills a fall-through indirect target */
    }
    return ((DWORD)cs_sel << 4) + (eip & 0xFFFFu);
}

/* Length (in bytes) of the port-I/O instruction at the CURRENT RIP, using
 * the FULL linear address (cs_base + full EIP).  Used to advance RIP past an
 * emulated IN/OUT when WHP reports no usable instruction length.
 *
 * MUST be used instead of predict_next_lin() for this: predict_next_lin()
 * truncates EIP to a WORD (it is a 16-bit-guest predictor), so for a 32-bit
 * flat code segment (e.g. DOOM/DOS4GW at CS=02FF base=0, EIP=0x011338be) it
 * reads the WRONG, aliased low address (0x38be) and returns a garbage length.
 * RIP then over-advances into the middle of the next instruction -> a
 * spurious, intermittent #GP (the bytes at the aliased address vary run to
 * run).  This was the DOOM/WHP "derail": a 1-byte IN AL,DX (port 21h, PIC
 * mask) at 011338be was advanced by 2 into 011338c0, which decodes as the
 * bogus `mov eax,[0xA2000000]`.
 *
 * Returns 0 if the opcode at RIP is not a port-I/O instruction. */
static int io_insn_len(void)
{
    DWORD lin = (DWORD)state._cs.base + (DWORD)state._eip;
    int   n   = 0;
    BYTE  op;
    while (n < 15) {
        BYTE b = sas_hw_at(lin + n);
        /* Legacy prefixes that may precede IN/OUT and INS/OUTS. */
        if (b == 0x66 || b == 0x67 || b == 0xF3 || b == 0xF2 ||
            b == 0x26 || b == 0x2E || b == 0x36 || b == 0x3E ||
            b == 0x64 || b == 0x65) { n++; continue; }
        break;
    }
    op = sas_hw_at(lin + n);
    switch (op) {
    case 0xE4: case 0xE5: case 0xE6: case 0xE7:   /* IN/OUT  AL/eAX, imm8 */
        return n + 2;
    case 0xEC: case 0xED: case 0xEE: case 0xEF:   /* IN/OUT  AL/eAX, DX   */
    case 0x6C: case 0x6D: case 0x6E: case 0x6F:   /* INS/OUTS            */
        return n + 1;
    default:
        return 0;
    }
}

static void predict_next_lin(struct insn_predict *out)
{
    BYTE bytes[16];
    DWORD cs_base = (DWORD)state._cs.base;
    /* CS D/B bit: 1 => 32-bit code segment (full EIP, rel32 branches), 0 =>
     * 16-bit (offset wraps at 64K, rel16).  Originally this routine forced
     * `ip = (WORD)state._eip`, which read an aliased low linear address for a
     * 32-bit guest (e.g. DOOM/DOS4GW at CS base 0, EIP 0x011338be -> 0x38be)
     * and made every prediction/length garbage.  Honor the segment size. */
    int   code_32 = (!!(state._cr0 & 1u) && !(state._eflags & 0x20000u))
                    ? (state._cs.operand_size ? 1 : 0) : 0; /* PM + CS.D; 16-bit in RM/V86 */
    DWORD ip      = code_32 ? (DWORD)state._eip : (DWORD)(WORD)state._eip;
    DWORD cur_lin = cs_base + ip;
    SIZE_SPECIFIER dec_sz = code_32 ? THIRTY_TWO_BIT : SIXTEEN_BIT;
    int p, i;
    int prefix_66;         /* operand-size override seen? */
    int prefix_67;         /* address-size override seen? */
    int seg_ov;            /* segment override 0=ES,1=CS,2=SS,3=DS,4=FS,5=GS,-1=none */
    int ss_skips = 0;      /* MOV/POP SS shadow instructions skipped over */
    BYTE op;
    DECODED_INST d_inst;

/* Wrap a computed next-IP to the code segment's address size (64K for a
 * 16-bit segment, full 32-bit otherwise). */
#define WRAP_IP(v)  (code_32 ? (DWORD)(v) : (DWORD)((DWORD)(v) & 0xFFFFu))

rescan:
    for (i = 0; i < 16; i++) bytes[i] = sas_hw_at(cur_lin + i);

    /* Skip prefix bytes -- they contribute to length but don't change
     * the branch decision.  Track 0x66 separately since IRET/RETF/RET
     * decoding needs to know the effective operand size. */
    p = 0;
    prefix_66 = 0;
    prefix_67 = 0;
    seg_ov    = -1;
    while (p < 15) {
        BYTE b = bytes[p];
        if (b == 0x66) { prefix_66 = 1; p++; continue; }
        if (b == 0x67) { prefix_67 = 1; p++; continue; }
        if (b == 0x26) { seg_ov = 0; p++; continue; }   /* ES */
        if (b == 0x2E) { seg_ov = 1; p++; continue; }   /* CS */
        if (b == 0x36) { seg_ov = 2; p++; continue; }   /* SS */
        if (b == 0x3E) { seg_ov = 3; p++; continue; }   /* DS */
        if (b == 0x64) { seg_ov = 4; p++; continue; }   /* FS */
        if (b == 0x65) { seg_ov = 5; p++; continue; }   /* GS */
        if (b == 0xF0 || b == 0xF2 || b == 0xF3) { p++; continue; }
        break;
    }

    op = bytes[p];

    /* MOV SS / POP SS / LSS open an interrupt+debug shadow: the CPU blocks
     * external interrupts, NMI, and ALL debug exceptions -- the single-step
     * trap AND instruction breakpoints -- until *after* the instruction
     * that follows the SS load (Intel SDM Vol 3, 6.8.3 / 17.3.1.1).  This
     * is what makes a `mov ss,[]; mov sp,[]` stack switch atomic: nothing
     * can be delivered onto the half-loaded SS:SP.
     *
     * If we arm a DR step breakpoint at that following instruction the
     * boundary is inside the shadow, so WHP cannot deliver the #DB there:
     * its TF/#DB intercept latches at the same RIP without advancing (see
     * project_hyperv_tf_intercept_bug).  The stuck detector then clears TF
     * and free-runs the CPU dark, and the guest derails -- chkcpu's stack
     * switch takes a spurious fault on the half-loaded SS:SP and spins
     * forever in its trap handler (the unexp_nt loop).
     *
     * Treat the SS load and the instruction after it as one atomic step:
     * skip past the SS load and predict from the next instruction so the
     * step breakpoint lands outside the shadow.  (The shadow instruction
     * simply isn't logged as its own trace point -- a negligible fidelity
     * cost.)  Bounded so a run of garbage bytes can't loop forever. */
    {
        int is_ss_load =
            (op == 0x17) ||                                    /* POP SS     */
            (op == 0x8E && ((bytes[p + 1] >> 3) & 7) == 2) ||  /* MOV SS,r/m */
            (op == 0x0F && bytes[p + 1] == 0xB2);              /* LSS        */
        if (is_ss_load && ++ss_skips <= 8) {
            int len;
            decode(cur_lin, &d_inst, dec_sz, sw_step_read_byte);
            len = (int)d_inst.inst_sz;
            if (len <= 0 || len > 15) len = 1;
            ip      = WRAP_IP(ip + (DWORD)len);
            cur_lin = cs_base + ip;
            goto rescan;
        }
    }

    out->indirect  = 0;
    out->n_targets = 1;
    out->target[0] = 0;
    out->target[1] = 0;

    switch (op) {
    /* --- Unconditional direct branches --- */
    case 0xEB: { /* JMP rel8 */
        INT8 off = (INT8)bytes[p + 1];
        out->length = p + 2;
        out->target[0] = cs_base + WRAP_IP(ip + (DWORD)out->length + (DWORD)(long)off);
        return;
    }
    case 0xE9:   /* JMP rel16/rel32 */
    case 0xE8: { /* CALL rel16/rel32 -- displacement size = operand size */
        int   op_32 = sw_step_op_size_32(prefix_66);
        DWORD disp;     /* added modulo the address size, so raw (rel32) or
                         * sign-extended-to-32 (rel16) both add correctly */
        if (op_32) {
            disp = (DWORD)bytes[p + 1] | ((DWORD)bytes[p + 2] << 8)
                 | ((DWORD)bytes[p + 3] << 16) | ((DWORD)bytes[p + 4] << 24);
            out->length = p + 5;
        } else {
            disp = (DWORD)(INT16)((WORD)bytes[p + 1] | ((WORD)bytes[p + 2] << 8));
            out->length = p + 3;
        }
        out->target[0] = cs_base + WRAP_IP(ip + (DWORD)out->length + disp);
        return;
    }
    case 0xEA:   /* JMP far direct ptr16:16 / ptr16:32 */
    case 0x9A: { /* CALL far direct -- offset size = operand size, PM-aware */
        int   op_32 = sw_step_op_size_32(prefix_66);
        int   ind   = 0;
        DWORD noff; WORD nsel;
        if (op_32) {
            noff = (DWORD)bytes[p + 1] | ((DWORD)bytes[p + 2] << 8)
                 | ((DWORD)bytes[p + 3] << 16) | ((DWORD)bytes[p + 4] << 24);
            nsel = (WORD)((WORD)bytes[p + 5] | ((WORD)bytes[p + 6] << 8));
            out->length = p + 7;
        } else {
            noff = (DWORD)((WORD)bytes[p + 1] | ((WORD)bytes[p + 2] << 8));
            nsel = (WORD)((WORD)bytes[p + 3] | ((WORD)bytes[p + 4] << 8));
            out->length = p + 5;
        }
        out->target[0] = sw_step_resolve_far(nsel, noff, op_32, &ind);
        if (ind) {
            out->indirect = 1;
            out->target[0] = cs_base + WRAP_IP(ip + (DWORD)out->length);
        }
        return;
    }

    /* --- Conditional (two-target) branches --- */
    case 0x70: case 0x71: case 0x72: case 0x73:
    case 0x74: case 0x75: case 0x76: case 0x77:
    case 0x78: case 0x79: case 0x7A: case 0x7B:
    case 0x7C: case 0x7D: case 0x7E: case 0x7F:
    case 0xE0: case 0xE1: case 0xE2: case 0xE3: {
        /* JCC rel8 / LOOPNZ / LOOPZ / LOOP / JCXZ */
        INT8 off = (INT8)bytes[p + 1];
        out->length = p + 2;
        out->n_targets = 2;
        out->target[0] = cs_base + WRAP_IP(ip + (DWORD)out->length);
        out->target[1] = cs_base + WRAP_IP(ip + (DWORD)out->length + (DWORD)(long)off);
        return;
    }

    /* --- Returns: predict by reading the stack.  Operand-size and PM
     *     awareness via sw_step_op_size_32 / sw_step_resolve_far. --- */
    case 0xC3: { /* RET near */
        int op_32 = sw_step_op_size_32(prefix_66);
        DWORD r_eip = op_32 ? sw_step_stack_dword(0)
                            : (DWORD)sw_step_stack_word(0);
        out->length = p + 1;
        /* CS doesn't change on near RET -- use current CS base. */
        out->target[0] = cs_base + (op_32 ? r_eip : (r_eip & 0xFFFFu));
        return;
    }
    case 0xC2: { /* RET near imm16 */
        int op_32 = sw_step_op_size_32(prefix_66);
        DWORD r_eip = op_32 ? sw_step_stack_dword(0)
                            : (DWORD)sw_step_stack_word(0);
        out->length = p + 3;
        out->target[0] = cs_base + (op_32 ? r_eip : (r_eip & 0xFFFFu));
        return;
    }
    case 0xCB: { /* RETF */
        int op_32 = sw_step_op_size_32(prefix_66);
        int ind   = 0;
        DWORD r_eip; WORD r_cs;
        out->length = p + 1;
        if (op_32) {
            r_eip = sw_step_stack_dword(0);
            r_cs  = (WORD)(sw_step_stack_dword(4) & 0xFFFFu);
        } else {
            r_eip = (DWORD)sw_step_stack_word(0);
            r_cs  = sw_step_stack_word(2);
        }
        out->target[0] = sw_step_resolve_far(r_cs, r_eip, op_32, &ind);
        if (ind) {
            out->indirect = 1;
            out->target[0] = cs_base + WRAP_IP(ip + (DWORD)out->length);
        }
        return;
    }
    case 0xCA: { /* RETF imm16 */
        int op_32 = sw_step_op_size_32(prefix_66);
        int ind   = 0;
        DWORD r_eip; WORD r_cs;
        out->length = p + 3;
        if (op_32) {
            r_eip = sw_step_stack_dword(0);
            r_cs  = (WORD)(sw_step_stack_dword(4) & 0xFFFFu);
        } else {
            r_eip = (DWORD)sw_step_stack_word(0);
            r_cs  = sw_step_stack_word(2);
        }
        out->target[0] = sw_step_resolve_far(r_cs, r_eip, op_32, &ind);
        if (ind) {
            out->indirect = 1;
            out->target[0] = cs_base + WRAP_IP(ip + (DWORD)out->length);
        }
        return;
    }
    case 0xCF: { /* IRET / IRETD
                  * 16-bit: pop IP/CS/FLAGS (6 bytes).
                  * 32-bit: pop EIP/CS/EFLAGS (12 bytes; CS is in low 16
                  *         of its dword, upper 16 ignored on push/pop).
                  * If the popped EFLAGS has VM=1 (only reachable via
                  * 32-bit IRETD from PM ring 0), the CPU transitions to
                  * V86 mode -- use real-mode addressing for the target.
                  * (Extra V86-frame entries -- ESP/SS/ES/DS/FS/GS -- live
                  * at higher offsets and don't affect our prediction.) */
        int op_32 = sw_step_op_size_32(prefix_66);
        int ind   = 0;
        out->length = p + 1;
        if (op_32) {
            DWORD r_eip    = sw_step_stack_dword(0);
            WORD  r_cs     = (WORD)(sw_step_stack_dword(4) & 0xFFFFu);
            DWORD r_eflags = sw_step_stack_dword(8);
            if (r_eflags & 0x20000u) {
                /* Returning to V86: real-mode addressing. */
                out->target[0] = ((DWORD)r_cs << 4) + (r_eip & 0xFFFFu);
            } else {
                out->target[0] = sw_step_resolve_far(r_cs, r_eip, 1, &ind);
            }
        } else {
            WORD r_ip = sw_step_stack_word(0);
            WORD r_cs = sw_step_stack_word(2);
            out->target[0] = sw_step_resolve_far(r_cs, (DWORD)r_ip, 0, &ind);
        }
        if (ind) {
            out->indirect = 1;
            out->target[0] = cs_base + WRAP_IP(ip + (DWORD)out->length);
        }
        return;
    }

    /* --- Software interrupts: predict via the IDT walker --- */
    case 0xCC: { /* INT 3 */
        DWORD t = idt_handler_lin(3);
        out->length = p + 1;
        if (t) { out->target[0] = t; }
        else   { out->indirect = 1;
                 out->target[0] = cs_base + WRAP_IP(ip + (DWORD)out->length); }
        return;
    }
    case 0xCD: { /* INT imm8 */
        DWORD t = idt_handler_lin(bytes[p + 1]);
        out->length = p + 2;
        if (t) { out->target[0] = t; }
        else   { out->indirect = 1;
                 out->target[0] = cs_base + WRAP_IP(ip + (DWORD)out->length); }
        return;
    }
    case 0xCE: { /* INTO -- conditional on OF */
        DWORD t = idt_handler_lin(4);
        out->length = p + 1;
        out->n_targets = 2;
        out->target[0] = cs_base + WRAP_IP(ip + (DWORD)out->length);
        if (t) out->target[1] = t;
        else { out->indirect = 1; out->target[1] = out->target[0]; }
        return;
    }

    /* --- FF /2..5 = indirect CALL/JMP through r/m.  RESOLVE the real
     *     target (decode ModRM -> effective address -> read the pointer)
     *     so the step BP lands ON the callee's first instruction (step
     *     INTO), not on the fall-through (which over-runs the whole call
     *     and derails the yoda trace on every CALLN [mem] -- see
     *     project_wpsnt_whp_derail).  SIB / unhandled forms fall through
     *     to the punt below (indirect=1). --- */
    case 0xFF: {
        BYTE modrm = bytes[p + 1];
        int  mod   = (modrm >> 6) & 3;
        int  reg   = (modrm >> 3) & 7;
        int  rm    = modrm & 7;
        int  addr_32 = code_32 ^ prefix_67;      /* address size */
        int  op_32   = code_32 ^ prefix_66;      /* operand size (ptr width) */
        DWORD ea = 0, disp = 0;
        int   disp_len = 0, seg_is_ss = 0;

        if (reg < 2 || reg > 5) break;           /* INC/DEC/PUSH r/m -- not a branch */

        if (mod == 3) {                          /* register-direct (near only) */
            DWORD r;
            switch (rm) {
              case 0: r = (DWORD)state._eax; break; case 1: r = (DWORD)state._ecx; break;
              case 2: r = (DWORD)state._edx; break; case 3: r = (DWORD)state._ebx; break;
              case 4: r = (DWORD)state._esp; break; case 5: r = (DWORD)state._ebp; break;
              case 6: r = (DWORD)state._esi; break; default: r = (DWORD)state._edi; break;
            }
            out->length    = p + 2;
            out->target[0] = cs_base + WRAP_IP(op_32 ? r : (r & 0xFFFFu));
            return;
        }

        if (!addr_32) {                          /* 16-bit effective address */
            DWORD b_reg;
            switch (rm) {
              case 0: b_reg = (WORD)((WORD)state._ebx + (WORD)state._esi); break;
              case 1: b_reg = (WORD)((WORD)state._ebx + (WORD)state._edi); break;
              case 2: b_reg = (WORD)((WORD)state._ebp + (WORD)state._esi); seg_is_ss = 1; break;
              case 3: b_reg = (WORD)((WORD)state._ebp + (WORD)state._edi); seg_is_ss = 1; break;
              case 4: b_reg = (WORD)state._esi; break;
              case 5: b_reg = (WORD)state._edi; break;
              case 6: if (mod == 0) { b_reg = 0; disp_len = 2; }        /* disp16 */
                      else { b_reg = (WORD)state._ebp; seg_is_ss = 1; } break;
              default:b_reg = (WORD)state._ebx; break;
            }
            if (mod == 1) disp_len = 1;
            else if (mod == 2) disp_len = 2;
            if (disp_len == 1)      disp = (DWORD)(long)(INT8)bytes[p + 2];
            else if (disp_len == 2) disp = (DWORD)((WORD)bytes[p + 2] | ((WORD)bytes[p + 3] << 8));
            ea = (WORD)(b_reg + disp);
        } else {                                 /* 32-bit EA (no SIB) */
            DWORD base = 0;
            if (rm == 4) break;                  /* SIB -> punt */
            if (rm == 5 && mod == 0) { disp_len = 4; }
            else {
                switch (rm) {
                  case 0: base = (DWORD)state._eax; break; case 1: base = (DWORD)state._ecx; break;
                  case 2: base = (DWORD)state._edx; break; case 3: base = (DWORD)state._ebx; break;
                  case 5: base = (DWORD)state._ebp; seg_is_ss = 1; break;
                  case 6: base = (DWORD)state._esi; break; default: base = (DWORD)state._edi; break;
                }
                if (mod == 1) disp_len = 1;
                else if (mod == 2) disp_len = 4;
            }
            if (disp_len == 1)      disp = (DWORD)(long)(INT8)bytes[p + 2];
            else if (disp_len == 4) disp = (DWORD)bytes[p + 2] | ((DWORD)bytes[p + 3] << 8)
                                         | ((DWORD)bytes[p + 4] << 16) | ((DWORD)bytes[p + 5] << 24);
            ea = base + disp;
        }
        out->length = p + 2 + disp_len;

        {
            DWORD seg_base, mem;
            switch (seg_ov) {
              case 0: seg_base = (DWORD)state._es.base; break;
              case 1: seg_base = (DWORD)state._cs.base; break;
              case 2: seg_base = (DWORD)state._ss.base; break;
              case 3: seg_base = (DWORD)state._ds.base; break;
              case 4: seg_base = (DWORD)state._fs.base; break;
              case 5: seg_base = (DWORD)state._gs.base; break;
              default: seg_base = seg_is_ss ? (DWORD)state._ss.base : (DWORD)state._ds.base; break;
            }
            mem = seg_base + ea;
            if (reg == 2 || reg == 4) {          /* near CALL/JMP -- read offset */
                DWORD tgt = op_32
                    ? ((DWORD)sas_hw_at(mem) | ((DWORD)sas_hw_at(mem + 1) << 8)
                       | ((DWORD)sas_hw_at(mem + 2) << 16) | ((DWORD)sas_hw_at(mem + 3) << 24))
                    : ((DWORD)sas_hw_at(mem) | ((DWORD)sas_hw_at(mem + 1) << 8));
                out->target[0] = cs_base + WRAP_IP(op_32 ? tgt : (tgt & 0xFFFFu));
            } else {                             /* far CALL/JMP -- offset:selector */
                int   ind = 0;
                DWORD noff = op_32
                    ? ((DWORD)sas_hw_at(mem) | ((DWORD)sas_hw_at(mem + 1) << 8)
                       | ((DWORD)sas_hw_at(mem + 2) << 16) | ((DWORD)sas_hw_at(mem + 3) << 24))
                    : ((DWORD)sas_hw_at(mem) | ((DWORD)sas_hw_at(mem + 1) << 8));
                WORD  nsel = op_32
                    ? (WORD)((WORD)sas_hw_at(mem + 4) | ((WORD)sas_hw_at(mem + 5) << 8))
                    : (WORD)((WORD)sas_hw_at(mem + 2) | ((WORD)sas_hw_at(mem + 3) << 8));
                out->target[0] = sw_step_resolve_far(nsel, noff, op_32, &ind);
                if (ind) { out->indirect = 1;
                           out->target[0] = cs_base + WRAP_IP(ip + (DWORD)out->length); }
            }
        }
        return;
    }

    /* --- FF /2..5 forms not resolved above (SIB / unusual): punt -- set
     *     DR1 at fall-through; if the CPU goes elsewhere we fall back into
     *     the natural-exit recovery path. --- */
    default:
        break;
    }

    /* Non-branch path: use decode() for length, set DR1 at RIP+length. */
    decode(cur_lin, &d_inst, dec_sz, sw_step_read_byte);
    out->length = (int)d_inst.inst_sz;
    if (out->length <= 0 || out->length > 15) out->length = 1;
    out->target[0] = cs_base + WRAP_IP(ip + (DWORD)out->length);

    if (op == 0xFF) {
        int reg_field = (bytes[p + 1] >> 3) & 7;
        if (reg_field >= 2 && reg_field <= 5) out->indirect = 1;
    }
}
#undef WRAP_IP

/* DR7 mask covering DR1+DR2 enable bits and their RW/LEN nibbles.
 * Bits 2..5 (L1,G1,L2,G2) + bits 20..27 (RW1,LEN1,RW2,LEN2). */
#define HV_DR7_STEP_MASK  0x0FF0003Cu

static void hyperv_arm_step_bp(DWORD addr1, DWORD addr2)
{
    state._dr1 = addr1;
    /* DR2 needs SOME value even if we're only using DR1, since DR7's L2
     * stays at 0 in that case so DR2's contents are dont-care -- still,
     * mirror addr1 to avoid leaving an unrelated stale address. */
    state._dr2 = addr2 ? addr2 : addr1;
    state._dr7 = (state._dr7 & ~HV_DR7_STEP_MASK)
               | 0x04u                       /* L1=1 */
               | (addr2 ? 0x10u : 0u);       /* L2 only if 2 targets */
    state._dr6 = 0;
    MARK(REG_GROUP_DEBUG);
}

static void hyperv_disarm_step_bp(void)
{
    state._dr7 &= ~HV_DR7_STEP_MASK;
    MARK(REG_GROUP_DEBUG);
}

/* Convenience: predict + arm in one call.  Returns 1 on success, 0 if
 * the prediction was suppressed (self-target loop, or indirect branch
 * where the target is unknown). */
static int hyperv_step_bp_for_next(void)
{
    struct insn_predict ip;
    DWORD cur_lin = (DWORD)state._cs.base + (DWORD)state._eip;
    predict_next_lin(&ip);

    /* Self-target guard.  A breakpoint armed at the *current* RIP would
     * fault on entry before the instruction ever executes (DR code
     * breakpoints are faults, not traps).  Filter each target: if a
     * target equals the current RIP, drop it.  If both do (EB FE), give
     * up arming entirely and let the CPU run via TF / external exits. */
    {
        DWORD t0 = ip.target[0];
        DWORD t1 = ip.n_targets > 1 ? ip.target[1] : 0u;
        if (t0 == cur_lin) t0 = 0;
        if (t1 == cur_lin) t1 = 0;
        if (!t0 && !t1) {
            hyperv_disarm_step_bp();
            return 0;
        }
        /* Promote t1 -> t0 if t0 was dropped. */
        if (!t0) { t0 = t1; t1 = 0; }
        hyperv_arm_step_bp(t0, t1);
    }
    return !ip.indirect;
}

BOOL hyperv_inject_interrupt(uint32_t vector)
{
    WHV_REGISTER_NAME  names[1] = { WHvRegisterPendingInterruption };
    WHV_REGISTER_VALUE vals[1]  = {0};
    HRESULT hr;
    WHV_X64_PENDING_INTERRUPTION_REGISTER pi = {0};

    /* Refuse to stomp an undelivered injection.  Caller must notice the
     * FALSE return and leave the IRQ pending (in the ICA / pNtVDMState)
     * so the next cpu_simulate loop iteration retries. */
    if (!hyperv_pending_interruption_slot_free()) {
#if WHP_DEBUG_VERBOSE
        {   /* DIAGNOSTIC: log every refusal. */
            static volatile LONG s_n = 0;
            char dbg[128];
            LONG cnt = InterlockedIncrement(&s_n);
            sprintf(dbg, "hyperv_inject_interrupt REFUSED #%ld vector=0x%02X (slot busy) TID=%lu\n",
                    cnt, (unsigned)vector, GetCurrentThreadId());
            OutputDebugStringA(dbg);
        }
#endif
        return FALSE;
    }

    /* While YODA single-step is on, arm DR0 at the handler entry so the
     * trace doesn't go dark inside the IRQ handler.  The mutual-exclusion
     * gate (one armed IRQ at a time) lives in hyperv_yoda_inject_allowed()
     * which the caller (int.c::DispatchInterrupts) checks before acking
     * the PIC -- so by the time we get here we know it's safe to arm.
     *
     * Arm failure (task gate, !present, descriptor walk fails) leaves
     * g_db_armed_vec == -1; we still inject below and the handler just
     * runs untraced for this one vector. */
    if (GLOBAL_nanoDebugControl & HV_NANO_ENABLED)
        hyperv_arm_idt_trap(vector);

    pi.InterruptionPending = 1;
    pi.InterruptionType    = WHvX64PendingInterrupt;
    pi.InterruptionVector  = vector;
    /* DeliverErrorCode = 0 — external IRQs do not push an error code. */
    vals[0].PendingInterruption.AsUINT64 = pi.AsUINT64;

    hr = WHvSetVirtualProcessorRegisters(g_Partition, g_VpIndex,
                                         names, 1, vals);
#if WHP_DEBUG_VERBOSE
    {   /* DIAGNOSTIC: log every accepted injection. */
        static volatile LONG s_n = 0;
        char dbg[128];
        LONG cnt = InterlockedIncrement(&s_n);
        sprintf(dbg, "hyperv_inject_interrupt #%ld vector=0x%02X hr=0x%08X TID=%lu\n",
                cnt, (unsigned)vector, (unsigned)hr, GetCurrentThreadId());
        OutputDebugStringA(dbg);
    }
#endif
    return SUCCEEDED(hr);
}

/* WHP interrupt-window (deliverability) notification arming.
 *
 * THE FIX for guests that spin with interrupts DISABLED while a HW IRQ is
 * pending (Commander Keen 4 "Loading" loop: IF=0, PIT IRQ0 queued, never
 * delivered -> hang; works on CCPU because the interpreter checks IRQs every
 * instruction).  We can only inject at a natural VM exit where IF=1; in an IF=0
 * spin there is never such a boundary.  WHP's remedy is WHvX64Register-
 * DeliverabilityNotifications.InterruptNotification: set it and WHP exits with
 * WHvRunVpExitReasonX64InterruptWindow the moment the guest can take an
 * interrupt (IF->1, no shadow); the InterruptWindow handler then injects.
 *
 * The old code only set the HAXM-style tunnel->request_interrupt_window field,
 * which WHvPlatform does not consume -- so the InterruptWindow exit never fired.
 *
 * g_intwin_armed caches the last value to avoid a redundant WHvSet every loop
 * iteration; it is reset at the InterruptWindow exit (WHP clears the
 * notification on delivery) so we re-arm if the IRQ is still pending. */
int g_intwin_armed = 0;

void hyperv_arm_interrupt_window(int on)
{
    WHV_REGISTER_NAME  nm = WHvX64RegisterDeliverabilityNotifications;
    WHV_REGISTER_VALUE vv = {0};
    if (on == g_intwin_armed)
        return;
    vv.DeliverabilityNotifications.InterruptNotification = on ? 1 : 0;
    if (SUCCEEDED(WHvSetVirtualProcessorRegisters(g_Partition, g_VpIndex,
                                                  &nm, 1, &vv)))
        g_intwin_armed = on;
}

/* Re-inject a CPU exception (vectors 0..31) back into the guest.
 *
 * Unlike hyperv_inject_interrupt(), which delivers external hardware IRQs
 * via the WHvX64PendingInterrupt slot, this routine uses
 * WHvX64PendingException -- the correct semantics for CPU faults like
 * #UD, #GP, #PF, etc.  Critical difference: WHP delivers an Exception as
 * a *fault* (saved RIP = the faulting instruction, so the guest's handler
 * can re-execute it after fixing the cause OR advance past it manually).
 * Misusing PendingInterrupt for an exception causes WHP to silently
 * mis-dispatch -- which is what was happening for chkcpu's invalid-opcode
 * detection: the #UD exit fired, we re-injected as Interrupt, WHP didn't
 * deliver it cleanly into the guest's IDT, the VCPU re-entered at the
 * same RIP, faulted again, and we got a 2800+/sec exception storm.
 *
 * Some exception vectors push an error code on the guest stack (8, 10,
 * 11, 12, 13, 14, 17, 30 per Intel SDM Vol 3 Table 6-1).  We let WHP
 * handle that via the DeliverErrorCode + ErrorCode fields. */
BOOL hyperv_inject_exception(uint32_t vector, BOOL has_errcode, uint32_t errcode)
{
    WHV_REGISTER_NAME  names[1] = { WHvRegisterPendingInterruption };
    WHV_REGISTER_VALUE vals[1]  = {0};
    HRESULT hr;
    WHV_X64_PENDING_INTERRUPTION_REGISTER pi = {0};

    if (!hyperv_pending_interruption_slot_free()) {
#if WHP_DEBUG_VERBOSE
        {   /* DIAGNOSTIC: log every refusal. */
            static volatile LONG s_n = 0;
            char dbg[128];
            LONG cnt = InterlockedIncrement(&s_n);
            sprintf(dbg, "hyperv_inject_exception REFUSED #%ld vector=0x%02X (slot busy) TID=%lu\n",
                    cnt, (unsigned)vector, GetCurrentThreadId());
            OutputDebugStringA(dbg);
        }
#endif
        return FALSE;
    }

    pi.InterruptionPending = 1;
    pi.InterruptionType    = WHvX64PendingException;
    pi.InterruptionVector  = vector;
    pi.DeliverErrorCode    = has_errcode ? 1 : 0;
    pi.ErrorCode           = has_errcode ? errcode : 0;
    vals[0].PendingInterruption.AsUINT64 = pi.AsUINT64;

    hr = WHvSetVirtualProcessorRegisters(g_Partition, g_VpIndex,
                                         names, 1, vals);
#if WHP_DEBUG_VERBOSE
    {   /* DIAGNOSTIC: log every accepted exception injection. */
        static volatile LONG s_n = 0;
        char dbg[160];
        LONG cnt = InterlockedIncrement(&s_n);
        sprintf(dbg, "hyperv_inject_exception #%ld vector=0x%02X errcode=%s%lX hr=0x%08X TID=%lu\n",
                cnt, (unsigned)vector,
                has_errcode ? "" : "(none) ",
                (unsigned long)errcode,
                (unsigned)hr, GetCurrentThreadId());
        OutputDebugStringA(dbg);
    }
#endif
    return SUCCEEDED(hr);
}

/* ------------------------------------------------------------------------ */
/*  Partition-level configuration.  The "extended VM exits" bitmask must be
 *  set BEFORE WHvSetupPartition; the exception-exit mask is then applied on
 *  top of it.  All of this happens once, from hypervvm_init().
 * ------------------------------------------------------------------------ */

void hyperv_set_exception_bitmap(uint64_t mask)
{
    WHV_PARTITION_PROPERTY prop = {0};
    HRESULT hr;

    s_exception_exit_bitmap = mask;
    prop.ExceptionExitBitmap = mask;
    hr = WHvSetPartitionProperty(g_Partition,
                                 WHvPartitionPropertyCodeExceptionExitBitmap,
                                 &prop, sizeof(prop));
    if (FAILED(hr))
        haxmvm_panic("WHvSetPartitionProperty(ExceptionExitBitmap=0x%llx) "
                       "failed (0x%08X) -- make sure the 'Windows Hypervisor "
                       "Platform' optional feature is enabled.",
                       (unsigned long long)mask, hr);
}

/* YODA bex/vex support for protected-mode faults on the WHP backend.
 *
 * On the CCPU core a PM #GP is caught by `vex` (verbose exceptions) / `bex`
 * (break on faults) in contributory_exception().  On WHP a PM #GP is delivered
 * NATIVELY by the hypervisor (no VP exit) unless its vector is in the
 * exception-exit bitmap, so bex/vex would otherwise never see it.  Mirror the
 * CCPU behaviour: while bex/vex is enabled, trap #GP (vector 13) so the
 * exception handler can log it / drop to YODA.  Toggled dynamically from the
 * cpu_simulate loop; self-disarms (restoring native delivery) when both flags
 * go off.  Only bit 13 is touched, so the default UD/NP and any live #DB
 * (data-BP/step) bits are preserved. */
static int s_pmfault_trap_armed = 0;

static void hyperv_sync_pmfault_trap(void)
{
    int want = (show_exceptions || trap_exceptions) ? 1 : 0;
    if (want == s_pmfault_trap_armed)
        return;
    if (want)
        hyperv_set_exception_bitmap(s_exception_exit_bitmap |  ((UINT64)1 << VECTOR_GP));
    else
        hyperv_set_exception_bitmap(s_exception_exit_bitmap & ~((UINT64)1 << VECTOR_GP));
    s_pmfault_trap_armed = want;
}

/* Dump a selector's descriptor (within the LDT/GDT? present? base/limit/access)
 * for the bex/vex PM-fault report -- e.g. to see why a MOV ES,sel #GP'd
 * (sel beyond the table limit => OUTSIDE TABLE, P=0 / bad base / bad type). */
static void hyperv_dump_sel_descr(const char *tag, WORD sel)
{
    LIN_ADDR descr_addr;
    char buf[160];

    if ((sel & 0xFFFC) == 0) {
        sprintf(buf, "  %s sel=%04X (null)\n", tag, (unsigned)sel);
    } else if (selector_outside_table(sel, &descr_addr) == 1) {
        sprintf(buf, "  %s sel=%04X OUTSIDE TABLE (ldt sel=%04X base=%08X limit=%05X)\n",
                tag, (unsigned)sel, (unsigned)state._ldt.selector,
                (unsigned)state._ldt.base, (unsigned)state._ldt.limit);
    } else {
        DESCR entry;
        DWORD hi = sas_dw_at((DWORD)descr_addr + 4);
        read_descriptor(descr_addr, &entry);
        sprintf(buf, "  %s sel=%04X descr@%08X base=%08X limit=%08X access=%02X P=%d\n",
                tag, (unsigned)sel, (unsigned)descr_addr,
                (unsigned)entry.base, (unsigned)entry.limit,
                (unsigned)((hi >> 8) & 0xFF), (int)((hi >> 15) & 1));
    }
    OutputDebugStringA(buf);
}

void hypervvm_init(void)
{
    static BOOL bInitialized = FALSE;
    WHV_CAPABILITY cap = {0};
    WHV_PARTITION_PROPERTY prop = {0};
    HRESULT hr;
    UINT32 written = 0;

    if (bInitialized) return;

    /* WinHvPlatform.dll is loaded dynamically so that the same NTVDMx64
     * binary can also start on Windows XP / 7 / pre-1803 builds where
     * the DLL does not exist.  A failure here means either "DLL not on
     * this OS" or "DLL present but missing one of the entry points we
     * need" -- either way the WHP backend is unusable on this host. */
    if (!whp_api_load())
        haxmvm_panic("Windows Hypervisor Platform is not available on "
                       "this system (WinHvPlatform.dll could not be "
                       "loaded, GetLastError=%lu).\n\n"
                       "WHP requires Windows 10 1803 / Server 2019 or "
                       "later with the optional features 'Hyper-V "
                       "Hypervisor' and 'Windows Hypervisor Platform' "
                       "enabled.  On older Windows (XP/7), pick a "
                       "different NTVDM backend.", GetLastError());

    /* The 32-bit wrapper resolves its 64-bit entry-point pointers via
     * Heaven's Gate inside its own DllMain, but has no way to report
     * failure to its caller.  Ask it now whether everything resolved --
     * if not, panic with the precise missing-symbol list instead of
     * far-calling RIP=0 in the 64-bit half on the next WHv* call. */
    {
        const char *diag = whp_api_wrapper_status();
        if (diag)
            haxmvm_panic(
                "The 32-bit WinHvPlatform.dll wrapper failed to resolve "
                "one or more 64-bit entry points via Heaven's Gate.\n\n"
                "%s\n"
                "Check that the optional Windows feature \"Windows "
                "Hypervisor Platform\" is installed (the real 64-bit "
                "WinHvPlatform.dll must exist in C:\\Windows\\System32), "
                "and that Load64bitDll.dll plus the 64-bit wow64hlp.dll "
                "are present next to NTVDMx64.exe.", diag);
    }

    /* Skip WHvGetCapability(HypervisorPresent).  When the WHP runtime is
     * reached from a WoW64 process via otya128's Heaven's Gate wrapper, the
     * internal "am I usable here?" check returns FALSE even when the
     * hypervisor itself is fully functional (verified by running the
     * native 64-bit QEMU/WHPX accelerator on the same host).  The probe
     * is advisory anyway -- WHvCreatePartition below does the real work
     * and returns a meaningful HRESULT if WHP is genuinely unavailable.
     * winevdm's whpxvm.dll skips the same probe for the same reason. */
    (void)cap; (void)written;

    hr = WHvCreatePartition(&g_Partition);
    if (FAILED(hr)) {
        /* STATUS_HV_NOT_PRESENT (0xC0351000) from the hypervisor partition
         * manager has two flavours: the hypervisor genuinely isn't running,
         * OR (much more commonly) it IS running but this process lacks the
         * permission to create a partition.  The kernel gives the same code
         * either way.  Detect via the side test "did 16-bit launches work
         * before in an elevated cmd?" -- but we can't do that here, so we
         * tell the user both possibilities, with the easy fix first. */
        if ((unsigned)hr == 0xC0351000u) {
            haxmvm_panic(
                "WHvCreatePartition returned STATUS_HV_NOT_PRESENT "
                "(0xC0351000).\n\n"
                "This almost always means the current user doesn't have "
                "permission to create a Hyper-V partition.  Two ways to "
                "fix it (pick one):\n\n"
                "  1) Run \"Setup-HyperV-User.cmd\" once (shipped next to "
                "NTVDMx64), then log out and back in.  It adds your user "
                "to the local \"Hyper-V Administrators\" group via its "
                "well-known SID, so it works on every Windows language.\n\n"
                "  2) Or always launch the parent shell (cmd.exe / "
                "Explorer / shortcut) elevated -- the kernel will then "
                "hand NTVDMx64 a high-integrity token.\n\n"
                "If neither helps, the Hyper-V hypervisor isn't actually "
                "running: enable both 'Hyper-V Hypervisor' and 'Windows "
                "Hypervisor Platform' optional features and reboot.");
        } else {
            haxmvm_panic("WHvCreatePartition failed (0x%08X).\n\n"
                         "If this looks like 0x80370301 / 0x80370302, the "
                         "Hyper-V hypervisor isn't running on this host.  "
                         "Enable both 'Hyper-V Hypervisor' and 'Windows "
                         "Hypervisor Platform' optional features and "
                         "reboot.", hr);
        }
    }

    /* One VCPU is enough; NTVDM is single-threaded by design. */
    prop.ProcessorCount = 1;
    hr = WHvSetPartitionProperty(g_Partition,
                                 WHvPartitionPropertyCodeProcessorCount,
                                 &prop, sizeof(prop));
    if (FAILED(hr))
        haxmvm_panic("WHvSetPartitionProperty(ProcessorCount) failed "
                       "(0x%08X)", hr);

    /* Enable exception-exit machinery before SetupPartition.  Without this
     * bit the per-vector ExceptionExitBitmap is silently ignored. */
    memset(&prop, 0, sizeof(prop));
    prop.ExtendedVmExits.ExceptionExit = 1;
    /* Also trap RDMSR / WRMSR.  The guest CPU now looks modern enough that
     * chkcpu (and friends) probe MSRs, but the VDM runs the guest at CPL 3,
     * where a real RDMSR/WRMSR #GPs -- and NTVDM cannot emulate that fault,
     * so it live-locks re-delivering #GP to the faulting instruction (the
     * unexp_nt loop).  The MSR exit fires ahead of the #GP regardless of CPL,
     * letting us emulate the access and skip the instruction (see the
     * WHvRunVpExitReasonX64MsrAccess handler in cpu_simulate).  Unlike adding
     * #GP to the exception bitmap, this does not perturb the V86 / PM
     * privileged-instruction faults the DOS games rely on. */
    prop.ExtendedVmExits.X64MsrExit = 1;
    hr = WHvSetPartitionProperty(g_Partition,
                                 WHvPartitionPropertyCodeExtendedVmExits,
                                 &prop, sizeof(prop));
    if (FAILED(hr))
        haxmvm_panic("WHvSetPartitionProperty(ExtendedVmExits) failed "
                       "(0x%08X)", hr);

    hr = WHvSetupPartition(g_Partition);
    if (FAILED(hr))
        haxmvm_panic("WHvSetupPartition failed (0x%08X)", hr);

    hr = WHvCreateVirtualProcessor(g_Partition, 0, 0);
    if (FAILED(hr))
        haxmvm_panic("WHvCreateVirtualProcessor failed (0x%08X)", hr);
    g_VpIndex = 0;

    tunnel = &s_tunnel;
    iobuf  = s_iobuf;

    bInitialized = TRUE;
}

#define SET_SEGMENT_ACCESS(seg, value) \
    (seg).available = ((value) >> 12) & 1; \
    (seg).present = ((value) >> 7) & 1; \
    (seg).dpl = ((value) >> 5) & ~(~0 << (6-5+1)); \
    (seg).desc = ((value) >> 4) & 1; \
    (seg).type = ((value) >> 0) & ~(~0 << (3-0+1));

EXPORT
VOID
cpu_init(
    )
{
    hypervvm_init();

    /*
     * Ask WHP to intercept #UD and #NP -- this is the equivalent of HAXM's
     * HAX_VCPU_IOCTL_SET_EXCBMP with VECTOR_UD | VECTOR_NP.  The vector
     * numbers correspond to WHV_EXCEPTION_TYPE.
     *
     * #DB and #BP are deliberately NOT in the default set:
     *   - Release builds have no YODA to consume them.
     *   - In debug builds, guest DOS debuggers (DEBUG.COM, Soft-ICE, ...)
     *     rely on INT3 going to the guest's IDT; intercepting #BP here
     *     would steal their breakpoints.
     * Both are added lazily by Nano_slow_yoda() when the user actually
     * enables YODA single-step.
     */
    {
        UINT64 mask = ((UINT64)1 << WHvX64ExceptionTypeInvalidOpcodeFault)
                    | ((UINT64)1 << WHvX64ExceptionTypeSegmentNotPresentFault);
        hyperv_set_exception_bitmap(mask);
    }

    /* Real-mode seed identical to the HAXM port. */
    state._cs.selector = 0;
    state._cs.base = 0;
    state._cs.limit = 0xffff;
    SET_SEGMENT_ACCESS(state._cs, 0x9b);

    state._ds.selector = 0; state._ds.base = 0; state._ds.limit = 0xffff;
    SET_SEGMENT_ACCESS(state._ds, 0x93);
    state._es.selector = 0; state._es.base = 0; state._es.limit = 0xffff;
    SET_SEGMENT_ACCESS(state._es, 0x93);
    state._fs.selector = 0; state._fs.base = 0; state._fs.limit = 0xffff;
    SET_SEGMENT_ACCESS(state._fs, 0x93);
    state._gs.selector = 0; state._gs.base = 0; state._gs.limit = 0xffff;
    SET_SEGMENT_ACCESS(state._gs, 0x93);
    state._ss.selector = 0; state._ss.base = 0; state._ss.limit = 0xffff;
    SET_SEGMENT_ACCESS(state._ss, 0x93);

    state._ldt.selector = 0; state._ldt.base = 0; state._ldt.limit = 0xffff;
    SET_SEGMENT_ACCESS(state._ldt, 0x10000);
    state._tr.selector = 0;  state._tr.base = 0;  state._tr.limit = 0xffff;
    SET_SEGMENT_ACCESS(state._tr, 0x83);

    state._gdt.base = 0; state._gdt.limit = 0x3FF;
    state._idt.base = 0; state._idt.limit = 0x3FF;

    state._rflags = 0x02L | EFLAGS_INTERRUPT_MASK;
    state._cr0 = 0;

    /* Everything we just seeded is "dirty" relative to the freshly-created
     * VCPU.  Mark all groups so the first hyperv_flush() in cpu_simulate
     * pushes the full real-mode initial context. */
    hyperv_mark_dirty(REG_GROUP_ALL);

    InitialVdmTibFlags |= RM_BIT_MASK;
    *pNtVDMState = InitialVdmTibFlags;

    cpu_createthread(NtCurrentThread()
#ifdef MINNT
        , NULL
#endif
        );

    InterruptInit();

    if (InitialBreakpoint)
        DbgBreakPoint();
}

EXPORT
VOID
cpu_terminate(
    )
{
    InterruptTerminate();
    if (g_Partition) {
        WHvDeleteVirtualProcessor(g_Partition, g_VpIndex);
        WHvDeletePartition(g_Partition);
        g_Partition = 0;
    }
}

/* ------------------------------------------------------------------------ */
/*  cpu_simulate - the WHP exit dispatch loop.
 *
 *  Maps each WHV_RUN_VP_EXIT_REASON onto our HAX_EXIT_* abstraction, then
 *  falls through into the same EventVdmIo / EventVdmBop / EventVdmMemAccess
 *  glue used by the HAXM backend.
 * ------------------------------------------------------------------------ */

/* Stub for the not-yet-ported hardware task switch emulator (the HAXM
 * commit 4094e4d implementation lives in core/vcpu.c; ~400 LOC).  For now
 * we surface a clear panic so the caller can see which guest selector
 * triggered the unsupported feature.
 */
static void handle_unsupported_task_switch(const WHV_RUN_VP_EXIT_CONTEXT *exit)
{
    /* TODO: port HAXM core/vcpu.c::exit_taskswitch (commit 4094e4d) into
     * user space here.  Read the new TSS selector via state._tr/state._gdt,
     * follow the JMP/CALL/IRET/task-gate path, swap GPRs and segments per
     * Intel SDM Vol 3 7.3, and write registers back via hyperv_load_state().
     */
    haxmvm_panic(
        "WHP UnsupportedFeature: hardware task switch (TSS) at CS:RIP "
        "%04X:%08llX, CR3=%08llX, TR.sel=%04X.\n\n"
        "WHP does not natively re-enter on TSS-based task switches.  The "
        "user-mode emulator for this path is not yet implemented (TODO: "
        "port HAXM commit 4094e4d 'Add Hardware task switching support').",
        (unsigned)state._cs.selector,
        (unsigned long long)state._rip,
        (unsigned long long)state._cr3,
        (unsigned)state._tr.selector);
}

/* =====================================================================
 * dasm386-based VRAM instruction handler (A0000 EGA aperture).
 *
 * There is NO 32-bit WinHvEmulation.dll: WHP ships only the 64-bit DLL, and
 * its instruction emulator is callback-driven -- gating it to 64-bit would
 * make those callbacks fire 64->32 (the reverse of NTVDM's one-way 32->64
 * Heaven's-Gate), which the codebase does not implement.  So instead we
 * decode the faulting instruction IN-PROCESS with SoftPC's own decoder
 * (softpc.new/base/dasm386, already used here for single-step prediction)
 * and replay its A0000 access through the SAS dispatch
 * (sas_PR8/16/32 -> read_b, sas_PW8/16/32 -> write_b), which carries the
 * full EGA latch / set-reset / ALU / map-mask semantics a flat aperture
 * cannot.  decode() parses real- AND protected-mode ModRM/SIB/prefixes
 * correctly, and we use the real segment bases from `state` -- this is the
 * accurate version of the reverted RMSEGOFFTOLIN stub.
 *
 * Covered: the forms DOS EGA blitters emit against VRAM -- MOV r/m<->reg,
 * MOV r/m<-imm, MOV moffs<->accumulator, and MOVS / STOS / LODS (incl. REP).
 * Anything else returns FALSE so the caller falls back to the legacy path.
 * The trap (sas.c) only arms in planar EGA/VGA BIOS modes (0Dh..12h), i.e.
 * real-mode DOS games; DOOM (mode 13h linear) is never trapped.
 * ===================================================================== */

/* Set by the trapped-VRAM exit path (hyperv_emulate_vram_access touches ONLY
 * GPRs, and MARKs GPRS).  The top of the cpu_simulate loop then skips its
 * unconditional mark-all-dirty for that one iteration, so hyperv_flush() pushes
 * just the dirty groups (GPRS, + DEBUG if YODA armed a step BP) instead of the
 * full register state -- the VCPU is authoritative for everything we did not
 * change.  Every NON-VRAM exit keeps the full mark-all sync.  See the loop. */
static int      g_vram_only_gprs_dirty = 0;

/* EGA-latch race mitigation.  A masked Keen blit is read-modify-write across
 * TWO trapped instructions: `mov al,es:[si]` (loads the CVIDC latch) ... then
 * `mov es:[di],al` (write_b USES the latch).  Each is a separate VM exit, so
 * the read->write window is ~1000x wider than on real HW; if a timer/kbd IRQ
 * is injected between them, the ISR's VRAM reads clobber the shared latch and
 * the write paints one row wrong (intermittent).  g_vram_latch_just_read is set
 * when a trapped VRAM access's last op was a READ (latch live, paired write
 * pending) and cleared on a WRITE; the loop top then defers IRQ injection by an
 * iteration so the write retires first.  Capped so a read-heavy loop (e.g. a
 * collision scan) can't starve interrupts. */
static int      g_vram_latch_just_read   = 0;
static int      g_vram_latch_defer_count = 0;
#define VRAM_LATCH_DEFER_CAP 32

/* General register read/write by (operand size 1/2/4, register number 0..7),
 * operating on the WHP shadow `state` (the get/set accessors are backed by
 * state._e?x and the setters already MARK(REG_GROUP_GPRS)). */
static DWORD vram_reg_get(int sz, unsigned n)
{
    if (sz == 1) switch (n) {
        case 0: return getAL(); case 1: return getCL();
        case 2: return getDL(); case 3: return getBL();
        case 4: return getAH(); case 5: return getCH();
        case 6: return getDH(); case 7: return getBH(); }
    else if (sz == 2) switch (n) {
        case 0: return getAX(); case 1: return getCX();
        case 2: return getDX(); case 3: return getBX();
        case 4: return getSP(); case 5: return getBP();
        case 6: return getSI(); case 7: return getDI(); }
    else switch (n) {
        case 0: return getEAX(); case 1: return getECX();
        case 2: return getEDX(); case 3: return getEBX();
        case 4: return getESP(); case 5: return getEBP();
        case 6: return getESI(); case 7: return getEDI(); }
    return 0;
}

static void vram_reg_set(int sz, unsigned n, DWORD v)
{
    if (sz == 1) switch (n) {
        case 0: setAL((UCHAR)v); break; case 1: setCL((UCHAR)v); break;
        case 2: setDL((UCHAR)v); break; case 3: setBL((UCHAR)v); break;
        case 4: setAH((UCHAR)v); break; case 5: setCH((UCHAR)v); break;
        case 6: setDH((UCHAR)v); break; case 7: setBH((UCHAR)v); break; }
    else if (sz == 2) switch (n) {
        case 0: setAX((USHORT)v); break; case 1: setCX((USHORT)v); break;
        case 2: setDX((USHORT)v); break; case 3: setBX((USHORT)v); break;
        case 4: setSP((USHORT)v); break; case 5: setBP((USHORT)v); break;
        case 6: setSI((USHORT)v); break; case 7: setDI((USHORT)v); break; }
    else switch (n) {
        case 0: setEAX(v); break; case 1: setECX(v); break;
        case 2: setEDX(v); break; case 3: setEBX(v); break;
        case 4: setESP(v); break; case 5: setEBP(v); break;
        case 6: setESI(v); break; case 7: setEDI(v); break; }
}

/* Physical (=linear, in these real-mode planar modes) VRAM access via SAS --
 * routes A0000 to CVIDC read_b/write_b exactly like hax_handle_fastmmio_op. */
static DWORD vram_read(DWORD pa, int sz)
{
    g_vram_latch_just_read = 1;   /* latch now live; protect a paired write */
    switch (sz) {
    case 1: return sas_PR8(pa);
    case 2: return sas_PR16(pa);
    default: return sas_PR32(pa);
    }
}
static void vram_write(DWORD pa, int sz, DWORD v)
{
    g_vram_latch_just_read = 0;   /* latch consumed by this write */
    switch (sz) {
    case 1: sas_PW8(pa, (BYTE)v);  break;
    case 2: sas_PW16(pa, (WORD)v); break;
    default: sas_PW32(pa, v);      break;
    }
}

/* True when arg_type is a memory operand (A_M..A_Mw, see d_oper.h). */
#define VRAM_ARG_IS_MEM(at) ((at) >= A_M  && (at) <= A_Mw)
#define VRAM_ARG_IS_REG(at) ((at) >= A_Rb && (at) <= A_Rd)

/* --- ALU (binary read/modify) support for VRAM operands -----------------
 * Keen composites sprites with CPU-side logic on the EGA aperture, e.g.
 * `AND AL,ES:[DI]`, `OR ES:[DI],AL`, `XCHG AL,ES:[DI]`.  These read VRAM
 * (loading the EGA latches) and, crucially, set FLAGS -- so we must compute
 * the result AND the flags or the guest mis-branches and derails. */
enum { ALU_ADD, ALU_OR, ALU_ADC, ALU_SBB, ALU_AND, ALU_SUB, ALU_XOR,
       ALU_CMP, ALU_TEST };

/* inst_id -> (alu op, size 1/2/4, writes-back?).  Returns -1 if not an ALU. */
static int alu_op_of(USHORT id, int *sz, int *writes)
{
    *writes = 1;
    switch (id) {
    case I_ADD8: *sz=1; return ALU_ADD;  case I_ADD16:*sz=2; return ALU_ADD;  case I_ADD32:*sz=4; return ALU_ADD;
    case I_OR8:  *sz=1; return ALU_OR;   case I_OR16: *sz=2; return ALU_OR;   case I_OR32: *sz=4; return ALU_OR;
    case I_ADC8: *sz=1; return ALU_ADC;  case I_ADC16:*sz=2; return ALU_ADC;  case I_ADC32:*sz=4; return ALU_ADC;
    case I_SBB8: *sz=1; return ALU_SBB;  case I_SBB16:*sz=2; return ALU_SBB;  case I_SBB32:*sz=4; return ALU_SBB;
    case I_AND8: *sz=1; return ALU_AND;  case I_AND16:*sz=2; return ALU_AND;  case I_AND32:*sz=4; return ALU_AND;
    case I_SUB8: *sz=1; return ALU_SUB;  case I_SUB16:*sz=2; return ALU_SUB;  case I_SUB32:*sz=4; return ALU_SUB;
    case I_XOR8: *sz=1; return ALU_XOR;  case I_XOR16:*sz=2; return ALU_XOR;  case I_XOR32:*sz=4; return ALU_XOR;
    case I_CMP8: *sz=1; *writes=0; return ALU_CMP;  case I_CMP16:*sz=2; *writes=0; return ALU_CMP;  case I_CMP32:*sz=4; *writes=0; return ALU_CMP;
    case I_TEST8:*sz=1; *writes=0; return ALU_TEST; case I_TEST16:*sz=2;*writes=0; return ALU_TEST; case I_TEST32:*sz=4;*writes=0; return ALU_TEST;
    default: return -1;
    }
}

static int byte_parity(DWORD v)   /* 1 if low byte has even number of set bits */
{
    v &= 0xFF; v ^= v >> 4; v ^= v >> 2; v ^= v >> 1; return !(v & 1);
}

/* Compute `a OP b` at width sz, updating *ef (the EFLAGS image). */
static DWORD alu_compute(int op, DWORD a, DWORD b, int sz, DWORD *ef)
{
    DWORD  mask = (sz==1)?0xFFu : (sz==2)?0xFFFFu : 0xFFFFFFFFu;
    DWORD  sign = (sz==1)?0x80u : (sz==2)?0x8000u : 0x80000000u;
    DWORD  cin  = (*ef & 0x1u);
    UINT64 wide;
    DWORD  res = 0;
    int    cf = 0, of = 0, af = 0;

    a &= mask; b &= mask;
    switch (op) {
    case ALU_ADD:           wide = (UINT64)a + b;       res = (DWORD)wide & mask; cf = (wide > mask); af = ((a^b^res)&0x10)!=0; of = ((~(a^b))&(a^res)&sign)!=0; break;
    case ALU_ADC:           wide = (UINT64)a + b + cin; res = (DWORD)wide & mask; cf = (wide > mask); af = ((a^b^res)&0x10)!=0; of = ((~(a^b))&(a^res)&sign)!=0; break;
    case ALU_SUB: case ALU_CMP: wide = (UINT64)a - b;  res = (DWORD)wide & mask; cf = (a < b);        af = ((a^b^res)&0x10)!=0; of = (((a^b))&(a^res)&sign)!=0; break;
    case ALU_SBB:           wide = (UINT64)a - b - cin; res = (DWORD)wide & mask; cf = ((UINT64)a < (UINT64)b + cin); af = ((a^b^res)&0x10)!=0; of = (((a^b))&(a^res)&sign)!=0; break;
    case ALU_AND: case ALU_TEST: res = (a & b) & mask; break;
    case ALU_OR:            res = (a | b) & mask;       break;
    case ALU_XOR:           res = (a ^ b) & mask;       break;
    }
    /* SF/ZF/PF from result; CF/OF/AF as computed (0 for logic ops). */
    *ef &= ~(0x1u|0x4u|0x10u|0x40u|0x80u|0x800u);
    if (cf)            *ef |= 0x1u;
    if (af)            *ef |= 0x10u;
    if (of)            *ef |= 0x800u;
    if (res == 0)      *ef |= 0x40u;
    if (res & sign)    *ef |= 0x80u;
    if (byte_parity(res)) *ef |= 0x4u;
    return res;
}

/* Decode the faulting instruction and replay its A0000 access.  Returns TRUE
 * if fully handled (state GPRs + RIP advanced), FALSE to fall back. */
static BOOL hyperv_emulate_vram_access(const WHV_MEMORY_ACCESS_CONTEXT *ma)
{
    int    code_32 = (!!(state._cr0 & 1u) && !(state._eflags & 0x20000u))
                     ? (state._cs.operand_size ? 1 : 0) : 0;
    DWORD  cs_base = (DWORD)state._cs.base;
    DWORD  ip      = code_32 ? (DWORD)state._eip : (DWORD)(WORD)state._eip;
    DWORD  cur_lin = cs_base + ip;
    SIZE_SPECIFIER dec_sz = code_32 ? THIRTY_TWO_BIT : SIXTEEN_BIT;
    DWORD  gpa     = (DWORD)ma->Gpa;
    DECODED_INST d;

    decode(cur_lin, &d, dec_sz, sw_step_read_byte);

    switch (d.inst_id) {

    /* ---- plain MOV: one operand in VRAM, the other reg / imm / accum ---- */
    case I_MOV8: case I_MOV16: case I_MOV32: {
        DECODED_ARG *a0 = &d.args[0];   /* destination (Intel order) */
        DECODED_ARG *a1 = &d.args[1];   /* source                    */
        int sz = (d.inst_id == I_MOV8) ? 1 : (d.inst_id == I_MOV16 ? 2 : 4);

        if (VRAM_ARG_IS_MEM(a0->arg_type) && !VRAM_ARG_IS_MEM(a1->arg_type)) {
            DWORD val;
            if (a1->arg_type == A_I)            val = (DWORD)DCD_IMMED1(a1);
            else if (VRAM_ARG_IS_REG(a1->arg_type))
                val = vram_reg_get(sz, (unsigned)DCD_IDENTIFIER(a1));
            else break;            /* odd src (e.g. Sreg) -> safe skip */
            vram_write(gpa, sz, val);
        } else if (VRAM_ARG_IS_MEM(a1->arg_type) &&
                   VRAM_ARG_IS_REG(a0->arg_type)) {
            vram_reg_set(sz, (unsigned)DCD_IDENTIFIER(a0), vram_read(gpa, sz));
        }
        /* else: odd operand layout -> safe skip (RIP still advances below) */
        break;
    }

    /* ---- STOS [ES:(E)DI] <- AL/AX/EAX (REP loops (E)CX) ---- */
    case I_STOSB: case I_STOSW: case I_STOSD:
    case I_R_STOSB: case I_R_STOSW: case I_R_STOSD: {
        int   rep  = (d.inst_id == I_R_STOSB || d.inst_id == I_R_STOSW ||
                      d.inst_id == I_R_STOSD);
        int   sz   = (d.inst_id == I_STOSB || d.inst_id == I_R_STOSB) ? 1
                   : (d.inst_id == I_STOSW || d.inst_id == I_R_STOSW) ? 2 : 4;
        int   a32  = (d.address_size == ADDR_32);
        int   step = getDF() ? -sz : sz;
        DWORD es   = (DWORD)state._es.base;
        DWORD edi  = a32 ? getEDI() : getDI();
        DWORD data = vram_reg_get(sz, 0);                 /* AL/AX/EAX */
        DWORD cnt  = rep ? (a32 ? getECX() : getCX()) : 1;
        g_vram_string_elems += cnt;

        while (cnt--) {
            vram_write(es + (a32 ? edi : (edi & 0xFFFF)), sz, data);
            edi += step;
        }
        if (a32) { setEDI(edi);            if (rep) setECX(0); }
        else     { setDI((USHORT)edi);     if (rep) setCX(0);  }
        break;
    }

    /* ---- MOVS [ES:(E)DI] <- [DS:(E)SI] (REP loops (E)CX) ---- */
    case I_MOVSB: case I_MOVSW: case I_MOVSD:
    case I_R_MOVSB: case I_R_MOVSW: case I_R_MOVSD: {
        int   rep  = (d.inst_id == I_R_MOVSB || d.inst_id == I_R_MOVSW ||
                      d.inst_id == I_R_MOVSD);
        int   sz   = (d.inst_id == I_MOVSB || d.inst_id == I_R_MOVSB) ? 1
                   : (d.inst_id == I_MOVSW || d.inst_id == I_R_MOVSW) ? 2 : 4;
        int   a32  = (d.address_size == ADDR_32);
        int   step = getDF() ? -sz : sz;
        DWORD ds   = (DWORD)state._ds.base;   /* segment override ignored */
        DWORD es   = (DWORD)state._es.base;
        DWORD esi  = a32 ? getESI() : getSI();
        DWORD edi  = a32 ? getEDI() : getDI();
        DWORD cnt  = rep ? (a32 ? getECX() : getCX()) : 1;
        g_vram_string_elems += cnt;

        while (cnt--) {
            DWORD v = vram_read(ds + (a32 ? esi : (esi & 0xFFFF)), sz);
            vram_write(es + (a32 ? edi : (edi & 0xFFFF)), sz, v);
            esi += step; edi += step;
        }
        if (a32) { setESI(esi); setEDI(edi);              if (rep) setECX(0); }
        else     { setSI((USHORT)esi); setDI((USHORT)edi); if (rep) setCX(0); }
        break;
    }

    /* ---- LODS AL/AX/EAX <- [DS:(E)SI] (REP loads last; rare) ---- */
    case I_LODSB: case I_LODSW: case I_LODSD:
    case I_R_LODSB: case I_R_LODSW: case I_R_LODSD: {
        int   rep  = (d.inst_id == I_R_LODSB || d.inst_id == I_R_LODSW ||
                      d.inst_id == I_R_LODSD);
        int   sz   = (d.inst_id == I_LODSB || d.inst_id == I_R_LODSB) ? 1
                   : (d.inst_id == I_LODSW || d.inst_id == I_R_LODSW) ? 2 : 4;
        int   a32  = (d.address_size == ADDR_32);
        int   step = getDF() ? -sz : sz;
        DWORD ds   = (DWORD)state._ds.base;
        DWORD esi  = a32 ? getESI() : getSI();
        DWORD cnt  = rep ? (a32 ? getECX() : getCX()) : 1;
        DWORD v    = vram_reg_get(sz, 0);

        g_vram_string_elems += cnt;
        while (cnt--) {
            v = vram_read(ds + (a32 ? esi : (esi & 0xFFFF)), sz);
            esi += step;
        }
        vram_reg_set(sz, 0, v);
        if (a32) { setESI(esi);        if (rep) setECX(0); }
        else     { setSI((USHORT)esi); if (rep) setCX(0);  }
        break;
    }

    default: {
        /* ---- binary ALU with one VRAM operand (AND/OR/XOR/ADD/...) ---- */
        int sz, writes, op = alu_op_of(d.inst_id, &sz, &writes);
        if (op >= 0) {
            DECODED_ARG *a0 = &d.args[0];   /* dest (Intel order) */
            DECODED_ARG *a1 = &d.args[1];   /* src                */
            int   m0 = VRAM_ARG_IS_MEM(a0->arg_type);
            int   m1 = VRAM_ARG_IS_MEM(a1->arg_type);
            DWORD ef = (DWORD)state._eflags, v0, v1, res;

            v0 = m0 ? vram_read(gpa, sz)
               : (a0->arg_type == A_I) ? (DWORD)DCD_IMMED1(a0)
               : vram_reg_get(sz, (unsigned)DCD_IDENTIFIER(a0));
            v1 = m1 ? vram_read(gpa, sz)
               : (a1->arg_type == A_I) ? (DWORD)DCD_IMMED1(a1)
               : vram_reg_get(sz, (unsigned)DCD_IDENTIFIER(a1));

            res = alu_compute(op, v0, v1, sz, &ef);
            state._eflags = ef;

            if (writes) {                   /* CMP/TEST write nothing */
                if (m0)      vram_write(gpa, sz, res);
                else if (VRAM_ARG_IS_REG(a0->arg_type))
                             vram_reg_set(sz, (unsigned)DCD_IDENTIFIER(a0), res);
            }
            break;
        }

        /* ---- XCHG reg <-> VRAM (no flags) ---- */
        if (d.inst_id == I_XCHG8 || d.inst_id == I_XCHG16 ||
            d.inst_id == I_XCHG32) {
            int sz2 = (d.inst_id == I_XCHG8) ? 1 : (d.inst_id == I_XCHG16) ? 2 : 4;
            DECODED_ARG *am = VRAM_ARG_IS_MEM(d.args[0].arg_type) ? &d.args[0] : &d.args[1];
            DECODED_ARG *ar = VRAM_ARG_IS_MEM(d.args[0].arg_type) ? &d.args[1] : &d.args[0];
            if (VRAM_ARG_IS_REG(ar->arg_type)) {
                DWORD mv = vram_read(gpa, sz2);
                DWORD rv = vram_reg_get(sz2, (unsigned)DCD_IDENTIFIER(ar));
                vram_write(gpa, sz2, rv);
                vram_reg_set(sz2, (unsigned)DCD_IDENTIFIER(ar), mv);
            }
            (void)am;
            break;
        }

        /* Truly unhandled form.  The legacy stub (handle_memory_access)
         * mis-decodes trapped protected-mode/REP VRAM accesses and corrupts
         * RIP -> hard derail.  Safer: advance past the instruction by the
         * DECODED length (decode() is reliable) and drop the access -- a
         * visual glitch, but control flow survives.  The diagnostic probe
         * above has already recorded the opcode so it can be added. */
        break;
    }
    }

    /* RIP advance.  Use decode()'s d.inst_sz -- it is the only RELIABLE length
     * source here.  WHP's ma->InstructionByteCount is NOT the instruction
     * length: it is the count of bytes WHP copied into ma->InstructionBytes[16]
     * (typically the full 16-byte window -> hwlen=16), and VpContext.Instruction
     * Length comes back 0 on these MemoryAccess exits.  Trusting either over-
     * advances RIP and skips instructions (see project_wpsnt_whp_derail:
     * spdos's B8000 text-detect skipped a POP ES -> unbalanced stack -> NEAR
     * RET into garbage -> INT3 hang).  decode() returns 5/4/5 correctly for the
     * `26 8a/88 26 ..` ES:MOV [disp16],r8 forms, confirmed by the probe. */
    state._rip += d.inst_sz;     /* inst_sz includes prefix bytes */
    MARK(REG_GROUP_GPRS);
    return TRUE;
}

/* REP INS/OUTS string port I/O.  Unlike HAXM (whose kernel pre-copies the guest
 * buffer and advances the registers), WHP hands us only the raw I/O context, so
 * we emulate the whole string op in process: loop RCX times moving each element
 * between the port (EventVdmIo over iobuf, exactly as the single-element path
 * does) and guest memory -- DS:SI for OUTS, ES:DI for INS, via vram_read/
 * vram_write which dispatch RAM and VRAM correctly -- then advance SI/DI and zero
 * RCX.  The old slow path only moved AL/AX once, which left a REP OUTS to the VGA
 * DAC (3C9) writing a single byte -> scrambled palette.  Segment overrides are
 * ignored, matching the MOVS handler.  RIP is advanced by the caller. */
static void hyperv_string_io(const WHV_X64_IO_PORT_ACCESS_CONTEXT *io)
{
    int    code_32 = (!!(state._cr0 & 1u) && !(state._eflags & 0x20000u))
                     ? (state._cs.operand_size ? 1 : 0) : 0;
    DWORD  ip      = code_32 ? (DWORD)state._eip : (DWORD)(WORD)state._eip;
    DWORD  cur_lin = (DWORD)state._cs.base + ip;
    SIZE_SPECIFIER dec_sz = code_32 ? THIRTY_TWO_BIT : SIXTEEN_BIT;
    DECODED_INST d;
    int    a32, sz, step;
    BOOL   is_out = io->AccessInfo.IsWrite;
    DWORD  ds, es, esi, edi, cnt, i;

    decode(cur_lin, &d, dec_sz, sw_step_read_byte);
    a32  = (d.address_size == ADDR_32);
    sz   = (int)io->AccessInfo.AccessSize;
    step = getDF() ? -sz : sz;
    cnt  = io->AccessInfo.RepPrefix ? (a32 ? getECX() : getCX()) : 1;
    ds   = (DWORD)state._ds.base;       /* OUTS source seg (override ignored) */
    es   = (DWORD)state._es.base;       /* INS dest seg                       */
    esi  = a32 ? getESI() : getSI();
    edi  = a32 ? getEDI() : getDI();

    for (i = 0; i < cnt; i++) {
        if (is_out) {
            DWORD v = vram_read(ds + (a32 ? esi : (esi & 0xFFFF)), sz);
            memcpy(iobuf, &v, sz);
            EventVdmIo();                       /* out{b,w} port, iobuf */
            esi += step;
        } else {
            DWORD v = 0;
            EventVdmIo();                       /* in{b,w} port -> iobuf */
            memcpy(&v, iobuf, sz);
            vram_write(es + (a32 ? edi : (edi & 0xFFFF)), sz, v);
            edi += step;
        }
    }

    if (is_out) { if (a32) setESI(esi); else setSI((USHORT)esi); }
    else        { if (a32) setEDI(edi); else setDI((USHORT)edi); }
    if (io->AccessInfo.RepPrefix) { if (a32) setECX(0); else setCX(0); }
    /* set*() already MARK(REG_GROUP_GPRS). */
}

/* Service a generic memory-access exit by forwarding to the existing
 * fast-MMIO handler.  Used for NON-video MMIO / page faults only -- the A0000
 * EGA aperture is fully serviced by hyperv_emulate_vram_access (which never
 * falls through to here).  hax_handle_fastmmio_op covers the simple MOV/string
 * forms NTVDM otherwise generates.
 */
static void handle_memory_access(const WHV_MEMORY_ACCESS_CONTEXT *ma)
{
    struct hax_fastmmio hft = {0};

    hft.gpa       = ma->Gpa;
    hft.direction = (ma->AccessInfo.AccessType == WHvMemoryAccessWrite) ? 1 : 0;
    /* The WHP exit does not carry the access width; we infer from the
     * decoded instruction bytes for the common widths and fall back to 4. */
    hft.size      = 4;
    if (ma->InstructionByteCount >= 1) {
        BYTE op = ma->InstructionBytes[0];
        if (op == 0x66) hft.size = 2;
        else if (op == 0xA0 || op == 0xA2 || op == 0x88 || op == 0x8A) hft.size = 1;
    }
    if (hft.direction == 1) {
        memcpy(&hft.value, ma->InstructionBytes, hft.size);
    }

    tunnel->_exit_status = HAX_EXIT_FAST_MMIO;
    hax_handle_fastmmio(&hft);

    /* Advance RIP past the faulting instruction by the DECODED length.  WHP's
     * ma->InstructionByteCount is the count of bytes copied into the 16-byte
     * InstructionBytes[] window (commonly 16), NOT the instruction length, and
     * VpContext.InstructionLength comes back 0 on these exits -- trusting
     * either over-advances RIP and skips instructions (project_wpsnt_whp_
     * derail).  decode() is the reliable source. */
    {
        int   code_32 = (!!(state._cr0 & 1u) && !(state._eflags & 0x20000u))
                        ? (state._cs.operand_size ? 1 : 0) : 0;
        DWORD ip      = code_32 ? (DWORD)state._eip : (DWORD)(WORD)state._eip;
        DWORD cur_lin = (DWORD)state._cs.base + ip;
        SIZE_SPECIFIER dec_sz = code_32 ? THIRTY_TWO_BIT : SIXTEEN_BIT;
        DECODED_INST d;
        DWORD adv;
        decode(cur_lin, &d, dec_sz, sw_step_read_byte);
        adv = d.inst_sz;
        if (adv < 1 || adv > 15) adv = 1;   /* decode failure: minimal step */
        state._rip += adv;
    }
    MARK(REG_GROUP_GPRS);
}

/* ------------------------------------------------------------------------
 *  I/O exit fast-path.
 *
 *  Profiling shows planar blitters dominate the exit mix with simple port
 *  OUTs -- DOOM hammers the VGA Map-Mask (3C5h) ~9000x/s, and on the slow
 *  path each one pays a full WHvGetVirtualProcessorRegisters(ALL) +
 *  WHvSetVirtualProcessorRegisters(ALL) round-trip (~73% of per-exit time)
 *  even though an IN/OUT only changes the I/O data (AL/AX/EAX) and RIP.
 *
 *  Here we service the instruction straight from the WHP exit context and
 *  write back ONLY RAX + RIP, leaving every other VCPU register untouched
 *  (and therefore authoritative -- no fetch-all, no clobbering push-all).
 *
 *  Returns 1 if it fully handled the access, 0 if it declined (string/REP
 *  I/O, odd size, or no trustworthy instruction length) -- the caller then
 *  falls back to the normal full-marshalling path.  Eligibility wrt pending
 *  IRQs / tracing / TF-recovery is checked by the caller. */
static int hyperv_io_fastpath(const WHV_RUN_VP_EXIT_CONTEXT *exit)
{
    const WHV_X64_IO_PORT_ACCESS_CONTEXT *io = &exit->IoPortAccess;
    unsigned size = io->AccessInfo.AccessSize;
    int      adv;
    UINT64   new_rax;
    HRESULT  hr;

    /* String / REP I/O needs RCX/RSI/RDI/DF + a copy loop -- defer to the
     * full-state slow path. */
    if (io->AccessInfo.StringOp || io->AccessInfo.RepPrefix)
        return 0;
    if (size == 0 || size > 4)
        return 0;

    /* Trustworthy RIP advance only.  io->InstructionByteCount is correct for
     * OUT (DOOM's 3C5h storm) and VpContext.InstructionLength is WHP's
     * general field; if neither is sane the slow path must decode at the
     * full linear RIP via io_insn_len() (which needs fetched state), so we
     * decline.  Never predict_next_lin() -- it truncates 32-bit EIP, the old
     * derail. */
    adv = (int)io->InstructionByteCount;
    if (adv <= 0 || adv > 15) adv = (int)exit->VpContext.InstructionLength;
    if (adv <= 0 || adv > 15) return 0;

    /* Drive the device emulation exactly as the slow path does. */
    tunnel->_exit_status  = HAX_EXIT_IO;
    tunnel->io._port      = io->PortNumber;
    tunnel->io._size      = (uint16_t)size;
    tunnel->io._direction = io->AccessInfo.IsWrite ? HAX_EXIT_IO_OUT
                                                   : HAX_EXIT_IO_IN;
    if (io->AccessInfo.IsWrite)
        memcpy(iobuf, &io->Rax, size);

    VdmTib.EventInfo.IoInfo.PortNumber = io->PortNumber;
    VdmTib.EventInfo.IoInfo.Size       = tunnel->io._size;
    VdmTib.EventInfo.IoInfo.Read       = !io->AccessInfo.IsWrite;
    EventVdmIo();

    /* IN merges the read bytes into the low size*8 bits of RAX; OUT leaves
     * RAX unchanged. */
    new_rax = io->Rax;
    if (!io->AccessInfo.IsWrite) {
        UINT64 rax  = 0;
        UINT64 mask = (1ULL << (size * 8)) - 1;   /* size in [1..4] */
        memcpy(&rax, iobuf, size);
        new_rax = (io->Rax & ~mask) | (rax & mask);
    }

    /* Push back RIP (always) and RAX (IN only).  We intentionally do NOT
     * preserve the interrupt shadow here: the fast path runs only when no IRQ
     * is pending, and the shadow (a one-instruction STI/MOV-SS artifact) no
     * longer applies once this I/O instruction has retired.
     *
     * For OUT -- DOOM's 3C5h Map-Mask storm is entirely OUT -- RAX is unchanged
     * by the instruction, so set ONLY RIP (1 register).  This is the dominant
     * exit, so trimming the WHvSetVirtualProcessorRegisters payload to a single
     * register shaves the per-exit set-back cost on the hottest path. */
    {
        WHV_REGISTER_NAME  names[2] = { WHvX64RegisterRip, WHvX64RegisterRax };
        WHV_REGISTER_VALUE vals[2]  = {0};
        UINT32             n;
        vals[0].Reg64 = exit->VpContext.Rip + (UINT64)adv;
        if (io->AccessInfo.IsWrite) {
            n = 1;                       /* OUT: RIP only (RAX unchanged) */
        } else {
            vals[1].Reg64 = new_rax;     /* IN: also merge the read into RAX */
            n = 2;
        }
        hr = WHvSetVirtualProcessorRegisters(g_Partition, g_VpIndex,
                                             names, n, vals);
        if (FAILED(hr))
            haxmvm_panic("WHvSetVirtualProcessorRegisters(IO fastpath) "
                         "failed (0x%08X)", hr);
    }
    return 1;
}

EXPORT
VOID
cpu_simulate(
    )
{
    VDMEVENTINFO  OldEventInfo;
    CONTEXT       OldMonitorContext;
    int           io_fast_prev = 0;   /* prev iter took the I/O fast-path */
    LONGLONG      pit_next_qpc = 0;   /* wall-clock deadline for next timer-0 tick */

    OldEventInfo      = VdmTib.EventInfo;
    OldMonitorContext = VdmTib.MonitorContext;
    ContinueExecution = TRUE;
#if WHP_PIT_TIMER_THREAD
    pit_timer_start_if_needed();   /* PIT-rate timer thread (sound timing fix) */
#endif

    /* Publish the VCPU/main thread ID so cpu_interrupt() (called from
     * timer / event / com / mouse worker threads) can tell when to
     * issue WHvCancelRunVirtualProcessor to wake us out of HLT. */
    g_cpu_simulate_tid = GetCurrentThreadId();

    while (ContinueExecution) {
        WHV_RUN_VP_EXIT_CONTEXT exit = {0};
        HRESULT hr;

        ASSERT(InterlockedIncrement(&NumTasks) == 0);

        tunnel->request_interrupt_window =
            (*pNtVDMState & VDM_INTERRUPT_PENDING) ? 1 : 0;

        /* I/O fast-path continuation: the previous iteration emulated a port
         * IN/OUT straight from the exit context and wrote back only RAX+RIP,
         * so the VCPU's other registers are authoritative.  A push-all here
         * would clobber whatever the guest changed during its free-run, so
         * re-enter the guest directly with an empty push phase.  Any IRQ that
         * raced in is caught below: the worker's WHvCancelRunVirtualProcessor
         * returns a Canceled exit, which fails the fast-path test and drops
         * us onto the slow path that fetches full state and injects. */
        if (io_fast_prev) {
            io_fast_prev = 0;
            goto run_vcpu;
        }

        /* Drain pending host-queued IRQs into the VCPU on every iteration.
         *
         * The HAXM backend got this for free: its kernel driver would exit
         * with an InterruptWindow status whenever IF transitioned to 1 and
         * an IRQ was pending in pNtVDMState, and the InterruptWindow
         * handler called DispatchInterrupts which queued the actual WHP
         * pending-interruption register.  WHP only fires its own
         * InterruptWindow exit if WHvRegisterPendingInterruption was
         * already set (chicken-and-egg) -- so when the guest is busy in
         * a tight loop that doesn't HLT (e.g. DOS idle BOP MS_bop_A
         * polling), the worker threads keep setting VDM_INT_TIMER /
         * VDM_INT_HARDWARE in pNtVDMState but nothing ever drains them
         * into a WHP-side injection, and the VCPU spins forever.
         *
         * Calling DispatchInterrupts here on every iteration:
         *   - advances host_timer_event() so the BIOS tick counter
         *     progresses (otherwise DOS / DOSX think no time passed)
         *   - if IF=1 and a hardware IRQ is queued, calls
         *     hyperv_inject_interrupt(vector) so the VCPU delivers it
         *     on the next WHvRunVirtualProcessor below.
         *
         * Cost is one cheap function call per loop iteration when no IRQ
         * is pending; the test on pNtVDMState early-outs in that case.
         *
         * No TF gate here anymore: hyperv_inject_interrupt() now arms
         * DR0 at the IDT handler's first instruction whenever
         * NANO_ENABLED is on, so a delivered IRQ traps at handler entry
         * and the trace steps through the handler instead of going dark.
         *
         * g_skip_next_dispatch is set by the #DB handler when it just
         * detected an IRET-target trap: skipping injection for that one
         * iteration lets the resumed main-code instruction execute
         * before the next queued IRQ is delivered, breaking the
         * "IRQ -> handler -> IRET -> same RIP" livelock that otherwise
         * starves the JMP at the return site forever. */

        /* 8259 lost-EOI recovery (same self-heal HAXM uses).  An IRQ0 stuck
         * in-service with no EOI -- delivered at a video mode change or during
         * an IF=0 level-entry spin where the guest never runs the timer ISR --
         * blocks every lower-priority ack in the 8259.  The slave mouse IRQ9
         * (cascade via IRQ2) then can never be acked/injected, so the mouse
         * driver's bMseEoiPending sticks and the mouse freezes (Lemmings
         * "entering the level": 0 timer + 0 mouse injections while a HW IRQ is
         * pending).  Force a specific EOI on a line whose in-service bit stays
         * set while the guest CS is PINNED (a genuine single-CS spin wedge),
         * NEVER while CS moves (a running ISR / interrupt reflection) -- that
         * would defeat 8259 nesting.  Run before the dispatch attempt so the
         * freed line can be injected this same iteration.  state._cs is fresh
         * (fetched after the previous run).  Cheap: scans 16 ISR bits. */
        {
            extern int  ica_unwedge_stuck(unsigned cur_cs);  /* ica.c */
            extern void host_ica_lock(void);                 /* nt_eoi.c */
            extern void host_ica_unlock(void);               /* nt_eoi.c */
            host_ica_lock();
            ica_unwedge_stuck((unsigned)state._cs.selector);
            host_ica_unlock();
        }

        if (g_skip_next_dispatch) {
            g_skip_next_dispatch = 0;
        } else if (g_vram_latch_just_read &&
                   g_vram_latch_defer_count < VRAM_LATCH_DEFER_CAP) {
            /* A trapped VRAM read just loaded the EGA latch and a paired write
             * is expected on the next exit -- defer IRQ injection one iteration
             * so the write retires before an ISR can clobber the latch (the
             * trap widens this window ~1000x vs real HW).  Capped against
             * interrupt starvation; inert when IF=0 (DispatchInterrupts would
             * decline anyway) -- the IF=0 window-arming below is independent. */
            g_vram_latch_defer_count++;
        } else {
            g_vram_latch_defer_count = 0;
            if (*pNtVDMState & VDM_INTERRUPT_PENDING)
                DispatchInterrupts();
        }
        g_vram_latch_just_read = 0;   /* one-shot: consumed each iteration */

        /* If a HW IRQ is STILL pending after the dispatch attempt, the guest
         * has interrupts disabled (IF=0) and DispatchInterrupts declined.
         * Arm WHP's interrupt-window notification so we get an InterruptWindow
         * exit the instant IF->1 and can inject then -- otherwise an IF=0 spin
         * loop (Keen 4 "Loading") never receives the pending timer IRQ.  Disarm
         * once nothing is pending.  state._eflags is fresh (fetched after the
         * last run / by the BOP slow path).
         *
         * CONSERVATIVE GATE: only arm after the (IRQ-pending && IF=0) condition
         * has held for INTWIN_STUCK_ITERS consecutive iterations -- a genuine
         * sustained disabled-interrupt spin (Keen).  A normally-delivering guest
         * (DOOM) only has brief transient IF=0 windows; arming on those added
         * ~12-32 InterruptWindow injections/sec, each a chance to hit the
         * IRQ0-stuck-in-service race (a delivered-but-never-EOI'd IRQ0 wedges
         * the 8259).  Gating it out keeps Keen fixed without perturbing DOOM. */
        {
            static int s_if0_pend_streak = 0;
            int stuck = (*pNtVDMState & VDM_INTERRUPT_PENDING)
                     && !(state._eflags & 0x200);
            if (stuck) {
                if (s_if0_pend_streak < INTWIN_STUCK_ITERS)
                    s_if0_pend_streak++;
            } else {
                s_if0_pend_streak = 0;
            }
            hyperv_arm_interrupt_window(s_if0_pend_streak >= INTWIN_STUCK_ITERS);
        }

        /* Symmetric full-state push (paired with the full-state fetch
         * after WHvRunVirtualProcessor below).
         *
         * Handler code dispatched from the previous iteration -- EventVdmBop,
         * host_swint_hook, host_hwint_hook, host_exint_hook, EventVdmIo,
         * video MMIO emulation, etc. -- runs general NTVDM C code that
         * mutates `state.*` without going through any MARK()-aware setter.
         * Combined with the symmetric fetch-all below, this guarantees
         * that whatever is in `state` at the top of the loop is what the
         * VCPU sees, and whatever the VCPU produced is what NTVDM handlers
         * see on the next exit -- no stale fields, no silently-dropped
         * IDT/GDT/CR0 changes (which was breaking DOSX/DPMI). */

        /* Software-step BP arming: while YODA single-step is active,
         * predict the next instruction's RIP and arm DR1 (and DR2 for
         * conditional branches) so we trap reliably -- WHP's TF #DB is
         * unreliable post-STI / post-IRQ-delivery (see memory:
         * project_hyperv_tf_intercept_bug.md).  Done before the flush so
         * the DR registers reach the VCPU. */
        if (GLOBAL_nanoDebugControl & HV_NANO_ENABLED)
            (void)hyperv_step_bp_for_next();

        /* Keep WHP #GP interception in sync with yoda's bex/vex flags so
         * break-on-fault / verbose-exceptions work for PM faults. */
        hyperv_sync_pmfault_trap();

        /* Normally re-sync the full register state to the VCPU before each
         * run.  But after a clean trapped-VRAM exit the handler changed only
         * GPRs (already MARKed), and the VCPU is authoritative for everything
         * else, so skip the mark-all and let hyperv_flush() push just the
         * dirty groups -- push_gprs(18) instead of push_all(39).  Any group
         * a pre-run step (YODA DR arming, etc.) marked is still flushed. */
        if (g_vram_only_gprs_dirty)
            g_vram_only_gprs_dirty = 0;
        else
            hyperv_mark_dirty(REG_GROUP_ALL);
        hyperv_flush();

        /* Mark ourselves "inside WHvRunVp" so worker threads can decide
         * whether a WHvCancelRunVirtualProcessor hypercall is actually
         * needed.  Reset g_cancel_pending so the FIRST worker call this
         * iteration issues the real cancel and subsequent ones piggy-back. */
    run_vcpu:
        InterlockedExchange(&g_cancel_pending, 0);
        InterlockedExchange(&g_in_whvrunvp,   1);
        hr = WHvRunVirtualProcessor(g_Partition, g_VpIndex,
                                    &exit, sizeof(exit));
        InterlockedExchange(&g_in_whvrunvp,   0);
        if (FAILED(hr))
            haxmvm_panic("WHvRunVirtualProcessor failed (0x%08X)", hr);

        ASSERT(InterlockedDecrement(&NumTasks) < 0);

        /* Auto-detect a Chinese-font TSR (spdos) and, if found, flip this session
         * to full-CVIDC (g_video_strategy 1->3).  UPDATE52. */
        {
            extern int  g_video_strategy;
            extern void video_strategy_autodetect(void);
            video_strategy_autodetect();
            /* Strategy 3 skips the flat-aperture sync/blit -- the display comes
             * from the CVIDC native refresh instead (like CCPU). */
            if (g_video_strategy != 3)
                haxm_sync_vram();
        }

        /* -------- I/O fast-path --------
         * The dominant exit for planar blitters (DOOM hammering the VGA
         * Map-Mask at 3C5h ~9000x/s) is a simple port OUT.  Service it from
         * the WHP exit context and write back only RAX+RIP, skipping the
         * fetch-all/push-all that otherwise costs ~73% of each exit.  Only
         * when no IRQ is pending (injection needs full state), not tracing,
         * and not mid TF-recovery; hyperv_io_fastpath() declines string/REP
         * I/O and any access without a trustworthy instruction length. */
        if (WHP_IO_FASTPATH
            && exit.ExitReason == WHvRunVpExitReasonX64IoPortAccess
            && !(*pNtVDMState & VDM_INTERRUPT_PENDING)
            && !(GLOBAL_nanoDebugControl & HV_NANO_ENABLED)
            && !g_retf_on_next_exit
            && hyperv_io_fastpath(&exit)) {
            io_fast_prev = 1;
            continue;        /* -> top of loop -> goto run_vcpu (no push/fetch) */
        }

        /*
         * Full snapshot pull.  Originally we did per-exit selective fetch
         * (only the groups the specific exit handler reads), pairing with
         * MARK()-tracked selective push on the next iteration.  That broke
         * for cross-file writes to `state` by general NTVDM code, AND
         * worse: the mark-all-dirty push above pushes groups we never
         * fetched, overwriting WHP-authoritative values (e.g. an IDTR
         * just loaded by DOSX with LIDT got silently reverted to whatever
         * stale value we had in state._idt before the run).
         *
         * Symmetric fetch-all + push-all is the only correct option until
         * every cross-file write goes through a MARK()-aware setter.  This
         * is one WHvGet + one WHvSet (k_AllNames), not five.  NB: profiling
         * showed it is NOT cheap for I/O-bound guests -- the fetch-all alone
         * was ~55% of per-exit time when DOOM floods 3C5h -- which is exactly
         * why simple port I/O is now serviced by the fast-path above and
         * never reaches here.  The per-exit selective fetches inside the
         * switch below become no-ops (state is already up to date) but we
         * leave them in place to minimise diff.
         *
         * EXCEPTION -- trapped-VRAM MMIO (when not tracing): that path fetches
         * its own minimal register set (pull_vram, 13 regs) in the switch AND
         * pushes GPRS-only next iteration (g_vram_only_gprs_dirty), so this
         * full snapshot is redundant for it.  Skip it -- the symmetry holds
         * (minimal fetch <-> GPRS-only push) and nothing in this iteration
         * reads the un-synced state: the loop-top interrupt path injects via
         * WHvRegisterPendingInterruption (hardware uses the VCPU's own SS:SP)
         * and only reads RFLAGS, which pull_vram refreshes.  The ONE consumer
         * that could read stale SS:SP is the YODA single-step predictor
         * (hyperv_step_bp_for_next), so require tracing OFF.  This is what
         * makes the trapped path's per-exit marshalling near-zero. */
        if (!(exit.ExitReason == WHvRunVpExitReasonMemoryAccess &&
              exit.MemoryAccess.Gpa >= gvi_pc_low_regen &&
              exit.MemoryAccess.Gpa <= gvi_pc_high_regen &&
              !(GLOBAL_nanoDebugControl & HV_NANO_ENABLED)))
            hyperv_fetch(REG_GROUP_ALL);

        /* WHP TF-stuck recovery: if we cleared TF to unstick a livelocked
         * #DB intercept, re-arm TF now -- as long as this exit is NOT
         * another #DB.  A #DB exit means we're either tracing normally
         * (good, TF is doing its job) or still stuck (don't re-arm yet,
         * the stuck detector will fire again).  Any other exit reason
         * (Halt, InterruptWindow, IO, MMIO, Exception-not-DB, ...) is a
         * natural boundary where it's safe to resume tracing. */
        if (g_retf_on_next_exit) {
            int still_db = 0;
            if (exit.ExitReason == WHvRunVpExitReasonException) {
                BYTE et = exit.VpException.ExceptionType;
                if (et == VECTOR_DB || et == VECTOR_BP) still_db = 1;
            }
            if (!still_db) {
                state._rflags |= FLG_TRAP;
                MARK(REG_GROUP_GPRS);
                g_retf_on_next_exit = 0;
                OutputDebugStringA("YODA: re-armed TF after natural exit\n");
            }
        }

        switch (exit.ExitReason)
        {
        case WHvRunVpExitReasonNone:
            /* No actionable exit (e.g. signalled but no work). */
            break;

        case WHvRunVpExitReasonCanceled:
            tunnel->_exit_status = HAX_EXIT_PAUSED;
            break;

        case WHvRunVpExitReasonX64InterruptWindow:
            /* The interrupt window opened (guest IF=1, no shadow).  State was
             * already fetched above, so DispatchInterrupts sees the fresh IF=1
             * and injects the pending IRQ.  WHP consumes (clears) the
             * deliverability notification on this exit -- reflect that so the
             * loop re-arms if something is still pending. */
            g_intwin_armed = 0;
            tunnel->_exit_status = HAX_EXIT_INTERRUPT;
            if (*pNtVDMState & VDM_INTERRUPT_PENDING)
                DispatchInterrupts();
            break;

        case WHvRunVpExitReasonX64Halt:
            /* For diagnostic / sleep, we want the RIP and RFLAGS to be
             * coherent on the next entry; the handler itself does not
             * read state, so just keep GPRs marked clean (they came from
             * the last flush and HLT did not modify GPRs). */
            tunnel->_exit_status = HAX_EXIT_HLT;
            if (*pNtVDMState & VDM_INTERRUPT_PENDING)
                DispatchInterrupts();
            else
                ContinueExecution = FALSE;
            break;

        case WHvRunVpExitReasonX64MsrAccess:
        {
            /* RDMSR / WRMSR trap (see the X64MsrExit enable in hypervvm_init).
             *
             * We do not model any MSRs: WRMSR is swallowed, RDMSR returns 0.
             * That is deliberately non-architectural (a real CPU would #GP at
             * CPL 3) -- but chkcpu issues the RDMSR expecting it to succeed,
             * and the alternative (inject #GP) is exactly the live-lock this
             * is meant to cure.  Returning 0 and stepping over the
             * instruction lets the probe complete and the guest move on. */
            const WHV_X64_MSR_ACCESS_CONTEXT *msr = &exit.MsrAccess;
            UINT32 ilen = exit.VpContext.InstructionLength;
            hyperv_fetch(REG_GROUP_GPRS);
            if (!msr->AccessInfo.IsWrite) {     /* RDMSR -> EDX:EAX = 0 */
                state._eax = 0;
                state._edx = 0;
            }
            /* WRMSR: ignore the written value entirely. */
            state._rip += ilen ? ilen : 2;       /* 0F 32 / 0F 30 are 2 bytes */
            MARK(REG_GROUP_GPRS);

            /* Like the IO path, an emulated MSR exit never went through the
             * #DB single-step path, so nudge YODA's per-instruction trace
             * machinery directly -- otherwise single-step would stall on the
             * RDMSR/WRMSR with step_count pinned. */
            if (GLOBAL_nanoDebugControl & HV_NANO_ENABLED)
                check_I();
            break;
        }

        case WHvRunVpExitReasonX64IoPortAccess:
        {
            /* I/O exits: the handler reads RIP (for advance) and DS:SI /
             * ES:DI / RFLAGS.DF for string I/O.  Fetch GPRs and SEGS. */
            const WHV_X64_IO_PORT_ACCESS_CONTEXT *io = &exit.IoPortAccess;
            hyperv_fetch(REG_GROUP_GPRS | REG_GROUP_SEGS);

            tunnel->_exit_status   = HAX_EXIT_IO;
            tunnel->io._port       = io->PortNumber;
            tunnel->io._size       = (uint16_t)io->AccessInfo.AccessSize;
            tunnel->io._direction  = io->AccessInfo.IsWrite ? HAX_EXIT_IO_OUT
                                                            : HAX_EXIT_IO_IN;

            VdmTib.EventInfo.IoInfo.PortNumber = io->PortNumber;
            VdmTib.EventInfo.IoInfo.Size       = tunnel->io._size;
            VdmTib.EventInfo.IoInfo.Read       = !io->AccessInfo.IsWrite;

            if (io->AccessInfo.StringOp) {
                /* REP INS/OUTS: emulate the whole string in process (loops
                 * RCX, moves guest mem <-> port).  Fixes the scrambled VGA
                 * palette from REP OUTS to the DAC (3C9). */
                hyperv_string_io(io);
            } else {
                if (io->AccessInfo.IsWrite)
                    memcpy(iobuf, &io->Rax, tunnel->io._size);

                EventVdmIo();

                if (!io->AccessInfo.IsWrite) {
                    UINT64 rax = 0;
                    memcpy(&rax, iobuf, tunnel->io._size);
                    state._rax = (state._rax & ~((UINT64)((1ULL << (tunnel->io._size * 8)) - 1)))
                               | rax;
                }
            }

            /* Advance RIP past the emulated IO instruction.  WHP fills
             * io->InstructionByteCount correctly for OUT exits, but
             * returns 0 for IN exits on this build (confirmed by
             * observing it always-0 for IN AL, 60h from the keyboard
             * ISR while OUT 20h reports ibc=2 correctly).
             *
             * Fallbacks, in order of trust:
             *   1. io->InstructionByteCount   (correct for OUT)
             *   2. exit.VpContext.InstructionLength (WHP's general field;
             *      reliable enough that the MSR path trusts it)
             *   3. io_insn_len() -- a correct IN/OUT decode at the FULL
             *      linear RIP.
             *
             * NB: do NOT use predict_next_lin() here.  It truncates EIP to
             * 16 bits, so for a 32-bit flat segment (DOOM at CS=02FF base=0,
             * EIP=0x011338be) it decoded a garbage byte at the aliased low
             * address 0x38be and returned the wrong length -- RIP then
             * over-advanced one byte into 011338c0 (a mid-instruction
             * `mov eax,[0xA2000000]`) -> the intermittent WHP-only #GP
             * "derail" we hunted for ages. */
            {
                int adv = (int)io->InstructionByteCount;
                if (adv <= 0 || adv > 15)
                    adv = (int)exit.VpContext.InstructionLength;
                if (adv <= 0 || adv > 15)
                    adv = io_insn_len();
                if (adv <= 0 || adv > 15)
                    adv = 1;
                state._rip += adv;
            }
            MARK(REG_GROUP_GPRS);

            /* If YODA is single-stepping, notify the per-instruction
             * trace machinery here.  IO exits don't go through the #DB
             * path (the OUT/IN didn't execute on the CPU -- we emulated
             * it), so check_I() would never see this step otherwise and
             * step_count would stay pinned at 1.  That's what was
             * causing 's' on an OUT 20h (EOI in IRQ handlers) to hang
             * with no return to the prompt. */
            if (GLOBAL_nanoDebugControl & HV_NANO_ENABLED)
                check_I();
            break;
        }

        case WHvRunVpExitReasonMemoryAccess:
        {
            /* MMIO: the VRAM decoder reads CS:IP, GPRs, RFLAGS.DF and the
             * segment bases.  PERF-VRAM showed this fetch is ~90% of per-exit
             * cost and that the cost scales with REGISTER COUNT (REG_GROUP_ALL
             * was ~2x slower than GPRS|SEGS), so for the video path we fetch
             * only the 13 regs the decoder needs, in one hypercall (pull_vram).
             * Non-video MMIO (page faults) keeps the full GPRS|SEGS fetch. */
            int is_video = (exit.MemoryAccess.Gpa >= gvi_pc_low_regen &&
                            exit.MemoryAccess.Gpa <= gvi_pc_high_regen);
            /* The standard video aperture (A0000-BFFFF) must ALWAYS go through
             * the in-process decoder, even when the current mode's gvi range is
             * narrower (e.g. a graphics mode leaves gvi=A0000..AFFFF yet spdos
             * still pokes the B8000 text page to probe for a card).  The legacy
             * handle_memory_access mis-handles these: ma->InstructionByteCount
             * is the 16-byte instruction WINDOW, not the length, so it over-
             * advances RIP and derails (project_wpsnt_whp_derail).  The emulate
             * path decodes the real length + operand size. */
            int in_aperture = (exit.MemoryAccess.Gpa >= 0xA0000u &&
                               exit.MemoryAccess.Gpa <= 0xBFFFFu);

            if (is_video) pull_vram();   /* minimal 13-reg single hypercall */
            else          hyperv_fetch(REG_GROUP_GPRS | REG_GROUP_SEGS);
            tunnel->_exit_status = HAX_EXIT_FAST_MMIO;
            /* The A0000 EGA aperture (write modes / latches / ALU) needs a
             * full instruction decode.  Route those faults through the
             * in-process decoder (hyperv_emulate_vram_access -> decode() ->
             * sas_PR/PW -> CVIDC read_b/write_b).  On success the shadow GPRs
             * and RIP are already advanced.  Anything it can't decode (or a
             * non-video GPA) falls back to the legacy reconstruction. */
            if (is_video || in_aperture) {
                if (hyperv_emulate_vram_access(&exit.MemoryAccess)) {
                    /* Handler touched only GPRs (it MARKed GPRS) -> let the
                     * next loop iteration push just the dirty groups instead
                     * of the full state. */
                    g_vram_only_gprs_dirty = 1;
                    break;
                }
            }
            /* Demand-map extended guest RAM the XMS/DPMI manager connected only
             * partially.  The manager connects extended memory page-by-page as the
             * app allocates it, but a PM extender (wps) touches pages it never
             * formally connected -- on WHP those fault here (CCPU reads them
             * directly and never faults).  Map the SINGLE faulting page (a per-page
             * map is compatible with the manager's later unmap+reconnect of that
             * page; a big bulk map is not -- WHP drops the whole containing mapping
             * on any sub-range unmap) and RE-RUN via `continue`.  Using `continue`
             * (not break) is essential: break returns to NTVDM with the stale
             * HAX_EXIT_FAST_MMIO set above and it runs a bogus MMIO instead of
             * retrying (that was the earlier 16x fault loop).  A small same-page
             * retry cap stops a genuinely unbacked address looping forever.
             * project_wpsnt_whp_derail UPDATE62. */
            {
                uint64_t mgpa = (uint64_t)exit.MemoryAccess.Gpa;
                if (mgpa >= 0x100000u && mgpa < (uint64_t)VdmSize) {
                    uint64_t pg = mgpa & ~0xFFFULL;
                    /* Best-effort: map the page so FUTURE accesses run native.
                     * On some WHP states this map (hr=S_OK) does not actually
                     * make the guest touch stop faulting -- so we do NOT rely on
                     * a retry.  Instead EMULATE this access in-process exactly
                     * like the A0000 aperture path: hyperv_emulate_vram_access
                     * decodes the instruction and does the read/write via
                     * sas_PR/PW (= Start_of_M_area+gpa) and advances RIP, so the
                     * guest always proceeds correctly whether or not the map
                     * sticks.  CCPU behaves this way for all of M.  UPDATE64. */
                    hyperv_unmap_gpa_range(pg, 0x1000);
                    (void)hyperv_map_gpa_range(pg, 0x1000,
                            (void *)(Start_of_M_area + pg),
                            WHvMapGpaRangeFlagRead | WHvMapGpaRangeFlagWrite |
                            WHvMapGpaRangeFlagExecute);
                    if (hyperv_emulate_vram_access(&exit.MemoryAccess)) {
                        /* MARK(GPRS) already done inside; normal push handles it. */
                        break;   /* access done in-process, RIP advanced */
                    }
                }
            }
            handle_memory_access(&exit.MemoryAccess);
            /* handle_memory_access already MARK()s GPRs. */
            break;
        }

        case WHvRunVpExitReasonException:
        {
            /* Exception (BOP / interrupt-hook path).  The host_*_hook
             * callees may walk segment descriptors via getES/getDS/...,
             * so fetch GPRs + SEGS. */
            const WHV_VP_EXCEPTION_CONTEXT *ex = &exit.VpException;
            int ret = 0;
            hyperv_fetch(REG_GROUP_GPRS | REG_GROUP_SEGS);

            /* bex/vex: a PM #GP we trapped because break-on-fault/verbose-
             * exceptions is on (hyperv_sync_pmfault_trap put vector 13 in the
             * bitmap).  Log it (vex) and/or drop to YODA (bex), then fall
             * through to the normal reflect/inject below so the guest still
             * sees the fault. */
            if (s_pmfault_trap_armed && getPE() && ex->ExceptionType == VECTOR_GP) {
                /* Dedup a repeating fault: log/break only the first time we see
                 * each distinct faulting CS:EIP, so a fault storm doesn't flood
                 * (and bex breaks once, not in a loop). */
                static DWORD s_last_gp_cs  = 0xFFFFFFFFu;
                static DWORD s_last_gp_rip = 0xFFFFFFFFu;
                DWORD cs  = (DWORD)state._cs.selector;
                DWORD rip = (DWORD)state._eip;
                if (cs != s_last_gp_cs || rip != s_last_gp_rip) {
                    char dbg[160];
                    s_last_gp_cs  = cs;
                    s_last_gp_rip = rip;
                    sprintf(dbg, "PM-Exception: vec=%u err=%04X at %04X:%08X (eax=%08X)\n",
                            (unsigned)ex->ExceptionType, (unsigned)ex->ErrorCode,
                            (unsigned)cs, (unsigned)rip, (unsigned)state._eax);
                    OutputDebugStringA(dbg);
                    /* For a segment-load #GP the error code is the offending
                     * selector; for MOV ES,AX the operand is AX -- dump both. */
                    hyperv_dump_sel_descr("errsel", (WORD)(ex->ErrorCode & 0xFFFC));
                    hyperv_dump_sel_descr("eax   ", (WORD)(state._eax & 0xFFFF));
                    if (trap_exceptions) force_yoda();
                }
            }

            /* --- YODA debug-trap path (mirrors HAXM's HAX_EXIT_DEBUG).
             *
             * #DB (TF single-step / data BPX / DR0..3) and #BP (guest
             * INT3) are consumed here: we call check_I() so YODA's
             * per-instruction machinery (vader/step_count, instruction
             * BPs, INT-call trace, etc.) can decide whether to drop to
             * the prompt, then we return without re-injecting.
             * Re-injecting would deliver #DB to the guest IDT; the
             * handler's IRET would pop EFLAGS with TF=0, silently
             * disabling single-step. */
            if (ex->ExceptionType == VECTOR_DB
                || ex->ExceptionType == VECTOR_BP)
            {
                LIN_ADDR descr_addr;
                DWORD next4;
                int dr_idt_fired = 0;

                /* DR1/DR2 step-BP fire is now the primary trapping
                 * mechanism (see hyperv_step_bp_for_next at top of
                 * cpu_simulate).  TF #DB is a backup, since WHP's TF
                 * intercept latches unreliably (project_hyperv_tf_intercept_bug).
                 *
                 * The legacy stuck detector below stayed in for cases
                 * where BOTH TF and DR step-BP somehow fail to advance
                 * RIP (e.g., guest jumped to an unpredicted address via
                 * an indirect JMP we couldn't decode).  After 2 consecutive
                 * same-RIP #DB exits, clear TF and let the CPU run until
                 * a natural exit; that exit's handler will re-arm step BPs
                 * fresh, so we resume tracing from there. */
                {
                    static DWORD s_prev_db_rip   = 0xFFFFFFFFu;
                    static int   s_db_stuck_run  = 0;
                    DWORD this_rip = (DWORD)state._eip;

                    if (this_rip == s_prev_db_rip) s_db_stuck_run++;
                    else                           s_db_stuck_run = 0;
                    s_prev_db_rip = this_rip;

                    if (s_db_stuck_run >= 2 && (state._rflags & FLG_TRAP)) {
                        char dbg[120];
                        sprintf(dbg,
                            "YODA: stuck @%04X:%08X (DR-step missed) -- "
                            "TF off until next natural exit\n",
                            (unsigned)state._cs.selector,
                            (unsigned)this_rip);
                        OutputDebugStringA(dbg);

                        state._rflags &= ~FLG_TRAP;
                        MARK(REG_GROUP_GPRS);
                        /* Also disarm any stale step BPs in case they
                         * are what's getting us stuck. */
                        hyperv_disarm_step_bp();
                        g_retf_on_next_exit = 1;
                        s_db_stuck_run      = 0;
                    }
                }

                /* Was this DR0 firing at an IDT handler entry we armed in
                 * hyperv_inject_interrupt?  DR6.B0..B3 distinguish a
                 * code-bp trap from a TF (BS, bit 14) trap.  If yes:
                 * disarm DR0, restore guest DR7, mark "in handler", and
                 * re-set RFLAGS.TF (the CPU cleared it during IRQ
                 * delivery -- without this the handler would run silent
                 * again). */
                if (g_db_armed_vec >= 0) {
                    if (state._dr6 & 0x1u) {   /* B0 = our IDT-trap DR0 (not a DR3 data BP) */
                        OutputDebugStringA("YODA: DR0 fire -> handler entry\n");
                        g_in_irq_handler = 1;
                        hyperv_disarm_idt_trap();
                        state._rflags |= FLG_TRAP;
                        MARK(REG_GROUP_GPRS);
                        dr_idt_fired = 1;
                    } else if ((DWORD)state._rip == g_irq_return_rip) {
                        /* Fallback path: DR0 didn't fire (maybe WHP
                         * isn't preserving DR state across the IRQ-
                         * delivery atomic, or the IVT entry didn't point
                         * where we computed), but we *are* trapping back
                         * at the pre-injection RIP -- the IRET-with-TF
                         * brought us here.  Clean up the arming state and
                         * skip the next IRQ dispatch so the main-code
                         * instruction can finally execute. */
                        OutputDebugStringA("YODA: DR0 missed -- post-IRET fallback\n");
                        hyperv_disarm_idt_trap();
                        g_skip_next_dispatch = 1;
                    }
                }

                /* TF trap whose RIP matches the address we saved when we
                 * armed DR0 means the handler's IRET just landed back at
                 * the original interrupted instruction (IRET-with-TF
                 * raises #DB on the next-instr boundary, SDM 17.3.1.1).
                 * Clear the in-handler flag and skip the next iteration's
                 * IRQ dispatch so the main-code instruction can actually
                 * execute before another timer IRQ is queued. */
                if (!dr_idt_fired && g_in_irq_handler
                    && (DWORD)state._rip == g_irq_return_rip) {
                    char dbg[96];
                    sprintf(dbg,
                            "YODA: leave INT handler, return @ %04X:%08X\n",
                            (unsigned)state._cs.selector,
                            (unsigned)state._eip);
                    OutputDebugStringA(dbg);
                    g_in_irq_handler     = 0;
                    g_skip_next_dispatch = 1;
                }

                /* Mid-mode-switch: CS may not yet describe a valid
                 * table entry (DOSX flip).  Skip to avoid faulting
                 * inside YODA's disassembler. */
                if (ISPESET && selector_outside_table(state._cs.selector,
                                                      &descr_addr))
                    break;

                /* check_D: a YODA data breakpoint (blwd/blrd/blww/...) fired.
                 * DR6.B0..B3 is set, it matches an active data-BP slot, and
                 * it is not our IDT-trap (dr_idt_fired) -- the step BPs in
                 * DR1/DR2 and the IDT-trap in DR0 are steered clear of the
                 * data-BP slot (yoda allocates data BPs from DR3 down).
                 * Report it via YODA (prints the triggered watchpoint, clears
                 * DR6, drops to the prompt) instead of treating it as a
                 * single-step; then we're done with this exit. */
                if (!dr_idt_fired && (state._dr6 & 0xfu)
                    && yoda_data_break_pending((unsigned long)state._dr6)) {
                    data_debug_exception();
                    break;
                }

                check_I();

                /* Inline BOP fast path: if the next opcode is C4 C4 xx,
                 * execute it so the trace logger sees it, then re-check.
                 * Same shape as HAXM's HAX_EXIT_DEBUG.  Skip when DR0
                 * just fired -- we're at handler entry, the bytes there
                 * are kernel code, not a BOP. */
                if (!dr_idt_fired) {
                    next4 = sas_dw_at(effective_addr(getCS(), getEIP()));
                    if (next4 && (WORD)next4 == 0xC4C4) {
                        PBYTE saved = iobuf;
                        state._rip += 3;
                        MARK(REG_GROUP_GPRS);
                        iobuf = (PBYTE)&next4;
                        EventVdmBop();
                        iobuf = saved;
                        check_I();
                    }
                }
                /* No injection: the trap was for YODA, not the guest. */
                break;
            }

            tunnel->_exit_status                          = HAX_EXIT_NMI;
            tunnel->nmi.exit_intr_info.raw                = 0;
            tunnel->nmi.exit_intr_info.vector             = ex->ExceptionType;
            tunnel->nmi.exit_intr_info.valid              = 1;
            tunnel->nmi.exit_intr_info.deliver_error_code = ex->ExceptionInfo.ErrorCodeValid;
            tunnel->nmi.exit_exception_error_code         = ex->ErrorCode;
            tunnel->nmi.exit_intr_info.type               = EXCEPTION;
            if (ex->InstructionByteCount > 0) {
                size_t n = ex->InstructionByteCount < sizeof(s_iobuf) ?
                           ex->InstructionByteCount : sizeof(s_iobuf);
                memcpy(iobuf, ex->InstructionBytes, n);
            }

            /* --- BOP fast path. --- */
            if (tunnel->nmi.exit_intr_info.vector == VECTOR_UD
                && iobuf[0] == 0xC4 && iobuf[1] == 0xC4)
            {
                state._rip += 3;
                MARK(REG_GROUP_GPRS);
                EventVdmBop();
                break;
            }

#ifdef HOOK_INTS
            else if (tunnel->nmi.exit_intr_info.vector == VECTOR_NP
                     && getPE() && iobuf[0] >= 0xCC && iobuf[0] <= 0xCE)
            {
                state._rip += (iobuf[0] == 0xCC || iobuf[0] == 0xCE) ? 1 : 2;
                MARK(REG_GROUP_GPRS);
                if (host_swint_hook(iobuf[0] == 0xCC ? 3 :
                                    (iobuf[0] == 0xCE ? 4 : iobuf[1])))
                    break;
            }
#endif

            /* Protected-mode interrupt-hook dispatch. */
            if (getPE()) {
                switch (tunnel->nmi.exit_intr_info.type) {
                case INTERRUPT:
                    ret = host_hwint_hook(tunnel->nmi.exit_intr_info.vector);
                    break;
                case SWINT:
                    ret = host_swint_hook(tunnel->nmi.exit_intr_info.vector);
                    break;
                case EXCEPTION:
                    ret = host_exint_hook(tunnel->nmi.exit_intr_info.vector,
                                          tunnel->nmi.exit_exception_error_code);
                    break;
                }
            }
            /* If the hook handled it, any state mutation already went
             * through MARK()'d setters.  If not, re-inject into the guest.
             *
             * CPU exceptions (vectors 0..31) must be injected with
             * WHvX64PendingException semantics so WHP delivers them as
             * faults (saved RIP = faulting RIP, guest handler chooses
             * how to advance).  Using PendingInterrupt for exceptions
             * misdispatches and causes infinite re-fault loops (seen with
             * chkcpu's invalid-opcode CPU detection).  Vectors >=32 are
             * external hardware IRQs and still go through the regular
             * PendingInterrupt path. */
            if (!ret) {
                uint32_t v = tunnel->nmi.exit_intr_info.vector;
                if (v < 32) {
                    hyperv_inject_exception(
                        v,
                        ex->ExceptionInfo.ErrorCodeValid ? TRUE : FALSE,
                        ex->ErrorCode);
                } else {
                    hyperv_inject_interrupt(v);
                }
            }
            break;
        }

        case WHvRunVpExitReasonUnsupportedFeature:
        {
            /* Task switch / other unsupported features need the full
             * state for emulation or diagnostics. */
            const WHV_X64_UNSUPPORTED_FEATURE_CONTEXT *uf =
                &exit.UnsupportedFeature;
            hyperv_fetch(REG_GROUP_ALL);

            if (uf->FeatureCode == WHvUnsupportedFeatureTaskSwitchTss) {
                handle_unsupported_task_switch(&exit);
            } else {
                /* Older WHP SDKs (and some leaked builds) only expose
                 * FeatureCode in WHV_X64_UNSUPPORTED_FEATURE_CONTEXT --
                 * the FeatureSpecificInfo field was added later.  The
                 * code is plenty for diagnostics on its own, so we no
                 * longer print the extra info. */
                haxmvm_panic(
                    "WHP UnsupportedFeature %u at CS:RIP %04X:%08llX",
                    (unsigned)uf->FeatureCode,
                    (unsigned)state._cs.selector,
                    (unsigned long long)state._rip);
            }
            break;
        }

        case WHvRunVpExitReasonUnrecoverableException:
            /* Full snapshot for the diagnostic dump. */
            hyperv_fetch(REG_GROUP_ALL);
            tunnel->_exit_status = HAX_EXIT_STATECHANGE;
#ifdef YODA
            printf("hypervisor unrecoverable exception @ CS:IP %04X:%04X\n",
                   getCS(), getIP());
            force_yoda();
#else
            haxmvm_panic(
                "Unrecoverable exception   CS:IP %04X:%04X SP:%04X BP:%08X CR0:%04X",
                getCS(), getIP(), getSP(), getBP(), getCR0());
#endif
            break;

        default:
            /* Unknown exit: fetch everything before panicking so the
             * diagnostic message has accurate CS:RIP. */
            hyperv_fetch(REG_GROUP_ALL);
            haxmvm_panic(
                "Unhandled WHvRunVpExitReason %u at CS:RIP %04X:%08llX",
                (unsigned)exit.ExitReason,
                (unsigned)state._cs.selector,
                (unsigned long long)state._rip);
            break;
        }
    }

    ContinueExecution = TRUE;
    VdmTib.EventInfo      = OldEventInfo;
    VdmTib.MonitorContext = OldMonitorContext;
}

VOID
host_unsimulate(
    )
{
    ContinueExecution = FALSE;
    /* Wake the running VCPU so the exit loop can pick up the change. */
    if (g_Partition)
        WHvCancelRunVirtualProcessor(g_Partition, g_VpIndex, 0);
}

/* ---- Below this point: hypervisor-agnostic NTVDM glue, unchanged from
 *      the HAXM port.  These functions operate on `state` only. ---- */

VOID
EventVdmIo(VOID)
{
    if (tunnel->io._size == 1) {
        if (tunnel->io._direction == HAX_EXIT_IO_IN)
            inb(tunnel->io._port, (half_word *)iobuf);
        else
            outb(tunnel->io._port, *((half_word *)iobuf));
    } else if (tunnel->io._size == 2) {
        if (tunnel->io._direction == HAX_EXIT_IO_IN)
            inw(tunnel->io._port, (word *)iobuf);
        else
            outw(tunnel->io._port, *((word *)iobuf));
    }
#if DBG
    else {
        DbgPrint("NtVdm: Unimplemented IO size %d\n", tunnel->io._size);
        DbgBreakPoint();
    }
#endif
}

VOID
EventVdmBop(VOID)
{
    if (iobuf[2] > MAX_BOP) {
#if DBG
        DbgPrint("NtVdm: Invalid BOP %lx\n", iobuf[2]);
#endif
        ContinueExecution = FALSE;
    } else {
#if DBG
        if (fShowBop) {
            DbgPrint("Ntvdm cpu_simulate : bop dispatch %x,%x\n",
                iobuf[2],
                (ULONG)(*((UCHAR *)Sim32GetVDMPointer(
                    (state._cs.selector << 16) | state._eip, 1, ISPESET))));
        }
        {
            char szdbg[256];
            sprintf(szdbg, "BOP %02X,%02X @%X", iobuf[2],
                (ULONG)(*((UCHAR *)Sim32GetVDMPointer(
                              (state._cs.selector << 16) | state._eip,
                              1, ISPESET))),
                Sim32GetVDMPointer(
                    (state._cs.selector << 16) | state._eip, 1, ISPESET));
            OutputDebugString(szdbg);
        }
#endif
#ifdef YODA
        if (iobuf[2] == BOP_DBGBREAKPOINT) force_yoda();
#endif
        (*BIOS[iobuf[2]])();
    }
}

VOID
EventVdmError(VOID)
{
    TerminateVDM();
    ContinueExecution = FALSE;
}

VOID
EventVdmIrq13(VOID)
{
    if (!IRQ13BeingHandled) {
        IRQ13BeingHandled = TRUE;
        ica_hw_interrupt(ICA_SLAVE, 5, 1);
    }
}

VOID
EventVdmMemAccess(VOID)
{
    DispatchPageFault(
        (ULONG)tunnel->pagefault.gpa,
        (tunnel->pagefault.flags & HAX_PAGEFAULT_ACC_W ? 1 : 0));
}

/* TRUE when the guest is in protected mode (not V86).  The trapped string-op
 * operand addresses must then be resolved via the segment descriptor BASE, not
 * the real-mode seg<<4. */
static int fastmmio_pm(void)
{
    return (state._cr0 & 1u) && !(state._eflags & 0x20000u);
}

/* Resolve the non-GPA operand of a trapped string op to a linear address.
 * PM: descriptor base + full 32-bit offset (wps/DPMI).  Real/V86: seg<<4+off.
 * The legacy path used RMSEGOFFTOLIN unconditionally, which for a protected-mode
 * guest with a large 32-bit EDI/ESI produced a wild pointer -> memcpy crash in
 * sas_PRWS.  project_wpsnt_whp_derail UPDATE61. */
static DWORD fastmmio_lin(DWORD seg_base, DWORD sel, DWORD off)
{
    return fastmmio_pm() ? (seg_base + off)
                         : (DWORD)RMSEGOFFTOLIN(sel, off & 0xFFFFu);
}

/* Bounds-checked wrapper around sas_PRWS: if either end or the length runs past
 * guest RAM (VdmSize), DROP the copy (control flow still advances) rather than
 * memcpy'ing through a wild pointer and taking down the host.  UPDATE61. */
static void sas_PRWS_safe(DWORD src, DWORD dst, DWORD len)
{
    if (len == 0) return;
    if ((uint64_t)src + len > (uint64_t)VdmSize ||
        (uint64_t)dst + len > (uint64_t)VdmSize) {
        return;
    }
    sas_PRWS(src, dst, len);
}

void hax_handle_fastmmio(struct hax_fastmmio *hft)
{
    UCHAR *pCmd = ((UCHAR *)Sim32GetVDMPointer(
                   (state._cs.selector << 16) | state._eip,
                   1, ISPESET));
    char szdbg[128];

    sprintf(szdbg, "hax_handle_fastmmio(dir=%d, size=%d, gpa=%08X\n",
            hft->direction, hft->size, (DWORD)hft->gpa);
    OutputDebugString(szdbg);

    if (*pCmd == 0xF3)
    {
        BOOL bHandled = FALSE;

        switch (hft->direction)
        {
        case 0:
            switch (pCmd[1])
            {
            case 0xA4:
                if (getDF()) break;
                sas_PRWS_safe((DWORD)hft->gpa,
                              fastmmio_lin((DWORD)state._es.base, getES(), getEDI()),
                              getECX());
                setEDI(getEDI() + getECX());
                setESI(getESI() + getECX());
                setECX(0);
                bHandled = TRUE;
                break;
            case 0xA5:
                if (getDF()) break;
                sas_PRWS_safe((DWORD)hft->gpa,
                              fastmmio_lin((DWORD)state._es.base, getES(), getEDI()),
                              getECX() * hft->size);
                setEDI(getEDI() + getECX() * hft->size);
                setESI(getESI() + getECX() * hft->size);
                setECX(0);
                bHandled = TRUE;
                break;
            }
            break;
        case 1:
            switch (pCmd[1])
            {
            case 0xA4:
            case 0xA5:
                if (getDF()) break;
                sas_PRWS_safe(fastmmio_lin((DWORD)state._ds.base, getDS(), getESI()),
                              (DWORD)hft->gpa, getECX() * hft->size);
                setEDI(getEDI() + getECX() * hft->size);
                setESI(getESI() + getECX() * hft->size);
                setECX(0);
                bHandled = TRUE;
                break;
            case 0xAA:
                sas_fills((DWORD)hft->gpa, getAL(), getECX());
                setECX(0);
                bHandled = TRUE;
                break;
            case 0xAB:
                switch (hft->size) {
                case 2:
                    sas_fillsw((DWORD)hft->gpa, getAX(), getECX());
                    setECX(0); bHandled = TRUE; break;
                case 4:
                    sas_fillsdw((DWORD)hft->gpa, getEAX(), getECX());
                    setECX(0); bHandled = TRUE; break;
                }
                break;
            }
            break;
        }
        if (bHandled) return;
    }

#if VIDEO_STRATEGY == 2 || VIDEO_STRATEGY == 4
    if (hft->gpa>=gvi_pc_low_regen && hft->gpa<=gvi_pc_high_regen)
        haxm_vram_mmio(hft);
#endif

    hax_handle_fastmmio_op(hft);
}

void hax_handle_fastmmio_op(struct hax_fastmmio *hft)
{
    switch (hft->direction)
    {
    case 0:
        switch (hft->size) {
        case 1: *((PBYTE)&hft->value) = sas_PR8((DWORD)hft->gpa); break;
        case 2: *((PWORD)&hft->value) = sas_PR16((DWORD)hft->gpa); break;
        case 4: *((PDWORD)&hft->value) = sas_PR32((DWORD)hft->gpa); break;
        default: haxmvm_panic("hax_handle_fastmmio_op size %d", hft->size);
        }
        break;
    case 1:
        switch (hft->size) {
        case 1: sas_PW8 ((DWORD)hft->gpa, (BYTE) hft->value); break;
        case 2: sas_PW16((DWORD)hft->gpa, (WORD) hft->value); break;
        case 4: sas_PW32((DWORD)hft->gpa, (DWORD)hft->value); break;
        default: haxmvm_panic("hax_handle_fastmmio_op write size %d", hft->size);
        }
        break;
    case 2:
        sas_PRWS((DWORD)hft->gpa, (DWORD)hft->gpa2, hft->size);
        break;
    }
}

void parse_lc_options(int *pargc, char ***pargv) { (void)pargc; (void)pargv; }

#define NANO_ENABLED 0x20

VOID Nano_slow_yoda(VOID)
{
    /* HAXM's MTF single-step is not exposed by WHP.  Substitute by setting
     * RFLAGS.TF; the resulting #DB will arrive as a WHvRunVpExitReasonException
     * with vector 1, provided we have included #DB in the exception bitmap.
     */
    if ((GLOBAL_nanoDebugControl & NANO_ENABLED) == 0)
    {
        GLOBAL_nanoDebugControl |= NANO_ENABLED;
        /* Switch on #DB (TF single-step / data BPX) AND #BP (guest INT3 ->
         * YODA prompt) for the rest of the session.  Until this point
         * INT3 was routed to the guest IDT so DOS debuggers worked
         * normally; from now on YODA owns both vectors. */
        s_exception_exit_bitmap |=
            ((UINT64)1 << WHvX64ExceptionTypeDebugTrapOrFault) |
            ((UINT64)1 << WHvX64ExceptionTypeBreakpointTrap);
        hyperv_set_exception_bitmap(s_exception_exit_bitmap);
        state._rflags |= FLG_TRAP;
        MARK(REG_GROUP_GPRS);
        hyperv_flush();
        printf("Nano yoda enabled (DR1/DR2 software-step + RFLAGS.TF backup, "
               "#DB/#BP intercept)\n");
    }
}

void load_seg(segment_desc_t *segment, WORD sel)
{
    LIN_ADDR descr_addr;
    DESCR entry;

    segment->selector = sel;
    if (!getPE() || getVM() || selector_outside_table(sel, &descr_addr) == 1)
    {
        segment->base  = sel << 4;
        segment->limit = 0xFFFF;
    }
    else
    {
        read_descriptor(descr_addr, &entry);
        segment->base  = entry.base;
        segment->limit = entry.limit;
        segment->ar    = sas_dw_at(descr_addr + 4);
        segment->ar  >>= 8;
        segment->ar  &= 0xF0FF;
        segment->ar  |= 1;
    }
}

/* --------------------------------------------------------------------- */
/*  Register get/set helpers.  Identical to the HAXM port -- they only
 *  touch the `state` mirror, so the rest of NTVDM is portable as-is.
 * --------------------------------------------------------------------- */

ULONG  getEAX(VOID) { return (state._eax); }
USHORT getAX (VOID) { return ((USHORT)(state._eax)); }
UCHAR  getAL (VOID) { return ((BYTE)(state._eax)); }
UCHAR  getAH (VOID) { return ((BYTE)(state._eax >> 8)); }
ULONG  getEBX(VOID) { return (state._ebx); }
USHORT getBX (VOID) { return ((USHORT)(state._ebx)); }
UCHAR  getBL (VOID) { return ((BYTE)(state._ebx)); }
UCHAR  getBH (VOID) { return ((BYTE)(state._ebx >> 8)); }
ULONG  getECX(VOID) { return (state._ecx); }
USHORT getCX (VOID) { return ((USHORT)(state._ecx)); }
UCHAR  getCL (VOID) { return ((BYTE)(state._ecx)); }
UCHAR  getCH (VOID) { return ((BYTE)(state._ecx >> 8)); }
ULONG  getEDX(VOID) { return (state._edx); }
USHORT getDX (VOID) { return ((USHORT)(state._edx)); }
UCHAR  getDL (VOID) { return ((BYTE)(state._edx)); }
UCHAR  getDH (VOID) { return ((BYTE)(state._edx >> 8)); }
ULONG  getESP(VOID) { return (state._esp); }
USHORT getSP (VOID) { return ((USHORT)state._esp); }
ULONG  getEBP(VOID) { return (state._ebp); }
USHORT getBP (VOID) { return ((USHORT)state._ebp); }
ULONG  getESI(VOID) { return (state._esi); }
USHORT getSI (VOID) { return ((USHORT)state._esi); }
ULONG  getEDI(VOID) { return (state._edi); }
USHORT getDI (VOID) { return ((USHORT)state._edi); }
ULONG  getEIP(VOID) { return (state._eip); }
USHORT getIP (VOID) { return ((USHORT)state._eip); }

USHORT getCS(VOID) { return ((USHORT)state._cs.selector); }
USHORT getSS(VOID) { return ((USHORT)state._ss.selector); }
USHORT getDS(VOID) { return ((USHORT)state._ds.selector); }
USHORT getES(VOID) { return ((USHORT)state._es.selector); }
USHORT getFS(VOID) { return ((USHORT)state._fs.selector); }
USHORT getGS(VOID) { return ((USHORT)state._gs.selector); }

ULONG getCF(VOID) { return ((state._eflags & FLG_CARRY)     ? 1 : 0); }
ULONG getPF(VOID) { return ((state._eflags & FLG_PARITY)    ? 1 : 0); }
ULONG getAF(VOID) { return ((state._eflags & FLG_AUXILIARY) ? 1 : 0); }
ULONG getZF(VOID) { return ((state._eflags & FLG_ZERO)      ? 1 : 0); }
ULONG getSF(VOID) { return ((state._eflags & FLG_SIGN)      ? 1 : 0); }
ULONG getTF(VOID) { return ((state._eflags & FLG_TRAP)      ? 1 : 0); }
ULONG getIF(VOID) { return ((state._eflags & FLG_INTERRUPT) ? 1 : 0); }
ULONG getDF(VOID) { return ((state._eflags & FLG_DIRECTION) ? 1 : 0); }
ULONG getOF(VOID) { return ((state._eflags & FLG_OVERFLOW)  ? 1 : 0); }
ULONG getVM(VOID) { return ((state._eflags & 0x20000)       ? 1 : 0); }
ULONG getIOPL(VOID)   { return (state._eflags >> 12) & 3; }
ULONG getNT(VOID)     { return ((state._eflags & 0x4000) ? 1 : 0); }
USHORT getSTATUS(VOID){ return (USHORT)state._eflags; }
ULONG getEFLAGS(VOID) { return (state._eflags); }
USHORT getFLAGS(VOID) { return (USHORT)state._eflags; }

ULONG getCPL(VOID)         { return ((state._cr0 & MSW_PE) ? state._cs.dpl : 0); }
ULONG getGDT_BASE(VOID)    { return ((ULONG)state._gdt.base); }
ULONG getGDT_LIMIT(VOID)   { return (state._gdt.limit); }
ULONG getIDT_BASE(VOID)    { return ((ULONG)state._idt.base); }
ULONG getIDT_LIMIT(VOID)   { return (state._idt.limit); }
ULONG getLDT_BASE(VOID)    { return ((ULONG)state._ldt.base); }
ULONG getLDT_LIMIT(VOID)   { return (state._ldt.limit); }
ULONG getLDT_SELECTOR(VOID){ return (state._ldt.selector); }
ULONG getTR_BASE(VOID)     { return ((ULONG)state._tr.base); }
ULONG getTR_LIMIT(VOID)    { return (state._tr.limit); }
ULONG getTR_SELECTOR(VOID) { return (state._tr.selector); }

ULONG getCS_BASE(VOID)  { return ((ULONG)state._cs.base); }
ULONG getCS_LIMIT(VOID) { return (state._cs.limit); }
ULONG getCS_AR(VOID)    { return (state._cs.ar); }
ULONG getDS_BASE(VOID)  { return ((ULONG)state._ds.base); }
ULONG getDS_LIMIT(VOID) { return (state._ds.limit); }
ULONG getDS_AR(VOID)    { return (state._ds.ar); }
ULONG getES_BASE(VOID)  { return ((ULONG)state._es.base); }
ULONG getES_LIMIT(VOID) { return (state._es.limit); }
ULONG getES_AR(VOID)    { return (state._es.ar); }
ULONG getFS_BASE(VOID)  { return ((ULONG)state._fs.base); }
ULONG getFS_LIMIT(VOID) { return (state._fs.limit); }
ULONG getFS_AR(VOID)    { return (state._fs.ar); }
ULONG getGS_BASE(VOID)  { return ((ULONG)state._gs.base); }
ULONG getGS_LIMIT(VOID) { return (state._gs.limit); }
ULONG getGS_AR(VOID)    { return (state._gs.ar); }
ULONG getSS_BASE(VOID)  { return ((ULONG)state._ss.base); }
ULONG getSS_LIMIT(VOID) { return (state._ss.limit); }
ULONG getSS_AR(VOID)    { return (state._ss.ar); }

USHORT getMSW(VOID) { return ((USHORT)state._cr0); }
ULONG  getCR0(VOID) { return ((LONG)state._cr0); }
ULONG  getPE(VOID)  { return ((state._cr0 & MSW_PE)        ? 1 : 0); }
ULONG  getMP(VOID)  { return ((state._cr0 & 2)             ? 1 : 0); }
ULONG  getEM(VOID)  { return ((state._cr0 & 4)             ? 1 : 0); }
ULONG  getTS(VOID)  { return ((state._cr0 & 8)             ? 1 : 0); }
ULONG  getET(VOID)  { return ((state._cr0 & 0x10)          ? 1 : 0); }
ULONG  getNE(VOID)  { return ((state._cr0 & 0x20)          ? 1 : 0); }
ULONG  getWP(VOID)  { return ((state._cr0 & 0x10000)       ? 1 : 0); }
ULONG  getAM(VOID)  { return ((state._cr0 & 0x40000)       ? 1 : 0); }
ULONG  getNW(VOID)  { return ((state._cr0 & 0x20000000)    ? 1 : 0); }
ULONG  getCD(VOID)  { return ((state._cr0 & 0x40000000)    ? 1 : 0); }
ULONG  getPG(VOID)  { return ((state._cr0 & 0x80000000)    ? 1 : 0); }

ULONG getCR2(VOID) { return ((ULONG)state._cr2); }
ULONG getCR3(VOID) { return ((ULONG)state._cr3); }
ULONG getCR4(VOID) { return ((ULONG)state._cr4); }

ULONG getDREG0(VOID) { return ((ULONG)state._dr0); }
ULONG getDREG1(VOID) { return ((ULONG)state._dr1); }
ULONG getDREG2(VOID) { return ((ULONG)state._dr2); }
ULONG getDREG3(VOID) { return ((ULONG)state._dr3); }
ULONG getDREG6(VOID) { return ((ULONG)state._dr6); }
ULONG getDREG7(VOID) { return ((ULONG)state._dr7); }

VOID setPE(ULONG val) { MARK(REG_GROUP_CONTROL); state._cr0 = state._cr0 & (~MSW_PE) | (val & MSW_PE); }
VOID setEAX(ULONG val) { MARK(REG_GROUP_GPRS); state._eax = val; }
VOID setAX (USHORT val){ MARK(REG_GROUP_GPRS); state._eax = (state._eax & 0xFFFF0000) | ((ULONG)val & 0x0000FFFF); }
VOID setAH (UCHAR val) { MARK(REG_GROUP_GPRS); state._eax = (state._eax & 0xFFFF00FF) | ((ULONG)(val << 8) & 0x0000FF00); }
VOID setAL (UCHAR val) { MARK(REG_GROUP_GPRS); state._eax = (state._eax & 0xFFFFFF00) | ((ULONG)val & 0x000000FF); }
VOID setEBX(ULONG val) { MARK(REG_GROUP_GPRS); state._ebx = val; }
VOID setBX (USHORT val){ MARK(REG_GROUP_GPRS); state._ebx = (state._ebx & 0xFFFF0000) | ((ULONG)val & 0x0000FFFF); }
VOID setBH (UCHAR val) { MARK(REG_GROUP_GPRS); state._ebx = (state._ebx & 0xFFFF00FF) | ((ULONG)(val << 8) & 0x0000FF00); }
VOID setBL (UCHAR val) { MARK(REG_GROUP_GPRS); state._ebx = (state._ebx & 0xFFFFFF00) | ((ULONG)val & 0x000000FF); }
VOID setECX(ULONG val) { MARK(REG_GROUP_GPRS); state._ecx = val; }
VOID setCX (USHORT val){ MARK(REG_GROUP_GPRS); state._ecx = (state._ecx & 0xFFFF0000) | ((ULONG)val & 0x0000FFFF); }
VOID setCH (UCHAR val) { MARK(REG_GROUP_GPRS); state._ecx = (state._ecx & 0xFFFF00FF) | ((ULONG)(val << 8) & 0x0000FF00); }
VOID setCL (UCHAR val) { MARK(REG_GROUP_GPRS); state._ecx = (state._ecx & 0xFFFFFF00) | ((ULONG)val & 0x000000FF); }
VOID setEDX(ULONG val) { MARK(REG_GROUP_GPRS); state._edx = val; }
VOID setDX (USHORT val){ MARK(REG_GROUP_GPRS); state._edx = (state._edx & 0xFFFF0000) | ((ULONG)val & 0x0000FFFF); }
VOID setDH (UCHAR val) { MARK(REG_GROUP_GPRS); state._edx = (state._edx & 0xFFFF00FF) | ((ULONG)(val << 8) & 0x0000FF00); }
VOID setDL (UCHAR val) { MARK(REG_GROUP_GPRS); state._edx = (state._edx & 0xFFFFFF00) | ((ULONG)val & 0x000000FF); }
VOID setESP(ULONG val) { MARK(REG_GROUP_GPRS); state._esp = val; }
VOID setSP (USHORT val){ MARK(REG_GROUP_GPRS); state._esp = (state._esp & 0xFFFF0000) | ((ULONG)val & 0x0000FFFF); }
VOID setEBP(ULONG val) { MARK(REG_GROUP_GPRS); state._ebp = val; }
VOID setBP (USHORT val){ MARK(REG_GROUP_GPRS); state._ebp = (state._ebp & 0xFFFF0000) | ((ULONG)val & 0x0000FFFF); }
VOID setESI(ULONG val) { MARK(REG_GROUP_GPRS); state._esi = val; }
VOID setSI (USHORT val){ MARK(REG_GROUP_GPRS); state._esi = (state._esi & 0xFFFF0000) | ((ULONG)val & 0x0000FFFF); }
VOID setEDI(ULONG val) { MARK(REG_GROUP_GPRS); state._edi = val; }
VOID setDI (USHORT val){ MARK(REG_GROUP_GPRS); state._edi = (state._edi & 0xFFFF0000) | ((ULONG)val & 0x0000FFFF); }
VOID setEIP(ULONG val) { MARK(REG_GROUP_GPRS); state._eip = val; }
VOID setIP (USHORT val){ MARK(REG_GROUP_GPRS); state._eip = (state._eip & 0xFFFF0000) | ((ULONG)val & 0x0000FFFF); }

VOID setCS(USHORT val) {
    MARK(REG_GROUP_SEGS);
    load_seg(&state._cs, val);
    if (!getPE() || getVM()) {
        state._cs.type = 11;
        state._cs.dpl  = getVM() ? 3 : 0;
        if (state._cs.limit > 0xffff) state._cs.limit = 0xffff;
    }
}
VOID setSS(USHORT val) {
    MARK(REG_GROUP_SEGS);
    load_seg(&state._ss, val);
    if (!getPE() || getVM())
        state._ss.dpl = getVM() ? 3 : 0;
}
VOID setDS(USHORT val) { MARK(REG_GROUP_SEGS); load_seg(&state._ds, val); }
VOID setES(USHORT val) { MARK(REG_GROUP_SEGS); load_seg(&state._es, val); }
VOID setFS(USHORT val) { MARK(REG_GROUP_SEGS); load_seg(&state._fs, val); }
VOID setGS(USHORT val) { MARK(REG_GROUP_SEGS); load_seg(&state._gs, val); }

VOID setCF(ULONG val) { MARK(REG_GROUP_GPRS); state._eflags = (state._eflags & ~FLG_CARRY)     | (((ULONG)val << FLG_CARRY_BIT)     & FLG_CARRY); }
VOID setPF(ULONG val) { MARK(REG_GROUP_GPRS); state._eflags = (state._eflags & ~FLG_PARITY)    | (((ULONG)val << FLG_PARITY_BIT)    & FLG_PARITY); }
VOID setAF(ULONG val) { MARK(REG_GROUP_GPRS); state._eflags = (state._eflags & ~FLG_AUXILIARY) | (((ULONG)val << FLG_AUXILIARY_BIT) & FLG_AUXILIARY); }
VOID setZF(ULONG val) { MARK(REG_GROUP_GPRS); state._eflags = (state._eflags & ~FLG_ZERO)      | (((ULONG)val << FLG_ZERO_BIT)      & FLG_ZERO); }
VOID setSF(ULONG val) { MARK(REG_GROUP_GPRS); state._eflags = (state._eflags & ~FLG_SIGN)      | (((ULONG)val << FLG_SIGN_BIT)      & FLG_SIGN); }
VOID setIF(ULONG val) { MARK(REG_GROUP_GPRS); state._eflags = (state._eflags & ~FLG_INTERRUPT) | (((ULONG)val << FLG_INTERRUPT_BIT) & FLG_INTERRUPT); }
VOID setDF(ULONG val) { MARK(REG_GROUP_GPRS); state._eflags = (state._eflags & ~FLG_DIRECTION) | (((ULONG)val << FLG_DIRECTION_BIT) & FLG_DIRECTION); }
VOID setOF(ULONG val) { MARK(REG_GROUP_GPRS); state._eflags = (state._eflags & ~FLG_OVERFLOW)  | (((ULONG)val << FLG_OVERFLOW_BIT)  & FLG_OVERFLOW); }
VOID setNT(ULONG val) { MARK(REG_GROUP_GPRS); state._eflags = (state._eflags & ~0x4000)        | (((ULONG)val << 14) & 0x4000); }

VOID setMSW(USHORT val) { MARK(REG_GROUP_CONTROL); (USHORT)state._cr0 = val; }
VOID setCR0(ULONG val)  { MARK(REG_GROUP_CONTROL); state._cr0 = val; }
VOID setPG (ULONG val)  { MARK(REG_GROUP_CONTROL); state._cr0 = (state._cr0 & ~0x80000000) | ((val & 1) << 31); }
VOID setSTATUS(USHORT val){ MARK(REG_GROUP_GPRS); state._eflags = (state._eflags & 0xFFFF0000) | val; }
VOID setEFLAGS(ULONG val) { MARK(REG_GROUP_GPRS); state._eflags = val; }

VOID setCS_BASE_LIMIT_AR(ULONG base, ULONG limit, USHORT ar) {
    MARK(REG_GROUP_SEGS);
    state._cs.base = base; state._cs.limit = limit; state._cs.ar = ar;
}
VOID setDS_BASE_LIMIT_AR(ULONG base, ULONG limit, USHORT ar) {
    MARK(REG_GROUP_SEGS);
    state._ds.base = base; state._ds.limit = limit; state._ds.ar = ar;
}
VOID setES_BASE_LIMIT_AR(ULONG base, ULONG limit, USHORT ar) {
    MARK(REG_GROUP_SEGS);
    state._es.base = base; state._es.limit = limit; state._es.ar = ar;
}
VOID setFS_BASE_LIMIT_AR(ULONG base, ULONG limit, USHORT ar) {
    MARK(REG_GROUP_SEGS);
    state._fs.base = base; state._fs.limit = limit; state._fs.ar = ar;
}
VOID setGS_BASE_LIMIT_AR(ULONG base, ULONG limit, USHORT ar) {
    MARK(REG_GROUP_SEGS);
    state._gs.base = base; state._gs.limit = limit; state._gs.ar = ar;
}
VOID setSS_BASE_LIMIT_AR(ULONG base, ULONG limit, USHORT ar) {
    MARK(REG_GROUP_SEGS);
    state._ss.base = base; state._ss.limit = limit; state._ss.ar = ar;
}
VOID setIDT_BASE_LIMIT(ULONG base, USHORT limit) {
    MARK(REG_GROUP_TABLES);
    state._idt.base = base; state._idt.limit = limit;
}

VOID setCPL(ULONG val) { (void)val; }

#define DR7_VALID_BITS 0xffff03ff
#define DR6_VALID_BITS 0x0000e00f

VOID moveToDebugRegister(ULONG debugRegister, ULONG src)
{
    /* WHP exposes the debug registers via the normal register interface;
     * there is no separate "VCPU_DEBUG" backdoor, so just update `state`
     * and let the next flush push the DEBUG group.  The NANO/MTF-style
     * single-step is handled via RFLAGS.TF in Nano_slow_yoda(). */
    MARK(REG_GROUP_DEBUG);
    switch (debugRegister) {
    case 0: state._dr0 = src; break;
    case 1: state._dr1 = src; break;
    case 2: state._dr2 = src; break;
    case 3: state._dr3 = src; break;
    case 4: case 5: break;
    case 6: state._dr6 = src & DR6_VALID_BITS; break;
    case 7: state._dr7 = src & DR7_VALID_BITS; break;
    }
}

/* WHP exposes the FPU as a single Xsave-style register; for status / control
 * / tag-word reads we pull WHvX64RegisterFpControlStatus and
 * WHvX64RegisterXmmControlStatus on demand.  NTVDM only uses these for the
 * debugger so we don't bother caching. */
static IU32 read_fpu_status_reg(BOOL want_control)
{
    /* WHP gives us the legacy x87 status block via WHvRegisterFpControlStatus
     * (FCW/FSW/FTW/FOP packed into a WHV_X64_FP_CONTROL_STATUS_REGISTER). */
    WHV_REGISTER_NAME  n  = WHvX64RegisterFpControlStatus;
    WHV_REGISTER_VALUE v  = {0};
    if (FAILED(WHvGetVirtualProcessorRegisters(g_Partition, g_VpIndex, &n, 1, &v)))
        return 0;
    return want_control ? v.FpControlStatus.FpControl
                        : v.FpControlStatus.FpStatus;
}

IU32 a_getNpxStatusReg ()  { return read_fpu_status_reg(FALSE); }
IU32 a_getNpxControlReg () { return read_fpu_status_reg(TRUE); }
IU32 a_getNpxTagwordReg () {
    WHV_REGISTER_NAME  n = WHvX64RegisterFpControlStatus;
    WHV_REGISTER_VALUE v = {0};
    if (FAILED(WHvGetVirtualProcessorRegisters(g_Partition, g_VpIndex, &n, 1, &v)))
        return 0;
    return v.FpControlStatus.FpTag;
}

CHAR *a_getNpxStackReg(IU32 reg_num, CHAR *dumpStore)
{
    (void)reg_num;
    strcpy(dumpStore, "not_impl");
    return dumpStore;
}

/* YODA calls this when a data breakpoint (blwd/blrd/...) is armed or the
 * last one is cleared.  "Snaffle the data debug exception" means: make sure
 * the DR-generated #DB is intercepted by us (so it reaches
 * data_debug_exception() in the VECTOR_DB handler) rather than being
 * delivered to the guest IDT.  We do that by toggling #DB in the partition
 * exception-exit bitmap -- NOT by touching EFLAGS.NT (the previous
 * implementation mis-set the guest Nested-Task flag, which corrupted IRETs). */
VOID setSnaffleDataDebugExcpn(IBOOL dosnaffle)
{
    UINT64 db = ((UINT64)1 << WHvX64ExceptionTypeDebugTrapOrFault);
    if (dosnaffle) {
        s_exception_exit_bitmap |= db;
    } else if (!(GLOBAL_nanoDebugControl & NANO_ENABLED)) {
        /* Only drop the #DB intercept if YODA single-step isn't also using
         * it (Nano_slow_yoda keeps #DB on for the whole session). */
        s_exception_exit_bitmap &= ~db;
    }
    hyperv_set_exception_bitmap(s_exception_exit_bitmap);
}

PX86CONTEXT
getIntelRegistersPointer(VOID)
{
    /* Not on the hot path (WoW context publishing).  Force a refresh of
     * GPRs and segment registers in case the caller fires between exit
     * handlers without them having been fetched. */
    hyperv_fetch(REG_GROUP_GPRS | REG_GROUP_SEGS);

    VdmTib.VdmContext.SegGs = state._gs.selector;
    VdmTib.VdmContext.SegFs = state._fs.selector;
    VdmTib.VdmContext.SegEs = state._es.selector;
    VdmTib.VdmContext.SegDs = state._ds.selector;
    VdmTib.VdmContext.SegCs = state._cs.selector;
    VdmTib.VdmContext.SegSs = state._ss.selector;

    VdmTib.VdmContext.Edi = state._edi;
    VdmTib.VdmContext.Esi = state._esi;
    VdmTib.VdmContext.Ebx = state._ebx;
    VdmTib.VdmContext.Edx = state._edx;
    VdmTib.VdmContext.Ecx = state._ecx;
    VdmTib.VdmContext.Eax = state._eax;

    VdmTib.VdmContext.Ebp    = state._ebp;
    VdmTib.VdmContext.Eip    = state._eip;
    VdmTib.VdmContext.EFlags = state._eflags & ~2;
    VdmTib.VdmContext.Esp    = state._esp;

    return &(VdmTib.VdmContext);
}
