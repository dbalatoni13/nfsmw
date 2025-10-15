/***
*crtdbg.h - Supports debugging features of the C runtime library.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Support CRT debugging features.
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#include <crtdefs.h>

#ifndef _INC_CRTDBG
#define _INC_CRTDBG

#ifdef  _MSC_VER
#pragma pack(push,_CRT_PACKING)
#endif  /* _MSC_VER */

/* Define NULL here since we depend on it and for back-compat
*/
#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#ifdef  __cplusplus
extern "C" {
#endif  /* __cplusplus */

 /****************************************************************************
 *
 * Debug Reporting
 *
 ***************************************************************************/

typedef void *_HFILE; /* file handle pointer */

#define _CRT_WARN           0
#define _CRT_ERROR          1
#define _CRT_ASSERT         2
#define _CRT_ERRCNT         3

#define _CRTDBG_MODE_FILE      0x1
#define _CRTDBG_MODE_DEBUG     0x2
#define _CRTDBG_MODE_WNDW      0x4
#define _CRTDBG_REPORT_MODE    -1

#define _CRTDBG_INVALID_HFILE ((_HFILE)-1)
#define _CRTDBG_HFILE_ERROR   ((_HFILE)-2)
#define _CRTDBG_REPORT_FILE   ((_HFILE)-6)

typedef int (__cdecl * _CRT_REPORT_HOOK)(int, char *, int *);
typedef int (__cdecl * _CRT_REPORT_HOOKW)(int, wchar_t *, int *);


#define _CRT_RPTHOOK_INSTALL  0
#define _CRT_RPTHOOK_REMOVE   1

 /****************************************************************************
 *
 * Heap
 *
 ***************************************************************************/

 /****************************************************************************
 *
 * Client-defined allocation hook
 *
 ***************************************************************************/

#define _HOOK_ALLOC     1
#define _HOOK_REALLOC   2
#define _HOOK_FREE      3

typedef int (__cdecl * _CRT_ALLOC_HOOK)(int, void *, size_t, int, long, const unsigned char *, int);

 /****************************************************************************
 *
 * Memory management
 *
 ***************************************************************************/

/*
 * Bit values for _crtDbgFlag flag:
 *
 * These bitflags control debug heap behavior.
 */

#define _CRTDBG_ALLOC_MEM_DF        0x01  /* Turn on debug allocation */
#define _CRTDBG_DELAY_FREE_MEM_DF   0x02  /* Don't actually free memory */
#define _CRTDBG_CHECK_ALWAYS_DF     0x04  /* Check heap every alloc/dealloc */
#define _CRTDBG_RESERVED_DF         0x08  /* Reserved - do not use */
#define _CRTDBG_CHECK_CRT_DF        0x10  /* Leak check/diff CRT blocks */
#define _CRTDBG_LEAK_CHECK_DF       0x20  /* Leak check at program exit */

/*
 * Some bit values for _crtDbgFlag which correspond to frequencies for checking
 * the the heap.
 */
#define _CRTDBG_CHECK_EVERY_16_DF   0x00100000  /* check heap every 16 heap ops */
#define _CRTDBG_CHECK_EVERY_128_DF  0x00800000  /* check heap every 128 heap ops */
#define _CRTDBG_CHECK_EVERY_1024_DF 0x04000000  /* check heap every 1024 heap ops */
/*
We do not check the heap by default at this point because the cost was too high
for some applications. You can still turn this feature on manually.
*/
#define _CRTDBG_CHECK_DEFAULT_DF    0           

#define _CRTDBG_REPORT_FLAG         -1    /* Query bitflag status */

#define _BLOCK_TYPE(block)          (block & 0xFFFF)
#define _BLOCK_SUBTYPE(block)       (block >> 16 & 0xFFFF)


 /****************************************************************************
 *
 * Memory state
 *
 ***************************************************************************/

/* Memory block identification */
#define _FREE_BLOCK      0
#define _NORMAL_BLOCK    1
#define _CRT_BLOCK       2
#define _IGNORE_BLOCK    3
#define _CLIENT_BLOCK    4
#define _MAX_BLOCKS      5

typedef void (__cdecl * _CRT_DUMP_CLIENT)(void *, size_t);

struct _CrtMemBlockHeader;
typedef struct _CrtMemState
{
        struct _CrtMemBlockHeader * pBlockHeader;
        size_t lCounts[_MAX_BLOCKS];
        size_t lSizes[_MAX_BLOCKS];
        size_t lHighWaterCount;
        size_t lTotalCount;
} _CrtMemState;


 /****************************************************************************
 *
 * Declarations, prototype and function-like macros
 *
 ***************************************************************************/

/* _STATIC_ASSERT is for enforcing boolean/integral conditions at compile time.
   Since it is purely a compile-time mechanism that generates no code, the check
   is left in even if _DEBUG is not defined. */

#ifndef _STATIC_ASSERT
#define _STATIC_ASSERT(expr) typedef char __static_assert_t[ (expr) ]
#endif

#ifndef _DEBUG

 /****************************************************************************
 *
 * Debug OFF
 * Debug OFF
 * Debug OFF
 *
 ***************************************************************************/

/*  We allow our basic _ASSERT macros to be overridden by pre-existing definitions. 
    This is not the ideal mechanism, but is helpful in some scenarios and helps avoid
    multiple definition problems */

#ifndef _ASSERT
#define _ASSERT(expr) ((void)0)
#endif 

#ifndef _ASSERTE
#define _ASSERTE(expr) ((void)0)
#endif

#ifndef _ASSERT_EXPR
#define _ASSERT_EXPR(expr, expr_str) ((void)0)
#endif

#define _RPT0(rptno, msg)
#define _RPTW0(rptno, msg)

#define _RPT1(rptno, msg, arg1)
#define _RPTW1(rptno, msg, arg1)

#define _RPT2(rptno, msg, arg1, arg2)
#define _RPTW2(rptno, msg, arg1, arg2)

#define _RPT3(rptno, msg, arg1, arg2, arg3)
#define _RPTW3(rptno, msg, arg1, arg2, arg3)

#define _RPT4(rptno, msg, arg1, arg2, arg3, arg4)
#define _RPTW4(rptno, msg, arg1, arg2, arg3, arg4)


#define _RPTF0(rptno, msg)
#define _RPTFW0(rptno, msg)

#define _RPTF1(rptno, msg, arg1)
#define _RPTFW1(rptno, msg, arg1)

#define _RPTF2(rptno, msg, arg1, arg2)
#define _RPTFW2(rptno, msg, arg1, arg2)

#define _RPTF3(rptno, msg, arg1, arg2, arg3)
#define _RPTFW3(rptno, msg, arg1, arg2, arg3)

#define _RPTF4(rptno, msg, arg1, arg2, arg3, arg4)
#define _RPTFW4(rptno, msg, arg1, arg2, arg3, arg4)

#define _malloc_dbg(s, t, f, l)         malloc(s)
#define _calloc_dbg(c, s, t, f, l)      calloc(c, s)
#define _realloc_dbg(p, s, t, f, l)     realloc(p, s)
#define _expand_dbg(p, s, t, f, l)      _expand(p, s)
#define _free_dbg(p, t)                 free(p)
#define _msize_dbg(p, t)                _msize(p)

#define _aligned_malloc_dbg(s, a, f, l)     _aligned_malloc(s, a)
#define _aligned_realloc_dbg(p, s, a, f, l) _aligned_realloc(p, s, a)
#define _aligned_free_dbg(p)                _aligned_free(p)
#define _aligned_offset_malloc_dbg(s, a, o, f, l)       _aligned_offset_malloc(s, a, o)
#define _aligned_offset_realloc_dbg(p, s, a, o, f, l)   _aligned_offset_realloc(p, s, a, o)

#define _alloca_s_dbg(s, t, f, l)       _alloca_s(s)
#define _freea_s_dbg(p, t)              _freea_s(p)

#define _strdup_dbg(s, t, f, l)         _strdup(s)
#define _wcsdup_dbg(s, t, f, l)         _wcsdup(s)
#define _mbsdup_dbg(s, t, f, l)         _mbsdup(s)
#define _tempnam_dbg(s1, s2, t, f, l)   _tempnam(s1, s2)
#define _wtempnam_dbg(s1, s2, t, f, l)  _wtempnam(s1, s2)
#define _fullpath_dbg(s1, s2, le, t, f, l)  _fullpath(s1, s2, le)
#define _wfullpath_dbg(s1, s2, le, t, f, l) _wfullpath(s1, s2, le)
#define _getcwd_dbg(s, le, t, f, l)     _getcwd(s, le)
#define _wgetcwd_dbg(s, le, t, f, l)    _wgetcwd(s, le)
#define _getdcwd_dbg(d, s, le, t, f, l)     _getdcwd(d, s, le)
#define _wgetdcwd_dbg(d, s, le, t, f, l)    _wgetdcwd(d, s, le)
#define _getdcwd_lk_dbg(d, s, le, t, f, l)  _getdcwd_nolock(d, s, le)
#define _wgetdcwd_lk_dbg(d, s, le, t, f, l) _wgetdcwd_nolock(d, s, le)
#define _dupenv_s_dbg(ps1, size, s2, t, f, l)  _dupenv_s(ps1, size, s2)
#define _wdupenv_s_dbg(ps1, size, s2, t, f, l) _wdupenv_s(ps1, size, s2)

#define _CrtSetReportHook(f)                ((_CRT_REPORT_HOOK)0)
#define _CrtGetReportHook()                 ((_CRT_REPORT_HOOK)0)
#define _CrtSetReportHook2(t, f)            ((int)0)
#define _CrtSetReportHookW2(t, f)           ((int)0)
#define _CrtSetReportMode(t, f)             ((int)0)
#define _CrtSetReportFile(t, f)             ((_HFILE)0)

#define _CrtDbgBreak()                      ((void)0)

#define _CrtSetBreakAlloc(a)                ((long)0)

#define _CrtSetAllocHook(f)                 ((_CRT_ALLOC_HOOK)0)
#define _CrtGetAllocHook()                  ((_CRT_ALLOC_HOOK)0)

#define _CrtCheckMemory()                   ((int)1)
#define _CrtSetDbgFlag(f)                   ((int)0)
#define _CrtDoForAllClientObjects(f, c)     ((void)0)
#define _CrtIsValidPointer(p, n, r)         ((int)1)
#define _CrtIsValidHeapPointer(p)           ((int)1)
#define _CrtIsMemoryBlock(p, t, r, f, l)    ((int)1)
#define _CrtReportBlockType(p)              ((int)-1)

#define _CrtSetDumpClient(f)                ((_CRT_DUMP_CLIENT)0)
#define _CrtGetDumpClient()                 ((_CRT_DUMP_CLIENT)0)

#define _CrtMemCheckpoint(s)                ((void)0)
#define _CrtMemDifference(s1, s2, s3)       ((int)0)
#define _CrtMemDumpAllObjectsSince(s)       ((void)0)
#define _CrtMemDumpStatistics(s)            ((void)0)
#define _CrtDumpMemoryLeaks()               ((int)0)


#else   /* _DEBUG */


 /****************************************************************************
 *
 * Debug ON
 * Debug ON
 * Debug ON
 *
 ***************************************************************************/


/* Define _MRTIMP */

#ifndef _MRTIMP
#define _MRTIMP __declspec(dllimport)
#endif  /* _MRTIMP */

/* Define _CRTIMP */

#ifndef _CRTIMP
#if defined(_DLL) && _XBOX_VER < 200
#define _CRTIMP __declspec(dllimport)
#else   /* ndef _DLL */
#define _CRTIMP
#endif  /* _DLL */
#endif  /* _CRTIMP */

 /****************************************************************************
 *
 * Debug Reporting
 *
 ***************************************************************************/

_CRTIMP extern long _crtAssertBusy;

_CRTIMP _CRT_REPORT_HOOK __cdecl _CrtSetReportHook
(
    __in_opt _CRT_REPORT_HOOK _P(_PFnNewHook)
);

_CRTIMP _CRT_REPORT_HOOK __cdecl _CrtGetReportHook
(
    void
);

_CRTIMP int __cdecl _CrtSetReportHook2(
        __in int _P(_Mode),
        __in_opt _CRT_REPORT_HOOK _P(_PFnNewHook)
        );

_CRTIMP int __cdecl _CrtSetReportHookW2(
        __in int _P(_Mode),
        __in_opt _CRT_REPORT_HOOKW _P(_PFnNewHook)
        );

_CRTIMP int __cdecl _CrtSetReportMode(
        __in int _P(_ReportType),
        __in_opt int _P(_ReportMode) 
        );

_CRTIMP _HFILE __cdecl _CrtSetReportFile(
        __in int _P(_ReportType),
        __in_opt _HFILE _P(_ReportFile) 
        );

_CRTIMP int __cdecl _CrtDbgReport(
        __in int _P(_ReportType),
        __in_opt const char * _P(_Filename),
        __in int _P(_Linenumber),
        __in_opt const char * _P(_ModuleName),
        __in_opt const char * _P(_Format),
        ...);

_CRTIMP int __cdecl _CrtDbgReportW(
        __in int _P(_ReportType),
        __in_opt const wchar_t * _P(_Filename),
        __in int _P(_LineNumber),
        __in_opt const wchar_t * _P(_ModuleName),
        __in_opt const wchar_t * _P(_Format),
        ...);


/* Asserts */

#if     _MSC_VER >= 1300 || !defined(_M_IX86) || defined(_CRT_PORTABLE)
#define _ASSERT_EXPR(expr, msg) \
        (void) ((expr) || \
                (1 != _CrtDbgReportW(_CRT_ASSERT, _CRT_WIDE(__FILE__), __LINE__, NULL, msg)) || \
                (_CrtDbgBreak(), 0))
#else
#define _ASSERT_EXPR(expr, msg) \
        do { if (!(expr) && \
                (1 == _CrtDbgReportW(_CRT_ASSERT, _CRT_WIDE(__FILE__), __LINE__, NULL, msg))) \
             _CrtDbgBreak(); } while (0)
#endif

#ifndef _ASSERT
#define _ASSERT(expr)   _ASSERT_EXPR((expr), NULL)
#endif

#ifndef _ASSERTE
#define _ASSERTE(expr)  _ASSERT_EXPR((expr), _CRT_WIDE(#expr))
#endif

/* Reports with no file/line info */

#if     _MSC_VER >= 1300 || !defined(_M_IX86) || defined(_CRT_PORTABLE)
#define _RPT_BASE(args) \
        (void) ((1 != _CrtDbgReport args) || \
                (_CrtDbgBreak(), 0))

#define _RPT_BASE_W(args) \
        (void) ((1 != _CrtDbgReportW args) || \
                (_CrtDbgBreak(), 0))
#else
#define _RPT_BASE(args) \
        do { if ((1 == _CrtDbgReport args)) \
                _CrtDbgBreak(); } while (0)

#define _RPT_BASE_W(args) \
        do { if ((1 == _CrtDbgReportW args)) \
                _CrtDbgBreak(); } while (0)
#endif

#define _RPT0(rptno, msg) \
        _RPT_BASE((rptno, NULL, 0, NULL, "%s", msg))

#define _RPTW0(rptno, msg) \
        _RPT_BASE_W((rptno, NULL, 0, NULL, L"%s", msg))

#define _RPT1(rptno, msg, arg1) \
        _RPT_BASE((rptno, NULL, 0, NULL, msg, arg1))

#define _RPTW1(rptno, msg, arg1) \
        _RPT_BASE_W((rptno, NULL, 0, NULL, msg, arg1))

#define _RPT2(rptno, msg, arg1, arg2) \
        _RPT_BASE((rptno, NULL, 0, NULL, msg, arg1, arg2))

#define _RPTW2(rptno, msg, arg1, arg2) \
        _RPT_BASE_W((rptno, NULL, 0, NULL, msg, arg1, arg2))

#define _RPT3(rptno, msg, arg1, arg2, arg3) \
        _RPT_BASE((rptno, NULL, 0, NULL, msg, arg1, arg2, arg3))

#define _RPTW3(rptno, msg, arg1, arg2, arg3) \
        _RPT_BASE_W((rptno, NULL, 0, NULL, msg, arg1, arg2, arg3))

#define _RPT4(rptno, msg, arg1, arg2, arg3, arg4) \
        _RPT_BASE((rptno, NULL, 0, NULL, msg, arg1, arg2, arg3, arg4))

#define _RPTW4(rptno, msg, arg1, arg2, arg3, arg4) \
        _RPT_BASE_W((rptno, NULL, 0, NULL, msg, arg1, arg2, arg3, arg4))

/* Reports with file/line info */

#define _RPTF0(rptno, msg) \
        _RPT_BASE((rptno, __FILE__, __LINE__, NULL, "%s", msg))

#define _RPTFW0(rptno, msg) \
        _RPT_BASE_W((rptno, _CRT_WIDE(__FILE__), __LINE__, NULL, L"%s", msg))

#define _RPTF1(rptno, msg, arg1) \
        _RPT_BASE((rptno, __FILE__, __LINE__, NULL, msg, arg1))

#define _RPTFW1(rptno, msg, arg1) \
        _RPT_BASE_W((rptno, _CRT_WIDE(__FILE__), __LINE__, NULL, msg, arg1))

#define _RPTF2(rptno, msg, arg1, arg2) \
        _RPT_BASE((rptno, __FILE__, __LINE__, NULL, msg, arg1, arg2))

#define _RPTFW2(rptno, msg, arg1, arg2) \
        _RPT_BASE_W((rptno, _CRT_WIDE(__FILE__), __LINE__, NULL, msg, arg1, arg2))

#define _RPTF3(rptno, msg, arg1, arg2, arg3) \
        _RPT_BASE((rptno, __FILE__, __LINE__, NULL, msg, arg1, arg2, arg3))

#define _RPTFW3(rptno, msg, arg1, arg2, arg3) \
        _RPT_BASE_W((rptno, _CRT_WIDE(__FILE__), __LINE__, NULL, msg, arg1, arg2, arg3))

#define _RPTF4(rptno, msg, arg1, arg2, arg3, arg4) \
        _RPT_BASE((rptno, __FILE__, __LINE__, NULL, msg, arg1, arg2, arg3, arg4))

#define _RPTFW4(rptno, msg, arg1, arg2, arg3, arg4) \
        _RPT_BASE_W((rptno, _CRT_WIDE(__FILE__), __LINE__, NULL, msg, arg1, arg2, arg3, arg4))

#if     _MSC_VER >= 1300 && !defined(_CRT_PORTABLE)
#define _CrtDbgBreak() __debugbreak()
#else
_CRTIMP void __cdecl _CrtDbgBreak(
        void
        );
#endif

 /****************************************************************************
 *
 * Heap routines
 *
 ***************************************************************************/

#ifdef  _CRTDBG_MAP_ALLOC

#define   malloc(s)         _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   calloc(c, s)      _calloc_dbg(c, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   realloc(p, s)     _realloc_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _expand(p, s)     _expand_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   free(p)           _free_dbg(p, _NORMAL_BLOCK)
#define   _msize(p)         _msize_dbg(p, _NORMAL_BLOCK)
#define   _aligned_malloc(s, a)                 _aligned_malloc_dbg(s, a, __FILE__, __LINE__)
#define   _aligned_realloc(p, s, a)             _aligned_realloc_dbg(p, s, a, __FILE__, __LINE__)
#define   _aligned_offset_malloc(s, a, o)       _aligned_offset_malloc_dbg(s, a, o, __FILE__, __LINE__)
#define   _aligned_offset_realloc(p, s, a, o)   _aligned_offset_realloc_dbg(p, s, a, o, __FILE__, __LINE__)
#define   _aligned_free(p)  _aligned_free_dbg(p)

#define   _alloca_s(s)       _alloca_s_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _freea_s(p)        _freea_s_dbg(p, _NORMAL_BLOCK)

#define   _strdup(s)         _strdup_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _wcsdup(s)         _wcsdup_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _mbsdup(s)         _strdup_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _tempnam(s1, s2)   _tempnam_dbg(s1, s2, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _wtempnam(s1, s2)  _wtempnam_dbg(s1, s2, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _fullpath(s1, s2, le)     _fullpath_dbg(s1, s2, le, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _wfullpath(s1, s2, le)    _wfullpath_dbg(s1, s2, le, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _getcwd(s, le)      _getcwd_dbg(s, le, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _wgetcwd(s, le)     _wgetcwd_dbg(s, le, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _getdcwd(d, s, le)  _getdcwd_dbg(d, s, le, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _wgetdcwd(d, s, le) _wgetdcwd_dbg(d, s, le, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _getdcwd_nolock(d, s, le)     _getdcwd_lk_dbg(d, s, le, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _wgetdcwd_nolock(d, s, le)    _wgetdcwd_lk_dbg(d, s, le, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _dupenv_s(ps1, size, s2)      _dupenv_s_dbg(ps1, size, s2, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _wdupenv_s(ps1, size, s2)     _wdupenv_s_dbg(ps1, size, s2, _NORMAL_BLOCK, __FILE__, __LINE__)

#if     !__STDC__
#define   strdup(s)          _strdup_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   wcsdup(s)          _wcsdup_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   tempnam(s1, s2)    _tempnam_dbg(s1, s2, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   getcwd(s, le)      _getcwd_dbg(s, le, _NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#endif  /* _CRTDBG_MAP_ALLOC */

_CRTIMP extern long _crtBreakAlloc;      /* Break on this allocation */

_CRTIMP long __cdecl _CrtSetBreakAlloc(
        __in long _P(_BreakAlloc) 
        );

/*
 * Prototypes for malloc, free, realloc, etc are in malloc.h
 */

_CRTIMP __checkReturn __bcount_opt(_Size) void * __cdecl _malloc_dbg(
        __in size_t _P(_Size),
        __in int _P(_BlockType),
        __in_opt const char * _P(_Filename),
        __in int _P(_LineNumber)
        );

_CRTIMP __checkReturn __bcount_opt(_NumOfElements*_SizeOfElements) void * __cdecl _calloc_dbg(
        __in size_t _P(_NumOfElements),
        __in size_t _P(_SizeOfElements),
        __in int _P(_BlockType),
        __in_opt const char * _P(_Filename),
        __in int _P(_LineNumber)
        );

_CRTIMP __checkReturn __bcount_opt(_NewSize) void * __cdecl _realloc_dbg(
        __inout_opt void * _P(_Memory),
        __in size_t _P(_NewSize),
        __in int _P(_BlockType),
        __in_opt const char * _P(_Filename),
        __in int _P(_LineNumber)
        );

_CRTIMP __checkReturn __bcount_opt(_NewSize) void * __cdecl _expand_dbg(
        __inout_opt void * _P(_Memory),
        __in size_t _P(_NewSize),
        __in int _P(_BlockType),
        __in_opt const char * _P(_Filename),
        __in int _P(_LineNumber)
        );

_CRTIMP void __cdecl _free_dbg(
        __inout_opt void * _P(_Memory),
        __in int _P(_BlockType)
        );

_CRTIMP size_t __cdecl _msize_dbg (
        __in void * _P(_Memory),
        __in int _P(_BlockType)
        );

_CRTIMP __checkReturn __bcount_opt(_Size) void * __cdecl _aligned_malloc_dbg(
        __in size_t _P(_Size),
        __in size_t _P(_Alignment),
        __in_opt const char * _P(_Filename),
        __in int _P(_LineNumber)
        );

_CRTIMP __checkReturn __bcount_opt(_Size) void * __cdecl _aligned_realloc_dbg(
        __inout_opt void * _P(_Memory),
        __in size_t _P(_Size),
        __in size_t _P(_Alignment),
        __in_opt const char * _P(_Filename),
        __in int _P(_LineNumber)
        );

_CRTIMP __checkReturn __bcount_opt(_Size) void * __cdecl _aligned_offset_malloc_dbg(
        __in size_t _P(_Size),
        __in size_t _P(_Alignment),
        __in size_t _P(_Offset),
        __in_opt const char * _P(_Filename),
        __in int _P(_LineNumber)
        );

_CRTIMP __checkReturn __bcount_opt(_Size) void * __cdecl _aligned_offset_realloc_dbg(
        __inout_opt void * _P(_Memory),
        __in size_t _P(_Size),
        __in size_t _P(_Alignment),
        __in size_t _P(_Offset),
        __in_opt const char * _P(_Filename),
        __in int _P(_LineNumber)
        );

_CRTIMP void __cdecl _aligned_free_dbg(
        __inout_opt void * _P(_Memory)
        );

_CRTIMP __checkReturn __out_opt char * __cdecl _strdup_dbg(
        __in_opt const char * _P(_Str),
        __in int _P(_BlockType),
        __in_opt const char * _P(_Filename),
        __in int _P(_LineNumber)
        );

_CRTIMP __checkReturn __out_opt wchar_t * __cdecl _wcsdup_dbg(
        __in_opt const wchar_t * _P(_Str),
        __in int _P(_BlockType),
        __in_opt const char * _P(_Filename),
        __in int _P(_LineNumber)
        );

_CRTIMP __checkReturn __out_opt char * __cdecl _tempnam_dbg(
        __in_opt const char * _P(_DirName),
        __in_opt const char * _P(_FilePrefix),
        __in int _P(_BlockType),
        __in_opt const char * _P(_Filename),
        __in int _P(_LineNumber)
        );

_CRTIMP __checkReturn __out_opt wchar_t * __cdecl _wtempnam_dbg(
        __in_opt const wchar_t * _P(_DirName),
        __in_opt const wchar_t * _P(_FilePrefix),
        __in int _P(_BlockType),
        __in_opt const char * _P(_Filename),
        __in int _P(_LineNumber)
        );

_CRTIMP __checkReturn __out_opt char * __cdecl _fullpath_dbg(
        __out_ecount_opt(_SizeInBytes) char * _P(_FullPath), 
        __in const char * _P(_Path), 
        __in size_t _P(_SizeInBytes),
        __in int _P(_BlockType),
        __in_opt const char * _P(_Filename),
        __in int _P(_LineNumber)
        );

_CRTIMP __checkReturn __out_opt wchar_t * __cdecl _wfullpath_dbg(
        __out_ecount_opt(_SizeInWords) wchar_t * _P(_FullPath), 
        __in const wchar_t * _P(_Path), 
        __in size_t _P(_SizeInWords),
        __in int _P(_BlockType),
        __in_opt const char * _P(_Filename),
        __in int _P(_LineNumber)
        );

_CRTIMP __checkReturn __out_opt char * __cdecl _getcwd_dbg(
        __out_ecount_opt(_SizeInBytes) char * _P(_DstBuf),
        __in int _P(_SizeInBytes),
        __in int _P(_BlockType),
        __in_opt const char * _P(_Filename),
        __in int _P(_LineNumber)
        );

_CRTIMP __checkReturn __out_opt wchar_t * __cdecl _wgetcwd_dbg(
        __out_ecount_opt(_SizeInWords) wchar_t * _P(_DstBuf),
        __in int _P(_SizeInWords),
        __in int _P(_BlockType),
        __in_opt const char * _P(_Filename),
        __in int _P(_LineNumber)
        );

_CRTIMP __checkReturn __out_opt char * __cdecl _getdcwd_dbg(
        __in int _P(_Drive),
        __out_ecount_opt(_SizeInBytes) char * _P(_DstBuf),
        __in int _P(_SizeInBytes),
        __in int _P(_BlockType),
        __in_opt const char * _P(_Filename),
        __in int _P(_LineNumber)
        );

_CRTIMP __checkReturn __out_opt wchar_t * __cdecl _wgetdcwd_dbg(
        __in int _P(_Drive),
        __out_ecount_opt(_SizeInWords) wchar_t * _P(_DstBuf),
        __in int _P(_SizeInWords),
        __in int _P(_BlockType),
        __in_opt const char * _P(_Filename),
        __in int _P(_LineNumber)
        );

__checkReturn __out_opt char * __cdecl _getdcwd_lk_dbg(
        __in int _P(_Drive),
        __out_ecount_opt(_SizeInBytes) char * _P(_DstBuf),
        __in int _P(_SizeInBytes),
        __in int _P(_BlockType),
        __in_opt const char * _P(_Filename),
        __in int _P(_LineNumber)
        );

__checkReturn __out_opt wchar_t * __cdecl _wgetdcwd_lk_dbg(
        __in int _P(_Drive),
        __out_ecount_opt(_SizeInWords) wchar_t * _P(_DstBuf),
        __in int _P(_SizeInWords),
        __in int _P(_BlockType),
        __in_opt const char * _P(_Filename),
        __in int _P(_LineNumber)
        );

_CRTIMP __checkReturn_wat errno_t __cdecl _dupenv_s_dbg(
        __deref_out_ecount_opt(*_PBufferSizeInBytes) char ** _P(_PBuffer),
        __out_opt size_t * _P(_PBufferSizeInBytes),
        __in const char * _P(_VarName),
        __in int _P(_BlockType),
        __in_opt const char * _P(_Filename),
        __in int _P(_LineNumber)
        );

_CRTIMP __checkReturn_wat errno_t __cdecl _wdupenv_s_dbg(
        __deref_out_ecount_opt (*_PBufferSizeInWords) wchar_t ** _P(_PBuffer),
        __out_opt size_t * _P(_PBufferSizeInWords),
        __in const wchar_t * _P(_VarName),
        __in int _P(_BlockType),
        __in_opt const char * _P(_Filename),
        __in int _P(_LineNumber)
        );

#define _alloca_s_dbg(s, t, f, l)   _malloc_dbg(s, t, f, l)
#define _freea_s_dbg(p, t)          _free_dbg(p, t)

 /****************************************************************************
 *
 * Client-defined allocation hook
 *
 ***************************************************************************/

_CRTIMP _CRT_ALLOC_HOOK __cdecl _CrtSetAllocHook
(
    __in_opt _CRT_ALLOC_HOOK _P(_PfnNewHook)
);

_CRTIMP _CRT_ALLOC_HOOK __cdecl _CrtGetAllocHook
(
    void
);



 /****************************************************************************
 *
 * Memory management
 *
 ***************************************************************************/

/*
 * Bitfield flag that controls CRT heap behavior
 * Default setting is _CRTDBG_ALLOC_MEM_DF
 */

_CRTIMP extern int _crtDbgFlag;

_CRTIMP int __cdecl _CrtCheckMemory(
        void
        );

_CRTIMP int __cdecl _CrtSetDbgFlag(
        __in int _P(_NewFlag)
        );

_CRTIMP void __cdecl _CrtDoForAllClientObjects(
        __in void (__cdecl *_P(_PFn))(void *, void *),
        void * _P(_Context)
        );


_CRTIMP __checkReturn int __cdecl _CrtIsValidPointer(
        __in_opt const void * _P(_Ptr),
        __in unsigned int _P(_Bytes),
        __in int _P(_ReadWrite)
        );

_CRTIMP __checkReturn int __cdecl _CrtIsValidHeapPointer(
        __in_opt const void * _P(_HeapPtr)
        );

_CRTIMP int __cdecl _CrtIsMemoryBlock(
        __in_opt const void * _P(_Memory),
        __in unsigned int _P(_Bytes),
        __out_opt long * _P(_RequestNumber),
        __out_opt char ** _P(_Filename),
        __out_opt int * _P(_LineNumber)
        );

_CRTIMP __checkReturn int __cdecl _CrtReportBlockType(
        __in_opt const void * _P(_Memory)
        );


 /****************************************************************************
 *
 * Memory state
 *
 ***************************************************************************/

_CRTIMP _CRT_DUMP_CLIENT __cdecl _CrtSetDumpClient
(
    __in_opt _CRT_DUMP_CLIENT _P(_PFnNewDump)
);

_CRTIMP _CRT_DUMP_CLIENT __cdecl _CrtGetDumpClient
(
    void
);


_CRTIMP void __cdecl _CrtMemCheckpoint(
        __out _CrtMemState * _P(_State)
        );

_CRTIMP int __cdecl _CrtMemDifference(
        __out _CrtMemState * _P(_State),
        __in const _CrtMemState * _P(_OldState),
        __in const _CrtMemState * _P(_NewState)
        );

_CRTIMP void __cdecl _CrtMemDumpAllObjectsSince(
        __in_opt const _CrtMemState * _P(_State)
        );

_CRTIMP void __cdecl _CrtMemDumpStatistics(
        __in const _CrtMemState * _P(_State)
        );

_CRTIMP int __cdecl _CrtDumpMemoryLeaks(
        void
        );

#endif  /* _DEBUG */

#ifdef  __cplusplus
}

#ifndef _MFC_OVERRIDES_NEW

extern "C++" {

#ifndef _DEBUG

 /****************************************************************************
 *
 * Debug OFF
 * Debug OFF
 * Debug OFF
 *
 ***************************************************************************/

 
void * __CRTDECL operator new[](size_t);

inline void * __CRTDECL operator new(size_t s, int, const char *, int)
        { return ::operator new(s); }

inline void* __CRTDECL operator new[](size_t s, int, const char *, int)
        { return ::operator new[](s); }

#if     _MSC_VER >= 1200


void __CRTDECL operator delete[](void *);

inline void __CRTDECL operator delete(void * _P, int, const char *, int)
        { ::operator delete(_P); }
inline void __CRTDECL operator delete[](void * _P, int, const char *, int)
        { ::operator delete[](_P); }
#endif
#else /* _DEBUG */

 /****************************************************************************
 *
 * Debug ON
 * Debug ON
 * Debug ON
 *
 ***************************************************************************/
 
void * __CRTDECL operator new[](size_t);

void * __CRTDECL operator new(
        size_t,
        int,
        const char *,
        int
        );

void * __CRTDECL operator new[](
        size_t,
        int,
        const char *,
        int
        );

#if     _MSC_VER >= 1200
void __CRTDECL operator delete[](void *);

inline void __CRTDECL operator delete(void * _P, int, const char *, int)
        { ::operator delete(_P); }

inline void __CRTDECL operator delete[](void * _P, int, const char *, int)
        { ::operator delete[](_P); }
#endif

#ifdef _CRTDBG_MAP_ALLOC

inline void * __CRTDECL operator new(size_t s)
        { return ::operator new(s, _NORMAL_BLOCK, __FILE__, __LINE__); }

inline void* __CRTDECL operator new[](size_t s)
        { return ::operator new[](s, _NORMAL_BLOCK, __FILE__, __LINE__); }

#endif  /* _CRTDBG_MAP_ALLOC */

#endif  /* _DEBUG */

}

#endif  /* _MFC_OVERRIDES_NEW */

#endif  /* __cplusplus */

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif  /* _INC_CRTDBG */
