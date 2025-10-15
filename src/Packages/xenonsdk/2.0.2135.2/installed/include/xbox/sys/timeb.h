/***
*sys/timeb.h - definition/declarations for _ftime()
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file define the _ftime() function and the types it uses.
*       [System V]
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_TIMEB
#define _INC_TIMEB

#include <crtdefs.h>

#if     !defined(_WIN32)
#error ERROR: Only Win32 target supported!
#endif


#ifdef  _MSC_VER
#pragma pack(push,_CRT_PACKING)
#endif  /* _MSC_VER */

#ifdef  __cplusplus
extern "C" {
#endif



/* Define _CRTIMP */

#ifndef _CRTIMP
#if defined(_DLL) && _XBOX_VER < 200
#define _CRTIMP __declspec(dllimport)
#else   /* ndef _DLL */
#define _CRTIMP
#endif  /* _DLL */
#endif  /* _CRTIMP */


/* Define __cdecl for non-Microsoft compilers */

#if     ( !defined(_MSC_VER) && !defined(__cdecl) )
#define __cdecl
#endif


#if !defined(_W64)
#if !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && _MSC_VER >= 1300
#define _W64 __w64
#else
#define _W64
#endif
#endif

#ifdef  _USE_32BIT_TIME_T
#ifdef  _WIN64
#include <crtwrn.h>
#pragma _CRT_WARNING( _NO_32BIT_TIME_T )
#undef  _USE_32BIT_TIME_T
#endif
#else
#if     _INTEGRAL_MAX_BITS < 64
#define _USE_32BIT_TIME_T
#endif
#endif

#ifndef _TIME32_T_DEFINED
typedef _W64 long __time32_t;   /* 32-bit time value */
#define _TIME32_T_DEFINED
#endif

#ifndef _TIME64_T_DEFINED
#if     _INTEGRAL_MAX_BITS >= 64
typedef __int64 __time64_t;     /* 64-bit time value */
#endif
#define _TIME64_T_DEFINED
#endif

#ifndef _TIME_T_DEFINED
#ifdef _USE_32BIT_TIME_T
typedef __time32_t time_t;      /* time value */
#else
typedef __time64_t time_t;      /* time value */
#endif
#define _TIME_T_DEFINED         /* avoid multiple def's of time_t */
#endif

/* Structure returned by _ftime system call */

#ifndef _TIMEB_DEFINED
struct __timeb32 {
        __time32_t time;
        unsigned short millitm;
        short timezone;
        short dstflag;
        };

#if     !__STDC__

/* Non-ANSI name for compatibility */
struct timeb {
        time_t time;
        unsigned short millitm;
        short timezone;
        short dstflag;
        };


#endif

#if     _INTEGRAL_MAX_BITS >= 64
struct __timeb64 {
        __time64_t time;
        unsigned short millitm;
        short timezone;
        short dstflag;
        };
#endif

#ifdef  _USE_32BIT_TIME_T

#define _timeb      __timeb32
#define _ftime      _ftime32
#define _ftime_s    _ftime32_s

#else

#define _timeb      __timeb64
#define _ftime      _ftime64
#define _ftime_s    _ftime64_s

#endif
#define _TIMEB_DEFINED
#endif

#include <crtdefs.h>

/* Function prototypes */

_CRT_INSECURE_DEPRECATE _CRTIMP void __cdecl _ftime32(__out struct __timeb32 * _P(_Time));
_CRTIMP errno_t __cdecl _ftime32_s(__out struct __timeb32 * _P(_Time));
#if     _INTEGRAL_MAX_BITS >= 64
_CRT_INSECURE_DEPRECATE _CRTIMP void __cdecl _ftime64(__out struct __timeb64 * _P(_Time));
_CRTIMP errno_t __cdecl _ftime64_s(__out struct __timeb64 * _P(_Time));
#endif

#if !defined(RC_INVOKED) && !defined(__midl)
#include <sys/timeb.inl>
#endif

#ifdef  __cplusplus
}
#endif

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif  /* _INC_TIMEB */
