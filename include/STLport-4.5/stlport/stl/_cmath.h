/*
 * Copyright (c) 1999 
 * Boris Fomitchev
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted 
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 */

#ifndef _STLP_CMATH_H_HEADER
# define _STLP_CMATH_H_HEADER

#if ! defined (_STLP_USE_NEW_C_HEADERS) 

_STLP_BEGIN_NAMESPACE

#  ifndef _STLP_HAS_NATIVE_FLOAT_ABS
inline double abs(double __x)                 { return _STLP_DO_ABS(double)(__x); }
inline float abs (float __x)                  { return _STLP_DO_ABS(float)(__x); }
#  endif

inline double pow(double __x, int __y)        { return _STLP_DO_POW(double)(__x, __y); }
inline float acos (float __x)                 { return _STLP_DO_ACOS(float)(__x); }
inline float asin (float __x)                 { return _STLP_DO_ASIN(float)(__x); }
inline float atan (float __x)                 { return _STLP_DO_ATAN(float)(__x); }
inline float atan2(float __x, float __y)      { return _STLP_DO_ATAN2(float)(__x, __y); }
inline float ceil (float __x)                 { return _STLP_DO_CEIL(float)(__x); }
inline float cos (float __x)                  { return _STLP_DO_COS(float)(__x); }
inline float cosh (float __x)                 { return _STLP_DO_COSH(float)(__x); }
inline float exp (float __x)                  { return _STLP_DO_EXP(float)(__x); }
# ifdef _STLP_USE_NAMESPACES
inline float fabs (float __x)                 { return _STLP_DO_ABS(float)(__x); }
# endif
inline float floor(float __x)                 { return _STLP_DO_FLOOR(float)(__x); }
inline float fmod (float __x, float __y)      { return _STLP_DO_FMOD(float)(__x, __y); }
inline float frexp(float __x, int* __y)       { return _STLP_DO_FREXP(float)(__x, __y); }
inline float ldexp(float __x, int __y)        { return _STLP_DO_LDEXP(float)(__x, __y); }
// fbp : float versions are not always available
inline float modf (float __x, float* __y)     { 
  double __dd[2]; 
  double __res = _STLP_DO_MODF(double)((double)__x, __dd); 
  __y[0] = (float)__dd[0] ; __y[1] = (float)__dd[1]; 
  return (float)__res; 
}

inline float log (float __x)                  { return _STLP_DO_LOG(float)(__x); }
inline float log10(float __x)                 { return _STLP_DO_LOG10(float)(__x); }
inline float pow (float __x, float __y)       { return _STLP_DO_POW(float)(__x, __y); }
inline float pow (float __x, int __y)         { return _STLP_DO_POW(float)(__x, __y); }
inline float sin (float __x)                  { return _STLP_DO_SIN(float)(__x); }
inline float sinh (float __x)                 { return _STLP_DO_SINH(float)(__x); }
inline float sqrt (float __x)                 { return _STLP_DO_SQRT(float)(__x); }
inline float tan (float __x)                  { return _STLP_DO_TAN(float)(__x); }
inline float tanh (float __x)                 { return _STLP_DO_TANH(float)(__x); }

#  if ! (defined  (_STLP_NO_LONG_DOUBLE) || defined(_STLP_VENDOR_LONG_DOUBLE_MATH))

#if !defined (__MVS__)
inline long double abs (long double __x)                  { return _STLP_DO_ABS(long double)((double)__x); }
#endif
inline long double acos (long double __x)                 { return _STLP_DO_ACOS(long double)(__x); }
inline long double asin (long double __x)                 { return _STLP_DO_ASIN(long double)(__x); }
inline long double atan (long double __x)                 { return _STLP_DO_ATAN(long double)(__x); }
inline long double atan2(long double __x, long double __y)      { return _STLP_DO_ATAN2(long double)(__x, __y); }
inline long double ceil (long double __x)                 { return _STLP_DO_CEIL(long double)(__x); }
inline long double cos (long double __x)                  { return _STLP_DO_COS(long double)(__x); }
inline long double cosh (long double __x)                 { return _STLP_DO_COSH(long double)(__x); }
inline long double exp (long double __x)                  { return _STLP_DO_EXP(long double)(__x); }
inline long double fabs (long double __x)                 { return _STLP_DO_ABS(long double)(__x); }
inline long double floor(long double __x)                 { return _STLP_DO_FLOOR(long double)(__x); }
inline long double fmod (long double __x, long double __y)      { return _STLP_DO_FMOD(long double)(__x, __y); }
inline long double frexp(long double __x, int* __y)       { return _STLP_DO_FREXP(long double)(__x, __y); }
inline long double ldexp(long double __x, int __y)        { return _STLP_DO_LDEXP(long double)(__x, __y); }
// fbp : long double versions are not available
inline long double modf (long double __x, long double* __y)     { 
  double __dd[2]; 
  double __res = _STLP_DO_MODF(double)((double)__x, __dd); 
  __y[0] = (long double)__dd[0] ; __y[1] = (long double)__dd[1]; 
  return (long double)__res; 
}
inline long double log (long double __x)                  { return _STLP_DO_LOG(long double)(__x); }
inline long double log10(long double __x)                 { return _STLP_DO_LOG10(long double)(__x); }
inline long double pow (long double __x, long double __y)       { return _STLP_DO_POW(long double)(__x, __y); }
inline long double pow (long double __x, int __y)         { return _STLP_DO_POW(long double)(__x, __y); }
inline long double sin (long double __x)                  { return _STLP_DO_SIN(long double)(__x); }
inline long double sinh (long double __x)                 { return _STLP_DO_SINH(long double)(__x); }
inline long double sqrt (long double __x)                 { return _STLP_DO_SQRT(long double)(__x); }
inline long double tan (long double __x)                  { return _STLP_DO_TAN(long double)(__x); }
inline long double tanh (long double __x)                 { return _STLP_DO_TANH(long double)(__x); }
#  endif

_STLP_END_NAMESPACE

# endif /* NEW_C_HEADERS */
#endif /* CMATH_H */
