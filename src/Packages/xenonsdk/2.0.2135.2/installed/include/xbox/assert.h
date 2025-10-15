/***
*assert.h - define the assert macro
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Defines the assert(exp) macro.
*       [ANSI/System V]
*
*       [Public]
*
****/

#include <crtdefs.h>

#undef  assert

#ifdef  NDEBUG

#define assert(_Expression)     ((void)0)

#else

#ifdef  __cplusplus
extern "C" {
#endif

_CRTIMP void __cdecl _wassert(__in const wchar_t * _P(_Message), __in const wchar_t *_P(_File), __in unsigned _P(_Line));

#ifdef  __cplusplus
}
#endif

#define assert(_Expression) (void)( (_Expression) || (_wassert(_CRT_WIDE(#_Expression), _CRT_WIDE(__FILE__), __LINE__), 0) )

#endif  /* NDEBUG */
