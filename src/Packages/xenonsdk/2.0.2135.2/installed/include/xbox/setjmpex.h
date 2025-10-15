/***
*setjmpex.h - definitions/declarations for extended setjmp/longjmp routines
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file causes _setjmpex to be called which will enable safe
*       setjmp/longjmp that work correctly with try/except/finally.
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_SETJMPEX
#define _INC_SETJMPEX

#if     !defined(_WIN32)
#error ERROR: Only Win32 target supported!
#endif


/*
 * Definitions specific to particular setjmp implementations.
 */


#ifdef setjmp
#undef setjmp
#endif
#define setjmp _setjmpex


#include <setjmp.h>

#endif  /* _INC_SETJMPEX */
