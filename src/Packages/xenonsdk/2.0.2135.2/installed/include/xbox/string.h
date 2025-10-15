/***
*string.h - declarations for string manipulation functions
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file contains the function declarations for the string
*       manipulation functions.
*       [ANSI/System V]
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_STRING
#define _INC_STRING

#include <crtdefs.h>

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef _NLSCMP_DEFINED
#define _NLSCMPERROR    2147483647  /* currently == INT_MAX */
#define _NLSCMP_DEFINED
#endif

/* Define NULL pointer value */

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

/* For backwards compatibility */
#define _WConst_return _CONST_RETURN

/* Function prototypes */

        _CRT_INSECURE_DEPRECATE_MEMORY void *  __cdecl memcpy(__out_bcount_full_opt(_Size) void * _P(_Dst), __in_bcount_opt(_Size) const void * _P(_Src), __in size_t _P(_Size));
_CRTIMP __checkReturn_wat errno_t __cdecl memcpy_s(__out_bcount_part_opt(_DstSize, _MaxCount) void * _P(_Dst), __in size_t _P(_DstSize), __in_bcount_opt(_MaxCount) const void * _P(_Src), __in size_t _P(_MaxCount));
        __checkReturn int     __cdecl memcmp(__in_bcount_opt(_Size) const void * _P(_Buf1), __in_bcount_opt(_Size) const void * _P(_Buf2), __in size_t _P(_Size));
        void *  __cdecl memset(__out_bcount_opt(_Size) void * _P(_Dst), __in int _P(_Val), __in size_t _P(_Size));
        _CRT_INSECURE_DEPRECATE char *  __cdecl _strset(__inout char * _P(_Str), __in int _P(_Val));
_CRTIMP_ALTERNATIVE __checkReturn_wat errno_t __cdecl _strset_s(__inout_ecount(_DstSize) char * _P(_Dst), __in size_t _P(_DstSize), __in int _P(_Value));
_CRTIMP_ALTERNATIVE __checkReturn_wat errno_t __cdecl strcpy_s(__out_ecount(_DstSize) char * _P(_Dst), __in size_t _P(_DstSize), __in const char * _P(_Src));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_1(errno_t, strcpy_s, __out_ecount(_Size) char, __in const char *)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_1(char *, __RETURN_POLICY_DST, __EMPTY_DECLSPEC, strcpy, __out char, __in const char *)
_CRTIMP_ALTERNATIVE __checkReturn_wat errno_t __cdecl strcat_s(__inout_ecount(_DstSize) char * _P(_Dst), __in size_t _P(_DstSize), __in const char * _P(_Src));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_1(errno_t, strcat_s, __inout_ecount(_Size) char, __in const char *)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_1(char *, __RETURN_POLICY_DST, __EMPTY_DECLSPEC, strcat, __inout char, __in const char *)
        __checkReturn int     __cdecl strcmp(__in const char * _P(_Str1), __in const char * _P(_Str2));
        __checkReturn size_t  __cdecl strlen(__in  const char * _P(_Str));
_CRTIMP __checkReturn size_t  __cdecl strnlen(__in_ecount(_MaxCount)  const char * _P(_Str), __in size_t _P(_MaxCount));
_CRTIMP void *  __cdecl _memccpy(__out_bcount_opt(_MaxCount)  void * _P(_Dst), __in_bcount_opt(_MaxCount) const void * _P(_Src), __in int _P(_Val), __in size_t _P(_MaxCount));
_CRTIMP _CONST_RETURN __checkReturn void *  __cdecl memchr(__in_bcount_opt(_MaxCount) const void * _P(_Buf), __in int _P(_Val), __in size_t _P(_MaxCount));
_CRTIMP __checkReturn int     __cdecl _memicmp(__in_bcount_opt(_Size) const void * _P(_Buf1), __in_bcount_opt(_Size) const void * _P(_Buf2), __in size_t _P(_Size));
_CRTIMP __checkReturn int     __cdecl _memicmp_l(__in_bcount_opt(_Size) const void * _P(_Buf1), __in_bcount_opt(_Size) const void * _P(_Buf2), __in size_t _P(_Size), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn_wat errno_t __cdecl memmove_s(__out_bcount_part_opt(_DstSize,_MaxCount) void * _P(_Dst), __in size_t _P(_DstSize), __in_bcount_opt(_MaxCount) const void * _P(_Src), __in size_t _P(_MaxCount));

_CRTIMP _CRT_INSECURE_DEPRECATE_MEMORY void *  __cdecl memmove(__out_bcount_full_opt(_Size) void * _P(_Dst), __in_bcount_opt(_Size) const void * _P(_Src), __in size_t _P(_Size));

#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
#pragma push_macro("_strdup")
#undef _strdup
#endif

_CRTIMP __checkReturn char *  __cdecl _strdup(__in_opt const char * _P(_Src));

#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
#pragma pop_macro("_strdup")
#endif

_CRTIMP __checkReturn _CONST_RETURN char *  __cdecl strchr(__in const char * _P(_Str), __in int _P(_Val));
_CRTIMP __checkReturn int     __cdecl _stricmp(__in  const char * _P(_Str1), __in  const char * _P(_Str2));
_CRTIMP __checkReturn int     __cdecl _strcmpi(__in  const char * _P(_Str1), __in  const char * _P(_Str2));
_CRTIMP __checkReturn int     __cdecl _stricmp_l(__in  const char * _P(_Str1), __in  const char * _P(_Str2), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int     __cdecl strcoll(__in  const char * _P(_Str1), __in  const  char * _P(_Str2));
_CRTIMP __checkReturn int     __cdecl _strcoll_l(__in  const char * _P(_Str1), __in  const char * _P(_Str2), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int     __cdecl _stricoll(__in  const char * _P(_Str1), __in  const char * _P(_Str2));
_CRTIMP __checkReturn int     __cdecl _stricoll_l(__in  const char * _P(_Str1), __in  const char * _P(_Str2), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int     __cdecl _strncoll  (__in const char * _P(_Str1), __in const char * _P(_Str2), __in size_t _P(_MaxCount));
_CRTIMP __checkReturn int     __cdecl _strncoll_l(__in const char * _P(_Str1), __in const char * _P(_Str2), __in size_t _P(_MaxCount), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int     __cdecl _strnicoll (__in const char * _P(_Str1), __in const char * _P(_Str2), __in size_t _P(_MaxCount));
_CRTIMP __checkReturn int     __cdecl _strnicoll_l(__in const char * _P(_Str1), __in const char * _P(_Str2), __in size_t _P(_MaxCount), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn size_t  __cdecl strcspn(__in  const char * _P(_Str), __in  const char * _P(_Control));
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn char *  __cdecl _strerror(__in_opt const char * _P(_ErrMsg));
_CRTIMP __checkReturn_wat errno_t __cdecl _strerror_s(__out_ecount(_SizeInBytes) char * _P(_Buf), __in size_t _P(_SizeInBytes), __in_opt const char * _P(_ErrMsg));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_1(errno_t, _strerror_s, __out_ecount(_Size) char, __in_opt const char *)
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn char *  __cdecl strerror(__in int);
_CRTIMP __checkReturn_wat errno_t __cdecl strerror_s(__out_ecount(_SizeInBytes) char * _P(_Buf), __in size_t _P(_SizeInBytes), __in int _P(_ErrNum));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_1(errno_t, strerror_s, __out_ecount(_Size) char, __in int)
_CRTIMP __checkReturn_wat errno_t __cdecl _strlwr_s(__inout_ecount(_Size) char * _P(_Str), __in size_t _P(_Size));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_0(errno_t, _strlwr_s, __inout_ecount(_Size) char)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_0(char *, __RETURN_POLICY_DST, _CRTIMP, _strlwr, __inout char)
_CRTIMP __checkReturn_wat errno_t __cdecl _strlwr_s_l(__inout_ecount(_Size) char * _P(_Str), __in size_t _P(_Size), __in_opt _locale_t _P(_PtLoci));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_1(errno_t, _strlwr_s_l, __inout_ecount(_Size) char, __in_opt _locale_t)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_1_EX(char *, __RETURN_POLICY_DST, _CRTIMP, _strlwr_l, _strlwr_s_l, __inout char, __in_opt _locale_t)
_CRTIMP_ALTERNATIVE __checkReturn_wat errno_t __cdecl strncat_s(__inout_ecount(_DstSize) char * _P(_Dst), __in size_t _P(_DstSize), __in const char * _P(_Src), __in size_t _P(_MaxCount));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_2(errno_t, strncat_s, __inout_ecount(_Size) char, __in const char *, __in size_t)
__DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_2(char *, __RETURN_POLICY_DST, _CRTIMP, strncat, __inout char, __in const char *, __in size_t)
_CRTIMP __checkReturn int     __cdecl strncmp(__in const char * _P(_Str1), __in const char * _P(_Str2), __in size_t _P(_MaxCount));
_CRTIMP __checkReturn int     __cdecl _strnicmp(__in const char * _P(_Str1), __in const char * _P(_Str2), __in size_t _P(_MaxCount));
_CRTIMP __checkReturn int     __cdecl _strnicmp_l(__in const char * _P(_Str1), __in const char * _P(_Str2), __in size_t _P(_MaxCount), __in_opt _locale_t _P(_PtLoci));
_CRTIMP_ALTERNATIVE __checkReturn_wat errno_t __cdecl strncpy_s(__out_ecount(_DstSize) char * _P(_Dst), __in size_t _P(_DstSize), __in_opt const char * _P(_Src), __in size_t _P(_MaxCount));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_2(errno_t, strncpy_s, __out_ecount(_Size) char, __in const char *, __in size_t)
__DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_2(char *, __RETURN_POLICY_DST, _CRTIMP_NOIA64, strncpy, __out char, __in const char *, __in size_t)
_CRT_INSECURE_DEPRECATE _CRTIMP char *  __cdecl _strnset(__inout char * _P(_Str), __in int _P(_Val), __in size_t _P(_MaxCount));
_CRTIMP_ALTERNATIVE __checkReturn_wat errno_t __cdecl _strnset_s(__inout_ecount(_Size) char * _P(_Str), __in size_t _P(_Size), __in int _P(_Val), __in size_t _P(_MaxCount));
_CRTIMP __checkReturn _CONST_RETURN char *  __cdecl strpbrk(__in const char * _P(_Str), __in const char * _P(_Control));
_CRTIMP __checkReturn _CONST_RETURN char *  __cdecl strrchr(__in const char * _P(_Str), __in int _P(_Ch));
_CRTIMP char *  __cdecl _strrev(__inout char * _P(_Str));
_CRTIMP __checkReturn size_t  __cdecl strspn(__in const char * _P(_Str), __in const char * _P(_Control));
_CRTIMP __checkReturn _CONST_RETURN char *  __cdecl strstr(__in const char * _P(_Str), __in const char * _P(_SubStr));
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn char *  __cdecl strtok(__inout_opt char * _P(_Str), __in const char * _P(_Delim));
_CRTIMP_ALTERNATIVE __checkReturn char *  __cdecl strtok_s(__inout_opt char * _P(_Str), __in const char * _P(_Delim), __deref_inout_opt char ** _P(_Context));
_CRTIMP __checkReturn_wat errno_t __cdecl _strupr_s(__inout_ecount(_Size) char * _P(_Str), __in size_t _P(_Size));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_0(errno_t, _strupr_s, __inout_ecount(_Size) char)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_0(char *, __RETURN_POLICY_DST, _CRTIMP, _strupr, __inout char)
_CRTIMP __checkReturn_wat errno_t __cdecl _strupr_s_l(__inout_ecount(_Size) char * _P(_Str), __in size_t _P(_Size), _locale_t _P(_PtLoci));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_1(errno_t, _strupr_s_l, __inout_ecount(_Size) char, _locale_t)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_1_EX(char *, __RETURN_POLICY_DST, _CRTIMP, _strupr_l, _strupr_s_l, __inout char, __in_opt _locale_t)
_CRTIMP __checkReturn_opt size_t  __cdecl strxfrm (__out_opt char * _P(_Dst), __in const char * _P(_Src), __in size_t _P(_MaxCount));
_CRTIMP __checkReturn_opt size_t  __cdecl _strxfrm_l(__out_opt char * _P(_Dst), __in const char * _P(_Src), __in size_t _P(_MaxCount), __in_opt _locale_t _P(_PtLoci));

#ifdef __cplusplus
extern "C++" {
#ifndef _CPP_NARROW_INLINES_DEFINED
#define _CPP_NARROW_INLINES_DEFINED
inline __checkReturn char * __CRTDECL strchr(__in char * _Str, __in int _Ch)
	{ return (char*)strchr((const char*)_Str, _Ch); }
inline __checkReturn char * __CRTDECL strpbrk(__in char * _Str, __in const char * _Control)
	{ return (char*)strpbrk((const char*)_Str, _Control); }
inline __checkReturn char * __CRTDECL strrchr(__in char * _Str, __in int _Ch)
	{ return (char*)strrchr((const char*)_Str, _Ch); }
inline __checkReturn char * __CRTDECL strstr(__in char * _Str, __in const char * _SubStr)
	{ return (char*)strstr((const char*)_Str, _SubStr); }
#endif
#ifndef _CPP_MEMCHR_DEFINED
#define _CPP_MEMCHR_DEFINED
inline __checkReturn void * __CRTDECL memchr(__in_bcount_opt(_N) void * _Pv, __in int _C, __in size_t _N)
	{ return (void*)memchr((const void*)_Pv, _C, _N); }
#endif
}
#endif

#if     !__STDC__

#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
#pragma push_macro("strdup")
#undef strdup
#endif

_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn char * __cdecl strdup(__in_opt const char * _P(_Src));

#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
#pragma pop_macro("strdup")
#endif

/* prototypes for oldnames.lib functions */
_CRT_NONSTDC_DEPRECATE _CRTIMP void * __cdecl memccpy(__out_bcount_opt(_Size) void * _P(_Dst), __in_bcount_opt(_Size) const void * _P(_Src), __in int _P(_Val), __in size_t _P(_Size));
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl memicmp(__in_bcount_opt(_Size) const void * _P(_Buf1), __in_bcount_opt(_Size) const void * _P(_Buf2), __in size_t _P(_Size));

_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl strcmpi(__in const char * _P(_Str1), __in const char * _P(_Str2));
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl stricmp(__in const char * _P(_Str1), __in const char * _P(_Str2));
_CRT_NONSTDC_DEPRECATE _CRTIMP char * __cdecl strlwr(__inout char * _P(_Str));
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl strnicmp(__in const char * _P(_Str1), __in const char * _P(_Str), __in size_t _P(_MaxCount));
_CRT_NONSTDC_DEPRECATE _CRTIMP char * __cdecl strnset(__inout char * _P(_Str), __in int _P(_Val), __in size_t _P(_MaxCount));
_CRT_NONSTDC_DEPRECATE _CRTIMP char * __cdecl strrev(__inout char * _P(_Str));
_CRT_NONSTDC_DEPRECATE         char * __cdecl strset(__inout char * _P(_Str), __in int _P(_Val));
_CRT_NONSTDC_DEPRECATE _CRTIMP char * __cdecl strupr(__inout char * _P(_Str));

#endif  /* !__STDC__ */


#ifndef _WSTRING_DEFINED

/* wide function prototypes, also declared in wchar.h  */

#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
#pragma push_macro("_wcsdup")
#undef _wcsdup
#endif

_CRTIMP __checkReturn wchar_t * __cdecl _wcsdup(__in const wchar_t * _P(_Str));

#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
#pragma pop_macro("_wcsdup")
#endif

_CRTIMP_ALTERNATIVE __checkReturn_wat errno_t __cdecl wcscat_s(__inout_ecount(_DstSize) wchar_t * _P(_Dst), __in size_t _P(_DstSize), const wchar_t * _P(_Src));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_1(errno_t, wcscat_s, __inout_ecount(_Size) wchar_t, __in const wchar_t *)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_1(wchar_t *, __RETURN_POLICY_DST, _CRTIMP, wcscat, __inout wchar_t, __in const wchar_t *)
_CRTIMP __checkReturn _CONST_RETURN wchar_t * __cdecl wcschr(__in const wchar_t * _P(_Str), wchar_t _P(_Ch));
_CRTIMP __checkReturn int __cdecl wcscmp(__in const wchar_t * _P(_Str1), __in const wchar_t * _P(_Str2));
_CRTIMP_ALTERNATIVE __checkReturn_wat errno_t __cdecl wcscpy_s(__out_ecount(_DstSize) wchar_t * _P(_Dst), __in size_t _P(_DstSize), __in const wchar_t * _P(_Src));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_1(errno_t, wcscpy_s, __out_ecount(_Size) wchar_t, __in const wchar_t *)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_1(wchar_t *, __RETURN_POLICY_DST, _CRTIMP, wcscpy, __out wchar_t, __in const wchar_t *)
_CRTIMP __checkReturn size_t __cdecl wcscspn(__in const wchar_t * _P(_Str), __in const wchar_t * _P(_Control));
_CRTIMP __checkReturn size_t __cdecl wcslen(__in const wchar_t * _P(_Str));
_CRTIMP __checkReturn size_t __cdecl wcsnlen(__in_ecount(_MaxCount) const wchar_t * _P(_Src), __in size_t _P(_MaxCount));
_CRTIMP_ALTERNATIVE __checkReturn_wat errno_t __cdecl wcsncat_s(__inout_ecount(_DstSize) wchar_t * _P(_Dst), __in size_t _P(_DstSize), __in const wchar_t * _P(_Src), __in size_t _P(_MaxCount));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_2(errno_t, wcsncat_s, __inout_ecount(_Size) wchar_t, __in const wchar_t *, __in size_t)
__DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_2(wchar_t *, __RETURN_POLICY_DST, _CRTIMP, wcsncat, __inout wchar_t, __in const wchar_t *, __in size_t)
_CRTIMP __checkReturn int __cdecl wcsncmp(__in const wchar_t * _P(_Str1), __in const wchar_t * _P(_Str2), __in size_t _P(_MaxCount));
_CRTIMP_ALTERNATIVE __checkReturn_wat errno_t __cdecl wcsncpy_s(__out_ecount(_DstSize) wchar_t * _P(_Dst), __in size_t _P(_DstSize), __in const wchar_t * _P(_Src), __in size_t _P(_MaxCount));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_2(errno_t, wcsncpy_s, __out_ecount(_Size) wchar_t, __in const wchar_t *, __in size_t)
__DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_2(wchar_t *, __RETURN_POLICY_DST, _CRTIMP, wcsncpy, __out wchar_t, __in const wchar_t *, __in size_t)
_CRTIMP __checkReturn _CONST_RETURN wchar_t * __cdecl wcspbrk(__in const wchar_t * _P(_Str), __in const wchar_t * _P(_Control));
_CRTIMP __checkReturn _CONST_RETURN wchar_t * __cdecl wcsrchr(__in const wchar_t * _P(_Str), wchar_t _P(_Ch));
_CRTIMP __checkReturn size_t __cdecl wcsspn(__in const wchar_t * _P(_Str), __in const wchar_t * _P(_Control));
_CRTIMP __checkReturn _CONST_RETURN wchar_t * __cdecl wcsstr(__in const wchar_t * _P(_Str), __in const wchar_t * _P(_SubStr));
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn wchar_t * __cdecl wcstok(__inout_opt wchar_t * _P(_Str), __in const wchar_t * _P(_Delim));
_CRTIMP_ALTERNATIVE __checkReturn wchar_t * __cdecl wcstok_s(__inout_opt wchar_t * _P(_Str), __in const wchar_t * _P(_Delim), __deref_inout_opt wchar_t ** _P(_Context));
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn wchar_t * __cdecl _wcserror(__in int _P(_ErrNum));
_CRTIMP __checkReturn_wat errno_t __cdecl _wcserror_s(__out_ecount_opt(_SizeInWords) wchar_t * _P(_Buf), __in size_t _P(_SizeInWords), __in int _P(_ErrNum));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_1(errno_t, _wcserror_s, __out_ecount(_Size) wchar_t, __in int)
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn wchar_t * __cdecl __wcserror(__in_opt const wchar_t * _P(_Str));
_CRTIMP __checkReturn_wat errno_t __cdecl __wcserror_s(__out_ecount_opt(_SizeInWords) wchar_t * _P(_Buffer), __in size_t _P(_SizeInWords), __in const wchar_t * _P(_ErrMsg));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_1(errno_t, __wcserror_s, __out_ecount(_Size) wchar_t, __in const wchar_t *)

_CRTIMP __checkReturn int __cdecl _wcsicmp(__in const wchar_t * _P(_Str1), __in const wchar_t * _P(_Str2));
_CRTIMP __checkReturn int __cdecl _wcsicmp_l(__in const wchar_t * _P(_Str1), __in const wchar_t * _P(_Str2), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl _wcsnicmp(__in const wchar_t * _P(_Str1), __in const wchar_t * _P(_Str2), __in size_t _P(_MaxCount));
_CRTIMP __checkReturn int __cdecl _wcsnicmp_l(__in const wchar_t * _P(_Str1), __in const wchar_t * _P(_Str2), __in size_t _P(_MaxCount), __in_opt _locale_t _P(_PtLoci));
_CRT_INSECURE_DEPRECATE _CRTIMP wchar_t * __cdecl _wcsnset(__inout wchar_t * _P(_Str), __in wchar_t _P(_Val), __in size_t _P(_MaxCount));
_CRTIMP_ALTERNATIVE __checkReturn_wat errno_t __cdecl _wcsnset_s(__inout_ecount(_DstSizeInWords) wchar_t * _P(_Dst), __in size_t _P(_DstSizeInWords), wchar_t _P(_Val), __in size_t _P(_MaxCount));
_CRTIMP wchar_t * __cdecl _wcsrev(__inout wchar_t * _P(_Str));
_CRT_INSECURE_DEPRECATE _CRTIMP wchar_t * __cdecl _wcsset(__inout wchar_t * _P(_Str), wchar_t _P(_Val));
_CRTIMP_ALTERNATIVE __checkReturn_wat errno_t __cdecl _wcsset_s(__inout_ecount(_SizeInWords) wchar_t * _P(_Str), __in size_t _P(_SizeInWords), wchar_t _P(_Val));

_CRTIMP __checkReturn_wat errno_t __cdecl _wcslwr_s(__inout_ecount(_SizeInWords) wchar_t * _P(_Str), __in size_t _P(_SizeInWords));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_0(errno_t, _wcslwr_s, __inout_ecount(_Size) wchar_t)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_0(wchar_t *, __RETURN_POLICY_DST, _CRTIMP, _wcslwr, __inout wchar_t)
_CRTIMP __checkReturn_wat errno_t __cdecl _wcslwr_s_l(__inout_ecount(_SizeInWords) wchar_t * _P(_Str), __in size_t _P(_SizeInWords), __in_opt _locale_t _P(_PtLoci));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_1(errno_t, _wcslwr_s_l, __inout_ecount(_Size) wchar_t, __in_opt _locale_t)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_1_EX(wchar_t *, __RETURN_POLICY_DST, _CRTIMP, _wcslwr_l, _wcslwr_s_l, __inout wchar_t, __in_opt _locale_t)
_CRTIMP __checkReturn_wat errno_t __cdecl _wcsupr_s(__inout_ecount(_Size) wchar_t * _P(_Str), __in size_t _P(_Size));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_0(errno_t, _wcsupr_s, __inout_ecount(_Size) wchar_t)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_0(wchar_t *, __RETURN_POLICY_DST, _CRTIMP, _wcsupr, __inout wchar_t)
_CRTIMP __checkReturn_wat errno_t __cdecl _wcsupr_s_l(__inout_ecount(_Size) wchar_t * _P(_Str), __in size_t _P(_Size), __in_opt _locale_t _P(_PtLoci));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_1(errno_t, _wcsupr_s_l, __inout_ecount(_size) wchar_t, __in_opt _locale_t)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_1_EX(wchar_t *, __RETURN_POLICY_DST, _CRTIMP, _wcsupr_l, _wcsupr_s_l, __inout wchar_t, __in_opt _locale_t)
_CRTIMP __checkReturn_opt size_t __cdecl wcsxfrm(__out_opt wchar_t * _P(_Dst), __in const wchar_t * _P(_Src), __in size_t _P(_MaxCount));
_CRTIMP __checkReturn_opt size_t __cdecl _wcsxfrm_l(__out_opt wchar_t * _P(_Dst), __in const wchar_t *_P(_Src), __in size_t _P(_MaxCount), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl wcscoll(__in const wchar_t * _P(_Str1), __in const wchar_t * _P(_Str2));
_CRTIMP __checkReturn int __cdecl _wcscoll_l(__in const wchar_t * _P(_Str1), __in const wchar_t * _P(_Str2), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl _wcsicoll(__in const wchar_t * _P(_Str1), __in const wchar_t * _P(_Str2));
_CRTIMP __checkReturn int __cdecl _wcsicoll_l(__in const wchar_t * _P(_Str1), __in const wchar_t *_P(_Str2), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl _wcsncoll(__in const wchar_t * _P(_Str1), __in const wchar_t * _P(_Str2), __in size_t _P(_MaxCount));
_CRTIMP __checkReturn int __cdecl _wcsncoll_l(__in const wchar_t * _P(_Str1), __in const wchar_t * _P(_Str2), __in size_t _P(_MaxCount), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl _wcsnicoll(__in const wchar_t * _P(_Str1), __in const wchar_t * _P(_Str2), __in size_t _P(_MaxCount));
_CRTIMP __checkReturn int __cdecl _wcsnicoll_l(__in const wchar_t * _P(_Str1), __in const wchar_t * _P(_Str2), __in size_t _P(_MaxCount), __in_opt _locale_t _P(_PtLoci));

#ifdef  __cplusplus
#ifndef _CPP_WIDE_INLINES_DEFINED
#define _CPP_WIDE_INLINES_DEFINED
extern "C++" {
inline __checkReturn wchar_t * __CRTDECL wcschr(__in wchar_t *_Str, wchar_t _Ch)
        {return ((wchar_t *)wcschr((const wchar_t *)_Str, _Ch)); }
inline __checkReturn wchar_t * __CRTDECL wcspbrk(__in wchar_t *_Str, __in const wchar_t *_Control)
        {return ((wchar_t *)wcspbrk((const wchar_t *)_Str, _Control)); }
inline __checkReturn wchar_t * __CRTDECL wcsrchr(__in wchar_t *_Str, __in wchar_t _Ch)
        {return ((wchar_t *)wcsrchr((const wchar_t *)_Str, _Ch)); }
inline __checkReturn wchar_t * __CRTDECL wcsstr(__in wchar_t *_Str, __in const wchar_t *_SubStr)
        {return ((wchar_t *)wcsstr((const wchar_t *)_Str, _SubStr)); }
}
#endif
#endif

#if     !__STDC__

#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
#pragma push_macro("wcsdup")
#undef wcsdup
#endif

_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn wchar_t * __cdecl wcsdup(__in const wchar_t * _P(_Str));

#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
#pragma pop_macro("wcsdup")
#endif

/* old names */
#define wcswcs wcsstr

/* prototypes for oldnames.lib functions */
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl wcsicmp(__in const wchar_t * _P(_Str1), __in const wchar_t * _P(_Str2));
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl wcsnicmp(__in const wchar_t * _P(_Str1), __in const wchar_t * _P(_Str2), __in size_t _P(_MaxCount));
_CRT_NONSTDC_DEPRECATE _CRTIMP wchar_t * __cdecl wcsnset(__inout wchar_t * _P(_Str), __in wchar_t _P(_Val), __in size_t _P(_MaxCount));
_CRT_NONSTDC_DEPRECATE _CRTIMP wchar_t * __cdecl wcsrev(__inout wchar_t * _P(_Str));
_CRT_NONSTDC_DEPRECATE _CRTIMP wchar_t * __cdecl wcsset(__inout wchar_t * _P(_Str), wchar_t _P(_Val));
_CRT_NONSTDC_DEPRECATE _CRTIMP wchar_t * __cdecl wcslwr(__inout wchar_t * _P(_Str));
_CRT_NONSTDC_DEPRECATE _CRTIMP wchar_t * __cdecl wcsupr(__inout wchar_t * _P(_Str));
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl wcsicoll(__in const wchar_t * _P(_Str1), __in const wchar_t * _P(_Str2));

#endif  /* !__STDC__ */

#define _WSTRING_DEFINED
#endif


#ifdef  __cplusplus
}
#endif

#endif  /* _INC_STRING */
