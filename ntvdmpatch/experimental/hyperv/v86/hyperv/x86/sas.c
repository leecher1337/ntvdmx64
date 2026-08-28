/*++

Copyright (c) 2019  leecher@dose.0wnz.at
Copyright (c) 2026  leecher@dose.0wnz.at  (Hyper-V / WHP port)

Module Name:

    sas.c
Abstract:

    Memory-mapping glue between NTVDM and the Windows Hypervisor Platform.

    The bulk of this file is hypervisor-agnostic SAS code shared with the
    HAXM port.  The hypervisor-touching primitives have been retargeted:

      haxm_alloc      -> WHvMapGpaRange of a VirtualAlloc'd backing page set
      haxm_mprotect   -> remap (unmap + map) with reduced permissions
      hax_set_ram     -> WHvMapGpaRange for RAM/ROM, drop unmapped for INVALID
      xtrn2phy        -> WHvTranslateGva
      hax_MapViewOfSection -> retired (the MEM_DOS_LIM hack is unnecessary on
                              WHP: guest physical memory is decoupled from
                              host VA)

Author:

    leecher@dose.0wnz.at (leecher1337)

Environment:

    User mode only

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
extern int    getVramPerPlane(void);    /* ega_prts.c: 1 once a subset Map-Mask (per-plane) was used */
extern int    getVramPlanarTrap(void);  /* ega_prts.c: 1 in a planar EGA/VGA BIOS mode (0Dh..12h) */
extern int    getCurrentVideoMode(void); /* ega_prts.c: current vd_video_mode byte (diagnostic) */
extern void hypervvm_init();

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

/*
 * On HAXM, haxm_alloc registered a host VA range with the driver so the
 * latter could later wire chunks of it into the guest physical space.  On
 * WHP we simply do nothing here -- the host memory must already be page
 * aligned (it is: host_sas_init / malloc gives us page-aligned blocks,
 * Start_of_M_area is the base), and the actual GPA mapping happens in
 * hax_set_ram() via WHvMapGpaRange.  We keep the function around so the
 * sas_init() call site is portable.
 */
GLOBAL VOID haxm_alloc(
    IN sys_addr Low,
    IN sys_addr Size
)
{
    (void)Low; (void)Size;
}

/*
 * Change the permissions on an already-mapped GPA range.  WHP does not
 * expose a "modify flags in place" call, so we unmap and re-map.  The host
 * VA is recovered from Start_of_M_area + Addr (the same arithmetic used by
 * hax_set_ram).  `protection` is a HAX_RAM_PERM_* bitmask.
 */
GLOBAL VOID haxm_mprotect(
    IN sys_addr Addr,
    IN sys_addr Size,
    IN DWORD protection)
{
    WHV_MAP_GPA_RANGE_FLAGS flags = WHvMapGpaRangeFlagNone;

    if (protection & 0x1) flags |= WHvMapGpaRangeFlagRead;
    if (protection & 0x2) flags |= WHvMapGpaRangeFlagWrite;
    if (protection & 0x4) flags |= WHvMapGpaRangeFlagExecute;

    /* WHvUnmapGpaRange + WHvMapGpaRange is the documented way to change
     * permissions on a range.  Errors are swallowed (same behaviour as the
     * HAXM IOCTL: it had no callers that checked the return). */
    hyperv_unmap_gpa_range((uint64_t)Addr, (uint64_t)Size);
    if (flags != WHvMapGpaRangeFlagNone) {
        hyperv_map_gpa_range((uint64_t)Addr, (uint64_t)Size,
                             (void *)(Start_of_M_area + Addr), flags);
    }
}

/*
 * The HAXM ntvdm fork shipped a kernel-mode hax_MapViewOfSection that
 * bypassed the user-mode MEM_DOS_LIM check in MiMapViewOfSection so that
 * EMS pages could be mapped into the host process below the 1 MB line.
 *
 * On WHP this hack is no longer needed: the guest physical address space
 * is fully decoupled from the host process VA layout, so EMS pages are
 * just regular WHvMapGpaRange operations against whatever host pages we
 * pick.  We keep the symbol as a pass-through to the standard system call
 * in case other glue still calls it.
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
    return NtMapViewOfSection(SectionHandle, ProcessHandle, BaseAddress,
                              ZeroBits, CommitSize, SectionOffset, ViewSize,
                              InheritDisposition, AllocationType, Protect);
}

#if VIDEO_STRATEGY == 1

/* ------------------------------------------------------------------------
 *
 *  VRAM sync, Hyper-V backend.
 *
 *  The HAXM port used a per-page byte scan with 0xFF as a "consumed"
 *  marker (any byte != 0xFF was a fresh write; after forwarding the page
 *  was memset to 0xFF).  That had three problems:
 *
 *    1. Guest writes of 0xFF were silently dropped.
 *    2. VRAM was destroyed after each sync; guest read-back returned 0xFF.
 *    3. Every page was scanned on every VM exit, regardless of activity.
 *
 *  The Hyper-V mapping for SAS_VIDEO is established with
 *  WHvMapGpaRangeFlagTrackDirtyPages, so we can ask WHP precisely which
 *  4 KB pages the guest wrote since the last sync.  For each dirty page
 *  we compare the live VRAM against haxm_videocmp_copy (the shadow), push
 *  changed bytes/words through CVIDC, and refresh the shadow with a plain
 *  memcpy.  VRAM is left intact, so the guest can read back what it wrote.
 *
 *  On the first call -- or on any host where the dirty-bitmap query is
 *  refused -- we fall back to an unconditional shadow-compare across the
 *  whole aperture.
 *
 *  *** WHP dirty-bitmap: perf optimization + safety net ***
 *
 *  Once the I/O fast-path (monitor.c) removes register marshalling, the
 *  ~64 KB full-aperture scan here dominates per-exit cost -- a planar
 *  blitter drives one sync per Map-Mask write (~17000/s), and scanning the
 *  whole aperture each time to find a few changed bytes is ~80% of the work.
 *  WHvQueryGpaRangeDirtyBitmap tells us which 4 KB pages the guest actually
 *  wrote, so we scan only those.
 *
 *  Caveat: the query clears the bits and WHP's write-detection re-arm is not
 *  always reliable -- a page can read back "clean" even after a guest write,
 *  which would skip its bytes and (historically, with the old non-self-
 *  healing diff) leave the permanent vertical-streak "rain".  Two things now
 *  make the bitmap safe:
 *    1. VRAM_GRAPHICS_SELFHEAL re-sends a page's whole content whenever it IS
 *       scanned, so a wrongly-skipped page self-corrects the next time the
 *       bitmap reports it.
 *    2. We force an unconditional full scan every VRAM_BITMAP_FULLSCAN_EVERY
 *       syncs, bounding any bitmap miss to a sub-millisecond transient
 *       instead of a persistent artifact.
 *
 *  (Earlier this was disabled on the theory that the bitmap caused the
 *  corruption; the real cause was the missing self-heal -- so with self-heal
 *  in place the bitmap is just a scan-narrowing perf win.)
 *
 * ------------------------------------------------------------------------ */

/* 0 = full-scan every sync (safe, but ~80% of per-exit time under the I/O
 *     fast-path).  1 = scan only WHP-reported-dirty pages, with a periodic
 *     full-scan safety net (see caveat above). */
/* Re-enabled: the SKYROADS "bleed" was NOT the bitmap (it was the destructive
 * self-heal corrupting VRAM read-back -- now fixed by the per-plane
 * discriminator).  The bitmap is a pure perf win (scan only WHP-reported-dirty
 * pages instead of the whole 64 KB aperture every sync), with the periodic
 * full-scan safety net (VRAM_BITMAP_FULLSCAN_EVERY) covering any dropped
 * page.  Self-heal/shadow-diff both self-correct a dropped page on the next
 * frame's rewrite. */
#define VRAM_USE_WHP_DIRTY_BITMAP 1

/* When the bitmap is on, force a full aperture scan every Nth sync so any
 * page WHP wrongly reported clean is re-sent within N syncs (~N/17000 s for
 * DOOM).  Keep small enough that a miss is imperceptible. */
#define VRAM_BITMAP_FULLSCAN_EVERY 16

/* Graphics-mode change detection:
 *   1 = HAXM-equivalent self-healing: forward every byte the guest wrote
 *       since the last sync (0xFF "consumed" marker), then reset VRAM to
 *       0xFF.  A full-frame blitter (DOOM, Build, ...) rewrites the whole
 *       frame each pass, so a stale byte in the CVIDC regen is overwritten
 *       on the next frame.  This is what the *working* HAXM backend does.
 *       Trade-offs: a genuine guest write of value 0xFF is dropped, and CPU
 *       read-back of VRAM returns 0xFF (fine for write-only blitters).
 *   0 = non-destructive shadow-diff: only forward bytes whose value differs
 *       from the previous frame.  Cheaper, preserves read-back, but does NOT
 *       self-heal a desynced CVIDC byte -- suspected cause of the WHP-only
 *       vertical-streak corruption (HAXM, which self-heals, is clean). */
/* Master enable for the destructive 0xFF self-heal sync.  The actual per-sync
 * choice is now self-heal ONLY when the guest renders per-plane (getVramPerPlane
 * -- DOOM's Mode-Y subset Map-Masks need it to avoid shadow-diff "rain"); games
 * that write all planes at once (SKYROADS, Map-Mask 0x0F) use the non-destructive
 * shadow-diff so their VRAM read-back survives (no progress-bar "bleed").
 * Set 0 to force shadow-diff everywhere. */
#define VRAM_GRAPHICS_SELFHEAL 1

/* Minimum gap between visible host-surface blits (host_graphics_tick), in
 * ms.  ~15 ms ≈ 66 Hz.  Decouples the expensive blit from the per-exit VRAM
 * sync; see haxm_sync_vram(). */
#define VRAM_BLIT_INTERVAL_MS 15
static DWORD s_vram_last_blit  = 0;
static BOOL  s_vram_blit_pending = FALSE;

static UINT64 *vram_dirty_bitmap = NULL;
static DWORD   vram_dirty_bitmap_qwords = 0;
static DWORD   vram_dirty_bitmap_for_size = 0;  /* what size it was sized for */
static BOOL    vram_shadow_primed = FALSE;

static BOOL vram_resize_bitmap(DWORD vram_size)
{
    DWORD pages, qwords;

    if (vram_size == vram_dirty_bitmap_for_size && vram_dirty_bitmap)
        return TRUE;

    pages  = (vram_size + 0xFFF) >> 12;
    qwords = (pages + 63) >> 6;

    free(vram_dirty_bitmap);
    vram_dirty_bitmap = (UINT64 *)calloc(qwords, sizeof(UINT64));
    if (!vram_dirty_bitmap) {
        vram_dirty_bitmap_qwords  = 0;
        vram_dirty_bitmap_for_size = 0;
        return FALSE;
    }
    vram_dirty_bitmap_qwords   = qwords;
    vram_dirty_bitmap_for_size = vram_size;
    return TRUE;
}

/* Forward a single dirty page to CVIDC, then refresh the shadow.
 * Width is WORD in text mode (a 2-byte attr+char cell), BYTE otherwise. */
static void sync_one_page(DWORD gpa, PBYTE shadow, PBYTE vram,
                          DWORD scan_size, DWORD mode_type, BOOL *any_changed)
{
    DWORD j;
    BOOL  page_changed = FALSE;
    int   use_selfheal = 0;   /* set in the graphics branch; see below */

    if (mode_type == TEXT)
    {
        PDWORD pdwSrc = (PDWORD)shadow;
        PDWORD pdwDst = (PDWORD)vram;
        DWORD  nd     = scan_size / sizeof(DWORD);
        DWORD  nw     = scan_size / sizeof(WORD);
        void (*write_w)() = read_w_write_ptrs(SAS_VIDEO);

        /* DWORD-stride fast skip: most cells are unchanged between frames. */
        for (j = 0; j < nd; j++) {
            if (pdwSrc[j] != pdwDst[j]) {
                PWORD pwSrc = (PWORD)shadow;
                PWORD pwDst = (PWORD)vram;
                DWORD w     = j * (sizeof(DWORD) / sizeof(WORD));
                for (; w < nw; w++) {
                    if (pwSrc[w] != pwDst[w]) {
                        (*write_w)((PBYTE)gpa + w * sizeof(WORD), pwDst[w]);
                        page_changed = TRUE;
                    }
                }
                break;
            }
        }
    }
    else /* graphics (planar or chained) -- byte-granular */
    {
        PDWORD pdwDst = (PDWORD)vram;
        DWORD  nd     = scan_size / sizeof(DWORD);
        void (*write_b)() = read_b_write_ptrs(SAS_VIDEO);

        /* Self-heal (0xFF "consumed" marker + memset, destructive) vs
         * non-destructive shadow-diff, chosen by getVramPerPlane():
         *
         *  - PER-PLANE renderers (DOOM Mode Y: subset Map-Masks 0x01/02/04/08)
         *    need self-heal.  When adjacent same-colour columns land in
         *    different planes, the flat aperture byte is unchanged, so a
         *    shadow-diff would skip it and leave a plane stale -> "rain".  The
         *    0xFF reset forces every written byte to be re-sent.  These games
         *    don't read VRAM back, so destroying it is harmless.
         *
         *  - ALL-PLANE writers (SKYROADS: Map-Mask 0x0F only) have no inter-
         *    plane same-value gap, so shadow-diff is correct AND it leaves
         *    VRAM intact -- essential because SKYROADS reads the framebuffer
         *    back to draw its progress bar; the 0xFF memset would feed it 0xFF
         *    (the magenta "bleed").
         *
         * VRAM_GRAPHICS_SELFHEAL=0 forces shadow-diff everywhere. */
        use_selfheal = VRAM_GRAPHICS_SELFHEAL && getVramPerPlane();

        if (use_selfheal) {
            /* forward every byte != 0xFF (written since the last 0xFF reset);
             * the page is reset to 0xFF below. */
            for (j = 0; j < nd; j++) {
                if (pdwDst[j] != 0xFFFFFFFFu) {
                    DWORD b = j * sizeof(DWORD);
                    for (; b < scan_size; b++) {
                        if (vram[b] != 0xFF) {
                            (*write_b)((PBYTE)gpa + b, vram[b]);
                            page_changed = TRUE;
                        }
                    }
                    break;
                }
            }
        } else {
            /* Non-destructive shadow-diff: forward only value-changed bytes. */
            PDWORD pdwSrc = (PDWORD)shadow;
            for (j = 0; j < nd; j++) {
                if (pdwSrc[j] != pdwDst[j]) {
                    DWORD b = j * sizeof(DWORD);
                    for (; b < scan_size; b++) {
                        if (shadow[b] != vram[b]) {
                            (*write_b)((PBYTE)gpa + b, vram[b]);
                            page_changed = TRUE;
                        }
                    }
                    break;
                }
            }
        }
    }

    if (page_changed) *any_changed = TRUE;

    /* Refresh the change-detection baseline for the next sync. */
    if (mode_type == TEXT) {
        memcpy(shadow, vram, scan_size);
    } else if (use_selfheal) {
        /* Consumed-marker reset (only touch pages that actually changed, so
         * idle frames stay cheap).  Self-heals CVIDC on the next blit. */
        if (page_changed) memset(vram, 0xFF, scan_size);
    } else {
        memcpy(shadow, vram, scan_size);   /* non-destructive: keep VRAM intact */
    }
}

/* WHP_EGA_TRAP: master gate for the planar-EGA A0000 instruction-emulation
 * trap.  Set to 0 to fall back to the flat-buffer-only behaviour (Keen video
 * stays garbled, but the path is fully disabled).  Even when 1, the trap only
 * arms when the current BIOS mode is a planar EGA/VGA mode (getVramPlanarTrap);
 * the in-process decoder that services the faults is always compiled in. */
#ifndef WHP_EGA_TRAP
#define WHP_EGA_TRAP 1   /* planar-EGA A0000 per-write trap (kept for Keen4 etc.).  See
                          * HAXM_EGA_TRAP note: wpsnt stripes are the guest composing to the
                          * wrong buffer (timing), faithfully rendered by the trap; A/B-verified
                          * that WHP_EGA_TRAP 0 -> flat-only -> garbled-but-not-striped. */
#endif

/* Forward decls: defined just after haxm_sync_vram (need vram_invalidate_shadow). */
static void vram_apply_mapping(uint64_t lo, uint64_t size);
static void vram_reeval_trap(void);
static BOOL vram_is_trapped(void);

GLOBAL VOID haxm_sync_vram(VOID)
{
    DWORD vram_size, pages, i;
    DWORD mode_type;
    BOOL  any_changed = FALSE;
    BOOL  have_bitmap = FALSE;

    if (gvi_pc_high_regen < gvi_pc_low_regen) return;
    vram_size  = gvi_pc_high_regen - gvi_pc_low_regen + 1;
    pages      = (vram_size + 0xFFF) >> 12;
    mode_type  = getModeType();

    /* Re-evaluate the trap for the current mode on the region hax_set_ram last
     * established (hax_set_ram is the sole authority on the *region* -- using
     * its bounds here avoids remap thrash if they differ from gvi_pc_*).  In a
     * planar EGA/VGA mode this unmaps A0000 so the guest's latched/ALU writes
     * fault to the in-process decoder (monitor.c hyperv_emulate_vram_access),
     * which replays them through write_b with full CVIDC semantics; in
     * linear/text modes the
     * fast flat dirty-tracked buffer stays.  Idempotent -- a no-op once the
     * desired state is in place.  When trapped the flat buffer is not the
     * source of truth, so skip the shadow scan and just keep the host surface
     * painted. */
    vram_reeval_trap();
    if (vram_is_trapped()) {
        /* Rate-limit: every trapped guest write is its own VM exit, so this
         * runs per write -- blitting on each would be ruinous.  Same cadence
         * as the flat-path blit below. */
        DWORD now = GetTickCount();
        if ((now - s_vram_last_blit) >= VRAM_BLIT_INTERVAL_MS) {
            s_vram_last_blit = now;
            host_graphics_tick();
        }
        return;
    }

    /* DIAGNOSTIC: log the mode + per-plane state (and the sync path it
     * selects) once, whenever it changes -- per-plane (DOOM) -> self-heal,
     * all-plane (SKYROADS) -> shadow-diff. */
    {
        static int s_last_mode = -2, s_last_pp = -2;
        int pp = getVramPerPlane();
        if ((int)mode_type != s_last_mode || pp != s_last_pp) {
            char dbg[96];
            s_last_mode = (int)mode_type; s_last_pp = pp;
            sprintf(dbg, "VRAMSYNC mode_type=%d perplane=%d selfheal=%d\n",
                    (int)mode_type, pp, (VRAM_GRAPHICS_SELFHEAL && pp));
            OutputDebugStringA(dbg);
        }
    }

    /* Default (VRAM_USE_WHP_DIRTY_BITMAP == 0): no bitmap, so have_bitmap
     * stays FALSE and the loop below scans every page each sync -- matching
     * the known-good HAXM backend.  When the bitmap is enabled we instead
     * ask WHP which pages changed (read-and-clear) and skip clean ones; the
     * first call after a SAS_VIDEO mapping ignores it (vram_shadow_primed
     * == FALSE) and full-scans once to prime the shadow.  See the caveat at
     * the top of this file for why the bitmap is off by default. */
#if VRAM_USE_WHP_DIRTY_BITMAP
    {
        /* Force a full scan every Nth sync as a safety net for WHP's
         * occasionally-unreliable dirty re-arm (see caveat at top).  We still
         * query on those syncs so the dirty bits stay cleared/consistent. */
        static DWORD s_sync_count = 0;
        BOOL force_full = (++s_sync_count % VRAM_BITMAP_FULLSCAN_EVERY) == 0;
        if (vram_resize_bitmap(vram_size) && vram_shadow_primed) {
            have_bitmap = hyperv_query_dirty((uint64_t)gvi_pc_low_regen,
                                             (uint64_t)vram_size,
                                             vram_dirty_bitmap,
                                             vram_dirty_bitmap_qwords);
            if (force_full) have_bitmap = FALSE;   /* scan all pages this time */
        }
    }
#else
    /* Full-scan path (see caveat above): never skip pages. */
    (void)vram_resize_bitmap;
    (void)vram_shadow_primed;
#endif

    for (i = 0; i < pages; i++) {
        DWORD off       = i * 0x1000;
        DWORD scan_size = (off + 0x1000 <= vram_size) ? 0x1000
                                                      : (vram_size - off);
        PBYTE shadow    = (PBYTE)haxm_videocmp_copy + off;
        PBYTE vram      = (PBYTE)getPtrToPhysAddrByte(gvi_pc_low_regen + off);

        if (have_bitmap) {
            BOOL page_dirty = ((vram_dirty_bitmap[i >> 6] >> (i & 63)) & 1ULL) != 0;
            if (!page_dirty) continue;
        }

        sync_one_page(gvi_pc_low_regen + off, shadow, vram,
                      scan_size, mode_type, &any_changed);
    }

    /* Rate-limit the visible blit to ~display refresh.  haxm_sync_vram runs
     * on every VM exit, and with the I/O fast-path a planar blitter (DOOM)
     * generates ~16000 Map-Mask exits/s -- calling host_graphics_tick (full
     * planar->packed + GDI blit) on each was ~80% of per-exit time.  The
     * CVIDC regen is already current (write_b above); we only defer the
     * expensive host-surface blit.  Any change that arrives inside the
     * interval is picked up by the next sync past the deadline (syncs are
     * frequent during gameplay; the timer-driven host_graphics_tick covers
     * the idle case). */
    if (any_changed) {
        DWORD now = GetTickCount();
        s_vram_blit_pending = TRUE;
        if ((now - s_vram_last_blit) >= VRAM_BLIT_INTERVAL_MS) {
            s_vram_last_blit    = now;
            s_vram_blit_pending = FALSE;
            host_graphics_tick();
        }
    } else if (s_vram_blit_pending) {
        /* No new writes this sync, but a deferred frame is still waiting --
         * flush it once the interval has elapsed so the last frame of a burst
         * isn't left unpainted. */
        DWORD now = GetTickCount();
        if ((now - s_vram_last_blit) >= VRAM_BLIT_INTERVAL_MS) {
            s_vram_last_blit    = now;
            s_vram_blit_pending = FALSE;
            host_graphics_tick();
        }
    }
    vram_shadow_primed = TRUE;
}

/* Called from hax_set_ram when the VRAM mapping is (re)established, so the
 * next haxm_sync_vram() does an unconditional priming pass. */
static void vram_invalidate_shadow(void)
{
    vram_shadow_primed = FALSE;
}

/* ---- A0000 planar-EGA instruction-emulation trap -----------------------
 *
 * The aperture is normally a flat, dirty-tracked host buffer (fast, no exits)
 * which is correct for text and *linear* graphics modes (13h chain-4).  But
 * the planar EGA/VGA modes (0Dh,0Eh,0Fh,10h,11h,12h) drive A0000 through the
 * latch / set-reset / ALU / map-mask machinery: a single linear address fans
 * out to four planes, which a flat buffer cannot represent (this is the cause
 * of Commander Keen's garbled video).  For those modes we UNMAP the aperture
 * so every read/write faults to WHvRunVpExitReasonMemoryAccess, where the
 * in-process decoder (monitor.c hyperv_emulate_vram_access) decodes the
 * faulting instruction and replays each access through sas_PR/PW ->
 * CVIDC read_b/write_b, reproducing the hardware exactly.
 *
 * vram_apply_mapping() is the single owner of the aperture mapping for
 * VIDEO_STRATEGY 1; both haxm_sync_vram (every exit) and hax_set_ram (mode
 * change) call it.  It caches the last-applied (region, trapped) state and
 * only touches WHP when something actually changes, so the per-exit call is a
 * cheap comparison in the common case.
 * ------------------------------------------------------------------------ */
static BOOL     s_vram_trapped   = FALSE;
static uint64_t s_vram_map_lo    = 0;
static uint64_t s_vram_map_size  = 0;

static BOOL vram_is_trapped(void) { return s_vram_trapped; }

/* Runtime video strategy: 1 = fast flat aperture + selective trap (default), 3 =
 * full-CVIDC (route every VRAM touch through the fault->emulate->CVIDC path, like
 * CCPU).  Switched 1->3 (sticky) by video_strategy_autodetect() only when the
 * user opts in via the NTVDM_CVIDC env var; normal DOS keeps strategy 1.  UPDATE57. */
int             g_video_strategy = 1;

static void vram_apply_mapping(uint64_t lo, uint64_t size)
{
    BOOL want_trap = WHP_EGA_TRAP
                  && getVramPlanarTrap();
    uint64_t manage_hi;

    if (size == 0) return;

    /* No-op unless the region or the desired trap state changed. */
    if (lo == s_vram_map_lo && size == s_vram_map_size &&
        want_trap == s_vram_trapped)
        return;

    /* In a planar mode the guest addresses the whole 64K-plane WRAP window
     * A0000-B7FFF: B-region offsets alias plane offset 0..0x7FFF.  The BIOS
     * memory_map can shrink the decode aperture to 64K (A0000-AFFFF), but the
     * guest still writes the B-region relying on the wrap, so when trapping the
     * A0000 aperture always manage the full A0000-B7FFF wrap window (UPDATE41). */
    manage_hi = lo + size;
    if (lo <= 0xA0000ULL && manage_hi < 0xB8000ULL && (want_trap || s_vram_trapped))
        manage_hi = 0xB8000ULL;   /* arm: trap B0000-B7FFF; disarm: restore it flat */

    /* Unmap the A-region and the B-region separately: A0000-AFFFF is its own
     * WHvMapGpaRange, but B0000-B7FFF is part of the boot-RAM block, and a single
     * unmap spanning both silently drops the B part.  Split at 0xB0000. UPDATE42. */
    {
        uint64_t split = (lo < 0xB0000ULL && manage_hi > 0xB0000ULL)
                       ? 0xB0000ULL : manage_hi;
        hyperv_unmap_gpa_range(lo, split - lo);
        if (manage_hi > split)
            hyperv_unmap_gpa_range(split, manage_hi - split);
    }

    /* Trap [lo, min(hi,B8000)) when planar; B8000-BFFFF stays flat (guest text/
     * scratch RAM, B800 write-probe must round-trip).  UPDATE31/UPDATE35. */
    {
        uint64_t hi      = manage_hi;          /* full A0000-B7FFF window when planar */
        uint64_t flat_lo = lo;                 /* start of the flat (mapped) sub-range */
        if (want_trap && lo < 0xB8000ULL)
            flat_lo = (hi < 0xB8000ULL) ? hi : 0xB8000ULL;   /* [lo, flat_lo) stays trapped */
        if (hi > flat_lo)
            hyperv_map_gpa_range(flat_lo, hi - flat_lo, (void *)(Start_of_M_area + flat_lo),
                                 WHvMapGpaRangeFlagRead | WHvMapGpaRangeFlagWrite |
                                 WHvMapGpaRangeFlagTrackDirtyPages);
        if (!want_trap)
            vram_invalidate_shadow();          /* re-prime the flat-path shadow scan */
    }

    s_vram_map_lo   = lo;
    s_vram_map_size = size;
    s_vram_trapped  = want_trap;
}

/* Re-evaluate only the trap state for the region hax_set_ram last mapped --
 * does nothing until video memory has been connected at least once. */
static void vram_reeval_trap(void)
{
    if (s_vram_map_size)
        vram_apply_mapping(s_vram_map_lo, s_vram_map_size);
}

/* Enable full-CVIDC (strategy 3) for a planar-font TSR such as spdos/WPS.
 *
 * This is a MANUAL opt-in via the host environment variable NTVDM_CVIDC (set it
 * before launching the emulator, e.g. `set NTVDM_CVIDC=1`).  We deliberately do
 * NOT auto-detect from video state: spdos's planar-text render and the BIOS
 * boot-time font load are indistinguishable in the live registers (both are
 * alpha_mode=0 / votes<2 / memory_map=0 / not-256 in a text mode), so any state
 * heuristic false-positives on every machine's boot font load and flips innocent
 * apps (DOOM/BLOOD) into strategy-3 trapping -- which trips the WHP #DB-latch bug
 * and crashes them.  The flag makes strategy 3 apply ONLY to sessions the user
 * opts in, and it cannot affect any other app.  Called once per VM exit from the
 * main loop; the env var is read once and cached.  project_wpsnt_whp_derail
 * UPDATE57 (replaces the UPDATE52 auto-detect, which was unsound). */
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

    /* --- switch to full-CVIDC for the rest of this session --- */
    haxm_sync_vram();                              /* flush current flat screen -> CVIDC (no loss) */
    g_video_strategy = 3;                          /* future SAS_VIDEO connects use bDoUnmap */
    {
        extern void __stdcall OutputDebugStringA(const char *);
        OutputDebugStringA("VIDSTRAT: NTVDM_CVIDC opt-in -> full-CVIDC (strategy 3)\n");
    }
    if (s_vram_map_size) {                         /* trap the current SAS-video aperture now */
        uint64_t lo    = s_vram_map_lo;
        uint64_t hi    = s_vram_map_lo + s_vram_map_size;
        uint64_t split = (lo < 0xB0000ULL && hi > 0xB0000ULL) ? 0xB0000ULL : hi;
        hyperv_unmap_gpa_range(lo, split - lo);
        if (hi > split)
            hyperv_unmap_gpa_range(split, hi - split);
        s_vram_trapped = TRUE;
    }
}
#endif /* VIDEO_STRATEGY == 1 */


#if VIDEO_STRATEGY == 2
#error VIDEO_STRATEGY == 2 relied on HAXM-specific MMIO write combining \
       and is not supported by the Hyper-V backend.  Use VIDEO_STRATEGY 1 \
       (per-iteration buffer compare) or 3 (force MMIO on every access).
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
#error VIDEO_STRATEGY == 4 relied on the HAXM HAX_RAM_INFO_FAULTISMMIO \
       feature with EPT-permission tricks that cannot be replicated under \
       WHP.  Use VIDEO_STRATEGY 1 (recommended) or 3.
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
	PDWORD pwDst;
	DWORD i, j, size;
	BOOL bChanged;

	if (!bDoVideoSync) return;
	size = gvi_pc_high_regen - gvi_pc_low_regen + 1;
	bChanged = FALSE;
	for (i=0; i<size/0x1000; i++)
	{
		pwDst = (PDWORD)getPtrToPhysAddrByte(gvi_pc_low_regen + (i * 0x1000));
		temp_func = read_b_write_ptrs(SAS_VIDEO);

		for (j=0; j<128; j++)
			if (0xFFFFFFFF != pwDst[j])
			{
				PBYTE pbDst = (PBYTE)pwDst;
				bChanged = TRUE;
				for (j*=sizeof(DWORD); j<0x1000; j++)
					if (0xFF!=pbDst[j])
						(*temp_func) ((PBYTE)gvi_pc_low_regen + i + j, pbDst[j]);
				break;
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

	hypervvm_init();

	/*
	 * Make the entire memory space RAM.  The ROM load routines
	 * will change some of this to being ROM.
	 */

	haxm_alloc((sys_addr)Start_of_M_area, required_mem);
	sas_connect_memory(0, 0x110000 - 1, SAS_RAM);

	/* NB: extended memory (>=1MB) is intentionally NOT bulk-mapped here.  A single
	 * big WHvMapGpaRange gets torn down the first time the guest's XMS/DPMI manager
	 * reconnects a sub-page (hax_set_ram unmaps first, and WHP unmapping a
	 * sub-range drops the whole containing mapping).  Instead the manager connects
	 * pages on demand, and pages a PM extender touches without connecting are
	 * demand-mapped per-page in the MemoryAccess handler (monitor.c UPDATE62). */

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

#if VIDEO_STRATEGY == 1
/* Forward declarations: defined alongside haxm_sync_vram() above. */
static void vram_invalidate_shadow(void);
static void vram_apply_mapping(uint64_t lo, uint64_t size);
#endif

/*
 * hax_set_ram on WHP: map / unmap a guest physical range.  Permission flags
 * are derived from the SAS Type rather than from the HAXM HAX_RAM_INFO_*
 * encoding -- the result is the same set of effective access rights but
 * expressed in WHP's vocabulary.
 *
 * For SAS_INACCESSIBLE / SAS_MM_LIM we unmap so the guest faults; the fault
 * arrives as a WHvRunVpExitReasonMemoryAccess in monitor.c and is forwarded
 * to DispatchPageFault().
 */
static void hax_set_ram(
    IN sys_addr Low,
    IN sys_addr High,
    IN int Type
    )
{
    uint64_t pa_start = (uint64_t)Low;
    uint64_t size     = (uint64_t)(High - Low + 1);
    void    *host_va  = (void *)(Start_of_M_area + Low);
    WHV_MAP_GPA_RANGE_FLAGS flags = WHvMapGpaRangeFlagNone;
    BOOL bDoUnmap = FALSE;
    extern byte *video_copy;
    (void)video_copy;

	switch (Type)
	{
	case SAS_RAM:
		flags = WHvMapGpaRangeFlagRead | WHvMapGpaRangeFlagWrite | WHvMapGpaRangeFlagExecute;
		break;
	case SAS_ROM:
		flags = WHvMapGpaRangeFlagRead | WHvMapGpaRangeFlagExecute;
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
		/* RUNTIME strategy select (UPDATE52): strategy 3 (auto-detected spdos
		 * session) forces every VRAM touch through the MemoryAccess exit ->
		 * CVIDC, like CCPU (slower but renders the spdos planar-font-in-text
		 * case correctly).  Strategy 1 (default) uses the fast flat aperture. */
		if (g_video_strategy == 3) {
			bDoUnmap = TRUE;
			break;
		}
#if VIDEO_STRATEGY == 1
		/* Delegate to the single aperture-mapping owner: it maps a flat,
		 * dirty-tracked R/W buffer for text/linear modes (the host buffer is
		 * used directly as the regen aperture and synced into CVIDC in
		 * haxm_sync_vram() on each VM-exit), or UNMAPS it for planar EGA/VGA
		 * modes so latched/ALU writes fault to the in-process decoder.  The
		 * Execute flag is intentionally never set: VRAM is data only.  This
		 * also re-primes the shadow when (re)mapping flat. */
		vram_apply_mapping(pa_start, size);
		return;
#endif

	case SAS_INACCESSIBLE:
#ifdef	LIM
	case SAS_MM_LIM:
#endif
		bDoUnmap = TRUE;
		break;
	}

	{
		char szdbg[256];
		sprintf(szdbg, "hyperv_set_ram GPA %08X-%08X VA %p flags=0x%x%s",
		        (ULONG)Low, (ULONG)High, host_va, (unsigned)flags,
		        bDoUnmap ? " (unmap)" : "");
		OutputDebugString(szdbg);
	}

	/* Always unmap first to avoid an "address-range overlap" error when
	 * changing the kind of a range. */
	hyperv_unmap_gpa_range(pa_start, size);

	if (!bDoUnmap) {
		HRESULT hr = hyperv_map_gpa_range(pa_start, size, host_va, flags);
		if (FAILED(hr)) {
			haxmvm_panic("hyperv_map_gpa_range failed (pa_start=%08X, "
			               "size=%08X, va=%p, flags=0x%x, hr=0x%08X).\n\n"
			               "Common failures:\n"
			               "  0x80070057 E_INVALIDARG -- gpa or size is "
			               "not 4-KB aligned, or the range overlaps an "
			               "existing mapping.\n"
			               "  0x80370301 WHV_E_INSUFFICIENT_BUFFER\n"
			               "  0x8007000E E_OUTOFMEMORY",
			               (ULONG)pa_start, (ULONG)size, host_va,
			               (unsigned)flags, (unsigned)hr);
		}
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
	extern byte  *video_copy;
	(void)video_copy;

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
	/* 20-bit wrap: alias the first 64K of physical memory at the 1 MB mark
	 * so that real-mode segment-offset arithmetic that runs past FFFF:0010
	 * resolves to the same backing pages as 0000:0000. */
	{
		HRESULT hr = hyperv_map_gpa_range(
			(uint64_t)ONEMEGA, (uint64_t)SIXTYFOURK,
			(void *)Start_of_M_area,
			WHvMapGpaRangeFlagRead |
			WHvMapGpaRangeFlagWrite |
			WHvMapGpaRangeFlagExecute);
		if (FAILED(hr))
			haxmvm_panic("hyperv_map_gpa_range (A20 wrap on) hr=0x%08X",
			             (unsigned)hr);
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
	/* 20-bit wrap off: map the aperture at the 1 MB mark to its native
	 * backing rather than aliasing it back to the start of M-area. */
	{
		HRESULT hr = hyperv_map_gpa_range(
			(uint64_t)ONEMEGA, (uint64_t)SIXTYFOURK,
			(void *)(Start_of_M_area + ONEMEGA),
			WHvMapGpaRangeFlagRead |
			WHvMapGpaRangeFlagWrite |
			WHvMapGpaRangeFlagExecute);
		if (FAILED(hr))
			haxmvm_panic("hyperv_map_gpa_range (A20 wrap off) hr=0x%08X",
			             (unsigned)hr);
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
	 * VIDEO destination: per-byte read+write loop. Mirrors per-instruction
	 * REP MOVSB — sas_PR8 dispatches VIDEO src through the cvidc byte read
	 * handler (loads latches), sas_store dispatches VIDEO dst through the
	 * cvidc byte write handler (correct mark_byte, correct plane semantics
	 * in all chain modes). Replaces the buggy bulk VIDEO→VIDEO path below.
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
	 * in sevid000.h — second byte goes to +3, not +1). Replaces the buggy
	 * bulk VIDEO→VIDEO dispatch below.
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
	 * VIDEO destination: per-dword read+write loop via sas_storedw.
	 * Matches per-instruction REP MOVSD semantics for VIDEO.
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
	uint64_t pa = 0;
	(void)access_request;

	if (!hyperv_translate_gva((uint64_t)lin, &pa))
		return FALSE;
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
			(*temp_func) (addr, val);
#if VIDEO_STRATEGY == 1
			(*(IU8 *) getPtrToPhysAddrByte(addr)) = val;
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
	 * argument as in sas_move_bytes_forward — bulk VIDEO→VIDEO path
	 * mishandles latch semantics and plane distribution, and bulk
	 * RAM→VIDEO via sas_PWS doesn't go through cvidc byte dispatch.
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

