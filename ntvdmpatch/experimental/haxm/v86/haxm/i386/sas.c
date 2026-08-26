/*++

Copyright (c) 2019  leecher@dose.0wnz.at

Module Name:

    sas.c
Abstract:

    This module implements the memory mapping between HAXM and NTVDM

    This is a very slow implementation based on CCPU SAS. We may need to
	improve this further in future releases.
	We do face the following problems:
		1) Host memory could be mapped to Address 0 like in X86 Monitor
		   here, but we CANNOT map LIM EMS pages to it due to allocation
		   granularity constraints of 64bit Windows (only 64k allowed).
		   MEM_DOS_LIM flag for NtMapViewOfSection is not available here.
		   So even though we could map the EMS pages to the guest in 4k
		   granularity with HAXM, this doesn't help us on supervisor memory
		   access.
		   Doing so would require us to emulate a single MOV call for
		   every memory access function like it can be seen in ccpusas4.c
		   biosDoInst calls. I guess this would make the calls even slower
		   due to the amount of context switches needed for a single fetch.

		   However this poses another problem for us: As EMS writes 
		   need to be shadowed to the appropriate LIM page via 
		   LIM_b_write etc., LIM memory area has to be marked as MMIO
		   space so that supervisor has the chance to do the correct memory
		   writes. This slows us down a lot on EMS usage.

		   Additionally, when in protected mode, we also have to do a Page-
		   translation, which implies a mode switch to the HAXM driver too.

           hax_MapViewOfSection is an attempt to circumvent the problem
		   by circumventing the LIM-check in NtMapViewOfSection.
		   We need to check and develop this further.

		2) We depend on CIDVC video emulation, as there is no X86GFX 
		   available on X64. MMIO on the video area is too slow.
		   Therefore haxm_sync_vram gets used as a workaround, which is fine
		   in textmode, but not really working in planar video modes.
		   I think this cannot be solved properly as long as we don't have
		   the ability to define write-only EPT pages.

Author:

    leecher@dose.0wnz.at (leecher1337) 10 Feb 2019

Environment:

    User mode only

Revision History:
    
--*/

#define VDD_INTEG 1
#define EGG
#define C_VID
#include "monitorp.h"
#include "host_gen.h"
#include "insignia.h"
#include "trace.h"
#include "debug.h"
#include "sas.h"
#include "gmi.h"
#include "yoda.h"
#include "gfx_upd.h"
#include "sas4gen.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <cpu_vid.h>
#include <gvi.h>

void sas_connect_memory(
    IN sys_addr Low,
    IN sys_addr High,
    IN int Type
    );

#define SIXTYFOURK 0x10000L
#define ONEMEGA 0x100000L

/*
   Page Access Types.
 */
#define PG_R 0x0 /* Read */
#define PG_W 0x1 /* Write */

/*
   Page Accessor Types.
 */
#define PG_S 0x0 /* Supervisor */
#define PG_U 0x2 /* User */

#define checkAccess(addr)

void rom_init();
void rom_checksum();
void copyROM();

USHORT get_lim_backfill_segment(void);
BOOL   HoldEMMBackfillMemory(ULONG Address, ULONG Size);
void haxmvm_panic(const char *fmt, ...);

#if DBG
extern unsigned short get_emm_page_size(void);
extern unsigned short get_intel_page_size(void);
#endif

extern UTINY *host_sas_term();
extern UTINY *host_sas_init(sys_addr);
extern ULONG  getPG(VOID);
extern ULONG  getCPL(VOID);
extern ULONG  getVM (VOID);
extern ULONG  getPE (VOID);
extern ULONG  getLDT_BASE (VOID);

extern int    getModeType(VOID);
extern int    getVramPerPlane(void);   /* ega_prts.c: 1 once a subset Map-Mask (per-plane) was used */
extern int    getVramPlanarTrap(void); /* ega_prts.c: 1 in a planar EGA/VGA BIOS mode (0Dh..12h) */
extern volatile int g_vram_backing_stale; /* ega_prts.c: Read-Map-Select changed -> reprime flat-read backing */
extern BOOL   g_haxm_faultismmio_supported; /* monitor.c: driver has SET_RAM FAULTISMMIO fix */
extern void haxmvm_init();

extern HANDLE hVM, hVCPU;
extern IU32 gvi_pc_low_regen;

/* SYNC THESE DEFINITIONS WITH BASE\EMM.H, or sas_init will assert */
#define EMM_PAGE_SIZE		0x4000
#define INTEL_PAGE_SIZE 	0x1000


// Internal Data
PMEMTYPE MemType = NULL;

extern READ_POINTERS read_pointers;

// M variables used by video.lib

host_addr Start_of_M_area;       /* host addr (char *) of start of M */
IHPE Length_of_M_area;       /* sys addr (long) offset of end of M */

ULONG SasWrapMask = 0xfffff;

#ifndef MEM_WRITE_WATCH
#define MEM_WRITE_WATCH 0x200000
#endif

void	    (*temp_func) ();

#define TYPE_RANGE ((int)SAS_INACCESSIBLE)
#define write_b_write_ptrs( offset, func )  ( b_write_ptrs[(offset)] = (func) )
#define write_w_write_ptrs( offset, func )  ( w_write_ptrs[(offset)] = (func) )
#define write_b_page_ptrs( offset, func )   ( b_move_ptrs[(offset)] = b_fill_ptrs[(offset)] = (func) )
#define write_w_page_ptrs( offset, func )   ( w_move_ptrs[(offset)] = w_fill_ptrs[(offset)] = (func) )
#define init_b_write_ptrs( offset, func )   ( b_write_ptrs[(offset)] = (func) )
#define init_w_write_ptrs( offset, func )   ( w_write_ptrs[(offset)] = (func) )
#define init_b_page_ptrs( offset, func )    ( b_move_ptrs[(offset)] = b_fill_ptrs[(offset)] = (func) )
#define init_w_page_ptrs( offset, func )    ( w_move_ptrs[(offset)] = w_fill_ptrs[(offset)] = (func) )
#define read_b_write_ptrs( offset )     ( b_write_ptrs[(offset)] )
#define read_w_write_ptrs( offset )     ( w_write_ptrs[(offset)] )
#define read_b_page_ptrs( offset )      ( b_move_ptrs[(offset)] )
#define read_w_page_ptrs( offset )      ( w_move_ptrs[(offset)] )
#define read_b_move_ptrs( offset )      ( b_move_ptrs[(offset)] )
#define read_w_move_ptrs( offset )      ( w_move_ptrs[(offset)] )
#define read_b_fill_ptrs( offset )      ( b_fill_ptrs[(offset)] )
#define read_w_fill_ptrs( offset )      ( w_fill_ptrs[(offset)] )

/*
*   The main gmi data structures are defined here
*/
void (*(b_write_ptrs[TYPE_RANGE]))() ; /* byte write function */
void (*(w_write_ptrs[TYPE_RANGE]))() ; /* word write function */
void (*(b_fill_ptrs[TYPE_RANGE]))() ;  /* byte str fill func */
void (*(w_fill_ptrs[TYPE_RANGE]))() ;  /* word str fill func */
void (*(b_move_ptrs[TYPE_RANGE]))() ;  /* byte str write func */
void (*(w_move_ptrs[TYPE_RANGE]))() ;  /* word str write func */

// MEM_HANDLERS doesn't have DWORD interface yet, but C_VID has them
#ifdef C_VID
extern IU32 gvi_pc_low_regen;
extern IU32 read_dword_ev_glue (IU32 eaOff);
extern void write_dword_ev_glue (IU32 eaOff, IU32 eaVal);
extern void move_dword_fwd_ev_glue (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM);
extern void fill_dword_ev_glue (IU32 eaOff, IU32 eaVal, IU32 count);
#endif

void    gmi_define_mem(type,handlers)
mem_type type;
MEM_HANDLERS *handlers;
{
    int int_type = (int)(type);
    init_b_write_ptrs(int_type, (void(*)())(handlers->b_write));
    init_w_write_ptrs(int_type, (void(*)())(handlers->w_write));
    b_move_ptrs[int_type] = (void(*)())(handlers->b_move);
    w_move_ptrs[int_type] = (void(*)())(handlers->w_move);
    b_fill_ptrs[int_type] = (void(*)())(handlers->b_fill);
    w_fill_ptrs[int_type] = (void(*)())(handlers->w_fill);
}

LOCAL	BOOL	selectors_set = FALSE;
LOCAL	IU16	code_sel, data_sel;
LOCAL	char*	haxm_videocmp_copy = NULL;

/* local functions */
LOCAL enum SAS_MEM_TYPE readSelfMod (PHY_ADDR addr, DWORD typeSize);
LOCAL IU8  bios_read_byte   IPT1(LIN_ADDR, linAddr);
LOCAL IU16 bios_read_word   IPT1(LIN_ADDR, linAddr);
LOCAL IU32 bios_read_double IPT1(LIN_ADDR, linAddr);
LOCAL void bios_write_byte   IPT2(LIN_ADDR, linAddr, IU8, value);
LOCAL void bios_write_word   IPT2(LIN_ADDR, linAddr, IU16, value);
LOCAL void bios_write_double IPT2(LIN_ADDR, linAddr, IU32, value);
LOCAL IUM8 bios_read_accreq(void);

GLOBAL IBOOL xtrn2phy IPT3(LIN_ADDR, lin, IUM8, access_request, PHY_ADDR *, phy);


GLOBAL IU8 * 
getPtrToPhysAddrByte IFN1(PHY_ADDR, addr)
{
	return((IU8 *)((IHPE)Start_of_M_area + (IHPE)addr));
}

GLOBAL IU8 *
getPtrToLinAddrByte IFN1(PHY_ADDR, linAddr)
{
	PHY_ADDR phyAddr;

	if (!getPG())
		return(getPtrToPhysAddrByte((PHY_ADDR)linAddr));
	else if (xtrn2phy(linAddr, (IUM8)0, &phyAddr))
		return(getPtrToPhysAddrByte(phyAddr));
	else {
		return(getPtrToPhysAddrByte(0));	/* as good as anything! */
	}
}

GLOBAL IU8 *NtGetPtrToLinAddrByte IFN1(IU32, lin_addr)
{
	return getPtrToLinAddrByte(lin_addr);
}

/* Compatibility with SoftPC2.0 access name (used in video) */
GLOBAL IU8* get_byte_addr IFN1(PHY_ADDR, addr)
{
	return (getPtrToLinAddrByte(addr));
}

GLOBAL VOID haxm_alloc(
    IN sys_addr Low,
    IN sys_addr Size
)
{
	struct hax_alloc_ram_info alloc_ram = { 0 };
	DWORD bytes;

	alloc_ram.size = Size;
	alloc_ram.va = Low;
	if (!DeviceIoControl(hVM, HAX_VM_IOCTL_ALLOC_RAM, &alloc_ram, sizeof(alloc_ram), NULL, 0, &bytes, NULL))
	{
		//haxmvm_panic("HAXM: Error allocating RAM: size = %d, base = %08X, va = %08X, gle = %d", 
		//	Size, Start_of_M_area, Start_of_M_area + Low, GetLastError());
	}
}

GLOBAL VOID haxm_mprotect(
	IN sys_addr Addr,
	IN sys_addr Size,
	IN DWORD protection)
{
	struct hax_protect_ram_info info = { 0 };
	DWORD bytes;

	info.pa_start = Addr;
	info.size = Size;
	info.flags = protection;
	DeviceIoControl(hVM, HAX_VM_IOCTL_PROTECT_RAM, &info, sizeof(info), NULL, 0, &bytes, NULL);
}

/* This was just a test to see, if circumventing the 
 * NtMapViewOfSection MEM_DOS_LIM check helps in re-enabling this functionslity in
 * Winx64. Seems it is not possible, though, MiMapViewOfSection doesn't like it.
 * However, I found:
 *   https://blogs.msdn.microsoft.com/wsl/2016/05/23/pico-process-overview/
 *   "Windows normally manages the user-mode address space in 64KB chunks, but was 
 *    updated to allow management at single-page 4KB granularity for pico processes."
 *   Maybe we need to become a pico-process...? But it seems that Patch guard doesn't like that:
 *   http://www.alex-ionescu.com/publications/BlueHat/bluehat2016.pdf
 */
GLOBAL NTSTATUS
NTAPI
hax_MapViewOfSection(IN HANDLE SectionHandle,
	IN HANDLE ProcessHandle,
	IN OUT PVOID* BaseAddress,
	IN ULONG ZeroBits,
	IN ULONG CommitSize,
	IN OUT PLARGE_INTEGER SectionOffset OPTIONAL,
	IN OUT PULONG ViewSize,
	IN SECTION_INHERIT InheritDisposition,
	IN ULONG AllocationType,
	IN ULONG Protect)
{
	struct hax_mapviewofsection p;
	DWORD bytes;
	NTSTATUS Status;
	/* 32bit compatibility: */
	uint64_t _BaseAddress = *(ULONG*)BaseAddress;
	uint64_t _ViewSize = *ViewSize;

	p.SectionHandle = (uint64_t)SectionHandle;
	p.ProcessHandle = (uint64_t)ProcessHandle;
	p.BaseAddress = (uint64_t)&_BaseAddress;
	p.ZeroBits = ZeroBits;
	p.CommitSize = CommitSize;
	p.SectionOffset = (uint64_t)SectionOffset;
	p.ViewSize = (uint64_t)&_ViewSize;
	p.InheritDisposition = InheritDisposition;
	p.AllocationType = AllocationType;
	p.Protect = Protect;

	if (!DeviceIoControl(hVM, HAX_VCPU_IOCTL_MAP_SECTION, &p, sizeof(p), &Status, sizeof(Status), &bytes, NULL))
		return STATUS_UNSUCCESSFUL;

	*(ULONG*)BaseAddress = (ULONG)_BaseAddress;
	*ViewSize = (ULONG)_ViewSize;
	return Status;
}

#if VIDEO_STRATEGY == 1

/* Minimum gap between visible host blits (ms); ~15 ms ~= 66 Hz.  Decouples
 * the expensive host_graphics_tick blit from the per-exit VRAM sync (ported
 * from the WHP backend). */
#define VRAM_BLIT_INTERVAL_MS 15
static DWORD s_vram_last_blit    = 0;
static BOOL  s_vram_blit_pending = FALSE;


/* EPT dirty-bitmap fast path (ported from the WHP backend): ask the driver
 * (HAX_VM_IOCTL_QUERY_DIRTY via haxm_query_dirty) which VRAM pages the guest
 * wrote since the last sync, and scan ONLY those -- instead of the full-aperture
 * 0xFF/shadow compare on every page, every exit.
 *
 * DEFAULT 0 (disabled) -- MEASURED NET-NEGATIVE (perf-doomhang21): the tracker
 * works (scan 263ms->37ms, pages/call 15.6->2.0) BUT the per-query QUERY_DIRTY
 * ioctl costs ~228ms/s (INVEPT + EPT-tree walk) AND the per-query INVEPT flushes
 * the whole guest EPT TLB thousands/s -> guest runs at ~half speed (exits
 * 7400->3819/s) -> net SLOWER.  Re-arming dirty tracking needs INVEPT, which is
 * whole-context-only, so at DOOM's per-Map-Mask query rate it thrashes the guest
 * (true for the EPT-A/D path too -- it also INVEPTs per query).  The driver
 * write-protect tracker (memory.c hax_vm_dirty_log_*) stays correct+dormant for a
 * lower-query-rate use or a host where INVEPT is cheap; the plain (QWORD) scan
 * wins here.  Set 1 to re-enable for experiments (e.g. amortised re-arm). */
#ifndef VRAM_USE_HAXM_DIRTY_BITMAP
#define VRAM_USE_HAXM_DIRTY_BITMAP 0
#endif
/* Force a full scan every Nth sync as a safety net against a missed dirty re-arm
 * (the query is best-effort -- see ept_tree_query_clear_dirty). */
#define VRAM_BITMAP_FULLSCAN_EVERY 16
#if VRAM_USE_HAXM_DIRTY_BITMAP
extern BOOL haxm_query_dirty(uint64_t gpa, uint64_t size,
                             uint64_t *bitmap, DWORD qwords);   /* monitor.c */
static UINT64 *vram_dirty_bitmap        = NULL;
static DWORD   vram_dirty_bitmap_qwords = 0;
static BOOL    vram_dirty_primed        = FALSE;

static BOOL vram_resize_bitmap(DWORD vram_size)
{
	DWORD pages  = (vram_size + 0xFFF) >> 12;
	DWORD qwords = (pages + 63) >> 6;
	if (qwords == vram_dirty_bitmap_qwords && vram_dirty_bitmap)
		return TRUE;
	if (vram_dirty_bitmap)
		free(vram_dirty_bitmap);
	vram_dirty_bitmap = (UINT64 *)calloc(qwords, sizeof(UINT64));
	if (!vram_dirty_bitmap) {
		vram_dirty_bitmap_qwords = 0;
		return FALSE;
	}
	vram_dirty_bitmap_qwords = qwords;
	return TRUE;
}
#endif

#define SP_BLIT()  host_graphics_tick()

/* ---- Fast VRAM scan (find the first changed byte in a page) -----------------
 * The sync's dominant cost is skipping the all-0xFF (self-heal) / unchanged
 * (shadow-diff) pages that make up ~99% of the aperture.  SSE2 scans 16 bytes
 * per compare (PCMPEQB + PMOVMSKB) -- ~4x the 8-byte QWORD path -- then a short
 * byte loop pinpoints the exact first change.
 *
 * SSE2 is guaranteed on every VT-x+EPT (HAXM) host, but the NTVDM SDK (VC7.1)
 * has no <emmintrin.h>, so the SSE2 ops are emitted as raw opcode bytes via
 * __asm _emit (loop control uses normal mnemonics).  movdqu is UNALIGNED so the
 * regen pointer need not be 16-aligned.  Set VRAM_SCAN_SSE2 0 for the portable
 * QWORD fallback (still 2x the original DWORD scan). */
#ifndef VRAM_SCAN_SSE2
#define VRAM_SCAN_SSE2 1
#endif

/* First offset in [p, p+len) where p[j] != 0xFF, or len if all 0xFF. */
static DWORD scan_first_nonFF(const BYTE *p, DWORD len)
{
#if VRAM_SCAN_SSE2
	DWORD result;
	__asm {
		push    esi
		mov     esi, p
		mov     edx, len
		xor     ecx, ecx              ; ecx = j
		_emit 0x66                    ; pcmpeqb xmm1, xmm1  (xmm1 = all 0xFF)
		_emit 0x0F
		_emit 0x74
		_emit 0xC9
	nff_sse:
		lea     eax, [ecx+16]
		cmp     eax, edx              ; j+16 > len ?
		ja      nff_tail
		_emit 0xF3                    ; movdqu xmm0, [esi+ecx]
		_emit 0x0F
		_emit 0x6F
		_emit 0x04
		_emit 0x0E
		_emit 0x66                    ; pcmpeqb xmm0, xmm1
		_emit 0x0F
		_emit 0x74
		_emit 0xC1
		_emit 0x66                    ; pmovmskb eax, xmm0
		_emit 0x0F
		_emit 0xD7
		_emit 0xC0
		cmp     eax, 0FFFFh           ; all 16 bytes == 0xFF ?
		jne     nff_tail
		add     ecx, 16
		jmp     nff_sse
	nff_tail:
		cmp     ecx, edx
		jae     nff_none
	nff_blp:
		mov     al, [esi+ecx]
		cmp     al, 0FFh
		jne     nff_done
		inc     ecx
		cmp     ecx, edx
		jb      nff_blp
	nff_none:
		mov     ecx, edx              ; return len (all 0xFF)
	nff_done:
		mov     result, ecx
		pop     esi
	}
	return result;
#else
	DWORD j = 0;
	const uint64_t *q = (const uint64_t *)p;
	for (; j + 8 <= len; j += 8)
		if (q[j >> 3] != 0xFFFFFFFFFFFFFFFFULL)
			break;
	for (; j < len; j++)
		if (p[j] != 0xFF)
			return j;
	return len;
#endif
}

/* First offset in [0, len) where a[j] != b[j], or len if equal. */
static DWORD scan_first_diff(const BYTE *a, const BYTE *b, DWORD len)
{
#if VRAM_SCAN_SSE2
	DWORD result;
	__asm {
		push    esi
		push    edi
		mov     esi, a
		mov     edi, b
		mov     edx, len
		xor     ecx, ecx              ; ecx = j
	dif_sse:
		lea     eax, [ecx+16]
		cmp     eax, edx
		ja      dif_tail
		_emit 0xF3                    ; movdqu xmm0, [esi+ecx]
		_emit 0x0F
		_emit 0x6F
		_emit 0x04
		_emit 0x0E
		_emit 0xF3                    ; movdqu xmm1, [edi+ecx]
		_emit 0x0F
		_emit 0x6F
		_emit 0x0C
		_emit 0x0F
		_emit 0x66                    ; pcmpeqb xmm0, xmm1
		_emit 0x0F
		_emit 0x74
		_emit 0xC1
		_emit 0x66                    ; pmovmskb eax, xmm0
		_emit 0x0F
		_emit 0xD7
		_emit 0xC0
		cmp     eax, 0FFFFh           ; all 16 bytes equal ?
		jne     dif_tail
		add     ecx, 16
		jmp     dif_sse
	dif_tail:
		cmp     ecx, edx
		jae     dif_none
	dif_blp:
		mov     al, [esi+ecx]
		mov     ah, [edi+ecx]         ; avoid 'byte ptr' -- 'byte' is a C typedef
		cmp     al, ah
		jne     dif_done
		inc     ecx
		cmp     ecx, edx
		jb      dif_blp
	dif_none:
		mov     ecx, edx
	dif_done:
		mov     result, ecx
		pop     edi
		pop     esi
	}
	return result;
#else
	DWORD j = 0;
	const uint64_t *qa = (const uint64_t *)a, *qb = (const uint64_t *)b;
	for (; j + 8 <= len; j += 8)
		if (qa[j >> 3] != qb[j >> 3])
			break;
	for (; j < len; j++)
		if (a[j] != b[j])
			return j;
	return len;
#endif
}

/* ---- A0000 planar-EGA per-access trap (ported from the WHP backend) ------
 * The flat regen aperture cannot reproduce EGA latched / set-reset / ALU
 * writes (Commander Keen): a single linear address fans out to four planes.
 * For planar EGA/VGA BIOS modes (0Dh..12h) we mark A0000 as
 * HAX_RAM_INFO_FAULTISMMIO so EVERY guest access faults: the HAXM driver's
 * in-kernel emulator decodes the instruction, hands us the write value (or
 * round-trips the read), and we route it through CVIDC write_b / read_b -- the
 * latch state lives there, exactly as on real hardware.  Linear/text modes
 * (mode 13h chain-4, DOOM, text) keep the fast flat dirty-diff aperture.
 *
 * Driver-verified (haxm/core): FAULTISMMIO never installs the EPT entry
 * (ept2.c) so it traps per-access (no batching); the kernel provides
 * hft->value for writes and writes the userspace value back for reads
 * (vcpu.c vcpu_write_memory / vcpu_read_memory_post).  FAULTISMMIO requires a
 * valid backing va (only INVALID demands va==0), so we pass the same regen va
 * either way -- the only difference between trapped and flat is ram.flags.
 *
 * WHP analog: vram_apply_mapping().  HAXM is simpler -- no decoder, no unmap,
 * just a flag flip.  Set HAXM_EGA_TRAP 0 to disable (flat-only, Keen garbled). */
#ifndef HAXM_EGA_TRAP
#define HAXM_EGA_TRAP 1   /* planar-EGA A0000 per-write trap (needed for Keen4 etc.).  NOTE
                           * (project_wpsnt_whp_derail): with the trap ON, wpsnt mode 0x12
                           * shows vertical stripes because write_b faithfully renders the
                           * guest's mm=F displayed pass -- the good per-plane image is
                           * composed to the WRONG buffer (off 0 vs displayed off 0xA000) due
                           * to the accelerated-exec timing divergence in the guest's
                           * double-buffer selector ([39e4] via [51f2]/[39de]).  A/B-verified:
                           * HAXM_EGA_TRAP 0 -> flat-only -> garbled-but-not-striped. */
#endif

/* HAXM_VRAM_FLATREAD: serve guest VRAM *reads* from the flat backing via a
 * read-only (RX) EPT entry (no VM exit) and trap only *writes* through CVIDC
 * write_b.  Halves the per-access VM exits for EGA read-modify-write drawing
 * (Lemmings-class games, where reads:writes are ~1:1) -- the per-access trap is
 * exit-bound, so this is the main throughput lever in high-exit-cost (nested)
 * hosts.  REQUIRES the matching driver change (ept2.c routes a write to a
 * read-only FAULTISMMIO page into MMIO emulation instead of -EACCES-skipping).
 * The aperture is mapped FAULTISMMIO|ROM; reads no longer fault, so sas_PW8
 * loads the EGA latch (read_b) before each write and refreshes the flat backing
 * with the read-map plane byte after, and a Read-Map-Select change re-syncs the
 * backing.  Default OFF -- opt-in until validated; full FAULTISMMIO stays the
 * correct path for cross-address latch copies (Commander Keen). */
#ifndef HAXM_VRAM_FLATREAD
#define HAXM_VRAM_FLATREAD 0
#endif

static BOOL     s_vram_trapped  = FALSE;
static sys_addr s_vram_map_lo   = 0;
static DWORD    s_vram_map_size = 0;

/* Runtime video strategy: 1 = fast flat aperture + selective trap (default), 3 =
 * full-CVIDC (FAULTISMMIO-trap the WHOLE aperture, every touch -> CVIDC, like
 * CCPU).  Switched 1->3 (sticky) by video_strategy_autodetect() only when the
 * user opts in via the NTVDM_CVIDC env var; normal DOS keeps strategy 1.
 * Mirrors hyperv/x86/sas.c.  UPDATE57. */
int             g_video_strategy = 1;

static BOOL vram_is_trapped(void) { return s_vram_trapped; }

/* Issue HAX_VM_IOCTL_SET_RAM for the regen aperture: FAULTISMMIO (per-access
 * trap) when `trap`, else flat R/W RAM.  Same pa/size/va either way. */
static void vram_set_ram(sys_addr lo, DWORD size, BOOL trap)
{
    struct hax_set_ram_info ram = { 0 };
    DWORD bytes;
    ram.pa_start = lo;
    ram.size     = size;
    ram.va       = (uint64_t)(ULONG)Start_of_M_area + lo;
    ram.flags    = trap ? HAX_RAM_INFO_FAULTISMMIO : 0;
#if HAXM_VRAM_FLATREAD
    /* Read-only aperture: reads served flat (no exit), writes fault -> MMIO. */
    if (trap) ram.flags |= HAX_RAM_INFO_ROM;
#endif
    if (!DeviceIoControl(hVM, HAX_VM_IOCTL_SET_RAM, &ram, sizeof(ram),
                         NULL, 0, &bytes, NULL))
        haxmvm_panic("vram_set_ram SET_RAM (pa=%08X size=%08X trap=%d gle=%d)",
                     (ULONG)lo, (ULONG)size, trap, GetLastError());
}

/* Apply the A0000 mapping: flip flat<->FAULTISMMIO per the current BIOS mode.
 *
 * `force` MUST be set when this is called right after a SAS_VIDEO connect,
 * because every EGA mode-set first does sas_disconnect_memory() == a generic
 * SAS_INACCESSIBLE SET_RAM (va=0) on the same A0000 range.  That disconnect
 * changes the driver mapping behind our back without touching our cache, so a
 * purely-idempotent check would no-op the reconnect and leave A0000 UNMAPPED
 * (va=0) -> all guest writes dropped -> black screen.  Forcing the reconnect
 * to re-issue SET_RAM restores the mapping.  The per-exit re-eval path passes
 * force=FALSE so it stays cheap (only re-issues when want_trap changes). */
static void vram_apply_mapping(sys_addr lo, DWORD size, BOOL force)
{
    /* g_haxm_faultismmio_supported (monitor.c) is FALSE on drivers that lack
     * the SET_RAM FAULTISMMIO fix -- asking those to trap BSODs the machine,
     * so we stay flat (Keen garbled but safe) and warn during init instead. */
    BOOL want_trap = HAXM_EGA_TRAP && g_haxm_faultismmio_supported &&
                     (getVramPlanarTrap() || g_video_strategy == 3);

    if (size == 0) return;
    if (!force && lo == s_vram_map_lo && size == s_vram_map_size &&
        want_trap == s_vram_trapped)
        return;

    /* Split the B-region when trapping the A0000 planar aperture:
     *   A0000-B7FFF : planar TRAP.  Mode 0x12 with screen_start 0xA000 spans plane
     *                 offset 0xA000..0x13600 -- it WRAPS past 64K, so the bottom
     *                 ~172 lines are written to gpa 0xB0000-0xB3600 and must land
     *                 in CVIDC plane 0-0x3600 (same wrap the interpreter does).
     *                 Mapping B0000+ flat diverts those writes to backing RAM so
     *                 the plane buffer never gets them -> bottom of screen renders
     *                 stale garbage (RENDERPG: CCPU writes plane pages 0-3, WHP was
     *                 missing 2-3).  project_wpsnt_whp_derail UPDATE35.
     *   B8000-BFFFF : flat R/W RAM.  The guest uses B8000 as flat text/scratch RAM
     *                 (WPS's B800 writability probe: write 0x55, read back, branch).
     *                 FAULTISMMIO-trapping it routes through planar write_b (wrap to
     *                 plane 0x8000) where the write doesn't round-trip -> guest reads
     *                 open-bus 0xFF, mis-detects, wrong render path -> garble + hang
     *                 (correct on CCPU which keeps B8000 flat).  Never the planar
     *                 display, so flat is safe.  project_wpsnt_whp_derail UPDATE31. */
    if (want_trap && g_video_strategy == 3) {
        /* full-CVIDC (auto-detected spdos): FAULTISMMIO-trap the WHOLE aperture
         * INCLUDING B8000-BFFFF, so the text buffer AND the custom planar Chinese
         * font both reach CVIDC and SP_BLIT renders them (like WHP strategy 3).
         * The B800 write-probe round-trips through CVIDC in this all-trapped mode.
         * project_wpsnt_whp_derail UPDATE52. */
        vram_set_ram(lo, size, TRUE);
    } else {
        sys_addr hi      = lo + size;
        sys_addr flat_lo = lo;
        /* In a planar mode the guest addresses the whole 64K-plane WRAP window
         * A0000-B7FFF (B-region offsets alias plane offset 0..0x7FFF, e.g. spdos's
         * Chinese glyph render writes gpa 0xB2D41 -> plane 0x2D41).  The BIOS
         * memory_map can shrink the decode aperture to 64K (A0000-AFFFF), but the
         * guest still writes the B-region relying on the wrap.  So always manage
         * the full A0000-B7FFF window: arm traps B0000-B7FFF, disarm restores it
         * flat.  Without this the 64K-aperture window leaves B0000-B7FFF flat and
         * the planar glyph writes bypass CVIDC (invisible text).  Mirrors
         * hyperv/x86/sas.c.  project_wpsnt_whp_derail UPDATE41. */
        if (lo <= 0xA0000 && hi < 0xB8000 && (want_trap || s_vram_trapped))
            hi = (sys_addr)0xB8000;
        if (want_trap && lo < 0xB8000) {
            flat_lo = (hi < 0xB8000) ? hi : (sys_addr)0xB8000;
            if (flat_lo > lo)
                vram_set_ram(lo, (DWORD)(flat_lo - lo), TRUE);   /* A0000..B7FFF: planar trap */
        }
        if (hi > flat_lo)
            vram_set_ram(flat_lo, (DWORD)(hi - flat_lo), FALSE); /* B8000..BFFFF (or all): flat */
    }
    s_vram_map_lo   = lo;
    s_vram_map_size = size;
    s_vram_trapped  = want_trap;
    if (want_trap)
        g_vram_backing_stale = 1;   /* (re)prime the flat-read backing on connect */
}

/* Re-evaluate the trap for the recorded region (catches vd_video_mode updates
 * that lag the SAS_VIDEO connect).  No-op until video memory is connected.
 * force=FALSE -- this runs on every exit, so it must stay idempotent/cheap. */
static void vram_reeval_trap(void)
{
    if (s_vram_map_size)
        vram_apply_mapping(s_vram_map_lo, s_vram_map_size, FALSE);
}

/* Enable full-CVIDC (strategy 3) for a planar-font TSR such as spdos/WPS.
 *
 * MANUAL opt-in via the host env var NTVDM_CVIDC (`set NTVDM_CVIDC=1` before
 * launching).  We do NOT auto-detect from video state: spdos's planar-text render
 * and the BIOS boot-time font load are indistinguishable in the live registers,
 * so any state heuristic false-positives on every machine's boot font load and
 * flips innocent apps (DOOM/BLOOD) into strategy-3 trapping (which crashes them).
 * The flag scopes strategy 3 to opted-in sessions only.  Env var read once and
 * cached.  On flip: flush flat screen -> CVIDC, then force=TRUE re-map so
 * vram_apply_mapping FAULTISMMIO-traps the whole aperture.  Mirrors hyperv.
 * project_wpsnt_whp_derail UPDATE57 (replaces the UPDATE52 auto-detect). */
extern GLOBAL VOID haxm_sync_vram(VOID);   /* defined below */
void video_strategy_autodetect(void)
{
    static int s_cvidc_opt_in = -1;                /* -1 = not yet read */

    if (g_video_strategy != 1)
        return;                                    /* already switched */

    if (s_cvidc_opt_in < 0) {
        char buf[8];
        DWORD n = GetEnvironmentVariableA("NTVDM_CVIDC", buf, sizeof(buf));
        s_cvidc_opt_in = (n > 0 && n < sizeof(buf) && buf[0] != '0') ? 1 : 0;
    }
    if (!s_cvidc_opt_in)
        return;                                    /* not opted in -- stay on strategy 1 */

    haxm_sync_vram();                              /* flush current flat screen -> CVIDC (no loss) */
    g_video_strategy = 3;
    if (s_vram_map_size)
        vram_apply_mapping(s_vram_map_lo, s_vram_map_size, TRUE); /* re-trap whole aperture */
}

#if HAXM_VRAM_FLATREAD
/* Re-derive the flat backing (what the read-only PTE serves to guest reads)
 * from CVIDC for the whole connected aperture: backing[i] = read_b(addr) under
 * the current Read-Map-Select.  Cost = one in-process read_b per byte; only run
 * on connect/mode-set and on a Read-Map-Select change (both infrequent), NOT per
 * write -- so it does not reintroduce the per-access exit cost. */
static void haxm_vram_backing_refresh(void)
{
    IU32 base = gvi_pc_low_regen, top = gvi_pc_high_regen, a;
    if (!s_vram_trapped || top < base) return;
    for (a = base; a <= top; a++)
        (*(IU8 *) getPtrToPhysAddrByte(a)) = sas_PR8(a);
    g_vram_backing_stale = 0;
}
#endif

/* Syncs our video RAM copy into the emulated devices.
   See comment in sas_connect_memory for details on why this
   has to be used

   Per-graphics-mode strategy (ported from the WHP backend):
     - PER-PLANE renderers (DOOM Mode Y, subset Map-Masks) use the destructive
       0xFF self-heal: adjacent same-colour columns land in different planes so
       a shadow-diff would skip them and leave a plane stale ("rain").  These
       games don't read VRAM back, so wiping it to 0xFF is harmless.
     - ALL-PLANE writers (SKYROADS, Map-Mask 0x0F) use the non-destructive
       shadow-diff so their VRAM read-back survives (the 0xFF memset would feed
       them 0xFF -> the magenta progress-bar "bleed").
   getVramPerPlane() (shared CVIDC detector in ega_prts.c/vga_prts.c) selects.
*/
GLOBAL VOID haxm_sync_vram(VOID)
{
	DWORD dwSize, dwModeType, i, j;
	PDWORD pdwSrc, pdwDst;
	BOOL any_changed = FALSE;
	int  use_selfheal;
#if VRAM_USE_HAXM_DIRTY_BITMAP
	BOOL have_bitmap = FALSE;
#endif

	dwSize = gvi_pc_high_regen-gvi_pc_low_regen;
	dwModeType = getModeType();
	use_selfheal = (dwModeType != TEXT) && getVramPerPlane();

	/* Re-evaluate the A0000 trap for the current BIOS mode.  When trapped
	 * (planar EGA/VGA) the aperture is FAULTISMMIO -- every guest write already
	 * went through CVIDC write_b live, so there is nothing to diff-sync; just
	 * keep the host surface painted (rate-limited).  Linear/text modes fall
	 * through to the flat dirty-diff below. */
	vram_reeval_trap();
	if (vram_is_trapped())
	{
		DWORD now = GetTickCount();
#if HAXM_VRAM_FLATREAD
		/* Re-derive the flat backing after a Read-Map-Select change (or the
		 * initial connect) so guest reads from the read-only PTE see real data. */
		if (g_vram_backing_stale)
			haxm_vram_backing_refresh();
#endif
		if ((now - s_vram_last_blit) >= VRAM_BLIT_INTERVAL_MS)
		{
			s_vram_last_blit = now;
			SP_BLIT();
		}
		return;
	}

#if VRAM_USE_HAXM_DIRTY_BITMAP
	/* Ask the driver which VRAM pages the guest actually wrote (EPT dirty bits,
	 * read-and-cleared).  Then scan only those pages below instead of the full
	 * 0xFF/shadow compare over the whole aperture every exit.  Force a full scan
	 * on the first sync (baseline not yet primed) and every Nth sync (safety net
	 * against a missed dirty re-arm); on those rounds we skip the query so the
	 * dirty bits accumulate and are picked up next time. */
	{
		static DWORD vram_sync_count = 0;
		if (vram_resize_bitmap(dwSize + 1))
		{
			BOOL force_full = !vram_dirty_primed ||
			    (vram_sync_count % VRAM_BITMAP_FULLSCAN_EVERY) == 0;
			vram_sync_count++;
			vram_dirty_primed = TRUE;
			if (!force_full)
				have_bitmap = haxm_query_dirty((uint64_t)gvi_pc_low_regen,
				                               (uint64_t)dwSize + 1,
				                               vram_dirty_bitmap,
				                               vram_dirty_bitmap_qwords);
		}
	}
#endif

	for (i=0; i<(dwSize+1); i+=0x1000)
	{
		BOOL page_changed = FALSE;
#if VRAM_USE_HAXM_DIRTY_BITMAP
		/* dirty-bitmap fast path: skip pages the guest didn't write */
		if (have_bitmap)
		{
			DWORD page = i >> 12;
			if (!((vram_dirty_bitmap[page >> 6] >> (page & 63)) & 1ULL))
				continue;
		}
#endif

		/* We compare our buffer with the last copy and only update changed bytes.
		 * We are a 32bit process, so first compare DWORDs and if there is a difference, 
		 * go down to byte comparison mode.
		 * As we know that it is VRAM, we don't go through sas-function on every byte 
		 * but call CVIDC directly.
		 */
		pdwSrc = (PDWORD)(haxm_videocmp_copy + i);
		pdwDst = (PDWORD)getPtrToPhysAddrByte(gvi_pc_low_regen + i);

		if (dwModeType == TEXT)
		{
			temp_func = read_w_write_ptrs(SAS_VIDEO);
			for (j=0; j<0x1000/sizeof(DWORD); j++)
				if (pdwSrc[j] != pdwDst[j])
				{
					PWORD pwSrc=(PWORD)pdwSrc, pwDst=(PWORD)pdwDst;
					page_changed = TRUE;
					for (j*=(sizeof(DWORD)/sizeof(WORD)); j<0x1000/sizeof(WORD); j++)
					{
						if (pwSrc[j]!=pwDst[j])
							(*temp_func) ((PBYTE)gvi_pc_low_regen + i + j*sizeof(WORD), pwDst[j]);
					}
					break;
				}
		}
		else if (use_selfheal) /* per-plane (DOOM): 0xFF consumed-marker self-heal */
		{
			/* Clean (untouched) pages are all 0xFF and dominate the scan (~99%);
			 * scan_first_nonFF skips them 16 bytes/compare (SSE2) to the first
			 * change, then the byte loop writes the changed bytes to end-of-page. */
			PBYTE pbDst = (PBYTE)pdwDst;
			DWORD start = scan_first_nonFF(pbDst, 0x1000);
			if (start < 0x1000)
			{
				page_changed = TRUE;
				temp_func = read_b_write_ptrs(SAS_VIDEO);
				for (j=start; j<0x1000; j++)
					if (0xFF!=pbDst[j])
						(*temp_func) ((PBYTE)gvi_pc_low_regen + i + j, pbDst[j]);
			}
		}
		else /* all-plane (SKYROADS): non-destructive shadow-diff (keep VRAM) */
		{
			PBYTE pbSrc = (PBYTE)pdwSrc, pbDst = (PBYTE)pdwDst;
			DWORD start = scan_first_diff(pbSrc, pbDst, 0x1000);
			if (start < 0x1000)
			{
				page_changed = TRUE;
				temp_func = read_b_write_ptrs(SAS_VIDEO);
				for (j=start; j<0x1000; j++)
					if (pbSrc[j]!=pbDst[j])
						(*temp_func) ((PBYTE)gvi_pc_low_regen + i + j, pbDst[j]);
			}
		}

		if (page_changed)
		{
			any_changed = TRUE;
			/* Refresh baseline: self-heal resets VRAM to 0xFF (destructive);
			 * text & shadow-diff refresh the shadow copy (non-destructive). */
			if (dwModeType != TEXT && use_selfheal)
				memset(pdwDst, 0xFF, 0x1000);
			else
				memcpy(pdwSrc, pdwDst, 0x1000);
		}
	}

	/* Rate-limit the (expensive) host blit to ~display refresh -- the sync
	 * runs on every exit; calling host_graphics_tick each changed page was a
	 * big per-exit cost (ported from the WHP backend). */
	if (any_changed)
	{
		DWORD now = GetTickCount();
		s_vram_blit_pending = TRUE;
		if ((now - s_vram_last_blit) >= VRAM_BLIT_INTERVAL_MS)
		{
			s_vram_last_blit    = now;
			s_vram_blit_pending = FALSE;
			SP_BLIT();
		}
	}
	else if (s_vram_blit_pending)
	{
		DWORD now = GetTickCount();
		if ((now - s_vram_last_blit) >= VRAM_BLIT_INTERVAL_MS)
		{
			s_vram_last_blit    = now;
			s_vram_blit_pending = FALSE;
			SP_BLIT();
		}
	}
}
#endif /* VIDEO_STRATEGY == 1 */


#if VIDEO_STRATEGY == 2
/* Bitmap, which specifies the pages that are currently activated for sychronizing 
 * 1 = gvi_pc_low_regen+0x1000, 2=gvi_pc_low_regen+0x2000, 4=gvi_pc_low_regen+0x3000, ...
 * This gets acticated and assigned on MMIO request.
 */
static DWORD video_shadow_bmp = 0;
/* This contains the number of times no change was detected for the specified page.
 * If this exceeds a certain amount defined below, page gets reset back (unmapped)
 * so that checking it needs to get triggered again by MMIO
 */
static BYTE video_shadow_unchanged[32] = {0};
/* The number of MMIO-Hits for a page. If it's above given threshold in haxm_vram_mmio,
 * memory gets connected and fast haxm_sync_vram_test gets activated
 */
static BYTE video_shadow_mmios[32] = {0};

GLOBAL VOID haxm_vram_mmio(struct hax_fastmmio *hft)
{
	/* In order to not generate overhead on read-write combinations, we have
	 * a threshold of 2 MMIO writes before we turn on memory buffer
	 */
	paddr_t gpa = hft->gpa & 0xFFFFF000;
	BYTE idx = (gpa - gvi_pc_low_regen)/0x1000;

	if (hft->direction == 0)
	{
		// Reset trigger counters on read
		memset(video_shadow_mmios, 0, sizeof(video_shadow_mmios));
	}
	else
	{
		// Write to shadow buffer
		PBYTE *pShadow = getPtrToPhysAddrByte(hft->gpa);
		memcpy(pShadow, &hft->value, hft->size);

		// Count up on write
		video_shadow_mmios[idx]++;
		if (video_shadow_mmios[idx] > 1)
		{
			struct hax_set_ram_info ram = { 0 };
			DWORD bytes;
			char szDbg[256];

			// Threshold exceeded, connect memory and let the data flow
			ram.pa_start = gpa;
			ram.size = 0x1000;
			ram.va = (uint64_t)pShadow & 0xFFFFF000;
			DeviceIoControl(hVM, HAX_VM_IOCTL_SET_RAM, &ram, sizeof(ram), NULL, 0, &bytes, NULL);

			video_shadow_mmios[idx] = 0;
			video_shadow_bmp |= (1<<idx);
			sprintf(szDbg, "Activated MMIO write combining on %08X", gpa);
			OutputDebugString(szDbg);
		}
	}
}

GLOBAL VOID haxm_sync_vram(VOID)
{
	PWORD pwSrc, pwDst;
	DWORD i,j,mask;
	BOOL bChanged;

	// Gets activated by MMIO write trigger
	if (!video_shadow_bmp) return;
	temp_func = read_w_write_ptrs(SAS_VIDEO);

	for (i=0, mask=1; i<32; i++, mask<<=1)
	{
		if (!(video_shadow_bmp & mask)) continue;

		pwSrc = (PWORD)(haxm_videocmp_copy + (i * 0x1000));
		pwDst = (PWORD)getPtrToPhysAddrByte(gvi_pc_low_regen + (i * 0x1000));

		for (j=0, bChanged=FALSE; j<0x1000/sizeof(WORD); j++)
			if (pwSrc[j] != pwDst[j])
			{
				bChanged = TRUE;
				(*temp_func) ((PBYTE)gvi_pc_low_regen + i + j*sizeof(WORD), pwDst[j]);
			}
		if (!bChanged)
		{
			/* The page didn't change, so unlink it from bitmap so that MMIO can trigger updating
			 * again, if needed
			 */
			video_shadow_unchanged[i]++;
			if (video_shadow_unchanged[i] > 32)		// <-- Adjust accordingly
			{
				struct hax_set_ram_info ram = { 0 };
				DWORD bytes;
				char szDbg[256];

				video_shadow_unchanged[i] = 0;
				video_shadow_bmp &= ~mask;

				ram.pa_start = gvi_pc_low_regen + (i*0x1000);
				ram.size = 0x1000;
				ram.va = 0;
				ram.flags = HAX_RAM_INFO_INVALID;
				DeviceIoControl(hVM, HAX_VM_IOCTL_SET_RAM, &ram, sizeof(ram), NULL, 0, &bytes, NULL);

				sprintf(szDbg, "Stopped MMIO write combining on %08X", gvi_pc_low_regen + (i*0x1000));
				OutputDebugString(szDbg);
			}
		}
		else
		{
			memcpy(pwSrc, pwDst, 0x1000);
			host_graphics_tick();
		}
	}
}

#endif /* VIDEO_STRATEGY == 2 */


#if VIDEO_STRATEGY == 4
static int bAllConnected = FALSE;
static BOOL bDoVideoSync = FALSE;

GLOBAL VOID haxm_vram_mmio(struct hax_fastmmio *hft)
{
	if (hft->direction == 1)
	{
		// We got a write
		DWORD size = gvi_pc_high_regen - gvi_pc_low_regen + 1;

		// Clear everything on page so that we can detect changes
		memset (getPtrToPhysAddrByte(gvi_pc_low_regen), 0xFF, size);

		// Now unprotect page so that data can flow onto it
		if (!bAllConnected)
		{
			// On first run, connect as RAM-Block
			struct hax_set_ram_info ram = { 0 };
			DWORD bytes;

			ram.pa_start = gvi_pc_low_regen;
			ram.size = size;
			ram.va = (uint64_t)getPtrToPhysAddrByte(gvi_pc_low_regen);
			ram.flags = HAX_RAM_INFO_FAULTISMMIO;
			DeviceIoControl(hVM, HAX_VM_IOCTL_SET_RAM, &ram, sizeof(ram), NULL, 0, &bytes, NULL);
			bAllConnected = TRUE;
		}
		else haxm_mprotect(gvi_pc_low_regen, size, HAX_RAM_PERM_RWX);
		bDoVideoSync = TRUE;
	}
}

GLOBAL VOID haxm_sync_vram(VOID)
{
	PDWORD pdwPage;
	PBYTE  pbPage;
	DWORD  page, d, j, size, npages;
	BOOL   bChanged;

	if (!bDoVideoSync) return;
	size = gvi_pc_high_regen - gvi_pc_low_regen + 1;
	npages = size / 0x1000;
	bChanged = FALSE;
	temp_func = read_b_write_ptrs(SAS_VIDEO);

	/* The aperture was memset to 0xFF at the start of this write window
	 * (haxm_vram_mmio), so any byte != 0xFF is a pixel the guest wrote under
	 * the CURRENT Sequencer state.  Replay each such byte through the software
	 * VGA write path so Map-Mask / write-mode / chain semantics are applied to
	 * the real plane store.  The 0xFF sentinel (rather than a value diff) is
	 * deliberate: in unchained Mode X four adjacent columns write the SAME
	 * aperture offset through four planes, and for a solid fill those bytes are
	 * EQUAL -- a value diff would drop planes 1..3; the clean-slate sentinel
	 * captures every written byte regardless of value.  (Residual: a pixel of
	 * index 0xFF is indistinguishable from "unwritten" and is dropped -- a
	 * minor artifact removed in the Phase 1 trap-and-decode path.) */
	for (page = 0; page < npages; page++)
	{
		pdwPage = (PDWORD)getPtrToPhysAddrByte(gvi_pc_low_regen + page * 0x1000);

		/* Fast skip: a page still entirely 0xFF was untouched this window.
		 * Scan ALL dwords of the page (the old code sampled only the first
		 * 128, missing changes past offset 0x200). */
		for (d = 0; d < 0x1000 / sizeof(DWORD); d++)
			if (pdwPage[d] != 0xFFFFFFFF)
				break;
		if (d == 0x1000 / sizeof(DWORD))
			continue;

		pbPage = (PBYTE)pdwPage;
		for (j = 0; j < 0x1000; j++)
			if (pbPage[j] != 0xFF)
			{
				/* Replay at the byte's TRUE aperture address.  The old code
				 * used (low_regen + page + j), where `page` is the page index
				 * -- so every page past 0 was replayed ~0x1000 bytes too low,
				 * corrupting any guest (e.g. BLOOD) that writes across the
				 * full 64 KB window. */
				(*temp_func)((PBYTE)gvi_pc_low_regen + page * 0x1000 + j, pbPage[j]);
				bChanged = TRUE;
			}
	}
	if (bChanged)
	{
		host_graphics_tick();
	}
	// Protect again to capture next MMIO exit
	if (bAllConnected) haxm_mprotect(gvi_pc_low_regen, size, HAX_RAM_PERM_NONE);
	bDoVideoSync = FALSE;
}
#endif /* VIDEO_STRATEGY == 4 */



EXPORT
VOID
sas_init(
    IN sys_addr Size
    )

/*++

Routine Description:

    This routine initializes the SAS module, and allocates the linear
    address space for the VDM, and loads the ROM.
	We use emulated ROM here, as X86GFX isn't available in newer Windows
	versions anyway, so just do it like the CCPU, reserve entire memory
	space and then sas_connect_memory in there.

Arguments:

    Size - Supplies the size of the VDMs linear address space.

Return Value:

    None.

--*/

{
	IU32	required_mem;

	required_mem = Size + NOWRAP_PROTECTION;
	Start_of_M_area = (PBYTE)host_sas_init(Size);
	if (Start_of_M_area == NULL) {
		if (!(Start_of_M_area = (host_addr)malloc(required_mem)))
			haxmvm_panic("Out of memory allocating %d bytes", required_mem);
	}

	if (!(haxm_videocmp_copy = calloc(1, 0x20000)))
		haxmvm_panic("Out of memory allocating %d bytes", 0x20000);

	VdmSize = Size;

	haxmvm_init();

	/*
	 * Make the entire memory space RAM.  The ROM load routines
	 * will change some of this to being ROM.
	 */

	haxm_alloc((sys_addr)Start_of_M_area, required_mem);
	sas_connect_memory(0, 0x110000 - 1, SAS_RAM);

#ifndef EGATEST
	rom_init();
#endif				/* EGATEST */

	copyROM();
}

#if VDD_INTEG

EXPORT
VOID
sas_term(
    VOID
    )
/*++

Routine Description:

    Free memory prior to reallocing it

Arguments:

    None.

Return Value:

    None.
--*/
{
	int i, j;

	if (host_sas_term() != NULL) {
		if (Start_of_M_area)
			free(Start_of_M_area);
	}

	if (haxm_videocmp_copy) free(haxm_videocmp_copy);

	Start_of_M_area = NULL;
}


EXPORT
sys_addr
sas_memory_size(
    VOID
    )
/*++

Routine Description:

    This routine returns the size of Intel memory

Arguments:

    none

Return Value:

    size of intel memory

--*/
{
    return(VdmSize);
}

static void hax_set_ram(
    IN sys_addr Low,
    IN sys_addr High,
    IN int Type
    )
{
	struct hax_set_ram_info ram = { 0 };
	DWORD bytes;
	extern byte  *video_copy;

    ram.pa_start = Low;
    ram.size = High-Low+1;
    ram.va = (uint64_t)(ULONG)Start_of_M_area + Low;

	switch (Type)
	{
	case SAS_RAM:
		ram.flags = 0;
		break;
	case SAS_ROM:
		ram.flags = HAX_RAM_INFO_ROM;
		break;
	case SAS_VIDEO:
		/* Video routines need to be handled with HAX_EXIT_FAST_MMIO to sync with CVIDC,
		 * so we could unlink memory area with HAX_RAM_INFO_INVALID
		 * Unfortunately, this is really sloooow and makes it barely usable :(
		 * You can try it out, if you want (set VIDEO_STRATEGY to 3).
		 *
		 * What about connecting get_screen_ptr(0) here?
		 * -> The problem is, that emulation expects 4-byte cells (plane format) here
		 *    and the console text buffer expects this format too (see nt_cga.c comment)
		 *    So this unfortunately won't work. Additionally taking 2byte-cells format
		 *    would break CVIDC graphic emulation (which doesn't work properly in HAXM
		 *    anyway, but just saying)
		 * What about connecting video_copy here, which is in sane format?
		 * -> You still need to take into account plane format of text buffer i.e. in nt_cga.c.
		 *    The copy won't sync back into adapter regen area (get_screen_ptr(0)), 
		 *    it's just for comparing.
		 *
		 * So what we do here is that we just map the Video area as normal R/W memory
		 * here and on every return from HAXM sync it into the emulated video card
		 * by calling haxm_sync_vram, which syncs bytes written in between to the emulated
		 * CVIDC card. We may be able to optimize this further by checking Pagetable
		 * dirty bit in EPT for changes in video memory. Not sure if this is faster than
		 * our comparison loop. Please note that GetWriteWatch API would be useful, but 
		 * doesn't work with the EPT-Tables from HAXM, so the only way would be to check
		 * and reset dirty-bit in HAXM EPT.
		 *
		 *       +--- jazz_text_update ---+
		 *       V                        V
		 * +---------------+  +----------------+  +--------------+
		 * |  screen_ptr   |  |   video_copy   |  |  textBuffer  |
		 * +---------------+  +----------------+  +--------------+
		 * | Adapter regen |  | Compare buffer |  | Console text |
		 * |      area     |  |                |  | buffer       |
		 * +---------------+  +----------------+  +--------------+
		 * | PLANAR format |  |   WORD format  |  | PLANAR format|
		 * +---------------+  +----------------+  +--------------+
		 *    ^  |                                         ^
		 *    |  +----------------- nt_text ---------------+
		 *    |
		 *  CVIDC <-------------------------------------+
		 *    |                                         |
		 *  sas_write ----------------------------+     |
		 *    |                                   |     |
		 *    |                                   |  haxm_sync_vram<--------+
		 *    | I/O calls                         V     |                   V
		 * +-----------------+  direct screen   +--------------+  +--------------------+
		 * | DOS Application |<---------------->|   B800-BFFF  |  | haxm_videocmp_copy |
		 * +-----------------+  buffer access   +--------------+  +--------------------+
		 *                                      | regen buffer |  |   Compare buffer   |
		 *                                      +--------------+  +--------------------+
		 *                                      |  WORD format |  |     WORD format    |
		 *                                      +--------------+  +--------------------+
		 */
#if VIDEO_STRATEGY == 1
		/* Dynamic A0000 trap: flat R/W regen buffer for text/linear modes,
		 * FAULTISMMIO per-access trap for planar EGA/VGA modes so latched/ALU
		 * writes hit CVIDC write_b.  It issues its own SET_RAM, so return
		 * instead of falling to the generic ioctl below.  force=TRUE: this
		 * connect is preceded by sas_disconnect_memory() (a generic INACCESSIBLE
		 * SET_RAM with va=0 on this same range), so we MUST re-issue SET_RAM to
		 * restore the mapping even if our cached trap-state is unchanged --
		 * otherwise A0000 stays unmapped and the screen goes black. */
		vram_apply_mapping(Low, High - Low + 1, TRUE);
		return;
#elif VIDEO_STRATEGY == 3
		ram.flags = HAX_RAM_INFO_INVALID | HAX_RAM_INFO_COALESCED;
		ram.va = 0;
		break;
#endif
		
	case SAS_INACCESSIBLE:
#ifdef	LIM
	case SAS_MM_LIM:
#endif
		ram.flags = HAX_RAM_INFO_INVALID;
		ram.va = 0;
		break;
	}

	{
		char szdbg[256];
		sprintf(szdbg, "haxm_set_ram VA %08X-%08X -> %08X", Low, High, (ULONG)ram.va);
		OutputDebugString(szdbg);
	}


	if (!DeviceIoControl(hVM, HAX_VM_IOCTL_SET_RAM, &ram, sizeof(ram), NULL, 0, &bytes, NULL))
	{
		haxmvm_panic("SET_RAM (pa_start = %08X, size=%08X, va = %08X, gle=%d", (ULONG)ram.pa_start, (ULONG)ram.size, 
			(ULONG)ram.va, GetLastError());
	} 
}


EXPORT
VOID
sas_connect_memory(
    IN sys_addr Low,
    IN sys_addr High,
    IN int Type
    )
/*++

Routine Description:

    This routine sets up a type record for the specified address region.
    If the specified address region was a different type, it is changed to
    the new type.

Arguments:

    Low -- the starting address of the region
    High -- the ending address of the region
    Type -- the type for the region, one of SAS_RAM, SAS_VIDEO, SAS_ROM,
        SAS_WRAP, SAS_INACCESSIBLE

Return Value:

    None.
--*/
{
    PMEMTYPE Current, Previous, New, Temp;
	struct hax_set_ram_info ram = { 0 };
	DWORD bytes;
	extern byte  *video_copy;


	{
		char szdbg[256];
		sprintf(szdbg, "sas_connect_memory VA %08X-%08X -> %08X", Low, High, (ULONG)Start_of_M_area + Low);
		OutputDebugString(szdbg);
	}

	hax_set_ram(Low, High, Type);

    //bugbug do we handle new block contained in old block correctly?
    if (!MemType) {
        MemType = (PMEMTYPE) ch_malloc(sizeof(MEMTYPE));
        MemType->Previous = NULL;
        MemType->Next = NULL;
        MemType->Start = Low;
        MemType->End = High;
        MemType->Type = (half_word)Type;
		//hax_set_ram(Low, High, Type);
        return;
    }

    Current = MemType;
    while (Current && (Low > Current->Start)) {
        Previous = Current;
        Current = Current->Next;
    }

    if ((Current) && (Low == Current->Start) && (High == Current->End)) {
		if (Current->Type != (half_word)Type)
		{
			//hax_set_ram(Low, High, SAS_INACCESSIBLE);
			//hax_set_ram(Low, High, Type);
			Current->Type = (half_word)Type;
		}
        return;
    }

    if (!Current) {
        // Block goes at end of list
        New = (PMEMTYPE) ch_malloc(sizeof(MEMTYPE));
        Previous->Next = New;
        New->Previous = Previous;
        New->Start = Low;
        New->End = High;
        New->Type = (half_word)Type;
        New->Next = NULL;
    } else {
        // Block goes in front of Current
        New = (PMEMTYPE) ch_malloc(sizeof(MEMTYPE));
        New->Start = Low;
        New->Type = (half_word)Type;
        New->End = High;
        New->Previous = Current->Previous;
        New->Next = Current;
        Current->Previous = New;
        if (!New->Previous) {
            MemType = New;
        } else {
            New->Previous->Next = New;
        }
    }


    // Block overlaps one or more existing blocks

    if (New->Previous) {
        if (New->Previous->End > New->End) {
            // block contained in exising block
			//hax_set_ram(New->Start, New->Previous->End, SAS_INACCESSIBLE);
			//hax_set_ram(New->End, New->Previous->End, New->Previous->Type);
			//hax_set_ram(New->Start, New->End, Type);

            Temp = (PMEMTYPE) ch_malloc(sizeof(MEMTYPE));
            Temp->Previous = New;
            Temp->Next = New->Next;
            New->Next = Temp;
            if (Temp->Next) {
                Temp->Next->Previous = Temp;
            }
            Temp->End = New->Previous->End;
            New->Previous->End = New->Start - 1;
            Temp->Start = New->End + 1;
            Temp->Type = New->Previous->Type;

            return;
        } else if (New->Previous->End >= New->Start){
            // block overlaps end of exising block
			//hax_set_ram(New->Start, New->Previous->End, SAS_INACCESSIBLE);
            New->Previous->End = New->Start - 1;
        }
    }

    // remove all blocks entirely contained in new block
    while ((New->Next) && (New->Next->End <= New->End)) {
		//hax_set_ram(New->Next->Start, New->Next->End, SAS_INACCESSIBLE);
        Temp = New->Next;
        New->Next = New->Next->Next;
        if (New->Next) {
            New->Next->Previous = New;
        }
        free(Temp);
    }

    // remove portion of next block overlapping new block
    if ((New->Next) && (New->Next->Start <= New->End)) {
		//hax_set_ram(New->Next->Start, New->End, SAS_INACCESSIBLE);
        New->Next->Start = New->End + 1;
    }

	//hax_set_ram(New->Start, New->End, Type);
}


EXPORT
half_word
sas_memory_type(
    IN sys_addr Address
    )
/*++

Routine Description:

    This routine returns the type of memory at a specific address

Arguments:

    Address -- linear address to return type for.

Return Value:

    the type for the region, one of SAS_RAM, SAS_VIDEO, SAS_ROM,
        SAS_WRAP, SAS_INACCESSIBLE
--*/
{
    PMEMTYPE Current;

    if (Address > VdmSize) {
        return SAS_INACCESSIBLE;
    }

    Current = MemType;
    while (Current && !((Address >= Current->Start) &&
        (Address <= Current->End))) {
        Current = Current->Next;
    }
    if (!Current) {
        return SAS_INACCESSIBLE;
    }
    return Current->Type;
}



EXPORT
VOID
sas_enable_20_bit_wrapping(
    VOID
    )
/*++

Routine Description:

    This routine causes memory addresses to wrap at 1MB

Arguments:

    None.

Return Value:

    None.

--*/
{
	/* Don't know if this ia good idea and needed */
	struct hax_set_ram_info ram = { 0 };
	DWORD bytes;

    ram.pa_start = ONEMEGA;
    ram.size = SIXTYFOURK;
    ram.va = (uint64_t)(ULONG)Start_of_M_area;

    if (!DeviceIoControl(hVM, HAX_VM_IOCTL_SET_RAM, &ram, sizeof(ram), NULL, 0, &bytes, NULL))
    {
        haxmvm_panic("SET_RAM");
    } 

	SasWrapMask = 0xfffff;
}

EXPORT
VOID
sas_disable_20_bit_wrapping(
    VOID
    )
/*++

Routine Description:

    This routine causes addressing to NOT wrap at 1MB

Arguments:

    None.

Return Value:

    None.

--*/
{
	/* Don't know if this ia good idea and needed */
	struct hax_set_ram_info ram = { 0 };
	DWORD bytes;

    ram.pa_start = ONEMEGA;
    ram.size = SIXTYFOURK;
    ram.va = (uint64_t)(ULONG)Start_of_M_area + ram.pa_start;

    if (!DeviceIoControl(hVM, HAX_VM_IOCTL_SET_RAM, &ram, sizeof(ram), NULL, 0, &bytes, NULL))
    {
        haxmvm_panic("SET_RAM");
    } 

	SasWrapMask = 0xffffffff;
}



EXPORT
half_word
sas_hw_at(
    IN sys_addr Address
    )
/*++

Routine Description:

    This routine returns the byte at the specified address

Arguments:

    Address -- address of byte to return

Return Value:

    value of byte at specified address

--*/
{
	return (bios_read_byte(Address));
}


EXPORT
word
sas_w_at(
    IN sys_addr Address
    )
/*++

Routine Description:

    This routine returns the word at the specified address

Arguments:

    Address -- address of word to return

Return Value:

    value of word at specified address

--*/
{
	if ((Address & 0xFFF) <= 0xFFE)
		return (bios_read_word(Address));
	else
	{
		return (bios_read_byte(Address) | ((IU16)bios_read_byte(Address+1) << 8));
	}
}


EXPORT
double_word
sas_dw_at(
    IN sys_addr Address
    )
/*++

Routine Description:

    This routine returns the dword at the specified address

Arguments:

    Address -- address of dword to return

Return Value:

    value of dword at specified address

--*/
{
	if ((Address & 0xFFF) <= 0xFFC)
		return (bios_read_double(Address));
	else
	{
		return (bios_read_word(Address) | ((IU32)bios_read_word(Address+2) << 16));
	}
}

#ifndef CPU_40_STYLE

EXPORT
VOID
sas_load(
    IN sys_addr Address,
    IN half_word *Value
    )
/*++

Routine Description:

    This routine stores the byte at the specified address in the supplied
    variable

Arguments:

    Address -- address of byte to return
    Value -- Variable to store the value in

Return Value:

    None.
--*/
{
    *Value = sas_hw_at(Address);
    return;
}



EXPORT
VOID
sas_loadw(
    IN sys_addr Address,
    IN word *Value
    )
/*++

Routine Description:

    This routine stores the word at the specified address in the supplied
    variable

Arguments:

    Address -- address of word to return
    Value -- Variable to store the value in

Return Value:

    None.
--*/
{
    //DbgPrint("NtVdm : sas_loadw\n");
    *Value = sas_dw_at(Address);
    return;
}
#endif
#endif


EXPORT
VOID
sas_store(
    IN sys_addr Address,
    IN half_word Value
    )
/*++

Routine Description:

    This routine stores the specified byte at the specified address

Arguments:

    Address -- address of word to return
    Value -- value to store

Return Value:

    None.
--*/
{
	sub_note_trace2(SAS_VERBOSE, "sas_store addr=%x, val=%x\n", Address, Value);
	bios_write_byte(Address, Value);
}


EXPORT
VOID
sas_storew(
    IN sys_addr Address,
    IN word Value
    )
/*++

Routine Description:

    This routine stores the specified word at the specified address

Arguments:

    Address -- address of word to return
    Value -- value to store at the specified address

Return Value:

    None.
--*/
{
	sub_note_trace2(SAS_VERBOSE, "c_sas_storew addr=%x, val=%x\n", Address, Value);
	if ((Address & 0xFFF) <= 0xFFE)
		bios_write_word(Address, Value);
	else
	{
		bios_write_byte(Address+1, Value >> 8);
		bios_write_byte(Address, Value & 0xFF);
	}
}


EXPORT
VOID
sas_storedw(
    IN sys_addr Address,
    IN double_word Value
    )
/*++

Routine Description:

    This routine stores the specified dword at the specified address

Arguments:

    Address -- address of word to return
    Value -- value to store at the specified address

Return Value:

    None.
--*/
{
	sub_note_trace2(SAS_VERBOSE, "c_sas_storedw addr=%x, val=%x\n", Address, Value);

	if ((Address & 0xFFF) <= 0xFFC)
		bios_write_double(Address, Value);
	else
	{
		bios_write_word(Address+2, Value >> 16);
		bios_write_word(Address, Value & 0xFFFF);
	}
}


EXPORT
VOID
sas_loads(
    IN sys_addr Source,
    IN host_addr Destination,
    IN sys_addr Length
    )
/*++

Routine Description:

     This routine copies the string from the specified intel address to the
     specified host address

Arguments:

    Source -- Intel address to copy from
    Destination -- host address to copy the string to
    Length -- length of the string to copy

Return Value:

    None.
--*/
{
	if (getPG())
	{
		if (!xtrn2phy(Source, bios_read_accreq(), &Source))
			return;
	}

	sas_PRS (Source, Destination, Length);
}



EXPORT
VOID
sas_stores(
    IN sys_addr Destination,
    IN host_addr Source,
    IN sys_addr Length
    )
/*++

Routine Description:

     This routine copies the string from the specified host address to the
     specified intel address

Arguments:

    Destination -- intel address to copy the string to
    Source -- host address to copy from
    Length -- length of the string to copy

Return Value:

    None.
--*/
{
	if (getPG())
	{
		if (!xtrn2phy(Destination, bios_read_accreq() | PG_W, &Destination))
			return;
	}

	sas_PWS(Destination, Source, Length);
}


EXPORT
VOID
sas_move_bytes_forward(
    IN sys_addr Source,
    IN sys_addr Destination,
    IN sys_addr Length
    )
/*++

Routine Description:

    This routine copies one region of intel memory to another.

Arguments:

    Source -- source intel address
    Destination -- destination intel address
    Length -- length of region to copy (in bytes)

Return Value:

    None.
--*/
{
	sys_addr i;
	SAS_MEM_TYPE src_type;
#ifdef EGG
	sys_addr dest_phy;
#endif

	if (getPG())
	{
		if (!xtrn2phy(Source, bios_read_accreq(), &Source))
			return;
	}

	Source &= SasWrapMask;
	src_type = readSelfMod(Source, Length);

#ifdef EGG
	/*
	 * VIDEO destination: per-byte read+write loop. Mirrors the per-instruction
	 * REP MOVSB exactly — sas_PR8 dispatches VIDEO src through the cvidc byte
	 * read handler (loads latches), sas_store dispatches VIDEO dst through the
	 * cvidc byte write handler (correct mark_byte, correct plane distribution
	 * in all chain modes).
	 *
	 * Replaces the buggy bulk VIDEO -> VIDEO move below (the (Source-base)*2+base
	 * hack documented in the original comment) and gives RAM→VIDEO byte ops
	 * the proper per-element dispatch instead of a raw-byte sas_stores loop.
	 *
	 * Skipping CPU decode/commit overhead still wins from fast_rep_movs.
	 */
	dest_phy = Destination;
	if (getPG())
	{
		if (!xtrn2phy(Destination, bios_read_accreq() | PG_W, &dest_phy))
			return;
	}
	if (readSelfMod(dest_phy & SasWrapMask, Length) == SAS_VIDEO)
	{
		for (i = 0; i < Length; i++) {
			sas_store(Destination + i, sas_PR8(Source + i));
		}
		return;
	}
#endif /* EGG */

	switch (src_type) {
		case SAS_MM_LIM:
		case SAS_RAM:
		case SAS_ROM:
		default:
			sas_stores(Destination, getPtrToPhysAddrByte(Source), Length);
			break;

#ifdef	EGG
		case SAS_VIDEO:
			/* VIDEO source, non-VIDEO dest (VIDEO dest handled above). */
			if (getPG())
			{
				if (!xtrn2phy(Destination, bios_read_accreq() | PG_W, &Destination))
					return;
			}
			Destination &= SasWrapMask;

			switch (readSelfMod(Destination, Length)) {
				case SAS_RAM:	/* Video -> RAM */
					temp_func = read_pointers.str_read;
					(*temp_func) (getPtrToPhysAddrByte(Destination), Source, Length);
					break;

#ifdef	LIM
				case SAS_MM_LIM:	/* Video -> LIM */
					temp_func = read_pointers.str_read;
					(*temp_func) (getPtrToPhysAddrByte(Destination), Source, Length);
					LIM_str_write(Destination, Length);
					break;
#endif /*LIM */

				case SAS_INACCESSIBLE:
				case SAS_ROM:
					/* No ROM_fix_sets !!! Yeh !!! */
					break;

				default:
					printf("Unknown Sas type\n");
					force_yoda();
					break;
			}
			break;
#endif /* EGG */

		case SAS_INACCESSIBLE:
		case SAS_DANGEROUS:
			for (i = 0; i < Length; i++)
				sas_store(Destination + i, sas_PR8(Source + i));
			break;
	}
}



EXPORT
VOID
sas_move_words_forward(
    IN sys_addr Source,
    IN sys_addr Destination,
    IN sys_addr Length
    )
/*++

Routine Description:

    This routine copies one region of intel memory to another.

Arguments:

    Source -- source intel address
    Destination -- destination intel address
    Length -- length of region to copy (in words)

Return Value:

    None.
--*/
{
	sys_addr i;
	SAS_MEM_TYPE src_type;
#ifdef EGG
	sys_addr dest_phy;
#endif

	if (getPG())
	{
		if (!xtrn2phy(Source, bios_read_accreq(), &Source))
			return;
	}

	Source &= SasWrapMask;
	Length <<= 1;
	src_type = readSelfMod(Source, Length);

#ifdef EGG
	/*
	 * VIDEO destination: per-word read+write loop. sas_storew dispatches
	 * through the cvidc word-write handler (UCWWRTF / chain-mode variants);
	 * NOT equivalent to two byte writes in chain-2 / Mode-X (see C2MXWWRTF
	 * in sevid000.h — second byte goes to +3, not +1). Also gives correct
	 * latch-load semantics for VIDEO src via sas_PR16. Replaces the buggy
	 * bulk VIDEO -> VIDEO dispatch below.
	 */
	dest_phy = Destination;
	if (getPG())
	{
		if (!xtrn2phy(Destination, bios_read_accreq() | PG_W, &dest_phy))
			return;
	}
	if (readSelfMod(dest_phy & SasWrapMask, Length) == SAS_VIDEO)
	{
		for (i = 0; i < Length; i += 2) {
			sas_storew(Destination + i, sas_PR16(Source + i));
		}
		return;
	}
#endif /* EGG */

	switch (src_type) {
		case SAS_MM_LIM:
		case SAS_RAM:
		case SAS_ROM:
		default:
			sas_stores(Destination, getPtrToPhysAddrByte(Source), Length);
			break;

#ifdef	EGG
		case SAS_VIDEO:
			/* VIDEO source, non-VIDEO dest (VIDEO dest handled above). */
			if (getPG())
			{
				if (!xtrn2phy(Destination, bios_read_accreq() | PG_W, &Destination))
					return;
			}
			Destination &= SasWrapMask;

			switch (readSelfMod(Destination, Length)) {
				case SAS_RAM:	/* Video -> RAM */
					temp_func = read_pointers.str_read;
					(*temp_func) (getPtrToPhysAddrByte(Destination), Source, Length);
					break;

#ifdef	LIM
				case SAS_MM_LIM:	/* Video -> LIM */
					temp_func = read_pointers.str_read;
					(*temp_func) (getPtrToPhysAddrByte(Destination), Source, Length);
					LIM_str_write(Destination, Length);
					break;
#endif /*LIM */

				case SAS_INACCESSIBLE:
				case SAS_ROM:
					/* No ROM_fix_sets !!! Yeh !!! */
					break;

				default:
					printf("Unknown Sas type\n");
					force_yoda();
					break;
			}
			break;
#endif /* EGG */

		case SAS_INACCESSIBLE:
		case SAS_DANGEROUS:
			for (i = 0; i < Length; i+=2)
				sas_storew(Destination + i, sas_PR16(Source + i));
			break;
	}
}



EXPORT
VOID
sas_move_doubles_forward(
    IN sys_addr Source,
    IN sys_addr Destination,
    IN sys_addr Length
    )
/*++

Routine Description:

    This routine copies one region of intel memory to another.

Arguments:

    Source -- source intel address
    Destination -- destination intel address
    Length -- length of region to copy (in dwords)

Return Value:

    None.
--*/
{
	sys_addr i;
	SAS_MEM_TYPE src_type;
#ifdef EGG
	sys_addr dest_phy;
#endif

	if (getPG())
	{
		if (!xtrn2phy(Source, bios_read_accreq(), &Source))
			return;
	}

	Source &= SasWrapMask;
	Length <<= 2;
	src_type = readSelfMod(Source, Length);

#ifdef EGG
	/*
	 * VIDEO destination: per-dword read+write loop via sas_storedw (routes
	 * to bios_write_double -> cvidc dword write). Matches per-instruction
	 * REP MOVSD semantics for VIDEO; works for both RAM and VIDEO src.
	 */
	dest_phy = Destination;
	if (getPG())
	{
		if (!xtrn2phy(Destination, bios_read_accreq() | PG_W, &dest_phy))
			return;
	}
	if (readSelfMod(dest_phy & SasWrapMask, Length) == SAS_VIDEO)
	{
		for (i = 0; i < Length; i += 4) {
			sas_storedw(Destination + i, sas_PR32(Source + i));
		}
		return;
	}
#endif /* EGG */

	switch (src_type) {
		case SAS_MM_LIM:
		case SAS_RAM:
		case SAS_ROM:
		default:
			sas_stores(Destination, getPtrToPhysAddrByte(Source), Length);
			break;

#ifdef	EGG
		case SAS_VIDEO:
			/* VIDEO source, non-VIDEO dest (VIDEO dest handled above). */
			if (getPG())
			{
				if (!xtrn2phy(Destination, bios_read_accreq() | PG_W, &Destination))
					return;
			}
			Destination &= SasWrapMask;

			switch (readSelfMod(Destination, Length)) {
				case SAS_RAM:	/* Video -> RAM */
					temp_func = read_pointers.str_read;
					(*temp_func) (getPtrToPhysAddrByte(Destination), Source, Length);
					break;

#ifdef	LIM
				case SAS_MM_LIM:	/* Video -> LIM */
					temp_func = read_pointers.str_read;
					(*temp_func) (getPtrToPhysAddrByte(Destination), Source, Length);
					LIM_str_write(Destination, Length);
					break;
#endif /*LIM */

				case SAS_INACCESSIBLE:
				case SAS_ROM:
					/* No ROM_fix_sets !!! Yeh !!! */
					break;

				default:
					printf("Unknown Sas type\n");
					force_yoda();
					break;
			}
			break;
#endif /* EGG */

		case SAS_INACCESSIBLE:
		case SAS_DANGEROUS:
			for (i = 0; i < Length; i+=4)
				sas_storedw(Destination + i, sas_PR32(Source + i));
			break;
	}
}



EXPORT
VOID
sas_move_bytes_backward(
    IN sys_addr Source,
    IN sys_addr Destination,
    IN sys_addr Length
    )
/*++

Routine Description:

    This routine copies one region of intel memory to another.
	Not used.

Arguments:

    Source -- source intel address
    Destination -- destination intel address
    Length -- length of region to copy (in bytes)

Return Value:

    None.
--*/
{
    _asm int 3;
}



EXPORT
VOID
sas_move_words_backward(
    IN sys_addr Source,
    IN sys_addr Destination,
    IN sys_addr Length
    )
/*++

Routine Description:

    This routine copies one region of intel memory to another.
	Not used.

Arguments:

    Source -- source intel address
    Destination -- destination intel address
    Length -- length of region to copy (in words)

Return Value:

    None.
--*/
{
    _asm int 3;
}

EXPORT
VOID
sas_fills(
    IN sys_addr Destination,
    IN half_word Value,
    IN sys_addr Length
    )
/*++

Routine Description:

    This routine fills a specified region of intel memory with a byte value

Arguments:

    Address -- address to fill at
    Value -- value to fill with
    Length -- length of region to fill

Return Value:

    None.
--*/
{
	sys_addr i, temp_val;

	if (getPG())
	{
		if (!xtrn2phy(Destination, bios_read_accreq() | PG_W, &Destination))
			return;
	}
	Destination &= SasWrapMask;

	switch (temp_val = readSelfMod(Destination, Length)) {
		case SAS_RAM:
			RtlFillMemory(getPtrToPhysAddrByte(Destination), Length, Value);
			break;

#ifdef	LIM
		case SAS_MM_LIM:
			RtlFillMemory(getPtrToPhysAddrByte(Destination), Length, Value);
			LIM_str_write(Destination, Length);
			break;
#endif

		case SAS_INACCESSIBLE:
		case SAS_ROM:
			/* No ROM_fix_sets !!! Yeh !!! */
			break;

		default:
			printf("Unknown Sas type\n");
			force_yoda();
			break;

		case SAS_VIDEO:
			temp_func = read_b_fill_ptrs(temp_val);
			(*temp_func) (Destination, Value, Length);
#if VIDEO_STRATEGY == 1
			RtlFillMemory(getPtrToPhysAddrByte(Destination), Length, Value);
			RtlFillMemory(haxm_videocmp_copy + Destination - gvi_pc_low_regen, Length, Value);
#endif
			break;
	}
}

EXPORT
VOID
sas_fillsw(
    IN sys_addr Destination,
    IN word Value,
    IN sys_addr Length
    )
/*++

Routine Description:

    This routine fills a specified region of intel memory with a word value

Arguments:

    Address -- address to fill at
    Value -- value to fill with
    Length -- length of region to fill

Return Value:

    None.
--*/
{
	sys_addr i, temp_val;
	IU16 *dst;

	if (getPG())
	{
		if (!xtrn2phy(Destination, bios_read_accreq() | PG_W, &Destination))
			return;
	}
	Destination &= SasWrapMask;

	switch (temp_val = readSelfMod(Destination, Length<<1)) {
		case SAS_RAM:
			for (i=0, dst=(IU16*)getPtrToPhysAddrByte(Destination); i<Length; i++)
				dst[i] = Value;
			break;

#ifdef	LIM
		case SAS_MM_LIM:
			for (i=0, dst=(IU16*)getPtrToPhysAddrByte(Destination); i<Length; i++)
				dst[i] = Value;
			LIM_str_write(Destination, Length<<1);
			break;
#endif

		case SAS_INACCESSIBLE:
		case SAS_ROM:
			/* No ROM_fix_sets !!! Yeh !!! */
			break;

		default:
			printf("Unknown Sas type\n");
			force_yoda();
			break;

		case SAS_VIDEO:
			temp_func = read_w_fill_ptrs(temp_val);
			(*temp_func) (Destination, Value, Length);
#if VIDEO_STRATEGY == 1
			for (i=0, dst=(IU16*)getPtrToPhysAddrByte(Destination); i<Length; i++)
				dst[i] = Value;
			for (i=0, dst=(IU16*)(haxm_videocmp_copy + Destination - gvi_pc_low_regen); i<Length; i++)
				dst[i] = Value;
#endif
			break;
	}
}

EXPORT
VOID
sas_fillsdw(
    IN sys_addr Destination,
    IN IU32 Value,
    IN sys_addr Length
    )
/*++

Routine Description:

    This routine fills a specified region of intel memory with a dword value

Arguments:

    Address -- address to fill at
    Value -- value to fill with
    Length -- length of region to fill

Return Value:

    None.
--*/
{
	sys_addr i, temp_val;
	IU32 *dst;

	if (getPG())
	{
		if (!xtrn2phy(Destination, bios_read_accreq() | PG_W, &Destination))
			return;
	}
	Destination &= SasWrapMask;

	switch (temp_val = readSelfMod(Destination, Length<<2)) {
		case SAS_RAM:
			for (i=0, dst=(IU32*)getPtrToPhysAddrByte(Destination); i<Length; i++)
				dst[i] = Value;
			break;

#ifdef	LIM
		case SAS_MM_LIM:
			for (i=0, dst=(IU32*)getPtrToPhysAddrByte(Destination); i<Length; i++)
				dst[i] = Value;
			LIM_str_write(Destination, Length<<2);
			break;
#endif

		case SAS_INACCESSIBLE:
		case SAS_ROM:
			/* No ROM_fix_sets !!! Yeh !!! */
			break;

		default:
			printf("Unknown Sas type\n");
			force_yoda();
			break;

		case SAS_VIDEO:
#ifdef C_VID
			fill_dword_ev_glue(Destination, Value, Length);
#else
			for (i=0; i<Length<<2; i+=4) {
				temp_func = read_w_write_ptrs(temp_val);
				(*temp_func) (Destination+i, (IU16)Value);
				(*temp_func) (Destination+i+2, (IU16)(Value >> 16));
			}
#endif		
#if VIDEO_STRATEGY == 1
			for (i=0, dst=(IU32*)getPtrToPhysAddrByte(Destination); i<Length; i++)
				dst[i] = Value;
			for (i=0, dst=(IU32*)(haxm_videocmp_copy + Destination - gvi_pc_low_regen); i<Length; i++)
				dst[i] = Value;
#endif
			break;
	}
}

host_addr scratch = NULL;

EXPORT
host_addr
sas_scratch_address(
    IN sys_addr Length
    )
/*++

Routine Description:

    This routine supplies a scratch buffer for short term use

Arguments

    Length -- length of buffer needed

Return Value:

    None.

--*/
{
    if (Length > 64 * 1024) {
        return NULL;
    }

    if (scratch)
        return scratch;

    if ((scratch = (host_addr) malloc (64 * 1024)) == NULL)
        return NULL;

    return scratch;
}

EXPORT
half_word
sas_hw_at_no_check(
    sys_addr addr
    )
{
	// BIOS should always be at beginning of physical memory, so we do this fast
	addr &= SasWrapMask;
    return *((half_word *)(getPtrToPhysAddrByte(addr)));
}

EXPORT
word
sas_w_at_no_check(
    sys_addr addr
    )
{
	// BIOS should always be at beginning of physical memory, so we do this fast
	addr &= SasWrapMask;
    return *((word *)(getPtrToPhysAddrByte(addr)));
}
EXPORT
double_word
sas_dw_at_no_check(
    sys_addr addr
    )
{
	// BIOS should always be at beginning of physical memory, so we do this fast
	addr &= SasWrapMask;
    return *((double_word *)(getPtrToPhysAddrByte(addr)));
}


EXPORT
VOID
sas_store_no_check(
    sys_addr addr,
    half_word val
    )
{
	// BIOS should always be at beginning of physical memory, so we do this fast
	addr &= SasWrapMask;
    *((half_word *)(getPtrToPhysAddrByte(addr))) = val;
}

EXPORT
VOID
sas_storew_no_check(
    sys_addr addr,
    word val
    )
{
	// BIOS should always be at beginning of physical memory, so we do this fast
	addr &= SasWrapMask;
    *((word *)(getPtrToPhysAddrByte(addr))) = val;
}

BOOL sas_twenty_bit_wrapping_enabled() {
    return (SasWrapMask == 0xfffff);
}

VOID sas_part_enable_20_bit_wrapping(){
}
VOID sas_part_disable_20_bit_wrapping(){
}

#define READ_SELF_MOD sas_memory_type	


/*(
 *========================== readSelfMod ===================================
 * readSelfMod
 *
 * Purpose
 *	This function reads the self modify table, and returns the
 *	memory type.  It will also indicate whether there is a type
 *	boundary within the length specified.
 *
 * Input
 *	addr		The physical intel address to read from
 *	typeSize	The size in bytes of the item to be read
 *
 * Outputs
 *	Memory type.
 *
 * Description
 *	We check that the memory type for both ends of the type is the same.
)*/

LOCAL enum SAS_MEM_TYPE
readSelfMod IFN2(PHY_ADDR, addr, DWORD, typeSize)
{
	enum SAS_MEM_TYPE startType;

	startType = READ_SELF_MOD(addr);

	if (startType == READ_SELF_MOD(addr + typeSize - 1))
		return(startType);
	else
		return(SAS_DANGEROUS);
}

GLOBAL IBOOL
xtrn2phy IFN3
   (
   LIN_ADDR, lin,		/* Linear Address */
   IUM8, access_request,	/* access mode request */
				/* Bit 0 => R/W (as per 486) */
				/* Bit 1 => U/S (as per 486) */
				/* Bit 2 => if set only return mapping
				   if accessed and dirty bits are set
				   for the required address translation.
				   */
   PHY_ADDR *, phy		/* pntr to Physical Address */
   )
{
	uint64_t va = lin, pa;
	DWORD bytes;

    if (!DeviceIoControl(hVCPU, HAX_VCPU_IOCTL_VA2GPA, &va, sizeof(va), &pa, sizeof(pa), &bytes, NULL))
    {
		return FALSE;
    }
	*phy = (PHY_ADDR)pa;
	return TRUE;
}



/*(
 *========================== phyR ===================================
 * phyR
 *
 * Purpose
 *	This is the generic physical read function and takes parameters
 *	of any size (well up to an IU32 that is).
 *
 * Input
 *	addr		The physical intel address to read from
 *	typeSize	The size in bytes of the item to be read
 *	vidFP		A video read function pointer of the appropriate size.
 *	name		"byte" for byte, etc.
 *
 * Outputs
 *	An IU32 that should be masked to get the right bits.
 *
 * Description
 *	We check for out of memory refernces, VIDEO and inaccessible references
 *	and also split reads that span a memory type boundary.
)*/
typedef IU32 (*VID_READ_FP) IPT1(PHY_ADDR, offset);

LOCAL IU32
phyR IFN4(PHY_ADDR, addr, IUM8, typeSize, VID_READ_FP, vidFP, char *, name)
{
	IUM8	byte;
	IUM32	retVal;

	addr &= SasWrapMask;

	if ((addr + typeSize + 1) >= Length_of_M_area) {
		SAVED IBOOL first = TRUE;
		SAVED IU32 junk_value = 0xfefefefe;
		if (first)
		{
			char *env = getenv("BEYOND_MEMORY_VALUE");
			if (env != NULL)
			{
				junk_value = strtol(env, (char **)0, 16);
				always_trace1("phyR: using %08x as value to read from outside physical M", junk_value);
			}
			first = FALSE;
		}
		always_trace2("phyR - %s read from outside physical M - address 0x%0x", name, addr);
		return(junk_value);
	}

	switch (readSelfMod(addr, typeSize)) {
	case SAS_DANGEROUS:
		retVal = 0;
		for (byte = 0; byte < typeSize; byte++) {
			retVal = retVal
				+((IUM32)phyR(addr, 1, read_pointers.b_read,
					"byte") << (byte * 8));
			addr++;
		}
		return(retVal);
#ifdef	EGG
	case SAS_VIDEO:
#ifndef C_VID
		if (typeSize>2)
		{
			/*
			 * MIKE!  This needs changing when we have a dword interface to the
			 * video.
			 */

			IU16 low, high;
			low = (IU16)phyR(addr, sizeof(IU16), read_pointers.w_read, "word");
			high = (IU16)phyR(addr + 2, sizeof(IU16), read_pointers.w_read, "word");

			return(((IU32)high << 16) + low);
		}
#endif
		return ((*vidFP)(addr));
		break;
#endif				/* EGG */

	case SAS_INACCESSIBLE:
		return (0xffffffff);

	case SAS_ROM:
	case SAS_RAM:
	default:
		switch (typeSize)
		{
		case 1: return *getPtrToPhysAddrByte(addr);
		case 2: return *((IU16*)getPtrToPhysAddrByte(addr));
		case 4: return *((IU32*)getPtrToPhysAddrByte(addr));
		default:
			addr = addr + typeSize - 1; /* move to last byte */
			retVal = 0;

			while (typeSize > 0) {
				retVal = retVal << 8;
				retVal += *(getPtrToPhysAddrByte(addr));
				addr -= 1;
				typeSize -= 1;
			}
			return(retVal);
		}
	}
}
/*(
 *========================== phy_rX ===================================
 * phy_rX
 *
 * Purpose
 *	These are the physical read functions.
 *
 * Input
 *	addr		The physical intel address to read from
 *
 * Outputs
 *	The value read
 *
 * Description
 *	Simply call the generic function with the right bits.
)*/

GLOBAL IU8 
sas_PR8 IFN1(PHY_ADDR, addr)
{
	IU8 retVal;

	retVal = (IU8)phyR(addr, sizeof(IU8), read_pointers.b_read, "byte");
	sub_note_trace2(SAS_VERBOSE, "sas_PR16 addr=%x, val=%x\n", addr, retVal);
	return(retVal);
}


GLOBAL IU16 
sas_PR16 IFN1(PHY_ADDR, addr)
{
	IU16 retVal;

	retVal = (IU16)phyR(addr, sizeof(IU16), read_pointers.w_read, "word");
	sub_note_trace2(SAS_VERBOSE, "sas_PR16 addr=%x, val=%x\n", addr, retVal);
	return(retVal);
}


GLOBAL IU32 
sas_PR32 IFN1(PHY_ADDR, addr)
{
	IU32 retVal;

#ifndef C_VIDxx /* FIXME: dword interface currently not working as expected? */
	IU16 low, high;
	low = (IU16)phyR(addr, sizeof(IU16), read_pointers.w_read, "word");
	high = (IU16)phyR(addr + 2, sizeof(IU16), read_pointers.w_read, "word");

	return(((IU32)high << 16) + low); 
#else
	retVal = (IU32)phyR(addr, sizeof(IU32), read_dword_ev_glue, "dword");
	sub_note_trace2(SAS_VERBOSE, "sas_PR32 addr=%x, val=%x\n", addr, retVal);
	return(retVal);
#endif
}

/* store a byte at the given address */

GLOBAL void sas_PW8 
IFN2(PHY_ADDR, addr, IU8, val)
{
	sys_addr	temp_val;

	sub_note_trace2(SAS_VERBOSE, "c_sas_store addr=%x, val=%x\n", addr, val);

	addr &= SasWrapMask;
	checkAccess(addr);

	if (addr < Length_of_M_area) {
		temp_val = readSelfMod(addr, sizeof(IU8));

		switch (temp_val) {
		case SAS_RAM:
			(*(IU8 *) getPtrToPhysAddrByte(addr)) = val;
			break;

#ifdef	LIM
		case SAS_MM_LIM:
			(*(IU8 *) getPtrToPhysAddrByte(addr)) = val;
			LIM_b_write(addr);
			break;
#endif

		case SAS_INACCESSIBLE:
		case SAS_ROM:
			/* No ROM_fix_sets !!! Yeh !!! */
			break;

		default:
			printf("Unknown Sas type\n");
			force_yoda();

		case SAS_VIDEO:
			temp_func = read_b_write_ptrs(temp_val);
#if HAXM_VRAM_FLATREAD
			/* Flat-read aperture: the guest's VRAM read no longer faults (it is
			 * served from the read-only PTE), so the EGA latch it would have
			 * loaded must be loaded here before the write applies.  This covers
			 * the common SAME-address read-modify-write (read di, write di). */
			(void) sas_PR8(addr);
#endif
			(*temp_func) (addr, val);
#if VIDEO_STRATEGY == 1
#if HAXM_VRAM_FLATREAD
			/* Keep the flat backing (what the read-only PTE serves to guest
			 * reads) equal to the read-map-selected plane byte after the write,
			 * so latch-discarding reads AND value-using reads see real data. */
			(*(IU8 *) getPtrToPhysAddrByte(addr)) = sas_PR8(addr);
#else
			(*(IU8 *) getPtrToPhysAddrByte(addr)) = val;
#endif
			(*(IU8 *) (haxm_videocmp_copy + addr - gvi_pc_low_regen)) = val;
#endif
			break;
		}

	} else
		printf("Byte written outside M %x\n", addr);
}

/* store a word at the given address */
GLOBAL void 
sas_PW16 IFN2(PHY_ADDR, addr, IU16, val)
{
	sys_addr	temp_val;

	sub_note_trace2(SAS_VERBOSE, "c_sas_storew addr=%x, val=%x\n", addr, val);

	addr &= SasWrapMask;
	checkAccess(addr);

	if ((addr + 1) < Length_of_M_area) {
		temp_val = readSelfMod(addr, sizeof(IU16));

		switch (temp_val) {
		case SAS_RAM:
			(*(IU16 *) getPtrToPhysAddrByte(addr)) = val;
			break;

#ifdef	LIM
		case SAS_MM_LIM:
			(*(IU16 *) getPtrToPhysAddrByte(addr)) = val;
			LIM_w_write(addr);
			break;
#endif

		case SAS_INACCESSIBLE:
		case SAS_ROM:
			/* No ROM_fix_sets !!! Yeh !!! */
			break;

		default:
			printf("Unknown Sas type\n");
			force_yoda();

		case SAS_VIDEO:
			temp_func = read_w_write_ptrs(temp_val);
			(*temp_func) (addr, val);
#if VIDEO_STRATEGY == 1
			(*(IU16 *) getPtrToPhysAddrByte(addr)) = val;
			(*(IU16 *) (haxm_videocmp_copy + addr - gvi_pc_low_regen)) = val;
#endif
			break;
		}

	} else
		printf("Word written outside M %x\n", addr);
}

GLOBAL void 
sas_PW32 IFN2(PHY_ADDR, addr, IU32, val)
{
	sys_addr	temp_val;

	sub_note_trace2(SAS_VERBOSE, "c_sas_storew addr=%x, val=%x\n", addr, val);

	addr &= SasWrapMask;
	checkAccess(addr);

	if ((addr + 3) < Length_of_M_area) {
		temp_val = readSelfMod(addr, sizeof(IU32));

		switch (temp_val) {
		case SAS_RAM:
			(*(IU32 *) getPtrToPhysAddrByte(addr)) = val;
			break;

#ifdef	LIM
		case SAS_MM_LIM:
			(*(IU32 *) getPtrToPhysAddrByte(addr)) = val;
			LIM_w_write(addr);
			LIM_w_write(addr+2);
			break;
#endif

		case SAS_INACCESSIBLE:
		case SAS_ROM:
			/* No ROM_fix_sets !!! Yeh !!! */
			break;

		default:
			printf("Unknown Sas type\n");
			force_yoda();

		case SAS_VIDEO:
#ifdef C_VID
			write_dword_ev_glue(addr, val);
#else
			temp_func = read_w_write_ptrs(temp_val);
			(*temp_func) (addr, (IU16)val);
			(*temp_func) (addr+2, (IU16)(val >> 16));
#endif
#if VIDEO_STRATEGY == 1
			(*(IU32 *) getPtrToPhysAddrByte(addr)) = val;
			(*(IU32 *) (haxm_videocmp_copy + addr - gvi_pc_low_regen)) = val;
#endif
			break;
		}

	} else
		printf("DWord written outside M %x\n", addr);
}

/*(
 *======================== sas_PWS =================================
 * sas_PWS
 *
 * Purpose
 *	This function writes a block of memory into Intel memory
 *	from host memory.  It is the physical address equivalent of
 *	sas_stores.
 *
 * Input
 *	Destination	Intel physical address
 *	Source		host address
 *	Length		number of IU8s to transfer
 *
 * Outputs
 *	None.
)*/

GLOBAL void
sas_PWS IFN3(PHY_ADDR, Destination, IU8 *, Source, PHY_ADDR, Length)
{
	sys_addr i, temp_val;

	Destination &= SasWrapMask;

	switch (temp_val = readSelfMod(Destination, Length)) {
		case SAS_RAM:
			RtlCopyMemory(getPtrToPhysAddrByte(Destination), Source, Length);
			break;

#ifdef	LIM
		case SAS_MM_LIM:
			RtlCopyMemory(getPtrToPhysAddrByte(Destination), Source, Length);
			LIM_str_write(Destination, Length);
			break;
#endif

		case SAS_INACCESSIBLE:
		case SAS_ROM:
			/* No ROM_fix_sets !!! Yeh !!! */
			break;

		default:
			printf("Unknown Sas type\n");
			force_yoda();

		case SAS_VIDEO:
#ifdef C_VID
			temp_func = read_b_move_ptrs(temp_val);
			(*temp_func)(Destination, Source, Length, 1);
#else
			if (Length % 2 == 0)
			{
				temp_func = read_w_write_ptrs(temp_val);
				for (i=0; i<Length; i+=2)
					(*temp_func) (Destination+i, *((IU16*)(Source+i)));
			}
			else
			{
				temp_func = read_b_write_ptrs(temp_val);
				for (i=0; i<Length; i++)
					(*temp_func) (Destination+i, Source[i]);
			}
#endif
#if VIDEO_STRATEGY == 1
			RtlCopyMemory(getPtrToPhysAddrByte(Destination), Source, Length);
			RtlCopyMemory(haxm_videocmp_copy + Destination - gvi_pc_low_regen, Source, Length);
#endif
			break;
	}
}

/*(
 *======================== sas_PWS_no_check ==========================
 * sas_PWS_no_check
 *
 * Purpose
 *	This function writes a block of memory from into Intel memory
 *	from host memory.  It is the physical address equivalent of
 *	sas_stores_no_check.
 *
 * Input
 *	dest	Intel physical address
 *	src	host address
 *	length	number of IU8s to transfer
 *
 * Outputs
 *	None.
 *
 * Description
 *	Just call c_sas_PWS()
)*/
GLOBAL void
sas_PWS_no_check IFN3(PHY_ADDR, dest, IU8 *, src, PHY_ADDR, length)
{
	sas_PWS(dest, src, length);
}

/*(
 *======================== sas_PRS =================================
 * sas_PRS
 *
 * Purpose
 *	This function reads a block of memory from  Intel memory
 *	into host memory.  It is the physical address equivalent of
 *	sas_loads.
 *
 * Input
 *	src	Intel physical address
 *	dest	host address
 *	length	number of IU8s to transfer
 *
 * Outputs
 *	None.
)*/

GLOBAL void
sas_PRS IFN3(PHY_ADDR, Source, IU8 *, Destination, PHY_ADDR, Length)
{
	IU8 *destP;

	Source &= SasWrapMask;
	switch (readSelfMod(Source, Length)) {
#ifdef	EGG
	case SAS_VIDEO:
		temp_func = read_pointers.str_read;
		(*temp_func) (Destination, Source, Length);
		break;
#endif
	case SAS_DANGEROUS:
		for (destP = Destination; destP < (Destination + Length); destP++) {
			*destP = sas_PR8(Source);
			Source++;
		}
		break;
	case SAS_INACCESSIBLE:
		RtlFillMemory(Destination, Length, 0xFF);
		break;

	case SAS_ROM:
	case SAS_RAM:
	default:
		RtlCopyMemory(Destination, getPtrToPhysAddrByte(Source), Length);
		break;
	}
}

/*(
 *======================== sas_PRS_no_check ===========================
 * sas_PRS_no_check
 *
 * Purpose
 *	This function reads a block of memory from  Intel memory
 *	into host memory.  It is the physical address equivalent of
 *	sas_loads_no_check.
 *
 * Input
 *	src	Intel physical address
 *	dest	host address
 *	length	number of IU8s to transfer
 *
 * Outputs
 *	None.
 *
 * Description
 *	Just call sas_PRS.
)*/

GLOBAL void
sas_PRS_no_check IFN3(PHY_ADDR, src, IU8 *, dest, PHY_ADDR, length)
{
	sas_PRS(src, dest, length);
}


/* This is a custom function, that does a transfer physical -> physical
 *
 * This may be needed in hax_handle_fastmmio for fast memory copy
 */
GLOBAL void
sas_PRWS IFN3(PHY_ADDR, Source, PHY_ADDR, Destination, PHY_ADDR, Length)
{
	sys_addr i;
	SAS_MEM_TYPE src_type;

	src_type = readSelfMod(Source, Length);

#ifdef EGG
	/*
	 * VIDEO destination: per-byte read+write loop. Same correctness
	 * argument as in sas_move_bytes_forward — bulk VIDEO -> VIDEO path
	 * mishandles latch semantics and plane distribution, and bulk
	 * RAM -> VIDEO via sas_PWS doesn't go through cvidc byte dispatch.
	 * Per-element via sas_PR8 + sas_PW8 mirrors the per-instruction
	 * REP MOVSB write semantics.
	 */
	if (readSelfMod(Destination, Length) == SAS_VIDEO)
	{
		for (i = 0; i < Length; i++) {
			sas_PW8(Destination + i, sas_PR8(Source + i));
		}
		return;
	}
#endif /* EGG */

	switch (src_type) {
		case SAS_MM_LIM:
		case SAS_RAM:
		case SAS_ROM:
		default:
			sas_PWS(Destination, getPtrToPhysAddrByte(Source), Length);
			break;

#ifdef	EGG
		case SAS_VIDEO:
			/* VIDEO source, non-VIDEO dest (VIDEO dest handled above). */
			switch (readSelfMod(Destination, Length)) {
				case SAS_RAM:	/* Video -> RAM */
					temp_func = read_pointers.str_read;
					(*temp_func) (getPtrToPhysAddrByte(Destination), Source, Length);
					break;

#ifdef	LIM
				case SAS_MM_LIM:	/* Video -> LIM */
					temp_func = read_pointers.str_read;
					(*temp_func) (getPtrToPhysAddrByte(Destination), Source, Length);
					LIM_str_write(Destination, Length);
					break;
#endif /*LIM */

				case SAS_INACCESSIBLE:
				case SAS_ROM:
					/* No ROM_fix_sets !!! Yeh !!! */
					break;

				default:
					printf("Unknown Sas type\n");
					force_yoda();
					break;
			}
			break;
#endif /* EGG */

		case SAS_INACCESSIBLE:
		case SAS_DANGEROUS:
			for (i = 0; i < Length; i++)
				sas_PW8(Destination + i, sas_PR8(Source + i));
			break;
	}
}


#define BIOS_VIRTUALISE_SEGMENT  0xf000
/*(
 *========================== biosDoInst ===================================
 * biosDoInst
 *
 * Purpose
 *	This function executes the instruction at the requested offset,
 *	saving CS and IP across it.
 *
 * Input
 *	vCS, vEIP, vEAX, vDS, vEDX	The values to used for the
 *					virtualised instruction.
 *
 * Outputs
 *	The value returned in EAX after virtualisation.
 *
 * Description
 *	Use host_simulate to execute an instruction in the bios1.rom
)*/

LOCAL IU32
biosDoInst IFN5(IU16, vCS, LIN_ADDR, vEIP, IU32, vEAX, IU16, vDS, IU32, vEDX)
{
	SAVED IBOOL first = TRUE;
	SAVED IBOOL trace_bios_inst = FALSE;
	SAVED int bodgeAdjustment = 0;

	IU16 savedCS;
	IU32 savedEIP;
	IU32 savedEAX;
	IU16 savedDS;
	IU32 savedEDX;
	IU32 savedEBP;
	IU32 result;

	if (first)
	{
		if (sas_w_at(0xF3030) == 0x9066)
		{
			/* These are still Keith's roms with garbage as
			 * first two bytes of each entry point
			 */
			bodgeAdjustment = 2;
			fprintf(stderr, "**** Warning: The bios1.rom is out of date. This Ccpu486 will not run Win/E\n");
		}
		if (getenv("biosDoInst") != NULL)
			trace_bios_inst = TRUE;
		first = FALSE;
	}

	savedCS  = getCS();
	savedEIP = getEIP(); //GetInstructionPointer();
	savedEAX = getEAX();
	savedDS  = getDS();
	savedEDX = getEDX();
	savedEBP = getEBP();

	setCS (vCS );
	setEIP(vEIP + bodgeAdjustment);
	setEAX(vEAX);
	setDS (vDS );
	setEDX(vEDX);

	/*
	 * Call the CPU.
	 */

	if (trace_bios_inst)
	{
		always_trace3("biosDoInst: @ %04x, EAX %08x, EDX %08X", vEIP, vEAX, vEDX);
	}

	host_simulate();

	result = getEAX();

	/* Restore the registers to the original values */

	setCS (savedCS );
	setEIP(savedEIP);
	setEAX(savedEAX);
	setDS (savedDS );
	setEDX(savedEDX);
	setEBP(savedEBP);

	return (result);
}



/*(
 *============================= IOVirtualised ==================================
 * IOVirtualised
 *
 * Purpose
 *	This function checks whether executing an IO instruction
 *	of the indicated width would cause an exception to go off.
 *
 *	If so, it executes the indicated identical instruction in ROM.
 *	This will allow the exception to go off correctly, and allow the
 *	Intel OS (e.g. Windows) to catch and virtualise it if it wishes.
 *
 *	Otherwise it will be up to the caller to execute the actual IO.
 *
 * Input
 *	port	The port to use
 *	value	Where output values are taken from, and input values
 *		written to. NOTE: THIS MUST BE AN IU32*, WHATEVER THE WIDTH.
 *	offset	The offset in the ROM of the equivalent instruction.
 *	width	byte, word, dword
 *
 * Outputs
 *	True if the operation went to ROM, false if the caller needs to do it.
 *
 * Description
 *	If this is an illegal IO operation, we need to save CS, IP, EAX, EDX
 *	and call host_simulate to execute the equivalent instruction in ROM.
)*/

GLOBAL IBOOL
IOVirtualised IFN4(io_addr, port, IU32 *, value, LIN_ADDR, offset, IU8, width)
{
	if (getVM())
	{
		*value = biosDoInst(BIOS_VIRTUALISE_SEGMENT, offset, *value, 0, port);
		return(TRUE);
	} else if ( getCPL() > getIOPL()) {
		
		switch (port)
		{
		case 0x23c:	/* mouse */
		case 0x23d:	/* mouse */
		case 0xa0:	/* ica */
		case 0x20:	/* ica */
			break;
		default:
			always_trace1("Virtualising PM I/O code called, port =0x%x\n",
				port);
		}

		if (!selectors_set) {
			sub_note_trace0(SAS_VERBOSE, 
				"Exiting as selectors not set !\n");
			return FALSE;
		}
		*value = biosDoInst(code_sel, offset, *value, 0, port);
		return(TRUE);
	}
	return FALSE;
}



LOCAL IUM8 
bios_read_accreq(void)
{
	IUM8 access_request = 0; /* BIT 0 = R/W */
				 /* BIT 1 = U/S */
				 /* BIT 2 = Ensure A and D are valid */

	/* Note default access_request (0) is Supervisor Read */

	/* We don't specifically disallow Protected Mode calls, they
	   are not designed to happen, but the Video at least has a habit
	   of reading BIOS variables on host timer ticks. We treat such
	   requests more leniently than V86 Mode requests, by not insisting
	   that the access and dirty bits are kosher.
	 */

	if ( getCPL() != 3 )
	{
		access_request = access_request | PG_U;
	}

	/* Beware V86 Mode, be strict about access and dirty bits */

	if ( getVM() )
	{
		access_request = access_request | 0x4;
	}

	return access_request;
}

/* Read byte from memory */
LOCAL IU8 
bios_read_byte IFN1(LIN_ADDR, linAddr)
{
	PHY_ADDR phyAddr;

	/* If no paging on, then no problem */

	if (!getPG())
	{
		return(sas_PR8((PHY_ADDR)linAddr));
	}

	/* Go translate the address. */

	if (xtrn2phy(linAddr, bios_read_accreq(), &phyAddr))
	{
		return((IU8)sas_PR8(phyAddr));
	}

	/* Handle Address Mapping Failure... */

	if(getPE() && !getVM())
	{
		always_trace1("Virtualising PM byte read, lin address 0x%x", linAddr);

		if (!selectors_set)
			return 0;

		return ((IU8)biosDoInst(code_sel, BIOS_RDB_OFFSET, 0, data_sel, linAddr));
	}
	else
	{
		sub_note_trace1(SAS_VERBOSE, "Page read VM virtualisation at 0x%x", linAddr);

		return ((IU8)biosDoInst(BIOS_VIRTUALISE_SEGMENT, BIOS_RDB_OFFSET, 0, data_sel, linAddr));
	}
}



/* Read word from memory */
LOCAL IU16
bios_read_word IFN1(LIN_ADDR, linAddr)
{
	PHY_ADDR phyAddr;

	/* If no paging on, then no problem */

	if (!getPG())
	{
		return(sas_PR16((PHY_ADDR)linAddr));
	}

	/* Go translate the address. Never called crossing a page boundary */

	if (xtrn2phy(linAddr, bios_read_accreq(), &phyAddr))
	{
		return((IU16)sas_PR16(phyAddr));
	}

	/* Handle Address Mapping Failure... */

	if(getPE() && !getVM())
	{
		always_trace1("Virtualising PM word read, lin address 0x%x", linAddr);

		if (!selectors_set)
			return 0;

		return ((IU8)biosDoInst(code_sel, BIOS_RDW_OFFSET, 0, data_sel, linAddr));
	}
	else
	{
		sub_note_trace1(SAS_VERBOSE, "Page read word VM virtualisation at 0x%x", linAddr);

		return ((IU8)biosDoInst(BIOS_VIRTUALISE_SEGMENT, BIOS_RDW_OFFSET, 0, data_sel, linAddr));
	}
}


/* Read double from memory */
LOCAL IU32
bios_read_double IFN1(LIN_ADDR, linAddr)
{
	PHY_ADDR phyAddr;

	/* If no paging on, then no problem */

	if (!getPG())
	{
		return(sas_PR32((PHY_ADDR)linAddr));
	}

	/* Go translate the address. Never called crossing a page boundary */

	if (xtrn2phy(linAddr, bios_read_accreq(), &phyAddr))
	{
		return(sas_PR32(phyAddr));
	}

	/* Handle Address Mapping Failure... */

	if(getPE() && !getVM())
	{
		always_trace1("Virtualising PM double read, lin address 0x%x", linAddr);

		if (!selectors_set)
			return 0;

		return ((IU8)biosDoInst(code_sel, BIOS_RDD_OFFSET, 0, data_sel, linAddr));
	}
	else
	{
		sub_note_trace1(SAS_VERBOSE, "Page read double VM virtualisation at 0x%x", linAddr);

		return ((IU8)biosDoInst(BIOS_VIRTUALISE_SEGMENT, BIOS_RDD_OFFSET, 0, data_sel, linAddr));
	}
}


/* Write byte to memory */
LOCAL void 
bios_write_byte IFN2(LIN_ADDR, linAddr, IU8, value)
{
	PHY_ADDR addr;

	/* If no paging on, then no problem */

	if (!getPG())
	{
		sas_PW8((PHY_ADDR)linAddr, value);
		return;
	}
	
	/* Go translate the address. */
	if (xtrn2phy(linAddr, bios_read_accreq() | PG_W, &addr))
	{
		sas_PW8(addr, value);
		return;
	}

	/* Handle Address Mapping Failure... */

	if(getPE() && !getVM())
	{
		always_trace1("Virtualising PM byte write, lin address 0x%x", linAddr);
		
		if (!selectors_set)
			return;

		(void)biosDoInst(code_sel, BIOS_WRTB_OFFSET, (IU32)value, data_sel, linAddr);
	}
	else
	{
		sub_note_trace1(SAS_VERBOSE, "Page write VM virtualisation at 0x%x", linAddr);

		(void)biosDoInst(BIOS_VIRTUALISE_SEGMENT, BIOS_WRTB_OFFSET, (IU32)value, data_sel, linAddr);
	}
}


/* Write word to memory */
LOCAL void 
bios_write_word IFN2(LIN_ADDR, linAddr, IU16, value)
{
	PHY_ADDR addr;

	/* If no paging on, then no problem */

	if (!getPG())
	{
		sas_PW16((PHY_ADDR)linAddr, value);
		return;
	}
	
	/* Go translate the address. Never called crossing a page boundary */
	if (xtrn2phy(linAddr, bios_read_accreq() | PG_W, &addr))
	{
		sas_PW16(addr, value);
		return;
	}

	/* Handle Address Mapping Failure... */

	if(getPE() && !getVM())
	{
		always_trace1("Virtualising PM word write, lin address 0x%x", linAddr);
		
		if (!selectors_set)
			return;

		(void)biosDoInst(code_sel, BIOS_WRTW_OFFSET, (IU32)value, data_sel, linAddr);
	}
	else
	{
		sub_note_trace1(SAS_VERBOSE, "Page word write VM virtualisation at 0x%x", linAddr);

		(void)biosDoInst(BIOS_VIRTUALISE_SEGMENT, BIOS_WRTW_OFFSET, (IU32)value, data_sel, linAddr);
	}
}


/* Write double to memory */
LOCAL void 
bios_write_double IFN2(LIN_ADDR, linAddr, IU32, value)
{
	PHY_ADDR addr;

	/* If no paging on, then no problem */

	if (!getPG())
	{
		sas_PW32((PHY_ADDR)linAddr, value);
		return;
	}
	
	/* Go translate the address. Never called crossing a page boundary */
	if (xtrn2phy(linAddr, bios_read_accreq() | PG_W, &addr))
	{
		sas_PW32(addr, value);
		return;
	}

	/* Handle Address Mapping Failure... */

	if(getPE() && !getVM())
	{
		always_trace1("Virtualising PM double write, lin address 0x%x", linAddr);
		
		if (!selectors_set)
			return;

		(void)biosDoInst(code_sel, BIOS_WRTD_OFFSET, (IU32)value, data_sel, linAddr);
	}
	else
	{
		sub_note_trace1(SAS_VERBOSE, "Page double write VM virtualisation at 0x%x", linAddr);

		(void)biosDoInst(BIOS_VIRTUALISE_SEGMENT, BIOS_WRTD_OFFSET, (IU32)value, data_sel, linAddr);
	}

}

/* Stubs */
GLOBAL void sas_overwrite_memory IFN2(PBYTE, addr, ULONG, type)
{
	UNUSED(addr);
	UNUSED(type);
}



/*(
 *======================= c_SasRegisterVirtualSelectors =========================
 *
 * Purpose
 *	The Sas virtualisation handler requires a code+data selector which
 *	are available in protected mode (when called from say the Insignia
 *	host windows driver.
 *	Our current experimental implementation does not worry about how
 *	long these live.
 *
 *	It is expected that this functionality should be moved from the
 *	windows driver itself, to the Insignia VxD so that correct
 *	initialisation/termination can be handled.
)*/

GLOBAL IBOOL sas_init_pm_selectors IFN2(IU16, sel1, IU16, sel2)
{
	IU32	addr;

	always_trace0 ("c_SasRegisterVirtualSelectors called\n");

	addr = getLDT_BASE() + ((data_sel = sel1) & ~7);

	/* Construct a flat writable data segment */

	sas_storedw (addr, 0x0000FFFF);
	sas_storedw (addr+4, 0x008ff300);

	addr = getLDT_BASE() + ((code_sel = sel2) & ~7);

	/* Construct a code segment with base 0xf0000 and large limits */

	sas_storedw (addr, 0x0000FFFF);
	sas_storedw (addr+4, 0x008f9f0f);

	selectors_set = TRUE;

	always_trace2 ("Set code_sel = %x, data_sel = %x\n",
		code_sel, data_sel);
    return TRUE;
}

/*(
 *========================= Cpu_find_dcache_entry ==============================
 * Cpu_find_dcache_entry
 *
 * Purpose
 *	In an assembler CPU, this function allows non-CPU code to try and look
 *	up a selector in the dcache, rather than constructing it from memory.
 *	We don't have a dcache, but it gives us a chance to intercept
 *	CS selector calls, as the CS descriptor may not be available.
 *
 * Input
 *	selector,	The selector to look-up
 *
 * Outputs
 *	returns		TRUE if selector found (i.e. CS in our case)
 *	base		The linear address of the base of the segment.
 *
 * Description
 *	Just look out for CS, and return the stored base if we get it.
)*/

GLOBAL IBOOL 
Cpu_find_dcache_entry (USHORT seg, LIN_ADDR * base)
{
	return FALSE;
}

