/***
*swprintf.inl - inline definitions for (v)swprintf
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file contains the function definitions for (v)swprintf
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#if !defined(__CRTDECL)
#define __CRTDECL   __cdecl
#endif


#ifndef _INC_SWPRINTF_INL_
#define _INC_SWPRINTF_INL_
#ifndef RC_INVOKED

#include <vadefs.h>

static __inline int swprintf(wchar_t * _String, size_t _Count, const wchar_t * _Format, ...)
{
    va_list _Arglist;
    int _Ret;
    _crt_va_start(_Arglist, _Format);
    _Ret = _vswprintf_c_l(_String, _Count, _Format, NULL, _Arglist);
    _crt_va_end(_Arglist);
    return _Ret;
}

static __inline int __CRTDECL vswprintf(wchar_t * _String, size_t _Count, const wchar_t * _Format, va_list _Ap)
{
    return _vswprintf_c_l(_String, _Count, _Format, NULL, _Ap);
}

static __inline int _swprintf_l(wchar_t * _String, size_t _Count, const wchar_t * _Format, _locale_t _Plocinfo, ...)
{
    va_list _Arglist;
    int _Ret;
    _crt_va_start(_Arglist, _Plocinfo);
    _Ret = _vswprintf_c_l(_String, _Count, _Format, _Plocinfo, _Arglist);
    _crt_va_end(_Arglist);
    return _Ret;
}

static __inline int __CRTDECL _vswprintf_l(wchar_t * _String, size_t _Count, const wchar_t * _Format, _locale_t _Plocinfo, va_list _Ap)
{
    return _vswprintf_c_l(_String, _Count, _Format, _Plocinfo, _Ap);
}

#ifdef __cplusplus
#pragma warning( push )
#pragma warning( disable : 4996 )

extern "C++" _SWPRINTFS_DEPRECATED __inline int swprintf(wchar_t * _String, const wchar_t * _Format, ...)
{
    va_list _Arglist;
    _crt_va_start(_Arglist, _Format);
    int _Ret = _vswprintf(_String, _Format, _Arglist);
    _crt_va_end(_Arglist);
    return _Ret;
}

extern "C++" _SWPRINTFS_DEPRECATED __inline int __CRTDECL vswprintf(wchar_t * _String, const wchar_t * _Format, va_list _Ap)
{
    return _vswprintf(_String, _Format, _Ap);
}

#pragma warning( pop )

#endif  /* __cplusplus */

#endif /* RC_INVOKED */
#endif /* _INC_SWPRINTF_INL_ */

