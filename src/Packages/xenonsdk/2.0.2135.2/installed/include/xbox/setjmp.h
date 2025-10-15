/***
*setjmp.h - definitions/declarations for setjmp/longjmp routines
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file defines the machine-dependent buffer used by
*       setjmp/longjmp to save and restore the program state, and
*       declarations for those routines.
*       [ANSI/System V]
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_SETJMP
#define _INC_SETJMP

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

/*
 * Definitions specific to particular setjmp implementations.
 */

/*
 * The Microsoft VC++ compiler uses an intrinsic _setjmp.
 */

#if     _MSC_VER > 850
#ifndef _INC_SETJMPEX
#undef _setjmp
#define setjmp  _setjmp
#endif
#endif

/*
 * Minimum length is 528 bytes
 * Since this is allocated as an array of "SETJMP_VECTOR4", the
 * number of entries required is 20 (16-byte aligned).
 */

/* Align to 16 bytes for VMX registers */
typedef struct __declspec(align(16)) SETJMP_VECTOR4 {
    float floats[4];
} SETJMP_VECTOR4;
#define _JBLEN  20
#define _JBTYPE SETJMP_VECTOR4

/*
 * Define jump buffer layout for PowerPC setjmp/longjmp.
 */

typedef struct __JUMP_BUFFER {
    double Fpr14;
    double Fpr15;
    double Fpr16;
    double Fpr17;
    double Fpr18;
    double Fpr19;
    double Fpr20;
    double Fpr21;
    double Fpr22;
    double Fpr23;
    double Fpr24;
    double Fpr25;
    double Fpr26;
    double Fpr27;
    double Fpr28;
    double Fpr29;
    double Fpr30;
    double Fpr31;
    unsigned __int64 Gpr1;
    unsigned __int64 Gpr13;
    unsigned __int64 Gpr14;
    unsigned __int64 Gpr15;
    unsigned __int64 Gpr16;
    unsigned __int64 Gpr17;
    unsigned __int64 Gpr18;
    unsigned __int64 Gpr19;
    unsigned __int64 Gpr20;
    unsigned __int64 Gpr21;
    unsigned __int64 Gpr22;
    unsigned __int64 Gpr23;
    unsigned __int64 Gpr24;
    unsigned __int64 Gpr25;
    unsigned __int64 Gpr26;
    unsigned __int64 Gpr27;
    unsigned __int64 Gpr28;
    unsigned __int64 Gpr29;
    unsigned __int64 Gpr30;
    unsigned __int64 Gpr31;
    unsigned long Cr;
    unsigned long Iar;
    unsigned long Type;
    unsigned long Reserved;
} _JUMP_BUFFER;



/* Define the buffer type for holding the state information */

#ifndef _JMP_BUF_DEFINED
typedef _JBTYPE jmp_buf[_JBLEN];
#define _JMP_BUF_DEFINED
#endif


/* Function prototypes */

int __cdecl setjmp(__out jmp_buf _P(_Buf));

#ifdef  __cplusplus
}
#endif

#ifdef  __cplusplus
extern "C"
{
_CRTIMP __declspec(noreturn) void __cdecl longjmp(__in jmp_buf _P(_Buf), __in int _P(_Value)) throw(...);
}
#else
_CRTIMP __declspec(noreturn) void __cdecl longjmp(__in jmp_buf _P(_Buf), __in int _P(_Value));
#endif

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif  /* _INC_SETJMP */
