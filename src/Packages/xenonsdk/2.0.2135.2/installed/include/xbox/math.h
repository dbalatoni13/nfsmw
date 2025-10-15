/***
*math.h - definitions and declarations for math library
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file contains constant definitions and external subroutine
*       declarations for the math subroutine library.
*       [ANSI/System V]
*
*       [Public]
*
****/

#ifndef _INC_MATH
#define _INC_MATH

#include <crtdefs.h>

#ifdef  _MSC_VER
/*
 * Currently, all MS C compilers for Win32 platforms default to 8 byte
 * alignment.
 */
#pragma pack(push,_CRT_PACKING)
#endif  /* _MSC_VER */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __assembler /* Protect from assembler */

/* Definition of _exception struct - this struct is passed to the matherr
 * routine when a floating point exception is detected
 */

#ifndef _EXCEPTION_DEFINED
struct _exception {
        int type;       /* exception type - see below */
        char *name;     /* name of function where error occured */
        double arg1;    /* first argument to function */
        double arg2;    /* second argument (if any) to function */
        double retval;  /* value to be returned by function */
        } ;

#define _EXCEPTION_DEFINED
#endif


/* Definition of a _complex struct to be used by those who use cabs and
 * want type checking on their argument
 */

#ifndef _COMPLEX_DEFINED
struct _complex {
        double x,y; /* real and imaginary parts */
        } ;

#if     !__STDC__ && !defined (__cplusplus)
/* Non-ANSI name for compatibility */
#define complex _complex
#endif

#define _COMPLEX_DEFINED
#endif
#endif  /* __assembler */


/* Constant definitions for the exception type passed in the _exception struct
 */

#define _DOMAIN     1   /* argument domain error */
#define _SING       2   /* argument singularity */
#define _OVERFLOW   3   /* overflow range error */
#define _UNDERFLOW  4   /* underflow range error */
#define _TLOSS      5   /* total loss of precision */
#define _PLOSS      6   /* partial loss of precision */

#define EDOM        33
#define ERANGE      34


/* Definitions of _HUGE and HUGE_VAL - respectively the XENIX and ANSI names
 * for a value returned in case of error by a number of the floating point
 * math routines
 */
#ifndef __assembler /* Protect from assembler */
_CRTIMP extern double _HUGE;
#endif  /* __assembler */

#define HUGE_VAL _HUGE

/* Function prototypes */

#if     !defined(__assembler)   /* Protect from assembler */
        int     __cdecl abs(__in int _P(_X));
        double  __cdecl acos(__in double _P(_X));
        double  __cdecl asin(__in double _P(_X));
        double  __cdecl atan(__in double _P(_X));
        double  __cdecl atan2(__in double _P(_Y), __in double _P(_X));
_CRTIMP double __cdecl _copysign (__in double _P(_Number), __in double _P(_Sign));
_CRTIMP double __cdecl _chgsign (__in double _P(_X));
        double  __cdecl cos(__in double _P(_X));
        double  __cdecl cosh(__in double _P(_X));
        double  __cdecl exp(__in double _P(_X));
        double  __cdecl fabs(__in double _P(_X));
        double  __cdecl fmod(__in double _P(_X), __in double _P(_Y));
        long    __cdecl labs(__in long _P(_X));
        double  __cdecl log(__in double _P(_X));
        double  __cdecl log10(__in double _P(_X));
        double  __cdecl pow(__in double _P(_X), __in double _P(_Y));
        double  __cdecl sin(__in double _P(_X));
        double  __cdecl sinh(__in double _P(_X));
        double  __cdecl tan(__in double _P(_X));
        double  __cdecl tanh(__in double _P(_X));
        double  __cdecl sqrt(__in double _P(_X));
_CRTIMP double  __cdecl atof(__in const char *_P(_String));
_CRTIMP double  __cdecl _atof_l(__in const char *_P(_String), __in_opt _locale_t _P(_PtLoci));
_CRTIMP double  __cdecl _cabs(__in struct _complex _P(_Complex));
_CRTIMP double  __cdecl ceil(__in double _P(_X));
_CRTIMP double  __cdecl floor(__in double _P(_X));
_CRTIMP double  __cdecl frexp(__in double _P(_X), __out int * _P(_Y));
_CRTIMP double  __cdecl _hypot(__in double _P(_X), __in double _P(_Y));
_CRTIMP double  __cdecl _j0(__in double _P(_X) );
_CRTIMP double  __cdecl _j1(__in double _P(_X) );
_CRTIMP double  __cdecl _jn(int _P(_X), __in double _P(_Y));
_CRTIMP double  __cdecl ldexp(__in double _P(_X), __in int _P(_Y));
        int     __cdecl _matherr(__inout struct _exception * _P(_Except));
_CRTIMP double  __cdecl modf(__in double _P(_X), __out double * _P(_Y));

_CRTIMP double  __cdecl _y0(__in double _P(_X));
_CRTIMP double  __cdecl _y1(__in double _P(_X));
_CRTIMP double  __cdecl _yn(__in int _P(_X), __in double _P(_Y));






/* Macros defining long double functions to be their double counterparts
 * (long double is synonymous with double in this implementation).
 */

#ifndef __cplusplus
#define acosl(x)        ((long double)acos((double)(x)))
#define asinl(x)        ((long double)asin((double)(x)))
#define atanl(x)        ((long double)atan((double)(x)))
#define atan2l(x,y)     ((long double)atan2((double)(x), (double)(y)))
#define ceill(x)        ((long double)ceil((double)(x)))
#define cosl(x)         ((long double)cos((double)(x)))
#define coshl(x)        ((long double)cosh((double)(x)))
#define expl(x)         ((long double)exp((double)(x)))
#define fabsl(x)        ((long double)fabs((double)(x)))
#define floorl(x)       ((long double)floor((double)(x)))
#define fmodl(x,y)      ((long double)fmod((double)(x), (double)(y)))
#define frexpl(x,y)     ((long double)frexp((double)(x), (y)))
#define _hypotl(x,y)        ((long double)_hypot((double)(x), (double)(y)))
#define ldexpl(x,y)     ((long double)ldexp((double)(x), (y)))
#define logl(x)         ((long double)log((double)(x)))
#define log10l(x)       ((long double)log10((double)(x)))
#define _matherrl       _matherr
#define modfl(x,y)      ((long double)modf((double)(x), (double *)(y)))
#define powl(x,y)       ((long double)pow((double)(x), (double)(y)))
#define sinl(x)         ((long double)sin((double)(x)))
#define sinhl(x)        ((long double)sinh((double)(x)))
#define sqrtl(x)        ((long double)sqrt((double)(x)))
#define tanl(x)         ((long double)tan((double)(x)))
#define tanhl(x)        ((long double)tanh((double)(x)))
#define _chgsignl(x)    ((long double)_chgsign((double)(x)))
#define _copysignl(x,y) ((long double)_copysign((double)(x), (double)(y)))

#define frexpf(x,y) ((float)frexp((double)(x),(y)))

#define fabsf(x)    ((float)fabs((double)(x)))
#define ldexpf(x,y) ((float)ldexp((double)(x),(y)))


#define acosf(x)    ((float)acos((double)(x)))
#define asinf(x)    ((float)asin((double)(x)))
#define atanf(x)    ((float)atan((double)(x)))
#define atan2f(x,y) ((float)atan2((double)(x), (double)(y)))
#define ceilf(x)    ((float)ceil((double)(x)))
#define cosf(x)     ((float)cos((double)(x)))
#define coshf(x)    ((float)cosh((double)(x)))
#define expf(x)     ((float)exp((double)(x)))
#define floorf(x)   ((float)floor((double)(x)))
#define fmodf(x,y)  ((float)fmod((double)(x), (double)(y)))
#define logf(x)     ((float)log((double)(x)))
#define log10f(x)   ((float)log10((double)(x)))
#define modff(x,y)  ((float)modf((double)(x), (double *)(y)))
#define powf(x,y)   ((float)pow((double)(x), (double)(y)))
#define sinf(x)     ((float)sin((double)(x)))
#define sinhf(x)    ((float)sinh((double)(x)))
#define sqrtf(x)    ((float)sqrt((double)(x)))
#define tanf(x)     ((float)tan((double)(x)))
#define tanhf(x)    ((float)tanh((double)(x)))


#else   /* __cplusplus */
inline long double acosl(__in long double _X)
        {return (acos((double)_X)); }
inline long double asinl(__in long double _X)
        {return (asin((double)_X)); }
inline long double atanl(__in long double _X)
        {return (atan((double)_X)); }
inline long double atan2l(__in long double _X, __in long double _Y)
        {return (atan2((double)_X, (double)_Y)); }
inline long double ceill(__in long double _X)
        {return (ceil((double)_X)); }
inline long double cosl(__in long double _X)
        {return (cos((double)_X)); }
inline long double coshl(__in long double _X)
        {return (cosh((double)_X)); }
inline long double expl(__in long double _X)
        {return (exp((double)_X)); }
inline long double fabsl(__in long double _X)
        {return (fabs((double)_X)); }
inline long double floorl(__in long double _X)
        {return (floor((double)_X)); }
inline long double fmodl(__in long double _X, __in long double _Y)
        {return (fmod((double)_X, (double)_Y)); }
inline long double frexpl(__in long double _X, __out int *_Y)
        {return (frexp((double)_X, _Y)); }
inline long double ldexpl(__in long double _X, __in int _Y)
        {return (ldexp((double)_X, _Y)); }
inline long double logl(__in long double _X)
        {return (log((double)_X)); }
inline long double log10l(__in long double _X)
        {return (log10((double)_X)); }
inline long double modfl(__in long double _X, __out long double *_Y)
        {double _Di, _Df = modf((double)_X, &_Di);
        *_Y = (long double)_Di;
        return (_Df); }
inline long double powl(__in long double _X, __in long double _Y)
        {return (pow((double)_X, (double)_Y)); }
inline long double sinl(__in long double _X)
        {return (sin((double)_X)); }
inline long double sinhl(__in long double _X)
        {return (sinh((double)_X)); }
inline long double sqrtl(__in long double _X)
        {return (sqrt((double)_X)); }
inline long double tanl(__in long double _X)
        {return (tan((double)_X)); }
inline long double tanhl(__in long double _X)
        {return (tanh((double)_X)); }

inline long double _chgsignl(__in long double _Number)
{
    return _chgsign(static_cast<double>(_Number)); 
}

inline long double _copysignl(__in long double _Number, __in long double _Sign)
{
    return _copysign(static_cast<double>(_Number), static_cast<double>(_Sign)); 
}

inline float frexpf(__in float _X, __out int *_Y)
        {return ((float)frexp((double)_X, _Y)); }

inline float fabsf(__in float _X)
        {return ((float)fabs((double)_X)); }
inline float ldexpf(__in float _X, __in int _Y)
        {return ((float)ldexp((double)_X, _Y)); }
inline float acosf(__in float _X)
        {return ((float)acos((double)_X)); }
inline float asinf(__in float _X)
        {return ((float)asin((double)_X)); }
inline float atanf(__in float _X)
        {return ((float)atan((double)_X)); }
inline float atan2f(__in float _X, __in float _Y)
        {return ((float)atan2((double)_X, (double)_Y)); }
inline float ceilf(__in float _X)
        {return ((float)ceil((double)_X)); }
inline float cosf(__in float _X)
        {return ((float)cos((double)_X)); }
inline float coshf(__in float _X)
        {return ((float)cosh((double)_X)); }
inline float expf(__in float _X)
        {return ((float)exp((double)_X)); }
inline float floorf(__in float _X)
        {return ((float)floor((double)_X)); }
inline float fmodf(__in float _X, __in float _Y)
        {return ((float)fmod((double)_X, (double)_Y)); }
inline float logf(__in float _X)
        {return ((float)log((double)_X)); }
inline float log10f(__in float _X)
        {return ((float)log10((double)_X)); }
inline float modff(__in float _X, __out float *_Y)
        { double _Di, _Df = modf((double)_X, &_Di);
        *_Y = (float)_Di;
        return ((float)_Df); }
inline float powf(__in float _X, __in float _Y)
        {return ((float)pow((double)_X, (double)_Y)); }
inline float sinf(__in float _X)
        {return ((float)sin((double)_X)); }
inline float sinhf(__in float _X)
        {return ((float)sinh((double)_X)); }
inline float sqrtf(__in float _X)
        {return ((float)sqrt((double)_X)); }
inline float tanf(__in float _X)
        {return ((float)tan((double)_X)); }
inline float tanhf(__in float _X)
        {return ((float)tanh((double)_X)); }
#endif  /* __cplusplus */
#endif  /* __assembler */

#if     !__STDC__

/* Non-ANSI names for compatibility */

#define DOMAIN      _DOMAIN
#define SING        _SING
#define OVERFLOW    _OVERFLOW
#define UNDERFLOW   _UNDERFLOW
#define TLOSS       _TLOSS
#define PLOSS       _PLOSS

#define matherr     _matherr

#ifndef __assembler /* Protect from assembler */

_CRTIMP extern double HUGE;

_CRT_NONSTDC_DEPRECATE _CRTIMP double  __cdecl cabs(__in struct _complex _P(_X));
_CRT_NONSTDC_DEPRECATE _CRTIMP double  __cdecl hypot(__in double _P(_X), __in double _P(_Y));
_CRT_NONSTDC_DEPRECATE _CRTIMP double  __cdecl j0(__in double _P(_X));
_CRT_NONSTDC_DEPRECATE _CRTIMP double  __cdecl j1(__in double _P(_X));
_CRT_NONSTDC_DEPRECATE _CRTIMP double  __cdecl jn(__in int _P(_X), __in double _P(_Y));
        int     __cdecl _matherr(__inout struct _exception * _P(_Except));
_CRT_NONSTDC_DEPRECATE _CRTIMP double  __cdecl y0(__in double _P(_X));
_CRT_NONSTDC_DEPRECATE _CRTIMP double  __cdecl y1(__in double _P(_X));
_CRT_NONSTDC_DEPRECATE _CRTIMP double  __cdecl yn(__in int _P(_X), __in double _P(_Y));

#endif  /* __assembler */

#endif  /* __STDC__ */

#ifdef  __cplusplus
}

extern "C++" {

template<class _Ty> inline
        _Ty _Pow_int(_Ty _X, int _Y)
        {unsigned int _N;
        if (_Y >= 0)
                _N = (unsigned int)_Y;
        else
                _N = (unsigned int)-_Y;
        for (_Ty _Z = _Ty(1); ; _X *= _X)
                {if ((_N & 1) != 0)
                        _Z *= _X;
                if ((_N >>= 1) == 0)
                        return (_Y < 0 ? _Ty(1) / _Z : _Z); }}

inline long __CRTDECL abs(__in long _X)
        {return (labs(_X)); }
inline double __CRTDECL abs(__in double _X)
        {return (fabs(_X)); }
inline double __CRTDECL pow(__in double _X, __in int _Y)
        {return (_Pow_int(_X, _Y)); }
inline float __CRTDECL abs(__in float _X)
        {return (fabsf(_X)); }
inline float __CRTDECL acos(__in float _X)
        {return (acosf(_X)); }
inline float __CRTDECL asin(__in float _X)
        {return (asinf(_X)); }
inline float __CRTDECL atan(__in float _X)
        {return (atanf(_X)); }
inline float __CRTDECL atan2(__in float _Y, __in float _X)
        {return (atan2f(_Y, _X)); }
inline float __CRTDECL ceil(__in float _X)
        {return (ceilf(_X)); }
inline float __CRTDECL cos(__in float _X)
        {return (cosf(_X)); }
inline float __CRTDECL cosh(__in float _X)
        {return (coshf(_X)); }
inline float __CRTDECL exp(__in float _X)
        {return (expf(_X)); }
inline float __CRTDECL fabs(__in float _X)
        {return (fabsf(_X)); }
inline float __CRTDECL floor(__in float _X)
        {return (floorf(_X)); }
inline float __CRTDECL fmod(__in float _X, __in float _Y)
        {return (fmodf(_X, _Y)); }
inline float __CRTDECL frexp(__in float _X, __out int * _Y)
        {return (frexpf(_X, _Y)); }
inline float __CRTDECL ldexp(__in float _X, __in int _Y)
        {return (ldexpf(_X, _Y)); }
inline float __CRTDECL log(__in float _X)
        {return (logf(_X)); }
inline float __CRTDECL log10(__in float _X)
        {return (log10f(_X)); }
inline float __CRTDECL modf(__in float _X, __out float * _Y)
        {return (modff(_X, _Y)); }
inline float __CRTDECL pow(__in float _X, __in float _Y)
        {return (powf(_X, _Y)); }
inline float __CRTDECL pow(__in float _X, __in int _Y)
        {return (_Pow_int(_X, _Y)); }
inline float __CRTDECL sin(__in float _X)
        {return (sinf(_X)); }
inline float __CRTDECL sinh(__in float _X)
        {return (sinhf(_X)); }
inline float __CRTDECL sqrt(__in float _X)
        {return (sqrtf(_X)); }
inline float __CRTDECL tan(__in float _X)
        {return (tanf(_X)); }
inline float __CRTDECL tanh(__in float _X)
        {return (tanhf(_X)); }
inline long double __CRTDECL abs(__in long double _X)
        {return (fabsl(_X)); }
inline long double __CRTDECL acos(__in long double _X)
        {return (acosl(_X)); }
inline long double __CRTDECL asin(__in long double _X)
        {return (asinl(_X)); }
inline long double __CRTDECL atan(__in long double _X)
        {return (atanl(_X)); }
inline long double __CRTDECL atan2(__in long double _Y, __in long double _X)
        {return (atan2l(_Y, _X)); }
inline long double __CRTDECL ceil(__in long double _X)
        {return (ceill(_X)); }
inline long double __CRTDECL cos(__in long double _X)
        {return (cosl(_X)); }
inline long double __CRTDECL cosh(__in long double _X)
        {return (coshl(_X)); }
inline long double __CRTDECL exp(__in long double _X)
        {return (expl(_X)); }
inline long double __CRTDECL fabs(__in long double _X)
        {return (fabsl(_X)); }
inline long double __CRTDECL floor(__in long double _X)
        {return (floorl(_X)); }
inline long double __CRTDECL fmod(__in long double _X, __in long double _Y)
        {return (fmodl(_X, _Y)); }
inline long double __CRTDECL frexp(__in long double _X, __out int * _Y)
        {return (frexpl(_X, _Y)); }
inline long double __CRTDECL ldexp(__in long double _X, __in int _Y)
        {return (ldexpl(_X, _Y)); }
inline long double __CRTDECL log(__in long double _X)
        {return (logl(_X)); }
inline long double __CRTDECL log10(__in long double _X)
        {return (log10l(_X)); }
inline long double __CRTDECL modf(__in long double _X, __in long double * _Y)
        {return (modfl(_X, _Y)); }
inline long double __CRTDECL pow(__in long double _X, __in long double _Y)
        {return (powl(_X, _Y)); }
inline long double __CRTDECL pow(__in long double _X, __in int _Y)
        {return (_Pow_int(_X, _Y)); }
inline long double __CRTDECL sin(__in long double _X)
        {return (sinl(_X)); }
inline long double __CRTDECL sinh(__in long double _X)
        {return (sinhl(_X)); }
inline long double __CRTDECL sqrt(__in long double _X)
        {return (sqrtl(_X)); }
inline long double __CRTDECL tan(__in long double _X)
        {return (tanl(_X)); }
inline long double __CRTDECL tanh(__in long double _X)
        {return (tanhl(_X)); }

}
#endif  /* __cplusplus */

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif  /* _INC_MATH */

#if defined(_USE_MATH_DEFINES) && !defined(_MATH_DEFINES_DEFINED)
#define _MATH_DEFINES_DEFINED

/* Define _USE_MATH_DEFINES before including math.h to expose these macro
 * definitions for common math constants.  These are placed under an #ifdef
 * since these commonly-defined names are not part of the C/C++ standards.
 */

/* Definitions of useful mathematical constants
 * M_E        - e
 * M_LOG2E    - log2(e)
 * M_LOG10E   - log10(e)
 * M_LN2      - ln(2)
 * M_LN10     - ln(10)
 * M_PI       - pi
 * M_PI_2     - pi/2
 * M_PI_4     - pi/4
 * M_1_PI     - 1/pi
 * M_2_PI     - 2/pi
 * M_2_SQRTPI - 2/sqrt(pi)
 * M_SQRT2    - sqrt(2)
 * M_SQRT1_2  - 1/sqrt(2)
 */

#define M_E        2.71828182845904523536
#define M_LOG2E    1.44269504088896340736
#define M_LOG10E   0.434294481903251827651
#define M_LN2      0.693147180559945309417
#define M_LN10     2.30258509299404568402
#define M_PI       3.14159265358979323846
#define M_PI_2     1.57079632679489661923
#define M_PI_4     0.785398163397448309616
#define M_1_PI     0.318309886183790671538
#define M_2_PI     0.636619772367581343076
#define M_2_SQRTPI 1.12837916709551257390
#define M_SQRT2    1.41421356237309504880
#define M_SQRT1_2  0.707106781186547524401

#endif  /* _USE_MATH_DEFINES */

