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
  GDP->VGAGlobals.mark_string(eaOff, count);
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
  C4DFLL(2875, S_2872_Chain4WordFill_00000001_00000000);
}

void S_2876_Chain4DwordMove_00000001_00000000_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  C4DMOVFW(2876,S_2873_Chain4WordMove_00000001_00000000_00000000);
}

