#include "sevid000.h"

void S_2831_Chain4DwordWrite_00000000_00000008 (IU32 eaOff, IU32 eaVal)
{
  C4DWRTF(2831, S_2782_Chain4WordWrite_00000000_00000008);
}

void S_2832_Chain4DwordWrite_00000000_00000009 (IU32 eaOff, IU32 eaVal)
{
  C4DWRTF(2832, S_2783_Chain4WordWrite_00000000_00000009);
}

void S_2833_Chain4DwordWrite_00000000_0000000e (IU32 eaOff, IU32 eaVal)
{
  C4DWRTF(2833, S_2784_Chain4WordWrite_00000000_0000000e);
}

void S_2834_Chain4DwordWrite_00000000_0000000f (IU32 eaOff, IU32 eaVal)
{
  C4DWRTF(2834, S_2785_Chain4WordWrite_00000000_0000000f);
}

void S_2835_Chain4DwordWrite_00000000_00000010 (IU32 eaOff, IU32 eaVal)
{
  C4DWRTF(2835, S_2786_Chain4WordWrite_00000000_00000010);
}

void S_2836_Chain4DwordWrite_00000000_00000011 (IU32 eaOff, IU32 eaVal)
{
  C4DWRTF(2836, S_2787_Chain4WordWrite_00000000_00000011);
}

void S_2837_Chain4DwordWrite_00000000_00000016 (IU32 eaOff, IU32 eaVal)
{
  C4DWRTF(2837, S_2788_Chain4WordWrite_00000000_00000016);
}

void S_2838_Chain4DwordWrite_00000000_00000017 (IU32 eaOff, IU32 eaVal)
{
  C4DWRTF(2838, S_2789_Chain4WordWrite_00000000_00000017);
}

void S_2839_Chain4DwordWrite_00000000_00000018 (IU32 eaOff, IU32 eaVal)
{
  C4DWRTF(2839, S_2790_Chain4WordWrite_00000000_00000018);
}

void S_2840_Chain4DwordWrite_00000000_00000019 (IU32 eaOff, IU32 eaVal)
{
  C4DWRTF(2840, S_2791_Chain4WordWrite_00000000_00000019);
}

void S_2841_Chain4DwordWrite_00000000_0000001e (IU32 eaOff, IU32 eaVal)
{
  C4DWRTF(2841, S_2792_Chain4WordWrite_00000000_0000001e);
}

void S_2842_Chain4DwordWrite_00000000_0000001f (IU32 eaOff, IU32 eaVal)
{
  C4DWRTF(2842, S_2793_Chain4WordWrite_00000000_0000001f);
}

/*
 * Chain-4 dword-fills, write mode 0.  C4DFLL signature changed to
 * (func, mask, trans); mask/trans copied verbatim from the sibling
 * Chain4WordFill's C4WFLL/C4WFLL1 call.  The old shape corrupted output
 * for count > 1 (see the C4DFLL comment in sevid000.h and the GVFM
 * VESA-8bpp regression).
 */
void S_2843_Chain4DwordFill_00000000_00000008 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  C4DFLL(2843, UCB4MSK, NOTRANS);
}

void S_2844_Chain4DwordFill_00000000_00000009 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  C4DFLL(2844, UCB4MSK, NOTRANS);
}

void S_2845_Chain4DwordFill_00000000_0000000e (IU32 eaOff, IU32 eaVal, IU32 count)
{
  C4DFLL(2845, UCBMSK, NOTRANS);
}

void S_2846_Chain4DwordFill_00000000_0000000f (IU32 eaOff, IU32 eaVal, IU32 count)
{
  C4DFLL(2846, UCBMSK, NOTRANS);
}

void S_2847_Chain4DwordFill_00000000_00000010 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  C4DFLL(2847, UCB5MSKD, NOTRANS);
}

void S_2848_Chain4DwordFill_00000000_00000011 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  C4DFLL(2848, UCB5MSKD, NOTRANS);
}

void S_2849_Chain4DwordFill_00000000_00000016 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  C4DFLL(2849, UCB5MSK, NOTRANS);
}

void S_2850_Chain4DwordFill_00000000_00000017 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  C4DFLL(2850, UCB5MSK, NOTRANS);
}

void S_2851_Chain4DwordFill_00000000_00000018 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  C4DFLL(2851, UCB45MSK, NOTRANS);
}

void S_2852_Chain4DwordFill_00000000_00000019 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  C4DFLL(2852, UCB45MSK, NOTRANS);
}

void S_2853_Chain4DwordFill_00000000_0000001e (IU32 eaOff, IU32 eaVal, IU32 count)
{
  C4DFLL(2853, UCB15MSK, NOTRANS);
}

void S_2854_Chain4DwordFill_00000000_0000001f (IU32 eaOff, IU32 eaVal, IU32 count)
{
  C4DFLL(2854, UCB15MSK, NOTRANS);
}

