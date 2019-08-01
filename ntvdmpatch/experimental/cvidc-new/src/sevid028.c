#include "sevid000.h"

void S_2782_Chain4WordWrite_00000000_00000008 (IU32 eaOff, IU16 eaVal)
{
  C4WWRTF1(2782, UCB4MSK, NOTRANS);
}

void S_2783_Chain4WordWrite_00000000_00000009 (IU32 eaOff, IU16 eaVal)
{
  C4WWRTF(2783, UCB4MSK, NOTRANS);
}

void S_2784_Chain4WordWrite_00000000_0000000e (IU32 eaOff, IU16 eaVal)
{
  C4WWRTF1(2784, UCBMSK, NOTRANS);
}

void S_2785_Chain4WordWrite_00000000_0000000f (IU32 eaOff, IU16 eaVal)
{
  C4WWRTF(2785, UCBMSK, NOTRANS);
}

void S_2786_Chain4WordWrite_00000000_00000010 (IU32 eaOff, IU16 eaVal)
{
  C4WWRTF1(2786, UCB5MSKD, NOTRANS);
}

void S_2787_Chain4WordWrite_00000000_00000011 (IU32 eaOff, IU16 eaVal)
{
  C4WWRTF(2787, UCB5MSKD, NOTRANS);
}

void S_2788_Chain4WordWrite_00000000_00000016 (IU32 eaOff, IU16 eaVal)
{
  C4WWRTF1(2788, UCB5MSK, NOTRANS);
}

void S_2789_Chain4WordWrite_00000000_00000017 (IU32 eaOff, IU16 eaVal)
{
  C4WWRTF(2789, UCB5MSK, NOTRANS);
}

void S_2790_Chain4WordWrite_00000000_00000018 (IU32 eaOff, IU16 eaVal)
{
  C4WWRTF1(2790, UCB45MSK, NOTRANS);
}

void S_2791_Chain4WordWrite_00000000_00000019 (IU32 eaOff, IU16 eaVal)
{
  C4WWRTF(2791, UCB45MSK, NOTRANS);
}

void S_2792_Chain4WordWrite_00000000_0000001e (IU32 eaOff, IU16 eaVal)
{
  C4WWRTF1(2792, UCB15MSK, NOTRANS);
}

void S_2793_Chain4WordWrite_00000000_0000001f (IU32 eaOff, IU16 eaVal)
{
  C4WWRTF(2793, UCB15MSK, NOTRANS);
}

void S_2794_Chain4WordFill_00000000_00000008 (IU32 eaOff, IU16 eaVal, IU32 count)
{
  C4WFLL(2794, UCB4MSK, NOTRANS);
}

void S_2795_Chain4WordFill_00000000_00000009 (IU32 eaOff, IU16 eaVal, IU32 count)
{
  C4WFLL1(2795, UCB4MSK, NOTRANS);
}

void S_2796_Chain44PlaneWordFill (IU32 eaOff, IU8 eaValLo, IU8 eaValHi, IU32 count)
{
  IU8 *data;
  ENTER_FUNC(2796);
  for (data=&GDP->VGAGlobals.VGA_wplane[eaOff]; count>0; count--)
  {
    *data = eaValLo;
    data++;
    *data = eaValHi;
    data++;
  }
}

void S_2797_Chain4WordFill_00000000_0000000e (IU32 eaOff, IU16 eaVal, IU32 count)
{
  C4WFLL(2797, UCBMSK, NOTRANS);
}

void S_2798_Chain4WordFill_00000000_0000000f (IU32 eaOff, IU16 eaVal, IU32 count)
{
  C4WFLL1(2798, UCBMSK, NOTRANS);
}

void S_2799_Chain4WordFill_00000000_00000010 (IU32 eaOff, IU16 eaVal, IU32 count)
{
  C4WFLL(2799, UCB5MSKD, NOTRANS);
}

void S_2800_Chain4WordFill_00000000_00000011 (IU32 eaOff, IU16 eaVal, IU32 count)
{
  C4WFLL1(2800, UCB5MSKD, NOTRANS);
}

void S_2801_Chain4WordFill_00000000_00000016 (IU32 eaOff, IU16 eaVal, IU32 count)
{
  C4WFLL(2801, UCB5MSK, NOTRANS);
}

void S_2802_Chain4WordFill_00000000_00000017 (IU32 eaOff, IU16 eaVal, IU32 count)
{
  C4WFLL1(2802, UCB5MSK, NOTRANS);
}

void S_2803_Chain4WordFill_00000000_00000018 (IU32 eaOff, IU16 eaVal, IU32 count)
{
  C4WFLL(2802, UCB45MSK, NOTRANS);
}

void S_2804_Chain4WordFill_00000000_00000019 (IU32 eaOff, IU16 eaVal, IU32 count)
{
  C4WFLL1(2804, UCB45MSK, NOTRANS);
}

void S_2805_Chain4WordFill_00000000_0000001e (IU32 eaOff, IU16 eaVal, IU32 count)
{
  C4WFLL(2805, UCB15MSK, NOTRANS);
}

void S_2806_Chain4WordFill_00000000_0000001f (IU32 eaOff, IU16 eaVal, IU32 count)
{
  C4WFLL1(2806, UCB15MSK, NOTRANS);
}

