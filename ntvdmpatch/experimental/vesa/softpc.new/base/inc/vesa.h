/*
 * SoftPC V 4.0
 *
 * Title	: VESA definitions
 *
 * Description	: VESA BIOS module definitions
 *
 * Author	: leecher1337
 *
 * Notes	: Reverse-engineered from IRIX SoftWindows95 V4.0
 *
 */

#include "vbe.h"

struct vesa_ctrl_config
{
	char *VbeSignature;
	word VbeVersion;
	char *OEMName;
	IU32 Capabilities;
	word OemSoftwareRev;
	char *OemVendorName;
	char *OemProductName;
	char *OemProductRev;
};

// Offsets in v7vga.rom
// TODO: put then im sas.h?
#define VESA_BIOS_OEM_NAME_OFFS		0x4999
#define VESA_BIOS_SUPP_MODES_OFFS	0x4A61
#define VESA_BIOS_BANK_SW_FUNC_OFFS	0x4A89
#define VESA_BIOS_PROT_M_TBL_OFFS	0x4A90
#define VESA_BIOS_PROT_M_TBL_LEN	60

