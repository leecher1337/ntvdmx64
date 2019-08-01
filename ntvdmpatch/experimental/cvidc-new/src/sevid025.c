#include "sevid000.h"

void S_2729_Chain2ByteMove_Copy_Bwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  C2CPMOVBW(2729);
}

void S_2730_Chain2WordMove_Copy_Bwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  ENTER_FUNC(2730);
  S_2729_Chain2ByteMove_Copy_Bwd(eaOff + 1, srcInRAM?fromOff+RAM_DIR:fromOff+1, 2 * count, srcInRAM);
}

void S_2731_Chain2DwordMove_Copy_Bwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  ENTER_FUNC(2731);
  S_2729_Chain2ByteMove_Copy_Bwd(eaOff, fromOff, sizeof(IU32) * count, srcInRAM);
}

