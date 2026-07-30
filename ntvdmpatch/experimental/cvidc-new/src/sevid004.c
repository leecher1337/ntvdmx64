#include "sevid000.h"

void S_2396_UnchainedByteWrite_00000001_00000000_00000000 (IU32 eaOff, IU8 eaVal)
{
  ENTER_FUNC(2396);
  EVID_MARK_BYTE(eaOff);
  UCBWRTWPL(4 * eaOff,GDP->VGAGlobals.latches);
}

void S_2397_UnchainedByteFill_00000001_00000000_00000000 (IU32 eaOff, IU8 eaVal, IU32 count)
{
  ENTER_FUNC(2397);  
  EVID_MARK_STRING(eaOff, count);
  UCBPLNBFLL1(4 * eaOff,GDP->VGAGlobals.latches,count);
}

void S_2398_UnchainedByteMove_00000001_00000000_00000000_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCBMOVFW1(2398, S_2397_UnchainedByteFill_00000001_00000000_00000000, S_2169_CopyDirByte1Plane_00000000);
}

void S_2399_UnchainedWordWrite_00000001_00000000_00000000 (IU32 eaOff, IU16 eaVal)
{
  ENTER_FUNC(2399);
  EVID_MARK_WORD(eaOff);
  UCBWRTWPL(4 * (eaOff+0),GDP->VGAGlobals.latches); \
  UCBWRTWPL(4 * (eaOff+1),GDP->VGAGlobals.latches);
}

void S_2400_UnchainedWordFill_00000001_00000000_00000000 (IU32 eaOff, IU8 eaVal, IU32 count)
{
  IU32 data;

  ENTER_FUNC(2400);  
  EVID_MARK_STRING(eaOff, count * 2);
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
  /*
   * Write mode 1 base state: sibling S_2400 is hand-written and IGNORES
   * eaVal (write mode 1 copies latches to destination).  N-dword fill =
   * 2N-word fill (same 4N byte count, same latch-derived per-byte value).
   */
  ENTER_FUNC(2403);
  S_2400_UnchainedWordFill_00000001_00000000_00000000(eaOff, (IU8)eaVal, 2 * count);
}

void S_2404_UnchainedDwordMove_00000001_00000000_00000000_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCDMOVFW(2404, S_2401_UnchainedWordMove_00000001_00000000_00000000_00000000);
}


