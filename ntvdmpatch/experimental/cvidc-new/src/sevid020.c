#include "sevid000.h"

void S_2696_Chain2ByteWrite_Copy (IU32 eaOff, IU8 eaVal)
{
  C2MXBWRTF(2696,NOTRANS,NOTRANS,C2CPWRTWPL);
}

void S_2697_Chain2ByteFill_Copy (IU32 eaOff, IU8 eaVal, IU32 count)
{
  IU32 inc;
  IU8 *dst;

  ENTER_FUNC(2697);
  CGA_MARK_STRING(count,sizeof(IU8));
  if (eaOff & 1)
  {
    dst = &GDP->VGAGlobals.VGA_wplane[2 * eaOff & (~3) + 1];
    inc = 3;
  }
  else
  {
    dst = &GDP->VGAGlobals.VGA_wplane[2 * eaOff & (~3)];
    inc = 1;
  }
  while(count--)
  {
    *dst = eaVal;
    dst += inc;
    inc ^= 2;
  }
}

void S_2698_Chain2ByteMove_Copy_Fwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  C2CPMOVFW(2698);
}

void S_2699_Chain2WordWrite_Copy (IU32 eaOff, IU16 eaVal)
{
  IU8 noteven;

  ENTER_FUNC(2699);
  noteven = eaOff & 1;
  GDP->VGAGlobals.dirty_total++;
  if ((eaOff & 0xDFFE) < GDP->VGAGlobals.dirty_low)
    GDP->VGAGlobals.dirty_low = eaOff & 0xDFFE;
  eaOff &= 0xFFFFFFFE;
  if (((eaOff + 1) & 0xDFFF) > GDP->VGAGlobals.dirty_high)
    GDP->VGAGlobals.dirty_high = ((eaOff + 1) & 0xDFFF);
  eaOff *= sizeof(IU16);
  GDP->VGAGlobals.VGA_wplane[eaOff + noteven] = (IU8)eaVal; 
  eaOff += 2*noteven + 1;
  GDP->VGAGlobals.VGA_wplane[eaOff + noteven] = (IU8)(eaVal>>8);
}

void S_2700_Chain2WordFill_Copy (IU32 eaOff, IU16 eaVal, IU32 count)
{
  ENTER_FUNC(2700);
  CGA_MARK_STRING(count, sizeof(IU16));

  if (eaOff & 1)
  {
    eaOff = (2 * eaOff & (~3)) | 1;
    GDP->VGAGlobals.VGA_wplane[eaOff] = (IU8)eaVal;
    eaVal = (IU16)(eaVal<<8) | (IU8)(eaVal>>8);
    eaOff += 3; 
  } else eaOff = 2 * eaOff & (~3);

  while (count--)
  {
    *((IU16*)&GDP->VGAGlobals.VGA_wplane[eaOff]) = eaVal;
    eaOff += 4;
  }
}

void S_2701_Chain2WordMove_Copy_Fwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCDMOVFW(2701, S_2698_Chain2ByteMove_Copy_Fwd);
}

void S_2702_Chain2DwordWrite_Copy (IU32 eaOff, IU32 eaVal)
{
  IU8 noteven;

  ENTER_FUNC(2702);
  noteven = eaOff & 1;
  GDP->VGAGlobals.dirty_total++;
  if ((eaOff & 0xDFFE) < GDP->VGAGlobals.dirty_low)
    GDP->VGAGlobals.dirty_low = eaOff & 0xDFFE;
  eaOff &= 0xFFFFFFFE;
  if (((eaOff + 3) & 0xDFFF) > GDP->VGAGlobals.dirty_high)
    GDP->VGAGlobals.dirty_high = ((eaOff + 3) & 0xDFFF);
  eaOff *= sizeof(IU16);
  GDP->VGAGlobals.VGA_wplane[eaOff + noteven] = (IU8)eaVal;
  if (noteven)
  {
    eaOff+=3;
    GDP->VGAGlobals.VGA_wplane[eaOff + noteven] = (IU8)(eaVal>>8);
    eaOff+=1;
  }
  else
  {
    eaOff+=1;
    GDP->VGAGlobals.VGA_wplane[eaOff + noteven] = (IU8)(eaVal>>8);
    eaOff+=3;
  }
  GDP->VGAGlobals.VGA_wplane[eaOff] = (IU8)(eaVal>>16);
  eaOff++;
  GDP->VGAGlobals.VGA_wplane[eaOff] = (IU8)(eaVal>>24);
}

void S_2703_Chain2DwordFill_Copy (IU32 eaOff, IU32 eaVal, IU32 count)
{
  ENTER_FUNC(2703);
  CGA_MARK_STRING(count, sizeof(IU32));

  if (eaOff & 1)
  {
    eaOff = (2 * eaOff & (~3)) | 1;
    GDP->VGAGlobals.VGA_wplane[eaOff] = (IU8)eaVal;
    eaVal = (IU16)(eaVal<<8) | (IU8)(eaVal>>8) | (eaVal>>24<<16);
    eaOff += 3;
  } else eaOff = 2 * eaOff & (~3);

  while (count--)
  {
    *((IU32*)&GDP->VGAGlobals.VGA_wplane[eaOff]) = eaVal;
    eaOff += 4;
  }
}

void S_2704_Chain2DwordMove_Copy_Fwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  ENTER_FUNC(2704);
  S_2698_Chain2ByteMove_Copy_Fwd(eaOff, fromOff, sizeof(IU32) * count, srcInRAM);
}

