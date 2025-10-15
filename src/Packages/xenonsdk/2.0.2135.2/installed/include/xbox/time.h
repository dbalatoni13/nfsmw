/***
*time.h - definitions/declarations for time routines
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file has declarations of time routines and defines
*       the structure returned by the localtime and gmtime routines and
*       used by asctime.
*       [ANSI/System V]
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_TIME
#define _INC_TIME

#include <crtdefs.h>

#if     !defined(_WIN32)
#error ERROR: Only Win32 target supported!
#endif


#ifdef  _MSC_VER
/*
 * Currently, all MS C compilers for Win32 platforms default to 8 byte
 * alignment.
 */
#pragma pack(push,_CRT_PACKING)
#endif  /* _MSC_VER */

#ifdef  __cplusplus
extern "C" {
#endif


#if !defined(_W64)
#if !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && _MSC_VER >= 1300
#define _W64 __w64
#else
#define _W64
#endif
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

#ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
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

#ifndef _CLOCK_T_DEFINED
typedef long clock_t;
#define _CLOCK_T_DEFINED
#endif

#ifndef _SIZE_T_DEFINED
#ifdef  _WIN64
typedef unsigned __int64    size_t;
#else
typedef _W64 unsigned int   size_t;
#endif
#define _SIZE_T_DEFINED
#endif

/* Define NULL pointer value */

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif


#ifndef _TM_DEFINED
struct tm {
        int tm_sec;     /* seconds after the minute - [0,59] */
        int tm_min;     /* minutes after the hour - [0,59] */
        int tm_hour;    /* hours since midnight - [0,23] */
        int tm_mday;    /* day of the month - [1,31] */
        int tm_mon;     /* months since January - [0,11] */
        int tm_year;    /* years since 1900 */
        int tm_wday;    /* days since Sunday - [0,6] */
        int tm_yday;    /* days since January 1 - [0,365] */
        int tm_isdst;   /* daylight savings time flag */
        };
#define _TM_DEFINED
#endif


/* Clock ticks macro - ANSI version */

#define CLOCKS_PER_SEC  1000


/* Extern declarations for the global variables used by the ctime family of
 * routines.
 */

/* non-zero if daylight savings time is used */
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn int* __cdecl __daylight(void);
#define _daylight (*__daylight())

/* offset for Daylight Saving Time */
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn long* __cdecl __dstbias(void);
#define _dstbias (*__dstbias())

/* difference in seconds between GMT and local time */
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn long* __cdecl __timezone(void);
#define _timezone (*__timezone())

/* standard/daylight savings time zone names */
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn __deref_out char ** __cdecl __tzname(void);
#define _tzname (__tzname())

_CRTIMP errno_t __cdecl _get_daylight(__out int * _Daylight);
_CRTIMP errno_t __cdecl _get_dstbias(__out long * _Daylight_savings_bias);
_CRTIMP errno_t __cdecl _get_timezone(__out long * _Timezone);
_CRTIMP errno_t __cdecl _get_tzname(__out size_t *_ReturnValue, __out_ecount(_SizeInBytes) char *_Buffer, __in size_t _SizeInBytes, __in int _Index);


/* Function prototypes */
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn char * __cdecl asctime(__in const struct tm * _P(_Tm));
_CRTIMP __checkReturn_wat errno_t __cdecl asctime_s(__out_ecount_part(_SizeInBytes, 26) char *_P(_Buf), __in size_t _P(_SizeInBytes), __in const struct tm * _P(_Tm));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_1(errno_t, asctime_s, __out_ecount_part(_Size,26) char, __in const struct tm *)

_CRT_INSECURE_DEPRECATE _CRTIMP char * __cdecl _ctime32(__in const __time32_t * _P(_Time));
_CRTIMP errno_t __cdecl _ctime32_s(__out_ecount_part(_SizeInBytes, 26) char *_P(_Buf), __in size_t _P(_SizeInBytes), __in const __time32_t *_P(_Time));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_1(errno_t, _ctime32_s, __out_ecount_part(_Size, 26) char, __in const __time32_t *)

_CRTIMP __checkReturn clock_t __cdecl clock(void);
_CRTIMP double __cdecl _difftime32(__in __time32_t _P(_Time1), __in __time32_t _P(_Time2));

_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn struct tm * __cdecl _gmtime32(__in const __time32_t * _P(_Time));
_CRTIMP __checkReturn_wat errno_t __cdecl _gmtime32_s(__in struct tm *_P(_Tm), __in const __time32_t * _P(_Time));

_CRT_INSECURE_DEPRECATE _CRTIMP struct tm * __cdecl _localtime32(__in const __time32_t * _P(_Time));
_CRTIMP errno_t __cdecl _localtime32_s(__out struct tm *_P(_Tm), __in const __time32_t * _P(_Time));

_CRTIMP size_t __cdecl strftime(__out_ecount(_SizeInBytes) char * _P(_Buf), __in size_t _P(_SizeInBytes), __in __format_string const char * _P(_Format), __in const struct tm * _P(_Tm));
_CRTIMP size_t __cdecl _strftime_l(__out char *_P(_Buf), __in size_t _P(_Max_size), __in __format_string const char * _P(_Format), __in const struct tm *_P(_Tm), __in_opt _locale_t _P(_PtLoci));

_CRTIMP __checkReturn_wat errno_t __cdecl _strdate_s(__out_ecount_part(_SizeInBytes, 9) char *_P(_Buf), __in size_t _P(_SizeInBytes));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_0(errno_t, _strdate_s, __out_ecount_part(_Size, 9) char)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_0(char *, __RETURN_POLICY_DST, _CRTIMP, _strdate, __out_ecount_part(_Size, 9) char)

_CRTIMP __checkReturn_wat errno_t __cdecl _strtime_s(__out_ecount_part(_SizeInBytes, 9) char *_P(_Buf) , __in size_t _P(_SizeInBytes));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_0(errno_t, _strtime_s, __out_ecount_part(_Size,9) char)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_0(char *, __RETURN_POLICY_DST, _CRTIMP, _strtime, __out_ecount_part(_Size, 9) char)

_CRTIMP __time32_t __cdecl _time32(__out_opt __time32_t * _P(_Time));
_CRTIMP __time32_t __cdecl _mktime32(__inout struct tm * _P(_Tm));
_CRTIMP __time32_t __cdecl _mkgmtime32(__inout struct tm * _P(_Tm));

_CRTIMP void __cdecl _tzset(void);

#if     _INTEGRAL_MAX_BITS >= 64
_CRTIMP __checkReturn double __cdecl _difftime64(__in __time64_t _P(_Time1), __in __time64_t _P(_Time2));
_CRT_INSECURE_DEPRECATE _CRTIMP char * __cdecl _ctime64(__in const __time64_t * _P(_Time));
_CRTIMP errno_t __cdecl _ctime64_s(__out_ecount(_SizeInBytes) char *_P(_Buf), __in size_t _P(_SizeInBytes), __in const __time64_t * _P(_Time));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_1(errno_t, _ctime64_s, __out_ecount(_Size) char, __in const __time64_t *)

_CRT_INSECURE_DEPRECATE _CRTIMP struct tm * __cdecl _gmtime64(__in const __time64_t * _P(_Time));
_CRTIMP errno_t __cdecl _gmtime64_s(__out struct tm *_P(_Tm), __in const __time64_t *_P(_Time));

_CRT_INSECURE_DEPRECATE _CRTIMP struct tm * __cdecl _localtime64(__in const __time64_t * _P(_Time));
_CRTIMP errno_t __cdecl _localtime64_s(__out struct tm *_Tm, __in const __time64_t *_P(_Time));

_CRTIMP __time64_t __cdecl _mktime64(__inout struct tm * _P(_Tm));
_CRTIMP __time64_t __cdecl _mkgmtime64(__inout struct tm * _P(_Tm));
_CRTIMP __time64_t __cdecl _time64(__out_opt __time64_t * _P(_Time));
#endif

#if defined(_XBOX) && !defined(_XBOX_CRT_DEPRECATE_TIME_FUNCTIONS) && !defined(_CRT_TIME_FUNCTIONS_NO_DEPRECATE)
#define _CRT_TIME_FUNCTIONS_NO_DEPRECATE
#endif

#ifndef _CRT_TIME_FUNCTIONS_NO_DEPRECATE
#define _CRT_TIME_FUNCTIONS_DEPRECATE __declspec(deprecated)
#else
#define _CRT_TIME_FUNCTIONS_DEPRECATE
#endif

/* The Win32 API GetLocalTime and SetLocalTime should be used instead. */
_CRT_TIME_FUNCTIONS_DEPRECATE unsigned __cdecl _getsystime(__out struct tm * _P(_Tm));


#ifndef _SIZE_T_DEFINED
typedef unsigned int size_t;
#define _SIZE_T_DEFINED
#endif

#ifndef _WTIME_DEFINED

/* wide function prototypes, also declared in wchar.h */
 
_CRT_INSECURE_DEPRECATE _CRTIMP wchar_t * __cdecl _wasctime(__in const struct tm * _P(_Tm));
_CRTIMP errno_t __cdecl _wasctime_s(__out_ecount_part(_SizeInWords, 26) wchar_t *_P(_Buf), __in size_t _SizeInWords, __in const struct tm * _P(_Tm));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_1(errno_t, _wasctime_s, __out_ecount_part(_Size,26) wchar_t, __in const struct tm *)

_CRT_INSECURE_DEPRECATE _CRTIMP wchar_t * __cdecl _wctime32(__in const __time32_t *_P(_Time));
_CRTIMP errno_t __cdecl _wctime32_s(__out_ecount_part(_SizeInWords, 26) wchar_t* _P(_Buf), __in size_t _SizeInWords, __in const __time32_t * _P(_Time));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_1(errno_t, _wctime32_s, __out_ecount_part(_Size,26) wchar_t, __in const __time32_t *)

_CRTIMP size_t __cdecl wcsftime(__out_ecount(_SizeInWords) wchar_t * _P(_Buf), __in size_t _P(_SizeInWords), __in __format_string const wchar_t * _P(_Format),  __in const struct tm * _P(_Tm));
_CRTIMP size_t __cdecl _wcsftime_l(__out_ecount(_SizeInWords) wchar_t * _P(_Buf), __in size_t _P(_SizeInWords), __in __format_string const wchar_t *_P(_Format), __in const struct tm *_P(_Tm), __in_opt _locale_t _P(_PtLoci));

_CRTIMP errno_t __cdecl _wstrdate_s(__out_ecount_part(_SizeInWords, 9) wchar_t * _P(_Buf), __in size_t _P(_SizeInWords));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_0(errno_t, _wstrdate_s, __out_ecount_part(_Size, 9) wchar_t)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_0(wchar_t *, __RETURN_POLICY_DST, _CRTIMP, _wstrdate, __out_ecount_part(_Size,9) wchar_t)

_CRTIMP errno_t __cdecl _wstrtime_s(__out_ecount_part(_SizeInWords,9) wchar_t * _P(_Buf), __in size_t _SizeInWords);
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_0(errno_t, _wstrtime_s, __out_ecount_part(_Size, 9) wchar_t)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_0(wchar_t *, __RETURN_POLICY_DST, _CRTIMP, _wstrtime, __out_ecount_part(_Size, 9) wchar_t)

#if     _INTEGRAL_MAX_BITS >= 64
_CRT_INSECURE_DEPRECATE _CRTIMP wchar_t * __cdecl _wctime64(__in const __time64_t * _P(_Time));
_CRTIMP errno_t __cdecl _wctime64_s(__out_ecount_part(_SizeInWords, 26) wchar_t* _P(_Buf), __in size_t _P(_SizeInWords), __in const __time64_t *_P(_Time));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_1(errno_t, _wctime64_s, __out_ecount_part(_Size,26) wchar_t, __in const __time64_t *)
#endif

#if !defined(RC_INVOKED) && !defined(__midl)
#include <wtime.inl>
#endif

#define _WTIME_DEFINED
#endif

#if !defined(RC_INVOKED) && !defined(__midl)
#include <time.inl>
#endif

#if     !__STDC__ || defined(_POSIX_)

/* Non-ANSI names for compatibility */

#define CLK_TCK  CLOCKS_PER_SEC

_CRT_INSECURE_DEPRECATE _CRTIMP extern int daylight;
_CRT_INSECURE_DEPRECATE _CRTIMP extern long timezone;
_CRT_INSECURE_DEPRECATE _CRTIMP extern char * tzname[2];

_CRT_NONSTDC_DEPRECATE _CRTIMP void __cdecl tzset(void);

#endif  /* __STDC__ */


#ifdef  __cplusplus
}
#endif

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif  /* _INC_TIME */
