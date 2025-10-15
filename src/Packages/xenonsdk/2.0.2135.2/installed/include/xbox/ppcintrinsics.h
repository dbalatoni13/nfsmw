/*

Copyright (C) Microsoft Corporation 2005.  All rights reserved.

Module Name:

    PPCIntrinsics.h

Abstract:
    Platform-specific intrinsics.
    
*/

#ifndef __PPCINTRINSICS_H__
#define __PPCINTRINSICS_H__

// VMX specific intrinsics
#include "VectorIntrinsics.h"

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////
// Standard emit-any-opcode intrinsic
void __emit(unsigned int opcode);

//////////////////////////////////////////////////////////////////////////////
// load/store with update intrinsics
    
/*
  loadbyteupdate(val, offset, base)
  is equivalent to

  val = *(base + offset);
  base += offset;

  All done in one instruction.  The compiler will store changes to "base"
  back to the variable itself if necessary (it will do so all the time
  in /Od builds).

  Note that the address of the load is exactly base + offset.  Offset is not
  multiplied by the size of the type, it is a byte offset.

  This code will generate the indexed form of the instruction if the
  offset cannot be lowered into an integer constant.
  
 */
unsigned char   __loadbyteupdate    ( int offset, /* INOUT */ void* base );
unsigned short  __loadshortupdate   ( int offset, /* INOUT */ void* base );
unsigned long   __loadwordupdate    ( int offset, /* INOUT */ void* base );

float           __loadfloatupdate   ( int offset, /* INOUT */ void* base );
double          __loaddoubleupdate  ( int offset, /* INOUT */ void* base );    

unsigned __int64 __loaddoublewordupdate ( int offset, /* INOUT */ void* base);

/*
  The reason we have these wrapper macros is because the form of the intrinsics
  may change.  This will insulate developers from that change.
 */
#define loadbyteupdate( val,  offset,  base) val = __loadbyteupdate(offset,base)
#define loadshortupdate( val, offset,  base) val = __loadshortupdate(offset,base)
#define loadwordupdate( val,  offset,  base) val = __loadwordupdate(offset,base)
#define loadfloatupdate( val,  offset,  base) val = __loadfloatupdate(offset,base)    
#define loaddoubleupdate( val,  offset,  base) val = __loaddoubleupdate(offset,base)
#define loaddoublewordupdate( val,  offset,  base) val = __loaddoublewordupdate(offset,base)
    
void __storebyteupdate      ( unsigned char  val,  int offset, /* INOUT */ void* base);
void __storeshortupdate     ( unsigned short val,  int offset, /* INOUT */ void* base);
void __storewordupdate      ( unsigned long  val,  int offset, /* INOUT */ void* base);
void __storefloatupdate     ( float          val,  int offset, /* INOUT */ void* base);
void __storedoubleupdate    ( double         val,  int offset, /* INOUT */ void* base);
void __storedoublewordupdate(unsigned __int64 val, int offset, /* INOUT */ void* base);

#define storebyteupdate __storebyteupdate
#define storeshortupdate __storeshortupdate
#define storewordupdate __storewordupdate
#define storefloatupdate __storefloatupdate
#define storedoubleupdate __storedoubleupdate
#define storedoublewordupdate __storedoublewordupdate


//////////////////////////////////////////////////////////////////////////////
// Cache Control
void        __dcbz128       (int offset, void * base);
void        __dcbz          (int offset, void * base);
void        __dcbt          (int offset, const void * base);
void        __xdcbt         (int offset, const void * base);
void        __dcbst         (int offset, const void * base);
void        __dcbf          (int offset, const void * base);


//////////////////////////////////////////////////////////////////////////////
// Byte reversal
// Note that the non-volatile versions of the intrinsics will not honor volatileness
// even if base is volatile.  
unsigned short  __loadshortbytereverse  (int offset, const void * base);
unsigned long   __loadwordbytereverse   (int offset, const void * base);

void            __storeshortbytereverse (unsigned short val, int offset, void * base);
void            __storewordbytereverse  (unsigned int   val, int offset, void * base);

// These aggressive G-forms only have global interference.  If you use these 
// forms you must not be loading/storing to a stack location.
#if (_MSC_FULL_VER > 14001509)
unsigned short  __loadshortbytereverseG(int offset, const void * base);
unsigned long   __loadwordbytereverseG   (int offset, const void * base);
void            __storeshortbytereverseG (unsigned short val, int offset, void * base);
void            __storewordbytereverseG(unsigned int   val, int offset, void * base);
#else
#define __loadshortbytereverseG     __loadshortbytereverse
#define __loadwordbytereverseG      __loadwordbytereverse
#define __storeshortbytereverseG __storeshortbytereverse
#define __storewordbytereverseG     __storewordbytereverse
#endif

// These volatile intrinsics will never get destroyed by optimizations and will always
// emit their respective instruction
unsigned short  __loadvolatileshortbytereverse  (int offset, const void volatile * base);
unsigned long   __loadvolatilewordbytereverse   (int offset, const void volatile* base);

void            __storevolatileshortbytereverse (unsigned short val, int offset, void volatile* base);
void            __storevolatilewordbytereverse  (unsigned int   val, int offset, void volatile * base);


#if (_MSC_FULL_VER > 14001524)
unsigned short  __loadvolatileshortbytereverseG  (int offset, const void volatile * base);
unsigned long   __loadvolatilewordbytereverseG   (int offset, const void volatile* base);

void            __storevolatileshortbytereverseG (unsigned short val, int offset, void volatile* base);
void            __storevolatilewordbytereverseG  (unsigned int   val, int offset, void volatile * base);
#else
#define __loadvolatileshortbytereverseG     __loadvolatileshortbytereverse
#define __loadvolatilewordbytereverseG      __loadvolatilewordbytereverse

#define __storevolatileshortbytereverseG    __storevolatileshortbytereverse
#define __storevolatilewordbytereverseG     __storevolatilewordbytereverse

#endif

// Standard intrinsics, declared here for completeness
unsigned short      _byteswap_ushort            (unsigned short     value);
unsigned long       _byteswap_ulong             (unsigned long      value);
unsigned __int64    _byteswap_uint64            (unsigned __int64   value);

// multiply high intrinsics
__int64             __mulh(__int64 qwA,__int64 qwB);
unsigned __int64    __umulh(unsigned __int64 qwA,unsigned __int64 qwB);

// Map PowerPC mul high instructions to standard VS mul high intrinsics
#define __mulhd     __mulh
#define __mulhdu    __umulh



//////////////////////////////////////////////////////////////////////////////
// floating point

double  __fsqrt(double fval);
float   __fsqrts(float fval);
double  __frsqrte(double fval);
float   __fres(double fval);
#define __fabs fabs

// if fComparand >= 0, return fValGE else returns fLT.  If fComparand is NaN, 
// returns fLT
double __fsel(double fComparand, double fValGE, double fLT);
#define fpmax(a,b) __fsel((a)-(b), a,b)
#define fpmin(a,b) __fsel((a)-(b), b,a)


#if (_MSC_FULL_VER > 14001727)

// These convert a float/double into an integer, but keep them in the FPR. 
// Store it out as a double, then later on load them in as integers manually 
// to avoid the load-hit-store flush of float conversions.
//
// They must be double so stfd can be used to store the values without any further tampering.
// Storing to misaligned memory is probably cheaper than the LHS flush.

// round to zero
double __fctiwz(double fParam);
double __fctidz(double fParam);
#endif


#if (_MSC_FULL_VER > 14001729)
// round with current round mode
double __fctiw(double fParam);
double __fctid(double fParam);

// convert an int64 bit pattern that is in a FPR to double with 
// current round mode
double __fcfid(double i64Param);
#endif

// floating point round.  uses fctid, fcfid pair to round a double
// to an integer using current rounding mode.  May not work for massively
// large values.
double __frnd(double fRoundee);

//////////////////////////////////////////////////////////////////////////////
// Count leading zeros
unsigned int _CountLeadingZeros64(__int64 val);
unsigned int _CountLeadingZeros (long val);


//////////////////////////////////////////////////////////////////////////////
// Read time base counter.
unsigned __int64 __mftb();

//////////////////////////////////////////////////////////////////////////////
// Synchronization
#define __sync()        __emit(0x7c0004ac)
#define __lwsync()      __emit(0x7C2004AC)
#define __eieio()       __emit(0x7c0006ac)


//////////////////////////////////////////////////////////////////////////////
// Miscellaneous

// Not a PPC specific intrinsic but specific to our compiler.  
// Returns 1 if val is an integer constant at compile time.  This means
// #define'd constants, enumerations and of course integer literals. 
// This does NOT mean static const variables, const variables or any variables 
// that become constant integers as a result of optimizations.  It also does 
// not include addresses of variables or functions.
int     __IsIntConst(int val);

#ifdef __cplusplus
}
#endif

#endif
