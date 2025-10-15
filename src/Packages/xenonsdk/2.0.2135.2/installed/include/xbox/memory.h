/***
*memory.h - declarations for buffer (memory) manipulation routines
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This include file contains the function declarations for the
*       buffer (memory) manipulation routines.
*       [System V]
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_MEMORY
#define _INC_MEMORY

#include <crtdefs.h>

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef _CONST_RETURN
#ifdef  __cplusplus
#define _CONST_RETURN  const
#define _CRT_CONST_CORRECT_OVERLOADS
#else
#define _CONST_RETURN
#endif
#endif

/* For backwards compatibility */
#define _WConst_return _CONST_RETURN

/* Function prototypes */

	_CRTIMP void *  __cdecl _memccpy( __out_bcount_opt(_MaxCount) void * _P(_Dst), __in const void * _P(_Src), __in int _P(_Val), __in size_t _P(_MaxCount));
_CRTIMP __checkReturn _CONST_RETURN void *  __cdecl memchr( __in_bcount_opt(_MaxCount) const void * _P(_Buf) , __in int _P(_Val), __in size_t _P(_MaxCount));
_CRTIMP __checkReturn int     __cdecl _memicmp(__in_bcount_opt(_Size) const void * _P(_Buf1), __in_bcount_opt(_Size) const void * _P(_Buf2), __in size_t _P(_Size));
        __checkReturn int     __cdecl memcmp(__in_bcount_opt(_Size) const void * _P(_Buf1), __in_bcount_opt(_Size) const void * _P(_Buf2), __in size_t _P(_Size));
        _CRT_INSECURE_DEPRECATE_MEMORY void *  __cdecl memcpy(__out_bcount_full_opt(_Size) void * _P(_Dst), __in_bcount_opt(_Size) const void * _P(_Src), __in size_t _P(_Size));
_CRTIMP errno_t  __cdecl memcpy_s(__out_bcount_part_opt(_DstSize, _MaxCount) void * _P(_Dst), __in size_t _P(_DstSize), __in_bcount_opt(_MaxCount) const void * _P(_Src), __in size_t _P(_MaxCount));
        void *  __cdecl memset(__out_bcount_full_opt(_Size) void * _P(_Dst), __in int _P(_Val), __in size_t _P(_Size));

#ifdef  __cplusplus
#ifndef _CPP_MEMCHR_DEFINED
#define _CPP_MEMCHR_DEFINED
extern "C++" inline __checkReturn void * __CRTDECL memchr( __in_bcount_opt(_N) void * _Pv , __in int _C, __in size_t _N)
	{ return (void*)memchr((const void*)_Pv, _C, _N); }
#endif
#endif

#if     !__STDC__

/* Non-ANSI names for compatibility */

_CRT_NONSTDC_DEPRECATE _CRTIMP void * __cdecl memccpy(__out_bcount_opt(_Size) void * _P(_Dst), __in_bcount_opt(_Size) const void * _P(_Src), __in int _P(_Val), __in size_t _P(_Size));
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl memicmp(__in_bcount_opt(_Size) const void * _P(_Buf1), __in_bcount_opt(_Size) const void * _P(_Buf2), __in size_t _P(_Size));

#endif  /* __STDC__ */

#ifdef  __cplusplus
}
#endif

#endif  /* _INC_MEMORY */
