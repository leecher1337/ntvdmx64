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
  GDP->VGAGlobals.mark_string(eaOff, count); \
  data = mask(trans(eaVal)); \
  wrt(4 * eaOff,data,count); \
}

#define UCWFLLF(func,mask,trans,wrt) \
{ \
  IU32 datalo, datahi; \
  ENTER_FUNC(func); \
  GDP->VGAGlobals.mark_string(eaOff, 2 * count); \
  datalo = mask(trans((IU8)eaVal)); \
  datahi = mask(trans((IU8)(eaVal>>8))); \
  wrt(4 * eaOff,datalo,datahi,count); \
}

#define UCDFLLF(func,wrt) \
  ENTER_FUNC(func); \
  wrt(eaOff, (IU16)eaVal, count); \
  wrt(eaOff + 8, (IU16)(eaVal>>16), count);


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
  GDP->VGAGlobals.mark_string(eaOff, count); \
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
  GDP->VGAGlobals.mark_string(eaOff, count); \
  S_2747_Chain44PlaneByteFill(eaOff, (IU8)mask(trans((IU32)eaVal)), count);

#define C4WFLL(func, mask, trans) \
{ \
  ENTER_FUNC(func); \
  GDP->VGAGlobals.mark_string(eaOff, count * sizeof(IU16)); \
  if (GDP->VGAGlobals.plane_enable & (1 << ((eaOff + 0) & 3))) \
    S_2745_Chain41PlaneByteFill(eaOff + 0, (IU8)mask(trans((IU8)(eaVal   ))), count/4); \
  if (GDP->VGAGlobals.plane_enable & (1 << ((eaOff + 1) & 3))) \
    S_2745_Chain41PlaneByteFill(eaOff + 1, (IU8)mask(trans((IU8)(eaVal>>8))), count/4); \
  if (GDP->VGAGlobals.plane_enable & (1 << ((eaOff + 2) & 3))) \
    S_2745_Chain41PlaneByteFill(eaOff + 2, (IU8)mask(trans((IU8)(eaVal   ))), count/4); \
  if (GDP->VGAGlobals.plane_enable & (1 << ((eaOff + 3) & 3))) \
    S_2745_Chain41PlaneByteFill(eaOff + 3, (IU8)mask(trans((IU8)(eaVal>>8))), count/4); \
}

#define C4WFLL1(func,mask,trans) \
  ENTER_FUNC(func); \
  GDP->VGAGlobals.mark_string(eaOff, 2 * count); \
  S_2796_Chain44PlaneWordFill(eaOff, (IU8)mask(trans((IU8)(eaVal))), (IU8)mask(trans((IU8)(eaVal>>8))), count);

#define C4DFLL(func,wrt) \
  wrt(eaOff, (IU16)eaVal, count); \
  wrt(eaOff + 2, (IU16)(eaVal>>16), count);



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
  GDP->VGAGlobals.mark_string(eaOff + dir - count * dir, count * fact); \
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
  GDP->VGAGlobals.mark_string(eaOff + dir - count * dir, count * fact); \
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
  GDP->VGAGlobals.mark_string(eaOff + dir - count * dir, count * fact); \
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
  GDP->VGAGlobals.mark_byte(eaOff); \
  data = mask(trans(eaVal)); \
  wrt(4*eaOff,data); \
}

#define UCWWRTF(func,mask,trans,wrt) \
{\
  IU32 data; \
  ENTER_FUNC(func); \
  GDP->VGAGlobals.mark_word(eaOff); \
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
  GDP->VGAGlobals.mark_byte(eaOff);

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
  GDP->VGAGlobals.mark_word(eaOff);

#define C4WWRTF1(func,mask,trans) \
  ENTER_FUNC(func) \
  if (GDP->VGAGlobals.plane_enable & (1 << (eaOff & 3))) \
    GDP->VGAGlobals.VGA_wplane[eaOff] = (IU8)mask(trans((IU8)eaVal)); \
  eaOff++; \
  if (GDP->VGAGlobals.plane_enable & (1 << (eaOff & 3))) \
  { \
    GDP->VGAGlobals.VGA_wplane[eaOff] = (IU8)mask(trans((IU8)(eaVal>>8))); \
    GDP->VGAGlobals.mark_word(eaOff); \
  }

#define C4DWRTF(func,wrt) \
  wrt(eaOff, (IU16)eaVal); \
  wrt(eaOff + 2, (IU16)(eaVal>>16));

