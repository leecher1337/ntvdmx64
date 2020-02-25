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

extern HANDLE hVM;
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

GLOBAL IU8 sas_PR8 (PHY_ADDR addr);
GLOBAL IU16 sas_PR16 (PHY_ADDR addr);
GLOBAL void sas_PW8 (PHY_ADDR addr, IU8 val);
GLOBAL void sas_PWS (PHY_ADDR Destination, IU8 * Source, PHY_ADDR Length);
GLOBAL void sas_PRS (PHY_ADDR Source, IU8 * Destination, PHY_ADDR Length);

/* local functions */
LOCAL enum SAS_MEM_TYPE readSelfMod (PHY_ADDR addr, DWORD typeSize);
LOCAL IU8  bios_read_byte   IPT1(LIN_ADDR, linAddr);
LOCAL IU16 bios_read_word   IPT1(LIN_ADDR, linAddr);
LOCAL IU32 bios_read_double IPT1(LIN_ADDR, linAddr);
LOCAL void bios_write_byte   IPT2(LIN_ADDR, linAddr, IU8, value);
LOCAL void bios_write_word   IPT2(LIN_ADDR, linAddr, IU16, value);
LOCAL void bios_write_double IPT2(LIN_ADDR, linAddr, IU32, value);
LOCAL IUM8 bios_read_accreq(void);



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

	*(ULONG*)BaseAddress = _BaseAddress;
	*ViewSize = _ViewSize;
	return Status;
}

#if VIDEO_STRATEGY == 1

/* Syncs our video RAM copy into the emulated devices.
   See comment in sas_connect_memory for details on why this
   has to be used
*/
GLOBAL VOID haxm_sync_vram(VOID)
{
	DWORD dwSize, dwCount, dwModeType, i, j;
	PDWORD pdwSrc, pdwDst;
	BOOL bChanged;

	dwSize = gvi_pc_high_regen-gvi_pc_low_regen;
	dwModeType = getModeType();
	bChanged = FALSE;
	for (i=0; i<(dwSize+1); i+=0x1000)
	{

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
					bChanged = TRUE;
					for (j*=(sizeof(DWORD)/sizeof(WORD)); j<0x1000/sizeof(WORD); j++)
					{
						if (pwSrc[j]!=pwDst[j])
						{
							/*
							char szDbg[256];
							sprintf(szDbg, "Zeile %d,Spalte %d: (%04X) => '%c' (%04X)\n", j/80, j%80, 
								pwSrc[j], (char)pwDst[j], pwDst[j]);
							OutputDebugString(szDbg);*/
							(*temp_func) ((PBYTE)gvi_pc_low_regen + i + j*sizeof(WORD), pwDst[j]);
						}
					}
					break;
				}
		}
		else /* Graphics mode, most likely planar, better mark what we consumed */
		{
			temp_func = read_b_write_ptrs(SAS_VIDEO);
			for (j=0; j<0x1000/sizeof(DWORD); j++)
				if (0xFFFFFFFF != pdwDst[j])
				{
					PBYTE pbSrc=(PBYTE)pdwSrc;
					PBYTE pbDst = (PBYTE)pdwDst;
					bChanged = TRUE;
					for (j*=sizeof(DWORD); j<0x1000; j++)
					{
						if (0xFF!=pbDst[j])
						{
							(*temp_func) ((PBYTE)gvi_pc_low_regen + i + j, pbDst[j]);
							// Of course this strategy is plain stupid, if someone writes a 0xFF
							// to video memory, we won't get it as changed.
							//pbSrc[j] = pbDst[j]= 0xFF;
						}
					}
					break;
				}
		}
		if (bChanged)
		{
			if (dwModeType == TEXT) memcpy(pdwSrc, pdwDst, 0x1000);
			else memset(pdwDst, 0xFF, 0x1000);

			host_graphics_tick();
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
	struct hax_alloc_ram_info alloc_ram = { 0 };

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

	//sas_connect_memory(0, alloc_ram.size - 1, SAS_RAM);
	haxm_alloc(Start_of_M_area, 0x110000);
	sas_connect_memory(0, 0x110000, SAS_RAM);

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
		// Map to normal RAM block, we use it as regen buffer
		ram.flags = 0;
		break;
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

	if (getPG())
	{
		if (!xtrn2phy(Source, bios_read_accreq(), &Source))
			return;
	}

	Source &= SasWrapMask;
	
	switch (readSelfMod(Source, Length)) {
		case SAS_MM_LIM:
		case SAS_RAM:
		case SAS_ROM:
		default:
			sas_stores(Destination, getPtrToPhysAddrByte(Source), Length);
			break;

#ifdef	EGG
		case SAS_VIDEO:
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

#ifdef C_VID
				case SAS_VIDEO:	/* Video -> Video */
					temp_func = read_b_move_ptrs(SAS_VIDEO);
					/* There seems to be a bug in move_byte_fwd_ev_glue in CVIDC.
					 * It assumed that source is a 2cell video area and dest is 4-cell
					 * If you check ega_copy_move, there 4byte->4byte is assumed.
					 * So we compensate for it: */
					Source = (Source - gvi_pc_low_regen)*2 + gvi_pc_low_regen;
					(*temp_func)(Destination, Source, Length, 0);
#if VIDEO_STRATEGY == 1
					RtlCopyMemory(getPtrToPhysAddrByte(Destination), getPtrToPhysAddrByte(Source), Length);
					RtlCopyMemory(haxm_videocmp_copy + Destination - gvi_pc_low_regen, getPtrToPhysAddrByte(Source), Length);
#endif
					break;
#endif /* C_VID */
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

	if (getPG())
	{
		if (!xtrn2phy(Source, bios_read_accreq(), &Source))
			return;
	}

	Source &= SasWrapMask;
	Length <<= 1;
	
	switch (readSelfMod(Source, Length)) {
		case SAS_MM_LIM:
		case SAS_RAM:
		case SAS_ROM:
		default:
			sas_stores(Destination, getPtrToPhysAddrByte(Source), Length);
			break;

#ifdef	EGG
		case SAS_VIDEO:
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

#ifdef C_VID
				case SAS_VIDEO:	/* Video -> Video */
					temp_func = read_w_move_ptrs(SAS_VIDEO);
					(*temp_func)(Destination, Source, Length/2, 0);
#if VIDEO_STRATEGY == 1
					RtlCopyMemory(getPtrToPhysAddrByte(Destination), getPtrToPhysAddrByte(Source), Length);
					RtlCopyMemory(haxm_videocmp_copy + Destination - gvi_pc_low_regen, getPtrToPhysAddrByte(Source), Length);

#endif
					break;
#endif /* C_VID */
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

	if (getPG())
	{
		if (!xtrn2phy(Source, bios_read_accreq(), &Source))
			return;
	}

	Source &= SasWrapMask;
	Length <<= 2;
	
	switch (readSelfMod(Source, Length)) {
		case SAS_MM_LIM:
		case SAS_RAM:
		case SAS_ROM:
		default:
			sas_stores(Destination, getPtrToPhysAddrByte(Source), Length);
			break;

#ifdef	EGG
		case SAS_VIDEO:
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

#ifdef C_VID
				case SAS_VIDEO:	/* Video -> Video */
					move_dword_fwd_ev_glue(Destination, Source, Length/4, 0);
#if VIDEO_STRATEGY == 1
					RtlCopyMemory(getPtrToPhysAddrByte(Destination), getPtrToPhysAddrByte(Source), Length);
					RtlCopyMemory(haxm_videocmp_copy + Destination - gvi_pc_low_regen, getPtrToPhysAddrByte(Source), Length);
#endif
					break;
#endif /* C_VID */
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
EXPORT
double_word
effective_addr(
    IN word Segment,
    IN word Offset
    )

/*++

Routine Description:

    This routine maps effective_addr to Sim32GetVdmPointer

Arguments:

    Segment -- segment of address
    Offset -- offset of address

Return Value:

    Actual Intel address corresponding to the address supplied
--*/
{
	if ((!getPE()) || getVM())
		return ((LIN_ADDR)Segment << 4) + Offset;

    return (ULONG)Sim32GetVDMPointer(((((ULONG)Segment) << 16) | Offset), 1,
        (UCHAR) (getMSW() & MSW_PE ? TRUE : FALSE));
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

    if (!DeviceIoControl(hVM, HAX_VCPU_IOCTL_VA2GPA, &va, sizeof(va), &pa, sizeof(pa), &bytes, NULL))
    {
		return FALSE;
    }
	*phy = pa;
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

	switch (readSelfMod(Source, Length)) {
		case SAS_MM_LIM:
		case SAS_RAM:
		case SAS_ROM:
		default:
			sas_PWS(Destination, getPtrToPhysAddrByte(Source), Length);
			break;

#ifdef	EGG
		case SAS_VIDEO:
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

#ifdef C_VID
				case SAS_VIDEO:	/* Video -> Video */
					temp_func = read_b_move_ptrs(SAS_VIDEO);
					(*temp_func) (Destination, Source, Length, 0);
#if VIDEO_STRATEGY == 1
					memmove(getPtrToPhysAddrByte(Destination), getPtrToPhysAddrByte(Source), Length);
					memmove(haxm_videocmp_copy + Destination - gvi_pc_low_regen, haxm_videocmp_copy + Source - gvi_pc_low_regen, Length);
#endif
					break;
#endif /* C_VID */
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
			return;

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
			return;

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
			return;

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
GLOBAL void sas_overwrite_memory IFN2(sys_addr, addr, int, type)
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

