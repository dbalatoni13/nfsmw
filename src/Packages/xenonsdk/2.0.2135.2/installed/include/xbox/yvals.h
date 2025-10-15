/* yvals.h values header for Microsoft C/C++ */
#pragma once
#ifndef _YVALS
#define _YVALS

#ifdef _NOTHISCALL_
    #define __thiscall
#endif

#include <crtdefs.h>

#ifdef _DLL
#ifndef _STATIC_CPPLIB
#define _STATIC_CPPLIB
#endif
#endif

#ifdef  _MSC_VER
#pragma pack(push,_CRT_PACKING)
#endif  /* _MSC_VER */

#define _CPPLIB_VER	404

#define __PURE_APPDOMAIN_GLOBAL

#ifndef __CRTDECL
#define __CRTDECL   __cdecl
#endif

#ifndef __CLR_OR_THIS_CALL
#define __CLR_OR_THIS_CALL
#endif

#ifndef __CLRCALL_OR_CDECL
#define __CLRCALL_OR_CDECL __cdecl
#endif

/*
TEMP: Because the m<>n thunks don't currently correctly call copy constructors
we force all SCL code to be inline here
*/

		/* NAMING PROPERTIES */
#define _WIN32_C_LIB	1

		/* THREAD AND LOCALE CONTROL */
#define _MULTI_THREAD 1 /* nontrivial locks if multithreaded */
#define _IOSTREAM_OP_LOCKS	1	/* lock iostream operations */
#define _GLOBAL_LOCALE	0	/* 0 for per-thread locales, 1 for shared */

		/* THREAD-LOCAL STORAGE */
#define _COMPILER_TLS	1	/* 1 if compiler supports TLS directly */
 #if _MULTI_THREAD
  #define _TLS_QUAL	__declspec(thread)	/* TLS qualifier, if any */

 #else /* _MULTI_THREAD */
  #define _TLS_QUAL
 #endif /* _MULTI_THREAD */

 #ifndef _HAS_EXCEPTIONS
  #ifndef _CPPUNWIND
   #define  _HAS_EXCEPTIONS  0  /* predefine as 1 to enable exceptions in STL even if compiler switch not thrown. */
  #else
   #define  _HAS_EXCEPTIONS  1  /* predefine as 0 to disable exceptions handling in STL */
  #endif /* _CPPUNWIND */
 #endif /* _HAS_EXCEPTIONS */


#ifndef _HAS_IMMUTABLE_SETS
 #define _HAS_IMMUTABLE_SETS 0
#endif /* _HAS_IMMUTABLE_SETS */

#ifndef _HAS_STRICT_CONFORMANCE
 #define _HAS_STRICT_CONFORMANCE 0
#endif /* _HAS_STRICT_CONFORMANCE */

#define _GLOBAL_USING	1

#if !defined (_HAS_ITERATOR_DEBUGGING)
	#if defined (_DEBUG)
		#define _HAS_ITERATOR_DEBUGGING	1	/* for range checks, etc. */
	#else
		#define _HAS_ITERATOR_DEBUGGING	0
	#endif	/* defined (_DEBUG) */
#else
	#if !defined (_DEBUG) && _HAS_ITERATOR_DEBUGGING != 0
		#include <crtwrn.h>
		#pragma _CRT_WARNING( _NO_ITERATOR_DEBUGGING )
		#undef _HAS_ITERATOR_DEBUGGING
		#define _HAS_ITERATOR_DEBUGGING 0
	#endif
#endif /* !defined (_HAS_ITERATOR_DEBUGGING) */

/* _SECURE_CRT definitions */

#define __STR2WSTR(str)    L##str
#define _STR2WSTR(str)     __STR2WSTR(str)

#define __FILEW__          _STR2WSTR(__FILE__)
#define __FUNCTIONW__      _STR2WSTR(__FUNCTION__)

/* _SECURE_SCL switches: default values */

#if !defined (_SECURE_SCL)
#define _SECURE_SCL 1
#endif

#if !defined (_SECURE_SCL_DEPRECATE)
#define _SECURE_SCL_DEPRECATE 0
#endif

#if !defined (_SECURE_SCL_THROWS)
#define _SECURE_SCL_THROWS 0
#endif

/* _SECURE_SCL switches: helper macros */

#if _SECURE_SCL && _SECURE_SCL_DEPRECATE
#define _SCL_INSECURE_DEPRECATE __declspec(deprecated)
#else
#define _SCL_INSECURE_DEPRECATE
#endif

/*
 * Assert in debug builds.
 * set errno and return
 *
 */
// Only define _SCL_SECURE_INVALID_PARAMETER if it has not previously been
// defined. This makes it easier for applications to override the error
// handling for _SECURE_SCL
#ifndef _SCL_SECURE_INVALID_PARAMETER
 #ifdef _DEBUG 
  #define _SCL_SECURE_INVALID_PARAMETER(expr) ::_invalid_parameter(__STR2WSTR(#expr), __FUNCTIONW__, __FILEW__, __LINE__, 0)
 #else
  // Handle errors without a function call, to avoid incurring significant
  // overhead even when no error is detected.
  #define _SCL_SECURE_INVALID_PARAMETER(expr) __debugbreak()
 #endif
#endif

 #if _SECURE_SCL_THROWS

 #define _SCL_SECURE_INVALID_ARGUMENT_NO_ASSERT		_Xinvarg()
 #define _SCL_SECURE_OUT_OF_RANGE_NO_ASSERT			_Xran()

 #else /* _SECURE_SCL_THROWS */

 #define _SCL_SECURE_INVALID_ARGUMENT_NO_ASSERT		_SCL_SECURE_INVALID_PARAMETER("invalid argument")
 #define _SCL_SECURE_OUT_OF_RANGE_NO_ASSERT			_SCL_SECURE_INVALID_PARAMETER("out of range")

 #endif /* _SECURE_SCL_THROWS */

 #define _SCL_SECURE_ALWAYS_VALIDATE(cond)				\
	{													\
		if (!(cond))									\
		{												\
			_ASSERTE((#cond, 0));						\
			_SCL_SECURE_INVALID_ARGUMENT_NO_ASSERT;		\
		}												\
	}

 #define _SCL_SECURE_ALWAYS_VALIDATE_RANGE(cond)		\
	{													\
		if (!(cond))									\
		{												\
			_ASSERTE((#cond, 0));						\
			_SCL_SECURE_OUT_OF_RANGE_NO_ASSERT;			\
		}												\
	}

// validation a la Secure CRT
 #define _SCL_SECURE_CRT_VALIDATE(cond, retvalue)		\
	{													\
		if (!(cond))									\
		{												\
			_ASSERTE((#cond, 0));						\
			_SCL_SECURE_INVALID_PARAMETER(cond);		\
			return (retvalue);							\
		}												\
	}

 #if _SECURE_SCL

 #define _SCL_SECURE_VALIDATE(cond)						\
	{													\
		if (!(cond))									\
		{												\
			_ASSERTE((#cond, 0));						\
			_SCL_SECURE_INVALID_ARGUMENT_NO_ASSERT;		\
		}												\
	}

 #define _SCL_SECURE_VALIDATE_RANGE(cond)				\
	{													\
		if (!(cond))									\
		{												\
			_ASSERTE((#cond, 0));						\
			_SCL_SECURE_OUT_OF_RANGE_NO_ASSERT;			\
		}												\
	}

 #define _SCL_SECURE_INVALID_ARGUMENT					\
	{													\
		_ASSERTE(("SCL Secure Invalid Argument", 0));	\
		_SCL_SECURE_INVALID_ARGUMENT_NO_ASSERT;			\
	}
 #define _SCL_SECURE_OUT_OF_RANGE						\
	{													\
		_ASSERTE(("SCL Secure Out Of Range", 0));		\
		_SCL_SECURE_OUT_OF_RANGE_NO_ASSERT;				\
	}

 #define _SCL_SECURE_MOVE(func, dst, size, src, count)	func((dst), (size), (src), (count))
 #define _SCL_SECURE_COPY(func, dst, size, src, count)	func((dst), (size), (src), (count))

 #if !defined(_SCL_INSECURE_DEPRECATE)
  #define _SCL_INSECURE_DEPRECATE	__declspec(deprecated)
 #endif

 #else /* _SECURE_SCL */
 
 #define _SCL_SECURE_VALIDATE(cond) 
 #define _SCL_SECURE_VALIDATE_RANGE(cond) 

 #define _SCL_SECURE_INVALID_ARGUMENT 
 #define _SCL_SECURE_OUT_OF_RANGE 

 #define _SCL_SECURE_MOVE(func, dst, size, src, count)	func((dst), (src), (count))
 #define _SCL_SECURE_COPY(func, dst, size, src, count)	func((dst), (src), (count))

 #endif /* _SECURE_SCL */

#include <use_ansi.h>

 #define _VC6SP2	0 /* define as 1 to fix linker errors with V6.0 SP2 */

/* Define _CRTIMP2 */
 #ifndef _CRTIMP2

   #if defined(_DLL) && !defined(_STATIC_CPPLIB) && _XBOX_VER < 200
    #define _CRTIMP2	__declspec(dllimport)

   #else   /* ndef _DLL && !STATIC_CPPLIB */
    #define _CRTIMP2
   #endif  /* _DLL && !STATIC_CPPLIB */

 #endif  /* _CRTIMP2 */

#ifndef _MRTIMP
    #define _MRTIMP __declspec(dllimport)
#endif  /* _MRTIMP */

/* Define _MRTIMP2 */
 #ifndef _MRTIMP2

   #if defined(_DLL) && !defined(_STATIC_CPPLIB) && _XBOX_VER < 200
    #define _MRTIMP2	__declspec(dllimport)

   #else   /* ndef _DLL && !STATIC_CPPLIB */
    #define _MRTIMP2
   #endif  /* _DLL && !STATIC_CPPLIB */

 #endif  /* _MRTIMP2 */

/* Define _MRTIMP2_NPURE */
 #ifndef _MRTIMP2_NPURE

    #define _MRTIMP2_NPURE

 #endif  /* _MRTIMP2_NPURE */

 #if defined(_DLL) && !defined(_STATIC_CPPLIB) && !defined(_M_CEE_PURE)
  #define _DLL_CPPLIB
 #endif

 #ifndef _CRTIMP2_PURE
     #define _CRTIMP2_PURE _CRTIMP2
 #endif

/* Do not dllexport if we are linking to the debug dll and turning 
 * _HAS_ITERATOR_DEBUGGING off
 */
#if defined(_DLL_CPPLIB) && defined(_DEBUG) && !_HAS_ITERATOR_DEBUGGING
 #define _CRTIMP2_PURE_DEBUG_IT
#else
 #define _CRTIMP2_PURE_DEBUG_IT _CRTIMP2
#endif

#if !defined(_CRTDATA2)
    #define _CRTDATA2 _CRTIMP2
#endif


  #define _DEPRECATED	__declspec(deprecated)



		/* NAMESPACE */

 #if defined(__cplusplus)
  #define _STD_BEGIN	namespace std {
  #define _STD_END		}
  #define _STD	::std::

/*
We use the stdext (standard extension) namespace to contain extensions that are not part of the current standard
*/
  #define _STDEXT_BEGIN	    namespace stdext {
  #define _STDEXT_END		}
  #define _STDEXT	        ::stdext::

  #ifdef _STD_USING
   #define _C_STD_BEGIN	namespace std {	/* only if *.c compiled as C++ */
   #define _C_STD_END	}
   #define _CSTD	::std::

  #else /* _STD_USING */
/* #define _GLOBAL_USING	*.h in global namespace, c* imports to std */

   #define _C_STD_BEGIN
   #define _C_STD_END
   #define _CSTD	::
  #endif /* _STD_USING */

  #define _C_LIB_DECL		extern "C" {	/* C has extern "C" linkage */
  #define _END_C_LIB_DECL	}
  #define _EXTERN_C			extern "C" {
  #define _END_EXTERN_C		}

 #else /* __cplusplus */
  #define _STD_BEGIN
  #define _STD_END
  #define _STD

  #define _C_STD_BEGIN
  #define _C_STD_END
  #define _CSTD

  #define _C_LIB_DECL
  #define _END_C_LIB_DECL
  #define _EXTERN_C
  #define _END_EXTERN_C
 #endif /* __cplusplus */

 #define _Restrict	restrict

 #ifdef __cplusplus
_STD_BEGIN
typedef bool _Bool;
_STD_END
 #endif /* __cplusplus */

		/* VC++ COMPILER PARAMETERS */
#define _LONGLONG	__int64
#define _ULONGLONG	unsigned __int64
#define _LLONG_MAX	0x7fffffffffffffff
#define _ULLONG_MAX	0xffffffffffffffff

		/* INTEGER PROPERTIES */
#define _C2			1	/* 0 if not 2's complement */

#define _MAX_EXP_DIG	8	/* for parsing numerics */
#define _MAX_INT_DIG	32
#define _MAX_SIG_DIG	36

typedef _LONGLONG _Longlong;
typedef _ULONGLONG _ULonglong;

		/* STDIO PROPERTIES */
#define _Filet _iobuf

 #ifndef _FPOS_T_DEFINED
  #define _FPOSOFF(fp)	((long)(fp))
 #endif /* _FPOS_T_DEFINED */

#define _IOBASE	_base
#define _IOPTR	_ptr
#define _IOCNT	_cnt

		/* MULTITHREAD PROPERTIES */
		/* LOCK MACROS */
#define _LOCK_LOCALE	0
#define _LOCK_MALLOC	1
#define _LOCK_STREAM	2
#define _LOCK_DEBUG		3
#define _MAX_LOCK		4	/* one more than highest lock number */

 #ifdef __cplusplus
_STD_BEGIN
		// CLASS _Lockit
class _CRTIMP2_PURE _Lockit
	{	// lock while object in existence -- MUST NEST
public:
  #if _MULTI_THREAD

	explicit __thiscall _Lockit();	// set default lock
	explicit __thiscall _Lockit(int);	// set the lock
	__thiscall ~_Lockit();	// clear the lock

private:
    static _MRTIMP2_NPURE void __cdecl _Lockit_ctor(_Lockit *);
    static _MRTIMP2_NPURE void __cdecl _Lockit_ctor(_Lockit *, int);
    static _MRTIMP2_NPURE void __cdecl _Lockit_dtor(_Lockit *);

	__CLR_OR_THIS_CALL _Lockit(const _Lockit&);				// not defined
	_Lockit& __CLR_OR_THIS_CALL operator=(const _Lockit&);	// not defined

	int _Locktype;

  #else /* _MULTI_THREAD */

	explicit _Lockit()
		{	// do nothing
		}

	explicit _Lockit(int)
		{	// do nothing
		}

	~_Lockit()
		{	// do nothing
		}
  #endif /* _MULTI_THREAD */

	};

class _CRTIMP2_PURE _Mutex
	{	// lock under program control
public:

  #if _MULTI_THREAD
    
	__thiscall _Mutex();
	__thiscall ~_Mutex();
	void __thiscall _Lock();
	void __thiscall _Unlock();

private:
    static _MRTIMP2_NPURE void __cdecl _Mutex_ctor(_Mutex *);
    static _MRTIMP2_NPURE void __cdecl _Mutex_dtor(_Mutex *);
    static _MRTIMP2_NPURE void __cdecl _Mutex_Lock(_Mutex *);
    static _MRTIMP2_NPURE void __cdecl _Mutex_Unlock(_Mutex *);

	__CLR_OR_THIS_CALL _Mutex(const _Mutex&);				// not defined
	_Mutex& __CLR_OR_THIS_CALL operator=(const _Mutex&);	// not defined
	void *_Mtx;

  #else /* _MULTI_THREAD */
    void _Lock()
		{	// do nothing
		}

	void _Unlock()
		{	// do nothing
		}
  #endif /* _MULTI_THREAD */

	};

class _CRTIMP2_PURE _Init_locks
	{	// initialize mutexes
public:

 #if _MULTI_THREAD
    
	__thiscall _Init_locks();
	__thiscall ~_Init_locks();

private:
    static _MRTIMP2_NPURE void __cdecl _Init_locks_ctor(_Init_locks *);
    static _MRTIMP2_NPURE void __cdecl _Init_locks_dtor(_Init_locks *);

 #else /* _MULTI_THREAD */
	_Init_locks()
		{	// do nothing
		}

	~_Init_locks()
		{	// do nothing
		}
 #endif /* _MULTI_THREAD */

	};
_STD_END
 #endif /* __cplusplus */



		/* MISCELLANEOUS MACROS AND TYPES */
_C_STD_BEGIN
_MRTIMP2 void __cdecl _Atexit(void (__cdecl *)(void));

typedef int _Mbstatet;

#define _ATEXIT_T	void
#define _Mbstinit(x)	mbstate_t x = {0}
_C_STD_END

  #define _EXTERN_TEMPLATE	template
  #define _THROW_BAD_ALLOC	_THROW1(...)

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif /* _YVALS */


/*
 * Copyright (c) 1992-2004 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
 V4.04:0009 */
