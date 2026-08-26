/*++

Copyright (c) 2019  leecher@dose.0wnz.at

Module Name:

    Monitor.c
Abstract:

    This module is the user mode portion of the haxm monitor

Author:

    leecher@dose.0wnz.at (leecher1337) 10 Feb 2019

Environment:

    User mode only

Revision History:

--*/


#include "monitorp.h"
#include "insignia.h"
#include "host_def.h"
#include "gvi.h"
#include "xt.h"
#include "../../../softpc.new/base/cvidc/gdpvar.h"
#include "sas4gen.h"
#include "yoda.h"
#undef DBG

extern host_addr Start_of_M_area;
extern host_addr Length_of_M_area;
extern const char *name_vmx_exit(int value);
#undef pNtVDMState
#define pNtVDMState   ((ULONG *)(Start_of_M_area + FIXED_NTVDMSTATE_LINEAR))

#define SERVICE_NAME L"IntelHaxmNtvdm"

struct hax_debug_t haxdbg={0};

/*
   Allow print out of exceptions or disallow it.
 */
BOOL show_exceptions = FALSE;
BOOL trap_exceptions = FALSE;

/* Set in haxmvm_init() from the HAXM capability handshake: TRUE iff the
 * installed driver correctly decodes the SET_RAM HAX_RAM_INFO_FAULTISMMIO flag
 * (advertises HAX_CAP_RAM_FAULTISMMIO).  Older drivers mis-decode it and BSOD,
 * so the EGA per-access VRAM trap (sas.c) must stay disabled unless this set. */
BOOL g_haxm_faultismmio_supported = FALSE;

/* Counts FAST_MMIO exits taken through the hot fast-path so we can still run a
 * (rate-limited) video sync occasionally during a long pure-VRAM burst that
 * produces no other exits.  See the fast path in cpu_simulate(). */
static DWORD g_fastmmio_burst = 0;

/* When 1, FAST_MMIO / STRING_MMIO exits take a hot path that skips the
 * per-access GET_REGS, DBG decode and per-exit haxm_sync_vram() and loops
 * straight back (faster per VRAM byte, but guest `state` is stale between syncs
 * and the screen/host cadence changes).  When 0, those exits fall through to the
 * normal per-exit GET_REGS + haxm_sync_vram + switch -- the original cadence.
 * Default 1 (enabled).  The "input-responsiveness regression" it was suspected
 * of turned out to be stale host/driver state -- cleared by a reboot, not a
 * code change -- so the fast path is back on. */
#ifndef HAXM_FASTMMIO_FASTPATH
#define HAXM_FASTMMIO_FASTPATH 1
#endif

/* I/O hot path: EventVdmIo()/EventVdmStringIo() work purely from tunnel->io +
 * iobuf (the kernel marshals the datum via io_buf and write-backs the guest
 * register on resume), so the GET_REGS + SET_REGS the slow path wraps them in
 * are pure round-trip overhead -- skipping those two ioctls per I/O exit is the
 * speed-up.  The sync MUST stay unconditional (see the fast-path block) because
 * DOOM's unchained Mode X rebuilds planes per Map-Mask write; a rate-limited
 * sync (the first attempt) garbled the planes.  Combined with the tighter QPC
 * heal (ica_unwedge_stuck) this is the "cheap experiment": does steadier IRQ0
 * delivery keep the music stable with the fast-path on?  HAXM_IO_PROBE histo-
 * grams exits by port in the KICKDBG IOPROBE line (hot port = 0x3C5). */
/* DISABLED again after the "cheap experiment" (unconditional sync + QPC heal):
 * the fast-path works (~1.7x, iters/s ~10k) but the speedup itself is the
 * problem -- the heavier/faster render bursts (a) outrun the Mode X planar
 * reconstruction (still garbled) and (b) starve the host timer thread, so IRQ0
 * processing falls BEHIND variably (PERF-HB MID=8-19 / BEHIND=0-11 of 19) ->
 * music wobble.  Neither is a wedge, so the heal can't fix it.  Unlocking this
 * needs the deep work: steady-timer-under-load (thread priority + decouple the
 * music from delivery jitter) AND proper Mode X planar (trap A0000).  Kept
 * gated for that future project. */
#ifndef HAXM_IO_FASTPATH
#define HAXM_IO_FASTPATH 0
#endif

/* Port-SELECTIVE I/O fast-path (the measured-safe variant of HAXM_IO_FASTPATH).
 * The exit profile (perf-doomhang14) showed ~52% of ALL exits are an OPL/AdLib
 * status-read busy-wait delay loop (ports 388/389, 96% reads) and ~45% are Mode
 * X plane-select OUTs (port 3C5) -- co-equal sound vs video.  The global fast
 * path was shelved for VIDEO reasons (Mode X garble + render-rate timer
 * starvation); but NON-video I/O (the OPL delay loop, PIC EOI, kbd) has nothing
 * to do with the VRAM sync, so we fast-path ONLY those: skip GET_REGS+SET_REGS
 * AND the irrelevant haxm_sync_vram(), and leave every VGA port (0x3B0-0x3DF,
 * incl. 3C5) on the unchanged slow path with its unconditional sync.  Strictly a
 * subset of the already-functional global fast path, minus the parts that broke.
 * Making the OPL reads ~3x cheaper frees CPU for the timer thread, so it should
 * help (not hurt) music timing.  Default 1; set 0 to A/B against the old cadence. */
#ifndef HAXM_IO_FASTPATH_NV
#define HAXM_IO_FASTPATH_NV 1
#endif
#if HAXM_IO_FASTPATH_NV
/* VGA register block -- writes here drive the flat-aperture planar reconstruction
 * (or palette/CRTC), so they MUST keep the per-exit sync (stay on the slow path). */
static int port_is_video(unsigned port)
{
    return port >= 0x3B0 && port <= 0x3DF;
}
#endif

extern int ErrorDialogBox(char *message, char *Edit, DWORD dwOptions);
extern GLOBAL VOID haxm_sync_vram(VOID);
extern BOOL host_hwint_hook IPT1(IS32, int_no);
extern BOOL host_swint_hook IPT1(IS32, int_no);
extern BOOL host_exint_hook IPT2(IS32, exp_no, IS32, error_code);


ULONG  getVM (VOID);
ULONG  getPE (VOID);

//
// Internal functions
//

VOID
EventVdmIo(
    VOID
    );

static void EventVdmStringIo(void);


VOID
EventVdmMemAccess(
    VOID
    );


VOID
EventVdmBop(
    VOID
    );

VOID
EventVdmError(
    VOID
    );

VOID
EventVdmIrq13(
    VOID
    );

void hax_handle_fastmmio(struct hax_fastmmio *hft);
void hax_handle_fastmmio_op(struct hax_fastmmio *hft);
void hax_handle_string_mmio(struct hax_string_mmio *sm);
void hax_handle_fastmmio_coalesced(struct hax_coalesced_mmio *coal);


static HANDLE hSystem;
HANDLE hVM;
HANDLE hVCPU;
static ULONG vm_id;

/* --- Async kick (exit-less guest loops, e.g. DOOM's tic-wait) ---------------
 * A worker thread (timer / keyboard / com / mouse) raising an IRQ sets
 * VDM_INTERRUPT_PENDING, but if the VCPU is spinning in the guest with no I/O,
 * BOP or exception it never re-enters the hypervisor to notice it, so the IRQ
 * is never injected and the guest stalls.  We force the VCPU out of
 * HAX_VCPU_IOCTL_RUN with HAX_VCPU_IOCTL_KICKOFF (driver does vcpu_takeoff IPI +
 * sets kick_pending -> the run loop bails to userspace with the benign
 * HAX_EXIT_TIMER).  The next cpu_simulate iteration arms request_interrupt_window
 * and the kernel injects the IRQ when the guest's IF allows it.
 *
 * This is on-demand: the kick is issued only from cpu_interrupt() when an IRQ is
 * actually raised while the VCPU is running -- so a normally-exiting guest pays
 * ~zero extra exits (unlike a periodic preemption timer).
 *
 *   g_in_haxrun        : 1 while the VCPU thread is inside HAX_VCPU_IOCTL_RUN.
 *   g_kick_pending     : dedupe -- at most one KICKOFF in flight per run.
 *   g_cpu_simulate_tid : VCPU thread id; never kick from the VCPU thread itself.
 */
volatile LONG g_in_haxrun       = 0;
volatile LONG g_kick_pending    = 0;
DWORD         g_cpu_simulate_tid = 0;


/* Called from cpu_interrupt() (haxm/x86/int.c) on worker threads after a pending
 * IRQ bit has been set.  Forces the running VCPU out to userspace; deduped so a
 * burst of IRQs costs a single KICKOFF per run iteration. */
void haxm_kick_if_running(void)
{
    DWORD bytes;

    if (!g_in_haxrun) return;
    if (!(*pNtVDMState & VDM_INTERRUPT_PENDING))
        return;
    if (InterlockedExchange(&g_kick_pending, 1) != 0)
        return;                                 /* a KICKOFF is already in flight */
    DeviceIoControl(hVCPU, HAX_VCPU_IOCTL_KICKOFF, NULL, 0, NULL, 0, &bytes, NULL);
}

/* Query (read-and-clear) the EPT dirty bitmap for a guest-physical range via the
 * driver (HAX_VM_IOCTL_QUERY_DIRTY).  Lets the video sync (sas.c) scan only the
 * pages the guest actually wrote.  Returns TRUE if the driver filled the bitmap;
 * FALSE -> caller should full-scan (e.g. driver too old / EPT A/D unsupported).
 * The bitmap is npages bits, 1/page LSB-first; on little-endian x86 the byte
 * layout equals reading it as a uint64_t array, so the caller indexes by qword. */
BOOL haxm_query_dirty(uint64_t gpa, uint64_t size, uint64_t *bitmap, DWORD qwords)
{
    struct hax_dirty_query q;
    DWORD bytes = 0;
    DWORD npages = (DWORD)((size + 0xFFF) >> 12);
    /* Latched off once the driver reports the host CPU can't track EPT A/D
     * (ERROR_NOT_SUPPORTED) -- otherwise we'd round-trip a doomed ioctl on every
     * video sync (~6000/s).  Permanent for the session: it's a hardware fact. */
    static BOOL g_dirty_unsupported = FALSE;

    if (g_dirty_unsupported || npages == 0 || !bitmap || qwords == 0)
        return FALSE;
    q.gpa_start = gpa;
    q.npages    = npages;
    q.pad       = 0;
    if (DeviceIoControl(hVM, HAX_VM_IOCTL_QUERY_DIRTY,
                        &q, sizeof(q),
                        bitmap, qwords * (DWORD)sizeof(uint64_t),
                        &bytes, NULL))
        return TRUE;
    {
        /* One-shot diagnostic so we can tell WHY the query fails:
         *   ERROR_INVALID_FUNCTION(1)/ERROR_NOT_SUPPORTED(50)-ish but actually
         *     the driver returns these as the ioctl not being recognised ->
         *     driver doesn't have the QUERY_DIRTY case (stale .sys / not rebooted)
         *   ERROR_NOT_SUPPORTED(50) from STATUS_NOT_SUPPORTED -> host CPU lacks
         *     EPT accessed/dirty flags (the guard in hax_vm_query_clear_dirty)
         *   ERROR_INSUFFICIENT_BUFFER(122) -> bitmap too small (npages=%u) */
        DWORD gle = GetLastError();
        static int logged = 0;
        if (!logged) {
            char b[160];
            logged = 1;
            sprintf(b, "PERF-SYNC QUERY_DIRTY FAILED gle=%lu npages=%lu outbytes=%lu",
                    (unsigned long)gle, (unsigned long)npages,
                    (unsigned long)(qwords * (DWORD)sizeof(uint64_t)));
            OutputDebugStringA(b);
        }
        /* ERROR_NOT_SUPPORTED == host lacks EPT A/D -> never retry this session. */
        if (gle == ERROR_NOT_SUPPORTED)
            g_dirty_unsupported = TRUE;
    }
    return FALSE;
}

static struct hax_tunnel *tunnel;
static PBYTE iobuf;
struct vcpu_state_t state ={0};

// [LATER]  how do you prevent a struct from straddling a page boundary?
VDM_TIB VdmTib;

ULONG   IntelBase;          // base memory address
ULONG   VdmSize;            // Size of memory in VDM
ULONG   VdmDebugLevel;      // used to control debugging
ULONG InitialBreakpoint = FALSE; // if set, breakpoint at end of cpu_init
ULONG InitialVdmTibFlags = 0; // VdmTib flags picked up from here
CONTEXT InitialContext;     // Initial context for all threads
BOOLEAN DebugContextActive = FALSE;
ULONG VdmFeatureBits = 0;   // bit to indicate special features

// Bop dispatch table

extern void (*BIOS[])();

BOOLEAN ContinueExecution;

// Debug control flags
BOOLEAN fShowBop = TRUE; // FALSE;
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

static BOOL haxm_stopservice(SC_HANDLE hSCManager, WCHAR *wszService)
{
    SC_HANDLE hService = NULL;
    SERVICE_STATUS ss;
    BOOL bRet = TRUE;

    hService = OpenServiceW(hSCManager, wszService, SERVICE_STOP);
    if (!hService)
    {
        DWORD dwErr = GetLastError();
        if (dwErr == ERROR_INVALID_NAME || dwErr == ERROR_SERVICE_DOES_NOT_EXIST)
            return TRUE; // Already stopped or not there
        return FALSE;
    }

    // service exist, try to stop it
    if (!ControlService(hService, SERVICE_CONTROL_STOP, &ss)) {
        if (GetLastError() != ERROR_SERVICE_NOT_ACTIVE)
            bRet = FALSE;
    }

    CloseServiceHandle(hService);
    return bRet;
}

static BOOL haxm_startservice(SC_HANDLE hSCManager, WCHAR *wszService)
{
    SC_HANDLE hService = NULL;
    BOOL bRet;

    hService = OpenServiceW(hSCManager, wszService, SERVICE_START);
    if (!hService)
        return FALSE;

    bRet = StartServiceW(hService, 0, NULL);

    CloseServiceHandle(hService);
    return bRet;
}

static BOOL haxm_ensureservice()
{
    SC_HANDLE hSCManager = NULL;
    BOOL bRet;

    hSCManager = OpenSCManagerW(NULL, NULL, SC_MANAGER_CONNECT);
    if (!hSCManager) {
        return FALSE;
    }

    // Original Intel HAXM driver is not suitable for our use, doesn't contain necessary functions yet
    haxm_stopservice(hSCManager, L"IntelHaxm");

    bRet = haxm_startservice(hSCManager, SERVICE_NAME);
    CloseServiceHandle(hSCManager);
    if (!bRet)
    {
        DWORD dwErr = GetLastError();
        if (dwErr == ERROR_SERVICE_ALREADY_RUNNING) return TRUE;
        /*
        if (dwErr == ERROR_INVALID_NAME || dwErr == ERROR_SERVICE_DOES_NOT_EXIST || dwErr == ERROR_PATH_NOT_FOUND)
        {
            // Setup .inf should do it
        }
        */

        // Other error, bail out
    }

    return bRet;
}


void haxmvm_init()
{
    HMODULE hMod = GetModuleHandle("kernel32.dll");
    static BOOL bInitialized = FALSE;
    struct hax_capabilityinfo cap;
    struct hax_qemu_version verq;
    struct hax_module_version ver;
    struct segment_desc_t nulldesc = {0};
    char buf[1000];
    DWORD bytes;


    if (bInitialized) return;

    haxm_ensureservice();

    hSystem = CreateFileW(L"\\\\.\\HAX", 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSystem == INVALID_HANDLE_VALUE)
    {
        haxmvm_panic("HAXM is not installed.\n");
    }

    if (!DeviceIoControl(hSystem, HAX_IOCTL_VERSION, NULL, 0, &ver, sizeof(ver), &bytes, NULL))
    {
        haxmvm_panic("HAXM VERSION");
    }

    if (!DeviceIoControl(hSystem, HAX_IOCTL_CAPABILITY, NULL, 0, &cap, sizeof(cap), &bytes, NULL))
    {
        haxmvm_panic("HAXM CAPABILITY");
    }

    if ((cap.wstatus & HAX_CAP_WORKSTATUS_MASK) == HAX_CAP_STATUS_NOTWORKING) 
    {
        if (cap.winfo & HAX_CAP_FAILREASON_VT) {
            haxmvm_panic("VTX feature is not enabled, HAX driver will not work.\n");
        } else if (cap.winfo & HAX_CAP_FAILREASON_NX) {
            haxmvm_panic("NX feature is not enabled, HAX driver will not work.\n");
        }
        haxmvm_panic("HAXM not working");
    } 

    if (!(cap.winfo & HAX_CAP_UG)) {
        haxmvm_panic("Unrestricted Guest mode is not supported by the hardware, cannot emulate Realmode with HAXM, bye");
    }

    /* Detect whether this driver can safely handle the EGA per-access VRAM
     * trap.  Asking an old driver to SET_RAM with HAX_RAM_INFO_FAULTISMMIO
     * makes it mis-decode the flag (NULL memslot block) and BSOD the machine,
     * so we must NOT enable the trap unless the driver advertises the fix.
     * sas.c's vram_apply_mapping() gates on g_haxm_faultismmio_supported. */
    g_haxm_faultismmio_supported = (cap.winfo & HAX_CAP_RAM_FAULTISMMIO) ? TRUE : FALSE;
    if (!g_haxm_faultismmio_supported) {
        OutputDebugStringA(
            "HAXM: installed driver is missing the SET_RAM FAULTISMMIO fix; "
            "EGA (Keen-style) planar video will render flat/garbled.  Please "
            "update IntelHaxmNtvdm.sys to a build that advertises "
            "HAX_CAP_RAM_FAULTISMMIO for correct EGA video.\n");
    }

    if (!DeviceIoControl(hSystem, HAX_IOCTL_CREATE_VM, NULL, 0, &vm_id, sizeof(vm_id), &bytes, NULL))
    {
        haxmvm_panic("HAXM CREATE_VM");
    }

    sprintf(buf, "\\\\.\\hax_vm%02d", vm_id);
    hVM = CreateFile(buf, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hVM == INVALID_HANDLE_VALUE)
    {
        haxmvm_panic("HAXM Could not create vm.");
    }

    /* 3~ enable fast mmio */
    verq.cur_version = 1;
    verq.least_version = 0;
    if (!DeviceIoControl(hVM, HAX_VM_IOCTL_NOTIFY_QEMU_VERSION, &verq, sizeof(verq), NULL, 0, &bytes, NULL))
    {
    }
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

/*++

Routine Description:

    This routine is used to prepare the IEU for instruction simulation.
    It will set the Intel registers to thier initial value, and perform
    any implementation specific initialization necessary.


Arguments:


Return Value:

    None.

--*/

{
    ULONG vcpu_id;
    char buf[1000];
    DWORD bytes;
    ULONG excbmp;
    struct hax_tunnel_info tunnel_info;

    haxmvm_init(); // normally sas_init() did it already.

    vcpu_id = 1;
    if (!DeviceIoControl(hVM, HAX_VM_IOCTL_VCPU_CREATE, &vcpu_id, sizeof(vcpu_id), NULL, 0, &bytes, NULL))
    {
        haxmvm_panic("HAXM could not create vcpu.");
    }

    sprintf(buf, "\\\\.\\hax_vm%02d_vcpu%02d", vm_id, vcpu_id);
    hVCPU = CreateFile(buf, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    
    if (!DeviceIoControl(hVCPU, HAX_VCPU_IOCTL_SETUP_TUNNEL, NULL, 0, &tunnel_info, sizeof(tunnel_info), &bytes, NULL))
    {
        haxmvm_panic("HAXM SETUP_TUNNEL");
    } 
    tunnel = (struct hax_tunnel*)tunnel_info.va;
    iobuf = (PBYTE)tunnel_info.io_va;

    excbmp = (1 << VECTOR_NP) | (1 << VECTOR_UD);
    if (!DeviceIoControl(hVCPU, HAX_VCPU_IOCTL_SET_EXCBMP, &excbmp, sizeof(excbmp), NULL, 0, &bytes, NULL))
    {
        haxmvm_panic("Cannot setup exception bitmap for BOPping, are you sure you are running our custom HAXM-version?");
    } 

    if (!DeviceIoControl(hVCPU, HAX_VCPU_GET_REGS, NULL, 0, &state, sizeof(state), &bytes, NULL))
        ZeroMemory(&state, sizeof(state));


    state._cs.selector = 0;
    state._cs.base = 0;
    state._cs.limit = 0xffff;
    SET_SEGMENT_ACCESS(state._cs, 0x9b);

    state._ds.selector = 0;
    state._ds.base = 0;
    state._ds.limit = 0xffff;
    SET_SEGMENT_ACCESS(state._ds, 0x93);

    state._es.selector = 0;
    state._es.base = 0;
    state._es.limit = 0xffff;
    SET_SEGMENT_ACCESS(state._es, 0x93);

    state._fs.selector = 0;
    state._fs.base = 0;
    state._fs.limit = 0xffff;
    SET_SEGMENT_ACCESS(state._fs, 0x93);

    state._gs.selector = 0;
    state._gs.base = 0;
    state._gs.limit = 0xffff;
    SET_SEGMENT_ACCESS(state._gs, 0x93);

    state._ss.selector = 0;
    state._ss.base = 0;
    state._ss.limit = 0xffff;
    SET_SEGMENT_ACCESS(state._ss, 0x93);

    state._ldt.selector = 0;
    state._ldt.base = 0;
    state._ldt.limit = 0xffff;
    SET_SEGMENT_ACCESS(state._ldt, 0x10000);

    state._tr.selector = 0;
    state._tr.base = 0;
    state._tr.limit = 0xffff;
    SET_SEGMENT_ACCESS(state._tr, 0x83);

    state._gdt.base = 0;
    state._gdt.limit = 0x3FF;

    state._idt.base = 0;
    state._idt.limit = 0x3FF;

    state._rflags = 0x02L | EFLAGS_INTERRUPT_MASK; 
    state._cr0 = 0;

    InitialVdmTibFlags |= RM_BIT_MASK;

    *pNtVDMState = InitialVdmTibFlags;


    //
    // Do the rest of thread initialization
    //
    cpu_createthread( NtCurrentThread() 
#ifdef MINNT
        ,NULL
#endif
        );

    InterruptInit();

    if (InitialBreakpoint) {
        DbgBreakPoint();
    }

}

EXPORT
VOID
cpu_terminate(
    )
/*++

Routine Description:


Arguments:


Return Value:


--*/
{
    InterruptTerminate();
    CloseHandle(hVCPU);
    CloseHandle(hVM);
    CloseHandle(hSystem);
}

EXPORT
VOID
cpu_simulate(
    )

/*++

Routine Description:

    This routine causes the simulation of intel instructions to start.

Arguments:

Return Value:

    None.

--*/

{
    NTSTATUS Status;
    VDMEVENTINFO OldEventInfo;
    CONTEXT OldMonitorContext;
    DWORD bytes;
    PBYTE tmp;

#if DBG
    OutputDebugString("cpu_simulate()");
#endif

    if (tunnel->_exit_status == HAX_EXIT_STATECHANGE)
    {
        haxmvm_panic("hypervisor is panicked!!!");
    }

    OldEventInfo = VdmTib.EventInfo;
    OldMonitorContext = VdmTib.MonitorContext;

    /* Publish the VCPU thread id so cpu_interrupt() (called on worker threads)
     * knows not to kick from this thread itself. */
    g_cpu_simulate_tid = GetCurrentThreadId();

    ContinueExecution = TRUE;

    // Load Context into HAXM CPU
    if (!DeviceIoControl(hVCPU, HAX_VCPU_SET_REGS, &state, sizeof(state), NULL, 0, &bytes, NULL))
        haxmvm_panic("HAXM cannot setup CPU state");


    while (ContinueExecution) {

        ASSERT(InterlockedIncrement(&NumTasks) == 0);

        /* Mark "running" BEFORE sampling the pending bit, so an IRQ raised by a
         * worker thread from here on is guaranteed to either (a) be seen by the
         * sample below and arm the interrupt window, or (b) trigger a KICKOFF in
         * cpu_interrupt() that bails us out on this (or the next) run.  Closing
         * the window the other way -- sample first, then mark running -- would
         * let an IRQ slip in between and stall an exit-less guest. */
        InterlockedExchange(&g_in_haxrun, 1);

        tunnel->request_interrupt_window = (*pNtVDMState & VDM_INTERRUPT_PENDING)?1:0;

        if (!DeviceIoControl(hVCPU, HAX_VCPU_IOCTL_RUN, NULL, 0, NULL, 0, &bytes, NULL))
            haxmvm_panic("HAXM cannot run CPU");

        InterlockedExchange(&g_in_haxrun, 0);
        InterlockedExchange(&g_kick_pending, 0);    /* allow the next kick */

        ASSERT(InterlockedDecrement(&NumTasks) < 0);

#if HAXM_FASTMMIO_FASTPATH && (VIDEO_STRATEGY == 1)
        /* FAST_MMIO hot path: with the REP bulk fast path disabled,
         * hax_handle_fastmmio() needs only the io_buf (hft) -- not the guest
         * register file -- so skip the per-access GET_REGS, the DBG decode and
         * the per-exit video sync, and loop straight back.  This is the dominant
         * cost of the per-byte EGA VRAM trap (paddle erase, scrolling, fills).
         * A long pure-VRAM burst produces other exits rarely (timer IRQ etc.),
         * so still run the (internally rate-limited) sync every 256 accesses so
         * the screen keeps updating mid-burst.
         * (Disabled by default -- see HAXM_FASTMMIO_FASTPATH.  When off, these
         * exits fall through to the per-exit GET_REGS + sync + switch below,
         * which has cases for both FAST_MMIO and STRING_MMIO.) */
        if (tunnel->_exit_status == HAX_EXIT_FAST_MMIO)
        {
            hax_handle_fastmmio((struct hax_fastmmio *)iobuf);
            if ((++g_fastmmio_burst & 0xFF) == 0)
                haxm_sync_vram();
            continue;
        }
        /* Batched REP MOVS/STOS into VRAM: kernel already retired the insn, we
         * just move the data.  Like FAST_MMIO it needs only the io_buf. */
        if (tunnel->_exit_status == HAX_EXIT_STRING_MMIO)
        {
            hax_handle_string_mmio((struct hax_string_mmio *)iobuf);
            if ((++g_fastmmio_burst & 0xFF) == 0)
                haxm_sync_vram();
            continue;
        }
#endif

#if HAXM_IO_FASTPATH
        /* I/O hot path (independent of VIDEO_STRATEGY): handle the port from
         * tunnel->io + iobuf and loop straight back -- skip GET_REGS, the DBG
         * decode, SET_REGS and the per-exit sync (rate-limited instead).  Safe
         * because EventVdmIo()/EventVdmStringIo() never read or write `state`.
         * The 8259 heal is NOT needed here: it runs on the (frequent) interrupt
         * exits that still take the slow path below. */
        if (tunnel->_exit_status == HAX_EXIT_IO)
        {
            VdmTib.EventInfo.IoInfo.PortNumber = tunnel->io._port;
            VdmTib.EventInfo.IoInfo.Size       = tunnel->io._size;
            VdmTib.EventInfo.IoInfo.Read       = tunnel->io._direction == HAX_EXIT_IO_IN;
            if (tunnel->io._flags & 1)
                EventVdmStringIo();
            else
                EventVdmIo();
#if HAXM_IO_PROBE
            g_io_port_hits[tunnel->io._port & 0x3FF]++;
#endif
            /* UNCONDITIONAL sync (NOT rate-limited): DOOM's unchained Mode X
             * reconstructs planes from the flat aperture on each Sequencer
             * Map-Mask write (the hot port 0x3C5), so the sync must run on every
             * I/O exit -- exactly as the slow path does.  We still win by
             * skipping the GET_REGS + SET_REGS the slow path wraps it in. */
            haxm_sync_vram();
            continue;
        }
#endif

#if HAXM_IO_FASTPATH_NV
        /* Unified I/O fast-path: handle EVERY HAX_EXIT_IO from tunnel->io + iobuf
         * and loop straight back -- skip GET_REGS, the DBG decode and SET_REGS
         * (the 22% "REGS" wall-time was pure marshalling on these exits, per the
         * time profile).  Safe: EventVdmIo()/EventVdmStringIo() work purely from
         * io_buf, never `state` (the kernel write-backs the guest reg on resume).
         *
         * Correctness obligation the slow path used to provide:
         *   VIDEO ports (Map Mask 3C5 etc.): haxm_sync_vram() MUST run BEFORE
         *   EventVdmIo() applies the write -- the just-written plane has to be
         *   captured under the STILL-CURRENT Map Mask.  Applying the new
         *   selection first (the old global fast-path's order) mis-attributed
         *   the old plane's pixels to the new mask -> the historical garble.
         *   Non-video ports touch no aperture, so they need no sync.
         *
         * NOTE: the 8259 lost-EOI heal is deliberately NOT called here -- it must
         * see a FRESH guest CS (its wedge test is "CS pinned"), but the fast path
         * skips GET_REGS so `state` (getCS()) is stale.  The slow path covers all
         * wedges anyway: a wedge means an IRQ is pending, which makes the async
         * kick force a TIMER exit down the slow path where CS is fresh. */
        if (tunnel->_exit_status == HAX_EXIT_IO)
        {
            if (port_is_video(tunnel->io._port))
                haxm_sync_vram();
            VdmTib.EventInfo.IoInfo.PortNumber = tunnel->io._port;
            VdmTib.EventInfo.IoInfo.Size       = tunnel->io._size;
            VdmTib.EventInfo.IoInfo.Read       = tunnel->io._direction == HAX_EXIT_IO_IN;
            if (tunnel->io._flags & 1)
                EventVdmStringIo();
            else
                EventVdmIo();
            continue;
        }
#endif

        DeviceIoControl(hVCPU, HAX_VCPU_GET_REGS, NULL, 0, &state, sizeof(state), &bytes, NULL);

#if DBG
        {
            char szdbg[256];
            DWORD addr;
            LIN_ADDR descr_addr;

            if (ISPESET && selector_outside_table(state._cs.selector, &descr_addr))
                addr=Sim32GetVDMPointer((state._cs.selector << 16) | state._eip, 1, 0);
            else
                addr=Sim32GetVDMPointer((state._cs.selector << 16) | state._eip, 1, ISPESET);
            sprintf (szdbg, "exit %d @ %04X:%04X (@%X)", tunnel->_exit_status, getCS(), getIP(), addr);
            OutputDebugString(szdbg);
        }
#endif

        /* Auto-detect a Chinese-font TSR (spdos); if found, flip this session to
         * full-CVIDC (g_video_strategy 1->3) so its planar font reaches CVIDC.
         * haxm_sync_vram keeps running -- in the flipped (all-trapped) state it
         * takes the trapped SP_BLIT path.  */
        { extern void video_strategy_autodetect(void); video_strategy_autodetect(); }
#if VIDEO_STRATEGY != 3
        haxm_sync_vram();
#endif

        /* 8259 lost-EOI recovery, every slow-path iteration -- far more often
         * than DispatchInterrupts (~18/s), so a wedged line clears within ~one
         * threshold period instead of being gated by the interrupt rate (that
         * gating was the residual DOOM choppiness).  Needs the ICA lock
         * (ica_unwedge_stuck -> ica_eoi).  cheap: usually just scans 16 ISR
         * bits and returns.  Skipped on the FAST_MMIO fast path above, which is
         * correct -- wedges form on interrupts (slow path), not VRAM bursts.
         * Pass the FRESH guest CS (GET_REGS ran above): the heal only force-EOIs
         * a line whose in-service bit stays set while CS is PINNED (a genuine
         * single-CS spin wedge), NOT while CS moves (a running ISR / PM<->RM
         * interrupt reflection, as at DOOM exit) -- force-EOIing the latter
         * defeats 8259 nesting and overflows DOS/4GW's transfer stack. */
        {
            extern int ica_unwedge_stuck(unsigned cur_cs);   /* ica.c */
            host_ica_lock();
            ica_unwedge_stuck((unsigned)getCS());
            host_ica_unlock();
        }

        switch (tunnel->_exit_status)
        {
        case HAX_EXIT_REALMODE:
            haxmvm_panic("BUGBUG: HAX_EXIT_REALMODE on unrestricted guest CPU shouldn't happen");
            break;

        case HAX_EXIT_INTERRUPT:
            // printf("hypervisor exit interrupt\n");
            if (*pNtVDMState & VDM_INTERRUPT_PENDING) DispatchInterrupts();
            break;

        case HAX_EXIT_TIMER:
            /* Benign: an async KICKOFF forced us out so a worker's pending IRQ
             * could be noticed.  Nothing to do here -- the loop top re-samples
             * VDM_INTERRUPT_PENDING into request_interrupt_window, and the next
             * run takes the interrupt window (HAX_EXIT_INTERRUPT) when the
             * guest's IF allows it.  Do NOT dispatch here: the kick can land
             * with IF=0, and only the kernel's interrupt-window logic respects
             * that. */
            break;

        case HAX_EXIT_STATECHANGE:
            {
                FILE *fp;

                if (fp = fopen("C:\\temp\\ntvdm.dmp", "wb"))
                {
                    fwrite(Start_of_M_area, 0x110000, 1, fp);
                    fclose(fp);
                }
                if (fp = fopen("C:\\temp\\ntvdm.rgs", "wb"))
                {
                    fwrite(&state, sizeof(state), 1, fp);
                    fclose(fp);
                }

            }
#ifdef YODA
            printf("hypervisor is panicked!!!  Reason: %s\n", name_vmx_exit(tunnel->_exit_reason));
            force_yoda();
#else
            haxmvm_panic("hypervisor is panicked!!!  Reason: %s   CS:IP %04X:%04X SP:%04X BP:%08X CR0:%04X",
                name_vmx_exit(tunnel->_exit_reason), getCS(), getIP(), getSP(), getBP(), getCR0());
#endif
            DeviceIoControl(hVCPU, HAX_VCPU_SET_REGS, &state, sizeof(state), NULL, 0, &bytes, NULL);
            break;

        case HAX_EXIT_IO:
            VdmTib.EventInfo.IoInfo.PortNumber = tunnel->io._port;
            VdmTib.EventInfo.IoInfo.Size = tunnel->io._size;
            VdmTib.EventInfo.IoInfo.Read = tunnel->io._direction == HAX_EXIT_IO_IN;
            /* The kernel batches a whole REP INS/OUTS into one HAX_EXIT_IO
             * (io._flags bit0).  EventVdmIo() only does ONE element, so without
             * this a REP OUTS (e.g. the 768-byte palette stream to the VGA DAC
             * at 3C9) wrote just the first byte -> scrambled palette. */
            if (tunnel->io._flags & 1)
                EventVdmStringIo();
            else
                EventVdmIo();
            DeviceIoControl(hVCPU, HAX_VCPU_SET_REGS, &state, sizeof(state), NULL, 0, &bytes, NULL);
            break;

        case HAX_EXIT_FAST_MMIO:
            hax_handle_fastmmio((struct hax_fastmmio *)iobuf);
            break;

        case HAX_EXIT_STRING_MMIO:
            hax_handle_string_mmio((struct hax_string_mmio *)iobuf);
            break;

        case HAX_EXIT_COALESCED_MMIO:
            hax_handle_fastmmio_coalesced(*(struct hax_coalesced_mmio **)iobuf);
            break;

        case HAX_EXIT_PAGEFAULT:
            EventVdmMemAccess();
            DeviceIoControl(hVCPU, HAX_VCPU_SET_REGS, &state, sizeof(state), NULL, 0, &bytes, NULL);
            break;

        case HAX_EXIT_DEBUG:
        {
            LIN_ADDR descr_addr;

            // After mode switch to PM, CS:IP is not valid
            if (ISPESET && selector_outside_table(state._cs.selector, &descr_addr)) break;

            check_I();
            // TODO: Implement check_D() for memory BPX

            if ((bytes = sas_dw_at(effective_addr(getCS(), getEIP()))) && (WORD)bytes == 0xC4C4)
            {
                state._rip += 3;
                tmp = iobuf;
                iobuf = (PBYTE)&bytes;
                EventVdmBop();
                iobuf = tmp;
                check_I();
            }
            DeviceIoControl(hVCPU, HAX_VCPU_SET_REGS, &state, sizeof(state), NULL, 0, &bytes, NULL);
            break;
        }
        case HAX_EXIT_NMI:
        {
			int ret;

            if (tunnel->nmi.exit_intr_info.vector == VECTOR_UD && (iobuf[0] == 0xC4 && iobuf[1] == 0xC4))
            {
                state._rip += 3;
                EventVdmBop();
                DeviceIoControl(hVCPU, HAX_VCPU_SET_REGS, &state, sizeof(state), NULL, 0, &bytes, NULL);
                break;
            }
#ifdef HOOK_INTS
            else if (tunnel->nmi.exit_intr_info.vector == VECTOR_NP && getPE() && iobuf[0] >= 0xCC && iobuf[0] <= 0xCE)
            {
                state._rip += (iobuf[0] == 0xCC||iobuf[0] == 0xCE)?1:2;
                if (host_swint_hook(iobuf[0] == 0xCC?3:(iobuf[0] == 0xCE?4:iobuf[1])))
                {
                    /* Interrupt processed by user defined handler */
                    DeviceIoControl(hVCPU, HAX_VCPU_SET_REGS, &state, sizeof(state), NULL, 0, &bytes, NULL);
                    break;
                }
            }
#endif

#if DBG
            {
                char szdbg[256];
                sprintf(szdbg, "NMI #%d type:%d", tunnel->nmi.exit_intr_info.vector, tunnel->nmi.exit_intr_info.type);
                OutputDebugString(szdbg);
            }
#endif
			ret = 0;
            if (getPE())
            {
                switch(tunnel->nmi.exit_intr_info.type)
                {
					case INTERRUPT:
						ret = host_hwint_hook(tunnel->nmi.exit_intr_info.vector);
						break;
					case SWINT:
						ret = host_swint_hook(tunnel->nmi.exit_intr_info.vector);
						break;
                    case EXCEPTION:
//force_yoda();
						ret = host_exint_hook(tunnel->nmi.exit_intr_info.vector, tunnel->nmi.exit_exception_error_code);
						break;
                }
            }
			if (ret)
			{
				DeviceIoControl(hVCPU, HAX_VCPU_SET_REGS, &state, sizeof(state), NULL, 0, &bytes, NULL);
			}
			else 
			{
				uint32_t vector = tunnel->nmi.exit_intr_info.vector;
				DeviceIoControl(hVCPU, HAX_VCPU_IOCTL_INTERRUPT, &vector, sizeof(vector), NULL, 0, &bytes, NULL);
			}
            break;
        }
        }

    }

    // set this back to true incase we are nested
    ContinueExecution = TRUE;

    //
    // Restore the old Vdm tib info.  This is necessary for the for the
    // case where the application thread is suspended, and a host simulate is
    // performed from another thread
    //

    VdmTib.EventInfo = OldEventInfo;
    VdmTib.MonitorContext = OldMonitorContext;
}


VOID
host_unsimulate(
    )

/*++

Routine Description:

    This routine causes execution of instructions in a VDM to stop.

Arguments:


Return Value:

    None.

--*/

{

    ContinueExecution = FALSE;

}


VOID
EventVdmIo(
    VOID
    )
/*++

Routine Description:

    This function calls the appropriate io simulation routine.

Arguments:


Return Value:

    None.

--*/
{
    if (tunnel->io._size == 1) {
        if (tunnel->io._direction == HAX_EXIT_IO_IN) {
            inb(tunnel->io._port,(half_word *)iobuf);
        } else {
            outb(tunnel->io._port,*((half_word *)iobuf));
        }
    } else if (tunnel->io._size == 2) {
        if (tunnel->io._direction == HAX_EXIT_IO_IN) {
            inw(tunnel->io._port,(word *)iobuf);
        } else {
            outw(tunnel->io._port,*((word *)iobuf));
        }
    }
#if DBG
    else {
    DbgPrint(
        "NtVdm: Unimplemented IO size %d\n",
        tunnel->io._size 
        );
    DbgBreakPoint();
    }
#endif
}


/* REP INS/OUTS string I/O.  The HAXM kernel (handle_string_io) batches the whole
 * REP into ONE HAX_EXIT_IO: for OUT it has already copied the guest source block
 * into io_buf; for IN we fill io_buf and the kernel copies it back to guest
 * memory on resume; and it has already advanced RSI/RDI/RCX and capped _count to
 * the io_buf size (re-exiting for the remainder).  We just replay _count
 * elements over the port in the guest's SI/DI traversal order -- forward, or
 * reversed when _df (DF) is set.  EventVdmIo() did only ONE element, which is
 * what broke REP OUTS to the VGA DAC (palette) and any other string port I/O. */
static void EventVdmStringIo(void)
{
    DWORD i, n = tunnel->io._count;
    DWORD sz   = tunnel->io._size;
    int   bwd  = tunnel->io._df;     /* 1 => guest walked SI/DI downward */
    BOOL  read = (tunnel->io._direction == HAX_EXIT_IO_IN);

    for (i = 0; i < n; i++) {
        DWORD idx  = bwd ? (n - 1 - i) : i;          /* match guest port order */
        PBYTE elem = ((PBYTE)iobuf) + idx * sz;

        if (sz == 1) {
            if (read) inb(tunnel->io._port, (half_word *)elem);
            else      outb(tunnel->io._port, *(half_word *)elem);
        } else if (sz == 2) {
            if (read) inw(tunnel->io._port, (word *)elem);
            else      outw(tunnel->io._port, *(word *)elem);
        }
#if DBG
        else {
            DbgPrint("NtVdm: string IO element size %d unhandled\n", sz);
            break;
        }
#endif
    }
}


VOID
EventVdmBop(
    VOID
    )
/*++

Routine Description:

    This routine dispatches to the appropriate bop handler

Arguments:


Return Value:

    None.

--*/
{
    if (iobuf[2] > MAX_BOP) {
#if DBG
        DbgPrint(
            "NtVdm: Invalid BOP %lx\n", iobuf[2]);
#endif
         ContinueExecution = FALSE;
    } else {
#if DBG
       if (fShowBop) {
       DbgPrint("Ntvdm cpu_simulate : bop dispatch %x,%x\n",
           iobuf[2],
           (ULONG)(*((UCHAR *)Sim32GetVDMPointer(
               (state._cs.selector << 16) | state._eip,
               1,
               ISPESET)))
           );
       }
       {
            char szdbg[256];
            sprintf (szdbg, "BOP %02X,%02X @%X", iobuf[2],
             (ULONG)(*((UCHAR *)Sim32GetVDMPointer(
                           (state._cs.selector << 16) | state._eip,
                           1,
                           ISPESET))),
            Sim32GetVDMPointer(
               (state._cs.selector << 16) | state._eip,
               1,
               ISPESET));
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
EventVdmError(
    VOID
    )
/*++

Routine Description:

    This routine exits the vdm

Arguments:


Return Value:

    None.

--*/
{
    TerminateVDM();
    ContinueExecution = FALSE;
}

VOID
EventVdmIrq13(
    VOID
    )
/*++

Routine Description:

    This routine simulates an IRQ 13 to the vdm
    Is this really needed? Our CPUs always have a coprocessor?
    Or am I mistaken by the use of this IRQ?

Arguments:


Return Value:

    None.

--*/
{
    if (!IRQ13BeingHandled) {
        IRQ13BeingHandled = TRUE;
        ica_hw_interrupt(
            ICA_SLAVE,
            5,
            1
            );
    }
}


VOID
EventVdmMemAccess(
    VOID
    )
/*++

Routine Description:

    This routine will call the page fault handler routine which
    is common to both x86 and mips.

Arguments:


Return Value:

    None.

--*/
{

    // RWMode is 0 if read fault or 1 if write fault.

    DispatchPageFault(
        (ULONG)tunnel->pagefault.gpa,
        (tunnel->pagefault.flags & HAX_PAGEFAULT_ACC_W?1:0)
        );
}

/* See the long comment below: the user-mode REP bulk fast path conflicts with
 * the in-kernel emulator's mid-instruction resume and corrupts REP EGA latch
 * copies/fills.  Disabled until reimplemented correctly. */
#ifndef HAXM_REP_FASTPATH
#define HAXM_REP_FASTPATH 0
#endif

void hax_handle_fastmmio(struct hax_fastmmio *hft)
{
#if HAXM_REP_FASTPATH
    UCHAR *pCmd = ((UCHAR *)Sim32GetVDMPointer(
                   (state._cs.selector << 16) | state._eip,
                   1, ISPESET));
#endif

#if DBG
    char szdbg[128];

    sprintf (szdbg, "hax_handle_fastmmio(dir=%d, size=%d, gpa=%08X\n", hft->direction, hft->size, (DWORD)hft->gpa);
    OutputDebugString(szdbg);
#endif


    /* emv86.asm emulates REP prefices, we should check for them too to avoid 
     * unneccesary overhead with thousands of calls and call sas_PRWS instead
     *
     * hft->direction = 2 unfortunately isn't implemented in HAXM yet :(
     *
     * DISABLED (HAXM_REP_FASTPATH 0): this bulk-handles the whole REP in user
     * mode (sas_PRWS/sas_fills + SET_REGS ECX=0), but the in-kernel emulator is
     * still MID-INSTRUCTION (em_ctxt not finished) -- on resume it continues its
     * half-done string op with cached operands, and for the read-direction batch
     * we never fill hft->value, so it writes a stale byte at the old position.
     * That corrupts REP-based EGA latch copies / fills (paddle erase leaves
     * traces, progress bar shows horizontal stripes) while non-REP ops are fine.
     * Falling through routes every REP byte through the normal per-access
     * read_b/write_b (correct latch semantics, matches the WHP backend).  Re-add
     * a batch optimisation later in a way that finishes the kernel instruction. */
#if HAXM_REP_FASTPATH
    if (*pCmd == 0xF3)
    {
        BOOL bHandled = FALSE;
        DWORD bytes;

        switch (hft->direction)
        {
        case 0: /* Read */
            switch (pCmd[1])
            {
            case 0xA4:  /* Move (E)CX bytes from DS:[(E)SI] to ES:[(E)DI].*/
                if (getDF()) break; // Not implemented yet
                sas_PRWS((DWORD)hft->gpa, (PHY_ADDR)RMSEGOFFTOLIN(getES(), getEDI()), getECX());
                setEDI(getEDI() + getECX());
                setESI(getESI() + getECX());
                setECX(0);
                bHandled = TRUE;
                break;

            case 0xA5:  /* Move (E)CX words from DS:[(E)SI] to ES:[(E)DI].*/
                if (getDF()) break; // Not implemented yet
                sas_PRWS((DWORD)hft->gpa, (PHY_ADDR)RMSEGOFFTOLIN(getES(), getEDI()), getECX() * hft->size);
                setEDI(getEDI() + getECX() * hft->size);
                setESI(getESI() + getECX() * hft->size);
                setECX(0);
                bHandled = TRUE;
                break;
            }
            break;
        case 1: /* Write */
            switch (pCmd[1])
            {
            case 0xA4:  /* Move (E)CX bytes from DS:[(E)SI] to ES:[(E)DI].*/
            case 0xA5:  /* Move (E)CX words from DS:[(E)SI] to ES:[(E)DI].*/
                if (getDF()) break; // Not implemented yet
                sas_PRWS((DWORD)RMSEGOFFTOLIN(getDS(), getESI()), (DWORD)hft->gpa, getECX() * hft->size);
                setEDI(getEDI() + getECX() * hft->size);
                setESI(getESI() + getECX() * hft->size);
                setECX(0);
                bHandled = TRUE;
                break;
            case 0xAA:  /* Fill (E)CX bytes at ES:[(E)DI] with AL. */
                sas_fills((DWORD)hft->gpa, getAL(), getECX());
                setECX(0);
                bHandled = TRUE;
                break;
            case 0xAB:  /* Fill (E)CX words at ES:[(E)DI] with AX. */
                switch (hft->size)
                {
                case 2: 
                    sas_fillsw((DWORD)hft->gpa, getAX(), getECX());
                    setECX(0);
                    bHandled = TRUE;
                    break;
                case 4: 
                    sas_fillsdw((DWORD)hft->gpa, getEAX(), getECX());
                    setECX(0);
                    bHandled = TRUE;
                    break;
                }
                break;
            }
            break;
        }
        if (bHandled)
        {
            if (!DeviceIoControl(hVCPU, HAX_VCPU_SET_REGS, &state, sizeof(state), NULL, 0, &bytes, NULL));
            return;
        }
    }
#endif /* HAXM_REP_FASTPATH */

#if VIDEO_STRATEGY == 2 || VIDEO_STRATEGY == 4
    if (hft->gpa>=gvi_pc_low_regen && hft->gpa<=gvi_pc_high_regen) haxm_vram_mmio(hft);
#endif

    hax_handle_fastmmio_op(hft);
}

/* Replay a whole REP MOVS/STOS the kernel batched into one HAX_EXIT_STRING_MMIO.
 * The kernel already retired the instruction (RIP/RDI/RSI advanced, RCX=0), so
 * we only move the data, element by element, through the SAME per-element
 * primitives the per-access path uses (sas_PR/PW -> read_b/write_b), preserving
 * EGA latch / set-reset / map-mask semantics exactly -- just without N VM exits. */
void hax_handle_string_mmio(struct hax_string_mmio *sm)
{
    DWORD i, n = sm->count;
    int   step = sm->df ? -(int)sm->size : (int)sm->size;
    DWORD dst  = (DWORD)sm->dst_gpa;

    if (sm->op == HAX_STRING_STOS)
    {
        switch (sm->size)
        {
        case 1: { BYTE  v = (BYTE) sm->value; for (i=0;i<n;i++){ sas_PW8 (dst,v); dst+=step; } break; }
        case 2: { WORD  v = (WORD) sm->value; for (i=0;i<n;i++){ sas_PW16(dst,v); dst+=step; } break; }
        case 4: { DWORD v = (DWORD)sm->value; for (i=0;i<n;i++){ sas_PW32(dst,v); dst+=step; } break; }
        }
    }
    else /* HAX_STRING_MOVS */
    {
        DWORD src = (DWORD)sm->src_gpa;
        switch (sm->size)
        {
        case 1: for (i=0;i<n;i++){ sas_PW8 (dst, sas_PR8 (src)); dst+=step; src+=step; } break;
        case 2: for (i=0;i<n;i++){ sas_PW16(dst, sas_PR16(src)); dst+=step; src+=step; } break;
        case 4: for (i=0;i<n;i++){ sas_PW32(dst, sas_PR32(src)); dst+=step; src+=step; } break;
        }
    }
}

void hax_handle_fastmmio_coalesced(struct hax_coalesced_mmio *coal)
{
    uint32_t i;

#if DBG
    char szdbg[128];

    sprintf (szdbg, "hax_handle_fastmmio_coalesced(size=%d\n", coal->size);
    OutputDebugString(szdbg);
#endif

    for (i=0; i<coal->size; i++)
        hax_handle_fastmmio_op(&coal->mmio[i]);
    coal->size = 0;
}

void hax_handle_fastmmio_op(struct hax_fastmmio *hft)
{
    switch (hft->direction)
    {
    case 0: /* Read */
        switch (hft->size)
        {
        case 1: *((PBYTE)&hft->value) = sas_PR8((DWORD)hft->gpa); break;
        case 2: *((PWORD)&hft->value) = sas_PR16((DWORD)hft->gpa); break;
        case 4: *((PDWORD)&hft->value) = sas_PR32((DWORD)hft->gpa); break;
        default: haxmvm_panic("Seems hax_handle_fastmmio also gets size %d", hft->size);
        }
        break;
    case 1: /* Write */
        switch (hft->size)
        {
        case 1: sas_PW8((DWORD)hft->gpa, (BYTE)hft->value); break;
        case 2: sas_PW16((DWORD)hft->gpa, (WORD)hft->value); break;
        case 4: sas_PW32((DWORD)hft->gpa, (DWORD)hft->value); break;
        default: haxmvm_panic("Seems hax_handle_fastmmio also writes size %d", hft->size);
        }
        break;
    case 2: /* gpa -> gpa2 memcpy */
        {
            /*
            struct hax_fastmmio tmp;
            IU8 size, offset;

            for (size = hft->size, offset = 0; size; size -= tmp.size, offset += tmp.size)
            {
                if (size>=4) tmp.size = 4; else
                if (size>=2) tmp.size = 2; else tmp.size = 1;
                tmp.direction = 0;
                tmp.gpa = hft->gpa + offset;
                hax_handle_fastmmio(&tmp);
                tmp.direction = 1;
                tmp.gpa = hft->gpa2 + offset;
                hax_handle_fastmmio(&tmp);
            }
            */

            // Should be faster via SAS
            sas_PRWS((DWORD)hft->gpa, (DWORD)hft->gpa2, hft->size);
            break;
        }
    }
}

void parse_lc_options (int * pargc, char *** pargv)
{
    return;
}

#define NANO_ENABLED    0x20

VOID Nano_slow_yoda(VOID)
{
    if ((GLOBAL_nanoDebugControl & NANO_ENABLED) == 0)
    {
        DWORD bytes;

        GLOBAL_nanoDebugControl |= NANO_ENABLED;
        haxdbg.control = HAX_DEBUG_ENABLE | HAX_DEBUG_MONSTEP | HAX_DEBUG_USE_HW_BP;
        DeviceIoControl(hVCPU, HAX_IOCTL_VCPU_DEBUG, &haxdbg, sizeof(haxdbg), NULL, 0, &bytes, NULL);
        printf ("Nano yoda enabled\n");
    }
}

void load_seg(segment_desc_t *segment, WORD sel)
{
    LIN_ADDR descr_addr;
    DESCR entry;

    segment->selector = sel;
    if (!getPE() || getVM() || selector_outside_table(sel, &descr_addr) == 1)
    {
        segment->base = sel << 4;
        segment->limit = 0xFFFF;
    }
    else
    {
        read_descriptor(descr_addr, &entry);
        segment->base = entry.base;
        segment->limit = entry.limit;
        segment->ar = sas_dw_at(descr_addr+4);
        segment->ar >>= 8;
        segment->ar &= 0xF0FF;
		segment->ar |= 1;  // Intel Vol3C Part 3: 26.3.1.2 Checks on Guest Segment Registers
    }
}


// Get and Set routines for intel registers.

ULONG  getEAX (VOID) { return  (state._eax); }
USHORT getAX  (VOID) { return  ((USHORT)(state._eax)); }
UCHAR  getAL  (VOID) { return  ((BYTE)(state._eax)); }
UCHAR  getAH  (VOID) { return  ((BYTE)(state._eax >> 8)); }
ULONG  getEBX (VOID) { return  (state._ebx); }
USHORT getBX  (VOID) { return  ((USHORT)(state._ebx)); }
UCHAR  getBL  (VOID) { return  ((BYTE)(state._ebx)); }
UCHAR  getBH  (VOID) { return  ((BYTE)(state._ebx >> 8)); }
ULONG  getECX (VOID) { return  (state._ecx); }
USHORT getCX  (VOID) { return  ((USHORT)(state._ecx)); }
UCHAR  getCL  (VOID) { return  ((BYTE)(state._ecx)); }
UCHAR  getCH  (VOID) { return  ((BYTE)(state._ecx >> 8)); }
ULONG  getEDX (VOID) { return  (state._edx); }
USHORT getDX  (VOID) { return  ((USHORT)(state._edx)); }
UCHAR  getDL  (VOID) { return  ((BYTE)(state._edx)); }
UCHAR  getDH  (VOID) { return  ((BYTE)(state._edx >> 8)); }
ULONG  getESP (VOID) { return  (state._esp); }
USHORT getSP  (VOID) { return  ((USHORT)state._esp); }
ULONG  getEBP (VOID) { return  (state._ebp); }
USHORT getBP  (VOID) { return  ((USHORT)state._ebp); }
ULONG  getESI (VOID) { return  (state._esi); }
USHORT getSI  (VOID) { return  ((USHORT)state._esi); }
ULONG  getEDI (VOID) { return  (state._edi); }
USHORT getDI  (VOID) { return  ((USHORT)state._edi); }
ULONG  getEIP (VOID) { return  (state._eip); }
USHORT getIP (VOID)  { return  ((USHORT)state._eip); }

USHORT getCS (VOID)  { return  ((USHORT)state._cs.selector); }
USHORT getSS (VOID)  { return  ((USHORT)state._ss.selector); }
USHORT getDS (VOID)  { return  ((USHORT)state._ds.selector); }
USHORT getES (VOID)  { return  ((USHORT)state._es.selector); }
USHORT getFS (VOID)  { return  ((USHORT)state._fs.selector); }
USHORT getGS (VOID)  { return  ((USHORT)state._gs.selector); }

ULONG  getCF (VOID)  { return  ((state._eflags & FLG_CARRY) ? 1 : 0); }
ULONG  getPF (VOID)  { return  ((state._eflags & FLG_PARITY) ? 1 : 0); }
ULONG  getAF (VOID)  { return  ((state._eflags & FLG_AUXILIARY) ? 1 : 0); }
ULONG  getZF (VOID)  { return  ((state._eflags & FLG_ZERO) ? 1 : 0); }
ULONG  getSF (VOID)  { return  ((state._eflags & FLG_SIGN) ? 1 : 0); }
ULONG  getTF (VOID)  { return  ((state._eflags & FLG_TRAP) ? 1 : 0); }
ULONG  getIF (VOID)  { return  ((state._eflags & FLG_INTERRUPT) ? 1 : 0); }
ULONG  getDF (VOID)  { return  ((state._eflags & FLG_DIRECTION) ? 1 : 0); }
ULONG  getOF (VOID)  { return  ((state._eflags & FLG_OVERFLOW) ? 1 : 0); }
ULONG  getVM (VOID)  { return  ((state._eflags & 0x20000) ? 1 : 0); }
ULONG  getIOPL(VOID)        { return (state._eflags >> 12) & 3; }
ULONG  getNT(VOID)          { return ((state._eflags & 0x4000) ? 1 : 0); }
USHORT getSTATUS(VOID)      { return (USHORT)state._eflags; }
ULONG  getEFLAGS(VOID)      { return (state._eflags); }
USHORT getFLAGS(VOID)       { return (USHORT)state._eflags; }

/* Not sure, only relevant in PM? */
ULONG  getCPL(VOID)         { return ((state._cr0 & MSW_PE) ? state._cs.dpl : 0); }
ULONG  getGDT_BASE(VOID)    { return ((ULONG)state._gdt.base); }
ULONG  getGDT_LIMIT(VOID)   { return (state._gdt.limit); }
ULONG  getIDT_BASE(VOID)    { return ((ULONG)state._idt.base); }
ULONG  getIDT_LIMIT(VOID)   { return (state._idt.limit); }
ULONG  getLDT_BASE(VOID)    { return ((ULONG)state._ldt.base); }
ULONG  getLDT_LIMIT(VOID)   { return (state._ldt.limit); }
ULONG  getLDT_SELECTOR(VOID){ return (state._ldt.selector); }
ULONG  getTR_BASE(VOID)     { return ((ULONG)state._tr.base); }
ULONG  getTR_LIMIT(VOID)    { return (state._tr.limit); }
ULONG  getTR_SELECTOR(VOID) { return (state._tr.selector); }

ULONG  getCS_BASE(VOID)     { return ((ULONG)state._cs.base); }
ULONG  getCS_LIMIT(VOID)    { return (state._cs.limit); }
ULONG  getCS_AR(VOID)       { return (state._cs.ar); }
ULONG  getDS_BASE(VOID)     { return ((ULONG)state._ds.base); }
ULONG  getDS_LIMIT(VOID)    { return (state._ds.limit); }
ULONG  getDS_AR(VOID)       { return (state._ds.ar); }
ULONG  getES_BASE(VOID)     { return ((ULONG)state._es.base); }
ULONG  getES_LIMIT(VOID)    { return (state._es.limit); }
ULONG  getES_AR(VOID)       { return (state._es.ar); }
ULONG  getFS_BASE(VOID)     { return ((ULONG)state._fs.base); }
ULONG  getFS_LIMIT(VOID)    { return (state._fs.limit); }
ULONG  getFS_AR(VOID)       { return (state._fs.ar); }
ULONG  getGS_BASE(VOID)     { return ((ULONG)state._gs.base); }
ULONG  getGS_LIMIT(VOID)    { return (state._gs.limit); }
ULONG  getGS_AR(VOID)       { return (state._gs.ar); }
ULONG  getSS_BASE(VOID)     { return ((ULONG)state._ss.base); }
ULONG  getSS_LIMIT(VOID)    { return (state._ss.limit); }
ULONG  getSS_AR(VOID)       { return (state._ss.ar); }

USHORT getMSW (VOID)        { return ((USHORT)state._cr0); }
ULONG  getCR0 (VOID)        { return ((LONG)state._cr0); }
ULONG  getPE(VOID)          { return ((state._cr0 & MSW_PE) ? 1 : 0); }
ULONG  getMP(VOID)          { return ((state._cr0 & 2) ? 1 : 0); }
ULONG  getEM(VOID)          { return ((state._cr0 & 4) ? 1 : 0); }
ULONG  getTS(VOID)          { return ((state._cr0 & 8) ? 1 : 0); }
ULONG  getET(VOID)          { return ((state._cr0 & 0x10) ? 1 : 0); }
ULONG  getNE(VOID)          { return ((state._cr0 & 0x20) ? 1 : 0); }
ULONG  getWP(VOID)          { return ((state._cr0 & 0x10000) ? 1 : 0); }
ULONG  getAM(VOID)          { return ((state._cr0 & 0x40000) ? 1 : 0); }
ULONG  getNW(VOID)          { return ((state._cr0 & 0x20000000) ? 1 : 0); }
ULONG  getCD(VOID)          { return ((state._cr0 & 0x40000000) ? 1 : 0); }
ULONG  getPG(VOID)          { return ((state._cr0 & 0x80000000) ? 1 : 0); }

ULONG  getCR2 (VOID)        { return ((ULONG)state._cr2); }
ULONG  getCR3 (VOID)        { return ((ULONG)state._cr3); }
ULONG  getCR4 (VOID)        { return ((ULONG)state._cr4); }

ULONG  getDREG0(VOID)       { return ((ULONG)state._dr0); }
ULONG  getDREG1(VOID)       { return ((ULONG)state._dr1); }
ULONG  getDREG2(VOID)       { return ((ULONG)state._dr2); }
ULONG  getDREG3(VOID)       { return ((ULONG)state._dr3); }
ULONG  getDREG6(VOID)       { return ((ULONG)state._dr6); }
ULONG  getDREG7(VOID)       { return ((ULONG)state._dr7); }

VOID setPE(ULONG val) {
    state._cr0 = state._cr0 & (~MSW_PE) | (val & MSW_PE);
}

VOID setEAX (ULONG val) {
    state._eax = val;
}

VOID setAX  (USHORT val) {
    state._eax = (state._eax & 0xFFFF0000) |
                            ((ULONG)val & 0x0000FFFF);
}

VOID setAH  (UCHAR val) {
    state._eax = (state._eax & 0xFFFF00FF) |
                            ((ULONG)(val << 8) & 0x0000FF00);
}

VOID setAL  (UCHAR val) {
    state._eax = (state._eax & 0xFFFFFF00) |
                            ((ULONG)val & 0x000000FF);
}

VOID setEBX (ULONG val) {
    state._ebx = val ;
}

VOID setBX  (USHORT val) {
    state._ebx = (state._ebx & 0xFFFF0000) |
                            ((ULONG)val & 0x0000FFFF);
}

VOID setBH  (UCHAR val) {
    state._ebx = (state._ebx & 0xFFFF00FF) |
                            ((ULONG)(val << 8) & 0x0000FF00);
}

VOID setBL  (UCHAR  val) {
    state._ebx = (state._ebx & 0xFFFFFF00) |
                            ((ULONG)val & 0x000000FF);
}

VOID setECX (ULONG val) {
    state._ecx = val ;
}

VOID setCX  (USHORT val) {
    state._ecx = (state._ecx & 0xFFFF0000) |
                            ((ULONG)val & 0x0000FFFF);
}

VOID setCH  (UCHAR val) {
    state._ecx = (state._ecx & 0xFFFF00FF) |
                            ((ULONG)(val << 8) & 0x0000FF00);
}

VOID setCL  (UCHAR val) {
    state._ecx = (state._ecx & 0xFFFFFF00) |
                            ((ULONG)val & 0x000000FF);
}

VOID setEDX (ULONG val) {
    state._edx = val ;
}

VOID setDX  (USHORT val) {
    state._edx = (state._edx & 0xFFFF0000) |
                            ((ULONG)val & 0x0000FFFF);
}

VOID setDH  (UCHAR val) {
    state._edx = (state._edx & 0xFFFF00FF) |
                            ((ULONG)(val << 8) & 0x0000FF00);
}

VOID setDL  (UCHAR val) {
    state._edx = (state._edx & 0xFFFFFF00) |
                                ((ULONG)val & 0x000000FF);
}

VOID setESP (ULONG val) {
    state._esp = val ;
}

VOID setSP  (USHORT val) {
    state._esp = (state._esp & 0xFFFF0000) |
                                ((ULONG)val & 0x0000FFFF);
}

VOID setEBP (ULONG val) {
    state._ebp = val;
}

VOID setBP  (USHORT val) {
    state._ebp = (state._ebp & 0xFFFF0000) |
                                ((ULONG)val & 0x0000FFFF);
}

VOID setESI (ULONG val) {
    state._esi = val ;
}

VOID setSI  (USHORT val) {
    state._esi = (state._esi & 0xFFFF0000) |
                                ((ULONG)val & 0x0000FFFF);
}
VOID setEDI (ULONG val) {
    state._edi = val ;
}

VOID setDI  (USHORT val) {
    state._edi = (state._edi & 0xFFFF0000) |
                                ((ULONG)val & 0x0000FFFF);
}

VOID setEIP (ULONG val) {
    state._eip = val ;
}

VOID setIP  (USHORT val) {
    state._eip = (state._eip & 0xFFFF0000) |
                                ((ULONG)val & 0x0000FFFF);
}

VOID setCS  (USHORT val) {
    load_seg(&state._cs, val);
    if (!getPE() || getVM())
    {
        state._cs.type = 11;
        state._cs.dpl = getVM()?3:0;
        if (state._cs.limit > 0xffff) state._cs.limit = 0xffff;
    }
}

VOID setSS  (USHORT val) {
    load_seg(&state._ss, val);
    if (!getPE() || getVM())
    {
        state._ss.dpl = getVM()?3:0;
    }
}

VOID setDS  (USHORT val) {
    load_seg(&state._ds, val);
}

VOID setES  (USHORT val) {
    load_seg(&state._es, val);
}

VOID setFS  (USHORT val) {
    load_seg(&state._fs, val);
}

VOID setGS  (USHORT val) {
    load_seg(&state._gs, val);
}

VOID setCF  (ULONG val)  {
    state._eflags = (state._eflags & ~FLG_CARRY) |
                                (((ULONG)val << FLG_CARRY_BIT) & FLG_CARRY);
}

VOID setPF  (ULONG val) {
    state._eflags = (state._eflags & ~FLG_PARITY) |
                                (((ULONG)val << FLG_PARITY_BIT) & FLG_PARITY);
}

VOID setAF  (ULONG val) {
    state._eflags = (state._eflags & ~FLG_AUXILIARY) |
                                (((ULONG)val << FLG_AUXILIARY_BIT) & FLG_AUXILIARY);
}

VOID setZF  (ULONG val) {
    state._eflags = (state._eflags & ~FLG_ZERO) |
                                (((ULONG)val << FLG_ZERO_BIT) & FLG_ZERO);
}

VOID setSF  (ULONG val) {
    state._eflags = (state._eflags & ~FLG_SIGN) |
                                (((ULONG)val << FLG_SIGN_BIT) & FLG_SIGN);
}

VOID setIF  (ULONG val) {
    state._eflags = (state._eflags & ~FLG_INTERRUPT) |
                                (((ULONG)val << FLG_INTERRUPT_BIT) & FLG_INTERRUPT);
}

VOID setDF  (ULONG val) {
    state._eflags = (state._eflags & ~FLG_DIRECTION) |
                                (((ULONG)val << FLG_DIRECTION_BIT) & FLG_DIRECTION);
}

VOID setOF  (ULONG val) {
    state._eflags = (state._eflags & ~FLG_OVERFLOW) |
                                (((ULONG)val << FLG_OVERFLOW_BIT) & FLG_OVERFLOW);
}

VOID setNT  (ULONG val) {
    state._eflags = (state._eflags & ~0x4000) |
                                (((ULONG)val << 14) & 0x4000);
}

VOID setMSW (USHORT val) {
    (USHORT)state._cr0 = val ;
}

VOID setCR0 (ULONG val) {
    state._cr0 = val ;
}

VOID setPG (ULONG val) {
    state._cr0 = (state._cr0 & ~0x80000000 | ((val) & 1) << 31);
}

VOID setSTATUS(USHORT val) {
    state._eflags = (state._eflags & 0xFFFF0000) | val;
}



VOID setEFLAGS (ULONG val) {
    state._eflags = val;
}

VOID setCS_BASE_LIMIT_AR(ULONG base, ULONG limit, USHORT ar) {
    state._cs.base = base;
    state._cs.limit = limit;
    state._cs.ar = ar;
}

VOID setDS_BASE_LIMIT_AR(ULONG base, ULONG limit, USHORT ar) {
    state._ds.base = base;
    state._ds.limit = limit;
    state._ds.ar = ar;
}

VOID setES_BASE_LIMIT_AR(ULONG base, ULONG limit, USHORT ar) {
    state._es.base = base;
    state._es.limit = limit;
    state._es.ar = ar;
}

VOID setFS_BASE_LIMIT_AR(ULONG base, ULONG limit, USHORT ar) {
    state._fs.base = base;
    state._fs.limit = limit;
    state._fs.ar = ar;
}

VOID setGS_BASE_LIMIT_AR(ULONG base, ULONG limit, USHORT ar) {
    state._gs.base = base;
    state._gs.limit = limit;
    state._gs.ar = ar;
}

VOID setSS_BASE_LIMIT_AR(ULONG base, ULONG limit, USHORT ar) {
    state._ss.base = base;
    state._ss.limit = limit;
    state._ss.ar = ar;
}

VOID setIDT_BASE_LIMIT(ULONG base, USHORT limit)
{
    state._idt.base = base;
    state._idt.limit = limit;
}


VOID setCPL(ULONG val) { }


#define DR7_VALID_BITS 0xffff03ff
#define DR6_VALID_BITS 0x0000e00f

VOID moveToDebugRegister(ULONG debugRegister, ULONG src)
{
    if (GLOBAL_nanoDebugControl & NANO_ENABLED)
    {
        DWORD bytes;

        haxdbg.dr[debugRegister] = src;
        DeviceIoControl(hVCPU, HAX_IOCTL_VCPU_DEBUG, &haxdbg, sizeof(haxdbg), NULL, 0, &bytes, NULL);
    }
    else
    {
        // Not sure if we want that.. 
        switch (debugRegister)
        {
            case 0: state._dr0 = src; break;
            case 1: state._dr1 = src; break;
            case 2: state._dr2 = src; break;
            case 3: state._dr3 = src; break;    // setup BPX
            case 4:   /* Reserved */
            case 5: break;
            case 6: state._dr6 = src & DR6_VALID_BITS; break;
            case 7: state._dr7 = src & DR7_VALID_BITS; break;   // setup BPX
        }
    }
}

//
// FPU status words are only demanded by debugger, so we can be wasteful
// and request FPU status just on demand
//
IU32 a_getNpxStatusReg ()
{
    struct fx_layout fl;
    DWORD bytes;

    DeviceIoControl(hVCPU, HAX_VCPU_IOCTL_GET_FPU, NULL, 0, &fl, sizeof(fl), &bytes, NULL);
    return fl.fsw;
}

IU32 a_getNpxControlReg ()
{
    struct fx_layout fl;
    DWORD bytes;

    DeviceIoControl(hVCPU, HAX_VCPU_IOCTL_GET_FPU, NULL, 0, &fl, sizeof(fl), &bytes, NULL);
    return fl.fcw;
}

IU32 a_getNpxTagwordReg ()
{
    struct fx_layout fl;
    DWORD bytes;

    DeviceIoControl(hVCPU, HAX_VCPU_IOCTL_GET_FPU, NULL, 0, &fl, sizeof(fl), &bytes, NULL);
    return fl.ftw;
}

CHAR *a_getNpxStackReg (IU32 reg_num, CHAR * dumpStore)
{
    strcpy(dumpStore, "not_impl");
    return dumpStore;
}


VOID setSnaffleDataDebugExcpn(IBOOL dosnaffle)
{
    setNT(dosnaffle & 0xFF);
}

PX86CONTEXT
getIntelRegistersPointer(
    VOID
    )
/*++

Routine Description:

    Return Address on Intel Registers for WOW Fast Access

Arguments:

    None

Return Value:

    Pointer to Intel Registers x86 Context Record


--*/
{
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

    VdmTib.VdmContext.Ebp = state._ebp;
    VdmTib.VdmContext.Eip = state._eip;
    VdmTib.VdmContext.EFlags = state._eflags & ~2;
    VdmTib.VdmContext.Esp = state._esp;

    return &(VdmTib.VdmContext);
}



