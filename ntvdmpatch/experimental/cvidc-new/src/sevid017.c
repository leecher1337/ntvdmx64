#include "sevid000.h"

void S_2642_Chain2ByteWrite_00000001 (IU32 eaOff, IU8 eaVal)
{
  ENTER_FUNC(2642);
  S_2643_Chain2ModeXByteWrite_00000001(eaOff, eaVal);
}

void S_2643_Chain2ModeXByteWrite_00000001 (IU32 eaOff, IU8 eaVal)
{
  C2MXBWRTF(2643,NOMSK,NOTRANS,C2MXWRTWPL);
}

void S_2644_Chain2ByteFill_00000001 (IU32 eaOff, IU8 eaVal, IU32 count)
{
  ENTER_FUNC(2644);
  S_2645_Chain2ModeXByteFill_00000001(eaOff, eaVal, count);
}

void S_2645_Chain2ModeXByteFill_00000001 (IU32 eaOff, IU8 eaVal, IU32 count)
{
  C2MXBFLLF(2645,NOTRANS,NOTRANS,S_2643_Chain2ModeXByteWrite_00000001);
}

void S_2646_Chain2ByteMove_00000001_Fwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  ENTER_FUNC(2646);
  S_2647_Chain2ModeXFwdByteMove_00000001(eaOff, fromOff, count, srcInRAM);
}

void S_2647_Chain2ModeXFwdByteMove_00000001 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  C2MXBMOVFWD(2647, S_2643_Chain2ModeXByteWrite_00000001);
}

void S_2648_Chain2WordWrite_00000001 (IU32 eaOff, IU16 eaVal)
{
  ENTER_FUNC(2648);
  S_2649_Chain2ModeXWordWrite_00000001(eaOff, (IU8)eaVal);
}

void S_2649_Chain2ModeXWordWrite_00000001 (IU32 eaOff, IU16 eaVal)
{
  C2MXWWRTF(2649,NOTRANS,NOTRANS,S_2643_Chain2ModeXByteWrite_00000001);
}

void S_2650_Chain2WordFill_00000001 (IU32 eaOff, IU16 eaVal, IU32 count)
{
  ENTER_FUNC(2650);
  S_2651_Chain2ModeXWordFill_00000001(eaOff, (IU8)eaVal, count);
}

void S_2651_Chain2ModeXWordFill_00000001 (IU32 eaOff, IU16 eaVal, IU32 count)
{
  C2MXWFLLF(2651,NOTRANS,NOTRANS,S_2643_Chain2ModeXByteWrite_00000001);
}

void S_2652_Chain2WordMove_00000001_Fwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  ENTER_FUNC(2652);
  S_2653_Chain2ModeXFwdWordMove_00000001(eaOff, fromOff, count, srcInRAM);
}

void S_2653_Chain2ModeXFwdWordMove_00000001 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  C2MXWMOVFWD(2653, S_2643_Chain2ModeXByteWrite_00000001, S_2649_Chain2ModeXWordWrite_00000001);
}

void S_2654_Chain2DwordWrite_00000001 (IU32 eaOff, IU32 eaVal)
{
  ENTER_FUNC(2654);
  S_2649_Chain2ModeXWordWrite_00000001(eaOff, eaVal);
  S_2649_Chain2ModeXWordWrite_00000001(eaOff + 4, 0);
}

void S_2655_Chain2ModeXDwordWrite_00000001 (IU32 eaOff, IU32 eaVal)
{
  C2MXDWRTF(2655, S_2649_Chain2ModeXWordWrite_00000001);
}

void S_2656_Chain2DwordFill_00000001 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  ENTER_FUNC(2656);
  S_2657_Chain2ModeXDwordFill_00000001(eaOff, eaVal, count);
}

void S_2657_Chain2ModeXDwordFill_00000001 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  C2MXDFLLF(2657,NOTRANS,NOTRANS,S_2643_Chain2ModeXByteWrite_00000001);
}

void S_2658_Chain2DwordMove_00000001_Fwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  S_2659_Chain2ModeXFwdDwordMove_00000001(eaOff, fromOff, count, srcInRAM);
}

void S_2659_Chain2ModeXFwdDwordMove_00000001 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  C2MXDMOVFWD(2658, S_2653_Chain2ModeXFwdWordMove_00000001);
}

