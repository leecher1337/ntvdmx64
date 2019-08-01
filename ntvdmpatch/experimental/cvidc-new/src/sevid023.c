#include "sevid000.h"

void S_2717_Chain2ByteMove_00000002_Bwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  ENTER_FUNC(2717);
  S_2718_Chain2ModeXBwdByteMove_00000002(eaOff, fromOff, count, srcInRAM);
}

void S_2718_Chain2ModeXBwdByteMove_00000002 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  C2MXBMOVBWD(2718, S_2661_Chain2ModeXByteWrite_00000002);
}

void S_2719_Chain2WordMove_00000002_Bwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  ENTER_FUNC(2713);
  S_2720_Chain2ModeXBwdWordMove_00000002(eaOff, fromOff, count, srcInRAM);
}

void S_2720_Chain2ModeXBwdWordMove_00000002 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  C2MXWMOVBWD(2720, S_2661_Chain2ModeXByteWrite_00000002, S_2667_Chain2ModeXWordWrite_00000002);
}

void S_2721_Chain2DwordMove_00000002_Bwd (IU32 eaOff, IU32 eaVal)
{
  S_2722_Chain2ModeXBwdDwordMove_00000002(eaOff, eaVal);
}

void S_2722_Chain2ModeXBwdDwordMove_00000002 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM) 
{
  C2MXDMOVBWD(2722, S_2720_Chain2ModeXBwdWordMove_00000002);
}
