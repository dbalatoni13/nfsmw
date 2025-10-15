/***
*vadefs.h - defines helper macros for stdarg.h
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This is a helper file for stdarg.h
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_VADEFS
#define _INC_VADEFS

#if     !defined(_WIN32)
#error ERROR: Only Win32 target supported!
#endif


#ifdef  _MSC_VER
/*
 * Currently, all MS C compilers for Win32 platforms default to 8 byte
 * alignment.
 */
#undef _CRT_PACKING
#define _CRT_PACKING 8
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

#ifdef  __cplusplus
#define _ADDRESSOF(v)   ( &reinterpret_cast<const char &>(v) )
#else
#define _ADDRESSOF(v)   ( &(v) )
#endif

#define _SLOTSIZEOF(t)   (sizeof(t))
#define _APALIGN(t,ap)  (__alignof(t))


#define _VA_ALIGN(t)    8
#define _VA_IS_LEFT_JUSTIFIED(t) (sizeof(t) > _VA_ALIGN(t) || 0 != (sizeof(t) & (sizeof(t)-1)))

/* bytes that a type occupies in the argument list */
#define _INTSIZEOF(n)   ( (sizeof(n) + _VA_ALIGN(n) - 1) & ~(_VA_ALIGN(n) - 1) )
/* return 'ap' adjusted for type 't' in arglist */
#define _ALIGNIT(ap,t)  ( ((int)(ap) + _VA_ALIGN(t) - 1) & ~(_VA_ALIGN(t) - 1) )

#define va_dcl          va_list va_alist;

#if (_MSC_FULL_VER > 14000800)
/* XENON 7980:
 * va_start now an intrinsic so it can detect if copy constructed objects are passed by reference 
 */
extern void __cdecl __va_start(va_list *, ...);
#define _crt_va_start(ap, x) __va_start(&ap, &x)
#else
/* 
 * If not being able to call va_start from a non-varargs function is too onerous, just
 * re-define _crt_va_start to the original macro.
 */
#define _crt_va_start(ap,v)  ( ap = ( _VA_IS_LEFT_JUSTIFIED(v) ? (va_list)_ADDRESSOF(v) + _INTSIZEOF(v) \
                                                          : (va_list)(&(v)+1) ))
#endif
#define _crt_va_arg(ap,t) (ap = (va_list) (_ALIGNIT(ap, t) + _INTSIZEOF(t)), \
                      _VA_IS_LEFT_JUSTIFIED(t) ? *(t*)((ap) - _INTSIZEOF(t)) \
                                               : ((t*)(ap))[-1] )

#define _crt_va_end(ap)      ( ap = (va_list)0 )


#ifdef  __cplusplus
}
#endif

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif  /* _INC_VADEFS */
