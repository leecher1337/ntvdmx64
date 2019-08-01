#include "sevid000.h"

void S_2711_Chain2ByteMove_00000001_Bwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  ENTER_FUNC(2711);
  S_2712_Chain2ModeXBwdByteMove_00000001(eaOff, fromOff, count, srcInRAM);
}

void S_2712_Chain2ModeXBwdByteMove_00000001 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  C2MXBMOVBWD(2712, S_2643_Chain2ModeXByteWrite_00000001);
}

void S_2713_Chain2WordMove_00000001_Bwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  ENTER_FUNC(2713);
  S_2714_Chain2ModeXBwdWordMove_00000001(eaOff, fromOff, count, srcInRAM);
}

void S_2714_Chain2ModeXBwdWordMove_00000001 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  C2MXWMOVBWD(2714, S_2643_Chain2ModeXByteWrite_00000001, S_2649_Chain2ModeXWordWrite_00000001);
}

void S_2715_Chain2DwordMove_00000001_Bwd (IU32 eaOff, IU32 eaVal)
{
  S_2716_Chain2ModeXBwdDwordMove_00000001(eaOff, eaVal);
}

void S_2716_Chain2ModeXBwdDwordMove_00000001 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM) 
{
  C2MXDMOVBWD(2716, S_2714_Chain2ModeXBwdWordMove_00000001);
}
