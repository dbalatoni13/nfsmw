/***
*varargs.h - XENIX style macros for variable argument functions
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file defines XENIX style macros for accessing arguments of a
*       function which takes a variable number of arguments.
*       [System V]
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_VARARGS
#define _INC_VARARGS

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

#if     __STDC__
#error varargs.h incompatible with ANSI (use stdarg.h)
#endif

#if !defined(_W64)
#if !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && _MSC_VER >= 1300
#define _W64 __w64
#else
#define _W64
#endif
#endif

#ifndef _UINTPTR_T_DEFINED
#ifdef  _WIN64
typedef unsigned __int64    uintptr_t;
#else
typedef _W64 unsigned int   uintptr_t;
#endif
#define _UINTPTR_T_DEFINED
#endif

#ifndef _VA_LIST_DEFINED
typedef char *  va_list;
#define _VA_LIST_DEFINED
#endif

#ifndef va_arg


#define _VA_ALIGN(t)    8
#define _VA_IS_LEFT_JUSTIFIED(t) (sizeof(t) > _VA_ALIGN(t) || 0 != (sizeof(t) & (sizeof(t)-1)))

/* bytes that a type occupies in the argument list */
#define _INTSIZEOF(n)   ( (sizeof(n) + _VA_ALIGN(n) - 1) & ~(_VA_ALIGN(n) - 1) )
/* return 'ap' adjusted for type 't' in arglist */
#define _ALIGNIT(ap,t)  ( ((int)(ap) + _VA_ALIGN(t) - 1) & ~(_VA_ALIGN(t) - 1) )

#define va_dcl          va_list va_alist;

#define va_start(ap,v)  ( ap = ( _VA_IS_LEFT_JUSTIFIED(v) ? (va_list)_ADDRESSOF(v) + _INTSIZEOF(v) \
                                                          : (va_list)(&(v)+1) ))

#define va_arg(ap,t) (ap = (va_list) (_ALIGNIT(ap, t) + _INTSIZEOF(t)), \
                      _VA_IS_LEFT_JUSTIFIED(t) ? *(t*)((ap) - _INTSIZEOF(t)) \
                                               : ((t*)(ap))[-1] )

#define va_end(ap)      ( ap = (va_list)0 )


#endif


#ifdef  __cplusplus
}
#endif

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif  /* _INC_VARARGS */
