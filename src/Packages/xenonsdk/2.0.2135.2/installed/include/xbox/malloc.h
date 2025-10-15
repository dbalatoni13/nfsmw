/***
*malloc.h - declarations and definitions for memory allocation functions
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Contains the function declarations for memory allocation functions;
*       also defines manifest constants and types used by the heap routines.
*       [System V]
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_MALLOC
#define _INC_MALLOC

#include <crtdefs.h>

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

/* Maximum heap request the heap manager will attempt */

#ifdef  _WIN64
#define _HEAP_MAXREQ    0xFFFFFFFFFFFFFFE0
#elif defined(_XBOX)
#define _HEAP_MAXREQ    0xFFFFF000
#endif

/* _STATIC_ASSERT is for enforcing boolean/integral conditions at compile time. */

#ifndef _STATIC_ASSERT
#define _STATIC_ASSERT(expr) typedef char __static_assert_t[ (expr) ]
#endif

/* Constants for _heapchk/_heapset/_heapwalk routines */

#define _HEAPEMPTY      (-1)
#define _HEAPOK         (-2)
#define _HEAPBADBEGIN   (-3)
#define _HEAPBADNODE    (-4)
#define _HEAPEND        (-5)
#define _HEAPBADPTR     (-6)
#define _FREEENTRY      0
#define _USEDENTRY      1

#ifndef _HEAPINFO_DEFINED
typedef struct _heapinfo {
        int * _pentry;
        size_t _size;
        int _useflag;
        } _HEAPINFO;
#define _HEAPINFO_DEFINED
#endif

/* External variable declarations */

extern _CRT_INSECURE_DEPRECATE unsigned int _amblksiz;

#define _mm_free(a)      _aligned_free(a)
#define _mm_malloc(a, b)    _aligned_malloc(a, b)

/* Function prototypes */

#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
#pragma push_macro("calloc")
#pragma push_macro("free")
#pragma push_macro("malloc")
#pragma push_macro("realloc")
#pragma push_macro("_aligned_free")
#pragma push_macro("_aligned_malloc")
#pragma push_macro("_aligned_offset_malloc")
#pragma push_macro("_aligned_realloc")
#pragma push_macro("_aligned_offset_realloc")
#pragma push_macro("_freea_s")
#undef calloc
#undef free
#undef malloc
#undef realloc
#undef _aligned_free
#undef _aligned_malloc
#undef _aligned_offset_malloc
#undef _aligned_realloc
#undef _aligned_offset_realloc
#undef _freea_s
#endif

_CRTIMP _CRTNOALIAS _CRTRESTRICT __checkReturn __bcount_opt(_NumOfElements* _SizeOfElemnts) void * __cdecl calloc(__in size_t _P(_NumOfElements), __in size_t _P(_SizeOfElements));;
_CRTIMP _CRTNOALIAS void   __cdecl free(__inout_opt void * _P(_Memory));;
_CRTIMP _CRTNOALIAS _CRTRESTRICT __checkReturn __bcount_opt(_Size) void * __cdecl malloc(__in size_t _P(_Size));;
_CRTIMP _CRTNOALIAS _CRTRESTRICT __checkReturn __bcount_opt(_NewSize) void *  __cdecl realloc(__in_opt void * _P(_Memory), __in size_t _P(_NewSize));;
_CRTIMP _CRTNOALIAS              void    __cdecl _aligned_free(__inout_opt void * _P(_Memory));
_CRTIMP _CRTNOALIAS _CRTRESTRICT __checkReturn __bcount_opt(_Size) void *  __cdecl _aligned_malloc(__in size_t _P(_Size), __in size_t _P(_Alignment));
_CRTIMP _CRTNOALIAS _CRTRESTRICT __checkReturn __bcount_opt(_Size) void *  __cdecl _aligned_offset_malloc(__in size_t _P(_Size), __in size_t _P(_Alignment), __in size_t _P(_Offset));
_CRTIMP _CRTNOALIAS _CRTRESTRICT __checkReturn __bcount_opt(_Size) void *  __cdecl _aligned_realloc(__in_opt void * _P(_Memory), __in size_t _P(_Size), __in size_t _P(_Alignment));
_CRTIMP _CRTNOALIAS _CRTRESTRICT __checkReturn __bcount_opt(_Size) void *  __cdecl _aligned_offset_realloc(__in_opt void * _P(_Memory), __in size_t _P(_Size), __in size_t _P(_Alignment), __in size_t _P(_Offset));
_CRTIMP _CRTNOALIAS              void    __cdecl _freea_s(__inout_opt void * _P(_Memory));

#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
#pragma pop_macro("calloc")
#pragma pop_macro("free")
#pragma pop_macro("malloc")
#pragma pop_macro("realloc")
#pragma pop_macro("_aligned_free")
#pragma pop_macro("_aligned_malloc")
#pragma pop_macro("_aligned_offset_malloc")
#pragma pop_macro("_aligned_realloc")
#pragma pop_macro("_aligned_offset_realloc")
#pragma pop_macro("_freea_s")
#endif

#define _resetstkoflw() (1)

#define _MAX_WAIT_MALLOC_CRT 60000

_CRTIMP unsigned long __cdecl _set_malloc_crt_max_wait(__in unsigned long _P(_NewValue));


#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
#pragma push_macro("_expand")
#pragma push_macro("_msize")
#undef _expand
#undef _msize
#endif

_CRTIMP __checkReturn __bcount_opt(_Size) void *  __cdecl _expand(__in void * _P(_Memory), __in size_t _P(_NewSize));
_CRTIMP __checkReturn size_t  __cdecl _msize(__in void * _P(_Memory));

#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
#pragma pop_macro("_expand")
#pragma pop_macro("_msize")
#endif

__checkReturn __bcount(_Size) void *          __cdecl _alloca(__in size_t _P(_Size));
_CRTIMP __checkReturn size_t  __cdecl _get_sbh_threshold(void);
_CRTIMP int     __cdecl _set_sbh_threshold(__in size_t _P(_NewValue));
_CRTIMP errno_t __cdecl _set_amblksiz(__in size_t _P(_Value));
_CRTIMP errno_t __cdecl _get_amblksiz(__out size_t * _P(_Value));
_CRTIMP __checkReturn int     __cdecl _heapadd(__in void * _P(_Memory), __in size_t _P(_Size));
_CRTIMP __checkReturn int     __cdecl _heapchk(void);
_CRTIMP __checkReturn int     __cdecl _heapmin(void);
_CRTIMP int     __cdecl _heapset(__in unsigned int _P(_Fill));
#if !defined(_XBOX) || defined(_DEBUG)
_CRTIMP int     __cdecl _heapwalk(__inout _HEAPINFO * _P(_EntryInfo));
#endif
_CRTIMP size_t  __cdecl _heapused(size_t * _P(_Used), size_t * _P(_Commit));

_CRTIMP intptr_t __cdecl _get_heap_handle(void);

#define _ALLOCA_S_THRESHOLD     1024
#define _ALLOCA_S_STACK_MARKER  0xCCCC
#define _ALLOCA_S_HEAP_MARKER   0xDDDD

#define _ALLOCA_S_MARKER_SIZE   16

_STATIC_ASSERT(sizeof(unsigned int) <= _ALLOCA_S_MARKER_SIZE);

__inline void *_MarkAllocaS(__out_opt __typefix(unsigned int*) void *_Ptr, unsigned int _Marker)
{
    if (_Ptr)
    {
        *((unsigned int*)_Ptr) = _Marker;
        _Ptr = (char*)_Ptr + _ALLOCA_S_MARKER_SIZE;
    }
    return _Ptr;
}

#if defined(_DEBUG)
#if !defined(_CRTDBG_MAP_ALLOC)
#undef _alloca_s
#define _alloca_s(size) \
        _MarkAllocaS(malloc((size) + _ALLOCA_S_MARKER_SIZE), _ALLOCA_S_HEAP_MARKER)
#endif
#else
#undef _alloca_s
#define _alloca_s(size) \
    ((((size) + _ALLOCA_S_MARKER_SIZE) <= _ALLOCA_S_THRESHOLD) ? \
        _MarkAllocaS(_alloca((size) + _ALLOCA_S_MARKER_SIZE), _ALLOCA_S_STACK_MARKER) : \
        _MarkAllocaS(malloc((size) + _ALLOCA_S_MARKER_SIZE), _ALLOCA_S_HEAP_MARKER))
#endif

#if     !__STDC__
/* Non-ANSI names for compatibility */
#define alloca  _alloca
#endif  /* __STDC__*/

#pragma intrinsic(_alloca)




#ifdef  __cplusplus
}
#endif

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif  /* _INC_MALLOC */
