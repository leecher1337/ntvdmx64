#include "sevid000.h"

void S_2867_Chain4ByteWrite_00000001_00000000 (IU32 eaOff, IU8 eaVal)
{
  C4BWRTF1(2867, NOMSK, NOTRANS);
}

void S_2868_Chain4ByteFill_00000001_00000000 (IU32 eaOff, IU8 eaVal, IU32 count)
{
  C4BFLL1(2868, NOMSK, NOTRANS);
}

void S_2869_Chain4ByteMove_00000001_00000000_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  ENTER_FUNC(2869);
  EVID_MARK_STRING(eaOff, count);
  if ( srcInRAM )
    S_2868_Chain4ByteFill_00000001_00000000(eaOff, -1, count);
  else
  {
    S_2870_CopyBytePlnByPlnChain4_00000001_00000000_00000000_00000000(eaOff, 
      &GDP->VGAGlobals.VGA_rplane[fromOff],fromOff, count, 0);
  }
}

void S_2870_CopyBytePlnByPlnChain4_00000001_00000000_00000000_00000000 (IU32 eaOff, IHPE fromOff, IHPE destOff, IU32 count, IBOOL srcInRAM)
{
  C4BCPYB4PLN1(2870,+1,NOTRANS,NOMSKL);
}

void S_2871_Chain4WordWrite_00000001_00000000 (IU32 eaOff, IU16 eaVal)
{
  C4WWRTF1(2871, NOMSK, NOTRANS);
}

void S_2872_Chain4WordFill_00000001_00000000 (IU32 eaOff, IU16 eaVal, IU32 count)
{
  ENTER_FUNC(2872);
  eaVal = (IU8)GDP->VGAGlobals.latches | (IU16)(GDP->VGAGlobals.latches<<8);
  C4WFLL(2872, NOMSKX, NOTRANS);
}

void S_2873_Chain4WordMove_00000001_00000000_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  C4WMOVFW1(2873,S_2872_Chain4WordFill_00000001_00000000,S_2174_CopyDirWord1Plane_00000000);
}

void S_2874_Chain4DwordWrite_00000001_00000000 (IU32 eaOff, IU32 eaVal)
{
  C4DWRTF(2874, S_2871_Chain4WordWrite_00000001_00000000);
}

void S_2875_Chain4DwordFill_00000001_00000000 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  /*
   * Write mode 1 (latch-copy): the sibling S_2872 word-fill is hand-written
   * and IGNORES eaVal (it overwrites it from latches), so a dword-fill of
   * `count` dwords (4*count bytes) delegates to word-fill with 2*count
   * words (4*count bytes) - same byte count, same latch-derived value.
   * Bypasses the new C4DFLL signature because this write-mode-1 case has
   * no mask/trans triple like the standard states.
   */
  ENTER_FUNC(2875);
  S_2872_Chain4WordFill_00000001_00000000(eaOff, (IU16)eaVal, 2 * count);
}

void S_2876_Chain4DwordMove_00000001_00000000_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  C4DMOVFW(2876,S_2873_Chain4WordMove_00000001_00000000_00000000);
}

