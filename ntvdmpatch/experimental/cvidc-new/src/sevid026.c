#include "sevid000.h"

void S_2732_Chain4ByteWrite_00000000_00000008 (IU32 eaOff, IU8 eaVal)
{
  C4BWRTF1(2732, UCB4MSK, NOTRANS);
}

void S_2733_Chain4ByteWrite_00000000_00000009 (IU32 eaOff, IU8 eaVal)
{
  C4BWRTF(2733, UCB4MSK, NOTRANS);
}

void S_2734_Chain4ByteWrite_00000000_0000000e (IU32 eaOff, IU8 eaVal)
{
  C4BWRTF1(2734, UCBMSK, NOTRANS);
}

void S_2735_Chain4ByteWrite_00000000_0000000f (IU32 eaOff, IU8 eaVal)
{
  C4BWRTF(2735, UCBMSK, NOTRANS);
}

void S_2736_Chain4ByteWrite_00000000_00000010 (IU32 eaOff, IU8 eaVal)
{
  C4BWRTF1(2736, UCB5MSKD, NOTRANS);
}

void S_2737_Chain4ByteWrite_00000000_00000011 (IU32 eaOff, IU8 eaVal)
{
  C4BWRTF(2737, UCB5MSKD, NOTRANS);
}

void S_2738_Chain4ByteWrite_00000000_00000016 (IU32 eaOff, IU8 eaVal)
{
  C4BWRTF1(2738, UCB5MSK, NOTRANS, NOTRANS);
}

void S_2739_Chain4ByteWrite_00000000_00000017 (IU32 eaOff, IU8 eaVal)
{
  C4BWRTF(2739, UCB5MSK, NOTRANS);
}

void S_2740_Chain4ByteWrite_00000000_00000018 (IU32 eaOff, IU8 eaVal)
{
  C4BWRTF1(2740, UCB45MSK, NOTRANS, NOTRANS);
}

void S_2741_Chain4ByteWrite_00000000_00000019 (IU32 eaOff, IU8 eaVal)
{
  C4BWRTF(2741, UCB45MSK, NOTRANS);
}

void S_2742_Chain4ByteWrite_00000000_0000001e (IU32 eaOff, IU8 eaVal)
{
  C4BWRTF1(2742, UCB15MSK, NOTRANS);
}

void S_2743_Chain4ByteWrite_00000000_0000001f (IU32 eaOff, IU8 eaVal)
{
  C4BWRTF(2743, UCB15MSK, NOTRANS);
}

void S_2744_Chain4ByteFill_00000000_00000008 (IU32 eaOff, IU8 eaVal, IU32 count)
{
  C4BFLL(2744, UCB4MSK, NOTRANS);
}

void S_2745_Chain41PlaneByteFill (IU32 eaOff, IU8 eaVal, IU32 count)
{
  IU8 *data;
  ENTER_FUNC(2745);
  for (data=&GDP->VGAGlobals.VGA_wplane[eaOff]; count>0; count--, data+=4)
    *data = eaVal;
}

void S_2746_Chain4ByteFill_00000000_00000009 (IU32 eaOff, IU8 eaVal, IU32 count)
{
  C4BFLL1(2746, UCB4MSK, NOTRANS);
}

void S_2747_Chain44PlaneByteFill (IU32 eaOff, IU8 eaVal, IU32 count)
{
  ENTER_FUNC(2747);
  memset(&GDP->VGAGlobals.VGA_wplane[eaOff], eaVal, count);
}

void S_2748_Chain4ByteFill_00000000_0000000e (IU32 eaOff, IU8 eaVal, IU32 count)
{
  C4BFLL(2748, UCBMSK, NOTRANS);
}

void S_2749_Chain4ByteFill_00000000_0000000f (IU32 eaOff, IU8 eaVal, IU32 count)
{
  C4BFLL1(2749, UCBMSK, NOTRANS);
}

void S_2750_Chain4ByteFill_00000000_00000010 (IU32 eaOff, IU8 eaVal, IU32 count)
{
  C4BFLL(2750, UCB5MSKD, NOTRANS);
}

void S_2751_Chain4ByteFill_00000000_00000011 (IU32 eaOff, IU8 eaVal, IU32 count)
{
  C4BFLL1(2751, UCB5MSKD, NOTRANS);
}

void S_2752_Chain4ByteFill_00000000_00000016 (IU32 eaOff, IU8 eaVal, IU32 count)
{
  C4BFLL(2752, UCB5MSK, NOTRANS);
}

void S_2753_Chain4ByteFill_00000000_00000017 (IU32 eaOff, IU8 eaVal, IU32 count)
{
  C4BFLL1(2753, UCB5MSK, NOTRANS);
}

void S_2754_Chain4ByteFill_00000000_00000018 (IU32 eaOff, IU8 eaVal, IU32 count)
{
  C4BFLL(2754, UCB45MSK, NOTRANS);
}

void S_2755_Chain4ByteFill_00000000_00000019 (IU32 eaOff, IU8 eaVal, IU32 count)
{
  C4BFLL1(2755, UCB45MSK, NOTRANS);
}

void S_2756_Chain4ByteFill_00000000_0000001e (IU32 eaOff, IU8 eaVal, IU32 count)
{
  C4BFLL(2756, UCB15MSK, NOTRANS);
}

void S_2757_Chain4ByteFill_00000000_0000001f (IU32 eaOff, IU8 eaVal, IU32 count)
{
  C4BFLL1(2757, UCB15MSK, NOTRANS);
}

