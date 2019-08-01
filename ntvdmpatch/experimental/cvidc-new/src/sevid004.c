#include "sevid000.h"

void S_2396_UnchainedByteWrite_00000001_00000000_00000000 (IU32 eaOff, IU8 eaVal)
{
  ENTER_FUNC(2396);
  GDP->VGAGlobals.mark_byte(eaOff);
  UCBWRTWPL(4 * eaOff,GDP->VGAGlobals.latches);
}

void S_2397_UnchainedByteFill_00000001_00000000_00000000 (IU32 eaOff, IU8 eaVal, IU32 count)
{
  ENTER_FUNC(2397);  
  GDP->VGAGlobals.mark_string(eaOff, count);
  UCBPLNBFLL1(4 * eaOff,GDP->VGAGlobals.latches,count);
}

void S_2398_UnchainedByteMove_00000001_00000000_00000000_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCBMOVFW1(2398, S_2397_UnchainedByteFill_00000001_00000000_00000000, S_2169_CopyDirByte1Plane_00000000);
}

void S_2399_UnchainedWordWrite_00000001_00000000_00000000 (IU32 eaOff, IU16 eaVal)
{
  ENTER_FUNC(2399);
  GDP->VGAGlobals.mark_word(eaOff);
  UCBWRTWPL(4 * (eaOff+0),GDP->VGAGlobals.latches); \
  UCBWRTWPL(4 * (eaOff+1),GDP->VGAGlobals.latches);
}

void S_2400_UnchainedWordFill_00000001_00000000_00000000 (IU32 eaOff, IU8 eaVal, IU32 count)
{
  IU32 data;

  ENTER_FUNC(2400);  
  GDP->VGAGlobals.mark_string(eaOff, count * 2);
  UCBPLNWFLL1(4 * eaOff,GDP->VGAGlobals.latches,GDP->VGAGlobals.latches,count);
}

void S_2401_UnchainedWordMove_00000001_00000000_00000000_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCWMOVFW1(2401, S_2400_UnchainedWordFill_00000001_00000000_00000000, S_2174_CopyDirWord1Plane_00000000);
}

void S_2402_UnchainedDwordWrite_00000001_00000000_00000000 (IU32 eaOff, IU32 eaVal)
{
  UCDWRTF(2402,S_2399_UnchainedWordWrite_00000001_00000000_00000000);
}

void S_2403_UnchainedDwordFill_00000001_00000000_00000000 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  UCDFLLF(2403,S_2400_UnchainedWordFill_00000001_00000000_00000000);
}

void S_2404_UnchainedDwordMove_00000001_00000000_00000000_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCDMOVFW(2404, S_2401_UnchainedWordMove_00000001_00000000_00000000_00000000);
}


