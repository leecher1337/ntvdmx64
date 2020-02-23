#include "sevid000.h"

void S_2723_Chain2ByteMove_00000003_Bwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  ENTER_FUNC(2723);
  S_2724_Chain2ModeXBwdByteMove_00000003(eaOff, fromOff, count, srcInRAM);
}

void S_2724_Chain2ModeXBwdByteMove_00000003 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  C2MXBMOVBWD(2724, S_2679_Chain2ModeXByteWrite_00000003);
}

void S_2725_Chain2WordMove_00000003_Bwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  ENTER_FUNC(2725);
  S_2726_Chain2ModeXBwdWordMove_00000003(eaOff, fromOff, count, srcInRAM);
}

void S_2726_Chain2ModeXBwdWordMove_00000003 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  C2MXWMOVBWD(2726, S_2679_Chain2ModeXByteWrite_00000003, S_2685_Chain2ModeXWordWrite_00000003);
}

void S_2727_Chain2DwordMove_00000003_Bwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM) 
{
  S_2728_Chain2ModeXBwdDwordMove_00000003(eaOff, fromOff, count, srcInRAM);
}

void S_2728_Chain2ModeXBwdDwordMove_00000003 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM) 
{
  C2MXDMOVBWD(2728, S_2726_Chain2ModeXBwdWordMove_00000003);
}
