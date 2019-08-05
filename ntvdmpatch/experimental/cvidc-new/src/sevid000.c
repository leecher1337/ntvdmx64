#include "sevid000.h"

#ifdef DEBUG_EVID
void sevid_enter_func(int nr)
{
  GDP->HookNumber = 82;
  *GDP->SubrRingPtr = nr;
  ++GDP->SubrRingPtr;
  if ( GDP->SubrRingPtr > GDP->SubrRingHighIncl )
    GDP->SubrRingPtr = GDP->SubrRingLowIncl;
  GDP->HookNumber = 83;
}
#endif


/*
./clang -Wl,pie  -O4 -c -m32 -o test.o test.c
*/


/******************************************************
 * Read Mode 0   Chain 2                              *
 ******************************************************/

GLOBAL IU32 S_2087_RdMode0Chain2ByteRead (IU32 eaOff)
{
  IU32 offs;

  ENTER_FUNC(2087);
  offs = (eaOff << 1) & (~3);
  SET_LATCHES(offs);
  return GDP->VGAGlobals.VGA_rplane[offs + (eaOff & 1)];
}
	

GLOBAL IU32 S_2088_RdMode0Chain2WordRead (IU32 eaOff)
{
  IU32 offs;
  IU16 ret;

  ENTER_FUNC(2088);
  offs = (eaOff << 1) & (~3);
  SET_LATCHES(((eaOff << 1) + 2) & (~3));
  if (eaOff&1)
    return (((IU16)GDP->VGAGlobals.VGA_rplane[offs+4])<<8) | GDP->VGAGlobals.VGA_rplane[offs+1];
  return *(IU16*)&GDP->VGAGlobals.VGA_rplane[offs];
}

GLOBAL IU32 S_2089_RdMode0Chain2DwordRead(IU32 eaOff)
{
  IU32 ret;

  ENTER_FUNC(2089);
  ret = S_2088_RdMode0Chain2WordRead (eaOff);
  return ret | ((IU32)S_2088_RdMode0Chain2WordRead (eaOff + 4)<<16);
}


GLOBAL void S_2090_RdMode0Chain2StringReadFwd(IU8 * dest, IU32 eaOff, IU32 count, IUH destInRam)
{
  IU32 offs;
  IU8 bit;

  ENTER_FUNC(2090);
  bit  = (eaOff << 1) & 2 | 1;
  offs = GDP->VGAGlobals.read_mapped_plane + ((eaOff << 1) & (~3));
#ifdef BACK_M
  if (destInRam)
  {
    while(count--)
    {
      *dest = GDP->VGAGlobals.VGA_rplane[offs];
      offs += bit;
      bit ^= 2;
      dest--;	/* Backwards */
    }
  }
  else
#endif
  {
    while(count--)
    {
      *dest = GDP->VGAGlobals.VGA_rplane[offs];
      offs += bit;
      bit ^= 2;
      dest++;	/* Forwards */
    }
  }
  SET_LATCHES((offs-1) & (~3));
}

GLOBAL void S_2119_RdMode0Chain2StringReadBwd(IU8 * dest, IU32 eaOff, IU32 count, IUH destInRam)
{
  IU32 offs;
  IU8 bit;

  ENTER_FUNC(2119);
  bit  = (eaOff << 1) & 2 ^ 3;
  offs = GDP->VGAGlobals.read_mapped_plane + eaOff;
#ifdef BACK_M
  if (destInRam)
  {
    while(count--)
    {
      *dest = GDP->VGAGlobals.VGA_rplane[offs];
      offs -= bit;
      bit ^= 2;
      dest++;	/* Forwards */
    }
  }
  else
#endif
  {
    while(count--)
    {
      *dest = GDP->VGAGlobals.VGA_rplane[offs];
      offs -= bit;
      bit ^= 2;
      dest--;	/* Backwards */
    }
  }
  SET_LATCHES((offs+3) & (~3));
}


/******************************************************
 * Read Mode 0   Chain 4                              *
 ******************************************************/

GLOBAL IU32 S_2091_RdMode0Chain4ByteRead (IU32 eaOff)
{
  ENTER_FUNC(2091);
  SET_LATCHES(eaOff & (~3));
  return GDP->VGAGlobals.VGA_rplane[eaOff];
}

GLOBAL IU32 S_2092_RdMode0Chain4WordRead (IU32 eaOff)
{
  ENTER_FUNC(2092);
  SET_LATCHES((eaOff + 1) & (~3));
  return *(IU16*)&GDP->VGAGlobals.VGA_rplane[eaOff];
}

GLOBAL IU32 S_2093_RdMode0Chain4DwordRead(IU32 eaOff)
{
  ENTER_FUNC(2093);
  SET_LATCHES((eaOff + 3) & (~3));
  return *(IU32*)&GDP->VGAGlobals.VGA_rplane[eaOff];
}

GLOBAL void S_2094_RdMode0Chain4StringReadFwd(IU8 * dest, IU32 eaOff, IU32 count, IUH destInRam)
{
  IU32 offs;

  ENTER_FUNC(2094);
  offs = GDP->VGAGlobals.read_mapped_plane + eaOff;
#ifdef BACK_M
  if (destInRam)
  {
    while(count--)
    {
      *dest = GDP->VGAGlobals.VGA_rplane[offs++];
      dest--;	/* Backwards */
    }
  }
  else
#endif
  {
    memcpy (dest, &GDP->VGAGlobals.VGA_rplane[offs], count);
    dest += offs;
  }
  SET_LATCHES((offs-1) & (~3));
}

GLOBAL void S_2120_RdMode0Chain4StringReadBwd(IU8 * dest, IU32 eaOff, IU32 count, IUH destInRam)
{
  IU32 offs;

  ENTER_FUNC(2120);
  offs = GDP->VGAGlobals.read_mapped_plane + eaOff;
#ifdef BACK_M
  if (destInRam)
  {
    while(count--)
    {
      *dest = GDP->VGAGlobals.VGA_rplane[offs--];
      dest++;	/* Forwards */
    }
  }
  else
#endif
  {
    while(count--)
    {
      *dest = GDP->VGAGlobals.VGA_rplane[offs--];
      dest--;	/* Backwards */
    }

  }
  SET_LATCHES((offs+1) & (~3));
}

/******************************************************
 * Read Mode 0   Unchained                            *
 ******************************************************/
GLOBAL IU32 S_2095_RdMode0UnchainedByteRead (IU32 eaOff)
{
  ENTER_FUNC(2095);
  SET_LATCHES(eaOff * 4);

  return GDP->VGAGlobals.read_shift_count<32?(IU8)(GDP->VGAGlobals.latches >> GDP->VGAGlobals.read_shift_count):0;
}

GLOBAL IU32 S_2096_RdMode0UnchainedWordRead (IU32 eaOff)
{
  IU16 ret;

  ENTER_FUNC(2096);
  ret = (IU8)(GDP->VGAGlobals.read_shift_count<32?(IU8)(*(IU32 *)&GDP->VGAGlobals.VGA_rplane[eaOff * 4] >> GDP->VGAGlobals.read_shift_count) : 4);
  eaOff++;
  SET_LATCHES(eaOff * 4);
  if (GDP->VGAGlobals.read_shift_count < 32) ret |= (Gdp->VGAGlobals.latches >> GDP->VGAGlobals.read_shift_count)<<8;
  return ret;
}

GLOBAL IU32 S_2097_RdMode0UnchainedDwordRead(IU32 eaOff)
{
  IU32 ret;

  ENTER_FUNC(2097);
  ret = S_2096_RdMode0UnchainedWordRead (eaOff);
  return  ret | ((IU32)S_2096_RdMode0UnchainedWordRead (eaOff + 8)<<16);
}

GLOBAL void S_2098_RdMode0UnchainedStringReadFwd(IU8 * dest, IU32 eaOff, IU32 count, IUH destInRam)
{
  IU32 offs;

  ENTER_FUNC(2098);
  offs = GDP->VGAGlobals.read_mapped_plane + 4 * eaOff;
#ifdef BACK_M
  if (destInRam)
  {
    while(count--)
    {
      *dest = GDP->VGAGlobals.VGA_rplane[offs];
      offs += 4;
      dest--;	/* Backwards */
    }
  }
  else
#endif
  {
    while(count--)
    {
      *dest = GDP->VGAGlobals.VGA_rplane[offs];
      offs += 4;
      dest++;	/* Forward */
    }
  }
  SET_LATCHES(offs-4-GDP->VGAGlobals.read_mapped_plane);
}

GLOBAL void S_2121_RdMode0UnchainedStringReadBwd(IU8 * dest, IU32 eaOff, IU32 count, IUH destInRam)
{
  IU32 offs;

  ENTER_FUNC(2121);
  offs = GDP->VGAGlobals.read_mapped_plane + 4 * eaOff;
#ifdef BACK_M
  if (destInRam)
  {
    while(count--)
    {
      *dest = GDP->VGAGlobals.VGA_rplane[offs];
      offs -= 4;
      dest++;
    }
  }
  else
#endif
  {
    while(count--)
    {
      *dest = GDP->VGAGlobals.VGA_rplane[offs];
      offs -= 4;
      dest--;
    }
  }
  SET_LATCHES(offs+4-GDP->VGAGlobals.read_mapped_plane);
}



/******************************************************
 * Read Mode 1   Chain 2                              *
 ******************************************************/

GLOBAL IU32 S_2099_RdMode1Chain2ByteRead (IU32 eaOff)
{
  IU32 offs;

  ENTER_FUNC(2099);
  offs = (eaOff << 1) & (~3);
  SET_LATCHES(offs);
  if (eaOff & 1) offs++;
  return M1CH2(offs);
}

GLOBAL IU32 S_2100_RdMode1Chain2WordRead (IU32 eaOff)
{
  IU16 ret;
  IU32 offs;

  ENTER_FUNC(2100);
  offs = (eaOff << 1) & (~3);
  SET_LATCHES(((eaOff << 1) + 2) & (~3));
  if (eaOff & 1)
    return ((IU16)M1CH2(offs+4)<<8) | M1CH2(offs+1);
  else
    return ((IU16)M1CH2(offs+1)<<8) | M1CH2(offs);
}

GLOBAL IU32 S_2101_RdMode1Chain2DwordRead(IU32 eaOff)
{
  IU32 ret;

  ENTER_FUNC(2101);
  ret = S_2100_RdMode1Chain2WordRead (eaOff);
  return ret | ((IU32)S_2100_RdMode1Chain2WordRead (eaOff + 4)<<16);
}


GLOBAL void S_2102_RdMode1Chain2StringReadFwd(IU8 * dest, IU32 eaOff, IU32 count, IUH destInRam)
{
  IU32 offs;
  IU8 bit;

  ENTER_FUNC(2102);
  bit  = (eaOff << 1) & 2 | 1;
  offs = GDP->VGAGlobals.read_mapped_plane + ((eaOff << 1) & (~3));
#ifdef BACK_M
  if (destInRam)
  {
    while(count--)
    {
      *dest = M1CH2(offs);
      offs += bit;
      bit ^= 2;
      dest--;	/* Backwards */
    }
  }
  else
#endif
  {
    while(count--)
    {
      *dest = M1CH2(offs);
      offs += bit;
      bit ^= 2;
      dest++;	/* Forwards */
    }
  }
  SET_LATCHES((offs-1) & (~3));
}

GLOBAL void S_2122_RdMode1Chain2StringReadBwd(IU8 * dest, IU32 eaOff, IU32 count, IUH destInRam)
{
  IU32 offs;
  IU8 bit;

  ENTER_FUNC(2122);
  bit  = (eaOff << 1) & 2 ^ 3;
  offs = GDP->VGAGlobals.read_mapped_plane + ((eaOff << 1) & (~3));
#ifdef BACK_M
  if (destInRam)
  {
    while(count--)
    {
      *dest = M1CH2(offs);
      offs -= bit;
      bit ^= 2;
      dest++;
    }
  }
  else
#endif
  {
    while(count--)
    {
      *dest = M1CH2(offs);
      offs -= bit;
      bit ^= 2;
      dest--;
    }
  }
  SET_LATCHES((offs+3) & (~3));
}


/******************************************************
 * Read Mode 1   Chain 4                              *
 ******************************************************/

GLOBAL IU32 S_2103_RdMode1Chain4ByteRead (IU32 eaOff)
{
  ENTER_FUNC(2103);
  SET_LATCHES(eaOff & (~3));
  return M1BYTE(eaOff);
}

GLOBAL IU32 S_2104_RdMode1Chain4WordRead (IU32 eaOff)
{
  ENTER_FUNC(2092);
  SET_LATCHES(eaOff & (~3));
  return M1WORD(eaOff);
}

GLOBAL IU32 S_2105_RdMode1Chain4DwordRead(IU32 eaOff)
{
  ENTER_FUNC(2105);
  SET_LATCHES((eaOff + 3) & (~3));
  return M1DWORD(eaOff);
}

GLOBAL void S_2106_RdMode1Chain4StringReadFwd(IU8 * dest, IU32 eaOff, IU32 count, IUH destInRam)
{
  ENTER_FUNC(2106);
#ifdef BACK_M
  if (destInRam)
  {
    while(count--)
    {
      *dest = M1BYTE(eaOff++);
      dest--;	/* Backwards */
    }
  }
  else
#endif
  {
    while(count--)
    {
      *dest = M1BYTE(eaOff++);
      dest++;	/* Forwards */
    }
  }
  SET_LATCHES((eaOff-1) & (~3));
}

GLOBAL void S_2123_RdMode1Chain4StringReadBwd(IU8 * dest, IU32 eaOff, IU32 count, IUH destInRam)
{
  ENTER_FUNC(2123);
#ifdef BACK_M
  if (destInRam)
  {
    while(count--)
    {
      *dest = M1BYTE(eaOff--);
      dest++;
    }
  }
  else
#endif
  {
    while(count--)
    {
      *dest = M1BYTE(eaOff--);
      dest--;	/* Forwards */
    }
  }
  SET_LATCHES((eaOff+1) & (~3));
}

/******************************************************
 * Read Mode 1   Unchained                            *
 ******************************************************/

GLOBAL IU32 S_2107_RdMode1UnchainedByteRead (IU32 eaOff)
{
  IU32 ret;

  ENTER_FUNC(2107);
  SET_LATCHES(eaOff * 4);
  ret  = M1DWORD(eaOff * 4);
  return M1UNC(ret);
}

GLOBAL IU32 S_2108_RdMode1UnchainedWordRead (IU32 eaOff)
{
  IU32 tmp;
  IU16 ret;

  ENTER_FUNC(2108);
  SET_LATCHES(eaOff * 4 + 4);
  tmp = M1DWORD(eaOff * 4);
  ret = (IU8)M1UNC(tmp);
  tmp = M1DWORD(eaOff * 4 + 4);
  return ret | (((IU16)M1UNC(tmp))<<8);
}

GLOBAL IU32 S_2109_RdMode1UnchainedDwordRead(IU32 eaOff)
{
  IU32 ret;

  /* NB: I think the EVID implementation is buggy here,because it only uses lower WORD and stores higher WORD into the void (localIUH)
   * So I just used the word read like we did in mode 0, hopefully this is correct
   */
  ENTER_FUNC(2109);
  ret = S_2108_RdMode1UnchainedWordRead (eaOff);
  return ret | ((IU32)S_2108_RdMode1UnchainedWordRead (eaOff + 8)<<16);
}

GLOBAL void S_2110_RdMode1UnchainedStringReadFwd(IU8 * dest, IU32 eaOff, IU32 count, IUH destInRam)
{
  IU32 tmp;
  IU32 offs;

  ENTER_FUNC(2110);
  offs = GDP->VGAGlobals.read_mapped_plane + 4 * eaOff;
#ifdef BACK_M
  if (destInRam)
  {
    while(count--)
    {
      tmp = M1DWORD(offs);
      *dest = M1UNC(tmp);
      offs += 4;
      dest--;	/* Backwards */
    }
  }
  else
#endif
  {
    while(count--)
    {
      tmp = M1DWORD(offs);
      *dest = M1UNC(tmp);
      offs += 4;
      dest++;	/* Forward */
    }
  }
  SET_LATCHES(offs-4-GDP->VGAGlobals.read_mapped_plane);
}

GLOBAL void S_2124_RdMode1UnchainedStringReadBwd(IU8 * dest, IU32 eaOff, IU32 count, IUH destInRam)
{
  IU32 tmp;
  IU32 offs;

  ENTER_FUNC(2124);
  offs = GDP->VGAGlobals.read_mapped_plane + 4 * eaOff;
#ifdef BACK_M
  if (destInRam)
  {
    while(count--)
    {
      tmp = M1DWORD(offs);
      *dest = M1UNC(tmp);
      offs -= 4;
      dest++;
    }
  }
  else
#endif
  {
    while(count--)
    {
      tmp = M1DWORD(offs);
      *dest = M1UNC(tmp);
      offs -= 4;
      dest--;
    }
  }
  SET_LATCHES(offs+4-GDP->VGAGlobals.read_mapped_plane);
}


/******************************************************
 * Read Disabled RAM                                  *
 ******************************************************/
GLOBAL IU32 S_2111_DisabledRAMByteRead (IU32 eaOff)
{
  ENTER_FUNC(2111);
  GDP->VGAGlobals.latches = -1;
  return -1;
}

GLOBAL IU32 S_2112_DisabledRAMWordRead(IU32 eaOff)
{
  ENTER_FUNC(2112);
  GDP->VGAGlobals.latches = -1;
  return -1;
}

GLOBAL IU32 S_2113_DisabledRAMDwordRead(IU32 eaOff)
{
  ENTER_FUNC(2113);
  GDP->VGAGlobals.latches = -1;
  return -1;
}

GLOBAL void S_2114_DisabledRAMStringReadFwd(IU8 * dest, IU32 eaOff, IU32 count, IUH destInRam)
{
  ENTER_FUNC(2114);
#ifdef BACK_M
  if (destInRam)
  {
    memset(dest-count, -1, count);
    dest-=count;
  }
  else
#endif
  {
    memset(dest, -1, count);
    dest+=count;
  }
  GDP->VGAGlobals.latches = -1;
}

GLOBAL void S_2125_DisabledRAMStringReadBwd(IU8 * dest, IU32 eaOff, IU32 count, IUH destInRam)
{
  ENTER_FUNC(2125);
#ifdef BACK_M
  if (destInRam)
  {
    memset(dest, -1, count);
    dest+=count;
  }
  else
#endif
  {
    memset(dest-count, -1, count);
    dest-=count;
  }
  GDP->VGAGlobals.latches = -1;
}


/******************************************************
 * Read simple bytes                                  *
 ******************************************************/
GLOBAL IU32 S_2115_SimpleByteRead (IU32 eaOff)
{
  ENTER_FUNC(2115);
  return GDP->VGAGlobals.screen_ptr[eaOff];
}

GLOBAL IU32 S_2116_SimpleWordRead(IU32 eaOff)
{
  ENTER_FUNC(2116);
  return *((IU16*)&GDP->VGAGlobals.screen_ptr[eaOff]);
}

GLOBAL IU32 S_2117_SimpleDwordRead(IU32 eaOff)
{
  ENTER_FUNC(2117);
  return *((IU32*)&GDP->VGAGlobals.screen_ptr[eaOff]);
}

GLOBAL void S_2118_SimpleStringRead(IU8 * dest, IU32 eaOff, IU32 count, IUH destInRam)
{
  IU32 offs;

  ENTER_FUNC(2118);
#ifdef BACK_M
  if (destInRam)
  {
    while(count--)
    {
      *dest = GDP->VGAGlobals.screen_ptr[eaOff++];
      dest--;	/* Backwards */
    }
  }
  else
#endif
  {
    memcpy (dest, &GDP->VGAGlobals.screen_ptr[eaOff], count);
    dest += count;
  }
}


//================================================= sevid00001 =============================================//

/*========= Mark Functions ============ */

GLOBAL IU32 S_2126_SimpleMark ()
{
  ENTER_FUNC(2126);
  GDP->VGAGlobals.dirty_total++;
}


/******************************************************
 * CGA mode                                           *
 ******************************************************/

GLOBAL IU32 S_2127_CGAMarkByte(IU32 eaOff)
{
  ENTER_FUNC(2127);
  GDP->VGAGlobals.dirty_total++;
  eaOff &= 0xDFFF;
  if (eaOff < GDP->VGAGlobals.dirty_low ) GDP->VGAGlobals.dirty_low  = eaOff;
  if (eaOff > GDP->VGAGlobals.dirty_high) GDP->VGAGlobals.dirty_high = eaOff;
}

GLOBAL IU32 S_2128_CGAMarkWord(IU32 eaOff)
{
  IU16 offs;

  ENTER_FUNC(2128);
  GDP->VGAGlobals.dirty_total += 2;
  offs = eaOff & 0xDFFF;
  if (offs < GDP->VGAGlobals.dirty_low ) GDP->VGAGlobals.dirty_low  = offs;
  offs = ((IU16)eaOff + 1) & 0xDFFF;
  if (offs > GDP->VGAGlobals.dirty_high) GDP->VGAGlobals.dirty_high = offs;
}

GLOBAL IU32 S_2129_CGAMarkDword(IU32 eaOff)
{
  IU16 offs;

  ENTER_FUNC(2129);
  GDP->VGAGlobals.dirty_total += 4;
  offs = eaOff & 0xDFFF;
  if (offs < GDP->VGAGlobals.dirty_low ) GDP->VGAGlobals.dirty_low  = offs;
  offs = ((IU16)eaOff + 3) & 0xDFFF;
  if (offs > GDP->VGAGlobals.dirty_high) GDP->VGAGlobals.dirty_high = offs;
}

GLOBAL IU32 S_2130_CGAMarkString(IU32 eaOff, IU32 count)
{
  ENTER_FUNC(2129);
  CGA_MARK_STRING(count,sizeof(IU8));
}



/******************************************************
 * Unchained mode                                     *
 ******************************************************/
GLOBAL IU32 S_2131_UnchainedMarkByte(IU32 eaOff)
{
  IUH offs;

  ENTER_FUNC(2131);
  GDP->VGAGlobals.dirty_total++;
  offs = GDP->VGAGlobals.v7_bank_vid_copy_off + (eaOff >> 2);
  if (offs < GDP->VGAGlobals.dirty_low ) GDP->VGAGlobals.dirty_low  = offs;
  if (offs > GDP->VGAGlobals.dirty_high) GDP->VGAGlobals.dirty_high = offs;
  GDP->VGAGlobals.video_copy[offs] = 1;
}

GLOBAL IU32 S_2132_UnchainedMarkWord(IU32 eaOff)
{
  IUH offs;

  ENTER_FUNC(2132);
  GDP->VGAGlobals.dirty_total += 2;

  offs = GDP->VGAGlobals.v7_bank_vid_copy_off + (eaOff >> 2);
  if (offs < GDP->VGAGlobals.dirty_low ) GDP->VGAGlobals.dirty_low  = offs;
  GDP->VGAGlobals.video_copy[offs] = 1;

  offs = GDP->VGAGlobals.v7_bank_vid_copy_off + ((eaOff + 1) >> 2);
  if (offs > GDP->VGAGlobals.dirty_high) GDP->VGAGlobals.dirty_high = offs;
  GDP->VGAGlobals.video_copy[offs] = 1;
}

GLOBAL IU32 S_2133_UnchainedMarkDword(IU32 eaOff)
{
  IUH offs;

  ENTER_FUNC(2133);
  // BUGBUG? In EVID src, it's 2, but that doesn't make sense to me:
  GDP->VGAGlobals.dirty_total += 4;

  offs = GDP->VGAGlobals.v7_bank_vid_copy_off + (eaOff >> 2);
  if (offs < GDP->VGAGlobals.dirty_low ) GDP->VGAGlobals.dirty_low  = offs;
  *((IU32*)&GDP->VGAGlobals.video_copy[offs]) = 0x01010101;

  offs = GDP->VGAGlobals.v7_bank_vid_copy_off + ((eaOff + 3) >> 2);
  if (offs > GDP->VGAGlobals.dirty_high) GDP->VGAGlobals.dirty_high = offs;
}

GLOBAL IU32 S_2134_UnchainedMarkString(IU32 eaOff, IU32 count)
{
  IUH offs, offslo;

  ENTER_FUNC(2134);
  GDP->VGAGlobals.dirty_total += count;
  offs = GDP->VGAGlobals.v7_bank_vid_copy_off + (eaOff >> 2);
  offslo = offs;
  if (offs < GDP->VGAGlobals.dirty_low ) GDP->VGAGlobals.dirty_low  = offs;
  offs = GDP->VGAGlobals.v7_bank_vid_copy_off + ((eaOff + count) >> 2);
  if (offs > GDP->VGAGlobals.dirty_high) GDP->VGAGlobals.dirty_high = offs;
  while (offs >= offslo)
    GDP->VGAGlobals.video_copy[offs--] = 1; 
}

/******************************************************
 * Chain 4                                            *
 ******************************************************/
GLOBAL IU32 S_2135_Chain4MarkByte(IU32 eaOff)
{
  ENTER_FUNC(2135);
  S_2131_UnchainedMarkByte(eaOff);
}

GLOBAL IU32 S_2136_Chain4MarkWord(IU32 eaOff)
{
  ENTER_FUNC(2136);
  S_2132_UnchainedMarkWord(eaOff);
}

GLOBAL IU32 S_2137_Chain4MarkDword(IU32 eaOff)
{
  ENTER_FUNC(2137);
  S_2133_UnchainedMarkDword(eaOff);
}

GLOBAL IU32 S_2138_Chain4MarkString(IU32 eaOff, IU32 count)
{
  ENTER_FUNC(2138);
  S_2134_UnchainedMarkString(eaOff, count);
}


/*========= Write Functions ============ */

/******************************************************
 * Write simple bytes                                 *
 ******************************************************/

GLOBAL void S_2139_SimpleByteWrite (IU32 eaOff, IU8 eaVal)
{
  ENTER_FUNC(2139);
  GDP->VGAGlobals.dirty_total++;
  GDP->VGAGlobals.screen_ptr[eaOff] = eaVal;  
}

GLOBAL void S_2140_SimpleByteFill (IU32 eaOff, IU8 eaVal, IU32 count)
{
  ENTER_FUNC(2140);
  GDP->VGAGlobals.dirty_total += count;  
  memset(&GDP->VGAGlobals.screen_ptr[eaOff], eaVal, count);
}

GLOBAL void S_2141_SimpleByteMove_Fwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  IU8 *dest;

  ENTER_FUNC(2141);
  GDP->VGAGlobals.dirty_total += count;  
  dest = (IU8*)&GDP->VGAGlobals.screen_ptr[eaOff];
  if (srcInRAM)
  {
#ifdef BACK_M
    while (count--)
    {
      *dest = *(IU8*)fromOff;
      dest++;
      fromOff--;
    }
#else
    memcpy(dest, fromOff, count);
#endif
  }
  else
  {
    memmove(dest, &GDP->VGAGlobals.screen_ptr[fromOff], count);
  }
}

GLOBAL void S_2142_SimpleWordWrite (IU32 eaOff, IU16 eaVal)
{
  ENTER_FUNC(2142);
  GDP->VGAGlobals.dirty_total += 2;
  *((IU16*)&GDP->VGAGlobals.screen_ptr[eaOff]) = eaVal; 
}

GLOBAL void S_2143_SimpleWordFill (IU32 eaOff, IU16 eaVal, IU32 count)
{
  IU16 *dest;

  ENTER_FUNC(2143);
  GDP->VGAGlobals.dirty_total += 2 * count;  
  dest = (IU16*)&GDP->VGAGlobals.screen_ptr[eaOff];
  while (count--)
  {
    *dest = eaVal;
    dest++;
  }
}

GLOBAL void S_2144_SimpleWordMove_Fwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  IU8 *dest;

  ENTER_FUNC(2143);
  GDP->VGAGlobals.dirty_total += 2 * count;
  dest = (IU16*)&GDP->VGAGlobals.screen_ptr[eaOff];
  if (srcInRAM)
  {
#ifdef BACK_M
    count *= 2;
    while (count--)
    {
      *dest = *(IU8*)fromOff;
      fromOff--;
      dest++;
    }
#else
    memcpy(dest, fromOff, count*2);
#endif
  }
  else
  {
    memmove(dest, &GDP->VGAGlobals.screen_ptr[fromOff], count * 2);
  } 
}

GLOBAL void S_2145_SimpleDwordWrite (IU32 eaOff, IU16 eaVal)
{
  ENTER_FUNC(2145);
  GDP->VGAGlobals.dirty_total += 4;
  *((IU32*)&GDP->VGAGlobals.screen_ptr[eaOff]) = eaVal; 
}

GLOBAL void S_2146_SimpleDwordFill (IU32 eaOff, IU32 eaVal, IU32 count)
{
  IU32 *dest;

  ENTER_FUNC(2146);
  GDP->VGAGlobals.dirty_total += 4 * count;  
  dest = (IU32*)&GDP->VGAGlobals.screen_ptr[eaOff];
  while (count--)
  {
    *dest = eaVal;
    dest++;
  }
}

GLOBAL void S_2147_SimpleDwordMove_Fwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  IU8 *dest;

  ENTER_FUNC(2143);
  GDP->VGAGlobals.dirty_total += 4 * count;
  dest = (IU8*)&GDP->VGAGlobals.screen_ptr[eaOff];
  if (srcInRAM)
  {
#ifdef BACK_M
    count *= 4;
    while (count--)
    {
      *dest = *(IU8*)fromOff;
      fromOff--;
      dest++;
    }
#else
    memcpy(dest, fromOff, count*4);
#endif
  }
  else
  {
    memmove(dest, &GDP->VGAGlobals.screen_ptr[fromOff], count * 4);
  } 
}

GLOBAL void S_2148_SimpleByteMove_Bwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  IU8 *dest;

  ENTER_FUNC(2148);
  GDP->VGAGlobals.dirty_total += count;
  dest = (IU8*)&GDP->VGAGlobals.screen_ptr[eaOff];
  if (srcInRAM)
  {
#ifdef BACK_M
    while (count--)
    {
      *dest = *(IU8*)fromOff;
      fromOff++;
      dest--;
    }
#else
    memcpy(dest-count, fromOff-count,count);
#endif
  }
  else
  {
    memmove(dest - count, &GDP->VGAGlobals.screen_ptr[fromOff - count], count);
  } 
}

GLOBAL void S_2149_SimpleWordMove_Bwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  IU8 *dest;

  ENTER_FUNC(2149);
  GDP->VGAGlobals.dirty_total += 2 * count;
  dest = (IU16*)&GDP->VGAGlobals.screen_ptr[eaOff];
  if (srcInRAM)
  {
#ifdef BACK_M
    count *= 2;
    while (count--)
    {
      *dest = *(IU8*)fromOff;
      fromOff++;
      dest--;
    }
#else
    memcpy(dest, fromOff-count*2, count*2);
#endif
  }
  else
  {
    memmove(dest, &GDP->VGAGlobals.screen_ptr[fromOff], count * 2);
  } 
}


GLOBAL void S_2150_SimpleDwordMove_Bwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  IU8 *dest;

  ENTER_FUNC(2150);
  GDP->VGAGlobals.dirty_total += 4 * count;
  dest = (IU8*)&GDP->VGAGlobals.screen_ptr[eaOff];
  if (srcInRAM)
  {
#ifdef BACK_M
    count *= 4;
    while (count--)
    {
      *dest = *(IU8*)fromOff;
      fromOff++;
      dest--;
    }
#else
    memcpy(dest, fromOff-count*4, count*4);
#endif
  }
  else
  {
    memmove(dest, &GDP->VGAGlobals.screen_ptr[fromOff], count * 4);
  } 
}


//================================================= sevid00002 =============================================//


/*************** V7 ***************/

// NB: eaVal seems to be ignored in all functions?
void S_2151_UnchainedByteWrite_00000000_0000000e_00000001 (IU32 eaOff, IU8 eaVal)
{
  ENTER_FUNC(2151);
  eaVal = GDP->VGAGlobals.v7_fg_latches;
  UCBWRTF(2151,UCBMSK,NOTRANS,UCBWRTWPL);
}

void S_2152_UnchainedByteFill_00000000_0000000e_00000001 (IU32 eaOff, IU8 eaVal, IU32 count)
{
  ENTER_FUNC(2152);
  eaVal = GDP->VGAGlobals.v7_fg_latches;
  UCBFLLF(2152,UCBMSK,NOTRANS,UCBPLNBFLL);
}

void S_2153_Unchained1PlaneByteFill (IU32 eaOff, IU8 eaVal, IU32 count)
{
  IU8 *dest;

  ENTER_FUNC(2153);
  dest = &GDP->VGAGlobals.VGA_wplane[eaOff];
  while (count--)
  {
    *dest = eaVal;
    dest += 4;
  }
}

void S_2154_UnchainedByteMove_00000000_0000000e_00000001_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCBMOVFW(2154, S_2155_CopyBytePlnByPlnUnchained_00000000_0000000e_00000001_00000000);
}

void S_2155_CopyBytePlnByPlnUnchained_00000000_0000000e_00000001_00000000 (IU32 eaOff, IHPE fromOff, IHPE destOff, IU32 count, IBOOL srcInRAM)
{
  UCBPLNCPY(2155, S_2156_CopyByte1PlaneUnchained_00000000_0000000e_00000001_00000000);
}

void S_2156_CopyByte1PlaneUnchained_00000000_0000000e_00000001_00000000 (IU32 eaOff, IHPE fromOff, IHPE destOff, IU32 count, IBOOL srcInRAM, IUH shift)
{
  UCBCPYB1PLN(2156,+1,1,NOTRANS,UCBMSKL);
}

void S_2157_UnchainedWordWrite_00000000_0000000e_00000001 (IU32 eaOff, IU16 eaVal)
{
  ENTER_FUNC(2157);
  GDP->VGAGlobals.mark_word(eaOff);
  UCBWRTWPL(4 * (eaOff+0), UCBMSK(GDP->VGAGlobals.v7_fg_latches));
  UCBWRTWPL(4 * (eaOff+1), UCBMSK(GDP->VGAGlobals.v7_fg_latches));
}

void S_2158_UnchainedWordFill_00000000_0000000e_00000001 (IU32 eaOff, IU16 eaVal, IU32 count)
{
  IU32 data;

  ENTER_FUNC(2158);  
  GDP->VGAGlobals.mark_string(eaOff, count * 2);
  data = UCBMSK(GDP->VGAGlobals.v7_fg_latches);
  UCBPLNWFLL(4 * eaOff,data,count);
}

void S_2159_Unchained1PlaneWordFill (IU32 eaOff, IU8 eaValLo, IU8 eaValHi, IU32 count)
{
  IU8 *dest;

  ENTER_FUNC(2159);
  dest = &GDP->VGAGlobals.VGA_wplane[eaOff];
  while (count--)
  {
    *dest = eaValLo;
    *(dest + 4) = eaValHi;
    dest += 8;
  }
}

void S_2160_UnchainedWordMove_00000000_0000000e_00000001_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCWMOVFW(2160, S_2161_CopyWordPlnByPlnUnchained_00000000_0000000e_00000001_00000000)
}

void S_2161_CopyWordPlnByPlnUnchained_00000000_0000000e_00000001_00000000 (IU32 eaOff, IHPE fromOff, IHPE destOff, IU32 count, IBOOL srcInRAM)
{
  UCBPLNCPY(2161, S_2162_CopyWord1PlaneUnchained_00000000_0000000e_00000001_00000000) 
}

void S_2162_CopyWord1PlaneUnchained_00000000_0000000e_00000001_00000000 (IU32 eaOff, IHPE fromOff, IHPE destOff, IU32 count, IBOOL srcInRAM, IUH shift)
{
  // NB: Original routines do all the stuff, but I think it's no different to:
  ENTER_FUNC(2162);
  return S_2156_CopyByte1PlaneUnchained_00000000_0000000e_00000001_00000000(eaOff, fromOff, destOff, count * 2, srcInRAM, shift);
}

void S_2163_UnchainedDwordWrite_00000000_0000000e_00000001(IU32 eaOff, IU32 eaVal)
{
  UCDWRTF(2163, S_2157_UnchainedWordWrite_00000000_0000000e_00000001);
}

void S_2164_UnchainedDwordFill_00000000_0000000e_00000001 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  UCDFLLF(2164, S_2158_UnchainedWordFill_00000000_0000000e_00000001);
}

void S_2165_UnchainedDwordMove_00000000_0000000e_00000001_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCDMOVFW(2165, S_2160_UnchainedWordMove_00000000_0000000e_00000001_00000000);
}

/*************** VGA ***************/

void S_2166_UnchainedByteWrite_00000001_0000000e_00000001 (IU32 eaOff, IU8 eaVal)
{
  UCBWRTF(2166,NOMSK,NOTRANS,UCBWRTWPL);
}

void S_2167_UnchainedByteFill_00000001_0000000e_00000001 (IU32 eaOff, IU8 eaVal, IU32 count)
{
  UCBFLLF(2167,NOMSK,NOTRANS,UCBPLNBFLL);
}

void S_2168_UnchainedByteMove_00000001_0000000e_00000001_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCBMOVFW1(2168,S_2167_UnchainedByteFill_00000001_0000000e_00000001,S_2169_CopyDirByte1Plane_00000000);
}

void S_2169_CopyDirByte1Plane_00000000 (IU32 eaOff, IHPE fromOff, IU32 count)
{
  ENTER_FUNC(2169);
  while (count--)
  {
    *((IU8*)eaOff) = *((IU8*)fromOff);
    eaOff += 4;
    fromOff += 4;
  }
}

void S_2170_CopyFwdByte1Plane (IU32 eaOff, IHPE fromOff, IU32 count)
{
  ENTER_FUNC(2170);
  while (count--)
  {
    *((IU8*)eaOff) = *((IU8*)fromOff);
    eaOff += 4;
    fromOff += 4;
  }
}

void S_2171_UnchainedWordWrite_00000001_0000000e_00000001 (IU32 eaOff, IU16 eaVal)
{
  ENTER_FUNC(2171);
  GDP->VGAGlobals.mark_word(eaOff);
  UCBWRTWPL(4 * (eaOff+0),GDP->VGAGlobals.latches);
  UCBWRTWPL(4 * (eaOff+1),GDP->VGAGlobals.latches);
}

void S_2172_UnchainedWordFill_00000001_0000000e_00000001 (IU32 eaOff, IU16 eaVal, IU32 count)
{
  IU32 data;

  ENTER_FUNC(2172);  
  GDP->VGAGlobals.mark_string(eaOff, count * 2);
  data = GDP->VGAGlobals.latches;
  UCBPLNWFLL(4 * eaOff,data,count);
}

void S_2173_UnchainedWordMove_00000001_0000000e_00000001_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCBMOVFW1(2173,S_2172_UnchainedWordFill_00000001_0000000e_00000001,S_2174_CopyDirWord1Plane_00000000);
}


void S_2174_CopyDirWord1Plane_00000000 (IU32 eaOff, IHPE fromOff, IU32 count)
{
  ENTER_FUNC(2174);
  S_2175_CopyFwdWord1Plane(eaOff, fromOff, count);
}


void S_2175_CopyFwdWord1Plane (IU32 eaOff, IHPE fromOff, IU32 count)
{
  while (count--)
  {
    *((IU8*)eaOff) = *((IU8*)fromOff);
    *((IU8*)(eaOff + 4)) = *((IU8*)fromOff);
    eaOff += 8;
    fromOff += 4;
  }
}

void S_2176_UnchainedDwordWrite_00000001_0000000e_00000001(IU32 eaOff, IU32 eaVal)
{
  UCDWRTF(2176,S_2171_UnchainedWordWrite_00000001_0000000e_00000001);
}

void S_2177_UnchainedDwordFill_00000001_0000000e_00000001 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  UCDFLLF(2177,S_2172_UnchainedWordFill_00000001_0000000e_00000001);
}

void S_2178_UnchainedDwordMove_00000001_0000000e_00000001_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCDMOVFW(2178,S_2173_UnchainedWordMove_00000001_0000000e_00000001_00000000);
}




void S_2179_UnchainedByteWrite_00000002_0000000e_00000001 (IU32 eaOff, IU8 eaVal)
{
  ENTER_FUNC(2179);
  eaVal = GDP->VGAGlobals.v7_fg_latches;
  UCBWRTF(2179,UCBMSK,NOTRANS,UCBWRTWPL);
}

void S_2180_UnchainedByteFill_00000002_0000000e_00000001 (IU32 eaOff, IU8 eaVal, IU32 count)
{
  ENTER_FUNC(2180);
  eaVal = GDP->VGAGlobals.v7_fg_latches;
  UCBFLLF(2180,UCBMSK,NOTRANS,UCBPLNBFLL);
}

void S_2181_UnchainedByteMove_00000002_0000000e_00000001_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCBMOVFW(2181, S_2182_CopyBytePlnByPlnUnchained_00000002_0000000e_00000001_00000000);
}

void S_2182_CopyBytePlnByPlnUnchained_00000002_0000000e_00000001_00000000 (IU32 eaOff, IHPE fromOff, IHPE destOff, IU32 count, IBOOL srcInRAM)
{
  UCBPLNCPY(2182, S_2183_CopyByte1PlaneUnchained_00000002_0000000e_00000001_00000000);
}

void S_2183_CopyByte1PlaneUnchained_00000002_0000000e_00000001_00000000 (IU32 eaOff, IHPE fromOff, IHPE destOff, IU32 count, IBOOL srcInRAM, IUH shift)
{
  UCBCPYB1PLN(2183,+1,0,NOTRANS,UCBMSKL);
}

void S_2184_UnchainedWordWrite_00000002_0000000e_00000001 (IU32 eaOff, IU16 eaVal)
{
  ENTER_FUNC(2184);	// Backward = Forward
  S_2157_UnchainedWordWrite_00000000_0000000e_00000001(eaOff, eaVal);
}

void S_2185_UnchainedWordFill_00000002_0000000e_00000001 (IU32 eaOff, IU16 eaVal, IU32 count)
{
  IU8 data;

  ENTER_FUNC(2185);  
  data = (IU8)UCBMSK(GDP->VGAGlobals.v7_fg_latches);
  UCBPLNWFLL1(4 * eaOff, data, data, count);
}

void S_2186_UnchainedWordMove_00000002_0000000e_00000001_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCWMOVFW(2186, S_2187_CopyWordPlnByPlnUnchained_00000002_0000000e_00000001_00000000);
}

void S_2187_CopyWordPlnByPlnUnchained_00000002_0000000e_00000001_00000000 (IU32 eaOff, IHPE fromOff, IHPE destOff, IU32 count, IBOOL srcInRAM)
{
  UCBPLNCPY(2161, S_2188_CopyWord1PlaneUnchained_00000002_0000000e_00000001_00000000);
}

void S_2188_CopyWord1PlaneUnchained_00000002_0000000e_00000001_00000000 (IU32 eaOff, IHPE fromOff, IHPE destOff, IU32 count, IBOOL srcInRAM, IUH shift)
{
  ENTER_FUNC(2188);
  // NB: Original routines do all the stuff, but I think it's no different to:
  return S_2183_CopyByte1PlaneUnchained_00000002_0000000e_00000001_00000000(eaOff, fromOff, destOff, count * 2, srcInRAM, shift);
}

void S_2189_UnchainedDwordWrite_00000002_0000000e_00000001(IU32 eaOff, IU32 eaVal)
{
  UCDWRTF(2189, S_2184_UnchainedWordWrite_00000002_0000000e_00000001);
}

void S_2190_UnchainedDwordFill_00000002_0000000e_00000001 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  UCDFLLF(2190, S_2185_UnchainedWordFill_00000002_0000000e_00000001);
}

void S_2191_UnchainedDwordMove_00000002_0000000e_00000001_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCDMOVFW(2191, S_2186_UnchainedWordMove_00000002_0000000e_00000001_00000000);
}

void S_2192_UnchainedByteWrite_00000003_0000000e_00000001 (IU32 eaOff, IU8 eaVal)
{
  UCBWRTF(2192,UCB3MSK,UCB3DATA,UCBWRTWPL);
}

void S_2193_UnchainedByteFill_00000003_0000000e_00000001 (IU32 eaOff, IU8 eaVal, IU32 count)
{
  UCBFLLF(2193,UCB3MSK,UCB3DATA,UCBPLNBFLL1);
}

void S_2194_UnchainedByteMove_00000003_0000000e_00000001_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCBMOVFW(2194, S_2195_CopyBytePlnByPlnUnchained_00000003_0000000e_00000001_00000000);
}

void S_2195_CopyBytePlnByPlnUnchained_00000003_0000000e_00000001_00000000 (IU32 eaOff, IHPE fromOff, IHPE destOff, IU32 count, IBOOL srcInRAM)
{
  UCBPLNCPY(2195, S_2196_CopyByte1PlaneUnchained_00000003_0000000e_00000001_00000000);
}

void S_2196_CopyByte1PlaneUnchained_00000003_0000000e_00000001_00000000 (IU32 eaOff, IHPE fromOff, IHPE destOff, IU32 count, IBOOL srcInRAM, IUH shift)
{
  UCBCPYB1PLN(2196,+1,0,NOTRANS,UCB3MSKL);
}

void S_2197_UnchainedWordWrite_00000003_0000000e_00000001 (IU32 eaOff, IU16 eaVal)
{
  UCWWRTF(2197,UCB3MSK,UCB3DATA,UCBWRTWPL);
}

void S_2198_UnchainedWordFill_00000003_0000000e_00000001 (IU32 eaOff, IU16 eaVal, IU32 count)
{
  UCWFLLF(2198,NOMSKX,UCB3DATA,UCBPLNWFLL1);
}

void S_2199_UnchainedWordMove_00000003_0000000e_00000001_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCWMOVFW(2199, S_2200_CopyWordPlnByPlnUnchained_00000003_0000000e_00000001_00000000);
}

void S_2200_CopyWordPlnByPlnUnchained_00000003_0000000e_00000001_00000000 (IU32 eaOff, IHPE fromOff, IHPE destOff, IU32 count, IBOOL srcInRAM)
{
  UCBPLNCPY(2200, S_2201_CopyWord1PlaneUnchained_00000003_0000000e_00000001_00000000);
}


void S_2201_CopyWord1PlaneUnchained_00000003_0000000e_00000001_00000000 (IU32 eaOff, IHPE fromOff, IHPE destOff, IU32 count, IBOOL srcInRAM, IUH shift)
{
  ENTER_FUNC(2196);
  S_2196_CopyByte1PlaneUnchained_00000003_0000000e_00000001_00000000(eaOff, fromOff, destOff, count * 2,srcInRAM, shift);
}

void S_2202_UnchainedDwordWrite_00000003_0000000e_00000001(IU32 eaOff, IU32 eaVal)
{
  UCDWRTF(2202, S_2197_UnchainedWordWrite_00000003_0000000e_00000001);
}

void S_2203_UnchainedDwordFill_00000003_0000000e_00000001 (IU32 eaOff, IU32 eaVal, IU32 count)
{
  UCDFLLF(2203, S_2198_UnchainedWordFill_00000003_0000000e_00000001);
}

void S_2204_UnchainedDwordMove_00000003_0000000e_00000001_00000000 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCDMOVFW(2204, S_2199_UnchainedWordMove_00000003_0000000e_00000001_00000000);
}

void S_2205_UnchainedByteMove_00000000_0000000e_00000001_00000001 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCBMOVBW(2205, S_2206_CopyBytePlnByPlnUnchained_00000000_0000000e_00000001_00000001);
}

void S_2206_CopyBytePlnByPlnUnchained_00000000_0000000e_00000001_00000001 (IU32 eaOff, IHPE fromOff, IHPE destOff, IU32 count, IBOOL srcInRAM)
{
  UCBPLNCPY(2206, S_2207_CopyByte1PlaneUnchained_00000000_0000000e_00000001_00000001);
}

void S_2207_CopyByte1PlaneUnchained_00000000_0000000e_00000001_00000001 (IU32 eaOff, IHPE fromOff, IHPE destOff, IU32 count, IBOOL srcInRAM, IUH shift)
{
  UCBCPYB1PLN(2207,-1,1,NOTRANS,UCBMSKL);
}

void S_2208_UnchainedWordMove_00000000_0000000e_00000001_00000001 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCWMOVBW(2208, S_2209_CopyWordPlnByPlnUnchained_00000000_0000000e_00000001_00000001);
}


void S_2209_CopyWordPlnByPlnUnchained_00000000_0000000e_00000001_00000001 (IU32 eaOff, IHPE fromOff, IHPE destOff, IU32 count, IBOOL srcInRAM)
{
  UCBPLNCPY(2209, S_2210_CopyWord1PlaneUnchained_00000000_0000000e_00000001_00000001);
}

void S_2210_CopyWord1PlaneUnchained_00000000_0000000e_00000001_00000001 (IU32 eaOff, IHPE fromOff, IHPE destOff, IU32 count, IBOOL srcInRAM, IUH shift)
{
  ENTER_FUNC(2210);
  S_2207_CopyByte1PlaneUnchained_00000000_0000000e_00000001_00000001(eaOff, fromOff, destOff, count * 2,srcInRAM, shift);
}


void S_2211_UnchainedDwordMove_00000000_0000000e_00000001_00000001 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCDMOVFW(2211, S_2208_UnchainedWordMove_00000000_0000000e_00000001_00000001);
}

void S_2212_UnchainedByteMove_00000001_0000000e_00000001_00000001 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCBMOVFW1(2212, S_2167_UnchainedByteFill_00000001_0000000e_00000001, S_2213_CopyDirByte1Plane_00000001);
}

void S_2213_CopyDirByte1Plane_00000001 (IU32 eaOff, IHPE fromOff, IU32 count)
{
  ENTER_FUNC(2213);
  S_2214_CopyBwdByte1Plane(eaOff, fromOff, count);
}

void S_2214_CopyBwdByte1Plane (IU32 eaOff, IHPE fromOff, IU32 count)
{
  ENTER_FUNC(2214);
  while(count--)
  {
    *(IU8*)eaOff = *(IU8*)fromOff;
    fromOff-=4;
    eaOff-=4;
  }
}

void S_2215_UnchainedWordMove_00000001_0000000e_00000001_00000001 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCWMOVBW1(2215,S_2172_UnchainedWordFill_00000001_0000000e_00000001,S_2216_CopyDirWord1Plane_00000001);
}

void S_2216_CopyDirWord1Plane_00000001 (IU32 eaOff, IHPE fromOff, IU32 count)
{
  ENTER_FUNC(2216);
  S_2217_CopyBwdWord1Plane(eaOff, fromOff, count);
}

void S_2217_CopyBwdWord1Plane (IU32 eaOff, IHPE fromOff, IU32 count)
{
  ENTER_FUNC(2214);
  while(count--)
  {
    *(IU8*)eaOff = *(IU8*)fromOff;
    *(IU8*)(eaOff + 4)= *(IU8*)fromOff;
    fromOff-=4;
    eaOff-=8;
  }
}

void S_2218_UnchainedDwordMove_00000001_0000000e_00000001_00000001 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCDMOVFW(2218, S_2215_UnchainedWordMove_00000001_0000000e_00000001_00000001);
}

void S_2219_UnchainedByteMove_00000002_0000000e_00000001_00000001 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCBMOVBW(2219, S_2220_CopyBytePlnByPlnUnchained_00000002_0000000e_00000001_00000001);
}

void S_2220_CopyBytePlnByPlnUnchained_00000002_0000000e_00000001_00000001 (IU32 eaOff, IHPE fromOff, IHPE destOff, IU32 count, IBOOL srcInRAM)
{
  UCBPLNCPY(2220, S_2221_CopyByte1PlaneUnchained_00000002_0000000e_00000001_00000001);
}

void S_2221_CopyByte1PlaneUnchained_00000002_0000000e_00000001_00000001 (IU32 eaOff, IHPE fromOff, IHPE destOff, IU32 count, IBOOL srcInRAM, IUH shift)
{
  UCBCPYB1PLN(2221,-1,0,NOTRANS,UCBMSK);
}

void S_2222_UnchainedWordMove_00000002_0000000e_00000001_00000001 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCWMOVBW(2222, S_2223_CopyWordPlnByPlnUnchained_00000002_0000000e_00000001_00000001);
}

void S_2223_CopyWordPlnByPlnUnchained_00000002_0000000e_00000001_00000001 (IU32 eaOff, IHPE fromOff, IHPE destOff, IU32 count, IBOOL srcInRAM)
{
  UCBPLNCPY(2223, S_2224_CopyWord1PlaneUnchained_00000002_0000000e_00000001_00000001);
}

void S_2224_CopyWord1PlaneUnchained_00000002_0000000e_00000001_00000001 (IU32 eaOff, IHPE fromOff, IHPE destOff, IU32 count, IBOOL srcInRAM, IUH shift)
{
  ENTER_FUNC(2224);
  S_2221_CopyByte1PlaneUnchained_00000002_0000000e_00000001_00000001(eaOff, fromOff, destOff, count * 2,srcInRAM, shift);
}


void S_2225_UnchainedDwordMove_00000002_0000000e_00000001_00000001 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCDMOVFW(2225, S_2222_UnchainedWordMove_00000002_0000000e_00000001_00000001);
}

void S_2226_UnchainedByteMove_00000003_0000000e_00000001_00000001 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCBMOVBW(2226, S_2227_CopyBytePlnByPlnUnchained_00000003_0000000e_00000001_00000001);
}


void S_2227_CopyBytePlnByPlnUnchained_00000003_0000000e_00000001_00000001 (IU32 eaOff, IHPE fromOff, IHPE destOff, IU32 count, IBOOL srcInRAM)
{
  UCBPLNCPY(2227, S_2228_CopyByte1PlaneUnchained_00000003_0000000e_00000001_00000001);
}

void S_2228_CopyByte1PlaneUnchained_00000003_0000000e_00000001_00000001 (IU32 eaOff, IHPE fromOff, IHPE destOff, IU32 count, IBOOL srcInRAM, IUH shift)
{
  UCBCPYB1PLN(2228,-1,0,NOTRANS,UCB3MSKL);
}

void S_2229_UnchainedWordMove_00000003_0000000e_00000001_00000001 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCWMOVBW(2229, S_2230_CopyWordPlnByPlnUnchained_00000003_0000000e_00000001_00000001);
}

void S_2230_CopyWordPlnByPlnUnchained_00000003_0000000e_00000001_00000001 (IU32 eaOff, IHPE fromOff, IHPE destOff, IU32 count, IBOOL srcInRAM)
{
  UCBPLNCPY(2230, S_2231_CopyWord1PlaneUnchained_00000003_0000000e_00000001_00000001);
}

void S_2231_CopyWord1PlaneUnchained_00000003_0000000e_00000001_00000001 (IU32 eaOff, IHPE fromOff, IHPE destOff, IU32 count, IBOOL srcInRAM, IUH shift)
{
  ENTER_FUNC(2231);
  S_2228_CopyByte1PlaneUnchained_00000003_0000000e_00000001_00000001(eaOff, fromOff, destOff, count * 2,srcInRAM, shift);
}


void S_2232_UnchainedDwordMove_00000003_0000000e_00000001_00000001 (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  UCDMOVFW(2232, S_2229_UnchainedWordMove_00000003_0000000e_00000001_00000001);
}

/* void S_2233-37 not in use */

void S_2238_GenericByteWrite(int eaOff, IU8 eaVal)
{
  IU8 rot;
  IU32 pattern, mask;

  ENTER_FUNC(2238);

  switch (GDP->VGAGlobals.wrmode)
  {
  case 1:
    pattern = GDP->VGAGlobals.latches;
    break;

  case 2:
    if (GDP->VGAGlobals.chain == 0 && GDP->VGAGlobals.dither)
      pattern = GDP->VGAGlobals.v7_fg_latches;
    else
      pattern = GDP->VGAGlobals.sr_lookup[eaVal & 0xF];
    pattern = UCBMSK(pattern);
    break;

  case 3:
    rot = (IU8)(GDP->VGAGlobals.rotate>>24);
    if (rot < 8) eaVal = (eaVal << (8 - rot)) | (eaVal >> rot);
    pattern = GDP->VGAGlobals.chain == 0?UCB3DATA(eaVal):eaVal;

    mask = GDP->VGAGlobals.dither?GDP->VGAGlobals.v7_fg_latches:GDP->VGAGlobals.sr_masked_val;
    switch (GDP->VGAGlobals.wrstate)
    {
      case 2: mask &= GDP->VGAGlobals.latches; break;
      case 4: mask |= GDP->VGAGlobals.latches; break;
      case 6: mask ^= GDP->VGAGlobals.latches; break;
      case 8: pattern &= GDP->VGAGlobals.bit_prot_mask; break;
    }
    pattern = pattern & mask | GDP->VGAGlobals.latches & ~pattern;
    break;

  case 0:
    if (GDP->VGAGlobals.chain == 0 && GDP->VGAGlobals.dither)
    {
      pattern = GDP->VGAGlobals.v7_fg_latches;
    }
    else 
    {
      rot = (IU8)(GDP->VGAGlobals.rotate>>24);
      if (rot < 8) eaVal = (eaVal << (8 - rot)) | (eaVal >> rot);
      pattern = GDP->VGAGlobals.chain == 0?UCB3DATA(eaVal):eaVal;
    }

    if ( GDP->VGAGlobals.wrstate & 16 )
      pattern = GDP->VGAGlobals.sr_masked_val | (GDP->VGAGlobals.sr_nmask & pattern);
    pattern = UCBMSK(pattern);
    break;
  }

  if (GDP->VGAGlobals.chain == 0)
  {
    GDP->VGAGlobals.mark_byte(4 * eaOff);
    UCBWRTWPL(4 * eaOff,pattern);
  }
  else 
  {
    if (GDP->VGAGlobals.plane_enable & (1 << (eaOff & 3)))
    {
      GDP->VGAGlobals.VGA_wplane[eaOff] = (IU8)pattern;
      GDP->VGAGlobals.mark_byte(eaOff);
    }
  }
}

void S_2239_GenericByteFill(int eaOff, IU8 eaVal, IU32 count)
{
  IU32 fact, i;

  ENTER_FUNC(2239);
  if (!count) return;
  fact = GDP->VGAGlobals.chain == 0?4:1;
  for (i=0; i<count; i++, eaOff+=fact)
    S_2238_GenericByteWrite(eaOff, eaVal);
}

void S_2240_GenericByteMove_Fwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  IU32 fact, i;

  ENTER_FUNC(2239);
  if (!count) return;
  fact = GDP->VGAGlobals.chain == 0?4:1;
  if (srcInRAM)
  {
    for (i=0; i<count; i++, eaOff+=fact)
    {
      S_2238_GenericByteWrite(eaOff, *((IU8*)fromOff));
#ifdef BACK_M
      fromOff--;
#else
      fromOff++;
#endif
    }
  }
  else
  {
    while(count--)
    {
      S_2238_GenericByteWrite(eaOff, GDP->VGAGlobals.read_byte_addr(fromOff));
      eaOff += fact;
      fromOff += fact;
    }
  }
}


void S_2241_GenericWordWrite(int eaOff, IU16 eaVal)
{
  IU8 rot;
  IU32 pattern, patternHi, mask;

  ENTER_FUNC(2238);

  switch (GDP->VGAGlobals.wrmode)
  {
  case 1:
    pattern   = patternHi = GDP->VGAGlobals.latches;
    break;

  case 2:
    if (GDP->VGAGlobals.chain == 0 && GDP->VGAGlobals.dither)
      pattern = patternHi = GDP->VGAGlobals.v7_fg_latches;
    else
      pattern = patternHi = GDP->VGAGlobals.sr_lookup[eaVal & 0xF];
    pattern   = UCBMSK(pattern);
    patternHi = UCBMSK(patternHi);
    break;

  case 0:
  case 3:
    if (GDP->VGAGlobals.wrmode == 0 && GDP->VGAGlobals.chain == 0 && GDP->VGAGlobals.dither)
      pattern = GDP->VGAGlobals.v7_fg_latches;
    else
    {
      rot = (IU8)(GDP->VGAGlobals.rotate>>24);
      if (rot < 8)
      {
        pattern   = (IU8)((IU8)eaVal << (8 - rot)) | ((IU8)eaVal >> rot);
        patternHi = (IU8)(eaVal>>8);
        patternHi = (IU8)((IU8)eaVal << (8 - rot)) | ((IU8)eaVal >> rot);
      }
      else
      {
        pattern = (IU8)eaVal;
        patternHi = (IU8)(eaVal>>8);
      }
      if (GDP->VGAGlobals.chain == 0)
      {
        pattern   = UCB3DATA(pattern);
        patternHi = UCB3DATA(pattern);
      }
    }

    if (GDP->VGAGlobals.wrmode == 3)
    {
      mask = GDP->VGAGlobals.dither?GDP->VGAGlobals.v7_fg_latches:GDP->VGAGlobals.sr_masked_val;
      switch (GDP->VGAGlobals.wrstate)
      {
        case 2: mask &= GDP->VGAGlobals.latches; break;
        case 4: mask |= GDP->VGAGlobals.latches; break;
        case 6: mask ^= GDP->VGAGlobals.latches; break;
        case 8: pattern &= GDP->VGAGlobals.bit_prot_mask; break;
      }
      pattern   = pattern   & mask | GDP->VGAGlobals.latches & ~pattern;
      patternHi = patternHi & mask | GDP->VGAGlobals.latches & ~pattern;
    }
    else
    {
      if ( GDP->VGAGlobals.wrstate & 16 )
      {
        pattern   = UCB5MSKD(pattern);
        patternHi = UCB5MSKD(pattern);
      }
      pattern   = UCBMSK(pattern);
      patternHi = UCBMSK(patternHi);
    }
    break;
  }

  if (GDP->VGAGlobals.chain == 0)
  {
    GDP->VGAGlobals.mark_byte(4 * eaOff);
    UCBWRTWPL(4 * (eaOff+0),pattern);
    UCBWRTWPL(4 * (eaOff+1),patternHi);
  }
  else 
  {
    if (GDP->VGAGlobals.plane_enable & (1 << (eaOff & 3)))
      GDP->VGAGlobals.VGA_wplane[eaOff] = (IU8)pattern;
    if (GDP->VGAGlobals.plane_enable & (1 << ((eaOff + 1) & 3)))
    {
      GDP->VGAGlobals.VGA_wplane[eaOff+1] = (IU8)patternHi;
      GDP->VGAGlobals.mark_byte(eaOff+1);
    }
  }
}

void S_2242_GenericWordFill(int eaOff, IU16 eaVal, IU32 count)
{
  IU32 fact, i;

  ENTER_FUNC(2242);
  if (!count) return;
  fact = GDP->VGAGlobals.chain == 0?8:2;
  for (i=0; i<count; i++, eaOff+=fact)
    S_2241_GenericWordWrite(eaOff, eaVal);
}

void S_2243_GenericWordMove_Fwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  IU32 fact, i;

  ENTER_FUNC(2243);
  if (!count) return;
  fact = GDP->VGAGlobals.chain == 0?8:2;
  if (srcInRAM)
  {
    for (i=0; i<count; i++, eaOff+=fact)
    {
#ifdef BACK_M
      S_2241_GenericWordWrite(eaOff, *((IU8*)fromOff) | (*((IU8*)fromOff-1))<<8);
      fromOff-=2;
#else
      S_2241_GenericWordWrite(eaOff, *((IU16*)fromOff));
      fromOff+=2;
#endif
    }
  }
  else
  {
    while(count--)
    {
      S_2241_GenericWordWrite(eaOff, (IU16)GDP->VGAGlobals.read_byte_addr(fromOff) | ((IU16)GDP->VGAGlobals.read_byte_addr(fromOff + 1)<<8));
      eaOff += fact;
      fromOff += 2;
    }
  }
}

void S_2244_GenericDwordWrite(int eaOff, IU32 eaVal)
{

  ENTER_FUNC(2244);
  S_2241_GenericWordWrite(eaOff, (IU16)eaVal);
  eaOff += GDP->VGAGlobals.chain?2:8;
  S_2241_GenericWordWrite(eaOff, (IU16)(eaVal>>16));
}

void S_2245_GenericDwordFill(int eaOff, IU32 eaVal, IU32 count)
{
  IU32 fact, i;

  ENTER_FUNC(2245);
  if (!count) return;
  fact = GDP->VGAGlobals.chain == 0?16:4;
  for (i=0; i<count; i++, eaOff+=fact)
    S_2244_GenericDwordWrite(eaOff, eaVal);
}

void S_2246_GenericDwordMove_Fwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  ENTER_FUNC(2246);
  S_2243_GenericWordMove_Fwd(eaOff, fromOff, count * 2, srcInRAM);
}

void S_2247_GenericByteMove_Bwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  IU32 fact, i;

  ENTER_FUNC(2247);
  if (!count) return;
  fact = GDP->VGAGlobals.chain == 0?-4:-1;
  if (srcInRAM)
  {
    for (i=0; i<count; i++, eaOff+=fact)
    {
      S_2238_GenericByteWrite(eaOff, (fromOff & 0xFFFFFF00) | *((IU8*)fromOff));
#ifdef BACK_M
      fromOff++;
#else
      fromOff--;
#endif
    }
  }
  else
  {
    while(count--)
    {
      S_2238_GenericByteWrite(eaOff, (fromOff & 0xFFFFFF00) | GDP->VGAGlobals.read_byte_addr(fromOff));
      eaOff += fact;
      fromOff += fact;
    }
  }
}

void S_2248_GenericWordMove_Bwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  IU32 fact, i;

  ENTER_FUNC(2248);
  if (!count) return;
  fact = GDP->VGAGlobals.chain == 0?-8:-2;
  if (srcInRAM)
  {
    for (i=0; i<count; i++, eaOff+=fact)
    {
#ifdef BACK_M
      S_2241_GenericWordWrite(eaOff, (fromOff & 0xFFFF0000) | *((IU8*)fromOff) | (*((IU8*)fromOff+1))<<8);
      fromOff+=2;
#else
      S_2241_GenericWordWrite(eaOff, *((IU16*)fromOff));
      fromOff-=2;
#endif
    }
  }
  else
  {
    while(count--)
    {
      S_2241_GenericWordWrite(eaOff,  ((fromOff-1) & 0xFFFF0000) |(IU16)GDP->VGAGlobals.read_byte_addr(fromOff) | ((IU16)GDP->VGAGlobals.read_byte_addr(fromOff - 1)<<8));
      eaOff += fact;
      fromOff -= 2;
    }
  }
}

void S_2249_GenericDwordMove_Bwd (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM)
{
  ENTER_FUNC(2249);
  S_2248_GenericWordMove_Bwd(eaOff, fromOff, count * 2, srcInRAM);
}


