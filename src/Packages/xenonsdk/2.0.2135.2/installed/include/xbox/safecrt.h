/***
*safecrt.h - secure crt downlevel for windows build
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file contains a subset of the Secure CRT. It is meant to
*       be used in the Windows source tree.
*
****/

#pragma once

/* guard against other includes */
#if !defined(_CRT_ALTERNATIVE_INLINES)
#error "_CRT_ALTERNATIVE_INLINES needs to be defined to use safecrt.h. This will make sure the safecrt functions are not declared in the standard headers."
#endif

#if defined(_CRT_ALTERNATIVE_IMPORTED)
#error "_CRT_ALTERNATIVE_IMPORTED is defined. This means some files were included with _CRT_ALTERNATIVE_INLINES undefined."
#endif

#if !defined(_INC_SAFECRT)
#define _INC_SAFECRT

/* REVIEW (alecont): Should we guard this with _SAFECRT_NO_INCLUDES as well? */
#include <stdarg.h>     /* for va_start, etc. */

/* _SAFECRT switches */
#if !defined(_SAFECRT_USE_INLINES)
#define _SAFECRT_USE_INLINES 0
#endif

#if !defined(_SAFECRT_SET_ERRNO)
#define _SAFECRT_SET_ERRNO 1
#endif

#if !defined(_SAFECRT_DEFINE_TCS_MACROS)
#define _SAFECRT_DEFINE_TCS_MACROS 0
#endif

#if !defined(_SAFECRT_DEFINE_MBS_FUNCTIONS)
#define _SAFECRT_DEFINE_MBS_FUNCTIONS 1
#endif

#if !defined(_SAFECRT_USE_CPP_OVERLOADS)
#define _SAFECRT_USE_CPP_OVERLOADS 0
#endif

#if !defined(_SAFECRT_FILL_BUFFER)
#if defined(_DEBUG)
#define _SAFECRT_FILL_BUFFER 1
#else
#define _SAFECRT_FILL_BUFFER 0
#endif
#endif

#if !defined(_SAFECRT_FILL_BUFFER_PATTERN)
#define _SAFECRT_FILL_BUFFER_PATTERN 0xFD
#endif

#if !defined(_SAFECRT_INVALID_PARAMETER_DEBUG_INFO)
#define _SAFECRT_INVALID_PARAMETER_DEBUG_INFO 0
#endif

/* additional includes */
#if _SAFECRT_USE_INLINES && !defined(_SAFECRT_NO_INCLUDES)
#include <stdlib.h>     /* for _MAX_DRIVE */
#include <string.h>     /* for memset */
#include <windows.h>    /* for NTSTATUS, RaiseException */
#if _SAFECRT_SET_ERRNO
#include <errno.h>
#endif
#if _SAFECRT_DEFINE_MBS_FUNCTIONS
#include <mbctype.h>
#endif
#endif

/* NULL */
#if !defined(NULL)
#if !defined(__cplusplus)
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

/* wchar_t */
#if !defined(_WCHAR_T_DEFINED)
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif

/* _W64 */
#if !defined(_W64)
#if !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && _MSC_VER >= 1300
#define _W64 __w64
#else
#define _W64
#endif
#endif

/* size_t */
#if !defined(_SIZE_T_DEFINED)
#if defined(_WIN64)
typedef unsigned __int64    size_t;
#else
typedef _W64 unsigned int   size_t;
#endif
#define _SIZE_T_DEFINED
#endif

/* uintptr_t */
#if !defined(_UINTPTR_T_DEFINED)
#if defined(_WIN64)
typedef unsigned __int64    uintptr_t;
#else
typedef _W64 unsigned int   uintptr_t;
#endif
#define _UINTPTR_T_DEFINED
#endif

/* errno_t */
#if !defined(_ERRCODE_DEFINED)
#define _ERRCODE_DEFINED
/* errcode is deprecated in favor or errno_t, which is part of the standard proposal */
__declspec(deprecated) typedef int errcode;
typedef int errno_t; /* standard */
#endif

/* error codes */
#if !defined(_SECURECRT_ERRCODE_VALUES_DEFINED)
#define _SECURECRT_ERRCODE_VALUES_DEFINED
#if !defined(EINVAL)
#define EINVAL          22
#endif
#if !defined(ERANGE)
#define ERANGE          34
#endif
#if !defined(EILSEQ)
#define EILSEQ          42
#endif
#if !defined(STRUNCATE)
#define STRUNCATE       80
#endif
#endif

/* _TRUNCATE */
#if !defined(_TRUNCATE)
#define _TRUNCATE ((size_t)-1)
#endif

/* _SAFECRT_AUTOMATICALLY_REPLACED_CALL */
#if !defined(_SAFECRT_AUTOMATICALLY_REPLACED_CALL)
#define _SAFECRT_AUTOMATICALLY_REPLACED_CALL(v) (v)
#endif

/* internal macros */
#if _SAFECRT_USE_INLINES
#define _SAFECRT__EXTERN_C
#else
#if defined(__cplusplus)
#define _SAFECRT__EXTERN_C extern "C"
#else
#define _SAFECRT__EXTERN_C extern
#endif
#endif /* _SAFECRT_USE_INLINES */

#if !defined(_SAFECRT_IMPL)

#define _SAFECRT__STR2WSTR(str)     L##str

#define _SAFECRT__STR2WSTR2(str)    _SAFECRT__STR2WSTR(str)

#if !defined(__FILEW__)
#define __FILEW__                   _SAFECRT__STR2WSTR2(__FILE__)
#endif

#if !defined(__FUNCTIONW__)
#define __FUNCTIONW__               _SAFECRT__STR2WSTR2(__FUNCTION__)
#endif

#endif

/* validation macros */
#if !defined(_SAFECRT_INVALID_PARAMETER)
#if _SAFECRT_INVALID_PARAMETER_DEBUG_INFO
#define _SAFECRT_INVALID_PARAMETER(message) _invalid_parameter(message, __FUNCTIONW__, __FILEW__, __LINE__, 0)
#else
#define _SAFECRT_INVALID_PARAMETER(message) _invalid_parameter(NULL, NULL, NULL, 0, 0)
#endif
#endif

#if !defined(_SAFECRT__SET_ERRNO)
#if _SAFECRT_SET_ERRNO
#define _SAFECRT__SET_ERRNO(_ErrorCode) errno = (_ErrorCode)
#else
#define _SAFECRT__SET_ERRNO(_ErrorCode)
#endif
#endif

#if !defined(_SAFECRT__RETURN_ERROR)
#define _SAFECRT__RETURN_ERROR(_Msg, _Ret) \
    _SAFECRT__SET_ERRNO(EINVAL); \
    _SAFECRT_INVALID_PARAMETER(_Msg); \
    return _Ret
#endif

#if !defined(_SAFECRT__VALIDATE_STRING_ERROR)
#define _SAFECRT__VALIDATE_STRING_ERROR(_String, _Size, _Ret) \
    if ((_String) == NULL || (_Size) == 0) \
    { \
        _SAFECRT__SET_ERRNO(EINVAL); \
        _SAFECRT_INVALID_PARAMETER(L"String " _SAFECRT__STR2WSTR(#_String) L" is invalid"); \
        return _Ret; \
    }
#endif

#if !defined(_SAFECRT__VALIDATE_STRING)
#define _SAFECRT__VALIDATE_STRING(_String, _Size) _SAFECRT__VALIDATE_STRING_ERROR(_String, _Size, EINVAL)
#endif

#if !defined(_SAFECRT__VALIDATE_POINTER_ERROR_RETURN)
#define _SAFECRT__VALIDATE_POINTER_ERROR_RETURN(_Pointer, _ErrorCode, _Ret) \
    if ((_Pointer) == NULL) \
    { \
        _SAFECRT__SET_ERRNO(_ErrorCode); \
        _SAFECRT_INVALID_PARAMETER(L"Pointer " _SAFECRT__STR2WSTR(#_Pointer) L" is invalid"); \
        return _Ret; \
    }
#endif

#if !defined(_SAFECRT__VALIDATE_POINTER_ERROR)
#define _SAFECRT__VALIDATE_POINTER_ERROR(_Pointer, _Ret) \
    _SAFECRT__VALIDATE_POINTER_ERROR_RETURN(_Pointer, EINVAL, _Ret)
#endif

#if !defined(_SAFECRT__VALIDATE_POINTER)
#define _SAFECRT__VALIDATE_POINTER(_Pointer) \
    _SAFECRT__VALIDATE_POINTER_ERROR(_Pointer, EINVAL)
#endif

#if !defined(_SAFECRT__VALIDATE_POINTER_RESET_STRING_ERROR)
#define _SAFECRT__VALIDATE_POINTER_RESET_STRING_ERROR(_Pointer, _String, _Size, _Ret) \
    if ((_Pointer) == NULL) \
    { \
        _SAFECRT__SET_ERRNO(EINVAL); \
        _SAFECRT__RESET_STRING(_String, _Size); \
        _SAFECRT_INVALID_PARAMETER(L"Pointer " _SAFECRT__STR2WSTR(#_Pointer) L" is invalid"); \
        return _Ret; \
    }
#endif

#if !defined(_SAFECRT__VALIDATE_POINTER_RESET_STRING)
#define _SAFECRT__VALIDATE_POINTER_RESET_STRING(_Pointer, _String, _Size) \
    _SAFECRT__VALIDATE_POINTER_RESET_STRING_ERROR(_Pointer, _String, _Size, EINVAL)
#endif

#if !defined(_SAFECRT__VALIDATE_CONDITION_ERROR_RETURN)
#define _SAFECRT__VALIDATE_CONDITION_ERROR_RETURN(_Condition, _ErrorCode, _Ret) \
    if (!(_Condition)) \
    { \
        _SAFECRT__SET_ERRNO(_ErrorCode); \
        _SAFECRT_INVALID_PARAMETER(_SAFECRT__STR2WSTR(#_Condition)); \
        return _Ret; \
    }
#endif

#if !defined(_SAFECRT__VALIDATE_CONDITION_ERROR)
#define _SAFECRT__VALIDATE_CONDITION_ERROR(_Condition, _Ret) \
    _SAFECRT__VALIDATE_CONDITION_ERROR_RETURN(_Condition, EINVAL, _Ret)
#endif

/* if _SAFECRT_FILL_BUFFER is on, fill the interval [_Offset, _Size) with _SAFECRT_FILL_BUFFER_PATTERN;
 * assume that the string has been validated with _SAFECRT__VALIDATE_STRING
 */
#if !defined(_SAFECRT__FILL_STRING)
#if _SAFECRT_FILL_BUFFER
#define _SAFECRT__FILL_STRING(_String, _Size, _Offset) \
    if ((size_t)(_Offset) < (_Size)) \
    { \
        memset((_String) + (_Offset), _SAFECRT_FILL_BUFFER_PATTERN, ((_Size) - (_Offset)) * sizeof(*(_String))); \
    }
#else
#define _SAFECRT__FILL_STRING(_String, _Size, _Offset)
#endif
#endif

/* if _SAFECRT_FILL_BUFFER is on, set the byte to _SAFECRT_FILL_BUFFER_PATTERN
 */
#if !defined(_SAFECRT__FILL_BYTE)
#if _SAFECRT_FILL_BUFFER
#define _SAFECRT__FILL_BYTE(_Position) \
    (_Position) = _SAFECRT_FILL_BUFFER_PATTERN
#else
#define _SAFECRT__FILL_BYTE(_Position)
#endif
#endif

/* put a null terminator at the beginning of the string and then calls _SAFECRT__FILL_STRING; 
 * assume that the string has been validated with _SAFECRT__VALIDATE_STRING
 */
#if !defined(_SAFECRT__RESET_STRING)
#define _SAFECRT__RESET_STRING(_String, _Size) \
    *(_String) = 0; \
    _SAFECRT__FILL_STRING(_String, _Size, 1);
#endif

#if !defined(_SAFECRT__RETURN_BUFFER_TOO_SMALL_ERROR)
#define _SAFECRT__RETURN_BUFFER_TOO_SMALL_ERROR(_String, _Size, _Ret) \
    _SAFECRT__SET_ERRNO(ERANGE); \
    _SAFECRT_INVALID_PARAMETER(L"Buffer " _SAFECRT__STR2WSTR(#_String) L" is too small"); \
    return _Ret;
#endif

#if !defined(_SAFECRT__RETURN_BUFFER_TOO_SMALL)
#define _SAFECRT__RETURN_BUFFER_TOO_SMALL(_String, _Size) \
    _SAFECRT__RETURN_BUFFER_TOO_SMALL_ERROR(_String, _Size, ERANGE)
#endif

#if !defined(_SAFECRT__RETURN_DEST_NOT_NULL_TERMINATED)
#define _SAFECRT__RETURN_DEST_NOT_NULL_TERMINATED(_String, _Size) \
    _SAFECRT__SET_ERRNO(EINVAL); \
    _SAFECRT_INVALID_PARAMETER(L"String " _SAFECRT__STR2WSTR(#_String) L" is not terminated"); \
    return EINVAL;
#endif

#if !defined(_SAFECRT__RETURN_EINVAL)
#define _SAFECRT__RETURN_EINVAL \
    _SAFECRT__SET_ERRNO(EINVAL); \
    _SAFECRT_INVALID_PARAMETER(L"Invalid parameter"); \
    return EINVAL;
#endif

/* MBCS handling: change these definitions if you do not need to support mbcs strings */
#if !defined(_SAFECRT__ISMBBLEAD)
#define _SAFECRT__ISMBBLEAD(_Character) \
    _ismbblead(_Character)
#endif

#if !defined(_SAFECRT__MBSDEC)
#define _SAFECRT__MBSDEC(_String, _Current) \
    _mbsdec(_String, _Current)
#endif

_SAFECRT__EXTERN_C
void __cdecl _invalid_parameter(__in_opt const wchar_t *_Message, __in_opt const wchar_t *_FunctionName, __in_opt const wchar_t *_FileName, __in unsigned int _LineNumber, __in uintptr_t _Reserved);

#if _SAFECRT_USE_INLINES && !defined(_SAFECRT_DO_NOT_DEFINE_INVALID_PARAMETER)

#ifndef STATUS_INVALID_PARAMETER
typedef LONG NTSTATUS;
#define STATUS_INVALID_PARAMETER ((NTSTATUS)0xC000000DL)
#endif

__inline
void __cdecl _invalid_parameter(__in_opt const wchar_t *_Message, __in_opt const wchar_t *_FunctionName, __in_opt const wchar_t *_FileName, __in unsigned int _LineNumber, __in uintptr_t _Reserved)
{
    (_Message);
    (_FunctionName);
    (_FileName);
    (_LineNumber);
    (_Reserved);
    /* invoke Watson */
    RaiseException(STATUS_INVALID_PARAMETER, 0, 0, NULL);
}

#endif

#if !defined(_SAFECRT_IMPL)

#if _SAFECRT_DEFINE_TCS_MACROS

/* _tcs macros */
#elif defined(_UNICODE)

#define _tcscpy_s       wcscpy_s
#define _tcsncpy_s      wcsncpy_s
#define _tcscat_s       wcscat_s
#define _tcsncat_s      wcsncat_s
#define _tcsset_s       _wcsset_s
#define _tcsnset_s      _wcsnset_s
#define _tcstok_s       wcstok_s
#define _tmakepath_s    _wmakepath_s
#define _tsplitpath_s   _wsplitpath_s
#define _stprintf_s     swprintf_s
#define _vstprintf_s    vswprintf_s
#define _sntprintf_s    _snwprintf_s
#define _vsntprintf_s   _vsnwprintf_s
#define _tscanf_s       wscanf_s
#define _tsscanf_s      swscanf_s
#define _tsnscanf_s     _swnscanf_s

#elif defined(_MBCS)

#define _tcscpy_s       _mbscpy_s
#define _tcsncpy_s      _mbsnbcpy_s
#define _tcscat_s       _mbscat_s
#define _tcsncat_s      _mbsnbcat_s
#define _tcsset_s       _mbsset_s
#define _tcsnset_s      _mbsnbset_s
#define _tcstok_s       _mbstok_s
#define _tmakepath_s    _makepath_s
#define _tsplitpath_s   _splitpath_s
#define _stprintf_s     sprintf_s
#define _vstprintf_s    vsprintf_s
#define _sntprintf_s    _snprintf_s
#define _vsntprintf_s   _vsnprintf_s
#define _tscanf_s       scanf_s
#define _tsscanf_s      sscanf_s
#define _tsnscanf_s     _snscanf_s

#endif

#endif /* _SAFECRT_DEFINE_TCS_MACROS */

/* strcpy_s */
/* 
 * strcpy_s, wcscpy_s copy string _Src into _Dst;
 * will call _SAFECRT_INVALID_PARAMETER if string _Src does not fit into _Dst
 */

_SAFECRT__EXTERN_C
errno_t __cdecl strcpy_s(__out_ecount(_SizeInBytes) char *_Dst, __in size_t _SizeInBytes, __in const char *_Src);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInBytes>
inline
errno_t __cdecl strcpy_s(__out_ecount(_SizeInBytes) char (&_Dst)[_SizeInBytes], __in const char *_Src)
{
    return strcpy_s(_Dst, _SizeInBytes, _Src);
}
#endif

#if _SAFECRT_USE_INLINES
##define _VALIDATE_STRING _SAFECRT__VALIDATE_STRING
##define _VALIDATE_POINTER _SAFECRT__VALIDATE_POINTER
##define _VALIDATE_POINTER_RESET_STRING _SAFECRT__VALIDATE_POINTER_RESET_STRING
##define _VALIDATE_STRING_ERROR _SAFECRT__VALIDATE_STRING_ERROR
##define _VALIDATE_POINTER_ERROR _SAFECRT__VALIDATE_POINTER_ERROR
##define _VALIDATE_POINTER_ERROR_RETURN _SAFECRT__VALIDATE_POINTER_ERROR_RETURN
##define _VALIDATE_POINTER_RESET_STRING_ERROR _SAFECRT__VALIDATE_POINTER_RESET_STRING_ERROR
##define _VALIDATE_CONDITION_ERROR _SAFECRT__VALIDATE_CONDITION_ERROR
##define _VALIDATE_CONDITION_ERROR_RETURN _SAFECRT__VALIDATE_CONDITION_ERROR_RETURN
##define _RESET_STRING _SAFECRT__RESET_STRING
##define _FILL_STRING _SAFECRT__FILL_STRING
##define _FILL_BYTE _SAFECRT__FILL_BYTE
##define _RETURN_BUFFER_TOO_SMALL_ERROR _SAFECRT__RETURN_BUFFER_TOO_SMALL_ERROR
##define _RETURN_BUFFER_TOO_SMALL _SAFECRT__RETURN_BUFFER_TOO_SMALL
##define _RETURN_DEST_NOT_NULL_TERMINATED _SAFECRT__RETURN_DEST_NOT_NULL_TERMINATED
##define _RETURN_EINVAL _SAFECRT__RETURN_EINVAL
##define _RETURN_ERROR _SAFECRT__RETURN_ERROR
##define _RETURN_NO_ERROR return 0
##define _RETURN_TRUNCATE return STRUNCATE
##define _RETURN_MBCS_ERROR \
    _SAFECRT__SET_ERRNO(EILSEQ); \
    return EILSEQ
##define _USE_LOCALE_ARG 0
##define _ISMBBLEAD(_Ch) _SAFECRT__ISMBBLEAD(_Ch)
##define _MBSDEC(_String, _Current) _SAFECRT__MBSDEC(_String, _Current)
##define _ASSIGN_IF_NOT_NULL(_Pointer, _Value) \
    if (_Pointer != NULL) \
    { \
        *_Pointer = _Value; \
    }

##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME strcpy_s
##define _CHAR char
##define _DEST _Dst
##define _SIZE _SizeInBytes
##define _SRC _Src
##include "tcscpy_s.inl"
#endif

/* wcscpy_s */
_SAFECRT__EXTERN_C
errno_t __cdecl wcscpy_s(__out_ecount(_SizeInWords) wchar_t *_Dst, __in size_t _SizeInWords, __in const wchar_t *_Src);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInWords>
inline
errno_t __cdecl wcscpy_s(__out_ecount(_SizeInWords) wchar_t (&_Dst)[_SizeInWords], __in const wchar_t *_Src)
{
    return wcscpy_s(_Dst, _SizeInWords, _Src);
}
#endif

#if _SAFECRT_USE_INLINES
##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME wcscpy_s
##define _CHAR wchar_t
##define _DEST _Dst
##define _SIZE _SizeInWords
##define _SRC _Src
##include "tcscpy_s.inl"
#endif

/* _mbscpy_s */
#if _SAFECRT_DEFINE_MBS_FUNCTIONS

_SAFECRT__EXTERN_C
errno_t __cdecl _mbscpy_s(__out_ecount(_SizeInBytes) unsigned char *_Dst, __in size_t _SizeInBytes, __in const unsigned char *_Src);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInBytes>
inline
errno_t __cdecl _mbscpy_s(__out_ecount(_SizeInBytes) unsigned char (&_Dst)[_SizeInBytes], __in const unsigned char *_Src)
{
    return _mbscpy_s(_Dst, _SizeInBytes, _Src);
}
#endif

#if _SAFECRT_USE_INLINES
##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME _mbscpy_s
##define _DEST _Dst
##define _SRC _Src
##include "mbscpy_s.inl"
#endif

#endif /* _SAFECRT_DEFINE_MBS_FUNCTIONS */

/* strncpy_s */
/* 
 * strncpy_s, wcsncpy_s copy at max _Count characters from string _Src into _Dst;
 * string _Dst will always be null-terminated;
 * will call _SAFECRT_INVALID_PARAMETER if there is not enough space in _Dst;
 * if _Count == _TRUNCATE, we will copy as many characters as we can from _Src into _Dst, and 
 *      return STRUNCATE if _Src does not entirely fit into _Dst (we will not call _SAFECRT_INVALID_PARAMETER);
 * if _Count == 0, then (_Dst == NULL && _SizeInBytes == 0) is allowed
 */
_SAFECRT__EXTERN_C
errno_t __cdecl strncpy_s(__out_ecount(_SizeInBytes) char *_Dst, __in size_t _SizeInBytes, __in const char *_Src, __in size_t _Count);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInBytes>
inline
errno_t __cdecl strncpy_s(__out_ecount(_SizeInBytes) char (&_Dst)[_SizeInBytes], __in const char *_Src, __in size_t _Count)
{
    return strncpy_s(_Dst, _SizeInBytes, _Src, _Count);
}
#endif

#if _SAFECRT_USE_INLINES
##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME strncpy_s
##define _CHAR char
##define _DEST _Dst
##define _SIZE _SizeInBytes
##define _SRC _Src
##define _COUNT _Count
##include "tcsncpy_s.inl"
#endif

/* wcsncpy_s */
_SAFECRT__EXTERN_C
errno_t __cdecl wcsncpy_s(__out_ecount(_SizeInWords) wchar_t *_Dst, __in size_t _SizeInWords, __in const wchar_t *_Src, __in size_t _Count);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInWords>
inline
errno_t __cdecl wcsncpy_s(__out_ecount(_SizeInWords) wchar_t (&_Dst)[_SizeInWords], __in const wchar_t *_Src, __in size_t _Count)
{
    return wcsncpy_s(_Dst, _SizeInWords, _Src, _Count);
}
#endif

#if _SAFECRT_USE_INLINES
##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME wcsncpy_s
##define _CHAR wchar_t
##define _DEST _Dst
##define _SIZE _SizeInWords
##define _SRC _Src
##define _COUNT _Count
##include "tcsncpy_s.inl"
#endif

/* _mbsnbcpy_s */
#if _SAFECRT_DEFINE_MBS_FUNCTIONS

_SAFECRT__EXTERN_C
errno_t __cdecl _mbsnbcpy_s(__out_ecount(_SizeInBytes) unsigned char *_Dst, __in size_t _SizeInBytes, __in const unsigned char *_Src, __in size_t _CountInBytes);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInBytes>
inline
errno_t __cdecl _mbsnbcpy_s(__out_ecount(_SizeInBytes) unsigned char (&_Dst)[_SizeInBytes], __in const unsigned char *_Src, __in size_t _CountInBytes)
{
    return _mbsnbcpy_s(_Dst, _SizeInBytes, _Src, _CountInBytes);
}
#endif

#if _SAFECRT_USE_INLINES
##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME _mbsnbcpy_s
##define _DEST _Dst
##define _SRC _Src
##define _COUNT _CountInBytes
##define _COUNT_IN_BYTES 1
##include "mbsncpy_s.inl"
#endif

#endif /* _SAFECRT_DEFINE_MBS_FUNCTIONS */

/* _mbsncpy_s */
#if _SAFECRT_DEFINE_MBS_FUNCTIONS

_SAFECRT__EXTERN_C
errno_t __cdecl _mbsncpy_s(__out_bcount(_SizeInBytes) unsigned char *_Dst, __in size_t _SizeInBytes, __in const unsigned char *_Src, __in size_t _CountInChars);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInBytes>
inline
errno_t __cdecl _mbsncpy_s(__out_bcount(_SizeInBytes) unsigned char (&_Dst)[_SizeInBytes], __in const unsigned char *_Src, __in size_t _CountInChars)
{
    return _mbsncpy_s(_Dst, _SizeInBytes, _Src, _CountInChars);
}
#endif

#if _SAFECRT_USE_INLINES
##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME _mbsncpy_s
##define _DEST _Dst
##define _SRC _Src
##define _COUNT _CountInChars
##define _COUNT_IN_BYTES 0
##include "mbsncpy_s.inl"
#endif

#endif /* _SAFECRT_DEFINE_MBS_FUNCTIONS */

/* strcat_s */
/* 
 * strcat_s, wcscat_s append string _Src to _Dst;
 * will call _SAFECRT_INVALID_PARAMETER if there is not enough space in _Dst
 */
_SAFECRT__EXTERN_C
errno_t __cdecl strcat_s(__inout_ecount(_SizeInBytes) char *_Dst, __in size_t _SizeInBytes, __in const char *_Src);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInBytes>
inline
errno_t __cdecl strcat_s(__inout_ecount(_SizeInBytes) char (&_Dst)[_SizeInBytes], __in const char *_Src)
{
    return strcat_s(_Dst, _SizeInBytes, _Src);
}
#endif

#if _SAFECRT_USE_INLINES
##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME strcat_s
##define _CHAR char
##define _DEST _Dst
##define _SIZE _SizeInBytes
##define _SRC _Src
##include "tcscat_s.inl"
#endif

/* wcscat_s */
_SAFECRT__EXTERN_C
errno_t __cdecl wcscat_s(__inout_ecount(_SizeInWords) wchar_t *_Dst, __in size_t _SizeInWords, __in const wchar_t *_Src);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInWords>
inline
errno_t __cdecl wcscat_s(__out_ecount(_SizeInWords) wchar_t (&_Dst)[_SizeInWords], __in const wchar_t *_Src)
{
    return wcscat_s(_Dst, _SizeInWords, _Src);
}
#endif

#if _SAFECRT_USE_INLINES
##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME wcscat_s
##define _CHAR wchar_t
##define _DEST _Dst
##define _SIZE _SizeInWords
##define _SRC _Src
##include "tcscat_s.inl"
#endif

/* _mbscat_s */
#if _SAFECRT_DEFINE_MBS_FUNCTIONS

_SAFECRT__EXTERN_C
errno_t __cdecl _mbscat_s(__inout_bcount(_SizeInBytes) unsigned char *_Dst, __in size_t _SizeInBytes, __in const unsigned char *_Src);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInBytes>
inline
errno_t __cdecl _mbscat_s(__inout_bcount(_SizeInBytes) unsigned char (&_Dst)[_SizeInBytes], __in const unsigned char *_Src)
{
    return _mbscat_s(_Dst, _SizeInBytes, _Src);
}
#endif

#if _SAFECRT_USE_INLINES
##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME _mbscat_s
##define _DEST _Dst
##define _SRC _Src
##include "mbscat_s.inl"
#endif

#endif /* _SAFECRT_DEFINE_MBS_FUNCTIONS */

/* strncat_s */
/* 
 * strncat_s, wcsncat_s append at max _Count characters from string _Src to _Dst;
 * string _Dst will always be null-terminated;
 * will call _SAFECRT_INVALID_PARAMETER if there is not enough space in _Dst;
 * if _Count == _TRUNCATE, we will append as many characters as we can from _Src to _Dst, and 
 *      return STRUNCATE if _Src does not entirely fit into _Dst (we will not call _SAFECRT_INVALID_PARAMETER);
 * if _Count == 0, then (_Dst == NULL && _SizeInBytes == 0) is allowed
 */
_SAFECRT__EXTERN_C
errno_t __cdecl strncat_s(__inout_ecount(_SizeInBytes) char *_Dst, __in size_t _SizeInBytes, __in const char *_Src, __in size_t _Count);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInBytes>
inline
errno_t __cdecl strncat_s(__inout_ecount(_SizeInBytes) char (&_Dst)[_SizeInBytes], __in const char *_Src, __in size_t _Count)
{
    return strncat_s(_Dst, _SizeInBytes, _Src, _Count);
}
#endif

#if _SAFECRT_USE_INLINES
##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME strncat_s
##define _CHAR char
##define _DEST _Dst
##define _SIZE _SizeInBytes
##define _SRC _Src
##define _COUNT _Count
##include "tcsncat_s.inl"
#endif

/* wcsncat_s */
_SAFECRT__EXTERN_C
errno_t __cdecl wcsncat_s(__inout_ecount(_SizeInWords) wchar_t *_Dst, __in size_t _SizeInWords, __in const wchar_t *_Src, __in size_t _Count);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInWords>
inline
errno_t __cdecl wcsncat_s(__inout_ecount(_SizeInWords) wchar_t (&_Dst)[_SizeInWords], __in const wchar_t *_Src, __in size_t _Count)
{
    return wcsncat_s(_Dst, _SizeInWords, _Src, _Count);
}
#endif

#if _SAFECRT_USE_INLINES
##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME wcsncat_s
##define _CHAR wchar_t
##define _DEST _Dst
##define _SIZE _SizeInWords
##define _SRC _Src
##define _COUNT _Count
##include "tcsncat_s.inl"
#endif

/* _mbsnbcat_s */
#if _SAFECRT_DEFINE_MBS_FUNCTIONS

_SAFECRT__EXTERN_C
errno_t __cdecl _mbsnbcat_s(__inout_bcount(_SizeInBytes) unsigned char *_Dst, __in size_t _SizeInBytes, __in const unsigned char *_Src, __in size_t _CountInBytes);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInBytes>
inline
errno_t __cdecl _mbsnbcat_s(__inout_bcount(_SizeInBytes) unsigned char (&_Dst)[_SizeInBytes], __in const unsigned char *_Src, __in size_t _CountInBytes)
{
    return _mbsnbcat_s(_Dst, _SizeInBytes, _Src, size_t _CountInBytes);
}
#endif

#if _SAFECRT_USE_INLINES
##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME _mbsnbcat_s
##define _DEST _Dst
##define _SRC _Src
##define _COUNT _CountInBytes
##define _COUNT_IN_BYTES 1
##include "mbsncat_s.inl"
#endif

#endif /* _SAFECRT_DEFINE_MBS_FUNCTIONS */

/* _mbsncat_s */
#if _SAFECRT_DEFINE_MBS_FUNCTIONS

_SAFECRT__EXTERN_C
errno_t __cdecl _mbsncat_s(__inout_bcount(_SizeInBytes) unsigned char *_Dst, __in size_t _SizeInBytes, __in const unsigned char *_Src, __in size_t _CountInChars);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInBytes>
inline
errno_t __cdecl _mbsncat_s(__inout_bcount(_SizeInbytes) unsigned char (&_Dst)[_SizeInBytes], __in const unsigned char *_Src, __in size_t _CountInChars)
{
    return _mbsncat_s(_Dst, _SizeInBytes, _Src, size_t _CountInChars);
}
#endif

#if _SAFECRT_USE_INLINES
##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME _mbsncat_s
##define _DEST _Dst
##define _SRC _Src
##define _COUNT _CountInChars
##define _COUNT_IN_BYTES 0
##include "mbsncat_s.inl"
#endif

#endif /* _SAFECRT_DEFINE_MBS_FUNCTIONS */

/* _strset_s */
/* 
 * _strset_s, _wcsset_s ;
 * will call _SAFECRT_INVALID_PARAMETER if _Dst is not null terminated.
 */
_SAFECRT__EXTERN_C
errno_t __cdecl _strset_s(__inout_ecount(_SizeInBytes) char *_Dst, __in size_t _SizeInBytes, __in int _Value);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInBytes>
inline
errno_t __cdecl _strset_s(__inout_ecount(_SizeInBytes) char (&_Dst)[_SizeInBytes], __in int _Value)
{
    return _strset_s(_Dst, _SizeInBytes, _Value);
}
#endif

#if _SAFECRT_USE_INLINES
##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME _strset_s
##define _CHAR char
##define _CHAR_INT int
##define _DEST _Dst
##define _SIZE _SizeInBytes
##include "tcsset_s.inl"
#endif

/* _wcsset_s */
_SAFECRT__EXTERN_C
errno_t __cdecl _wcsset_s(__inout_ecount(_SizeInWords) wchar_t *_Dst, __in size_t _SizeInWords, __in wchar_t _Value);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInWords>
inline
errno_t __cdecl _wcsset_s(__inout_ecount(_SizeInWords) wchar_t (&_Dst)[_SizeInWords], __in wchar_t _Value)
{
    return _wcsset_s(_Dst, _SizeInWords, _Value);
}
#endif

#if _SAFECRT_USE_INLINES
##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME _wcsset_s
##define _CHAR wchar_t
##define _CHAR_INT wchar_t
##define _DEST _Dst
##define _SIZE _SizeInWords
##include "tcsset_s.inl"
#endif

/* _mbsset_s */
#if _SAFECRT_DEFINE_MBS_FUNCTIONS

_SAFECRT__EXTERN_C
errno_t __cdecl _mbsset_s(__inout_bcount(_SizeInBytes) unsigned char *_Dst, __in size_t _SizeInBytes, __in unsigned int _Value);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInBytes>
inline
errno_t __cdecl _mbsset_s(__inout_bcount(_SizeInBytes) unsigned char (&_Dst)[_SizeInBytes], __in unsigned int _Value)
{
    return _mbsset_s(_Dst, _SizeInBytes, _Value);
}
#endif

#if _SAFECRT_USE_INLINES
##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME _mbsset_s
##define _DEST _Dst
##include "mbsset_s.inl"
#endif

#endif /* _SAFECRT_DEFINE_MBS_FUNCTIONS */

/* _strnset_s */
/* 
 * _strnset_s, _wcsnset_s ;
 * will call _SAFECRT_INVALID_PARAMETER if _Dst is not null terminated.
 */
_SAFECRT__EXTERN_C
errno_t __cdecl _strnset_s(__inout_ecount(_SizeInBytes) char *_Dst, __in size_t _SizeInBytes, __in int _Value, __in size_t _Count);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInBytes>
inline
errno_t __cdecl _strnset_s(__inout_ecount(_SizeInBytes) char (&_Dst)[_SizeInBytes], __in int _Value, __in size_t _Count)
{
    return _strnset_s(_Dst, _SizeInBytes, _Value, _Count);
}
#endif

#if _SAFECRT_USE_INLINES
##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME _strnset_s
##define _CHAR char
##define _CHAR_INT int
##define _DEST _Dst
##define _SIZE _SizeInBytes
##define _COUNT _Count
##include "tcsnset_s.inl"
#endif

/* _wcsnset_s */
_SAFECRT__EXTERN_C
errno_t __cdecl _wcsnset_s(__inout_ecount(_SizeInWords) wchar_t *_Dst, __in size_t _SizeInWords, __in wchar_t _Value, __in size_t _Count);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInWords>
inline
errno_t __cdecl _wcsnset_s(__inout_ecount(_SizeInWords) wchar_t (&_Dst)[_SizeInWords], __in wchar_t _Value, __in size_t _Count)
{
    return _wcsnset_s(_Dst, _SizeInWords, _Value, _Count);
}
#endif

#if _SAFECRT_USE_INLINES
##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME _wcsnset_s
##define _CHAR wchar_t
##define _CHAR_INT wchar_t
##define _DEST _Dst
##define _SIZE _SizeInWords
##define _COUNT _Count
##include "tcsnset_s.inl"
#endif

/* _mbsnbset_s */
#if _SAFECRT_DEFINE_MBS_FUNCTIONS

_SAFECRT__EXTERN_C
errno_t __cdecl _mbsnbset_s(__inout_bcount(_SizeInBytes) unsigned char *_Dst, __in size_t _SizeInBytes, __in unsigned int _Value, __in size_t _CountInBytes);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInBytes>
inline
errno_t __cdecl _mbsnbset_s(__inout_bcount(_SizeInBytes) unsigned char (&_Dst)[_SizeInBytes], __in unsigned int _Value, __in size_t _CountInBytes)
{
    return _mbsnbset_s(_Dst, _SizeInBytes, _Value, _CountInBytes);
}
#endif

#if _SAFECRT_USE_INLINES
##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME _mbsnbset_s
##define _DEST _Dst
##define _COUNT _CountInBytes
##define _COUNT_IN_BYTES 1
##include "mbsnset_s.inl"
#endif

#endif /* _SAFECRT_DEFINE_MBS_FUNCTIONS */

/* _mbsnset_s */
#if _SAFECRT_DEFINE_MBS_FUNCTIONS

_SAFECRT__EXTERN_C
errno_t __cdecl _mbsnset_s(__inout_bcount(_SizeInBytes) unsigned char *_Dst, __in size_t _SizeInBytes, __in unsigned int _Value, __in size_t _CountInChars);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInBytes>
inline
errno_t __cdecl _mbsnset_s(__inout_bcount(_SizeInBytes) unsigned char (&_Dst)[_SizeInBytes], __in unsigned int _Value, __in size_t _CountInChars)
{
    return _mbsnset_s(_Dst, _SizeInBytes, _Value, _CountInChars);
}
#endif

#if _SAFECRT_USE_INLINES
##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME _mbsnset_s
##define _DEST _Dst
##define _COUNT _CountInChars
##define _COUNT_IN_BYTES 0
##include "mbsnset_s.inl"
#endif

#endif /* _SAFECRT_DEFINE_MBS_FUNCTIONS */

/* _mbccpy_s */
#if _SAFECRT_DEFINE_MBS_FUNCTIONS

_SAFECRT__EXTERN_C
errno_t __cdecl _mbccpy_s(__out_bcount(_SizeInBytes) unsigned char *_Dst, __in size_t _SizeInBytes, __out_opt int *_PCopied, __in const unsigned char *_Src);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInBytes>
inline
errno_t __cdecl _mbccpy_s(__out_bcount(_SizeInBytes) unsigned char (&_Dst)[_SizeInBytes], __out_opt int *_PCopied, __in const unsigned char *_Src)
{
    return _mbccpy_s(_Dst, _SizeInBytes, _PCopied, _Src);
}
#endif

#if _SAFECRT_USE_INLINES
##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME _mbccpy_s
##define _DEST _Dst
##define _SRC _Src
##include "mbccpy_s.inl"
#endif

#endif /* _SAFECRT_DEFINE_MBS_FUNCTIONS */

/* strtok_s */
/* 
 * strtok_s, wcstok_s ;
 * uses _Context to keep track of the position in the string.
 */
_SAFECRT__EXTERN_C
char * __cdecl strtok_s(__inout_opt char *_String, __in const char *_Control, __deref_inout_opt char **_Context);

#if _SAFECRT_USE_INLINES
##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME strtok_s
##include "strtok_s.inl"
#endif

/* wcstok_s */
_SAFECRT__EXTERN_C
wchar_t * __cdecl wcstok_s(__inout_opt wchar_t *_String, __in const wchar_t *_Control, __deref_inout_opt wchar_t **_Context);

#if _SAFECRT_USE_INLINES
##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME wcstok_s
##define _CHAR wchar_t
##include "tcstok_s.inl"
#endif

/* _mbstok_s */
#if _SAFECRT_DEFINE_MBS_FUNCTIONS

_SAFECRT__EXTERN_C
unsigned char * __cdecl _mbstok_s(__inout_opt unsigned char *_String, __in const unsigned char *_Control, __deref_inout_opt unsigned char **_Context);

#if _SAFECRT_USE_INLINES
##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME _mbstok_s
##include "mbstok_s.inl"
#endif

#endif /* _SAFECRT_DEFINE_MBS_FUNCTIONS */

/* _makepath_s */
/* 
 * _makepath_s, _wmakepath_s build up a path starting from the specified components;
 * will call _SAFECRT_INVALID_PARAMETER if there is not enough space in _Dst;
 * any of _Drive, _Dir, _Filename and _Ext can be NULL
 */
_SAFECRT__EXTERN_C
errno_t __cdecl _makepath_s(__out_ecount(_SizeInBytes) char *_Dst, __in size_t _SizeInBytes, __in_opt const char *_Drive, __in_opt const char *_Dir, __in_opt const char *_Filename, __in_opt const char *_Ext);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInBytes>
inline
errno_t __cdecl _makepath_s(__out_ecount(_SizeInBytes) char (&_Dst)[_SizeInBytes], __in_opt const char *_Drive, __in_opt const char *_Dir, __in_opt const char *_Filename, __in_opt const char *_Ext)
{
    return _makepath_s(_Dst, _SizeInBytes, _Drive, _Dir, _Filename, _Ext);
}
#endif

#if _SAFECRT_USE_INLINES
##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME _makepath_s
##define _CHAR char
##define _DEST _Dst
##define _SIZE _SizeInBytes
##define _T(_Character) _Character
##define _MBS_SUPPORT 1
##include "tmakepath_s.inl"
#endif

/* _wmakepath_s */
_SAFECRT__EXTERN_C
errno_t __cdecl _wmakepath_s(__out_ecount(_SizeInWords) wchar_t *_Dst, __in size_t _SizeInWords, __in_opt const wchar_t *_Drive, __in_opt const wchar_t *_Dir, __in_opt const wchar_t *_Filename, __in_opt const wchar_t *_Ext);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInWords>
inline
errno_t __cdecl _wmakepath_s(__out_ecount(_SizeInWords) wchar_t (&_Dst)[_SizeInWords], __in_opt const wchar_t *_Drive, __in_opt const wchar_t *_Dir, __in_opt const wchar_t *_Filename, __in_opt const wchar_t *_Ext)
{
    return _wmakepath_s(_Dst, _SizeInWords, _Drive, _Dir, _Filename, _Ext);
}
#endif

#if _SAFECRT_USE_INLINES
##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME _wmakepath_s
##define _CHAR wchar_t
##define _DEST _Dst
##define _SIZE _SizeInWords
##define _T(_Character) L##_Character
##define _MBS_SUPPORT 0
##include "tmakepath_s.inl"
#endif

/* _splitpath_s */
/* 
 * _splitpath_s, _wsplitpath_s decompose a path into the specified components;
 * will call _SAFECRT_INVALID_PARAMETER if there is not enough space in
 *      any of _Drive, _Dir, _Filename and _Ext;
 * any of _Drive, _Dir, _Filename and _Ext can be NULL, but the correspondent size must
 *      be set to 0, e.g. (_Drive == NULL && _DriveSize == 0) is allowed, but
 *      (_Drive == NULL && _DriveSize != 0) is considered an invalid parameter
 */
_SAFECRT__EXTERN_C
errno_t __cdecl _splitpath_s(
    __in const char *_Path,
    __out_ecount_opt(_DriveSize) char *_Drive, __in size_t _DriveSize,
    __out_ecount_opt(_DirSize) char *_Dir, __in size_t _DirSize,
    __out_ecount_opt(_FilenameSize) char *_Filename, __in size_t _FilenameSize,
    __out_ecount_opt(_ExtSize) char *_Ext, __in size_t _ExtSize
);

/* no C++ overload for _splitpath_s */

#if _SAFECRT_USE_INLINES
##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME _splitpath_s
##define _CHAR char
##define _TCSNCPY_S strncpy_s
##define _T(_Character) _Character
##define _MBS_SUPPORT 1
##include "tsplitpath_s.inl"
#endif

/* _wsplitpath_s */
_SAFECRT__EXTERN_C
errno_t __cdecl _wsplitpath_s(
    __in const wchar_t *_Path,
    __out_ecount_opt(_DriveSize) wchar_t *_Drive, __in size_t _DriveSize,
    __out_ecount_opt(_DirSize) wchar_t *_Dir, __in size_t _DirSize,
    __out_ecount_opt(_FilenameSize) wchar_t *_Filename, __in size_t _FilenameSize,
    __out_ecount_opt(_ExtSize) wchar_t *_Ext, __in size_t _ExtSize
);

/* no C++ overload for _wsplitpath_s */

#if _SAFECRT_USE_INLINES
##define _FUNC_PROLOGUE __inline
##define _FUNC_NAME _wsplitpath_s
##define _CHAR wchar_t
##define _TCSNCPY_S wcsncpy_s
##define _T(_Character) L##_Character
##define _MBS_SUPPORT 0
##include "tsplitpath_s.inl"
#endif

/* sprintf_s, vsprintf_s */
/* 
 * sprintf_s, swprintf_s, vsprintf_s, vswprintf_s format a string and copy it into _Dst;
 * need safecrt.lib and msvcrt.dll;
 * will call _SAFECRT_INVALID_PARAMETER if there is not enough space in _Dst;
 * will call _SAFECRT_INVALID_PARAMETER if the format string is malformed;
 * the %n format type is not allowed;
 * return the length of string _Dst;
 * return a negative number if something goes wrong with mbcs conversions (we will not call _SAFECRT_INVALID_PARAMETER);
 * _SizeInBytes/_SizeInWords must be <= (INT_MAX / sizeof(char/wchar_t));
 * cannot be used without safecrt.lib
 */
_SAFECRT__EXTERN_C
int __cdecl sprintf_s(__out_ecount(_SizeInBytes) char *_Dst, __in size_t _SizeInBytes, __in __format_string const char *_Format, ...);
_SAFECRT__EXTERN_C
int __cdecl vsprintf_s(__out_ecount(_SizeInBytes) char *_Dst, __in size_t _SizeInBytes, __in __format_string const char *_Format, va_list _ArgList);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInBytes>
inline
int __cdecl sprintf_s(__out_ecount(_SizeInBytes) char (&_Dst)[_SizeInBytes], __in __format_string const char *_Format, ...)
{
    va_list _ArgList;
    va_start(_ArgList, _Format);
    return vsprintf_s(_Dst, _SizeInBytes, _Format, _ArgList);
}

template <size_t _SizeInBytes>
inline
int __cdecl vsprintf_s(__out_ecount(_SizeInBytes) char (&_Dst)[_SizeInBytes], __in __format_string const char *_Format, va_list _ArgList)
{
    return vsprintf_s(_Dst, _SizeInBytes, _Format, _ArgList);
}
#endif

/* no inline version of sprintf_s, vsprintf_s */

/* swprintf_s, vswprintf_s */
_SAFECRT__EXTERN_C
int __cdecl swprintf_s(__out_ecount(_SizeInWords) wchar_t *_Dst, __in size_t _SizeInWords, __in __format_string const wchar_t *_Format, ...);
_SAFECRT__EXTERN_C
int __cdecl vswprintf_s(__out_ecount(_SizeInWords) wchar_t *_Dst, __in size_t _SizeInWords, __in __format_string const wchar_t *_Format, va_list _ArgList);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInWords>
inline
int __cdecl swprintf_s(__out_ecount(_SizeInWords) char (&_Dst)[_SizeInWords], __in __format_string const char *_Format, ...)
{
    va_list _ArgList;
    va_start(_ArgList, _Format);
    return vswprintf_s(_Dst, _SizeInWords, _Format, _ArgList);
}

template <size_t _SizeInWords>
inline
int __cdecl vswprintf_s(__out_ecount(_SizeInWords) char (&_Dst)[_SizeInWords], __in __format_string const char *_Format, va_list _ArgList)
{
    return vswprintf_s(_Dst, _SizeInWords, _Format, _ArgList);
}
#endif

/* no inline version of swprintf_s, vswprintf_s */

/* _snprintf_s, _vsnprintf_s */
/* 
 * _snprintf_s, _snwprintf_s, _vsnprintf_s, _vsnwprintf_s format a string and copy at max _Count characters into _Dst;
 * need safecrt.lib and msvcrt.dll;
 * string _Dst will always be null-terminated;
 * will call _SAFECRT_INVALID_PARAMETER if there is not enough space in _Dst;
 * will call _SAFECRT_INVALID_PARAMETER if the format string is malformed;
 * the %n format type is not allowed;
 * return the length of string _Dst;
 * return a negative number if something goes wrong with mbcs conversions (we will not call _SAFECRT_INVALID_PARAMETER);
 * _SizeInBytes/_SizeInWords must be <= (INT_MAX / sizeof(char/wchar_t));
 * cannot be used without safecrt.lib;
 * if _Count == _TRUNCATE, we will copy into _Dst as many characters as we can, and 
 *      return -1 if the formatted string does not entirely fit into _Dst (we will not call _SAFECRT_INVALID_PARAMETER);
 * if _Count == 0, then (_Dst == NULL && _SizeInBytes == 0) is allowed
 */
_SAFECRT__EXTERN_C
int __cdecl _snprintf_s(__out_ecount(_SizeInBytes) char *_Dst, __in size_t _SizeInBytes, __in size_t _Count, __in __format_string const char *_Format, ...);
_SAFECRT__EXTERN_C
int __cdecl _vsnprintf_s(__out_ecount(_SizeInBytes) char *_Dst, __in size_t _SizeInBytes, __in size_t _Count, __in __format_string const char *_Format, va_list _ArgList);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInBytes>
inline
int __cdecl _snprintf_s(__out_ecount(_SizeInBytes) char (&_Dst)[_SizeInBytes], __in size_t _Count, __in __format_string const char *_Format, ...)
{
    va_list _ArgList;
    va_start(_ArgList, _Format);
    return _vsnprintf_s(_Dst, _SizeInBytes, _Count, _Format, _ArgList);
}

template <size_t _SizeInBytes>
inline
int __cdecl _vsnprintf_s(__out_ecount(_SizeInBytes) char (&_Dst)[_SizeInBytes], __in size_t _Count, __in __format_string const char *_Format, va_list _ArgList)
{
    return _vsnprintf_s(_Dst, _SizeInBytes, _Count, _Format, _ArgList);
}
#endif

/* no inline version of _snprintf_s, _vsnprintf_s */

/* _snwprintf_s, _vsnwprintf_s */
_SAFECRT__EXTERN_C
int __cdecl _snwprintf_s(__out_ecount(_SizeInWords) wchar_t *_Dst, __in size_t _SizeInWords, __in size_t _Count, __in __format_string const wchar_t *_Format, ...);
_SAFECRT__EXTERN_C
int __cdecl _vsnwprintf_s(__out_ecount(_SizeInWords) wchar_t *_Dst, __in size_t _SizeInWords, __in size_t _Count, __in __format_string const wchar_t *_Format, va_list _ArgList);

#if defined(__cplusplus) && _SAFECRT_USE_CPP_OVERLOADS
template <size_t _SizeInWords>
inline
int __cdecl _snwprintf_s(__out_ecount(_SizeInWords) char (&_Dst)[_SizeInWords], __in size_t _Count, __in __format_string const char *_Format, ...)
{
    va_list _ArgList;
    va_start(_ArgList, _Format);
    return _vsnwprintf_s(_Dst, _SizeInWords, _Count, _Format, _ArgList);
}

template <size_t _SizeInWords>
inline
int __cdecl _vsnwprintf_s(__out_ecount(_SizeInWords) char (&_Dst)[_SizeInWords], __in size_t _Count, __in __format_string const char *_Format, va_list _ArgList)
{
    return _vsnwprintf_s(_Dst, _SizeInWords, _Count, _Format, _ArgList);
}
#endif

/* no inline version of _snwprintf_s, _vsnwprintf_s */

/* scanf_s */
/*
 * read formatted data from the standard input stream;
 * need safecrt.lib and msvcrt.dll;
 * will call _SAFECRT_INVALID_PARAMETER if the format string is malformed;
 * for format types %s, %S, %[, %c and %C, in the argument list the buffer pointer
 *      need to be followed by the size of the buffer, e.g.:
 *          #define BUFFSIZE 100
 *          char buff[BUFFSIZE];
 *          scanf_s("%s", buff, BUFFSIZE);
 * as scanf, returns the number of fields successfully converted and assigned;
 * if a buffer field is too small, scanf set the buffer to the empty string and returns.
 * do not support floating-point, for now
 */
_SAFECRT__EXTERN_C
int __cdecl scanf_s(__in __format_string const char *_Format, ...);

/* no C++ overload for scanf_s */

/* no inline version of scanf_s */

/* wscanf_s */
_SAFECRT__EXTERN_C
int __cdecl wscanf_s(__in __format_string const wchar_t *_Format, ...);

/* no C++ overload for wscanf_s */

/* no inline version of wscanf_s */

/* sscanf_s */
_SAFECRT__EXTERN_C
int __cdecl sscanf_s(__in const char *_String, __in __format_string const char *_Format, ...);

/* no C++ overload for sscanf_s */

/* no inline version of sscanf_s */

/* swscanf_s */
_SAFECRT__EXTERN_C
int __cdecl swscanf_s(__in const wchar_t *_String, __in __format_string const wchar_t *_Format, ...);

/* no C++ overload for swscanf_s */

/* no inline version of swscanf_s */

/* _snscanf_s */
_SAFECRT__EXTERN_C
int __cdecl _snscanf_s(__in_ecount(_Count) const char *_String, size_t _Count, __in __format_string const char *_Format, ...);

/* no C++ overload for snscanf_s */

/* no inline version of snscanf_s */

/* _swnscanf_s */
_SAFECRT__EXTERN_C
int __cdecl _swnscanf_s(__in_ecount(_Count) const wchar_t *_String, __in size_t _Count, __in __format_string const wchar_t *_Format, ...);

/* no C++ overload for _swnscanf_s */

/* no inline version of _swnscanf_s */

#endif /* ndef _SAFECRT_IMPL */

#endif  /* _INC_SAFECRT */
