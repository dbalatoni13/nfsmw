
/***
*new.h - declarations and definitions for C++ memory allocation functions
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Contains the declarations for C++ memory allocation functions.
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_NEW
#define _INC_NEW

#ifdef  __cplusplus

#ifndef _MSC_EXTENSIONS
#include <new>
#endif

#include <crtdefs.h>

/* Protect against #define of new */
#pragma push_macro("new")
#undef  new


/* Define _CRTIMP2 */
#ifndef _CRTIMP2
#if defined(_DLL) && !defined(_STATIC_CPPLIB) && _XBOX_VER < 200
#define _CRTIMP2 __declspec(dllimport)
#else   /* ndef _DLL && !STATIC_CPPLIB */
#define _CRTIMP2
#endif  /* _DLL && !STATIC_CPPLIB */
#endif  /* _CRTIMP2 */

/* Define __cdecl for non-Microsoft compilers */

#if     ( !defined(_MSC_VER) && !defined(__cdecl) )
#define __cdecl
#endif

#ifdef  _MSC_EXTENSIONS

namespace std {

typedef void (__cdecl * new_handler) ();
_CRTIMP2 new_handler __cdecl set_new_handler(__in_opt new_handler _P(_NewHandler)) throw();

};

using std::new_handler;
using std::set_new_handler;
#endif

#ifndef __NOTHROW_T_DEFINED
#define __NOTHROW_T_DEFINED
namespace std {
        /* placement new tag type to suppress exceptions */
        struct nothrow_t {};

        /* constant for placement new tag */
        extern const nothrow_t nothrow;
};

__bcount_opt(_Size) void *__CRTDECL operator new(size_t _P(_Size), const std::nothrow_t&) throw();
__bcount_opt(_Size) void *__CRTDECL operator new[](size_t _P(_Size), const std::nothrow_t&) throw();
void __CRTDECL operator delete(void *, const std::nothrow_t&) throw();
void __CRTDECL operator delete[](void *, const std::nothrow_t&) throw();
#endif

#ifndef __PLACEMENT_NEW_INLINE
#define __PLACEMENT_NEW_INLINE
inline void *__CRTDECL operator new(size_t, void *_Where)
        {return (_Where); }
#if     _MSC_VER >= 1200
inline void __CRTDECL operator delete(void *, void *)
        {return; }
#endif
#endif


/* 
 * new mode flag -- when set, makes malloc() behave like new()
 */

_CRTIMP int __cdecl _query_new_mode( void );
_CRTIMP int __cdecl _set_new_mode( __in int _P(_NewMode));

#ifndef _PNH_DEFINED
typedef int (__cdecl * _PNH)( size_t );
#define _PNH_DEFINED
#endif

_CRTIMP _PNH __cdecl _query_new_handler( void );
_CRTIMP _PNH __cdecl _set_new_handler( __in_opt _PNH _P(_NewHandler));
/* _set_new_handler(int) is used to disambiguate NULL/0 */
_CRTIMP _PNH __cdecl _set_new_handler( __in int _P(_NewHandler));

#if _MSC_VER >= 1400 && defined(_M_CEE)
#ifndef __MPNH_DEFINED
typedef int (__clrcall * __MPNH)( size_t );
#define __MPNH_DEFINED
#endif

_MRTIMP __MPNH __cdecl __query_new_handler_m( void );
#endif

/*
 * Microsoft extension: 
 *
 * _NO_ANSI_NEW_HANDLER de-activates the ANSI new_handler. Use this special value
 * to support old style (_set_new_handler) behavior.
 */

#ifndef _NO_ANSI_NH_DEFINED
#define _NO_ANSI_NEW_HANDLER  ((new_handler)-1)
#define _NO_ANSI_NEW_HANDLER_M  ((_new_handler_m)-1)
#define _NO_ANSI_NH_DEFINED
#endif

#pragma pop_macro("new")

#endif  /* __cplusplus */

#endif  /* _INC_NEW */
