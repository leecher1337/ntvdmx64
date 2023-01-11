#include "insignia.h"
#include "host_def.h"
/*[
======================================================================

				 SoftPC Revision 5.0

 Title:
		vesa_video.c

 Description:
		Code for VESA support
		Reverse engineered from SoftWindows95 V5.0
		SoftWindows 5.0 Video engine differs a lot from previous versions,
		so this module may become handy in case the SoftPC 5 video engine 
		gets reverse engineered and implemented.
		As long as this is not the case (most probably, it never will), 
		this module just sits there and does nothing.
		Currently, it possibly doesn't even compile, ignore it for now.

 Author:
		leecher1337

 Date:
		06 January 2023

 SccsID	"@(#)vesa_video.c	1.19 06/13/23 Copyright Insignia Solutions Ltd."

======================================================================
]*/

#ifdef V7VGA

#include "xt.h"
#include CpuH
#include "sas.h"
#include "config.h"
#include "gvi.h"
#include "gmi.h"
#include "ios.h"
#include "gfx_upd.h"
#include "debug.h"
#include "egacpu.h"
#include "egaports.h"
#include "egagraph.h"
#include "egaread.h"
#include "egavideo.h"
#include "vgaports.h"
#include "video.h"

#include "vesa.h"

#define MAX_VESA_MODES		20	// Maximum list of VESA modes supported, 20 is Max. in BIOS ROM for VBE1

#pragma pack(1)

struct vesa_mode_data
{
  word mode;
  word unsigned2;
  struct vbe_mode_info info;
  word unk36;
  boolean available;
  word SizeOfScanline;
  word unk3E;
};

#pragma pack()

struct vesa_config
{
  struct vesa_mode_data *mode_data;
  word current_mode;
  boolean is_vbe2;
  word supported_modes[MAX_VESA_MODES];
  word mode_count;
  void (*paint)(int, byte*, int);
};

static struct vesa_ctrl_config vesa_ctrl_config = {
	"VESA", 0x200, "Insignia Solutions VESA BIOS(VBE2)", 0, 0x101, "Insignia Solutions", "Insignia SoftWindows", "1.1"
};

struct vesa_config vesa_config;
struct vesa_mode_data vesa_mode_tab[] = {
	//
	// Modes originally supported by Insignia SoftWindows95 5.0
	// NB: If list gets extended over 19 modes, update MAX_VESA_MODES #define!
	//
	{0x100, 0x66, 
		{0x9F, 7, 0, 64, 64, 0xA000, 0, 0, 0,  640,  640, 400, 8, 16, 1,  8, 1, 4, 0, 1, 1, 0,  0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0}, 
	 0, TRUE, 640, 0},

	{0x101, 0x67, 
		{0x9F, 7, 0, 64, 64, 0xA000, 0, 0, 0,  640,  640, 480, 8, 16, 1,  8, 1, 4, 0, 0, 1, 0,  0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0}, 
	 0, TRUE, 640, 0},

	{0x103, 0x69, 
		{0x9F, 7, 0, 64, 64, 0xA000, 0, 0, 0,  800,  800, 600, 8,  8, 1,  8, 1, 4, 0, 0, 1, 0,  0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0}, 
	 0, TRUE, 800, 0},

	{0x105, 0, 
		{0xDF, 0, 0, 64, 64, 0xA000, 0, 0, 0,  640,  640, 768, 8, 16, 1,  8, 1, 4, 0, 0, 1, 0,  0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0}, 
	 0, TRUE, 640, 0},

	{0x107, 0, 
		{0xDF, 0, 0, 64, 64, 0xA000, 0, 0, 0, 1280, 1280, 640, 8, 16, 1,  8, 1, 4, 0, 0, 1, 0,  0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0}, 
	 0, TRUE, 1280, 0},

	{0x380, 0, 
		{0xFB, 0, 0,  0,  0, 0x0000, 0, 0, 0,  320,  320, 200, 8, 16, 1,  8, 1, 4, 0, 1, 1, 0,  0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0}, 
	 0, TRUE, 320, 0},

	{0x480, 0, 
		{0xFB, 7, 0, 64, 64, 0xA000, 0, 0, 0,  320,  320, 200, 8, 16, 1,  8, 1, 4, 0, 1, 1, 0,  0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0}, 
	 0, TRUE, 320, 0},

	{0x180, 0, 
		{0xFB, 7, 0, 64, 64, 0xA000, 0, 0, 0,  320,  320, 200, 8, 16, 1,  8, 1, 4, 0, 1, 1, 0,  0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0}, 
	 0, TRUE, 320, 0},

	{0x10D, 0, 
		{0xDF, 0, 0,  0,  0, 0x0000, 0, 0, 0,  640,  320, 200, 8,  8, 1, 15, 1, 6, 0, 0, 1, 5, 10, 5, 5, 5, 0, 1, 15, 2, 0, 0, 0}, 
	 0, TRUE, 640, 0},

	{0x10E, 0, 
		{0xDF, 0, 0,  0,  0, 0x0000, 0, 0, 0,  640,  320, 200, 8,  8, 1, 16, 1, 6, 0, 0, 1, 5, 11, 6, 5, 5, 0, 0,  0, 0, 0, 0, 0}, 
	 0, TRUE, 640, 0},

	{0x10F, 0, 
		{0xDF, 0, 0,  0,  0, 0x0000, 0, 0, 0, 1280,  320, 200, 8,  8, 1, 32, 1, 6, 0, 0, 1, 8, 16, 8, 8, 8, 0, 8, 24, 0, 0, 0, 0}, 
	 0, TRUE, 1280, 0},

	{0x110, 0, 
		{0x9B, 7, 0, 64, 64, 0xA000, 0, 0, 0, 1280,  640, 480, 8, 16, 1, 15, 1, 6, 0, 0, 1, 5, 10, 5, 5, 5, 0, 1, 15, 2, 0, 0, 0}, 
	 0, TRUE, 1280, 0},

	{0x111, 0, 
		{0xDF, 7, 0, 64, 64, 0xA000, 0, 0, 0, 1280,  640, 480, 8, 16, 1, 16, 1, 6, 0, 0, 1, 5, 11, 6, 5, 5, 0, 0,  0, 0, 0, 0, 0},
	 0, TRUE, 1280, 0},

	{0x112, 0, 
		{0xDF, 0, 0,  0,  0, 0x0000, 0, 0, 0, 2560,  640, 480, 8, 16, 1, 32, 1, 6, 0, 0, 1, 8, 16, 8, 8, 8, 0, 8, 24, 0, 0, 0, 0}, 
	 0, TRUE, 2560, 0},

	{0x113, 0, 
		{0xFB, 0, 0,  0,  0, 0x0000, 0, 0, 0, 1600,  800, 600, 8, 16, 1, 15, 1, 6, 0, 0, 1, 5, 10, 5, 5, 5, 0, 1, 15, 2, 0, 0, 0}, 
	 0, TRUE, 1600, 0},

	{0x114, 0, 
		{0xDF, 0, 0,  0,  0, 0x0000, 0, 0, 0, 1600,  800, 600, 8, 16, 1, 16, 1, 6, 0, 0, 1, 5, 11, 6, 5, 5, 0, 0,  0, 0, 0, 0, 0}, 
	 0, TRUE, 1600, 0},

	{0x115, 0, 
		{0xDF, 0, 0,  0,  0, 0x0000, 0, 0, 0, 3200,  800, 600, 8, 16, 1, 32, 1, 6, 0, 0, 1, 8, 16, 8, 8, 8, 0, 8, 24, 0, 0, 0, 0}, 
	 0, TRUE, 3200, 0},
	 //
	 // Maybe you want to add modes? Here is an example:
	 //
	 //                                       Bytes  PixW PixH          BPP     M     *
	{0x116, 0, 
		{0xFB, 0, 0,  0,  0, 0x0000, 0, 0, 0, 2048, 1024, 768, 8, 16, 1, 15, 1, 6, 0, 0, 1, 5, 10, 5, 5, 5, 0, 1, 15, 2, 0, 0, 0}, 
	 0, TRUE, 2048, 0},

};

boolean vesa_painting_enabled = FALSE;

// VESA controller information
static char *VbeSignature = "VESA";
static word  VbeVersion   = 0x200;
static char *

// Global Settings
// Original is placed in struct with name display_config
// struct {
unsigned int video_memory_address;
unsigned int video_memory_ptr;
unsigned int video_memory_total;
unsigned int video_memory_maxsize = 0xFFFFFFFF;
unsigned int video_memory_256k_blocks = 0;
unsigned int video_memory_window_offset;
unsigned int video_memory_cur_page;
boolean      video_memory_valid = FALSE;
unsigned int video_memory_upper;
// } display_config;

/* TODO: reset() -> Set video_memory_total; set display_config
 * v7vga.rom von SoftWindows nehmen, beinhaltet VESA
 * XREF vesa_video_func
 * Update v7vga_get_memory_configuration
 */

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef offsetof
#define offsetof(st, m) \ ((size_t)&(((st *)0)->m))
#endif

#ifdef NTVDM
// This is a stub
GLOBAL boolean
AntiShearStillActive(void)
{
	return FALSE;
}
#endif

GLOBAL boolean
vbe2_rom()
{
	static int vbe2_rom_present = -1;
	char oem_name[36];

	if ( vbe2_rom_present == -1 )
	{
		sas_loads(effective_addr(getCS(), VESA_BIOS_OEM_NAME_OFFS), oem_name, 35);
		oem_name[35] = 0;
		vbe2_rom_present = strstr(oem_name, "VBE2") != 0;
	}
	return vbe2_rom_present;
}

// TODO: Put this directly into rom_init, as it originally is!
GLOBAL VOID
vesa_preinit()
{
	unsigned int old_video_memory_256k_blocks = video_memory_256k_blocks;

	video_memory_upper = video_memory_maxsize & 0x7FFFF;
	video_memory_total = video_memory_maxsize - video_memory_upper;
	video_memory_256k_blocks = video_memory_maxsize >= 0x100000 ? 1024 : 512;
	if (old_video_memory_256k_blocks != video_memory_256k_blocks)
		video_memory_valid = FALSE;
}

GLOBAL VOID
vesa_init()
{
	int i, j;
	unsigned int buffer_size;

	vesa_config.modes = 0;
	vesa_config.current_mode = 0;

	host_validate_vesa_modes(vesa_mode_tab, sizeof(vesa_mode_tab)/sizeof(vesa_mode_tab[0]));
	for (i=0, j=0; i<sizeof(vesa_mode_tab)/sizeof(vesa_mode_tab[0]); i++)
	{
		if (vesa_mode_tab[i].available)
		{
			buffer_size = vesa_mode_tab[i].info.PixelHeight * vesa_mode_tab[i].info.BytesPerScanLine;
			assert0(buffer_size>0, "VESA buffer is empty");
			if ( video_memory_total / buffer_size )
			{
				vesa_config.supported_modes[j++] = vesa_mode_tab[i].mode;
				vesa_mode_tab[i].info.NumberOfImagePages = video_memory_total / buffer_size - 1;
				vesa_mode_tab[i].info.PhysBasePtr = video_memory_address;
			}
			else
				vesa_mode_tab[i].available = FALSE;
		}
	}
	vesa_config.supported_modes[j] = -1;
	vesa_config.mode_count = j;
	vesa_painting_enabled = FALSE;
}

GLOBAL VOID
vesa_controller_info()
{
	struct vesa_ctrl_info *cinfo;
	int i, bios_supp_modes, offset, len;
	word mode_count, OEMNameOffset, SupportedModesOffset;
	char signature[4];

	cinfo = (struct vesa_ctrl_info *)effective_addr(getES(), getDI());
	sas_loads(cinfo, signature, sizeof(signature));
	
	sas_stores((IU32)&cinfo->VbeSignature, vesa_ctrl_config.VbeSignature, 4);
	sas_storew((IU32)&cinfo->VbeVersion, vesa_ctrl_config.VbeVersion);
	sas_storedw((IU32)&cinfo->Capabilities, vesa_ctrl_config.Capabilities);
	sas_storew((IU32)&cinfo->TotalMemory, (word)((video_memory_total>>16)&0xFFFF));

	if (strncmp(signature, "VBE2", sizeof(signature)))
	{
		vesa_config.is_vbe2 = FALSE;

		bios_supp_modes = effective_addr(getCS(), VESA_BIOS_SUPP_MODES_OFFS);
		mode_count = MIN(vesa_config.mode_count,19); // For VESA1, 19 Modes maximum!
		if (mode_count > 0)
		{
			for (i=0; i<mode_count; i++, bios_supp_modes+=2)
			{
				poke_rom_byte(bios_supp_modes, *(byte*)vesa_config.supported_modes[i], 0);
				poke_rom_byte(bios_supp_modes+1, *((byte*)vesa_config.supported_modes[i]+1), 0);
			}
		}
		poke_rom_byte(bios_supp_modes, 0xFF, 0);
		poke_rom_byte(bios_supp_modes + 1, 0xFF, 0);
		sas_storew((IU32)&cinfo->OEMNameOffset, VESA_BIOS_OEM_NAME_OFFS);
		sas_storew((IU32)&cinfo->OEMNameSegment, getCS());
		sas_storew((IU32)&cinfo->SupportedModesOffset, VESA_BIOS_SUPP_MODES_OFFS);
		sas_storew((IU32)&cinfo->SupportedModesSegment, getCS());

	} else {
		vesa_config.is_vbe2 = TRUE;

		OEMNameOffset = getDI() + offsetof(struct vesa_ctrl_info, Reserved2);
		sas_storew((IU32)&cinfo->OEMNameOffset, OEMNameOffset);
		sas_storew((IU32)&cinfo->OEMNameSegment, getES());
		len = strlen(vesa_ctrl_config.OEMName) + 1;
		sas_stores((IU32)cinfo->Reserved2, OEMName, len);
		offset = len;
		
		SupportedModesOffset = getDI() + offsetof(struct vesa_ctrl_info, Reserved1);
		sas_storew((IU32)&cinfo->SupportedModesOffset, SupportedModesOffset);
		sas_storew((IU32)&cinfo->SupportedModesSegment, getES());
		if (vesa_config.mode_count > 0)
		{
			bios_supp_modes = &cinfo->Reserved1;
			for (i=0; i<vesa_config.mode_count; i++, bios_supp_modes+=2)
				sas_storew(bios_supp_modes, vesa_config.supported_modes[i]);
		}
		sas_storew(bios_supp_modes, 0xFFFF);

		sas_storew((IU32)&cinfo->OemSoftwareRev, vesa_ctrl_config.OemSoftwareRev);
		
		sas_storew((IU32)&cinfo->OemVendorNameOffset, OEMNameOffset + offset);
		sas_storew(IU32)(&cinfo->OemVendorNameSegment, getES());
		len = strlen(vesa_ctrl_config.OemVendorName) + 1;
		sas_stores((IU32)&cinfo->Reserved2[offset], vesa_ctrl_config.OemVendorName, len);
		offset += len;

		sas_storew((IU32)&cinfo->OemProductNameOffset, OEMNameOffset + offset);
		sas_storew((IU32)&cinfo->OemProductNameSegment, getES());
		len = strlen(vesa_ctrl_config.OemProductName) + 1;
		sas_stores((IU32)&cinfo->Reserved2[offset], vesa_ctrl_config.OemProductName, len);
		offset += len;

		sas_storew((IU32)&cinfo->OemProductRevOffset, OEMNameOffset + offset);
		sas_storew((IU32)&cinfo->OemProductRevSegment, getES());
		len = strlen(vesa_ctrl_config.OemProductRev) + 1;
		sas_stores((IU32)&cinfo->Reserved2[offset], vesa_ctrl_config.OemProductRev, len);
	}
	setAX(0x004F);
}

GLOBAL VOID
vesa_mode_info()
{
	struct vesa_mode_info *modeinfo;
	struct vesa_mode_data *modedata; 

	modeinfo = (struct vesa_mode_info *)effective_addr(getES(), getDI());
	if ( get_mode_info(getCX() & 0x3FFF, &modedata) )
	{
		sas_storew((IU32)&modeinfo->info.ModeAttributes, modedata->info.ModeAttributes);
		sas_store((IU32)&modeinfo->info.WindowAAttributes, modedata->info.WindowAAttributes);
		sas_store((IU32)&modeinfo->info.WindowBAttributes, modedata->info.WindowBAttributes);
		sas_storew((IU32)&modeinfo->info.WindowGranularity, modedata->info.WindowGranularity);
		sas_storew((IU32)&modeinfo->info.WindowSize, modedata->info.WindowSize);
		sas_storew((IU32)&modeinfo->info.StartSegmentWindowA, modedata->info.StartSegmentWindowA);
		sas_storew((IU32)&modeinfo->info.StartSegmentWindowB, modedata->info.StartSegmentWindowB);
		sas_storew((IU32)&modeinfo->info.BankSwitchFunctionOffset, VESA_BIOS_BANK_SW_FUNC_OFFS);
		sas_storew((IU32)&modeinfo->info.BankSwitchFunctionSegment, getCS());
		sas_storew((IU32)&modeinfo->info.BytesPerScanLine, modedata->info.BytesPerScanLine);
		sas_storew((IU32)&modeinfo->info.PixelWidth, modedata->info.PixelWidth);
		sas_storew((IU32)&modeinfo->info.PixelHeight, modedata->info.PixelHeight);
		sas_store((IU32)&modeinfo->info.CharacterCellPixelWidth, modedata->info.CharacterCellPixelWidth);
		sas_store((IU32)&modeinfo->info.CharacterCellPixelHeight, modedata->info.CharacterCellPixelHeight);
		sas_store((IU32)&modeinfo->info.NumberOfMemoryPlanes, modedata->info.NumberOfMemoryPlanes);
		sas_store((IU32)&modeinfo->info.BitsPerPixel, modedata->info.BitsPerPixel);
		sas_store((IU32)&modeinfo->info.NumberOfBanks, modedata->info.NumberOfBanks);
		sas_store((IU32)&modeinfo->info.MemoryModelType, modedata->info.MemoryModelType);
		sas_store((IU32)&modeinfo->info.SizeOfBank, modedata->info.SizeOfBank);
		sas_store((IU32)&modeinfo->info.NumberOfImagePages, modedata->info.NumberOfImagePages);
		sas_store((IU32)&modeinfo->info.Reserved1, modedata->info.Reserved1);
		sas_store((IU32)&modeinfo->info.RedMaskSize, modedata->info.RedMaskSize);
		sas_store((IU32)&modeinfo->info.RedFieldPosition, modedata->info.RedFieldPosition);
		sas_store((IU32)&modeinfo->info.GreenMaskSize, modedata->info.GreenMaskSize);
		sas_store((IU32)&modeinfo->info.GreenFieldPosition, modedata->info.GreenFieldPosition);
		sas_store((IU32)&modeinfo->info.BlueMaskSize, modedata->info.BlueMaskSize);
		sas_store((IU32)&modeinfo->info.BlueFieldPosition, modedata->info.BlueFieldPosition);
		sas_store((IU32)&modeinfo->info.ReservedMaskSize, modedata->info.ReservedMaskSize);
		sas_store((IU32)&modeinfo->info.ReservedFieldPosition, modedata->info.ReservedFieldPosition);
		sas_store((IU32)&modeinfo->info.DirectColourModeInfo, modedata->info.DirectColourModeInfo);
		sas_storedw((IU32)&modeinfo->info.PhysBasePtr, modedata->info.PhysBasePtr);
		sas_storedw((IU32)&modeinfo->info.OffScreenMemOffset, modedata->info.OffScreenMemOffset);
		sas_storew((IU32)&modeinfo->info.OffScreenMemSize, modedata->info.OffScreenMemSize);
		sas_move_doubles_backward(modeinfo->Reserved2, 0, sizeof(modeinfo->Reserved2));
		setAX(0x004F);
	}
	else
	{
		setAX(0x14F);
	}
}

LOCAL boolean
get_mode_info(word mode, struct vesa_mode_data **modedata)
{
	int i;

	for (i=0; i<sizeof(vesa_mode_tab)/sizeof(vesa_mode_tab[0]); i++)
	{
		if (vesa_mode_tab[i].available && vesa_mode_tab[i].mode == mode)
		{
			*modedata = vesa_mode_tab[i];
			return TRUE;
		}
	}
	return FALSE;
}

/*
 * Update the window to look like the regen buffer says it should
 */
LOCAL VOID vesa_update(void)
{
	if (!vesa_painting_enabled || AntiShearStillActive()) return;

#ifdef NTVDM
	// Here is my own code...
	if (getVideodirty_total() == 0 || get_display_disabled() )
		return;

	host_start_update();


	host_end_update();
#endif
}

// ------------------ todo others -------------



GLOBAL VOID poke_rom_byte IFN2(sys_addr, addr, half_word, val, IBOOL, lin_addr)
{
#if !defined(NTVDM) || (defined(NTVDM) && !defined(X86GFX) )
	UTINY	old_val;

	/*
	 * 4.0 style CPUs don't export this variable, and if sas hasn't been
	 * inited, then the sas_connect will drop out to yoda.
	 */

#ifdef CPU_40_STYLE

	IU8	*hostPtr;

	/* TMM 14/2/95
	 * -----------
	 * What we are doing here is replacing the sas_connect() method of writing to ROM
	 * with the new approach of poking the values directly in there. See display_string()
	 * below for a more detiled discussion of the why's and wherefores.
	 */

#ifdef macintosh

	/* The Mac config system wants to call this routine before the
	 * CPU exists, so we'd better invent a Mac-specific IBOOL to
	 * make the symptom non-fatal - finding and fixing the cause
	 * is too hard.
	 */
	{
		extern IBOOL SafeToCallSas;

		if (!SafeToCallSas)
			return;	
	}

#endif /* macintosh */
 
	/* The page might not be present (Arrggghhhh!!!!!)
	** so we can't do anything sensible and must give
	** up. We print an error though.
	*/
	if (lin_addr)
		hostPtr = getPtrToLinAddrByte(addr);
	else
		hostPtr = getPtrToPhysAddrByte (addr);
	if (hostPtr == 0)
	{
		host_error(EG_OWNUP, ERR_QUIT, NULL);
		return;
	}

	old_val = *hostPtr;

	/* Optimisation - don't upset the world if the value is unchanged.
	 */
	if (old_val == val)
		return;

	*hostPtr = val;

#else	/* CPU_40_STYLE */

	/*
	 * 4.0 style CPUs don't export this variable, and if sas hasn't been
	 * inited, then the sas_connect will drop out to yoda.
	 */

	if (Length_of_M_area == 0) 
		return;
 
	old_val = sas_hw_at( addr );

	/* Optimisation - don't upset the world if the value is unchanged.
	 */
	if (old_val == val)
		return;

	sas_connect_memory (addr, addr, SAS_RAM);
	sas_store (addr,val);
	sas_connect_memory (addr, addr, SAS_ROM);
#endif	/* CPU_40_STYLE */

#endif	/* !NTVDM | (NTVDM & !X86GFX) */
}


#endif /* V7VGA */ 
