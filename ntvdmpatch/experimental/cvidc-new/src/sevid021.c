#include "sevid000.h"

void S_2705_Chain2ByteMove_00000000_Bwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  ENTER_FUNC(2705);
  S_2706_Chain2ModeXBwdByteMove_00000000(eaOff, fromOff, count, srcInRAM);
}

void S_2706_Chain2ModeXBwdByteMove_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  C2MXBMOVBWD(2706, S_2625_Chain2ModeXByteWrite_00000000);
}

void S_2707_Chain2WordMove_00000000_Bwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  ENTER_FUNC(2707);
  S_2708_Chain2ModeXBwdWordMove_00000000(eaOff, fromOff, count, srcInRAM);
}

void S_2708_Chain2ModeXBwdWordMove_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  C2MXWMOVBWD(2708, S_2625_Chain2ModeXByteWrite_00000000, S_2631_Chain2ModeXWordWrite_00000000);
}

void S_2709_Chain2DwordMove_00000000_Bwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM) 
{
  S_2710_Chain2ModeXBwdDwordMove_00000000(eaOff, fromOff,count,srcInRAM);
}

void S_2710_Chain2ModeXBwdDwordMove_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM) 
{
  C2MXDMOVBWD(2710, S_2708_Chain2ModeXBwdWordMove_00000000);
}
