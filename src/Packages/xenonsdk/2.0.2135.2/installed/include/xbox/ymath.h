/* ymath.h internal header */
#pragma once
#ifndef _YMATH
#define _YMATH
#include <yvals.h>
_C_STD_BEGIN
_C_LIB_DECL

#ifdef  _MSC_VER
#pragma pack(push,_CRT_PACKING)
#endif  /* _MSC_VER */

		/* MACROS FOR _Dtest RETURN (0 => ZERO) */
#define _DENORM		(-2)	/* C9X only */
#define _FINITE		(-1)
#define _INFCODE	1
#define _NANCODE	2

		/* MACROS FOR _Feraise ARGUMENT */
#define _FE_DIVBYZERO	0x04
#define _FE_INEXACT		0x20
#define _FE_INVALID		0x01
#define _FE_OVERFLOW	0x08
#define _FE_UNDERFLOW	0x10

		/* TYPE DEFINITIONS */
typedef union
	{	/* pun float types as integer array */
	unsigned short _Word[8];
	float _Float;
	double _Double;
	long double _Long_double;
	} _Dconst;

		/* ERROR REPORTING */
void __cdecl _Feraise(int);

		/* double DECLARATIONS */
_MRTIMP2 double __cdecl _Cosh(double, double);
_MRTIMP2 short __cdecl _Dtest(double *);
_MRTIMP2 short __cdecl _Exp(double *, double, short);
_MRTIMP2 double __cdecl _Log(double, int);
_MRTIMP2 double __cdecl _Sin(double, unsigned int);
_MRTIMP2 double __cdecl _Sinh(double, double);
extern _CRTIMP2_PURE /* const */ _Dconst _Denorm, _Hugeval, _Inf,
	_Nan, _Snan;

		/* float DECLARATIONS */
_MRTIMP2 float __cdecl _FCosh(float, float);
_MRTIMP2 short __cdecl _FDtest(float *);
_MRTIMP2 short __cdecl _FExp(float *, float, short);
_MRTIMP2 float __cdecl _FLog(float, int);
_MRTIMP2 float __cdecl _FSin(float, unsigned int);
_MRTIMP2 float __cdecl _FSinh(float, float);
extern _CRTIMP2_PURE /* const */ _Dconst _FDenorm, _FInf, _FNan, _FSnan;

		/* long double DECLARATIONS */
_MRTIMP2 long double __cdecl _LCosh(long double, long double);
_MRTIMP2 short __cdecl _LDtest(long double *);
_MRTIMP2 short __cdecl _LExp(long double *, long double, short);
_MRTIMP2 long double __cdecl _LLog(long double, int);
_MRTIMP2 long double __cdecl _LSin(long double, unsigned int);
_MRTIMP2 long double __cdecl _LSinh(long double, long double);
extern _CRTIMP2_PURE /* const */ _Dconst _LDenorm, _LInf, _LNan, _LSnan;
_END_C_LIB_DECL
_C_STD_END

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif /* _YMATH */

/*
 * Copyright (c) 1992-2004 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
 V4.04:0009 */
