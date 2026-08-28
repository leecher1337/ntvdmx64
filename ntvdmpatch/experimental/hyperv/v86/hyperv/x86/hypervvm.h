/*++

Copyright (c) 2026  leecher@dose.0wnz.at

Module Name:

    hypervvm.h

Abstract:

    User-mode header for the Windows Hypervisor Platform (WHP / WinHvPlatform)
    backend for NTVDMx64.  This is the pluggable replacement for haxmvm.h.

    The layout of `vcpu_state_t` and the helper `segment_desc_t` is kept
    bit-identical to the HAXM port so that the rest of NTVDM (sas.c, int.c,
    BIOS BOPs, etc.) can read `state._eax`, `state._cs.base`, etc., with no
    changes.  The HAXM-specific tunnel / IOCTL definitions are gone; the
    monitor loop talks to WHP directly via WinHvPlatform.dll.

Environment:

    User mode only

--*/

#pragma once
#include <stdio.h>
#include "whp_api.h"   /* pulls in <windows.h>, <WinHvPlatform.h>, <WinHvPlatformDefs.h>,
                          declares g_whp, installs the WHv* -> g_whp.pWHv* macros */

// Signed Types
typedef signed char         int8;
typedef signed short        int16;
typedef signed int          int32;
typedef __int64             int64;

// Unsigned Types
typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned int        uint;
typedef unsigned __int64    uint64;
typedef unsigned long       ulong;

typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned __int64    uint64_t;
typedef unsigned long       ulong_t;

typedef uint64 hax_pa_t;
typedef uint64 hax_pfn_t;
typedef uint64 paddr_t;
typedef uint64 vaddr_t;

#include <pshpack1.h>

/*
 * Bit-compatible mirrors of the HAXM register snapshot.  We do not exchange
 * this with the kernel any more; it is purely an in-process scratch area that
 * lets the rest of NTVDM keep working unchanged.  hyperv_load_state() and
 * hyperv_store_state() (see monitor.c) convert this to/from WHV_REGISTER_VALUE
 * arrays around every WHvRunVirtualProcessor call.
 */

union interruptibility_state_t {
    uint32 raw;
    struct {
        uint32 sti_blocking : 1;
        uint32 movss_blocking : 1;
        uint32 smi_blocking : 1;
        uint32 nmi_blocking : 1;
        uint32 reserved : 28;
    };
    uint64_t pad;
};
typedef union interruptibility_state_t interruptibility_state_t;

struct segment_desc_t {
    uint16 selector;
    uint16 _dummy;
    uint32 limit;
    uint64 base;
    union {
        struct {
            uint32 type : 4;
            uint32 desc : 1;
            uint32 dpl : 2;
            uint32 present : 1;
            uint32 : 4;
                     uint32 available : 1;
                     uint32 long_mode : 1;
                     uint32 operand_size : 1;
                     uint32 granularity : 1;
                     uint32 null : 1;
                     uint32 : 15;
        };
        uint32 ar;
    };
    uint32 ipad;
};
typedef struct segment_desc_t segment_desc_t;

struct vcpu_state_t {
    union {
        uint64 _regs[16];
        struct {
            union {
                struct { uint8 _al, _ah; };
                uint16 _ax; uint32 _eax; uint64 _rax;
            };
            union {
                struct { uint8 _cl, _ch; };
                uint16 _cx; uint32 _ecx; uint64 _rcx;
            };
            union {
                struct { uint8 _dl, _dh; };
                uint16 _dx; uint32 _edx; uint64 _rdx;
            };
            union {
                struct { uint8 _bl, _bh; };
                uint16 _bx; uint32 _ebx; uint64 _rbx;
            };
            union { uint16 _sp; uint32 _esp; uint64 _rsp; };
            union { uint16 _bp; uint32 _ebp; uint64 _rbp; };
            union { uint16 _si; uint32 _esi; uint64 _rsi; };
            union { uint16 _di; uint32 _edi; uint64 _rdi; };

            uint64 _r8;
            uint64 _r9;
            uint64 _r10;
            uint64 _r11;
            uint64 _r12;
            uint64 _r13;
            uint64 _r14;
            uint64 _r15;
        };
    };

    union { uint32 _eip; uint64 _rip; };
    union { uint32 _eflags; uint64 _rflags; };

    segment_desc_t _cs;
    segment_desc_t _ss;
    segment_desc_t _ds;
    segment_desc_t _es;
    segment_desc_t _fs;
    segment_desc_t _gs;
    segment_desc_t _ldt;
    segment_desc_t _tr;

    segment_desc_t _gdt;
    segment_desc_t _idt;

    uint64 _cr0;
    uint64 _cr2;
    uint64 _cr3;
    uint64 _cr4;

    uint64 _dr0;
    uint64 _dr1;
    uint64 _dr2;
    uint64 _dr3;
    uint64 _dr6;
    uint64 _dr7;
    uint64 _pde;

    uint32 _efer;

    uint32 _sysenter_cs;
    uint64 _sysenter_eip;
    uint64 _sysenter_esp;

    uint32 _activity_state;
    uint32 pad;
    interruptibility_state_t _interruptibility_state;
};

/*
 * Public exit-status enum.  Kept under the HAX_EXIT_* names so the rest of
 * NTVDM (which switches on these values) is portable across backends.  We
 * still emit HAX_EXIT_FAST_MMIO because the existing handler in monitor.c
 * uses an `iobuf` of type `struct hax_fastmmio` -- we synthesise that struct
 * locally from the WHV_MEMORY_ACCESS_CONTEXT.
 *
 * HAX_EXIT_COALESCED_MMIO is intentionally absent: WHP has no equivalent and
 * the monitor loop no longer dispatches it.  HAX_EXIT_NMI keeps its name for
 * source compatibility (it is the BOP / interrupt-hook path), even though
 * the underlying mechanism on WHP is an Exception exit.
 */
typedef enum exit_status {
    HAX_EXIT_IO = 1,
    HAX_EXIT_MMIO,
    HAX_EXIT_REALMODE,
    HAX_EXIT_INTERRUPT,
    HAX_EXIT_UNKNOWN,
    HAX_EXIT_HLT,
    HAX_EXIT_STATECHANGE,
    HAX_EXIT_PAUSED,
    HAX_EXIT_FAST_MMIO,
    HAX_EXIT_PAGEFAULT,
    HAX_EXIT_DEBUG,
    HAX_EXIT_NMI
} exit_status;

/*
 * Legacy VMX-exit identifiers, retained only because debug.c's name_vmx_exit()
 * uses them as diagnostic strings.  They are not used in the dispatch path.
 */
typedef enum exit_reason {
    VMX_EXIT_INT_EXCEPTION_NMI       =  0,
    VMX_EXIT_EXT_INTERRUPT           =  1,
    VMX_EXIT_TRIPLE_FAULT            =  2,
    VMX_EXIT_INIT_EVENT              =  3,
    VMX_EXIT_SIPI_EVENT              =  4,
    VMX_EXIT_SMI_IO_EVENT            =  5,
    VMX_EXIT_SMI_OTHER_EVENT         =  6,
    VMX_EXIT_PENDING_INTERRUPT       =  7,
    VMX_EXIT_PENDING_NMI             =  8,
    VMX_EXIT_TASK_SWITCH             =  9,
    VMX_EXIT_CPUID                   = 10,
    VMX_EXIT_GETSEC                  = 11,
    VMX_EXIT_HLT                     = 12,
    VMX_EXIT_INVD                    = 13,
    VMX_EXIT_INVLPG                  = 14,
    VMX_EXIT_RDPMC                   = 15,
    VMX_EXIT_RDTSC                   = 16,
    VMX_EXIT_RSM                     = 17,
    VMX_EXIT_VMCALL                  = 18,
    VMX_EXIT_VMCLEAR                 = 19,
    VMX_EXIT_VMLAUNCH                = 20,
    VMX_EXIT_VMPTRLD                 = 21,
    VMX_EXIT_VMPTRST                 = 22,
    VMX_EXIT_VMREAD                  = 23,
    VMX_EXIT_VMRESUME                = 24,
    VMX_EXIT_VMWRITE                 = 25,
    VMX_EXIT_VMXOFF                  = 26,
    VMX_EXIT_VMXON                   = 27,
    VMX_EXIT_CR_ACCESS               = 28,
    VMX_EXIT_DR_ACCESS               = 29,
    VMX_EXIT_IO                      = 30,
    VMX_EXIT_MSR_READ                = 31,
    VMX_EXIT_MSR_WRITE               = 32,
    VMX_EXIT_FAILED_VMENTER_GS       = 33,
    VMX_EXIT_FAILED_VMENTER_MSR      = 34,
    VMX_EXIT_MWAIT                   = 36,
    VMX_EXIT_MTF_EXIT                = 37,
    VMX_EXIT_MONITOR                 = 39,
    VMX_EXIT_PAUSE                   = 40,
    VMX_EXIT_MACHINE_CHECK           = 41,
    VMX_EXIT_TPR_BELOW_THRESHOLD     = 43,
    VMX_EXIT_APIC_ACCESS             = 44,
    VMX_EXIT_GDT_IDT_ACCESS          = 46,
    VMX_EXIT_LDT_TR_ACCESS           = 47,
    VMX_EXIT_EPT_VIOLATION           = 48,
    VMX_EXIT_EPT_MISCONFIG           = 49,
    VMX_EXIT_INVEPT                  = 50,
    VMX_EXIT_RDTSCP                  = 51,
    VMX_EXIT_VMX_TIMER_EXIT          = 52,
    VMX_EXIT_INVVPID                 = 53,
    VMX_EXIT_WBINVD                  = 54,
    VMX_EXIT_XSETBV                  = 55,
    VMX_EXIT_APIC_WRITE              = 56,
    VMX_EXIT_RDRAND                  = 57,
    VMX_EXIT_INVPCID                 = 58,
    VMX_EXIT_VMFUNC                  = 59,
    VMX_EXIT_ENCLS                   = 60,
    VMX_EXIT_RDSEED                  = 61,
    VMX_EXIT_XSAVES                  = 63,
    VMX_EXIT_XRSTORS                 = 64
} exit_reason;

union interruption_info_t {
    uint32_t raw;
    struct {
        uint32_t vector : 8;
        uint32_t type : 3;
        uint32_t deliver_error_code : 1;
        uint32_t nmi_unmasking : 1;
        uint32_t reserved : 18;
        uint32_t valid : 1;
    };
};
typedef union interruption_info_t interruption_info_t;

enum {
    VECTOR_DE = 0,
    VECTOR_DB = 1,
    VECTOR_NMI = 2,
    VECTOR_BP = 3,
    VECTOR_OF = 4,
    VECTOR_BR = 5,
    VECTOR_UD = 6,
    VECTOR_NM = 7,
    VECTOR_DF = 8,
    VECTOR_TS = 10,
    VECTOR_NP = 11,
    VECTOR_SS = 12,
    VECTOR_GP = 13,
    VECTOR_PF = 14,
    VECTOR_MF = 16,
    VECTOR_AC = 17,
    VECTOR_MC = 18,
    VECTOR_XM = 19,
    VECTOR_VE = 20
};

enum {
    INTERRUPT   = 0,
    NMI         = 2,
    EXCEPTION   = 3,
    SWINT       = 4,
    PRIV_TRAP   = 5,
    UNPRIV_TRAP = 6,
    OTHER       = 7
};

/*
 * Pluggable tunnel.  The HAXM driver delivered this via shared memory; we
 * fill it in user space from each WHvRunVirtualProcessor result and expose
 * the same pointer (`tunnel`, `iobuf`) to the rest of the code.  iobuf is
 * a per-CPU scratch buffer used by EventVdmIo / EventVdmBop.
 */
struct hax_tunnel {
    exit_reason _exit_reason;
    uint32_t pad0;
    exit_status _exit_status;
    uint32_t user_event_pending;
    int ready_for_interrupt_injection;
    int request_interrupt_window;

    union {
        struct {
#define HAX_EXIT_IO_IN  1
#define HAX_EXIT_IO_OUT 0
            uint8_t _direction;
            uint8_t _df;
            uint16_t _size;
            uint16_t _port;
            uint16_t _count;
            uint8_t _flags;
            uint8_t _pad0;
            uint16_t _pad1;
            uint32_t _pad2;
            vaddr_t _vaddr;
        } io;
        struct {
            paddr_t gla;
        } mmio;
        struct {
            paddr_t gpa;
#define HAX_PAGEFAULT_ACC_R  (1 << 0)
#define HAX_PAGEFAULT_ACC_W  (1 << 1)
#define HAX_PAGEFAULT_ACC_X  (1 << 2)
#define HAX_PAGEFAULT_PERM_R (1 << 4)
#define HAX_PAGEFAULT_PERM_W (1 << 5)
#define HAX_PAGEFAULT_PERM_X (1 << 6)
            uint32_t flags;
            uint32_t reserved1;
            uint64_t reserved2;
        } pagefault;
        struct {
            paddr_t dummy;
        } state;
        struct {
            uint64_t rip;
            uint64_t dr6;
            uint64_t dr7;
        } debug;
        struct {
            interruption_info_t exit_intr_info;
            uint32_t exit_exception_error_code;
        } nmi;
    };
    uint64_t apic_base;
};

#define HAX_RAM_INFO_ROM         0x01
#define HAX_RAM_INFO_FAULTISMMIO 0x10
#define HAX_RAM_INFO_INVALID     0x80

/*
 * Fast-MMIO record.  We keep the layout so the existing handlers in
 * monitor.c / sas.c can be reused; we synthesise it in user space from
 * WHV_MEMORY_ACCESS_CONTEXT + the bundled instruction emulator output.
 */
struct hax_fastmmio {
    paddr_t gpa;
    union {
        uint64_t value;
        paddr_t gpa2;
    };
    uint8_t size;
    uint8_t direction;
    uint16_t reg_index;
    uint32_t pad0;
    uint64_t _cr0;
    uint64_t _cr2;
    uint64_t _cr3;
    uint64_t _cr4;
};

#define HAX_RAM_PERM_NONE 0x0
#define HAX_RAM_PERM_RWX  0x7
#define HAX_RAM_PERM_MASK 0x7

#define HAX_DEBUG_ENABLE     (1 << 0)
#define HAX_DEBUG_STEP       (1 << 1)
#define HAX_DEBUG_USE_SW_BP  (1 << 2)
#define HAX_DEBUG_USE_HW_BP  (1 << 3)
#define HAX_DEBUG_MONSTEP    (1 << 4)

struct hax_debug_t {
    uint32_t control;
    uint32_t reserved;
    uint64_t dr[8];
};

#include <pshpack8.h>
struct fx_layout {
    uint16  fcw;
    uint16  fsw;
    uint8   ftw;
    uint8   res1;
    uint16  fop;
    union {
        struct { uint32 fip; uint16 fcs; uint16 res2; };
        uint64  fpu_ip;
    };
    union {
        struct { uint32 fdp; uint16 fds; uint16 res3; };
        uint64  fpu_dp;
    };
    uint32  mxcsr;
    uint32  mxcsr_mask;
    uint8   st_mm[8][16];
    uint8   mmx_1[8][16];
    uint8   mmx_2[8][16];
    uint8   pad[96];
};
#include <poppack.h>

#include <poppack.h>

/* -------- WHP backend handles, exposed for the rest of the interface -------- */

extern WHV_PARTITION_HANDLE       g_Partition;
extern UINT32                     g_VpIndex;

/* -------- Register-group bitmasks --------
 *
 * Every WHP register the backend pushes or pulls belongs to exactly one
 * group.  The cpu_simulate loop fetches only the groups a given exit
 * handler reads, and each setter flips a group bit in g_dirty_mask so the
 * next entry pushes only what actually changed since the last RunVp.
 */
#define REG_GROUP_GPRS    0x01u   /* RAX-R15 + RIP + RFLAGS */
#define REG_GROUP_SEGS    0x02u   /* CS, SS, DS, ES, FS, GS, LDTR, TR */
#define REG_GROUP_TABLES  0x04u   /* GDTR, IDTR */
#define REG_GROUP_CONTROL 0x08u   /* CR0, CR2, CR3, CR4, EFER */
#define REG_GROUP_DEBUG   0x10u   /* DR0-DR3, DR6, DR7 */
#define REG_GROUP_ALL     0x1Fu

/* -------- Backend entry points used by sas.c / int.c -------- */

void  hypervvm_panic(const char *fmt, ...);
void  hypervvm_init(void);
void  hyperv_cancel_run(void);  /* wake WHvRunVirtualProcessor from another thread */

/* TRUE if the WHP single pending-interruption slot is currently free.
 * DispatchInterrupts must check this before ica_intack() / inject -- if
 * we stomp an undelivered injection, the PIC marks the previous IRQ as
 * in-service forever (no EOI ever comes), deadlocking DOS keyboard
 * handlers like KB16.COM. */
BOOL  hyperv_pending_interruption_slot_free(void);
/* Both return S_OK on success or the raw HRESULT from the underlying
 * WHv*GpaRange call.  Callers should use SUCCEEDED(hr) / FAILED(hr) and
 * print hr on failure -- WHP APIs do not set GetLastError(). */
HRESULT hyperv_map_gpa_range(uint64_t gpa, uint64_t size, void *host_va,
                             WHV_MAP_GPA_RANGE_FLAGS flags);
HRESULT hyperv_unmap_gpa_range(uint64_t gpa, uint64_t size);
BOOL  hyperv_translate_gva(uint64_t gva, uint64_t *gpa);

/* Query the per-page dirty bitmap for a GPA range that was mapped with
 * WHvMapGpaRangeFlagTrackDirtyPages.  Each bit covers one 4 KB page;
 * bitmap_qwords is the size of `bitmap` in UINT64 elements.  The call
 * READS-and-CLEARS the WHP dirty state, so a subsequent invocation returns
 * only pages written since this one.  Returns FALSE if the range is not
 * tracked or the query failed; the caller should fall back to scanning. */
BOOL  hyperv_query_dirty(uint64_t gpa, uint64_t size,
                         UINT64 *bitmap, UINT32 bitmap_qwords);
BOOL  hyperv_inject_interrupt(uint32_t vector);

/* YODA tracing gate.  Returns FALSE if a YODA single-step session is
 * mid-IRQ (either DR0 armed but not yet fired, or we're stepping through
 * a handler whose IRET hasn't returned).  Callers should treat FALSE the
 * same way they treat a busy WHP injection slot: do NOT advance the PIC
 * state (no ica_intack), and retry on the next cpu_simulate iteration. */
BOOL  hyperv_yoda_inject_allowed(void);

/* Per-group marshalling.  hyperv_fetch() pulls the requested groups from
 * WHP into `state`; hyperv_flush() pushes only the dirty groups back and
 * clears them.  hyperv_mark_dirty() is used by the exit handlers when they
 * mutate `state` directly (the get/set helpers already call it for you). */
void  hyperv_fetch(uint32_t groups);
void  hyperv_flush(void);
void  hyperv_mark_dirty(uint32_t groups);

/* Convenience wrappers — push/pull EVERY group.  Used by the task-switch
 * handler (which touches all of state) and by diagnostic dumps.  Equivalent
 * to hyperv_mark_dirty(REG_GROUP_ALL); hyperv_flush(); and
 * hyperv_fetch(REG_GROUP_ALL) respectively. */
void  hyperv_load_state(void);
void  hyperv_store_state(void);

void  hyperv_set_exception_bitmap(uint64_t mask);
BOOL hyperv_yoda_inject_allowed(void);

/* The exit-loop tunnel pointer + io scratch buffer, populated by the WHP
 * exit handler in monitor.c.  Same names as in the HAXM port.
 */
extern struct hax_tunnel *tunnel;
extern PBYTE              iobuf;
