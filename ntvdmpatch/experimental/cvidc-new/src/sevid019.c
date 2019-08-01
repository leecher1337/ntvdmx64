#include "sevid000.h"

void S_2678_Chain2ByteWrite_00000003 (IU32 eaOff, IU8 eaVal)
{
  ENTER_FUNC(2678);
  S_2679_Chain2ModeXByteWrite_00000003(eaOff, eaVal);
}

void S_2679_Chain2ModeXByteWrite_00000003 (IU32 eaOff, IU8 eaVal)
{
  C2MXBWRTF(2679,UCB7MSK,NOTRANS,C2MXWRTWPL);
}

void S_2680_Chain2ByteFill_00000003 (IU32 eaOff, IU8 eaVal, IU32 count)
{
  ENTER_FUNC(2680);
  S_2681_Chain2ModeXByteFill_00000003(eaOff, eaVal, count);
}

void S_2681_Chain2ModeXByteFill_00000003 (IU32 eaOff, IU8 eaVal, IU32 count)
{
  C2MXBFLLF(2681,NOTRANS,NOTRANS,S_2679_Chain2ModeXByteWrite_00000003);
}

void S_2682_Chain2ByteMove_00000003_Fwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  ENTER_FUNC(2682);
  S_2683_Chain2ModeXFwdByteMove_00000003(eaOff, fromOff, count, srcInRAM);
}

void S_2683_Chain2ModeXFwdByteMove_00000003 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  C2MXBMOVFWD(2683, S_2679_Chain2ModeXByteWrite_00000003);
}

void S_2684_Chain2WordWrite_00000003 (IU32 eaOff, IU16 eaVal)
{
  ENTER_FUNC(2684);
  S_2685_Chain2ModeXWordWrite_00000003(eaOff, (IU8)eaVal);
}

void S_2685_Chain2ModeXWordWrite_00000003 (IU32 eaOff, IU16 eaVal)
{
  C2MXWWRTF(2685,NOTRANS,NOTRANS,S_2679_Chain2ModeXByteWrite_00000003);
}

void S_2686_Chain2WordFill_00000003 (IU32 eaOff, IU16 eaVal, IU32 count)
{
  ENTER_FUNC(2686);
  S_2687_Chain2ModeXWordFill_00000003(eaOff, (IU8)eaVal, count);
}

void S_2687_Chain2ModeXWordFill_00000003 (IU32 eaOff, IU16 eaVal, IU32 count)
{
  C2MXWFLLF(2687,NOTRANS,NOTRANS,S_2679_Chain2ModeXByteWrite_00000003);
}

void S_2688_Chain2WordMove_00000003_Fwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  ENTER_FUNC(2688);
  S_2689_Chain2ModeXFwdWordMove_00000003(eaOff, fromOff, count, srcInRAM);
}

void S_2689_Chain2ModeXFwdWordMove_00000003 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  C2MXWMOVFWD(2689, S_2679_Chain2ModeXByteWrite_00000003, S_2685_Chain2ModeXWordWrite_00000003);
}

void S_2690_Chain2DwordWrite_00000003 (IU32 eaOff, IU32 eaVal)
{
  ENTER_FUNC(2690);
  S_2685_Chain2ModeXWordWrite_00000003(eaOff, eaVal);
  S_2685_Chain2ModeXWordWrite_00000003(eaOff + 4, 0);
}

void S_2691_Chain2ModeXDwordWrite_00000003 (IU32 eaOff, IU32 eaVal)
{
  C2MXDWRTF(2691, S_2685_Chain2ModeXWordWrite_00000003);
}

void S_2692_Chain2DwordFill_00000003 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  ENTER_FUNC(2692);
  S_2693_Chain2ModeXDwordFill_00000003(eaOff, eaVal, count);
}

void S_2693_Chain2ModeXDwordFill_00000003 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  C2MXDFLLF(2693,NOTRANS,NOTRANS,S_2679_Chain2ModeXByteWrite_00000003);
}

void S_2694_Chain2DwordMove_00000003_Fwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  S_2695_Chain2ModeXFwdDwordMove_00000003(eaOff, fromOff, count, srcInRAM);
}

void S_2695_Chain2ModeXFwdDwordMove_00000003 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  C2MXDMOVFWD(2695, S_2689_Chain2ModeXFwdWordMove_00000003);
}

