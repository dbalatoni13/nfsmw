/***
*stdarg.h - defines ANSI-style macros for variable argument functions
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file defines ANSI-style macros for accessing arguments
*       of functions which take a variable number of arguments.
*       [ANSI]
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_STDARG
#define _INC_STDARG

#if     !defined(_WIN32)
#error ERROR: Only Win32 target supported!
#endif


#include <vadefs.h>

#ifdef  _MSC_VER
/*
 * Currently, all MS C compilers for Win32 platforms default to 8 byte
 * alignment.
 */
#pragma pack(push,_CRT_PACKING)
#endif  /* _MSC_VER */

#define va_start _crt_va_start
#define va_arg _crt_va_arg
#define va_end _crt_va_end

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif  /* _INC_STDARG */
