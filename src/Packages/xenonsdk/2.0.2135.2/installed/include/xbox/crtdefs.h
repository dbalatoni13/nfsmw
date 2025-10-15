/***
*crtdefs.h - definitions/declarations common to all CRT
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file has mostly defines used by the entire CRT.
*
*       [Public]
*
****/

/* Define _CRTIMP */

#ifndef _CRTIMP
#if defined(_DLL) && _XBOX_VER < 200
#define _CRTIMP __declspec(dllimport)
#else   /* ndef _DLL */
#define _CRTIMP
#endif  /* _DLL */
#endif  /* _CRTIMP */


#ifndef _INC_CRTDEFS
#define _INC_CRTDEFS



/* Disable ddcrtsafe.h */
/* TODO (alecont): Do this during the security push, see VSW#405247; see below the pop_macro too */
/* #define _CRT_NO_DDCRTSAFE */
#pragma push_macro("_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES")
#pragma push_macro("_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT")
#pragma push_macro("_CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES")

#if     !defined(_WIN32)
#error ERROR: Only Win32 target supported!
#endif


#if !defined(_CRT_NOFORCE_MANIFEST) && defined(_VC_NODEFAULTLIB)
#define _CRT_NOFORCE_MANIFEST
#endif

#include <sal.h>

#ifdef _DLL

#if !defined(_CRT_NOFORCE_MANIFEST)

#if _MSC_FULL_VER >= 140040130

#include <crtassem.h>




#endif	/* _MSC_FULL_VER */

#endif	/* !defined(_CRT_NOFORCE_MANIFEST) */

#endif	/* _DLL */

#ifdef  _MSC_VER
#undef _CRT_PACKING
#define _CRT_PACKING 8

#pragma pack(push,_CRT_PACKING)
#endif  /* _MSC_VER */

#include <vadefs.h>

#ifdef  __cplusplus
extern "C" {
#endif

/* preprocessor string helpers */
#ifndef _CRT_STRINGIZE
#define __CRT_STRINGIZE(_Value) #_Value
#define _CRT_STRINGIZE(_Value) __CRT_STRINGIZE(_Value)
#endif

#ifndef _CRT_WIDE
#define __CRT_WIDE(_String) L ## _String
#define _CRT_WIDE(_String) __CRT_WIDE(_String)
#endif


#if !defined(_W64)
#if !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && _MSC_VER >= 1300
#define _W64 __w64
#else
#define _W64
#endif
#endif

/*
TEMP: Because the m<>n thunks don't currently correctly call copy constructors
we force all SCL code to be inline here
*/



/* Define _CRTIMP_NOIA64 */
#ifndef _CRTIMP_NOIA64
#define _CRTIMP_NOIA64 _CRTIMP
#endif

/* Define _CRTIMP2 */

#ifndef _CRTIMP2
#if defined(_DLL) && !defined(_STATIC_CPPLIB)
#if _XBOX_VER < 200
#define _CRTIMP2 __declspec(dllimport)
#else
#define _CRTIMP2
#endif
#else   /* ndef _DLL && !STATIC_CPPLIB */
#define _CRTIMP2
#endif  /* _DLL && !STATIC_CPPLIB */
#endif  /* _CRTIMP2 */

/* Define _CRTIMP_ALTERNATIVE */

#ifndef _CRTIMP_ALTERNATIVE
#ifdef  _DLL
#ifdef _CRT_ALTERNATIVE_INLINES
#define _CRTIMP_ALTERNATIVE
#else
#define _CRTIMP_ALTERNATIVE _CRTIMP
#define _CRT_ALTERNATIVE_IMPORTED
#endif
#else   /* ndef _DLL */
#define _CRTIMP_ALTERNATIVE
#endif  /* _DLL */
#endif  /* _CRTIMP_ALTERNATIVE */

/* Define _MRTIMP */

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


#ifndef _MCRTIMP
#if defined(_DLL) && _XBOX_VER < 200
#define _MCRTIMP __declspec(dllimport)
#else   /* ndef _DLL */
#define _MCRTIMP
#endif  /* _DLL */
#endif  /* _CRTIMP */

#ifndef __CLR_OR_THIS_CALL
#define __CLR_OR_THIS_CALL
#endif

#ifndef __CLRCALL_OR_CDECL
#define __CLRCALL_OR_CDECL __cdecl
#endif

#ifndef _CRTIMP_PURE
 #if defined(_M_CEE_PURE) || defined(_STATIC_CPPLIB)
  #define _CRTIMP_PURE
 #else
  #define _CRTIMP_PURE _CRTIMP
 #endif
#endif

#ifndef _PGLOBAL
#define _PGLOBAL
#endif

#ifndef _AGLOBAL
#define _AGLOBAL
#endif

/* define a specific constant for mixed mode */

/* Define __GOT_SECURE_LIB__ */
#define __GOT_SECURE_LIB__ 200402L

/* Define _CRT_INSECURE_DEPRECATE */
#ifndef _CRT_INSECURE_DEPRECATE
#if defined(_XBOX) && !defined(_XBOX_CRT_DEPRECATE_INSECURE) && !defined(_CRT_SECURE_NO_DEPRECATE)
#define _CRT_SECURE_NO_DEPRECATE
#endif
#ifdef _CRT_SECURE_NO_DEPRECATE
#define _CRT_INSECURE_DEPRECATE
#else
#define _CRT_INSECURE_DEPRECATE __declspec(deprecated)
#endif
#endif

/* Define _CRT_INSECURE_DEPRECATE_MEMORY */
#ifndef _CRT_INSECURE_DEPRECATE_MEMORY
#if defined(_CRT_SECURE_NO_DEPRECATE) || !defined(_CRT_SECURE_DEPRECATE_MEMORY)
#define _CRT_INSECURE_DEPRECATE_MEMORY
#else
#define _CRT_INSECURE_DEPRECATE_MEMORY __declspec(deprecated)
#endif
#endif

/* _SECURECRT_FILL_BUFFER_PATTERN is the same as _bNoMansLandFill */
#define _SECURECRT_FILL_BUFFER_PATTERN 0xFD

/* obsolete stuff */
#define _CRT_OBSOLETE __declspec(deprecated)

/* Define overload switches */
#if !defined(_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES)
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 0
#endif

#if !defined(_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT)
/* _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT is ignored if _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES is set to 0 */
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 0
#endif

#if !defined(_CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES)
#define _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES 0
#endif

/* Define _CRT_NONSTDC_DEPRECATE */
#if !defined(_CRT_NONSTDC_DEPRECATE)
#if defined(_XBOX) && !defined(_XBOX_CRT_DEPRECATE_NONSTDC) && !defined(_CRT_NONSTDC_NO_DEPRECATE)
#define _CRT_NONSTDC_NO_DEPRECATE
#endif
#if defined(_CRT_NONSTDC_NO_DEPRECATE) || defined(_POSIX_)
#define _CRT_NONSTDC_DEPRECATE
#else
#define _CRT_NONSTDC_DEPRECATE __declspec(deprecated)
#endif
#endif

#ifndef _SIZE_T_DEFINED
#ifdef  _WIN64
typedef unsigned __int64    size_t;
#else
typedef _W64 unsigned int   size_t;
#endif
#define _SIZE_T_DEFINED
#endif

#ifndef _INTPTR_T_DEFINED
#ifdef  _WIN64
typedef __int64             intptr_t;
#else
typedef _W64 int            intptr_t;
#endif
#define _INTPTR_T_DEFINED
#endif

#ifndef _UINTPTR_T_DEFINED
#ifdef  _WIN64
typedef unsigned __int64    uintptr_t;
#else
typedef _W64 unsigned int   uintptr_t;
#endif
#define _UINTPTR_T_DEFINED
#endif

#ifndef _PTRDIFF_T_DEFINED
#ifdef  _WIN64
typedef __int64             ptrdiff_t;
#else
typedef _W64 int            ptrdiff_t;
#endif
#define _PTRDIFF_T_DEFINED
#endif

#ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif

#ifndef _WCTYPE_T_DEFINED
typedef unsigned short wint_t;
typedef unsigned short wctype_t;
#define _WCTYPE_T_DEFINED
#endif

#ifndef _VA_LIST_DEFINED
typedef char *  va_list;
#define _VA_LIST_DEFINED
#endif

#ifdef  _USE_32BIT_TIME_T
#ifdef  _WIN64
#error You cannot use 32-bit time_t (_USE_32BIT_TIME_T) with _WIN64
#undef  _USE_32BIT_TIME_T
#endif
#else
#if     _INTEGRAL_MAX_BITS < 64
#define _USE_32BIT_TIME_T
#endif
#endif

#ifndef _ERRCODE_DEFINED
#define _ERRCODE_DEFINED
/* errcode is deprecated in favor or errno_t, which is part of the standard proposal */
#if !defined(__midl)
__declspec(deprecated) typedef int errcode;
#else
typedef int errcode;
#endif

typedef int errno_t;
#endif

#ifndef _TIME32_T_DEFINED
typedef _W64 long __time32_t;   /* 32-bit time value */
#define _TIME32_T_DEFINED
#endif

#ifndef _TIME64_T_DEFINED
#if     _INTEGRAL_MAX_BITS >= 64
typedef __int64 __time64_t;     /* 64-bit time value */
#endif
#define _TIME64_T_DEFINED
#endif

#ifndef _TIME_T_DEFINED
#ifdef _USE_32BIT_TIME_T
typedef __time32_t time_t;      /* time value */
#else
typedef __time64_t time_t;      /* time value */
#endif
#define _TIME_T_DEFINED         /* avoid multiple def's of time_t */
#endif

#ifndef _CONST_RETURN
#ifdef  __cplusplus
#define _CONST_RETURN  const
#define _CRT_CONST_CORRECT_OVERLOADS
#else
#define _CONST_RETURN
#endif
#endif

#if !defined(UNALIGNED)
#define UNALIGNED __unaligned
#endif

/* Define _CRTNOALIAS, _CRTRESTRICT */

#if     _MSC_FULL_VER >= 13102050
#if !defined(_MSC_VER_GREATER_THEN_13102050)
#define _MSC_VER_GREATER_THEN_13102050 
#endif
#endif

#if     ( defined(_M_IA64) && defined(_MSC_VER_GREATER_THEN_13102050) ) || _MSC_VER >= 1400
#ifndef _CRTNOALIAS
#define _CRTNOALIAS __declspec(noalias)
#endif  /* _CRTNOALIAS */

#ifndef _CRTRESTRICT
#define _CRTRESTRICT __declspec(restrict)
#endif  /* _CRTRESTRICT */

#else

#ifndef _CRTNOALIAS
#define _CRTNOALIAS
#endif  /* _CRTNOALIAS */

#ifndef _CRTRESTRICT
#define _CRTRESTRICT
#endif  /* _CRTRESTRICT */

#endif

/* Define __cdecl for non-Microsoft compilers */

#if     ( !defined(_MSC_VER) && !defined(__cdecl) )
#define __cdecl
#endif

#if !defined(__CRTDECL)
#define __CRTDECL   __cdecl
#endif


#define _ARGMAX 100

/* _TRUNCATE */
#if !defined(_TRUNCATE)
#define _TRUNCATE ((size_t)-1)
#endif

/* helper macros for cpp overloads */
#if defined(__cplusplus) && _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES

#define __DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_0(_ReturnType, _FuncName, _DstType) \
    extern "C++" \
    { \
    template <size_t _Size> \
    inline \
    _ReturnType __cdecl _FuncName(_DstType (&_Dst)[_Size]) \
    { \
        return _FuncName(_Dst, _Size); \
    } \
    }

#define __DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_1(_ReturnType, _FuncName, _DstType, _TType1) \
    extern "C++" \
    { \
    template <size_t _Size> \
    inline \
    _ReturnType __cdecl _FuncName(_DstType (&_Dst)[_Size], _TType1 _TArg1) \
    { \
        return _FuncName(_Dst, _Size, _TArg1); \
    } \
    }

#define __DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_2(_ReturnType, _FuncName, _DstType, _TType1, _TType2) \
    extern "C++" \
    { \
    template <size_t _Size> \
    inline \
    _ReturnType __cdecl _FuncName(_DstType (&_Dst)[_Size], _TType1 _TArg1, _TType2 _TArg2) \
    { \
        return _FuncName(_Dst, _Size, _TArg1, _TArg2); \
    } \
    }

#define __DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_3(_ReturnType, _FuncName, _DstType, _TType1, _TType2, _TType3) \
    extern "C++" \
    { \
    template <size_t _Size> \
    inline \
    _ReturnType __cdecl _FuncName(_DstType (&_Dst)[_Size], _TType1 _TArg1, _TType2 _TArg2, _TType3 _TArg3) \
    { \
        return _FuncName(_Dst, _Size, _TArg1, _TArg2, _TArg3); \
    } \
    }

#define __DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_4(_ReturnType, _FuncName, _DstType, _TType1, _TType2, _TType3, _TType4) \
    extern "C++" \
    { \
    template <size_t _Size> \
    inline \
    _ReturnType __cdecl _FuncName(_DstType (&_Dst)[_Size], _TType1 _TArg1, _TType2 _TArg2, _TType3 _TArg3, _TType4 _TArg4) \
    { \
        return _FuncName(_Dst, _Size, _TArg1, _TArg2, _TArg3, _TArg4); \
    } \
    }

#define __DEFINE_CPP_OVERLOAD_SECURE_FUNC_1_1(_ReturnType, _FuncName, _HType1, _DstType, _TType1) \
    extern "C++" \
    { \
    template <size_t _Size> \
    inline \
    _ReturnType __cdecl _FuncName(_HType1 _HArg1, _DstType (&_Dst)[_Size], _TType1 _TArg1) \
    { \
        return _FuncName(_HArg1, _Dst, _Size, _TArg1); \
    } \
    }

#define __DEFINE_CPP_OVERLOAD_SECURE_FUNC_1_2(_ReturnType, _FuncName, _HType1, _DstType, _TType1, _TType2) \
    extern "C++" \
    { \
    template <size_t _Size> \
    inline \
    _ReturnType __cdecl _FuncName(_HType1 _HArg1, _DstType (&_Dst)[_Size], _TType1 _TArg1, _TType2 _TArg2) \
    { \
        return _FuncName(_HArg1, _Dst, _Size, _TArg1, _TArg2); \
    } \
    }

#define __DEFINE_CPP_OVERLOAD_SECURE_FUNC_1_3(_ReturnType, _FuncName, _HType1, _DstType, _TType1, _TType2, _TType3) \
    extern "C++" \
    { \
    template <size_t _Size> \
    inline \
    _ReturnType __cdecl _FuncName(_HType1 _HArg1, _DstType (&_Dst)[_Size], _TType1 _TArg1, _TType2 _TArg2, _TType3 _TArg3) \
    { \
        return _FuncName(_HArg1, _Dst, _Size, _TArg1, _TArg2, _TArg3); \
    } \
    }

#define __DEFINE_CPP_OVERLOAD_SECURE_FUNC_2_0(_ReturnType, _FuncName, _HType1, _HType2, _DstType) \
    extern "C++" \
    { \
    template <size_t _Size> \
    inline \
    _ReturnType __cdecl _FuncName(_HType1 _HArg1, _HType2 _HArg2, _DstType (&_Dst)[_Size]) \
    { \
        return _FuncName(_HArg1, _HArg2, _Dst, _Size); \
    } \
    }

#define __DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_1_ARGLIST(_ReturnType, _FuncName, _VFuncName, _DstType, _TType1) \
    extern "C++" \
    { \
    template <size_t _Size> \
    inline \
    _ReturnType __cdecl _FuncName(_DstType (&_Dst)[_Size], _TType1 _TArg1, ...) \
    { \
        va_list _ArgList; \
        _crt_va_start(_ArgList, _TArg1); \
        return _VFuncName(_Dst, _Size, _TArg1, _ArgList); \
    } \
    }

#define __DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_2_ARGLIST(_ReturnType, _FuncName, _VFuncName, _DstType, _TType1, _TType2) \
    extern "C++" \
    { \
    template <size_t _Size> \
    inline \
    _ReturnType __cdecl _FuncName(_DstType (&_Dst)[_Size], _TType1 _TArg1, _TType2 _TArg2, ...) \
    { \
        va_list _ArgList; \
        _crt_va_start(_ArgList, _TArg2); \
        return _VFuncName(_Dst, _Size, _TArg1, _TArg2, _ArgList); \
    } \
    }

#define __DEFINE_CPP_OVERLOAD_SECURE_FUNC_SPLITPATH(_ReturnType, _FuncName, _DstType) \
    extern "C++" \
    { \
    template <size_t _DriveSize, size_t _DirSize, size_t _NameSize, size_t _ExtSize> \
    inline \
    _ReturnType __cdecl _FuncName(__in const _DstType *_Src, __out_ecount_opt(_DriveSize) _DstType (&_Drive)[_DriveSize], __out_ecount_opt(_DirSize) _DstType (&_Dir)[_DirSize], __out_ecount_opt(_NameSize) _DstType (&_Name)[_NameSize], __out_ecount_opt(_ExtSize) _DstType (&_Ext)[_ExtSize]) \
    { \
        return _FuncName(_Src, _Drive, _DriveSize, _Dir, _DirSize, _Name, _NameSize, _Ext, _ExtSize); \
    } \
    }

#else

#define __DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_0(_ReturnType, _FuncName, _DstType)
#define __DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_1(_ReturnType, _FuncName, _DstType, _TType1)
#define __DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_2(_ReturnType, _FuncName, _DstType, _TType1, _TType2)
#define __DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_3(_ReturnType, _FuncName, _DstType, _TType1, _TType2, _TType3)
#define __DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_4(_ReturnType, _FuncName, _DstType, _TType1, _TType2, _TType3, _TType4)
#define __DEFINE_CPP_OVERLOAD_SECURE_FUNC_1_1(_ReturnType, _FuncName, _HType1, _DstType, _TType1)
#define __DEFINE_CPP_OVERLOAD_SECURE_FUNC_1_2(_ReturnType, _FuncName, _HType1, _DstType, _TType1, _TType2)
#define __DEFINE_CPP_OVERLOAD_SECURE_FUNC_1_3(_ReturnType, _FuncName, _HType1, _DstType, _TType1, _TType2, _TType3)
#define __DEFINE_CPP_OVERLOAD_SECURE_FUNC_2_0(_ReturnType, _FuncName, _HType1, _HType2, _DstType)
#define __DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_1_ARGLIST(_ReturnType, _FuncName, _VFuncName, _DstType, _TType1)
#define __DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_2_ARGLIST(_ReturnType, _FuncName, _VFuncName, _DstType, _TType1, _TType2)
#define __DEFINE_CPP_OVERLOAD_SECURE_FUNC_SPLITPATH(_ReturnType, _FuncName, _DstType)

#endif /* _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES */

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_0(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _DstType) \
    __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_0_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _FuncName##_s, _DstType) \

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_1(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _DstType, _TType1) \
    __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_1_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _FuncName##_s, _DstType, _TType1)

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_2(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _DstType, _TType1, _TType2) \
    __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_2_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _FuncName##_s, _DstType, _TType1, _TType2)

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_3(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _DstType, _TType1, _TType2, _TType3) \
    __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_3_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _FuncName##_s, _DstType, _TType1, _TType2, _TType3)

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_4(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _DstType, _TType1, _TType2, _TType3, _TType4) \
    __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_4_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _FuncName##_s, _DstType, _TType1, _TType2, _TType3, _TType4)

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_1_1(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _HType1, _DstType, _TType1) \
    __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_1_1_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _FuncName##_s, _HType1, _DstType, _TType1)

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_2_0(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _HType1, _HType2, _DstType) \
    __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_2_0_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _FuncName##_s, _HType1, _HType2, _DstType)

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_1_ARGLIST(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _VFuncName, _DstType, _TType1) \
    __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_1_ARGLIST_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _VFuncName, _VFuncName##_s, _DstType, _TType1)

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_2_SIZE(_DeclSpec, _FuncName, _DstType, _TType1, _TType2) \
    __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_2_SIZE_EX(_DeclSpec, _FuncName, _FuncName##_s, _DstType, _TType1, _TType2)

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_3_SIZE(_DeclSpec, _FuncName, _DstType, _TType1, _TType2, _TType3) \
    __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_3_SIZE_EX(_DeclSpec, _FuncName, _FuncName##_s, _DstType, _TType1, _TType2, _TType3)


#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_0(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _DstType) \
    __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_0_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _FuncName##_s, _DstType) \

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_1(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _DstType, _TType1) \
    __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_1_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _FuncName##_s, _DstType, _TType1)

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_2(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _DstType, _TType1, _TType2) \
    __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_2_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _FuncName##_s, _DstType, _TType1, _TType2)

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_3(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _DstType, _TType1, _TType2, _TType3) \
    __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_3_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _FuncName##_s, _DstType, _TType1, _TType2, _TType3)

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_4(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _DstType, _TType1, _TType2, _TType3, _TType4) \
    __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_4_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _FuncName##_s, _DstType, _TType1, _TType2, _TType3, _TType4)

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_1_1(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _HType1, _DstType, _TType1) \
    __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_1_1_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _FuncName##_s, _HType1, _DstType, _TType1)

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_2_0(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _HType1, _HType2, _DstType) \
    __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_2_0_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _FuncName##_s, _HType1, _HType2, _DstType)

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_1_ARGLIST(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _VFuncName, _DstType, _TType1) \
    __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_1_ARGLIST_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _VFuncName, _VFuncName##_s, _DstType, _TType1)

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_2_SIZE(_DeclSpec, _FuncName, _DstType, _TType1, _TType2) \
    __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_2_SIZE_EX(_DeclSpec, _FuncName, _FuncName##_s, _DstType, _TType1, _TType2)

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_3_SIZE(_DeclSpec, _FuncName, _DstType, _TType1, _TType2, _TType3) \
    __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_3_SIZE_EX(_DeclSpec, _FuncName, _FuncName##_s, _DstType, _TType1, _TType2, _TType3)


#if defined(__cplusplus) && _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES

#define __RETURN_POLICY_SAME(_FunctionCall) return (_FunctionCall)
#define __RETURN_POLICY_DST(_FunctionCall) return ((_FunctionCall) == 0 ? _Dst : 0)
#define __RETURN_POLICY_VOID(_FunctionCall) (_FunctionCall); return
#define __EMPTY_DECLSPEC

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_0_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType) \
    __inline \
    _ReturnType __cdecl __insecure_##_FuncName(_DstType *_Dst) \
    { \
        _DeclSpec _ReturnType __cdecl _FuncName(_DstType *); \
        return _FuncName(_Dst); \
    } \
    extern "C++" \
    { \
    template <typename _T> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    _ReturnType __cdecl _FuncName(_T _Dst) \
    { \
        return __insecure_##_FuncName((_DstType *)_Dst); \
    } \
    template <size_t _Size> \
    inline \
    _ReturnType __cdecl _FuncName(_DstType (&_Dst)[_Size]) \
    { \
        _ReturnPolicy(_SecureFuncName(_Dst, _Size)); \
    } \
    template <> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    _ReturnType __cdecl _FuncName<1>(_DstType (&_Dst)[1]) \
    { \
        _ReturnPolicy(_SecureFuncName(_Dst, 1)); \
    } \
    }

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_0_CGETS(_ReturnType, _DeclSpec, _FuncName, _DstType) \
    __inline \
    _ReturnType __cdecl __insecure_##_FuncName(_DstType *_Dst) \
    { \
        _DeclSpec _ReturnType __cdecl _FuncName(_DstType *); \
        return _FuncName(_Dst); \
    } \
    extern "C++" \
    { \
    template <typename _T> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    _ReturnType __cdecl _FuncName(_T _Dst) \
    { \
        return __insecure_##_FuncName((_DstType *)_Dst); \
    } \
    template <size_t _Size> \
    inline \
    _ReturnType __cdecl _FuncName(_DstType (&_Dst)[_Size]) \
    { \
        size_t _SizeRead = 0; \
        errno_t _Err = _FuncName##_s(_Dst + 2, (_Size - 2) < ((size_t)_Dst[0]) ? (_Size - 2) : ((size_t)_Dst[0]), &_SizeRead); \
        _Dst[1] = (_DstType)(_SizeRead); \
        return (_Err == 0 ? _Dst + 2 : 0); \
    } \
    template <> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    _ReturnType __cdecl _FuncName<1>(_DstType (&_Dst)[1]) \
    { \
        return __insecure_##_FuncName((_DstType *)_Dst); \
    } \
    template <> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    _ReturnType __cdecl _FuncName<2>(_DstType (&_Dst)[2]) \
    { \
        return __insecure_##_FuncName((_DstType *)_Dst); \
    } \
    }

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_1_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1) \
    __inline \
    _ReturnType __cdecl __insecure_##_FuncName(_DstType *_Dst, _TType1 _TArg1) \
    { \
        _DeclSpec _ReturnType __cdecl _FuncName(_DstType *, _TType1); \
        return _FuncName(_Dst, _TArg1); \
    } \
    extern "C++" \
    { \
    template <typename _T> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    _ReturnType __cdecl _FuncName(_T _Dst, _TType1 _TArg1) \
    { \
        return __insecure_##_FuncName((_DstType *)_Dst, _TArg1); \
    } \
    template <size_t _Size> \
    inline \
    _ReturnType __cdecl _FuncName(_DstType (&_Dst)[_Size], _TType1 _TArg1) \
    { \
        _ReturnPolicy(_SecureFuncName(_Dst, _Size, _TArg1)); \
    } \
    template <> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    _ReturnType __cdecl _FuncName<1>(_DstType (&_Dst)[1], _TType1 _TArg1) \
    { \
        _ReturnPolicy(_SecureFuncName(_Dst, 1, _TArg1)); \
    } \
    }

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_2_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2) \
    __inline \
    _ReturnType __cdecl __insecure_##_FuncName(_DstType *_Dst, _TType1 _TArg1, _TType2 _TArg2) \
    { \
        _DeclSpec _ReturnType __cdecl _FuncName(_DstType *, _TType1, _TType2); \
        return _FuncName(_Dst, _TArg1, _TArg2); \
    } \
    extern "C++" \
    { \
    template <typename _T> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    _ReturnType __cdecl _FuncName(_T _Dst, _TType1 _TArg1, _TType2 _TArg2) \
    { \
        return __insecure_##_FuncName((_DstType *)_Dst, _TArg1, _TArg2); \
    } \
    template <size_t _Size> \
    inline \
    _ReturnType __cdecl _FuncName(_DstType (&_Dst)[_Size], _TType1 _TArg1, _TType2 _TArg2) \
    { \
        _ReturnPolicy(_SecureFuncName(_Dst, _Size, _TArg1, _TArg2)); \
    } \
    template <> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    _ReturnType __cdecl _FuncName<1>(_DstType (&_Dst)[1], _TType1 _TArg1, _TType2 _TArg2) \
    { \
        _ReturnPolicy(_SecureFuncName(_Dst, 1, _TArg1, _TArg2)); \
    } \
    }

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_3_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2, _TType3) \
    __inline \
    _ReturnType __cdecl __insecure_##_FuncName(_DstType *_Dst, _TType1 _TArg1, _TType2 _TArg2, _TType3 _TArg3) \
    { \
        _DeclSpec _ReturnType __cdecl _FuncName(_DstType *, _TType1, _TType2, _TType3); \
        return _FuncName(_Dst, _TArg1, _TArg2, _TArg3); \
    } \
    extern "C++" \
    { \
    template <typename _T> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    _ReturnType __cdecl _FuncName(_T _Dst, _TType1 _TArg1, _TType2 _TArg2, _TType3 _TArg3) \
    { \
        return __insecure_##_FuncName((_DstType *)_Dst, _TArg1, _TArg2, _TArg3); \
    } \
    template <size_t _Size> \
    inline \
    _ReturnType __cdecl _FuncName(_DstType (&_Dst)[_Size], _TType1 _TArg1, _TType2 _TArg2, _TType3 _TArg3) \
    { \
        _ReturnPolicy(_SecureFuncName(_Dst, _Size, _TArg1, _TArg2, _TArg3)); \
    } \
    template <> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    _ReturnType __cdecl _FuncName<1>(_DstType (&_Dst)[1], _TType1 _TArg1, _TType2 _TArg2, _TType3 _TArg3) \
    { \
        _ReturnPolicy(_SecureFuncName(_Dst, 1, _TArg1, _TArg2, _TArg3)); \
    } \
    }

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_4_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2, _TType3, _TType4) \
    __inline \
    _ReturnType __cdecl __insecure_##_FuncName(_DstType *_Dst, _TType1 _TArg1, _TType2 _TArg2, _TType3 _TArg3, _TType4 _TArg4) \
    { \
        _DeclSpec _ReturnType __cdecl _FuncName(_DstType *, _TType1, _TType2, _TType3, _TType4); \
        return _FuncName(_Dst, _TArg1, _TArg2, _TArg3, _TArg4); \
    } \
    extern "C++" \
    { \
    template <typename _T> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    _ReturnType __cdecl _FuncName(_T _Dst, _TType1 _TArg1, _TType2 _TArg2, _TType3 _TArg3, _TType4 _TArg4) \
    { \
        return __insecure_##_FuncName((_DstType *)_Dst, _TArg1, _TArg2, _TArg3, _TArg4); \
    } \
    template <size_t _Size> \
    inline \
    _ReturnType __cdecl _FuncName(_DstType (&_Dst)[_Size], _TType1 _TArg1, _TType2 _TArg2, _TType3 _TArg3, _TType4 _TArg4) \
    { \
        _ReturnPolicy(_SecureFuncName(_Dst, _Size, _TArg1, _TArg2, _TArg3, _TArg4)); \
    } \
    template <> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    _ReturnType __cdecl _FuncName<1>(_DstType (&_Dst)[1], _TType1 _TArg1, _TType2 _TArg2, _TType3 _TArg3, _TType4 _TArg4) \
    { \
        _ReturnPolicy(_SecureFuncName(_Dst, 1, _TArg1, _TArg2, _TArg3, _TArg4)); \
    } \
    }

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_1_1_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _HType1, _DstType, _TType1) \
    __inline \
    _ReturnType __cdecl __insecure_##_FuncName(_HType1 _HArg1, _DstType *_Dst, _TType1 _TArg1) \
    { \
        _DeclSpec _ReturnType __cdecl _FuncName(_HType1, _DstType *, _TType1); \
        return _FuncName(_HArg1, _Dst, _TArg1); \
    } \
    extern "C++" \
    { \
    template <typename _T> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    _ReturnType __cdecl _FuncName(_HType1 _HArg1, _T _Dst, _TType1 _TArg1) \
    { \
        return __insecure_##_FuncName(_HArg1, (_DstType *)_Dst, _TArg1); \
    } \
    template <size_t _Size> \
    inline \
    _ReturnType __cdecl _FuncName(_HType1 _HArg1, _DstType (&_Dst)[_Size], _TType1 _TArg1) \
    { \
        _ReturnPolicy(_SecureFuncName(_HArg1, _Dst, _Size, _TArg1)); \
    } \
    template <> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    _ReturnType __cdecl _FuncName<1>(_HType1 _HArg1, _DstType (&_Dst)[1], _TType1 _TArg1) \
    { \
        _ReturnPolicy(_SecureFuncName(_HArg1, _Dst, 1, _TArg1)); \
    } \
    }

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_2_0_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _HType1, _HType2, _DstType) \
    __inline \
    _ReturnType __cdecl __insecure_##_FuncName(_HType1 _HArg1, _HType2 _HArg2, _DstType *_Dst) \
    { \
        _DeclSpec _ReturnType __cdecl _FuncName(_HType1, _HType2, _DstType *); \
        return _FuncName(_HArg1, _HArg2, _Dst); \
    } \
    extern "C++" \
    { \
    template <typename _T> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    _ReturnType __cdecl _FuncName(_HType1 _HArg1, _HType2 _HArg2, _T _Dst) \
    { \
        return __insecure_##_FuncName(_HArg1, _HArg2, (_DstType *)_Dst); \
    } \
    template <size_t _Size> \
    inline \
    _ReturnType __cdecl _FuncName(_HType1 _HArg1, _HType2 _HArg2, _DstType (&_Dst)[_Size]) \
    { \
        _ReturnPolicy(_SecureFuncName(_HArg1, _HArg2, _Dst, _Size)); \
    } \
    template <> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    _ReturnType __cdecl _FuncName<1>(_HType1 _HArg1, _HType2 _HArg2, _DstType (&_Dst)[1]) \
    { \
        _ReturnPolicy(_SecureFuncName(_HArg1, _HArg2, _Dst, 1)); \
    } \
    }

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_1_ARGLIST_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _VFuncName, _SecureVFuncName, _DstType, _TType1) \
    __inline \
    _ReturnType __cdecl __insecure_##_VFuncName(_DstType *_Dst, _TType1 _TArg1, va_list _ArgList) \
    { \
        _DeclSpec _ReturnType __cdecl _VFuncName(_DstType *, _TType1, va_list); \
        return _VFuncName(_Dst, _TArg1, _ArgList); \
    } \
    extern "C++" \
    { \
    template <typename _T> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    _ReturnType __cdecl _FuncName(_T _Dst, _TType1 _TArg1, ...) \
    { \
        va_list _ArgList; \
        _crt_va_start(_ArgList, _TArg1); \
        return __insecure_##_VFuncName((_DstType *)_Dst, _TArg1, _ArgList); \
    } \
    template <size_t _Size> \
    inline \
    _ReturnType __cdecl _FuncName(_DstType (&_Dst)[_Size], _TType1 _TArg1, ...) \
    { \
        va_list _ArgList; \
        _crt_va_start(_ArgList, _TArg1); \
        _ReturnPolicy(_SecureVFuncName(_Dst, _Size, _TArg1, _ArgList)); \
    } \
    template <> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    _ReturnType __cdecl _FuncName<1>(_DstType (&_Dst)[1], _TType1 _TArg1, ...) \
    { \
        va_list _ArgList; \
        _crt_va_start(_ArgList, _TArg1); \
        _ReturnPolicy(_SecureVFuncName(_Dst, 1, _TArg1, _ArgList)); \
    } \
    template <typename _T> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    _ReturnType __cdecl _VFuncName(_T _Dst, _TType1 _TArg1, va_list _ArgList) \
    { \
        return __insecure_##_VFuncName((_DstType *)_Dst, _TArg1, _ArgList); \
    } \
    template <size_t _Size> \
    inline \
    _ReturnType __cdecl _VFuncName(_DstType (&_Dst)[_Size], _TType1 _TArg1, va_list _ArgList) \
    { \
        _ReturnPolicy(_SecureVFuncName(_Dst, _Size, _TArg1, _ArgList)); \
    } \
    template <> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    _ReturnType __cdecl _VFuncName<1>(_DstType (&_Dst)[1], _TType1 _TArg1, va_list _ArgList) \
    { \
        _ReturnPolicy(_SecureVFuncName(_Dst, 1, _TArg1, _ArgList)); \
    } \
    }

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_2_ARGLIST(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _VFuncName, _DstType, _TType1, _TType2) \
    __inline \
    _ReturnType __cdecl __insecure_##_VFuncName(_DstType *_Dst, _TType1 _TArg1, _TType2 _TArg2, va_list _ArgList) \
    { \
        _DeclSpec _ReturnType __cdecl _VFuncName(_DstType *, _TType1, _TType2, va_list); \
        return _VFuncName(_Dst, _TArg1, _TArg2, _ArgList); \
    } \
    extern "C++" \
    { \
    template <typename _T> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    _ReturnType __cdecl _FuncName(_T _Dst, _TType1 _TArg1, _TType2 _TArg2, ...) \
    { \
        va_list _ArgList; \
        _crt_va_start(_ArgList, _TArg2); \
        return __insecure_##_VFuncName((_DstType *)_Dst, _TArg1, _TArg2, _ArgList); \
    } \
    template <size_t _Size> \
    inline \
    _ReturnType __cdecl _FuncName(_DstType (&_Dst)[_Size], _TType1 _TArg1, _TType2 _TArg2, ...) \
    { \
        va_list _ArgList; \
        _crt_va_start(_ArgList, _TArg2); \
        _ReturnPolicy(_VFuncName##_s(_Dst, _Size, _TArg1, _TArg2, _ArgList)); \
    } \
    template <> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    _ReturnType __cdecl _FuncName<1>(_DstType (&_Dst)[1], _TType1 _TArg1, _TType2 _TArg2, ...) \
    { \
        va_list _ArgList; \
        _crt_va_start(_ArgList, _TArg2); \
        _ReturnPolicy(_VFuncName##_s(_Dst, 1, _TArg1, _TArg2, _ArgList)); \
    } \
    template <typename _T> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    _ReturnType __cdecl _VFuncName(_T _Dst, _TType1 _TArg1, _TType2 _TArg2, va_list _ArgList) \
    { \
        return __insecure_##_VFuncName((_DstType *)_Dst, _TArg1, _TArg2, _ArgList); \
    } \
    template <size_t _Size> \
    inline \
    _ReturnType __cdecl _VFuncName(_DstType (&_Dst)[_Size], _TType1 _TArg1, _TType2 _TArg2, va_list _ArgList) \
    { \
        _ReturnPolicy(_VFuncName##_s(_Dst, _Size, _TArg1, _TArg2, _ArgList)); \
    } \
    template <> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    _ReturnType __cdecl _VFuncName<1>(_DstType (&_Dst)[1], _TType1 _TArg1, _TType2 _TArg2, va_list _ArgList) \
    { \
        _ReturnPolicy(_VFuncName##_s(_Dst, 1, _TArg1, _TArg2, _ArgList)); \
    } \
    }

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_2_SIZE_EX(_DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2) \
    __inline \
    size_t __cdecl __insecure_##_FuncName(_DstType *_Dst, _TType1 _TArg1, _TType2 _TArg2) \
    { \
        _DeclSpec size_t __cdecl _FuncName(_DstType *, _TType1, _TType2); \
        return _FuncName(_Dst, _TArg1, _TArg2); \
    } \
    extern "C++" \
    { \
    template <typename _T> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    size_t __cdecl _FuncName(_T _Dst, _TType1 _TArg1, _TType2 _TArg2) \
    { \
        return __insecure_##_FuncName((_DstType *)_Dst, _TArg1, _TArg2); \
    } \
    template <size_t _Size> \
    inline \
    size_t __cdecl _FuncName(_DstType (&_Dst)[_Size], _TType1 _TArg1, _TType2 _TArg2) \
    { \
        size_t _Ret = 0; \
        _SecureFuncName(&_Ret, _Dst, _Size, _TArg1, _TArg2); \
        return (_Ret > 0 ? (_Ret - 1) : _Ret); \
    } \
    template <> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    size_t __cdecl _FuncName<1>(_DstType (&_Dst)[1], _TType1 _TArg1, _TType2 _TArg2) \
    { \
        size_t _Ret = 0; \
        _SecureFuncName(&_Ret, _Dst, 1, _TArg1, _TArg2); \
        return (_Ret > 0 ? (_Ret - 1) : _Ret); \
    } \
    }

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_3_SIZE_EX(_DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2, _TType3) \
    __inline \
    size_t __cdecl __insecure_##_FuncName(_DstType *_Dst, _TType1 _TArg1, _TType2 _TArg2, _TType3 _TArg3) \
    { \
        _DeclSpec size_t __cdecl _FuncName(_DstType *, _TType1, _TType2, _TType3); \
        return _FuncName(_Dst, _TArg1, _TArg2, _TArg3); \
    } \
    extern "C++" \
    { \
    template <typename _T> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    size_t __cdecl _FuncName(_T _Dst, _TType1 _TArg1, _TType2 _TArg2, _TType3 _TArg3) \
    { \
        return __insecure_##_FuncName((_DstType *)_Dst, _TArg1, _TArg2, _TArg3); \
    } \
    template <size_t _Size> \
    inline \
    size_t __cdecl _FuncName(_DstType (&_Dst)[_Size], _TType1 _TArg1, _TType2 _TArg2, _TType3 _TArg3) \
    { \
        size_t _Ret = 0; \
        _SecureFuncName(&_Ret, _Dst, _Size, _TArg1, _TArg2, _TArg3); \
        return (_Ret > 0 ? (_Ret - 1) : _Ret); \
    } \
    template <> \
    inline \
    _CRT_INSECURE_DEPRECATE \
    size_t __cdecl _FuncName<1>(_DstType (&_Dst)[1], _TType1 _TArg1, _TType2 _TArg2, _TType3 _TArg3) \
    { \
        size_t _Ret = 0; \
        _SecureFuncName(&_Ret, _Dst, 1, _TArg1, _TArg2, _TArg3); \
        return (_Ret > 0 ? (_Ret - 1) : _Ret); \
    } \
    }

#if _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_0_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType) \
    __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_0_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType)

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_0_CGETS(_ReturnType, _DeclSpec, _FuncName, _DstType) \
    __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_0_CGETS(_ReturnType, _DeclSpec, _FuncName, _DstType)

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_1_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1) \
    __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_1_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1)

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_2_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2) \
    __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_2_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2)

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_3_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2, _TType3) \
    __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_3_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2, _TType3)

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_4_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2, _TType3, _TType4) \
    __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_4_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2, _TType3, _TType4)

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_1_1_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _HType1, _DstType, _TType1) \
    __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_1_1_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _HType1, _DstType, _TType1)

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_2_0_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _HType1, _HType2, _DstType) \
    __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_2_0_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _HType1, _HType2, _DstType)

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_1_ARGLIST_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _VFuncName, _SecureVFuncName, _DstType, _TType1) \
    __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_1_ARGLIST_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _VFuncName, _SecureVFuncName, _DstType, _TType1)

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_2_ARGLIST(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _VFuncName, _DstType, _TType1, _TType2) \
    __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_2_ARGLIST(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _VFuncName, _DstType, _TType1, _TType2)

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_2_SIZE_EX(_DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2) \
    __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_2_SIZE_EX(_DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2)

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_3_SIZE_EX(_DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2, _TType3) \
    __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_3_SIZE_EX(_DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2, _TType3)

#else

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_0_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_DstType *);

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_0_GETS(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _DstType) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_DstType *);

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_1_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_DstType *, _TType1);

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_2_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_DstType *, _TType1, _TType2);

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_3_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2, _TType3) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_DstType *, _TType1, _TType2, _TType3);

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_4_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2, _TType3, _TType4) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_DstType *, _TType1, _TType2, _TType3, _TType4);

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_1_1_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _HType1, _DstType, _TType1) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_HType1, _DstType *, _TType1);

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_2_0_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _HType1, _HType2, _DstType) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_HType1, _HType2, _DstType *);

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_1_ARGLIST_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _VFuncName, _SecureVFuncName, _DstType, _TType1) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_DstType *, _TType1, ...); \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _VFuncName(_DstType *, _TType1, va_list);

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_2_ARGLIST(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _VFuncName, _DstType, _TType1, _TType2) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_DstType *, _TType1, _TType2, ...); \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _VFuncName(_DstType *, _TType1, _TType2, va_list);

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_2_SIZE_EX(_DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2) \
    _CRT_INSECURE_DEPRECATE _DeclSpec size_t __cdecl _FuncName(_DstType *, _TType1, _TType2);

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_3_SIZE_EX(_DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2, _TType3) \
    _CRT_INSECURE_DEPRECATE _DeclSpec size_t __cdecl _FuncName(_DstType *, _TType1, _TType2, _TType3);

#endif /* _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT */

#else

#define __RETURN_POLICY_SAME(_FunctionCall)
#define __RETURN_POLICY_DST(_FunctionCall)
#define __RETURN_POLICY_VOID(_FunctionCall)
#define __EMPTY_DECLSPEC

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_0_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_DstType *);

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_0_CGETS(_ReturnType, _DeclSpec, _FuncName, _DstType) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_DstType *);

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_1_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_DstType *, _TType1);

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_2_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_DstType *, _TType1, _TType2);

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_3_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2, _TType3) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_DstType *, _TType1, _TType2, _TType3);

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_4_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2, _TType3, _TType4) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_DstType *, _TType1, _TType2, _TType3, _TType4);

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_1_1_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _HType1, _DstType, _TType1) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_HType1, _DstType *, _TType1);

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_2_0_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _HType1, _HType2, _DstType) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_HType1, _HType2, _DstType *);

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_1_ARGLIST_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _VFuncName, _SecureVFuncName, _DstType, _TType1) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_DstType *, _TType1, ...); \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _VFuncName(_DstType *, _TType1, va_list);

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_2_ARGLIST(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _VFuncName, _DstType, _TType1, _TType2) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_DstType *, _TType1, _TType2, ...); \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _VFuncName(_DstType *, _TType1, _TType2, va_list);

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_2_SIZE_EX(_DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2) \
    _CRT_INSECURE_DEPRECATE _DeclSpec size_t __cdecl _FuncName(_DstType *, _TType1, _TType2);

#define __DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_3_SIZE_EX(_DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2, _TType3) \
    _CRT_INSECURE_DEPRECATE _DeclSpec size_t __cdecl _FuncName(_DstType *, _TType1, _TType2, _TType3);

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_0_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_DstType *);

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_0_GETS(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _DstType) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_DstType *);

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_1_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_DstType *, _TType1);

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_2_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_DstType *, _TType1, _TType2);

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_3_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2, _TType3) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_DstType *, _TType1, _TType2, _TType3);

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_4_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2, _TType3, _TType4) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_DstType *, _TType1, _TType2, _TType3, _TType4);

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_1_1_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _HType1, _DstType, _TType1) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_HType1, _DstType *, _TType1);

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_2_0_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _SecureFuncName, _HType1, _HType2, _DstType) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_HType1, _HType2, _DstType *);

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_1_ARGLIST_EX(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _VFuncName, _SecureVFuncName, _DstType, _TType1) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_DstType *, _TType1, ...); \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _VFuncName(_DstType *, _TType1, va_list);

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_2_ARGLIST(_ReturnType, _ReturnPolicy, _DeclSpec, _FuncName, _VFuncName, _DstType, _TType1, _TType2) \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _FuncName(_DstType *, _TType1, _TType2, ...); \
    _CRT_INSECURE_DEPRECATE _DeclSpec _ReturnType __cdecl _VFuncName(_DstType *, _TType1, _TType2, va_list);

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_2_SIZE_EX(_DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2) \
    _CRT_INSECURE_DEPRECATE _DeclSpec size_t __cdecl _FuncName(_DstType *, _TType1, _TType2);

#define __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_3_SIZE_EX(_DeclSpec, _FuncName, _SecureFuncName, _DstType, _TType1, _TType2, _TType3) \
    _CRT_INSECURE_DEPRECATE _DeclSpec size_t __cdecl _FuncName(_DstType *, _TType1, _TType2, _TType3);

#endif /* _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES */

struct threadlocaleinfostruct;
struct threadmbcinfostruct;
typedef struct threadlocaleinfostruct * pthreadlocinfo;
typedef struct threadmbcinfostruct * pthreadmbcinfo;
struct __lc_time_data;

typedef struct localeinfo_struct
{
    pthreadlocinfo locinfo;
    pthreadmbcinfo mbcinfo;
} _locale_tstruct, *_locale_t;

#ifndef _TAGLC_ID_DEFINED
typedef struct tagLC_ID {
        unsigned short wLanguage;
        unsigned short wCountry;
        unsigned short wCodePage;
} LC_ID, *LPLC_ID;
#define _TAGLC_ID_DEFINED
#endif  /* _TAGLC_ID_DEFINED */

#ifndef _THREADLOCALEINFO
typedef struct threadlocaleinfostruct {
        int refcount;
        unsigned int lc_codepage;
        unsigned int lc_collate_cp;
        unsigned long lc_handle[6]; /* LCID */
        LC_ID lc_id[6];
        struct {
            char *locale;
            wchar_t *wlocale;
            int *refcount;
            int *wrefcount;
        } lc_category[6];
        int lc_clike;
        int mb_cur_max;
        int * lconv_intl_refcount;
        int * lconv_num_refcount;
        int * lconv_mon_refcount;
        struct lconv * lconv;
        int * ctype1_refcount;
        unsigned short * ctype1;
        const unsigned short * pctype;
        const unsigned char * pclmap;
        const unsigned char * pcumap;
        struct __lc_time_data * lc_time_curr;
} threadlocinfo;
#define _THREADLOCALEINFO
#endif

#ifdef  __cplusplus
}
#endif

/* 
We want to have declarations of our parameters in our headers to allow autocomplete to see
more descriptive information. However, to avoid any perf hit of adding all those extra
symbols to the symbol table on every compilation, we protect the parameters with _P macro
that expands to something only during autocomplete processing

In case a third party tool wants to see the parameter names, we also provide a named CRT
option that has the same impact as __FEACP__
*/

#define _PARAM(x) _P(x)
#if defined(__FEACP__) || defined (_CRT_ENABLE_PARAMETER_NAMES) || defined(_PREFAST_)
#define _P(x)   x
#else
#define _P(x)
#endif

#if defined(_PREFAST_) && defined(_PFT_SHOULD_CHECK_RETURN)
#define __checkReturn_opt __checkReturn
#else
#define __checkReturn_opt
#endif

#if defined(_PREFAST_) && defined(_PFT_SHOULD_CHECK_RETURN_WAT)
#define __checkReturn_wat __checkReturn
#else
#define __checkReturn_wat
#endif

/* TODO (alecont): Remove this during the security push, see VSW#405247 */
#pragma pop_macro("_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES")
#pragma pop_macro("_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT")
#pragma pop_macro("_CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES")

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif  /* _INC_CRTDEFS */
