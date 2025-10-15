/***
*conio.h - console and port I/O declarations
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This include file contains the function declarations for
*       the MS C V2.03 compatible console I/O routines.
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_CONIO
#define _INC_CONIO

#include <crtdefs.h>

#ifndef _XBSTRICT

#ifdef __cplusplus
extern "C" {
#endif

/* Function prototypes */

_CRTIMP __checkReturn_wat errno_t __cdecl _cgets_s(__out_ecount_part(_Size, *_SizeRead) char * _P(_Buffer), size_t _P(_Size), __out size_t * _P(_SizeRead));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_1(errno_t, _cgets_s, __out_ecount_part(_Size, *_TArg1) char, __out size_t *)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_0_CGETS(char *, _CRTIMP, _cgets, __out char)
_CRTIMP __checkReturn_wat errno_t __cdecl _cgets_s(__out_ecount(_Size) char * _P(_Buffer), size_t _P(_Size), __out size_t * _P(_SizeRead));
_CRTIMP __checkReturn_opt int __cdecl _cprintf(__in __format_string const char * _P(_Format), ...);
_CRTIMP __checkReturn_opt int __cdecl _cprintf_s(__in __format_string const char * _P(_Format), ...);
_CRTIMP __checkReturn_opt int __cdecl _cputs(__in const char * _P(_Str));
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl _cscanf(__in __format_string const char * _P(_Format), ...);
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl _cscanf_l(__in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn_opt int __cdecl _cscanf_s(__in __format_string const char * _P(_Format), ...);
_CRTIMP __checkReturn_opt int __cdecl _cscanf_s_l(__in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn int __cdecl _getch(void);
_CRTIMP __checkReturn int __cdecl _getche(void);
_CRTIMP __checkReturn_opt int __cdecl _vcprintf(__in __format_string const char * _P(_Format), va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl _vcprintf_s(__in __format_string const char * _P(_Format), va_list _P(_ArgList));

_CRTIMP __checkReturn_opt int __cdecl _cprintf_p(__in __format_string const char * _P(_Format), ...);
_CRTIMP __checkReturn_opt int __cdecl _vcprintf_p(__in const char * _P(_Format), va_list _P(_ArgList));

_CRTIMP __checkReturn_opt int __cdecl _cprintf_l(__in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn_opt int __cdecl _cprintf_s_l(__in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn_opt int __cdecl _vcprintf_l(__in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl _vcprintf_s_l(__in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl _cprintf_p_l(__in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn_opt int __cdecl _vcprintf_p_l(__in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));

_CRTIMP int __cdecl _kbhit(void);
_CRTIMP int __cdecl _putch(__in int _P(_Ch));
_CRTIMP int __cdecl _ungetch(__in int _P(_Ch));

int __cdecl _getch_nolock(void);
int __cdecl _getche_nolock(void);
int __cdecl _putch_nolock(__in int _P(_Ch));
int __cdecl _ungetch_nolock(__in int _P(_Ch));



#ifndef _WCONIO_DEFINED

/* wide function prototypes, also declared in wchar.h */

#ifndef WEOF
#define WEOF (wint_t)(0xFFFF)
#endif

_CRTIMP __checkReturn_wat errno_t __cdecl _cgetws_s(__out_ecount_part(_SizeInWords, *_SizeRead) wchar_t * _P(_Buffer), size_t _P(_SizeInWords), __out size_t * _P(_SizeRead));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_1(errno_t, _cgetws_s, __out_ecount_part(_Size, *_TArg1) wchar_t, size_t *)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_0_CGETS(wchar_t *, _CRTIMP, _cgetws, __out wchar_t)
_CRTIMP __checkReturn wint_t __cdecl _getwch(void);
_CRTIMP __checkReturn wint_t __cdecl _getwche(void);
_CRTIMP __checkReturn wint_t __cdecl _putwch(wchar_t _P(_WCh));
_CRTIMP __checkReturn wint_t __cdecl _ungetwch(wint_t _P(_WCh));
_CRTIMP __checkReturn_opt int __cdecl _cputws(__in const wchar_t * _P(_String));
_CRTIMP __checkReturn_opt int __cdecl _cwprintf(__in __format_string const wchar_t * _P(_Format), ...);
_CRTIMP __checkReturn_opt int __cdecl _cwprintf_s(__in __format_string const wchar_t * _P(_Format), ...);
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl _cwscanf(__in __format_string const wchar_t * _P(_Format), ...);
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl _cwscanf_l(__in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn_opt int __cdecl _cwscanf_s(__in __format_string const wchar_t * _P(_Format), ...);
_CRTIMP __checkReturn_opt int __cdecl _cwscanf_s_l(__in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn_opt int __cdecl _vcwprintf(__in __format_string const wchar_t *_P(_Format), va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl _vcwprintf_s(__in __format_string const wchar_t *_P(_Format), va_list _P(_ArgList));

_CRTIMP __checkReturn_opt int __cdecl _cwprintf_p(__in __format_string const wchar_t * _P(_Format), ...);
_CRTIMP __checkReturn_opt int __cdecl _vcwprintf_p(__in __format_string const wchar_t*  _P(_Format), va_list _P(_ArgList));

_CRTIMP int __cdecl _cwprintf_l(__in const __format_string wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP int __cdecl _cwprintf_s_l(__in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP int __cdecl _vcwprintf_l(__in __format_string const wchar_t *_P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));
_CRTIMP int __cdecl _vcwprintf_s_l(__in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));
_CRTIMP int __cdecl _cwprintf_p_l(__in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP int __cdecl _vcwprintf_p_l(__in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));

__checkReturn_opt wint_t __cdecl _putwch_nolock(wchar_t _P(_WCh));
__checkReturn wint_t __cdecl _getwch_nolock(void);
__checkReturn wint_t __cdecl _getwche_nolock(void);
__checkReturn_opt wint_t __cdecl _ungetwch_nolock(wint_t _P(_WCh));


#define _WCONIO_DEFINED
#endif  /* _WCONIO_DEFINED */

#if     !__STDC__

/* Non-ANSI names for compatibility */

#pragma warning(push)
#pragma warning(disable: 4141) /* Using deprecated twice */ 
_CRT_NONSTDC_DEPRECATE _CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt char * __cdecl cgets(__out_ecount(((int)_Buffer[0])+2) char * _P(_Buffer));
#pragma warning(pop)
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl cprintf(__in __format_string const char * _P(_Format), ...);
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl cputs(__in const char * _P(_Str));
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl cscanf(__in __format_string const char * _P(_Format), ...);
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl getch(void);
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl getche(void);
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl kbhit(void);
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl putch(int _P(_Ch));
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl ungetch(int _P(_Ch));

#endif  /* __STDC__ */

#ifdef  __cplusplus
}
#endif

#endif // XBSTRICT

#endif  /* _INC_CONIO */
