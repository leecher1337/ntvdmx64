#include  "sinit010.h"
#include  "ga_mark.h"
#include  "cpu_vid.h"
#include  "evidfunc.h"
#include  "evidcopy.h"
#include <stdlib.h> 

/* 
 .-._                                                   _,-,
  `._`-._                                           _,-'_,'
     `._ `-._                                   _,-' _,'
        `._  `-._        __.-----.__        _,-'  _,'
           `._   `#==="""           """===#'   _,'
              `._/)  ._               _.  (\_,'
               )*'     **.__     __.**     '*( 
               #  .==..__  ""   ""  __..==,  # 
               #   `"._(_).       .(_)_."'   #


                   WELCOME TO MACRO HELL! 
*/


/* FIXME: CVIDC interface assumes BACK_M
 * As we are currently not finished converting all the CVID functions, we still have
 * these patches in there, that reverse memory direction in our SAS interface.
 * As soon as implementation and testing finished, we can remove this and also the
 * hacks that reverse memory:
 */
//#define BACK_M

// May be needed for yoda, disabled for performance reasons
#ifdef DEBUG_EVID
#define ENTER_FUNC sevid_enter_func
#define GDP Gdp
#else
#define ENTER_FUNC(x) register struct gdp_struct *GDP = Gdp;
#endif

// Direction of pointer on forward and backward memory
#ifdef BACK_M
#define RAM_DIR -1
#else
#define RAM_DIR +1
#endif

// This macro marks code that is present in original CVIDC but is buggy there
// and thus shouldn't be there
#define BUGGY_IN_CVID(x)


/******************************************************************
 * MASKS                                                          *
 ******************************************************************/

/**** Transformations ****/

// No transformation in macro parameters that offer transformation of input
#define NOTRANS(x) x
#define UCB3DATA(x) (((IU32)(x)<<24) | ((IU32)(x)<<16) | ((IU32)(x)<<8) | ((IU32)(x)))
#define TRANS1(x) GDP->VGAGlobals.sr_lookup[(x) & 0xF]

#define SET_LATCHES(offs) GDP->VGAGlobals.latches = *(IU32 *)&GDP->VGAGlobals.VGA_rplane[offs];

#define M1BYTE(offs) ((IU8)(GDP->VGAGlobals.dont_care >> 24) | (IU8)(GDP->VGAGlobals.colour_comp >> 24) ^ GDP->VGAGlobals.VGA_rplane[offs])
#define M1WORD(offs) ((IU16)(GDP->VGAGlobals.dont_care >> 16) | (IU16)(GDP->VGAGlobals.colour_comp >> 16) ^ *((IU16*)&GDP->VGAGlobals.VGA_rplane[offs]))
#define M1DWORD(offs) (GDP->VGAGlobals.dont_care | GDP->VGAGlobals.colour_comp ^ *((IU32*)&GDP->VGAGlobals.VGA_rplane[offs]))

#define M1CH2(offs) (M1BYTE(offs) & M1BYTE((offs) + 2))
// x should be M1DWORD(offs), didn't create one macro to ensure that it also gets a bit optimized in -O0 builds
#define M1UNC(x) ((x)>>24)&((x)>>16)&((x)>>8)&(x)

/**** Masks ****/

#define NOMSKX(x) (x)
#define NOMSKL(x,latches) (x)
#define NOMSK(x) NOMSKX(GDP->VGAGlobals.latches)

#define UCBMSKL(x,latches) ( latches & \
  (GDP->VGAGlobals.data_xor_mask  ^ x & GDP->VGAGlobals.data_and_mask) | x & \
  (GDP->VGAGlobals.latch_xor_mask ^ latches))
#define UCBMSK(x) UCBMSKL(x,GDP->VGAGlobals.latches)

#define UCB3MSKL(x,latches) ((GDP->VGAGlobals.bit_prot_mask & (x)) & \
  (latches ^ GDP->VGAGlobals.v7_fg_latches) | latches & \
  ~(GDP->VGAGlobals.bit_prot_mask & (x)))
#define UCB3MSK(x) UCB3MSKL(x,GDP->VGAGlobals.latches)

#define UCB4MSKL(x,latches) ((GDP->VGAGlobals.bit_prot_mask & (x)) | latches &  ~(GDP->VGAGlobals.bit_prot_mask))
#define UCB4MSK(x) UCB4MSKL(x,GDP->VGAGlobals.latches)

#define UCB5MSKD(x) (GDP->VGAGlobals.sr_masked_val | GDP->VGAGlobals.sr_nmask & (x))
#define UCB5MSKL(x,latches) (latches ^ UCB5MSKD(x))
#define UCB5MSK(x) UCB5MSKL(x,GDP->VGAGlobals.latches)
#define UCB5MSKDL(x,latches) UCB5MSKD(x)

#define UCB45MSKL(x,latches) UCB4MSKL(UCB5MSKD(x),latches)
#define UCB45MSK(x) UCB45MSKL(x,GDP->VGAGlobals.latches)
#define UCB15MSKL(x,latches) UCBMSKL(UCB5MSKD(x),latches)
#define UCB15MSK(x) UCB15MSKL(x,GDP->VGAGlobals.latches)

#define UCB6MSKL(x,latches) (GDP->VGAGlobals.sr_masked_val & (GDP->VGAGlobals.bit_prot_mask & (x))  | latches &  ~(GDP->VGAGlobals.bit_prot_mask & (x)))
#define UCB6MSK(x) UCB6MSKL(x,GDP->VGAGlobals.latches)
#define UCB7MSKL(x,latches) ((GDP->VGAGlobals.bit_prot_mask & (x)) & \
  (latches ^ GDP->VGAGlobals.sr_masked_val) | latches & \
  ~(GDP->VGAGlobals.bit_prot_mask & (x)))
#define UCB7MSK(x) UCB7MSKL(x,GDP->VGAGlobals.latches)


/******************************************************************
 * PLANE WRITES                                                   *
 ******************************************************************/
#define UCBWRTWPL(offs,x) *(IU32*)&GDP->VGAGlobals.VGA_wplane[offs] = GDP->VGAGlobals.plane_enable_mask & (x) | *(IU32*)&GDP->VGAGlobals.VGA_wplane[offs] & ~GDP->VGAGlobals.plane_enable_mask
#define UCBWRTWPD(offs,x) *(IU32*)&GDP->VGAGlobals.VGA_wplane[offs] = (x)
// Chain 2 mode X
#define C2MXWRTWPL(offs,x) \
  if (offs & 1) \
  { \
    if (GDP->VGAGlobals.plane_enable & 2) \
      GDP->VGAGlobals.VGA_wplane[2 * (offs & 0x7FFFFFFE) + 1] = (IU8)(x); \
  } \
  else if (GDP->VGAGlobals.plane_enable & 1) \
  { \
    GDP->VGAGlobals.VGA_wplane[2 * (offs & 0x7FFFFFFE)] = (IU8)(x); \
  }
#define C2CPWRTWPL(offs,x) GDP->VGAGlobals.VGA_wplane[2 * (offs & 0x7FFFFFFE) + (offs & 1)] = (IU8)(x)


/******************************************************************
 * MARK                                                           *
 ******************************************************************/

 /*
 * Given an offset into CGA memory return the offset
 * within an 8K bank of video memory.
 */
#define BANK_OFFSET(off) ((off) & 0xDFFF)

#define CGA_MARK_STRING(count,sz) \
{ \
  register	int	offset = BANK_OFFSET(eaOff); \
  if (offset < GDP->VGAGlobals.dirty_low ) GDP->VGAGlobals.dirty_low  = offset; \
  offset = BANK_OFFSET(eaOff + count * sz); \
  if (offset > GDP->VGAGlobals.dirty_high) GDP->VGAGlobals.dirty_high = offset; \
  GDP->VGAGlobals.dirty_total += count; \
}


/*
 * Inline mark macros — replace the per-write indirect function-pointer
 * call through GDP->VGAGlobals.mark_byte/word/string with a switch on
 * the small ev_mark_type tag (set once at mode-change by setMarkPointers
 * in ev_glue.c). Bodies mirror S_2126_SimpleMark / S_2127_CGAMarkByte /
 * S_2128_CGAMarkWord / S_2130_CGAMarkString / S_2131_UnchainedMarkByte /
 * S_2132_UnchainedMarkWord / S_2134_UnchainedMarkString exactly.
 * UNCHAINED_MARK and CHAIN_4_MARK share a body (Chain4 originally just
 * delegated to Unchained — S_2135_Chain4MarkByte calls S_2131_UnchainedMarkByte).
 *
 * The mark constants SIMPLE_MARK / CGA_MARK / UNCHAINED_MARK / CHAIN_4_MARK
 * come from evid_c.h. ev_mark_type lives outside the GDP struct because
 * the J-code-derived gdpvar.h uses byte-offset macros (not a real C
 * typedef), making it awkward to add fields cleanly to VGAGlobals.
 * The cost is identical at runtime: one flat global load vs one
 * struct-indirection load.
 */
IMPORT IU8 ev_mark_type;

#define EVID_MARK_BYTE(off) \
   do { switch (ev_mark_type) { \
   case SIMPLE_MARK: \
      GDP->VGAGlobals.dirty_total++; break; \
   case CGA_MARK: { \
      int _o = BANK_OFFSET(off); \
      if (_o < GDP->VGAGlobals.dirty_low ) GDP->VGAGlobals.dirty_low  = _o; \
      if (_o > GDP->VGAGlobals.dirty_high) GDP->VGAGlobals.dirty_high = _o; \
      GDP->VGAGlobals.dirty_total++; break; \
   } \
   case UNCHAINED_MARK: \
   case CHAIN_4_MARK: { \
      IS32 _o = GDP->VGAGlobals.v7_bank_vid_copy_off + ((IU32)(off) >> 2); \
      GDP->VGAGlobals.dirty_total++; \
      if (_o < GDP->VGAGlobals.dirty_low ) GDP->VGAGlobals.dirty_low  = _o; \
      if (_o > GDP->VGAGlobals.dirty_high) GDP->VGAGlobals.dirty_high = _o; \
      GDP->VGAGlobals.video_copy[_o] = 1; break; \
   } \
   } } while (0)

#define EVID_MARK_WORD(off) \
   do { switch (ev_mark_type) { \
   case SIMPLE_MARK: \
      GDP->VGAGlobals.dirty_total++; break; \
   case CGA_MARK: { \
      int _o = BANK_OFFSET(off); \
      if (_o < GDP->VGAGlobals.dirty_low ) GDP->VGAGlobals.dirty_low  = _o; \
      if (_o > GDP->VGAGlobals.dirty_high) GDP->VGAGlobals.dirty_high = _o + 1; \
      GDP->VGAGlobals.dirty_total += 2; break; \
   } \
   case UNCHAINED_MARK: \
   case CHAIN_4_MARK: { \
      IU32 _off = (off); \
      IS32 _o = GDP->VGAGlobals.v7_bank_vid_copy_off + (_off >> 2); \
      GDP->VGAGlobals.dirty_total += 2; \
      if (_o < GDP->VGAGlobals.dirty_low ) GDP->VGAGlobals.dirty_low  = _o; \
      GDP->VGAGlobals.video_copy[_o] = 1; \
      _o = GDP->VGAGlobals.v7_bank_vid_copy_off + ((_off + 1) >> 2); \
      if (_o > GDP->VGAGlobals.dirty_high) GDP->VGAGlobals.dirty_high = _o; \
      GDP->VGAGlobals.video_copy[_o] = 1; break; \
   } \
   } } while (0)

#define EVID_MARK_STRING(off, count) \
   do { switch (ev_mark_type) { \
   case SIMPLE_MARK: \
      /* matches S_2126_SimpleMark (single bump regardless of count) */ \
      GDP->VGAGlobals.dirty_total++; break; \
   case CGA_MARK: { \
      IU32 _off = (off); \
      IU32 _c = (count); \
      int _o = BANK_OFFSET(_off); \
      if (_o < GDP->VGAGlobals.dirty_low ) GDP->VGAGlobals.dirty_low  = _o; \
      _o = BANK_OFFSET(_off + _c); \
      if (_o > GDP->VGAGlobals.dirty_high) GDP->VGAGlobals.dirty_high = _o; \
      GDP->VGAGlobals.dirty_total += _c; break; \
   } \
   case UNCHAINED_MARK: \
   case CHAIN_4_MARK: { \
      IU32 _off = (off); \
      IU32 _c = (count); \
      IS32 _olo = GDP->VGAGlobals.v7_bank_vid_copy_off + (_off >> 2); \
      IS32 _ohi = GDP->VGAGlobals.v7_bank_vid_copy_off + ((_off + _c) >> 2); \
      GDP->VGAGlobals.dirty_total += _c; \
      if (_olo < GDP->VGAGlobals.dirty_low ) GDP->VGAGlobals.dirty_low  = _olo; \
      if (_ohi > GDP->VGAGlobals.dirty_high) GDP->VGAGlobals.dirty_high = _ohi; \
      while (_ohi >= _olo) GDP->VGAGlobals.video_copy[_ohi--] = 1; break; \
   } \
   } } while (0)


/******************************************************************
 * FILL                                                           *
 ******************************************************************/

/*-----------------+
 |   Unchained     |
 +-----------------*/
#define UCBPLNBFLL(eaOff,data,count) \
  if ( GDP->VGAGlobals.plane_enable & 1 ) \
    S_2153_Unchained1PlaneByteFill(eaOff + 0, (IU8)data, count); \
  if ( GDP->VGAGlobals.plane_enable & 2 ) \
    S_2153_Unchained1PlaneByteFill(eaOff + 1, (IU8)(data>>8), count); \
  if ( GDP->VGAGlobals.plane_enable & 4 ) \
    S_2153_Unchained1PlaneByteFill(eaOff + 2, (IU8)(data>>16), count); \
  if ( GDP->VGAGlobals.plane_enable & 8 ) \
    S_2153_Unchained1PlaneByteFill(eaOff + 3, (IU8)(data>>24), count);

#define UCBPLNWFLL(eaOff,data,count) \
  if ( GDP->VGAGlobals.plane_enable & 1 ) \
    S_2159_Unchained1PlaneWordFill(eaOff + 0, (IU8)data, (IU8)data, count); \
  if ( GDP->VGAGlobals.plane_enable & 2 ) \
    S_2159_Unchained1PlaneWordFill(eaOff + 1, (IU8)(data>>8), (IU8)(data>>8), count); \
  if ( GDP->VGAGlobals.plane_enable & 4 ) \
    S_2159_Unchained1PlaneWordFill(eaOff + 2, (IU8)(data>>16), (IU8)(data>>16), count); \
  if ( GDP->VGAGlobals.plane_enable & 8 ) \
    S_2159_Unchained1PlaneWordFill(eaOff + 3, (IU8)(data>>24), (IU8)(data>>24), count);

// Not sure what this is good for, the arg is just 1 byte anyway, so the masked count is good for nothing?
#define UCBPLNBFLL1(eaOff,data,count) \
  if ( GDP->VGAGlobals.plane_enable & 1 ) \
    S_2153_Unchained1PlaneByteFill(eaOff + 0, (IU8)data | (count & 0xFFFFFF00), count); \
  if ( GDP->VGAGlobals.plane_enable & 2 ) \
    S_2153_Unchained1PlaneByteFill(eaOff + 1, (IU8)(data>>8) | (count & 0xFFFFFF00), count); \
  if ( GDP->VGAGlobals.plane_enable & 4 ) \
    S_2153_Unchained1PlaneByteFill(eaOff + 2, (IU8)(data>>16) | (count & 0xFFFFFF00), count); \
  if ( GDP->VGAGlobals.plane_enable & 8 ) \
    S_2153_Unchained1PlaneByteFill(eaOff + 3, (IU8)(data>>24) | (count & 0xFFFFFF00), count);


#define UCBPLNWFLL1(eaOff,datalo,datahi,count) \
  if ( GDP->VGAGlobals.plane_enable & 1 ) \
    S_2159_Unchained1PlaneWordFill(eaOff + 0, (IU8)datalo       | ((count<<1) & 0xFFFFFF00), (IU8)datahi       | (count & 0xFFFFFF00), count); \
  if ( GDP->VGAGlobals.plane_enable & 2 ) \
    S_2159_Unchained1PlaneWordFill(eaOff + 1, (IU8)(datalo>>8)  | ((count<<1) & 0xFFFFFF00), (IU8)(datahi>>8)  | (count & 0xFFFFFF00), count); \
  if ( GDP->VGAGlobals.plane_enable & 4 ) \
    S_2159_Unchained1PlaneWordFill(eaOff + 2, (IU8)(datalo>>16) | ((count<<1) & 0xFFFFFF00), (IU8)(datahi>>16) | (count & 0xFFFFFF00), count); \
  if ( GDP->VGAGlobals.plane_enable & 8 ) \
    S_2159_Unchained1PlaneWordFill(eaOff + 3, (IU8)(datalo>>24) | ((count<<1) & 0xFFFFFF00), (IU8)(datahi>>24) | (count & 0xFFFFFF00), count);

#define UCBFLLF(func,mask,trans,wrt) \
{ \
  IU32 data; \
  ENTER_FUNC(func); \
  EVID_MARK_STRING(eaOff, count); \
  data = mask(trans(eaVal)); \
  wrt(4 * eaOff,data,count); \
}

#define UCWFLLF(func,mask,trans,wrt) \
{ \
  IU32 datalo, datahi; \
  ENTER_FUNC(func); \
  EVID_MARK_STRING(eaOff, 2 * count); \
  datalo = mask(trans((IU8)eaVal)); \
  datahi = mask(trans((IU8)(eaVal>>8))); \
  wrt(4 * eaOff,datalo,datahi,count); \
}

/*
 * Unchained dword fill dispatch macros -- pick one and pass as `dispatch`
 * argument to UCDFLLF, mirroring how UCWFLLF picks between the bulk
 * S_2319_Unchained4PlaneWordFill and the per-plane UCBPLNWFLL1.
 *
 * Both variants fill `count` consecutive CPU dwords (= 4 * count bytes)
 * at wplane offset eaOffx4 (which is 4 * cpu_col).  The four IU32 args
 * b0..b3 are the plane-broadcast, mask/trans-processed values for the
 * four byte lanes of the source dword.
 *
 * UCDFLL_4P: inlined 4-plane bulk path, used when map_mask == 0xF.
 *            Writes 4 wplane dwords per iteration directly in the caller.
 *            Assumes mask() returns a value that already accounts for
 *            all four planes (as UCBMSK does when plane_mask = 0xFFFFFFFF).
 * UCDFLL_PLN: per-plane dispatch via S_3069_Unchained1PlaneDwordFill for
 *             each enabled plane.  Used when only some planes are enabled;
 *             skipping disabled planes avoids corrupting them with stale
 *             latch bytes (which mask() would return for those planes).
 *
 * Replaces the old UCDFLLF that split a dword fill into two word-fill
 * calls at eaOff and eaOff + 8, which was wrong on two axes: (1) the
 * high-word CPU offset should be +2 not +8, and (2) word-fill uses a
 * 2-byte stride but REP STOSD needs 4-byte stride.  Same fundamental
 * shape as the C4DFLL bug fixed for Chain-4.  Not on GVFM's path (which
 * is Chain 4), but any planar-EGA app using REP STOSD (Keen4-style byte
 * scrolling, some Windows-3.1 VESA planar drivers) would hit it.
 */
#define UCDFLL_4P(eaOffx4, b0, b1, b2, b3, count) \
  do { \
    IU32 *_d = (IU32*)&GDP->VGAGlobals.VGA_wplane[eaOffx4]; \
    IU32 _n = (count); \
    while (_n--) { \
      _d[0] = (b0); _d[1] = (b1); _d[2] = (b2); _d[3] = (b3); \
      _d += 4; \
    } \
  } while (0)

#define UCDFLL_PLN(eaOffx4, b0, b1, b2, b3, count) \
  do { \
    if (GDP->VGAGlobals.plane_enable & 1) \
      S_3069_Unchained1PlaneDwordFill((eaOffx4) + 0, \
        (IU8) (b0),        (IU8) (b1),        (IU8) (b2),        (IU8) (b3),        (count)); \
    if (GDP->VGAGlobals.plane_enable & 2) \
      S_3069_Unchained1PlaneDwordFill((eaOffx4) + 1, \
        (IU8)((b0) >>  8), (IU8)((b1) >>  8), (IU8)((b2) >>  8), (IU8)((b3) >>  8), (count)); \
    if (GDP->VGAGlobals.plane_enable & 4) \
      S_3069_Unchained1PlaneDwordFill((eaOffx4) + 2, \
        (IU8)((b0) >> 16), (IU8)((b1) >> 16), (IU8)((b2) >> 16), (IU8)((b3) >> 16), (count)); \
    if (GDP->VGAGlobals.plane_enable & 8) \
      S_3069_Unchained1PlaneDwordFill((eaOffx4) + 3, \
        (IU8)((b0) >> 24), (IU8)((b1) >> 24), (IU8)((b2) >> 24), (IU8)((b3) >> 24), (count)); \
  } while (0)

#define UCDFLLF(func, mask, trans, dispatch) \
{ \
  IU32 _b0, _b1, _b2, _b3; \
  ENTER_FUNC(func); \
  EVID_MARK_STRING(eaOff, 4 * count); \
  _b0 = mask(trans((IU8)(eaVal      ))); \
  _b1 = mask(trans((IU8)(eaVal >>  8))); \
  _b2 = mask(trans((IU8)(eaVal >> 16))); \
  _b3 = mask(trans((IU8)(eaVal >> 24))); \
  dispatch(4 * eaOff, _b0, _b1, _b2, _b3, count); \
}


/*-----------------+
 | Chain 2 Mode X  |
 +-----------------*/
#define C2MXBFLLF(func,mask,trans,wrt) \
{\
  IU32 data; \
  ENTER_FUNC(func) \
  data = mask(trans(eaVal)); \
  if (eaOff & 1) \
  { \
    count--; \
    wrt(eaOff, data); \
    eaOff+=3; \
  } \
  while (count>1) \
  { \
    wrt(eaOff, data); \
    wrt(eaOff + 1, data); \
    eaOff +=4; \
    count -= 2; \
  } \
  if (count == 1) wrt(eaOff, data); \
}

// Chain 2 Mode X word fill
#define C2MXWFLLF(func,mask,trans,wrt,wrtw) \
  ENTER_FUNC(func); \
  eaVal = mask(trans(eaVal)); \
  if (eaOff & 1) \
  { \
    wrt(eaOff, eaVal); \
    count--; \
    eaOff--; \
    wrt(eaOff + 4 * count - 1, eaVal>>8); \
    eaOff += 4; \
    eaVal = (IU16)(eaVal<<8) | (IU8)(eaVal>>8); \
  } \
  while (count--) \
  { \
    wrtw(eaOff, eaVal); \
    eaOff += 4; \
  }

#define C2MXDFLLF(func,mask,trans,wrt,wrtw) \
  ENTER_FUNC(func); \
  eaVal = mask(trans(eaVal)); \
  if (eaOff & 1) \
  { \
    wrt(eaOff, eaVal); \
    count--; \
    wrt(eaOff + 8 * count - 1, eaVal>>24); \
    wrtw(eaOff + 3, eaVal & 0xFFFF0000 | eaVal >> 24); \
    eaOff += 7; \
    eaVal = (IU16)(eaVal<<8) | (IU8)(eaVal>>8) | (eaVal>>24<<16); \
  } \
  while (count--) \
  { \
    wrtw(eaOff, eaVal); \
    wrtw(eaOff + 4, eaVal & 0xFFFF0000 | (eaVal>>16)); \
    eaOff += 8; \
  }

/*-----------------+
 |    Chain 4      |
 +-----------------*/
#define C4BFLL(func, mask, trans) \
  ENTER_FUNC(func); \
  EVID_MARK_STRING(eaOff, count); \
  eaVal = mask(trans(eaVal)); \
  if (GDP->VGAGlobals.plane_enable & (1 << ((eaOff + 0) & 3))) \
    S_2745_Chain41PlaneByteFill(eaOff + 0, (IU8)eaVal, count/4); \
  if (GDP->VGAGlobals.plane_enable & (1 << ((eaOff + 1) & 3))) \
    S_2745_Chain41PlaneByteFill(eaOff + 1, (IU8)eaVal, count/4); \
  if (GDP->VGAGlobals.plane_enable & (1 << ((eaOff + 2) & 3))) \
    S_2745_Chain41PlaneByteFill(eaOff + 2, (IU8)eaVal, count/4); \
  if (GDP->VGAGlobals.plane_enable & (1 << ((eaOff + 3) & 3))) \
    S_2745_Chain41PlaneByteFill(eaOff + 3, (IU8)eaVal, count/4); 
// ^^ NB: eaval+3 is +2 again in S_2744_Chain4ByteFill_00000000_00000008 ???

#define C4BFLL1(func, mask, trans) \
  ENTER_FUNC(func); \
  EVID_MARK_STRING(eaOff, count); \
  S_2747_Chain44PlaneByteFill(eaOff, (IU8)mask(trans((IU32)eaVal)), count);

/*
 * Chain-4 per-plane word fill.  The old form passed count/4 to each of
 * four S_2745 per-plane byte-fill calls, which is wrong on multiple axes:
 *   - For REP STOSW count=N words = 2N bytes, each plane should get ~N/2
 *     bytes (not N/4).  So half of every fill was silently dropped.
 *   - For N=1 word, count/4 = 0 -> no bytes written at all.
 *   - For unaligned eaOff, plane assignments rotate and the four fixed
 *     if-blocks over-count some planes and under-count others.
 * Fix: iterate `count` words, writing each word's two bytes to their
 * respective planes (based on (offset & 3)) if that plane is enabled.
 * Correct for any count, any alignment, any plane_enable pattern.
 * Not on GVFM's path (GVFM uses C4WFLL1, the 4-plane bulk variant), but
 * cleans up analogous bug shape to C4DFLL/UCDFLLF.
 */
#define C4WFLL(func, mask, trans) \
{ \
  IU8 _lo, _hi; \
  IU32 _o, _n; \
  ENTER_FUNC(func); \
  EVID_MARK_STRING(eaOff, 2 * count); \
  _lo = (IU8)mask(trans((IU8)(eaVal   ))); \
  _hi = (IU8)mask(trans((IU8)(eaVal>>8))); \
  _o = eaOff; \
  _n = count; \
  while (_n--) { \
    if (GDP->VGAGlobals.plane_enable & (1 << (_o & 3))) \
      GDP->VGAGlobals.VGA_wplane[_o] = _lo; \
    if (GDP->VGAGlobals.plane_enable & (1 << ((_o + 1) & 3))) \
      GDP->VGAGlobals.VGA_wplane[_o + 1] = _hi; \
    _o += 2; \
  } \
}

#define C4WFLL1(func,mask,trans) \
  ENTER_FUNC(func); \
  EVID_MARK_STRING(eaOff, 2 * count); \
  S_2796_Chain44PlaneWordFill(eaOff, (IU8)mask(trans((IU8)(eaVal))), (IU8)mask(trans((IU8)(eaVal>>8))), count);

/*
 * Chain-4 dword fill.  The old shape "call word-fill twice at +0 and +2"
 * was correct only for count == 1: for count > 1 the two word-fills
 * overlap (word-fill uses 2-byte stride but REP STOSD needs 4-byte
 * stride), so bytes at [X+2, X+3] get overwritten with the high word
 * value from the second call, [X+4, X+5] wrongly get (b2, b3) instead
 * of (b0, b1), and the tail bytes [X + 4*count - 2, X + 4*count - 1]
 * are never written at all.
 *
 * Fix: mirror C4BFLL's per-plane-byte-fill dispatch, but with a distinct
 * byte value per plane (the four byte lanes of the source dword).  Each
 * of the four planes gets `count` bytes at stride 4 via the existing
 * S_2745_Chain41PlaneByteFill primitive.  Total bytes written per plane
 * = count, total across all four planes = 4 * count = matches REP STOSD.
 *
 * Signature changed from (func, wrt) to (func, mask, trans) so mask/trans
 * are supplied directly at the call site, matching UCBFLLF / UCWFLLF /
 * C4BFLL / C4WFLL / C4WFLL1 style.  The `wrt` word-fill helper is no
 * longer needed.
 *
 * Regression this fixed: GVFM (Turbo-Vision-style DPMI app in VESA
 * 800x600x256 packed, i.e. Chain 4) rendered with blank / shifted /
 * duplicated regions because its REP STOSD-based rectangle clears hit
 * the buggy C4DFLL path.
 */
#define C4DFLL(func, mask, trans) \
{ \
  IU32 _v; \
  IU8  _b0, _b1, _b2, _b3; \
  ENTER_FUNC(func); \
  EVID_MARK_STRING(eaOff, 4 * count); \
  _v  = eaVal; \
  _b0 = (IU8)mask(trans((IU8)(_v      ))); \
  _b1 = (IU8)mask(trans((IU8)(_v >>  8))); \
  _b2 = (IU8)mask(trans((IU8)(_v >> 16))); \
  _b3 = (IU8)mask(trans((IU8)(_v >> 24))); \
  if (GDP->VGAGlobals.plane_enable & (1 << ((eaOff + 0) & 3))) \
    S_2745_Chain41PlaneByteFill(eaOff + 0, _b0, count); \
  if (GDP->VGAGlobals.plane_enable & (1 << ((eaOff + 1) & 3))) \
    S_2745_Chain41PlaneByteFill(eaOff + 1, _b1, count); \
  if (GDP->VGAGlobals.plane_enable & (1 << ((eaOff + 2) & 3))) \
    S_2745_Chain41PlaneByteFill(eaOff + 2, _b2, count); \
  if (GDP->VGAGlobals.plane_enable & (1 << ((eaOff + 3) & 3))) \
    S_2745_Chain41PlaneByteFill(eaOff + 3, _b3, count); \
}



/******************************************************************
 * COPY                                                           *
 ******************************************************************/

/*-----------------+
 |   Unchained     |
 +-----------------*/
#define UCBPLNCPY(func,cpy) \
  ENTER_FUNC(func) \
  if ( GDP->VGAGlobals.plane_enable & 1 ) \
    cpy(eaOff + 0, fromOff, destOff, count, srcInRAM, 0); \
  if ( GDP->VGAGlobals.plane_enable & 2 ) \
    cpy(eaOff + 1, fromOff, destOff, count, srcInRAM, 8); \
  if ( GDP->VGAGlobals.plane_enable & 4 ) \
    cpy(eaOff + 2, fromOff, destOff, count, srcInRAM, 16); \
  if ( GDP->VGAGlobals.plane_enable & 8 ) \
    cpy(eaOff + 3, fromOff, destOff, count, srcInRAM, 24);
#define UCWPLNCPY UCBPLNCPY

// Copy Byte1 Plane Unchained
#define UCBCPYB1PLNC(funcnum,dir,shf,trans,macro,cnt) \
{ \
  IU32 data, *rplane; \
  IU8 *dest; \
  ENTER_FUNC(funcnum); \
  count = cnt; \
  if ( srcInRAM ) \
  { \
    dest = &GDP->VGAGlobals.VGA_wplane[eaOff]; \
    while(count--) \
    { \
      data = trans(*((IU8*)fromOff)); \
      if (shf && shift<32) data <<= shift; \
      data = macro(data, GDP->VGAGlobals.latches); \
      if (shift<32) data >>= shift; \
      *dest = (IU8)data; \
      dest = dest dir * 4; \
      fromOff = fromOff RAM_DIR * (dir); \
    } \
  } \
  else \
  { \
    dest = &GDP->VGAGlobals.VGA_wplane[eaOff]; \
    rplane = (IU32*)&GDP->VGAGlobals.VGA_rplane[destOff]; \
    while(count--) \
    { \
      data = trans(*((IU8*)fromOff)); \
      if (shf && shift<32) data <<= shift; \
      data = macro(data, *rplane); \
      if (shift<32) data >>= shift; \
      *dest = (IU8)data; \
      dest = dest dir * 4; \
      fromOff = fromOff dir; \
      rplane = rplane dir; \
    } \
  } \
}
#define UCBCPYB1PLN(funcnum,dir,shf,trans,macro)  UCBCPYB1PLNC(funcnum,dir,shf,trans,macro,count)
#define UCWCPYW1PLN(funcnum,dir,shf,trans,macro)  UCBCPYB1PLNC(funcnum,dir,shf,trans,macro,count*2)

// Copy Byte4 Plane Unchained
#define UCBCPYB4PLNC(funcnum,dir,trans,macro,dstsz,cnt) \
{ \
  IU32 data, *rplane; \
  dstsz *dest; \
  ENTER_FUNC(funcnum); \
  count = cnt; \
  if ( srcInRAM ) \
  { \
    dest = (dstsz *)&GDP->VGAGlobals.VGA_wplane[eaOff]; \
    while(count--) \
    { \
      data = trans(*((IU8*)fromOff)); \
      *dest = macro(data, GDP->VGAGlobals.latches); \
      dest = dest dir; \
      fromOff = fromOff RAM_DIR * (dir); \
    } \
  } \
  else \
  { \
    dest = (dstsz *)&GDP->VGAGlobals.VGA_wplane[eaOff]; \
    rplane = (IU32*)&GDP->VGAGlobals.VGA_rplane[destOff]; \
    while(count--) \
    { \
      data = trans(*((IU8*)fromOff)); \
      *dest = macro(data, *rplane); \
      dest = dest dir; \
      fromOff = fromOff dir; \
      rplane = rplane dir; \
    } \
  } \
}
#define UCBCPYB4PLN(funcnum,dir,trans,macro)  UCBCPYB4PLNC(funcnum,dir,trans,macro,IU32,count)
#define UCWCPYW4PLN(funcnum,dir,trans,macro)  UCBCPYB4PLNC(funcnum,dir,trans,macro,IU32,count*2)


/*-----------------+
 |    Chain 4      |
 +-----------------*/
#define C4WPLNCPY(func,cpy) \
  ENTER_FUNC(func) \
  cpy(eaOff, fromOff, destOff, 2 * count, srcInRAM);

#define C4CPYB4PLNC1(funcnum,dir,trans,macro,count) \
{ \
  IU32 data, *rplane; \
  IU8 *dest; \
  ENTER_FUNC(funcnum); \
  if ( srcInRAM ) \
  { \
    dest = &GDP->VGAGlobals.VGA_wplane[eaOff]; \
    while(count--) \
    { \
      if (GDP->VGAGlobals.plane_enable & (1 << (eaOff & 3))) \
      {\
        data = trans(*((IU8*)fromOff)); \
        *dest = macro(data, GDP->VGAGlobals.latches); \
      } \
      dest = dest dir; \
      fromOff = fromOff RAM_DIR * (dir); \
    } \
  } \
  else \
  { \
    dest = &GDP->VGAGlobals.VGA_wplane[eaOff]; \
    rplane = (IU32*)&GDP->VGAGlobals.VGA_rplane[destOff]; \
    while(count--) \
    { \
      if (GDP->VGAGlobals.plane_enable & (1 << (eaOff & 3))) \
      {\
        data = trans(*((IU8*)fromOff)); \
        *dest = macro(data, *rplane); \
      } \
      dest = dest dir; \
      fromOff = fromOff dir; \
      rplane = rplane dir; \
    } \
  } \
}
#define C4BCPYB4PLN(funcnum,dir,trans,macro)  UCBCPYB4PLNC(funcnum,dir,trans,macro,IU8,count)
#define C4WCPYW4PLN(funcnum,dir,trans,macro)  UCBCPYB4PLNC(funcnum,dir,trans,macro,IU8,count*2)

#define C4BCPYB4PLN1(funcnum,dir,trans,macro)  C4CPYB4PLNC1(funcnum,dir,trans,macro,count)
#define C4WCPYW4PLN1(funcnum,dir,trans,macro)  C4CPYB4PLNC1(funcnum,dir,trans,macro,count*2)


/******************************************************************
 * MOVE                                                           *
 ******************************************************************/

/*-----------------+
 |   Unchained     |
 +-----------------*/
#define UCMOVFW(fact,dir,func,cpf) \
  ENTER_FUNC(func) \
  EVID_MARK_STRING(eaOff + dir - count * dir, count * fact); \
  if ( srcInRAM ) \
    cpf(4 * eaOff, fromOff, -1, count, srcInRAM); \
  else \
  { \
    BUGGY_IN_CVID(GDP->VGAGlobals.scratch = eaOff + dir - count * dir;) \
    GDP->VGAGlobals.fwd_str_read_addr(GDP->VGAGlobals.scratch, fromOff - count * (dir?1:0), count, srcInRAM); \
    cpf(4 * eaOff, GDP->VGAGlobals.scratch, fromOff, count, srcInRAM); \
  }


// Unchained byte move forward
#define UCBMOVFW(func,cpf) UCMOVFW(1,0,func,cpf)

// Unchained byte move backward
#define UCBMOVBW(func,cpf) UCMOVFW(1,1,func,cpf)

// Unchained word move forward
#define UCWMOVFW(func,cpf) UCMOVFW(2,0,func,cpf)


// Unchained word move backward
#define UCWMOVBW(func,cpf) UCMOVFW(2,2,func,cpf)

// Unchained dword word move forward
#define UCDMOVFW(func,cpf) \
  ENTER_FUNC(func) \
  cpf(eaOff, fromOff, 2 * count, srcInRAM);

#define UCDMOVBW(func,cpf) UCDMOVFW(func,cpf)

// -----------

// Unchained byte move forward 1
#define UCMOVFW1(fact,dir,patt,mul,func,fllf,cpf) \
  ENTER_FUNC(func) \
  EVID_MARK_STRING(eaOff + dir - count * dir, count * fact); \
  if ( srcInRAM ) \
    fllf(eaOff + (dir?1:0) - count * dir, patt, count); \
  else \
  { \
    if ( GDP->VGAGlobals.plane_enable & 1 ) \
      cpf(&GDP->VGAGlobals.VGA_wplane[mul * eaOff + 0], &GDP->VGAGlobals.VGA_rplane[mul * fromOff + 0], count); \
    if ( GDP->VGAGlobals.plane_enable & 2 ) \
      cpf(&GDP->VGAGlobals.VGA_wplane[mul * eaOff + 1], &GDP->VGAGlobals.VGA_rplane[mul * fromOff + 1], count); \
    if ( GDP->VGAGlobals.plane_enable & 4 ) \
      cpf(&GDP->VGAGlobals.VGA_wplane[mul * eaOff + 2], &GDP->VGAGlobals.VGA_rplane[mul * fromOff + 2], count); \
    if ( GDP->VGAGlobals.plane_enable & 8 ) \
      cpf(&GDP->VGAGlobals.VGA_wplane[mul * eaOff + 3], &GDP->VGAGlobals.VGA_rplane[mul * fromOff + 3], count); \
  }

#define UCBMOVFW1(func,fllf,cpf) UCMOVFW1(1,0,-1,4,func,fllf,cpf)
#define UCBMOVBW1(func,fllf,cpf) UCMOVFW1(1,1,-1,4,func,fllf,cpf)
#define UCWMOVFW1(func,fllf,cpf) UCMOVFW1(2,0,-1,4,func,fllf,cpf)
#define UCWMOVBW1(func,fllf,cpf) UCMOVFW1(2,2,((count*2)&0xFFFF0000|0xFFFF),4,func,fllf,cpf)


/*-----------------+
 | Chain 2 Mode X  |
 +-----------------*/
#define C2MXBMOV(func,dir,wrf) \
  ENTER_FUNC(func); \
  CGA_MARK_STRING(count,sizeof(IU8)*dir); \
  if (srcInRAM) \
  { \
    if (((eaOff & 1) && dir>0) || (!(eaOff & 1) && dir<0)) \
    { \
      wrf(eaOff, *(IU8*)fromOff); \
      count--; \
      fromOff += RAM_DIR * dir; \
      eaOff+=3 * dir; \
    } \
    while (count>1) \
    { \
      wrf(eaOff,  *(IU8*)fromOff); \
      fromOff += RAM_DIR * dir; \
      wrf(eaOff + 1,  *(IU8*)fromOff); \
      fromOff += RAM_DIR * dir; \
      eaOff += 4 * dir; \
      count-=2; \
    } \
    if (count == 1) \
      wrf(eaOff,  *(IU8*)fromOff); \
  } \
  else \
  { \
    BUGGY_IN_CVID(GDP->VGAGlobals.scratch = eaOff); \
    GDP->VGAGlobals.fwd_str_read_addr(GDP->VGAGlobals.scratch, fromOff, count, srcInRAM); \
    fromOff = dir>0?0:count-1; \
    if (((eaOff & 1) && dir>0) || (!(eaOff & 1) && dir<0)) \
    { \
      wrf(eaOff, GDP->VGAGlobals.scratch[fromOff]); \
      fromOff+=dir; \
      eaOff+=3; \
      count--; \
    } \
    while (count>1) \
    { \
      wrf(eaOff,  GDP->VGAGlobals.scratch[fromOff]); \
      fromOff += dir; \
      wrf(eaOff + (dir),  GDP->VGAGlobals.scratch[fromOff]); \
      fromOff += dir; \
      eaOff += 4 * dir; \
      count-=2; \
    } \
    if (count == 1) \
      wrf(eaOff,  GDP->VGAGlobals.scratch[fromOff]); \
  }

#define C2MXBMOVFWD(func,wrf) C2MXBMOV(func,+1,wrf)
#define C2MXBMOVBWD(func,wrf) C2MXBMOV(func,-1,wrf)

#define C2MXWMOV(func,dir,wrbf,wrwf) \
  ENTER_FUNC(func); \
  CGA_MARK_STRING(count,sizeof(IU16)*(dir)); \
  if (srcInRAM) \
  { \
    if (((eaOff & 1) && dir>0) || (!(eaOff & 1) && dir<0)) \
    { \
      wrbf(eaOff, *(IU8*)(fromOff)); \
      count--; \
      wrbf(eaOff + (4 * count - 1) * (dir), *((IU8*)fromOff + ((count * sizeof(IU16) - 1) * RAM_DIR * (dir)))); \
      fromOff += RAM_DIR * (dir); \
      eaOff+=3 * (dir); \
    } \
    if (RAM_DIR<0) fromOff+=RAM_DIR; \
    while (count) \
    { \
      wrwf(eaOff,  *(IU16*)(fromOff)); \
      fromOff += RAM_DIR * (dir) * (int)sizeof(IU16); \
      count--; \
      eaOff += 4 * (dir); \
    } \
  } \
  else \
  { \
    BUGGY_IN_CVID(GDP->VGAGlobals.scratch = eaOff); \
    GDP->VGAGlobals.fwd_str_read_addr(GDP->VGAGlobals.scratch, fromOff, count, srcInRAM); \
    fromOff = 0; \
    if (((eaOff & 1) && dir>0) || (!(eaOff & 1) && dir<0)) \
    { \
      wrbf(eaOff, GDP->VGAGlobals.scratch[fromOff]); \
      fromOff+=(dir>0?1:-2); \
      count--; \
      wrbf(eaOff + (4 * count - 1) * (dir), GDP->VGAGlobals.scratch[(count * sizeof(IU16) - 1) * (dir)]); \
      eaOff+=(dir>0?2:-4); \
    } \
    while (count) \
    { \
      wrwf(eaOff,  *(IU16*)GDP->VGAGlobals.scratch[fromOff]); \
      eaOff += 4 * (dir); \
      fromOff += (int)sizeof(IU16) * (dir); \
      count--; \
    } \
  }

#define C2MXWMOVFWD(func,wrbf,wrwf) C2MXWMOV(func,+1,wrbf,wrwf)
#define C2MXWMOVBWD(func,wrbf,wrwf) C2MXWMOV(func,-1,wrbf,wrwf)

#define C2MXDMOVFWD UCDMOVFW
#define C2MXDMOVBWD UCDMOVBW

/*-----------------+
 |    Chain 2      |
 +-----------------*/
#define C2CPMOV(func,dir) \
{ \
  IU32 inc, srcinc; \
  IU8 *dst; \
  ENTER_FUNC(func); \
  CGA_MARK_STRING(count,sizeof(IU8)); \
  if (srcInRAM) \
  { \
    if (eaOff & 1) \
    { \
      dst = &GDP->VGAGlobals.VGA_wplane[(2 * eaOff & (~3)) | 1]; \
      inc = dir>0?3:1; \
    } \
    else \
    { \
      dst = &GDP->VGAGlobals.VGA_wplane[2 * eaOff & (~3)]; \
      inc = dir>0?1:3; \
    } \
    while(count--) \
    { \
      *dst = *(IU8*)fromOff; \
      fromOff += RAM_DIR * dir; \
      dst += inc * dir; \
      inc ^= 2; \
    } \
  } \
  else \
  { \
    if (eaOff & 1) \
    { \
      dst = &GDP->VGAGlobals.VGA_wplane[(2 * eaOff & (~1)) | 1]; \
      inc = dir>0?3:1; \
    } \
    else \
    { \
      dst = &GDP->VGAGlobals.VGA_wplane[2 * eaOff & (~3)]; \
      inc = dir>0?1:3; \
    } \
    srcinc = dir>0?2 * (fromOff & 1) + 1:2 * fromOff & 2 ^ 3; \
    while(count--) \
    { \
      *dst = GDP->VGAGlobals.VGA_rplane[fromOff]; \
      fromOff += srcinc * dir; \
      dst += inc * dir; \
      srcinc ^= 2; \
      inc ^= 2; \
    } \
  } \
}

#define C2CPMOVFW(func) C2CPMOV(func,1)
#define C2CPMOVBW(func) C2CPMOV(func,-1)

/*-----------------+
 |    Chain 4      |
 +-----------------*/
#define C4MOV(fact,dir,func,cpf,readfunc) \
  ENTER_FUNC(func) \
  EVID_MARK_STRING(eaOff + dir - count * dir, count * fact); \
  if ( srcInRAM ) \
    cpf(eaOff, fromOff, -1, count, srcInRAM); \
  else \
  { \
    BUGGY_IN_CVID(GDP->VGAGlobals.scratch = eaOff + dir - count * dir); \
    GDP->VGAGlobals.readfunc(GDP->VGAGlobals.scratch, fromOff - count * (dir?1:0), count, srcInRAM); \
    cpf(eaOff, GDP->VGAGlobals.scratch, fromOff, count, srcInRAM); \
  }

#define C4BMOVFW(func,cpf) C4MOV(1,0,func,cpf,fwd_str_read_addr)
#define C4BMOVBW(func,cpf) C4MOV(1,1,func,cpf,bwd_str_read_addr)
#define C4WMOVFW(func,cpf) C4MOV(2,0,func,cpf,fwd_str_read_addr)
#define C4WMOVBW(func,cpf) C4MOV(2,2,func,cpf,bwd_str_read_addr)
#define C4DMOVFW UCDMOVFW
#define C4DMOVBW UCDMOVBW

#define C4BMOVFW1(func,fllf,cpf) UCMOVFW1(1,0,-1,1,func,fllf,cpf)
#define C4BMOVBW1(func,fllf,cpf) UCMOVFW1(1,1,-1,1,func,fllf,cpf)
#define C4WMOVFW1(func,fllf,cpf) UCMOVFW1(2,0,-1,1,func,fllf,cpf)
#define C4WMOVBW1(func,fllf,cpf) UCMOVFW1(2,2,((count*2)&0xFFFF0000|0xFFFF),1,func,fllf,cpf)



/******************************************************************
 * WRITE                                                          *
 ******************************************************************/

/*-----------------+
 |   Unchained     |
 +-----------------*/
#define UCBWRTF(func,mask,trans,wrt) \
{\
  IU32 data; \
  ENTER_FUNC(func); \
  EVID_MARK_BYTE(eaOff); \
  data = mask(trans(eaVal)); \
  wrt(4*eaOff,data); \
}

#define UCWWRTF(func,mask,trans,wrt) \
{\
  IU32 data; \
  ENTER_FUNC(func); \
  EVID_MARK_WORD(eaOff); \
  data = mask(trans((IU8)eaVal)); \
  wrt(4 * (eaOff+0),data); \
  data = mask(trans((IU8)(eaVal>>8))); \
  wrt(4 * (eaOff+1),data); \
}

#define UCDWRTF(func,wrt) \
{\
  IU32 data; \
  ENTER_FUNC(func); \
  wrt(eaOff, (IU16)eaVal); \
  wrt(eaOff + 2, (IU16)(eaVal>>16)); \
}



/*-----------------+
 | Chain 2 Mode X  |
 +-----------------*/
#define C2MXBWRTF(func,mask,trans,wrt) \
  ENTER_FUNC(func) \
  register	int	offset = BANK_OFFSET(eaOff); \
  if (offset < GDP->VGAGlobals.dirty_low ) GDP->VGAGlobals.dirty_low  = offset; \
  if (offset > GDP->VGAGlobals.dirty_high) GDP->VGAGlobals.dirty_high = offset; \
  GDP->VGAGlobals.dirty_total ++; \
  wrt(eaOff, mask(trans(eaVal)));

#define C2MXWWRTF(func,mask,trans,wrt) \
{ \
  IU32 data; \
  ENTER_FUNC(func); \
  data = mask(trans((IU8)eaVal)); \
  wrt(eaOff,data); \
  data = mask(trans((IU8)(eaVal>>8))); \
  if ( eaOff & 1 ) eaOff += 3; else eaOff += 1; \
  wrt(eaOff,data); \
}

#define C2MXDWRTF(func,wrt) \
{ \
  IU32 data; \
  ENTER_FUNC(func); \
  wrt(eaOff, (IU16)eaVal); \
  wrt(eaOff + 4, (IU16)(eaVal>>16)); \
}


/*-----------------+
 |    Chain 4      |
 +-----------------*/
#define C4BWRTFC(mask,trans) \
  GDP->VGAGlobals.VGA_wplane[eaOff] = (IU8)mask(trans(eaVal)); \
  EVID_MARK_BYTE(eaOff);

#define C4BWRTF(func,mask,trans) \
  ENTER_FUNC(func); \
  C4BWRTFC(mask,trans)

#define C4BWRTF1(func,mask,trans) \
  ENTER_FUNC(func); \
  if (GDP->VGAGlobals.plane_enable & (1 << (eaOff & 3))) \
  { C4BWRTFC(mask,trans) } 


#define C4WWRTF(func,mask,trans) \
  ENTER_FUNC(func); \
  GDP->VGAGlobals.VGA_wplane[eaOff + 0] = (IU8)mask(trans((IU8)eaVal)); \
  GDP->VGAGlobals.VGA_wplane[eaOff + 1] = (IU8)mask(trans((IU8)(eaVal>>8))); \
  EVID_MARK_WORD(eaOff);

#define C4WWRTF1(func,mask,trans) \
  ENTER_FUNC(func) \
  if (GDP->VGAGlobals.plane_enable & (1 << (eaOff & 3))) \
    GDP->VGAGlobals.VGA_wplane[eaOff] = (IU8)mask(trans((IU8)eaVal)); \
  eaOff++; \
  if (GDP->VGAGlobals.plane_enable & (1 << (eaOff & 3))) \
  { \
    GDP->VGAGlobals.VGA_wplane[eaOff] = (IU8)mask(trans((IU8)(eaVal>>8))); \
    EVID_MARK_WORD(eaOff); \
  }

#define C4DWRTF(func,wrt) \
  wrt(eaOff, (IU16)eaVal); \
  wrt(eaOff + 2, (IU16)(eaVal>>16));

