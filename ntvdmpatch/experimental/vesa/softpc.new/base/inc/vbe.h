/*
 * SoftPC V 4.0
 *
 * Title	: VBE definitions
 *
 * Description	: VESA BIOS Extensions definitions for use with VESA functions
 *
 * Author	: leecher1337
 *
 * Notes	: Reverse-engineered from IRIX SoftWindows95 V4.0
 *
 */

#pragma pack(1)

/* 
 * VESA Mode information structure
 */
struct vbe_mode_info
{
/*
Bit(s)  Description     (Table 00080)
0      mode supported by present hardware configuration
1      optional information available (must be =1 for VBE v1.2+)
2      BIOS output supported
3      set if color, clear if monochrome
4      set if graphics mode, clear if text mode
---VBE v2.0+ ---
5      mode is not VGA-compatible
6      bank-switched mode not supported
7      linear framebuffer mode supported
8      double-scan mode available (e.g. 320x200 and 320x240)
---VBE v3.0 ---
9      interlaced mode available
10     hardware supports triple buffering
11     hardware supports stereoscopic display
12     dual display start address support
13-15  reserved
---VBE/AF v1.0P---
9      application must call EnableDirectAccess before calling bank-switching
functions
*/
  word ModeAttributes;
  // + D0 = Relocatable windows supported (0=single non-reloc win only)
  // + D1 = Window readable  (0=not readable)
  // + D2 = Window writeable (0=not writeable);
  // - D3-D7 = Reserved 
  byte WindowAAttributes;
  byte WindowBAttributes;
  word WindowGranularity;
  word WindowSize;
  word StartSegmentWindowA;
  word StartSegmentWindowB;
  word BankSwitchFunctionOffset;
  word BankSwitchFunctionSegment;
  word BytesPerScanLine;
  word PixelWidth;
  word PixelHeight;
  byte CharacterCellPixelWidth;
  byte CharacterCellPixelHeight;
  byte NumberOfMemoryPlanes;
  byte BitsPerPixel;
  byte NumberOfBanks;
  //   00h =           Text mode
  //   01h =           CGA graphics
  //   02h =           Hercules graphics
  //   03h =           4-plane planar
  // + 04h =           Packed pixel
  //   05h =           Non-chain 4, 256 color
  // + 06h =           Direct Color
  //   07h =           YUV
  //   08h-0Fh =       Reserved, to be defined by VESA
  //   10h-FFh =       To be defined by OEM 
  byte MemoryModelType;
  byte SizeOfBank;
  byte NumberOfImagePages;
  byte Reserved1;
  byte RedMaskSize;
  byte RedFieldPosition;
  byte GreenMaskSize;
  byte GreenFieldPosition;
  byte BlueMaskSize;
  byte BlueFieldPosition;
  byte ReservedMaskSize;
  byte ReservedFieldPosition;
  // - D0 = Color ramp is fixed/programmable (0=fixed)
  // + D1 = Bits in Rsvd field are usable/reserved (0=reserved) 
  byte DirectColourModeInfo;
  double_word PhysBasePtr;
  double_word OffScreenMemOffset;
  word OffScreenMemSize;
};

struct vesa_mode_info
{
  struct vbe_mode_info info;
  byte Reserved2[206];
};


/* 
 * VESA Controller information structure
 */
struct vesa_ctrl_info
{
  char VbeSignature[4];
  word VbeVersion;
  word OEMNameOffset;
  word OEMNameSegment;
  IU32 Capabilities;
  word SupportedModesOffset;
  word SupportedModesSegment;
  word TotalMemory;
  word OemSoftwareRev;
  word OemVendorNameOffset;
  word OemVendorNameSegment;
  word OemProductNameOffset;
  word OemProductNameSegment;
  word OemProductRevOffset;
  word OemProductRevSegment;
  byte Reserved1[222];
  byte Reserved2[256];
};
#pragma pack()
