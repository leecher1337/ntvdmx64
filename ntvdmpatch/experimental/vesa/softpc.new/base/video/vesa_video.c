#include "insignia.h"
#include "host_def.h"
/*[
======================================================================

				 SoftPC Revision 4.0

 Title:
		vesa_video.c

 Description:
		Code for VESA support
		Reverse engineered from SoftWindows95 V4.0
		This module implements the V7 VGA VESA BIOS functions

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

#ifndef offsetof
#define offsetof(st, m) ((size_t)&(((st *)0)->m))
#endif

IMPORT void ega_set_mode(void);

FORWARD	void	vesa_controller_info();
FORWARD	void	vesa_mode_info();
FORWARD	void	vesa_set_mode();
FORWARD	void	vesa_get_mode();
FORWARD	void	vesa_save_restore();
FORWARD	void	vesa_window_control();
FORWARD	void	vesa_logical_scan_len();
FORWARD	void	vesa_display_start();
FORWARD	void	vesa_dac_palette();
FORWARD	void	vesa_palette_data();
FORWARD	void	vesa_return_prot_mode_interface();

LOCAL boolean	v7_extension_enabled();
LOCAL VOID		v7_enable_extension();
LOCAL boolean	v7_1mb_bank_wrap_get();
LOCAL VOID		v7_1mb_bank_wrap_set();
LOCAL boolean	v7_bank_selection_get();
LOCAL VOID		v7_bank_selection_set();
LOCAL boolean	vga_plane_enabled();
LOCAL VOID		v7_window_control();
LOCAL word		v7_get_bytes_per_scanline();

GLOBAL	void		(*vesa_video_func[]) () =
{
	vesa_controller_info,
	vesa_mode_info,
	vesa_set_mode,
	vesa_get_mode,
	vesa_save_restore,
	vesa_window_control,
	vesa_logical_scan_len,
	vesa_display_start,
	vesa_dac_palette,
	vesa_palette_data,
	vesa_return_prot_mode_interface
}; 


struct vbe_mode_info vesa_std_mode_tab[] =
{
  {0x0E, 6, 0, 32, 32, 0xB800, 0, 0, 0,  80,   40,  25, 9, 16, 1, 4, 1, 0, 0, 15, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* 00 */
  {0x0E, 6, 0, 32, 32, 0xB800, 0, 0, 0,  80,   40,  25, 9, 16, 1, 4, 1, 0, 0, 15, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* 01 */
  {0x0E, 6, 0, 32, 32, 0xB800, 0, 0, 0, 160,   80,  25, 9, 16, 1, 4, 1, 0, 0,  7, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* 02 */
  {0x0E, 6, 0, 32, 32, 0xB800, 0, 0, 0, 160,   80,  25, 9, 16, 1, 4, 1, 0, 0,  7, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* 03 */
  {0x1E, 6, 0, 32, 32, 0xB800, 0, 0, 0,  80,  320, 200, 8,  8, 1, 2, 2, 1, 8,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* 04 */
  {0x1E, 6, 0, 32, 32, 0xB800, 0, 0, 0,  80,  320, 200, 8,  8, 1, 2, 2, 1, 8,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* 05 */
  {0x1E, 6, 0, 32, 32, 0xB800, 0, 0, 0,  80,  640, 200, 8,  8, 1, 1, 2, 1, 8,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* 06 */
  {0x06, 6, 0, 32, 32, 0xB000, 0, 0, 0, 160,   80,  25, 9, 16, 1, 2, 1, 0, 0,  7, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* 07 */
  {0x00, 0, 0,  0,  0, 0x0000, 0, 0, 0,   0,    0,   0, 0,  0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* 08 */
  {0x00, 0, 0,  0,  0, 0x0000, 0, 0, 0,   0,    0,   0, 0,  0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* 09 */
  {0x00, 0, 0,  0,  0, 0x0000, 0, 0, 0,   0,    0,   0, 0,  0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* 0A */
  {0x00, 0, 0,  0,  0, 0x0000, 0, 0, 0,   0,    0,   0, 0,  0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* 0B */
  {0x00, 0, 0,  0,  0, 0x0000, 0, 0, 0,   0,    0,   0, 0,  0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* 0C */
  {0x1E, 6, 0, 64, 64, 0xA000, 0, 0, 0,  40,  320, 200, 8,  8, 4, 4, 1, 3, 0,  7, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* 0D */
  {0x1E, 6, 0, 64, 64, 0xA000, 0, 0, 0,  80,  640, 200, 8,  8, 4, 4, 1, 3, 0,  3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* 0E */
  {0x16, 6, 0, 64, 64, 0xA000, 0, 0, 0,  80,  640, 350, 8, 14, 4, 4, 1, 3, 0,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* 0F */
  {0x1E, 6, 0, 64, 64, 0xA000, 0, 0, 0,  80,  640, 350, 8, 14, 4, 4, 1, 3, 0,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* 10 */
  {0x1E, 6, 0, 64, 64, 0xA000, 0, 0, 0,  80,  640, 480, 8, 16, 4, 4, 1, 3, 0,  0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* 11 */
  {0x1E, 6, 0, 64, 64, 0xA000, 0, 0, 0,  80,  640, 480, 8, 16, 4, 4, 1, 3, 0,  0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* 12 */
  {0x1E, 6, 0, 64, 64, 0xA000, 0, 0, 0, 320,  320, 200, 8,  8, 1, 8, 1, 4, 0,  0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}  /* 13 */
};


struct vbe_mode_info vesa_ext_mode_tab[] =
{
  {0x1F, 7, 0, 64, 64, 0xA000, 0, 0, 0, 640,  640, 400, 8, 16, 1, 8, 1, 4, 0,  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* 100 */
  {0x1F, 7, 0, 64, 64, 0xA000, 0, 0, 0, 640,  640, 480, 8, 16, 1, 8, 1, 4, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* 101 */
  {0x1F, 7, 0, 64, 64, 0xA000, 0, 0, 0, 100,  800, 600, 8,  8, 4, 4, 1, 3, 0,  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* 102 */
  {0x1F, 7, 0, 64, 64, 0xA000, 0, 0, 0, 800,  800, 600, 8,  8, 1, 8, 1, 4, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* 103 */
  {0x1F, 7, 0, 64, 64, 0xA000, 0, 0, 0, 128, 1024, 768, 8,  8, 4, 4, 1, 3, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}  /* 104 */
};

LOCAL byte vesa_v7_mode_map[]     = {0x66 , 0x67 , 0x62 , 0x69 , 0x65 , 0x00  };
LOCAL word vesa_supported_modes[] = {0x100, 0x101, 0x102, 0x103, 0x104, 0xFFFF};

static struct vesa_ctrl_config vesa_ctrl_config = {
	"VESA", 0x200, "Insignia Solutions VESA BIOS(VBE2)", 0, 0x100, "Insignia Solutions", "Insignia SoftWindows", "1.0"
};


LOCAL word    video_memory_total = 8;
LOCAL boolean vesa_is_vbe2       = FALSE;
LOCAL byte    vesa_v7mode        = 0;
LOCAL word    vesa_vesamode      = 0;

//
//
//

GLOBAL boolean
vbe2_rom()
{
	static int vbe2_rom_present = -1;
	char oem_name[36];

	if ( vbe2_rom_present == -1 )
	{
		sas_loads(effective_addr(EGA_SEG, VESA_BIOS_OEM_NAME_OFFS), oem_name, 35);
		oem_name[35] = 0;
		vbe2_rom_present = strstr(oem_name, "VBE2") != 0;
	}
	return vbe2_rom_present;
}


//
// INT10h functions
//

/*(
----------------------------------------------------------------------

Function:	
		vesa_controller_info()

Purpose:
		Perform the int 10 extended BIOS function 4F - Subfunction 0

Input:
		ES:DI = buffer for SuperVGA information

Output:
		AL = 4Fh if function supported
		AH = status
			00h successful
				ES:DI buffer filled
			01h failed

----------------------------------------------------------------------
)*/
GLOBAL VOID
vesa_controller_info()
{
	struct vesa_ctrl_info *cinfo;
	int i, offset, len;
	word mode_count, OEMNameOffset, SupportedModesOffset;
	char signature[4];

	cinfo = (struct vesa_ctrl_info *)effective_addr(getES(), getDI());
	sas_loads((IU32)&cinfo->VbeSignature, signature, sizeof(signature));
	
	sas_stores((IU32)&cinfo->VbeSignature, vesa_ctrl_config.VbeSignature, 4);
	sas_storew((IU32)&cinfo->VbeVersion, vesa_ctrl_config.VbeVersion);
	sas_storedw((IU32)&cinfo->Capabilities, vesa_ctrl_config.Capabilities);
	sas_storew((IU32)&cinfo->TotalMemory, video_memory_total);

	if (strncmp(signature, "VBE2", sizeof(signature)))
	{
		vesa_is_vbe2 = FALSE;

		sas_storew((IU32)&cinfo->OEMNameOffset, VESA_BIOS_OEM_NAME_OFFS);
		sas_storew((IU32)&cinfo->OEMNameSegment, EGA_SEG);
		sas_storew((IU32)&cinfo->SupportedModesOffset, VESA_BIOS_SUPP_MODES_OFFS);
		sas_storew((IU32)&cinfo->SupportedModesSegment, EGA_SEG);

	} else {
		vesa_is_vbe2 = TRUE;

		OEMNameOffset = getDI() + offsetof(struct vesa_ctrl_info, Reserved2);
		sas_storew((IU32)&cinfo->OEMNameOffset, OEMNameOffset);
		sas_storew((IU32)&cinfo->OEMNameSegment, getES());
		len = strlen(vesa_ctrl_config.OEMName) + 1;
		sas_stores((IU32)cinfo->Reserved2, vesa_ctrl_config.OEMName, len);
		offset = len;
		
		SupportedModesOffset = getDI() + offsetof(struct vesa_ctrl_info, Reserved1);
		sas_storew((IU32)&cinfo->SupportedModesOffset, SupportedModesOffset);
		sas_storew((IU32)&cinfo->SupportedModesSegment, getES());
		for (i=0; i<sizeof(vesa_supported_modes); i++)
			sas_storew(((word*)cinfo->Reserved1)[i], vesa_supported_modes[i]);

		sas_storew((IU32)&cinfo->OemSoftwareRev, vesa_ctrl_config.OemSoftwareRev);
		
		sas_storew((IU32)&cinfo->OemVendorNameOffset, OEMNameOffset + offset);
		sas_storew((IU32)&cinfo->OemVendorNameSegment, getES());
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

/*(
----------------------------------------------------------------------

Function:	
		vesa_controller_info()

Purpose:
		Perform the int 10 extended BIOS function 4F - Subfunction 1

Input:
		CX    =  SuperVGA video mode
		ES:DI = 256-byte buffer for mode information

Output:
		AL = 4Fh if function supported
		AH = status
			00h successful
				ES:DI buffer filled
			01h failed

----------------------------------------------------------------------
)*/
GLOBAL VOID
vesa_mode_info()
{
	struct vesa_mode_info *modeinfo;
	struct vbe_mode_info  *modedata; 
	word mode = getCX();

	modeinfo = (struct vesa_mode_info *)effective_addr(getES(), getDI());
	if (mode >= 0x100 && mode <= 0x104)
	{
		modedata = (struct vbe_mode_info  *)&vesa_ext_mode_tab[mode - 0x100];
	}
	else
	{
		if ((mode >= 0 && mode <= 7) || (mode >= 0x0D && mode <= 0x13))
			modedata = &vesa_std_mode_tab[mode];
		else
		{
			setAX(0x014F);
			return;
		}
	}

	sas_storew ((IU32)&modeinfo->info.ModeAttributes, modedata->ModeAttributes);
	sas_store  ((IU32)&modeinfo->info.WindowAAttributes, modedata->WindowAAttributes);
	sas_store  ((IU32)&modeinfo->info.WindowBAttributes, modedata->WindowBAttributes);
	sas_storew ((IU32)&modeinfo->info.WindowGranularity, modedata->WindowGranularity);
	sas_storew ((IU32)&modeinfo->info.WindowSize, modedata->WindowSize);
	sas_storew ((IU32)&modeinfo->info.StartSegmentWindowA, modedata->StartSegmentWindowA);
	sas_storew ((IU32)&modeinfo->info.StartSegmentWindowB, modedata->StartSegmentWindowB);
	sas_storew ((IU32)&modeinfo->info.BankSwitchFunctionOffset, VESA_BIOS_BANK_SW_FUNC_OFFS);
	sas_storew ((IU32)&modeinfo->info.BankSwitchFunctionSegment, EGA_SEG);
	sas_storew ((IU32)&modeinfo->info.BytesPerScanLine, modedata->BytesPerScanLine);
	sas_storew ((IU32)&modeinfo->info.PixelWidth, modedata->PixelWidth);
	sas_storew ((IU32)&modeinfo->info.PixelHeight, modedata->PixelHeight);
	sas_store  ((IU32)&modeinfo->info.CharacterCellPixelWidth, modedata->CharacterCellPixelWidth);
	sas_store  ((IU32)&modeinfo->info.CharacterCellPixelHeight, modedata->CharacterCellPixelHeight);
	sas_store  ((IU32)&modeinfo->info.NumberOfMemoryPlanes, modedata->NumberOfMemoryPlanes);
	sas_store  ((IU32)&modeinfo->info.BitsPerPixel, modedata->BitsPerPixel);
	sas_store  ((IU32)&modeinfo->info.NumberOfBanks, modedata->NumberOfBanks);
	sas_store  ((IU32)&modeinfo->info.MemoryModelType, modedata->MemoryModelType);
	sas_store  ((IU32)&modeinfo->info.SizeOfBank, modedata->SizeOfBank);
	sas_store  ((IU32)&modeinfo->info.NumberOfImagePages, modedata->NumberOfImagePages);
	sas_store  ((IU32)&modeinfo->info.Reserved1, modedata->Reserved1);
	sas_store  ((IU32)&modeinfo->info.RedMaskSize, modedata->RedMaskSize);
	sas_store  ((IU32)&modeinfo->info.RedFieldPosition, modedata->RedFieldPosition);
	sas_store  ((IU32)&modeinfo->info.GreenMaskSize, modedata->GreenMaskSize);
	sas_store  ((IU32)&modeinfo->info.GreenFieldPosition, modedata->GreenFieldPosition);
	sas_store  ((IU32)&modeinfo->info.BlueMaskSize, modedata->BlueMaskSize);
	sas_store  ((IU32)&modeinfo->info.BlueFieldPosition, modedata->BlueFieldPosition);
	sas_store  ((IU32)&modeinfo->info.ReservedMaskSize, modedata->ReservedMaskSize);
	sas_store  ((IU32)&modeinfo->info.ReservedFieldPosition, modedata->ReservedFieldPosition);
	sas_store  ((IU32)&modeinfo->info.DirectColourModeInfo, modedata->DirectColourModeInfo);
	if (vesa_is_vbe2)
	{
		sas_storedw((IU32)&modeinfo->info.PhysBasePtr, modedata->PhysBasePtr);
		sas_storedw((IU32)&modeinfo->info.OffScreenMemOffset, modedata->OffScreenMemOffset);
		sas_storew ((IU32)&modeinfo->info.OffScreenMemSize, modedata->OffScreenMemSize);
	}
	setAX(0x004F);
}

/*(
----------------------------------------------------------------------

Function:	
		vesa_set_mode()

Purpose:
		Perform the int 10 extended BIOS function 4F - Subfunction 2

Input:
		BX    = new video mode
		ES:DI = (VBE 3.0+) CRTC information block, bit mode bit 11 set

Output:
		AL = 4Fh if function supported
		AH = status
			00h successful
			01h failed

----------------------------------------------------------------------
)*/
GLOBAL VOID
vesa_set_mode()
{
	word mode = getBX() & 0x1FF;

	if (mode >= 0x100 && mode <= 0x104)
	{
		vesa_vesamode = mode;
		vesa_v7mode   = vesa_v7_mode_map[mode - 0x100];
		setBL(vesa_v7mode);
		v7vga_extended_set_mode();
		setBX(mode);
		setAX(0x004F);
	}
	else
	{
		if ((mode >= 0 && mode <= 7) || (mode >= 0x0D && mode <= 0x13))
		{
			vesa_v7mode   = (byte)mode;
			vesa_vesamode = mode;
			setAL(getBL());
			ega_set_mode();
			setAX(0x004F);
		}
		else
		{
			setAX(0x014F);
		}
	}
	if (getAX() == 0x4F)
	{
		if (!v7_extension_enabled()) v7_enable_extension();
		if (!v7_1mb_bank_wrap_get()) v7_1mb_bank_wrap_set();
		if (!v7_bank_selection_get()) v7_bank_selection_set();
	}
}

/*(
----------------------------------------------------------------------

Function:	
		vesa_get_mode()

Purpose:
		Perform the int 10 extended BIOS function 4F - Subfunction 3

Input:
		None

Output:
		AL = 4Fh if function supported
		AH = status
			00h successful
				BX = video mode
					bit 13: VBE/AF v1.0P accelerated video mode
					bit 14: linear frame buffer enabled (VBE v2.0+)
					bit 15: don't clear video memory
			01h failed

----------------------------------------------------------------------
)*/
GLOBAL VOID
vesa_get_mode()
{
	if (!vesa_vesamode)
		setBX(sas_hw_at(vd_video_mode));
	else
		setBX(vesa_vesamode);
	
	setAX(0x004F);
}

/*(
----------------------------------------------------------------------

Function:	
		vesa_save_restore()

Purpose:
		Perform the int 10 extended BIOS function 4F - Subfunction 4

Input:
		DL    =  subfunction
			00h get state buffer size
				Return: BX = number of 64-byte blocks needed
			01h save video states
				ES:BX -> buffer
			02h restore video states
				ES:BX -> buffer
		CX    = states to save/restore

Output:
		AL = 4Fh if function supported
		AH = status
			00h successful
			01h failed

----------------------------------------------------------------------
)*/
GLOBAL VOID
vesa_save_restore()
{
	video_adapter = VGA;
	vga_int_1C();
}


/*(
----------------------------------------------------------------------

Function:	
		vesa_window_control()

Purpose:
		Perform the int 10 extended BIOS function 4F - Subfunction 5

Input:
		BH     = subfunction
			00h select video memory window
				DX = window address in video memory (in granularity units)
			01h get video memory window
				Return: DX = window address in video memory (in gran. units)
		BL     = window number
			00h window A
			01h window B
		ES     = selector for memory-mapped registers (VBE 2.0+, when 
		         called from 32-bit protected mode)

Output:
		AL = 4Fh if function supported
		AH = status
			00h successful
			01h failed

----------------------------------------------------------------------
)*/
GLOBAL VOID
vesa_window_control()
{
	byte bit16, bit17, bitlo;

	if (getBL() == 0)
	{
		switch (getBH())
		{
		case 0: // select video memory window
			if (getDX() < 8)
			{
				v7_window_control();
				setAX(0x004F);
				return;
			}
			break;
		case 1: // get video memory window
			if (vga_plane_enabled())
			{
				outb(EGA_SEQ_INDEX, 0xf9);  // Page Select
				inb(EGA_SEQ_DATA, &bit16);
				inb(EGA_GC_POS1, &bit17);
				outb(EGA_SEQ_INDEX, 0xf6);
				inb(EGA_SEQ_DATA, &bitlo);
				setDX((bit16&1) | (((bit17 >> 5) & 1) << 1) | ((bitlo & 3) << 2));
			}
			else
			{
				outb(EGA_SEQ_INDEX, 0xf6);
				inb(EGA_SEQ_DATA, &bitlo);
				setDX(bitlo & 3);
			}
			setAX(0x004F);
			return;
		}
	}
	setAX(0x014F);
}


/*(
----------------------------------------------------------------------

Function:	
		vesa_logical_scan_len()

Purpose:
		Perform the int 10 extended BIOS function 4F - Subfunction 6

Input:
		BH     = subfunction
			00h set scan line length
				CX = desired width in pixels
			01h get scan line length

Output:
		AL = 4Fh if function supported
		AH = status
			00h successful
			01h failed
		BX = bytes per scan line
		CX = number of pixels per scan line
		DX = maximum number of scan lines

----------------------------------------------------------------------
)*/
GLOBAL VOID
vesa_logical_scan_len()
{
	word mode, width, bytes_per_scanline, lines;
	boolean plane_enabled;
	byte val, bytes_per_pixel;
	struct vbe_mode_info *mode_entry;

	mode = vesa_vesamode & 0x1FF;
	if (mode < 0x100 || mode >= 0x105)
	{
		setAX(0x014F);
		return;
	}
	plane_enabled = vga_plane_enabled();

	switch(getBL())
	{
	case 0: // set scan line length
		width = getCX();
		width >>= (plane_enabled?2:3);

set_scanlines:
		outb(EGA_CRTC_INDEX, 0x17);
		inb(EGA_CRTC_DATA, &val);
		bytes_per_pixel = (val & 8)?4:2;

		lines = width / bytes_per_pixel;
		if (lines >= 256)
		{
			setAX(0x024F);
			return;
		}
		outb(EGA_CRTC_INDEX, 0x13);
		outb(EGA_CRTC_DATA, (byte)lines);
		bytes_per_scanline = bytes_per_pixel * lines;
ret_scanlines:
		setBX(plane_enabled?bytes_per_scanline*4:bytes_per_scanline);
		setCX(bytes_per_scanline * (plane_enabled?4:8));
		setDX(0x80000 / (4 * bytes_per_scanline));
		setAX(0x004F);
		break;
	case 1: // get scan line length
		bytes_per_scanline = v7_get_bytes_per_scanline();
		goto ret_scanlines;
	case 2:
		width = getCX();
		if (plane_enabled) width >>= 2;
		goto set_scanlines;
	case 3:
		mode_entry = (struct vbe_mode_info *)&vesa_ext_mode_tab[mode - 0x100];
		bytes_per_scanline = mode_entry->PixelHeight * mode_entry->BitsPerPixel;
		if (bytes_per_scanline < 0) bytes_per_scanline += 7;
		bytes_per_scanline = 0x80000 / ((int)bytes_per_scanline / 8);
		setBX(bytes_per_scanline<4096?bytes_per_scanline:4096);
		setAX(0x004F);
		break;
	default:
		setAX(0x0200);
		break;
	}
}

/*(
----------------------------------------------------------------------

Function:	
		vesa_display_start()

Purpose:
		Perform the int 10 extended BIOS function 4F - Subfunction 7

Input:
		BL     = subfunction
			00h,80h set display start
				CX = leftmost displayed pixel in scan line
				DX = first displayed scan line
			01h get display start
				Return: BH = 00h
				CX = leftmost displayed pixel in scan line
				DX = first displayed scan line
		BH     = 00h (reserved)

Output:
		AL = 4Fh if function supported
		AH = status
			00h successful
			01h failed

----------------------------------------------------------------------
)*/
GLOBAL VOID
vesa_display_start()
{
	word mode, bytes_per_scanline;
	byte banksel, offs_in[2];
	boolean plane_enabled;
	unsigned int offset;
	struct vbe_mode_info *mode_entry;

	if (getBH()) return;
	mode = vesa_vesamode & 0x1FF;
	if (mode < 0x100 || mode >= 0x105)
	{
		setAX(0x014F);
		return;
	}
	bytes_per_scanline = v7_get_bytes_per_scanline();
	plane_enabled = vga_plane_enabled();

	switch(getBL())
	{
	case 0x00:
	case 0x80:
		offset = (getCX() / (plane_enabled?4:8)) + (getDX() * bytes_per_scanline);
		mode_entry = (struct vbe_mode_info *)&vesa_ext_mode_tab[mode - 0x100];
		if (offset < 0x40000 && offset + (mode_entry->PixelHeight * mode_entry->PixelWidth / (plane_enabled?4:8)) * 4)
		{
			outb(EGA_CRTC_INDEX, 0xc);
			outb(EGA_CRTC_DATA, (byte)(offset >> 16));
			outb(EGA_CRTC_INDEX, 0xd);
			outb(EGA_CRTC_DATA, (byte)(offset));
			outb(EGA_SEQ_INDEX, 0xf6);
			inb (EGA_SEQ_DATA, &banksel);
			outb(EGA_SEQ_DATA, banksel & 0xCF | (offset >> 12) & 0x30);
			setAX(0x004F);
			return;
		}
		break;
	case 0x01:
		outb(EGA_CRTC_INDEX, 0xc);
		inb (EGA_CRTC_DATA, &offs_in[1]);
		outb(EGA_CRTC_INDEX, 0xd);
		inb (EGA_CRTC_DATA, &offs_in[0]);
		outb(EGA_SEQ_INDEX, 0xf6);
		inb (EGA_SEQ_DATA, &banksel);
		offset = (word)offs_in | (((word)banksel & 0x30) << 12);
		setCX((plane_enabled?4:8) * offset - offset);
		setDX(offset / bytes_per_scanline);
		setAX(0x004F);
		return;
	}
	setAX(0x014F);
}

/*(
----------------------------------------------------------------------

Function:	
		vesa_dac_palette()

Purpose:
		Perform the int 10 extended BIOS function 4F - Subfunction 8

Input:
		BL     = subfunction
			00h set DAC palette width
				BH = desired number of bits per primary color
			01h get DAC palette width

Output:
		AL = 4Fh if function supported
		AH = status
			00h successful
				BH = current number of bits per primary (06h = standard VGA)
			01h failed

----------------------------------------------------------------------
)*/
GLOBAL VOID
vesa_dac_palette()
{
	switch (getBL())
	{
	case 0:
		if (getBH() == 6)
		{
			setAX(0x004F);
			return;
		}
		break;
	case 1:
		setBH(6);
		setAX(0x004F);
		return;
	}
	setAX(0x014F);
}

/*(
----------------------------------------------------------------------

Function:	
		vesa_palette_data()

Purpose:
		Perform the int 10 extended BIOS function 4F - Subfunction 9

Input:
		BL     = subfunction
		    00h set (primary) palette
			01h get (primary) palette
			02h set secondary palette data
			03h get secondary palette data
			80h set palette during vertical retrace
		CX     = number of entries to change
		DX     = starting palette index
		ES:DI  = palette buffer, array of DAC entries (see #00086)


Output:
		AL = 4Fh if function supported
		AH = status
			00h successful
			01h failed

----------------------------------------------------------------------
)*/
GLOBAL VOID
vesa_palette_data()
{
	IUH addr;
	int i, len;
	byte val;

	switch (getBL())
	{
	case 0x00:
	case 0x80:
		addr = effective_addr(getES(), getDI());
		outb(VGA_DAC_WADDR, getDL());
		for (i=0, len=getCX(); i<len; i++, addr+=4)
		{
			outb(VGA_DAC_DATA, sas_hw_at_no_check(addr + 2));
			outb(VGA_DAC_DATA, sas_hw_at_no_check(addr + 1));
			outb(VGA_DAC_DATA, sas_hw_at_no_check(addr + 0));
		}
		setAX(0x004F);
		return;
	case 0x01:
		addr = effective_addr(getES(), getDI());
		outb(VGA_DAC_RADDR, getDL());
		for (i=0, len=getCX(); i<len; i++, addr+=4)
		{
			inb(VGA_DAC_DATA, &val);
			sas_store(addr + 2, val);
			inb(VGA_DAC_DATA, &val);
			sas_store(addr + 1, val);
			inb(VGA_DAC_DATA, &val);
			sas_store(addr + 0, val);
		}
		setAX(0x004F);
		return;
	case 0x02:
	case 0x03:
		setAX(0x0200);
		return;
	}
	setAX(0x014F);
}

/*(
----------------------------------------------------------------------

Function:	
		vesa_return_prot_mode_interface()

Purpose:
		Perform the int 10 extended BIOS function 4F - Subfunction 0A

Input:
		BL     = 00h

Output:
		AL = 4Fh if function supported
		AH = status
			00h successful
				ES:DI -> protected-mode table
				CX = length of table in bytes, included protected-mode code
			01h failed

----------------------------------------------------------------------
)*/
GLOBAL VOID
vesa_return_prot_mode_interface()
{
	if ( getBL() )
	{
		setES(0);
		setDI(0);
		setCX(0);
		setAX(0x014F);
	}
	else
	{
		setES(EGA_SEG);
		setDI(VESA_BIOS_PROT_M_TBL_OFFS);
		setCX(VESA_BIOS_PROT_M_TBL_LEN);
		setAX(0x004F);
	}
}

//
// Local VIDEO7 configuration routines
//

LOCAL boolean
v7_extension_enabled()
{
	byte enabled;

	outb(EGA_SEQ_INDEX, 6);
	inb(EGA_SEQ_DATA, &enabled);
	return enabled == 1;
}

LOCAL VOID
v7_enable_extension()
{
  outb(EGA_SEQ_INDEX, 6);
  outb(EGA_SEQ_DATA, 0xEA);
}

LOCAL boolean
v7_1mb_bank_wrap_get()
{
	byte banksel;

	outb(EGA_SEQ_INDEX, 0xf6);
	inb(EGA_SEQ_DATA, &banksel);
	return banksel & 0x40;
}

LOCAL VOID
v7_1mb_bank_wrap_set()
{
	byte banksel;

	outb(EGA_SEQ_INDEX, 0xf6);
	inb(EGA_SEQ_DATA, &banksel);
	outb(EGA_SEQ_DATA, banksel | 0x40);
}

LOCAL boolean
v7_bank_selection_get()
{
	byte banksel;

	outb(EGA_SEQ_INDEX, 0xff);
	inb(EGA_SEQ_DATA, &banksel);
	return banksel & 0x10;
}

LOCAL VOID
v7_bank_selection_set()
{
	byte banksel;

	outb(EGA_SEQ_INDEX, 0xff);
	inb(EGA_SEQ_DATA, &banksel);
	outb(EGA_SEQ_DATA, banksel | 0x10);
}

LOCAL boolean
vga_plane_enabled()
{
	byte val;

	outb(EGA_SEQ_INDEX, 0x4);
	inb(EGA_SEQ_DATA, &val);
	return val & 8;
}

LOCAL VOID
v7_window_control()
{
	byte dx = getDL(), bit17, bank;

	if (vga_plane_enabled())
	{
		outb(EGA_SEQ_INDEX, 0xf9);  // Page Select
		outb(EGA_SEQ_DATA, dx & 1);
		inb(EGA_GC_POS1, &bit17);
		outb(EGA_MISC_REG, bit17 & 0xdf | ((dx & 2) << 4));
		outb(EGA_SEQ_INDEX, 0xf6); // 1MB RAM Bank select
		inb(EGA_SEQ_DATA, &bank);
		outb(EGA_SEQ_DATA, bank & 0xF0 | dx & 0xC | (dx >> 2) & 3);
	}
	else
	{
		outb(EGA_SEQ_INDEX, 0xf6); // 1MB RAM Bank select
		inb(EGA_SEQ_DATA, &bank);
		outb(EGA_SEQ_DATA, bank & 0xF0 | dx & 3 | (dx & 3) << 2);
	}
}

LOCAL word
v7_get_bytes_per_scanline()
{
	byte val, bytes_per_pixel, lines;

	outb(EGA_CRTC_INDEX, 0x13);
	inb(EGA_CRTC_DATA, &lines);
	
	outb(EGA_CRTC_INDEX, 0x17);
	inb(EGA_CRTC_DATA, &val);
	bytes_per_pixel = (val & 8)?4:2;

	return (word)bytes_per_pixel * (word)lines;
}

#endif /* V7VGA */ 