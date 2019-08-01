#include "sevid000.h"

void S_2624_Chain2ByteWrite_00000000 (IU32 eaOff, IU8 eaVal)
{
  ENTER_FUNC(2624);
  S_2625_Chain2ModeXByteWrite_00000000(eaOff, eaVal);
}

void S_2625_Chain2ModeXByteWrite_00000000 (IU32 eaOff, IU8 eaVal)
{
  C2MXBWRTF(2625,UCBMSK,NOTRANS,C2MXWRTWPL);
}

void S_2626_Chain2ByteFill_00000000 (IU32 eaOff, IU8 eaVal, IU32 count)
{
  ENTER_FUNC(2626);
  S_2627_Chain2ModeXByteFill_00000000(eaOff, eaVal, count);
}

void S_2627_Chain2ModeXByteFill_00000000 (IU32 eaOff, IU8 eaVal, IU32 count)
{
  C2MXBFLLF(2627,NOTRANS,NOTRANS,S_2625_Chain2ModeXByteWrite_00000000);
}

void S_2628_Chain2ByteMove_00000000_Fwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  ENTER_FUNC(2628);
  S_2629_Chain2ModeXFwdByteMove_00000000(eaOff, fromOff, count, srcInRAM);
}

void S_2629_Chain2ModeXFwdByteMove_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  C2MXBMOVFWD(2629, S_2625_Chain2ModeXByteWrite_00000000);
}

void S_2630_Chain2WordWrite_00000000 (IU32 eaOff, IU16 eaVal)
{
  ENTER_FUNC(2630);
  S_2631_Chain2ModeXWordWrite_00000000(eaOff, (IU8)eaVal);
}

void S_2631_Chain2ModeXWordWrite_00000000 (IU32 eaOff, IU16 eaVal)
{
  C2MXWWRTF(2631,NOTRANS,NOTRANS,S_2625_Chain2ModeXByteWrite_00000000);
}

void S_2632_Chain2WordFill_00000000 (IU32 eaOff, IU16 eaVal, IU32 count)
{
  ENTER_FUNC(2632);
  S_2633_Chain2ModeXWordFill_00000000(eaOff, (IU8)eaVal, count);
}

void S_2633_Chain2ModeXWordFill_00000000 (IU32 eaOff, IU16 eaVal, IU32 count)
{
  C2MXWFLLF(2633,NOTRANS,NOTRANS,S_2625_Chain2ModeXByteWrite_00000000);
}

void S_2634_Chain2WordMove_00000000_Fwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  ENTER_FUNC(2634);
  S_2635_Chain2ModeXFwdWordMove_00000000(eaOff, fromOff, count, srcInRAM);
}

void S_2635_Chain2ModeXFwdWordMove_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  C2MXWMOVFWD(2635, S_2625_Chain2ModeXByteWrite_00000000, S_2631_Chain2ModeXWordWrite_00000000);
}

void S_2636_Chain2DwordWrite_00000000 (IU32 eaOff, IU32 eaVal)
{
  ENTER_FUNC(2636);
  S_2631_Chain2ModeXWordWrite_00000000(eaOff, eaVal);
  S_2631_Chain2ModeXWordWrite_00000000(eaOff + 4, 0);
}

void S_2637_Chain2ModeXDwordWrite_00000000 (IU32 eaOff, IU32 eaVal)
{
  C2MXDWRTF(2637, S_2631_Chain2ModeXWordWrite_00000000);
}

void S_2638_Chain2DwordFill_00000000 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  ENTER_FUNC(2638);
  S_2639_Chain2ModeXDwordFill_00000000(eaOff, eaVal, count);
}

void S_2639_Chain2ModeXDwordFill_00000000 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  C2MXDFLLF(2639,NOTRANS,NOTRANS,S_2625_Chain2ModeXByteWrite_00000000);
}

void S_2640_Chain2DwordMove_00000000_Fwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  S_2641_Chain2ModeXFwdDwordMove_00000000(eaOff, fromOff, count, srcInRAM);
}

void S_2641_Chain2ModeXFwdDwordMove_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  C2MXDMOVFWD(2641, S_2635_Chain2ModeXFwdWordMove_00000000);
}
