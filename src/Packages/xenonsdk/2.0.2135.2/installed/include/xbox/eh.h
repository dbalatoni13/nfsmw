/***
*eh.h - User include file for exception handling.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       User include file for exception handling.
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#include <crtdefs.h>

#ifndef _INC_EH
#define _INC_EH
#ifndef RC_INVOKED
#ifdef  _MSC_VER
/* Currently, all MS C compilers for Win32 platforms default to 8 byte 
 * alignment.
 */
#pragma pack(push,_CRT_PACKING)
#endif  /* _MSC_VER */

#ifndef __cplusplus
#error "eh.h is only for C++!"
#endif

typedef void (__cdecl *terminate_function)();
typedef void (__cdecl *unexpected_function)();
typedef void (__cdecl *terminate_handler)();
typedef void (__cdecl *unexpected_handler)();

struct _EXCEPTION_POINTERS;
typedef void (__cdecl *_se_translator_function)(unsigned int, struct _EXCEPTION_POINTERS*);
_CRTIMP __declspec(noreturn) void __cdecl terminate(void);
_CRTIMP void __cdecl unexpected(void);

_CRTIMP int __cdecl _is_exception_typeof(__in const type_info &_P(_Type), __in struct _EXCEPTION_POINTERS * _P(_ExceptionPtr));

_CRTIMP terminate_function __cdecl set_terminate(__in_opt terminate_function _P(_NewPtFunc));
extern "C" _CRTIMP terminate_function __cdecl _get_terminate(void);
_CRTIMP unexpected_function __cdecl set_unexpected(__in_opt unexpected_function _P(_NewPtFunc));
extern "C" _CRTIMP unexpected_function __cdecl _get_unexpected(void);
_CRTIMP _se_translator_function __cdecl _set_se_translator(__in_opt _se_translator_function _P(_NewPtFunc));
_CRTIMP bool __cdecl __uncaught_exception();

/*
 * These overload helps in resolving NULL
 */
_CRTIMP terminate_function __cdecl set_terminate(__in_opt int _P(_Zero));
_CRTIMP unexpected_function __cdecl set_unexpected(__in_opt int _P(_Zero));
_CRTIMP _se_translator_function __cdecl _set_se_translator(__in_opt int _P(_Zero));

#if _MSC_VER >= 1400 && defined(__cplusplus) && defined(_M_CEE)
typedef void (__clrcall *__terminate_function_m)();
typedef void (__clrcall *__unexpected_function_m)();
typedef void (__clrcall *__terminate_handler_m)();

_MRTIMP __terminate_function_m __cdecl set_terminate(__in_opt __terminate_function_m _P(_NewPtFunc));
_MRTIMP __unexpected_function_m __cdecl set_unexpected(__in_opt __unexpected_function_m _P(_NewPtFunc));
#endif

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */
#endif /* RC_INVOKED */
#endif  /* _INC_EH */
