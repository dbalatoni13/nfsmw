/***
*search.h - declarations for searcing/sorting routines
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file contains the declarations for the sorting and
*       searching routines.
*       [System V]
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_SEARCH
#define _INC_SEARCH

#include <crtdefs.h>
#include <stddef.h>

#ifdef  __cplusplus
extern "C" {
#endif

/* Function prototypes */

_CRTIMP __checkReturn void * __cdecl bsearch_s(__in const void * _P(_Key), __in_bcount(_NumOfElements * _SizeOfElements) const void * _P(_Base), 
        __in size_t _P(_NumofElements), __in size_t _P(_SizeOfElemnts),
        __in int (__cdecl * _P(_PtFuncCompare))(void *, const void *, const void *), void * _P(_Context));
_CRTIMP __checkReturn void * __cdecl bsearch(__in const void * _P(_Key), __in_bcount(_NumOfElements * _SizeOfElemtns) const void * _P(_Base), 
        __in size_t _P(_NumOfElements), __in size_t _P(_SizeOfElements),
        __in int (__cdecl * _P(_PtFuncCompare))(const void *, const void *));

_CRTIMP __checkReturn void * __cdecl _lfind_s(__in const void * _P(_Key), __in_bcount((*_NumOfElements) * _SizeOfElements) const void * _P(_Base),
        __inout unsigned int * _P(_NumOfElements), __in size_t _P(_SizeOfElements), 
        __in int (__cdecl * _P(_PtFuncCompare))(void *, const void *, const void *), void * _P(_Context));
_CRTIMP __checkReturn void * __cdecl _lfind(__in const void * _P(_Key), __in_bcount((*_NumOfElements) * _SizeOfElements) const void * _P(_Base), 
        __inout unsigned int * _P(_NumOfElements), __in unsigned int _P(_SizeOfElements), 
	__in int (__cdecl * _P(_PtFuncCompare))(const void *, const void *));

_CRTIMP __checkReturn void * __cdecl _lsearch_s(__in const void * _P(_Key), __inout_bcount((*_NumOfElements ) * _SizeOfElements) void  * _P(_Base), 
        __inout unsigned int * _P(_NumOfElements), __in size_t _P(_SizeofElements), 
	__in int (__cdecl * _P(_PtFuncCompare))(void *, const void *, const void *), void * _P(_Context));
_CRTIMP __checkReturn void * __cdecl _lsearch(__in const void * _P(_Key), __inout_bcount((*_NumOfElements ) * _SizeOfElements) void  * _P(_Base), 
        __inout unsigned int * _P(_NumOfElements), __in unsigned int _P(_SizeOfElements), 
	__in int (__cdecl * _P(_PtFuncCompare))(const void *, const void *));

_CRTIMP void __cdecl qsort_s(__inout_bcount(_NumOfElements* _SizeOfElements) void * _P(_Base), 
        __in size_t _P(_NumOfElements), __in size_t _P(_SizeOfElements),
        __in int (__cdecl * _P(_PtFuncCompare))(void *, const void *, const void *), void *_P(_Context));
_CRTIMP void __cdecl qsort(__inout_bcount(_NumOfElements * _SizeOfElements) void * _P(_Base), 
	__in size_t _P(_NumOfElements), __in size_t _P(_SizeOfElements), 
        __in int (__cdecl * _P(_PtFuncCompare))(const void *, const void *));


#if _MSC_VER >= 1400 && defined(__cplusplus) && defined(_M_CEE)
/*
 * Managed search routines. Note __cplusplus, this is because we only support
 * managed C++.
 */
extern "C++"
{

__checkReturn void * __clrcall bsearch_s(__in const void * _P(_Key), __in_bcount(_NumOfElements*_SizeOfElements) const void * _P(_Base), 
        __in size_t _P(_NumOfElements), __in size_t _P(_SizeOfElements), 
        __in int (__clrcall * _P(_PtFuncCompare))(void *, const void *, const void *), void * _P(_Context));
__checkReturn void * __clrcall bsearch(__in const void * _P(_Key), __in_bcount(_NumOfElements*_SizeOfElements) const void * _P(_Base), 
        __in size_t _P(_NumOfElements), __in size_t _P(_SizeOfElements),
        __in int (__clrcall * _P(_PtFuncCompare))(const void *, const void *));

__checkReturn void * __clrcall _lfind_s(__in const void * _P(_Key), __in_bcount(_NumOfElements*_SizeOfElements) const void * _P(_Base), 
        __inout unsigned int * _P(_NumOfElements), __in unsigned int _P(_SizeOfElements), 
        __in int (__clrcall * _P(_PtFuncCompare))(void *, const void *, const void *), void * _P(_Context));
__checkReturn void * __clrcall _lfind(__in const void * _P(_Key), __in_bcount((*_NumOfElements)*_SizeOfElements) const void * _P(_Base), 
        __inout unsigned int * _P(_NumOfElements), __in unsigned int _P(_SizeOfElements),
        __in int (__clrcall * _P(_PtFuncCompare))(const void *, const void *));

__checkReturn void * __clrcall _lsearch_s(__in const void * _P(_Key), __in_bcount((*_NumOfElements)*_SizeOfElements) void * _P(_Base), 
        __in unsigned int * _P(_NumOfElements), __in unsigned int _P(_SizeOfElements), 
        __in int (__clrcall * _P(_PtFuncCompare))(void *, const void *, const void *), void * _P(_Context));
__checkReturn void * __clrcall _lsearch(__in const void * _P(_Key), __inout_bcount((*_NumOfElements)*_SizeOfElements) void * _P(_Base), 
        __inout unsigned int * _P(_NumOfElements), __in unsigned int _P(_SizeOfElements),
        __in int (__clrcall * _P(_PtFuncCompare))(const void *, const void *));

void __clrcall qsort_s(__inout_bcount_full(_NumOfElements*_SizeOfElements) void * _P(_Base), 
        __in size_t _P(_NumOfElements), __in size_t _P(_SizeOfElment), 
        __in int (__clrcall * _P(_PtFuncCompare))(void *, const void *, const void *), void * _P(_Context));
void __clrcall qsort(__inout_bcount_full(_NumOfElements*_SizeOfElements) void * _P(_Base), 
        __in size_t _P(_NumOfElements), __in size_t _P(_SizeOfElements), 
        __in int (__clrcall * _P(_PtFuncCompare))(const void *, const void *));

}
#endif


#if     !__STDC__
/* Non-ANSI names for compatibility */

_CRTIMP _CRT_NONSTDC_DEPRECATE __checkReturn void * __cdecl lfind(__in const void * _P(_Key), __in_bcount((*_NumOfElements) * _SizeOfElements) const void * _P(_Base),
        __inout unsigned int * _P(_NumOfElements), __in unsigned int _P(_SizeOfElements), 
	__in int (__cdecl * _P(_PtFuncCompare))(const void *, const void *));
_CRTIMP _CRT_NONSTDC_DEPRECATE __checkReturn void * __cdecl lsearch(__in const void * _P(_Key), __inout_bcount((*_NumOfElements) * _SizeOfElements) void  * _P(_Base),
        __inout unsigned int * _P(_NumOfElements), __in unsigned int _P(_SizeOfElements), 
	__in int (__cdecl * _P(_PtFuncCompare))(const void *, const void *));

#endif  /* __STDC__ */


#ifdef  __cplusplus
}
#endif

#endif  /* _INC_SEARCH */
