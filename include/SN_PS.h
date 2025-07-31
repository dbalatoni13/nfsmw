// ====================================================================================
// SN_PS.h - SN Systems Paired Single support
//
// (c) Copyright 2001, 2002 SN Systems Ltd.
//     All Rights reserved.
//

/* ===============================================================
 *
 *  SN Systems Ltd : File Revision History
 *
 *  $Archive: /Dolphin/SNAPS/SN_PS.h $
 *
 *  Last Modified 
 *  $Date: 1/08/02 17:08 $ by $Author: Phil $
 *
 *  History:
 *
 *  $History: SN_PS.h $
 * 
 * *****************  Version 1  *****************
 * User: Phil         Date: 1/08/02    Time: 17:08
 * Created in $/Dolphin/SNAPS
 * 
 * 
 *  $NoKeywords: $
 *
 * =============================================================== */

#ifndef __SN_PS_H
#define __SN_PS_H

// 
// Some basic Paired Single (PS) definitions
//


// simple PS type
typedef float sn_ps __attribute__((mode(PS))) ; 

// unioned ps with float type

typedef union SN_PS_type
{
  sn_ps ps ;
  struct tflt
  {
    float hi, lo ;
#ifndef __cplusplus
  } flt ;
#else
} ;
  tflt flt ;
#endif
} ;

// Quantization Registers and handling

// the registers
#define GQR0 912
#define GQR1 913
#define GQR2 914
#define GQR3 915
#define GQR4 916
#define GQR5 917
#define GQR6 918
#define GQR7 919

// the type field
#define GQR_SCALE_F32 0x0
#define GQR_SCALE_U8  0x4
#define GQR_SCALE_U16 0x5
#define GQR_SCALE_S8  0x6
#define GQR_SCALE_S16 0x7

// some ps operations

// Build two floats into a PS variable. 
// e.g.  ps = PS_BUILD(0.0f,1.0f) ;
#define PS_BUILD(hi,lo) *(sn_ps*)(float[]){(lo),(hi)}


// =============================================================
// SNAPS - The SN API for PS definitions
//

// 1 result, 1 argument opcodes
#define ps_neg(x)     __builtin_ps_neg(x)
#define ps_abs(x)     __builtin_ps_abs(x)
#define ps_nabs(x)    __builtin_ps_nabs(x)
#define ps_res(x)     __builtin_ps_res(x)
#define ps_rsqrte(x)  __builtin_ps_rsqrte(x)

// 1 result, 2 argument opcodes
#define ps_add(x,y)         __builtin_ps_add(x,y)
#define ps_sub(x,y)         __builtin_ps_sub(x,y)
#define ps_div(x,y)         __builtin_ps_div(x,y)
#define ps_mul(x,y)         __builtin_ps_mul(x,y)
#define ps_merge00(x,y)     __builtin_ps_merge00(x,y)
#define ps_merge01(x,y)     __builtin_ps_merge01(x,y)
#define ps_merge10(x,y)     __builtin_ps_merge10(x,y)
#define ps_merge11(x,y)     __builtin_ps_merge11(x,y)
#define ps_muls0(x,y)       __builtin_ps_muls0(x,y)
#define ps_muls1(x,y)       __builtin_ps_muls1(x,y)

// 1 result, 3 argument opcodes
#define ps_madd(x,y,z)      __builtin_ps_madd(x,y,z)
#define ps_msub(x,y,z)      __builtin_ps_msub(x,y,z)
#define ps_nmadd(x,y,z)     __builtin_ps_nmadd(x,y,z)
#define ps_nmsub(x,y,z)     __builtin_ps_nmsub(x,y,z)
#define ps_sum0(x,y,z)      __builtin_ps_sum0(x,y,z)
#define ps_sum1(x,y,z)      __builtin_ps_sum1(x,y,z)
#define ps_madds0(x,y,z)    __builtin_ps_madds0(x,y,z)
#define ps_madds1(x,y,z)    __builtin_ps_madds1(x,y,z)
#define ps_sel(x,y,z)       __builtin_ps_sel(x,y,z)

// load and stores 
// (NB. Should normally only be used for scaled and/or conversion load/stores. The compiler
//  will generate non-scaled/converted loads and stores for you.)

#define ps_l(v,w,i)         __builtin_psq_l(v,w,i)
#define ps_lu(v,w,i)        __builtin_psq_lu(v,w,i)
#define ps_lx(v,o,w,i)      __builtin_psq_lx(v,o,w,i)
#define ps_lux(v,o,w,i)     __builtin_psq_lux(v,o,w,i)

#define ps_st(d,v,w,i)      __builtin_psq_st(d,v,w,i) 
#define ps_stu(d,v,w,i)     __builtin_psq_stu(d,v,w,i)
#define ps_stx(d,v,o,w,i)   __builtin_psq_stx(d,v,o,w,i)
#define ps_stux(d,v,o,w,i)  __builtin_psq_stux(d,v,o,w,i)

#endif
