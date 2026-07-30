#include "sevid000.h"

/***********************************
 * Unchained dword functions
 ***********************************/

void S_2360_UnchainedDwordWrite_00000000_00000008_00000000 (IU32 eaOff, IU32 eaVal)
{
  UCDWRTF(2360,S_2305_UnchainedWordWrite_00000000_00000008_00000000);
}

void S_2361_UnchainedDwordWrite_00000000_00000009_00000000 (IU32 eaOff, IU32 eaVal)
{
  UCDWRTF(2361,S_2306_UnchainedWordWrite_00000000_00000009_00000000);
}

void S_2362_UnchainedDwordWrite_00000000_0000000e_00000000 (IU32 eaOff, IU32 eaVal)
{
  UCDWRTF(2362,S_2307_UnchainedWordWrite_00000000_0000000e_00000000);
}

void S_2363_UnchainedDwordWrite_00000000_0000000f_00000000 (IU32 eaOff, IU32 eaVal)
{
  UCDWRTF(2363,S_2308_UnchainedWordWrite_00000000_0000000f_00000000);
}

void S_2364_UnchainedDwordWrite_00000000_00000010_00000000 (IU32 eaOff, IU32 eaVal)
{
  UCDWRTF(2364,S_2309_UnchainedWordWrite_00000000_00000010_00000000);
}

void S_2365_UnchainedDwordWrite_00000000_00000011_00000000 (IU32 eaOff, IU32 eaVal)
{
  UCDWRTF(2365,S_2310_UnchainedWordWrite_00000000_00000011_00000000);
}

void S_2366_UnchainedDwordWrite_00000000_00000016_00000000 (IU32 eaOff, IU32 eaVal)
{
  UCDWRTF(2366,S_2311_UnchainedWordWrite_00000000_00000016_00000000);
}

void S_2367_UnchainedDwordWrite_00000000_00000017_00000000 (IU32 eaOff, IU32 eaVal)
{
  UCDWRTF(2367,S_2312_UnchainedWordWrite_00000000_00000017_00000000);
}

void S_2368_UnchainedDwordWrite_00000000_00000018_00000000 (IU32 eaOff, IU32 eaVal)
{
  UCDWRTF(2368,S_2313_UnchainedWordWrite_00000000_00000018_00000000);
}

void S_2369_UnchainedDwordWrite_00000000_00000019_00000000 (IU32 eaOff, IU32 eaVal)
{
  UCDWRTF(2369,S_2314_UnchainedWordWrite_00000000_00000019_00000000);
}

void S_2370_UnchainedDwordWrite_00000000_0000001e_00000000 (IU32 eaOff, IU32 eaVal)
{
  UCDWRTF(2370,S_2315_UnchainedWordWrite_00000000_0000001e_00000000);
}

void S_2371_UnchainedDwordWrite_00000000_0000001f_00000000 (IU32 eaOff, IU32 eaVal)
{
  UCDWRTF(2371,S_2316_UnchainedWordWrite_00000000_0000001f_00000000);
}


/*
 * Unchained dword-fills, write mode 0.  New UCDFLLF signature is
 * (func, mask, trans, dispatch), matching UCBFLLF/UCWFLLF.  mask/trans
 * are copied verbatim from the sibling S_23XX_UnchainedWordFill (which
 * uses UCWFLLF); dispatch picks between the inline 4-plane bulk path
 * (UCDFLL_4P, for odd-numbered states with map_mask=0xF) and the
 * per-plane path via S_3069_Unchained1PlaneDwordFill (UCDFLL_PLN, for
 * even-numbered states with partial planes).  Replaces the previous
 * broken shape that called word-fill twice with an incorrect +8 offset
 * and mismatched 2-byte stride.
 */
void S_2372_UnchainedDwordFill_00000000_00000008_00000000 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  UCDFLLF(2372, UCB4MSK, UCB3DATA, UCDFLL_PLN);
}

void S_2373_UnchainedDwordFill_00000000_00000009_00000000 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  UCDFLLF(2373, UCB4MSK, UCB3DATA, UCDFLL_4P);
}

void S_2374_UnchainedDwordFill_00000000_0000000e_00000000 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  UCDFLLF(2374, UCBMSK, UCB3DATA, UCDFLL_PLN);
}

void S_2375_UnchainedDwordFill_00000000_0000000f_00000000 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  UCDFLLF(2375, UCBMSK, UCB3DATA, UCDFLL_4P);
}

void S_2376_UnchainedDwordFill_00000000_00000010_00000000 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  UCDFLLF(2376, UCB5MSKD, UCB3DATA, UCDFLL_PLN);
}

void S_2377_UnchainedDwordFill_00000000_00000011_00000000 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  UCDFLLF(2377, UCB5MSKD, UCB3DATA, UCDFLL_4P);
}

void S_2378_UnchainedDwordFill_00000000_00000016_00000000 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  UCDFLLF(2378, UCB5MSK, UCB3DATA, UCDFLL_PLN);
}

void S_2379_UnchainedDwordFill_00000000_00000017_00000000 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  UCDFLLF(2379, UCB5MSK, UCB3DATA, UCDFLL_4P);
}

void S_2380_UnchainedDwordFill_00000000_00000018_00000000 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  UCDFLLF(2380, UCB45MSK, UCB3DATA, UCDFLL_PLN);
}

void S_2381_UnchainedDwordFill_00000000_00000019_00000000 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  UCDFLLF(2381, UCB45MSK, UCB3DATA, UCDFLL_4P);
}

void S_2382_UnchainedDwordFill_00000000_0000001e_00000000 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  UCDFLLF(2382, UCB15MSK, UCB3DATA, UCDFLL_PLN);
}

void S_2383_UnchainedDwordFill_00000000_0000001f_00000000 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  UCDFLLF(2383, UCB15MSK, UCB3DATA, UCDFLL_4P);
}

void S_2384_UnchainedDwordMove_00000000_00000008_00000000_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCDMOVFW(2384, S_2330_UnchainedWordMove_00000000_00000008_00000000_00000000);
}

void S_2385_UnchainedDwordMove_00000000_00000009_00000000_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCDMOVFW(2385, S_2333_UnchainedWordMove_00000000_00000009_00000000_00000000);
}

void S_2386_UnchainedDwordMove_00000000_0000000e_00000000_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCDMOVFW(2386, S_2335_UnchainedWordMove_00000000_0000000e_00000000_00000000);
}

void S_2387_UnchainedDwordMove_00000000_0000000f_00000000_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCDMOVFW(2387, S_2338_UnchainedWordMove_00000000_0000000f_00000000_00000000);
}

void S_2388_UnchainedDwordMove_00000000_00000010_00000000_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCDMOVFW(2388, S_2340_UnchainedWordMove_00000000_00000010_00000000_00000000);
}

void S_2389_UnchainedDwordMove_00000000_00000011_00000000_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCDMOVFW(2389, S_2343_UnchainedWordMove_00000000_00000011_00000000_00000000);
}

void S_2390_UnchainedDwordMove_00000000_00000016_00000000_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCDMOVFW(2390, S_2345_UnchainedWordMove_00000000_00000016_00000000_00000000);
}

void S_2391_UnchainedDwordMove_00000000_00000017_00000000_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCDMOVFW(2391, S_2348_UnchainedWordMove_00000000_00000017_00000000_00000000);
}

void S_2392_UnchainedDwordMove_00000000_00000018_00000000_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCDMOVFW(2392, S_2350_UnchainedWordMove_00000000_00000018_00000000_00000000);
}

void S_2393_UnchainedDwordMove_00000000_00000019_00000000_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCDMOVFW(2393, S_2353_UnchainedWordMove_00000000_00000019_00000000_00000000);
}

void S_2394_UnchainedDwordMove_00000000_0000001e_00000000_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCDMOVFW(2394, S_2355_UnchainedWordMove_00000000_0000001e_00000000_00000000);
}

void S_2395_UnchainedDwordMove_00000000_0000001f_00000000_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCDMOVFW(2395, S_2358_UnchainedWordMove_00000000_0000001f_00000000_00000000);
}

