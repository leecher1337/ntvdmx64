#include "sevid000.h"

void S_2660_Chain2ByteWrite_00000002 (IU32 eaOff, IU8 eaVal)
{
  ENTER_FUNC(2660);
  S_2661_Chain2ModeXByteWrite_00000002(eaOff, eaVal);
}

void S_2661_Chain2ModeXByteWrite_00000002 (IU32 eaOff, IU8 eaVal)
{
  C2MXBWRTF(2661,UCBMSK,TRANS1,C2MXWRTWPL);
}

void S_2662_Chain2ByteFill_00000002 (IU32 eaOff, IU8 eaVal, IU32 count)
{
  ENTER_FUNC(2662);
  S_2663_Chain2ModeXByteFill_00000002(eaOff, eaVal, count);
}

void S_2663_Chain2ModeXByteFill_00000002 (IU32 eaOff, IU8 eaVal, IU32 count)
{
  C2MXBFLLF(2663,NOTRANS,NOTRANS,S_2661_Chain2ModeXByteWrite_00000002);
}

void S_2664_Chain2ByteMove_00000002_Fwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  ENTER_FUNC(2664);
  S_2665_Chain2ModeXFwdByteMove_00000002(eaOff, fromOff, count, srcInRAM);
}

void S_2665_Chain2ModeXFwdByteMove_00000002 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  C2MXBMOVFWD(2665, S_2661_Chain2ModeXByteWrite_00000002);
}

void S_2666_Chain2WordWrite_00000002 (IU32 eaOff, IU16 eaVal)
{
  ENTER_FUNC(2666);
  S_2667_Chain2ModeXWordWrite_00000002(eaOff, (IU8)eaVal);
}

void S_2667_Chain2ModeXWordWrite_00000002 (IU32 eaOff, IU16 eaVal)
{
  C2MXWWRTF(2667,NOTRANS,NOTRANS,S_2661_Chain2ModeXByteWrite_00000002);
}

void S_2668_Chain2WordFill_00000002 (IU32 eaOff, IU16 eaVal, IU32 count)
{
  ENTER_FUNC(2668);
  S_2669_Chain2ModeXWordFill_00000002(eaOff, (IU8)eaVal, count);
}

void S_2669_Chain2ModeXWordFill_00000002 (IU32 eaOff, IU16 eaVal, IU32 count)
{
  C2MXWFLLF(2669,NOTRANS,NOTRANS,S_2667_Chain2ModeXWordWrite_00000002);
}

void S_2670_Chain2WordMove_00000002_Fwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  ENTER_FUNC(2670);
  S_2671_Chain2ModeXFwdWordMove_00000002(eaOff, fromOff, count, srcInRAM);
}

void S_2671_Chain2ModeXFwdWordMove_00000002 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  C2MXWMOVFWD(2671, S_2661_Chain2ModeXByteWrite_00000002, S_2667_Chain2ModeXWordWrite_00000002);
}

void S_2672_Chain2DwordWrite_00000002 (IU32 eaOff, IU32 eaVal)
{
  ENTER_FUNC(2672);
  S_2667_Chain2ModeXWordWrite_00000002(eaOff, eaVal);
  S_2667_Chain2ModeXWordWrite_00000002(eaOff + 4, 0);
}

void S_2673_Chain2ModeXDwordWrite_00000002 (IU32 eaOff, IU32 eaVal)
{
  C2MXDWRTF(2673, S_2667_Chain2ModeXWordWrite_00000002);
}

void S_2674_Chain2DwordFill_00000002 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  ENTER_FUNC(2674);
  S_2675_Chain2ModeXDwordFill_00000002(eaOff, eaVal, count);
}

void S_2675_Chain2ModeXDwordFill_00000002 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  C2MXDFLLF(2675,NOTRANS,NOTRANS,S_2667_Chain2ModeXWordWrite_00000002);
}

void S_2676_Chain2DwordMove_00000002_Fwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  S_2677_Chain2ModeXFwdDwordMove_00000002(eaOff, fromOff, count, srcInRAM);
}

void S_2677_Chain2ModeXFwdDwordMove_00000002 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  C2MXDMOVFWD(2658, S_2671_Chain2ModeXFwdWordMove_00000002);
}

