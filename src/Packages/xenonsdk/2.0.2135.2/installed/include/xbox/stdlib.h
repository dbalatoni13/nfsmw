/***
*stdlib.h - declarations/definitions for commonly used library functions
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This include file contains the function declarations for commonly
*       used library functions which either don't fit somewhere else, or,
*       cannot be declared in the normal place for other reasons.
*       [ANSI]
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_STDLIB
#define _INC_STDLIB

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

/* Define NULL pointer value */

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

/* Definition of the argument values for the exit() function */

#define EXIT_SUCCESS    0
#define EXIT_FAILURE    1


#ifndef _ONEXIT_T_DEFINED

typedef int (__cdecl * _onexit_t)(void);


#if     !__STDC__
/* Non-ANSI name for compatibility */
#define onexit_t _onexit_t
#endif

#define _ONEXIT_T_DEFINED
#endif


/* Data structure definitions for div and ldiv runtimes. */

#ifndef _DIV_T_DEFINED

typedef struct _div_t {
        int quot;
        int rem;
} div_t;

typedef struct _ldiv_t {
        long quot;
        long rem;
} ldiv_t;

#define _DIV_T_DEFINED
#endif

/*
 * structs used to fool the compiler into not generating floating point
 * instructions when copying and pushing [long] double values
 */

#ifndef _CRT_DOUBLE_DEC

#ifndef _LDSUPPORT

#pragma pack(4)
typedef struct {
    unsigned char ld[10];
} _LDOUBLE;
#pragma pack()

#define _PTR_LD(x) ((unsigned char  *)(&(x)->ld))

#else

/* push and pop long, which is #defined as __int64 by a spec2k test */
#pragma push_macro("long")
#undef long
typedef long double _LDOUBLE;
#pragma pop_macro("long")

#define _PTR_LD(x) ((unsigned char  *)(x))

#endif

typedef struct {
        double x;
} _CRT_DOUBLE;

typedef struct {
    float f;
} _CRT_FLOAT;

/* push and pop long, which is #defined as __int64 by a spec2k test */
#pragma push_macro("long")
#undef long

typedef struct {
        /*
         * Assume there is a long double type
         */
        long double x;
} _LONGDOUBLE;

#pragma pop_macro("long")

#pragma pack(4)
typedef struct {
    unsigned char ld12[12];
} _LDBL12;
#pragma pack()

#define _CRT_DOUBLE_DEC
#endif

/* Maximum value that can be returned by the rand function. */

#define RAND_MAX 0x7fff

/*
 * Maximum number of bytes in multi-byte character in the current locale
 * (also defined in ctype.h).
 */
#ifndef MB_CUR_MAX
#define MB_CUR_MAX ___mb_cur_max_func()
_CRTIMP extern int __mb_cur_max;
_CRTIMP int __cdecl ___mb_cur_max_func(void);
#endif  /* MB_CUR_MAX */

/* Minimum and maximum macros */

#define __max(a,b)  (((a) > (b)) ? (a) : (b))
#define __min(a,b)  (((a) < (b)) ? (a) : (b))

/*
 * Sizes for buffers used by the _makepath() and _splitpath() functions.
 * note that the sizes include space for 0-terminator
 */
#define _MAX_PATH   260 /* max. length of full pathname */
#define _MAX_DRIVE  3   /* max. length of drive component */
#define _MAX_DIR    256 /* max. length of path component */
#define _MAX_FNAME  256 /* max. length of file name component */
#define _MAX_EXT    256 /* max. length of extension component */

/*
 * Argument values for _set_error_mode().
 */
#define _OUT_TO_DEFAULT 0
#define _OUT_TO_STDERR  1
#define _OUT_TO_MSGBOX  2
#define _REPORT_ERRMODE 3

/*
 * Argument values for _set_abort_behavior().
 */
#define _WRITE_ABORT_MSG    0x1
#define _CALL_REPORTFAULT   0x2

/*
 * Sizes for buffers used by the getenv/putenv family of functions.
 */
#define _MAX_ENV 32767   

/*
 * Typedefs and argument values for _set_security_error_handler()
 */
#define _SECERR_BUFFER_OVERRUN 1        /* void* arg ignored */
typedef void (__cdecl * _secerr_handler_func)(int, void *);

/* a purecall handler procedure. Never returns normally */
typedef void (__cdecl *_purecall_handler)(void); 

/* establishes a purecall handler for the process */
_CRTIMP _purecall_handler __cdecl _set_purecall_handler(__in_opt _purecall_handler _P(_Handler));
_CRTIMP _purecall_handler __cdecl _get_purecall_handler();

#if defined(__cplusplus)
extern "C++"
{

}
#endif

/* a invalid_arg handler procedure. */
typedef void (__cdecl *_invalid_parameter_handler)(const wchar_t *, const wchar_t *, const wchar_t *, unsigned int, uintptr_t); 

/* establishes a invalid_arg handler for the process */
_CRTIMP _invalid_parameter_handler __cdecl _set_invalid_parameter_handler(__in_opt _invalid_parameter_handler _P(_Handler));
_CRTIMP _invalid_parameter_handler __cdecl _get_invalid_parameter_handler(void);

#if defined(__cplusplus)
extern "C++"
{

}
#endif

/* External variable declarations */

_CRTIMP int * __cdecl _errno(void);
_CRTIMP unsigned long * __cdecl __doserrno(void);
#define errno       (*_errno())
#define _doserrno   (*__doserrno())

errno_t __cdecl _set_doserrno(__in unsigned long _P(_Value));
errno_t __cdecl _get_doserrno(__out unsigned long * _P(_Value));
errno_t __cdecl _set_errno(__in int _P(_Value));
errno_t __cdecl _get_errno(__out int * _P(_Value));

/* you can't modify this, but it is non-const for backcompat */
_CRTIMP _CRT_INSECURE_DEPRECATE char ** __cdecl __sys_errlist(void);
#define _sys_errlist (__sys_errlist())

_CRTIMP _CRT_INSECURE_DEPRECATE int * __cdecl __sys_nerr(void);
#define _sys_nerr (*__sys_nerr())


_CRTIMP extern int __argc;          /* count of cmd line args */
_CRTIMP extern char ** __argv;      /* pointer to table of cmd line args */
_CRTIMP extern wchar_t ** __wargv;  /* pointer to table of wide cmd line args */

_CRTIMP extern char ** _environ;    /* pointer to environment table */
_CRTIMP extern wchar_t ** _wenviron;    /* pointer to wide environment table */

_CRT_INSECURE_DEPRECATE _CRTIMP extern char * _pgmptr;      /* points to the module (EXE) name */
_CRT_INSECURE_DEPRECATE _CRTIMP extern wchar_t * _wpgmptr;  /* points to the module (EXE) wide name */



errno_t __cdecl _get_pgmptr(__deref_out char ** _P(_Value));
errno_t __cdecl _get_wpgmptr(__deref_out wchar_t ** _P(_Value));


_CRT_INSECURE_DEPRECATE _CRTIMP extern int _fmode;          /* default file translation mode */

_CRTIMP errno_t __cdecl _set_fmode(__in int _P(_Mode));
_CRTIMP errno_t __cdecl _get_fmode(__out int * _P(_PMode));


/* _countof helper */
#if !defined(_countof)
#if !defined(__cplusplus)
#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#else
extern "C++"
{
template <typename _CountofType, size_t _SizeOfArray>
char (*__countof_helper(UNALIGNED _CountofType (&_Array)[_SizeOfArray]))[_SizeOfArray];
#define _countof(_Array) sizeof(*__countof_helper(_Array))
}
#endif
#endif

/* function prototypes */

#if     _MSC_VER >= 1200
_CRTIMP __declspec(noreturn) void   __cdecl exit(__in int _P(_Code));
#else
_CRTIMP void   __cdecl exit(__in int _P(_Code));
#endif

_CRTIMP void   __cdecl abort(void);
_CRTIMP unsigned int __cdecl _set_abort_behavior(__in unsigned int _P(_Flags), __in unsigned int _P(_Mask));

        __checkReturn int    __cdecl abs(__in int _P(_Num));
#if     _INTEGRAL_MAX_BITS >= 64
        __int64    __cdecl _abs64(__int64);
#endif
#if _MSC_VER >= 1400 && defined(_M_CEE)
        __checkReturn int    __clrcall _atexit_m_appdomain(__in_opt void (__clrcall * _P(_Func))(void));
        __checkReturn inline int __clrcall _atexit_m(__in_opt void (__clrcall *_Function)(void))
        {
            return _atexit_m_appdomain(_Function);
        }
#endif
        int    __cdecl atexit(void (__cdecl *)(void));
_CRTIMP __checkReturn double __cdecl atof(__in const char *_P(_Str));
_CRTIMP __checkReturn double __cdecl _atof_l(__in const char *_P(_Str), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int    __cdecl atoi(__in const char *_P(_Str));
_CRTIMP __checkReturn int    __cdecl _atoi_l(__in const char *_P(_Str), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn long   __cdecl atol(__in const char *_P(_Str));
_CRTIMP __checkReturn long   __cdecl _atol_l(__in const char *_P(_Str), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn void * __cdecl bsearch_s(__in const void * _P(_Key), __in_bcount(_NumOfElements*_SizeOfElements) const void * _P(_Base), size_t _P(_NumOfElements), size_t _P(__SizeOfElements), 
		__in int (__cdecl * _P(_PtFuncCompare))(void *, const void *, const void *), void * _P(_Context));
_CRTIMP __checkReturn void * __cdecl bsearch(__in const void * _P(_Key), __in_bcount(_NumOfElements*_SizeOfElements) const void * _P(_Base), size_t _P(_NumOfElements), size_t _P(_SizeOfElements),
        __in int (__cdecl *_P(_PtFuncCompare))(const void *, const void *) );
        __checkReturn unsigned short __cdecl _byteswap_ushort(__in unsigned short _P(_Short));
        __checkReturn unsigned long  __cdecl _byteswap_ulong (__in unsigned long _P(_Long));
#if     _INTEGRAL_MAX_BITS >= 64
        __checkReturn unsigned __int64 __cdecl _byteswap_uint64(unsigned __int64 _P(_Int64));
#endif
_CRTIMP _CRTNOALIAS _CRTRESTRICT __checkReturn __bcount_opt(_NumOfElements* _SizeOfElemnts) void * __cdecl calloc(__in size_t _P(_NumOfElements), __in size_t _P(_SizeOfElements));
_CRTIMP __checkReturn div_t  __cdecl div(__in int _P(_Numerator), __in int _P(_Denominator));
_CRTIMP _CRTNOALIAS void   __cdecl free(__inout_opt void * _P(_Memory));
#ifndef _XBSTRICT
_CRTIMP _CRT_INSECURE_DEPRECATE __checkReturn char * __cdecl getenv(__in const char * _P(_VarName));
_CRTIMP __checkReturn_opt errno_t __cdecl getenv_s(__out size_t * _P(_ReturnSize), __out_ecount(_DstSize) char * _P(_DstBuf), __in size_t _P(_DstSize), __in const char * _P(_VarName));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_1_1(errno_t, getenv_s, __out size_t *, __out_ecount(_Size) char, __in const char *)
#endif // XBSTRICT
_CRTIMP __checkReturn_opt errno_t __cdecl _dupenv_s(__deref_out_ecount_opt(*_PBufferSizeInBytes) char **_PBuffer, __out_opt size_t * _P(_PBufferSizeInBytes), __in const char * _P(_VarName));
_CRTIMP __checkReturn_opt errno_t __cdecl _itoa_s(__in int _P(_Value), __out_ecount(_Size) char * _P(_DstBuf), __in size_t _P(_Size), __in int _P(_Radix));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_1_1(errno_t, _itoa_s, __in int, __out_ecount(_Size) char, __in int)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_1_1(char *, __RETURN_POLICY_DST, _CRTIMP, _itoa, __in int, __out char, __in int)
#if     _INTEGRAL_MAX_BITS >= 64
_CRTIMP __checkReturn_opt errno_t __cdecl _i64toa_s(__in __int64 _P(_Val), __out_ecount(_Size) char * _P(_DstBuf), __in size_t _P(_Size), __in int _P(_Radix));
_CRTIMP _CRT_INSECURE_DEPRECATE char * __cdecl _i64toa(__in __int64 _P(_Val), __out char * _P(_DstBuf), __in int _P(_Radix));
_CRTIMP __checkReturn_opt errno_t __cdecl _ui64toa_s(__in unsigned __int64 _P(_Val), __out_ecount(_Size) char * _P(_DstBuf), __in size_t _P(_Size), __in int _P(_Radix));
_CRTIMP _CRT_INSECURE_DEPRECATE char * __cdecl _ui64toa(__in unsigned __int64 _P(_Val), __out char * _P(_DstBuf), __in int _P(_Radix));
_CRTIMP __checkReturn __int64 __cdecl _atoi64(__in const char * _P(_String));
_CRTIMP __checkReturn __int64 __cdecl _atoi64_l(__in const char * _P(_String), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn __int64 __cdecl _strtoi64(__in const char * _P(_String), __deref_opt_out char ** _P(_EndPtr), __in int _P(_Radix));
_CRTIMP __checkReturn __int64 __cdecl _strtoi64_l(__in const char * _P(_String), __deref_opt_out char ** _P(_EndPtr), __in int _P(_Radix), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn unsigned __int64 __cdecl _strtoui64(__in const char * _P(_String), __deref_opt_out char ** _P(_EndPtr), __in int _P(_Radix));
_CRTIMP __checkReturn unsigned __int64 __cdecl _strtoui64_l(__in const char * _P(_String), __deref_opt_out char ** _P(_EndPtr), __in int  _P(_Radix), __in_opt _locale_t _P(_PtLoci));
#endif
        __checkReturn long __cdecl labs(__in long _P(_Val));
_CRTIMP __checkReturn ldiv_t __cdecl ldiv(__in long _P(_Numerator), __in long _P(_Denominator));
#ifdef __cplusplus
extern "C++"
{
    inline ldiv_t  div(__in long _A1, __in long _A2)
    {
        return ldiv(_A1, _A2);
    }
}
#endif
_CRTIMP __checkReturn_opt errno_t __cdecl _ltoa_s(__in long _P(_Val), __out_ecount(_Size) char * _P(_DstBuf), __in size_t _P(_Size), __in int _P(_Radix));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_1_1(errno_t, _ltoa_s, __in long, __out_ecount(_Size) char, __in int)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_1_1(char *, __RETURN_POLICY_DST, _CRTIMP, _ltoa, __in long, __out char, __in int)
_CRTIMP _CRTNOALIAS _CRTRESTRICT __checkReturn __bcount_opt(_Size) void * __cdecl malloc(__in size_t _P(_Size));
_CRTIMP __checkReturn int    __cdecl mblen(__in_bcount_opt(_MaxCount) const char * _P(_Ch), __in size_t _P(_MaxCount));
_CRTIMP __checkReturn int    __cdecl _mblen_l(__in_bcount_opt(_MaxCount) const char * _P(_Ch), __in size_t _P(_MaxCount), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn size_t __cdecl _mbstrlen(__in const char * _P(_Str));
_CRTIMP __checkReturn size_t __cdecl _mbstrlen_l(__in const char *_P(_Str), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn size_t __cdecl _mbstrnlen(__in_bcount(_MaxCount) const char *_P(_Str), __in size_t _P(_MaxCount));
_CRTIMP __checkReturn size_t __cdecl _mbstrnlen_l(__in_bcount(_MaxCount) const char *_P(_Str), __in size_t _P(_MaxCount), __in_opt _locale_t _P(_PtLoci));
_CRTIMP int    __cdecl mbtowc(__out wchar_t * _P(_DstCh), __in_bcount_opt(_SrcSizeInBytes) const char * _P(_SrcCh), __in size_t _P(_SrcSizeInBytes));
_CRTIMP int    __cdecl _mbtowc_l(__out wchar_t * _P(_DstCh), __in_bcount_opt(_SrcSizeInBytes) const char * _P(_SrcCh), __in size_t _P(_SrcSizeInBytes), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn_opt errno_t __cdecl mbstowcs_s(__out_opt size_t * _P(_PtNumOfCharConverted), __out_ecount_part_opt(_SizeInWords, *_PtNumOfCharConverted) wchar_t * _P(_DstBuf), __in size_t _P(_SizeInWords), __in_ecount(_MaxCount) const char * _P(_SrcBuf), __in size_t _P(_MaxCount) );
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_1_2(errno_t, mbstowcs_s, __out_opt size_t *, __out_ecount(_Size) wchar_t, __in const char *, __in size_t)
__DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_2_SIZE(_CRTIMP, mbstowcs, __out_opt wchar_t, __in const char *, __in size_t)

_CRTIMP __checkReturn_opt errno_t __cdecl _mbstowcs_s_l(__out_opt size_t * _P(_PtNumOfCharConverted), __out_ecount_part_opt(_SizeInWords, *_PtNumOfCharConverted) wchar_t * _P(_DstBuf), __in size_t _P(_SizeInWords), __in_ecount(_MaxCount) const char * _P(_SrcBuf), __in size_t _P(_MaxCount), __in_opt _locale_t _P(_PtLoci));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_1_3(errno_t, _mbstowcs_s_l, __out_opt size_t *, __out_ecount(_Size) wchar_t, __in const char *, __in size_t, __in_opt _locale_t)
__DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_3_SIZE_EX(_CRTIMP, _mbstowcs_l, _mbstowcs_s_l, __out_opt wchar_t, __in const char *, __in size_t, __in_opt _locale_t)

_CRTIMP void __cdecl qsort_s(__inout_bcount(_NumOfElements*_SizeOfElements) void * _P(_Base), __in size_t _P(_NumOfElements), __in size_t _P(_SizeOfElements), 
        __in int (__cdecl * _P(_PtFuncCompare))(void *, const void *, const void *), void * _P(_Context));
_CRTIMP void   __cdecl qsort(__inout_bcount(_NumOfElements*_SizeOfElements) void * _P(_Base), __in size_t _P(_NumOfElements), __in size_t _P(_SizeOfElements), 
        __in int (__cdecl * _P(_PtFuncCompare))(const void *, const void *) );
_CRTIMP __checkReturn int    __cdecl rand(void);
_CRTIMP errno_t __cdecl rand_s ( __out unsigned int *_P(_RandomValue));

_CRTIMP _CRTNOALIAS _CRTRESTRICT __checkReturn __bcount_opt(_NewSize) void *  __cdecl realloc(__in_opt void * _P(_Memory), __in size_t _P(_NewSize));
_CRTIMP __checkReturn_opt int    __cdecl _set_error_mode(__in int _P(_Mode));

_CRTIMP void   __cdecl srand(__in unsigned int _P(_Seed));
_CRTIMP __checkReturn double __cdecl strtod(__in const char * _P(_Str), __deref_opt_out char ** _P(_EndPtr));
_CRTIMP __checkReturn double __cdecl _strtod_l(__in const char * _P(_Str), __deref_opt_out char ** _P(_EndPtr), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn long   __cdecl strtol(__in const char * _P(_Str), __deref_opt_out char ** _P(_EndPtr), __in int _P(_Radix) );
_CRTIMP __checkReturn long   __cdecl _strtol_l(__in const char *_P(_Str), __deref_opt_out char **_P(_EndPtr), __in int _P(_Radix), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn unsigned long __cdecl strtoul(__in const char * _P(_Str), __deref_opt_out char ** _P(_EndPtr), __in int _P(_Radix));
_CRTIMP __checkReturn unsigned long __cdecl _strtoul_l(const char * _P(_Str), __deref_opt_out char **_P(_EndPtr), __in int _P(_Radix), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int    __cdecl system(__in const char * _P(_Command) );
_CRTIMP __checkReturn_opt errno_t __cdecl _ultoa_s(__in unsigned long _P(_Val), __out_ecount(_Size) char * _P(_DstBuf), __in size_t _P(_Size), __in int _P(_Radix));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_1_1(errno_t, _ultoa_s, __in unsigned long, __out_ecount(_Size) char, __in int)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_1_1(char *, __RETURN_POLICY_DST, _CRTIMP, _ultoa, __in unsigned long, __out char, __in int)
_CRTIMP _CRT_INSECURE_DEPRECATE int    __cdecl wctomb(__out_bcount_opt(MB_CUR_MAX) char * _P(_MbCh), __in wchar_t _P(_WCh));
_CRTIMP _CRT_INSECURE_DEPRECATE int    __cdecl _wctomb_l(__out_opt char * _P(_MbCh), __in wchar_t _P(_WCh), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn_wat errno_t __cdecl wctomb_s(__out_opt int * _P(_SizeConverted), __out_bcount_part_opt(_SizeInBytes, *_SizeConverted) char * _P(_MbCh), __in size_t _P(_SizeInBytes), __in wchar_t _P(_WCh));
_CRTIMP __checkReturn_wat errno_t __cdecl _wctomb_s_l(__out_opt int * _P(_SizeConverted), __out_bcount_opt(_SizeInBytes) char * _P(_MbCh), __in size_t _P(_SizeInBytes), __in wchar_t _P(_WCh), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn_wat errno_t __cdecl wcstombs_s(__out_opt size_t * _P(_PtNumOfCharConverted), __out_bcount_part_opt(_DstSizeInBytes, *_PtNumOfCharConverted) char * _P(_Dst), __in size_t _P(_DstSizeInBytes), __in const wchar_t * _P(_Src), __in size_t _P(_MaxCountInBytes));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_1_2(errno_t, wcstombs_s, __out_opt size_t *, __out_bcount_opt(_Size) char, __in const wchar_t *, __in size_t)
__DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_2_SIZE(_CRTIMP, wcstombs, __out_opt char, __in const wchar_t *, __in size_t)
_CRTIMP __checkReturn_wat errno_t __cdecl _wcstombs_s_l(__out_opt size_t * _P(_PtNumOfCharConverted), __out_bcount_part_opt(_DstSizeInBytes, *_PtNumOfCharConverted) char * _P(_Dst), __in size_t _P(_DstSizeInBytes), __in const wchar_t * _P(_Src), __in size_t _P(_MaxCountInBytes), __in_opt _locale_t _P(_PtLoci));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_1_3(errno_t, _wcstombs_s_l, __out_opt size_t *, __out_ecount_opt(_Size) char, __in const wchar_t *, __in size_t, __in_opt _locale_t)
__DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_3_SIZE_EX(_CRTIMP, _wcstombs_l, _wcstombs_s_l, __out_opt char, __in const wchar_t *, __in size_t, __in_opt _locale_t)

#if _MSC_VER >= 1400 && defined(__cplusplus) && defined(_M_CEE)
/*
 * Managed search routines. Note __cplusplus, this is because we only support
 * managed C++.
 */
extern "C++"
{
__checkReturn void * __clrcall bsearch_s(__in const void * _P(_Key), __in_bcount(_NumOfElements*_SizeOfElements) const void * _P(_Base), __in size_t _P(_NumOfElements), __in size_t _P(_SizeOfElements), 
        __in int (__clrcall * _P(_PtFuncCompare))(void *, const void *, const void *), void * _P(_Context));
__checkReturn void * __clrcall bsearch  (__in const void * _P(_Key), __in_bcount(_NumOfElements*_SizeOfElements) const void * _P(_Base), __in size_t _P(_NumOfElements), __in size_t _P(_SizeOfElements),
        __in int (__clrcall * _P(_PtFuncCompare))(const void *, const void *));

void __clrcall qsort_s(__inout_bcount(_NumOfElements*_SizeOfElements) void * _P(_Base), __in size_t _P(_NumOfElements), __in size_t _P(_SizeOfElements), 
        __in int (__clrcall * _P(_PtFuncCompare))(void *, const void *, const void *), void * _P(_Context));
void __clrcall qsort  (__inout_bcount(_NumOfElements*_SizeOfElements) void * _P(_Base), __in size_t _P(_NumOfElements), __in size_t _P(_SizeOfElements), 
        __in int (__clrcall * _P(_PtFuncCompare))(const void *, const void *));
}
#endif

#ifndef _WSTDLIB_DEFINED

/* wide function prototypes, also declared in wchar.h  */

_CRTIMP __checkReturn_wat errno_t __cdecl _itow_s (__in int _P(_Val), __out_ecount(_SizeInWords) wchar_t * _P(_DstBuf), __in size_t _P(_SizeInWords), __in int _P(_Radix));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_1_1(errno_t, _itow_s, __in int, __out_ecount(_Size) wchar_t, __in int)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_1_1(wchar_t *, __RETURN_POLICY_DST, _CRTIMP, _itow, __in int, __out wchar_t, __in int)
_CRTIMP __checkReturn_wat errno_t __cdecl _ltow_s (__in long _P(_Val), __out_ecount(_SizeInWords) wchar_t * _P(_DstBuf), __in size_t _P(_SizeInWords), __in int _P(_Radix));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_1_1(errno_t, _ltow_s, __in long, __out_ecount(_Size) wchar_t, __in int)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_1_1(wchar_t *, __RETURN_POLICY_DST, _CRTIMP, _ltow, __in long, __out wchar_t, __in int)
_CRTIMP __checkReturn_wat errno_t __cdecl _ultow_s (__in unsigned long _P(_Val), __out_ecount(_SizeInWords) wchar_t * _P(_DstBuf), __in size_t _P(_SizeInWords), __in int _P(_Radix));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_1_1(errno_t, _ultow_s, __in unsigned long, __out_ecount(_Size) wchar_t, __in int)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_1_1(wchar_t *, __RETURN_POLICY_DST, _CRTIMP, _ultow, __in unsigned long, __out wchar_t, __in int)
_CRTIMP __checkReturn double __cdecl wcstod(__in const wchar_t * _P(_Str), __deref_opt_out wchar_t ** _P(_EndPtr));
_CRTIMP __checkReturn double __cdecl _wcstod_l(__in const wchar_t *_P(_Str), __deref_opt_out wchar_t ** _P(_EndPtr), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn long   __cdecl wcstol(__in const wchar_t *_P(_Str), __deref_opt_out wchar_t ** _P(_EndPtr), int _P(_Radix));
_CRTIMP __checkReturn long   __cdecl _wcstol_l(__in const wchar_t *_P(_Str), __deref_opt_out wchar_t **_P(_EndPtr), int _P(_Radix), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn unsigned long __cdecl wcstoul(__in const wchar_t *_P(_Str), __deref_opt_out wchar_t ** _P(_EndPtr), int _P(_Radix));
_CRTIMP __checkReturn unsigned long __cdecl _wcstoul_l(__in const wchar_t *_P(_Str), __deref_opt_out wchar_t **_P(_EndPtr), int _P(_Radix), __in_opt _locale_t _P(_PtLoci));
#ifndef _XBSTRICT
_CRTIMP _CRT_INSECURE_DEPRECATE __checkReturn wchar_t * __cdecl _wgetenv(__in const wchar_t * _P(_VarName));
_CRTIMP __checkReturn_wat errno_t __cdecl _wgetenv_s(__out size_t * _P(_ReturnSize), __out_ecount(_DstSizeInWords) wchar_t * _P(_DstBuf), __in size_t _P(_SizeInWords), __in const wchar_t * _P(_VarName));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_1_1(errno_t, _wgetenv_s, __out size_t *, __out_ecount(_Size) wchar_t, __in const wchar_t *)
#endif // XBSTRICT
_CRTIMP __checkReturn_wat errno_t __cdecl _wdupenv_s(__deref_out_ecount_opt(*_BufferSizeInWords) wchar_t **_P(_Buffer), __out_opt size_t *_P(_BufferSizeInWords), __in const wchar_t *_P(_VarName));
_CRTIMP __checkReturn int    __cdecl _wsystem(__in const wchar_t * _P(_Command));
_CRTIMP __checkReturn double __cdecl _wtof(__in const wchar_t *_P(_Str));
_CRTIMP __checkReturn double __cdecl _wtof_l(__in const wchar_t *_P(_Str), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl _wtoi(__in const wchar_t *_P(_Str));
_CRTIMP __checkReturn int __cdecl _wtoi_l(__in const wchar_t *_P(_Str), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn long __cdecl _wtol(__in const wchar_t *_P(_Str));
_CRTIMP __checkReturn long __cdecl _wtol_l(__in const wchar_t *_P(_Str), __in_opt _locale_t _P(_PtLoci));

#if     _INTEGRAL_MAX_BITS >= 64
_CRTIMP __checkReturn_wat errno_t __cdecl _i64tow_s(__in __int64 _P(_Val), __out_ecount(_SizeInWords) wchar_t * _P(_DstBuf), __in size_t _P(_SizeInWords), __in int _P(_Radix));
_CRTIMP _CRT_INSECURE_DEPRECATE wchar_t * __cdecl _i64tow(__in __int64 _P(_Val), __out wchar_t * _P(_DstBuf), __in int _P(_Radix));
_CRTIMP __checkReturn_wat errno_t __cdecl _ui64tow_s(__in unsigned __int64 _P(_Val), __out_ecount(_SizeInWords) wchar_t * _P(_DstBuf), __in size_t _P(_SizeInWords), __in int _P(_Radix));
_CRTIMP _CRT_INSECURE_DEPRECATE wchar_t * __cdecl _ui64tow(__in unsigned __int64 _P(_Val), __out wchar_t * _P(_DstBuf), __in int _P(_Radix));
_CRTIMP __checkReturn __int64   __cdecl _wtoi64(__in const wchar_t *_P(_Str));
_CRTIMP __checkReturn __int64   __cdecl _wtoi64_l(__in const wchar_t *_P(_Str), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn __int64   __cdecl _wcstoi64(__in const wchar_t * _P(_Str), __deref_opt_out wchar_t ** _P(_EndPtr), __in int _P(_Radix));
_CRTIMP __checkReturn __int64   __cdecl _wcstoi64_l(__in const wchar_t * _P(_Str), __deref_opt_out wchar_t ** _P(_EndPtr), __in int _P(_Radix), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn unsigned __int64  __cdecl _wcstoui64(__in const wchar_t * _P(_Str), __deref_opt_out wchar_t ** _P(_EndPtr), __in int _P(_Radix));
_CRTIMP __checkReturn unsigned __int64  __cdecl _wcstoui64_l(__in const wchar_t *_P(_Str) , __deref_opt_out wchar_t ** _P(_EndPtr), __in int _P(_Radix), __in_opt _locale_t _P(_PtLoci));
#endif

#define _WSTDLIB_DEFINED
#endif



/* 
Buffer size required to be passed to _gcvt, fcvt and other fp conversion routines
*/
#define _CVTBUFSIZE (309+40) /* # of digits in max. dp value + slop */

#if     _MSC_VER >= 1200
_CRTIMP __declspec(noreturn) void   __cdecl _exit(__in int _P(_Code));
#else
_CRTIMP void   __cdecl _exit(__in int _P(_Code));
#endif

#ifndef _XBSTRICT

#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
#pragma push_macro("_fullpath")
#undef _fullpath
#endif

_CRTIMP __checkReturn char * __cdecl _fullpath(__out_ecount_opt(_SizeInBytes) char * _P(_FullPath), __in const char * _P(_Path), __in size_t _P(_SizeInBytes));

#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
#pragma pop_macro("_fullpath")
#endif

#endif // XBSTRICT

_CRTIMP __checkReturn_wat errno_t __cdecl _ecvt_s(__out_ecount(_Size) char * _P(_DstBuf), __in size_t _P(_Size), __in double _P(_Val), __in int _P(_NumOfDights), __out int * _P(_PtDec), __out int * _P(_PtSign));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_4(errno_t, _ecvt_s, __out_ecount(_Size) char, __in double, __in int, __out int *, __out int *)
_CRTIMP _CRT_INSECURE_DEPRECATE __checkReturn char * __cdecl _ecvt(__in double _P(_Val), __in int _P(_NumOfDigits), __out int * _P(_PtDec), __out int * _P(_PtSign));
_CRTIMP __checkReturn_wat errno_t __cdecl _fcvt_s(__out_ecount(_Size) char * _P(_DstBuf), __in size_t _P(_Size), __in double _P(_Val), __in int _P(_NumOfDec), __out int * _P(_PtDec), __out int * _P(_PtSign));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_4(errno_t, _fcvt_s, __out_ecount(_Size) char, __in double, __in int, __out int *, __out int *)
_CRTIMP _CRT_INSECURE_DEPRECATE __checkReturn char * __cdecl _fcvt(__in double _P(_Val), __in int _P(_NumOfDec), __out int * _P(_PtDec), __out int * _P(_PtSign));
_CRTIMP errno_t __cdecl _gcvt_s(__out_ecount(_Size) char * _P(_DstBuf), __in size_t _P(_Size), __in double _P(_Val), __in int _P(_NumOfDigits));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_2(errno_t, _gcvt_s, __out_ecount(_Size) char, __in double, __in int)
_CRTIMP _CRT_INSECURE_DEPRECATE char * __cdecl _gcvt(__in double _P(_Val), __in int _P(_NumOfDigits), __out char * _P(_DstBuf));

_CRTIMP __checkReturn int __cdecl _atodbl(__out _CRT_DOUBLE * _P(_Result), __in char * _P(_Str));
_CRTIMP __checkReturn int __cdecl _atoldbl(__out _LDOUBLE * _P(_Result), __in char * _P(_Str));
_CRTIMP __checkReturn int __cdecl _atoflt(__out _CRT_FLOAT * _P(_Result), __in char * _P(_Str));
_CRTIMP __checkReturn int __cdecl _atodbl_l(__out _CRT_DOUBLE * _P(_Result), __in char * _P(_Str), __in_opt _locale_t _P(_PtLoci));
_CRTIMP int __cdecl _atoldbl_l(__out _LDOUBLE * _P(_Result), __in char * _P(_Str), __in_opt _locale_t _P(_PtLoci));
_CRTIMP int __cdecl _atoflt_l(__out _CRT_FLOAT * _P(_Result), __in char * _P(_Str), __in_opt _locale_t _P(_PtLoci));
        __checkReturn unsigned long __cdecl _lrotl(__in unsigned long _P(_Val), __in int _P(_Shift));
        __checkReturn unsigned long __cdecl _lrotr(__in unsigned long _P(_Val), __in int _P(_Shift));
_CRTIMP_ALTERNATIVE __checkReturn_wat errno_t   __cdecl _makepath_s(__out_ecount(_Size) char * _P(_PathResult), __in_opt size_t _P(_Size), __in_opt const char * _P(_Drive), __in_opt const char * _P(_Dir), __in_opt const char * _P(_Filename),
        __in_opt const char * _P(_Ext));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_4(errno_t, _makepath_s, __out_ecount(_Size) char, __in_opt const char *, __in_opt const char *, __in_opt const char *, __in_opt const char *)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_4(void, __RETURN_POLICY_VOID, _CRTIMP, _makepath, __out char, __in_opt const char *, __in_opt const char *, __in_opt const char *, __in_opt const char *)

#if _MSC_VER >= 1400 && defined(_M_CEE)
        _onexit_m_t    __clrcall _onexit_m(_onexit_m_t _Function);
        _onexit_m_t    __clrcall _onexit_m_appdomain(_onexit_m_t _Function);
#endif
        _onexit_t __cdecl _onexit(__in_opt _onexit_t _P(_Func));

_CRTIMP void   __cdecl perror(__in const char * _P(_ErrMsg));
#ifndef _XBSTRICT
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn int    __cdecl _putenv(__in const char * _P(_EnvString));
_CRTIMP __checkReturn_wat errno_t __cdecl _putenv_s(__in const char * _P(_Name), __in const char * _P(_Value));
#endif // XBSTRICT
        __checkReturn unsigned int __cdecl _rotl(__in unsigned int _P(_Val), __in int _P(_Shift));
#if     _INTEGRAL_MAX_BITS >= 64
        __checkReturn unsigned __int64 __cdecl _rotl64(__in unsigned __int64 _P(_Val), __in int _P(_Shift));
#endif
        __checkReturn unsigned int __cdecl _rotr(__in unsigned int _P(_Val), __in int _P(_Shift));
#if     _INTEGRAL_MAX_BITS >= 64
        __checkReturn unsigned __int64 __cdecl _rotr64(__in unsigned __int64 _P(_Val), __in int _P(_Shift));
#endif
_CRTIMP errno_t __cdecl _searchenv_s(__in const char * _P(_Filename), __in const char * _P(_EnvVar), __out_ecount(_SizeInBytes) char * _P(_ResultPath), __in size_t _P(_SizeInBytes));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_2_0(errno_t, _searchenv_s, __in const char *, __in const char *, __out_ecount(_Size) char)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_2_0(void, __RETURN_POLICY_VOID, _CRTIMP, _searchenv, __in const char *, __in const char *, __out char)

_CRT_INSECURE_DEPRECATE _CRTIMP void   __cdecl _splitpath(__in const char * _P(_FullPath), __out_opt char * _P(_Drive), __out_opt char * _P(_Dir), __out_opt char * _P(_Filename), __out_opt char * _P(_Ext));
_CRTIMP_ALTERNATIVE __checkReturn_wat errno_t  __cdecl _splitpath_s(__in const char * _P(_FullPath), 
		__out_ecount_opt(_DriveSize) char * _P(_Drive), __in size_t _P(_DriveSize), 
		__out_ecount_opt(_DirSize) char * _P(_Dir), __in size_t _P(_DirSize), 
		__out_ecount_opt(_FilenameSize) char * _P(_Filename), __in size_t _P(_FilenameSize), 
		__out_ecount_opt(_ExtSize) char * _P(_Ext), __in size_t _P(_ExtSize));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_SPLITPATH(errno_t, _splitpath_s,  char)
/* TODO (alecont) Add the standard overload */

_CRTIMP void   __cdecl _swab(__inout_ecount_full(_SizeInBytes) char * _P(_Buf1), __inout_ecount_full(_SizeInBytes) char * _P(_Buf2), int _P(_SizeInBytes));

#ifndef _WSTDLIBP_DEFINED

/* wide function prototypes, also declared in wchar.h  */

#ifndef _XBSTRICT

#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
#pragma push_macro("_wfullpath")
#undef _wfullpath
#endif

_CRTIMP __checkReturn wchar_t * __cdecl _wfullpath(__out_ecount_opt(_SizeInWords) wchar_t * _P(_FullPath), __in const wchar_t * _P(_Path), __in size_t _P(_SizeInWords));

#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
#pragma pop_macro("_wfullpath")
#endif

#endif // XBSTRICT

_CRTIMP_ALTERNATIVE __checkReturn_wat errno_t __cdecl _wmakepath_s(__out_ecount(_SizeInWords) wchar_t * _P(_PathResult), __in_opt size_t _P(_SizeInWords), __in_opt const wchar_t * _P(_Drive), __in_opt const wchar_t * _P(_Dir), __in_opt const wchar_t * _P(_Filename),
        __in_opt const wchar_t * _P(_Ext));        
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_4(errno_t, _wmakepath_s, __out_ecount(_Size) wchar_t, __in_opt const wchar_t *, __in_opt const wchar_t *, __in_opt const wchar_t *, __in_opt const wchar_t *)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_4(void, __RETURN_POLICY_VOID, _CRTIMP, _wmakepath, __out wchar_t, __in_opt const wchar_t *, __in_opt const wchar_t *, __in_opt const wchar_t *, __in_opt const wchar_t *)
_CRTIMP void   __cdecl _wperror(__in const wchar_t * _P(_ErrMsg));
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn int    __cdecl _wputenv(__in const wchar_t * _P(_EnvString));
_CRTIMP __checkReturn_wat errno_t __cdecl _wputenv_s(__in const wchar_t * _P(_Name), __in const wchar_t * _P(_Value));
_CRTIMP errno_t __cdecl _wsearchenv_s(__in const wchar_t * _P(_Filename), __in const wchar_t * _P(_Dir), __out_ecount(_SizeInWords) wchar_t * _P(_ResultPath), __in size_t _P(_SizeInWords));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_2_0(errno_t, _wsearchenv_s, __in const wchar_t *, __in const wchar_t *, __out_ecount(_Size) wchar_t)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_2_0(void, __RETURN_POLICY_VOID, _CRTIMP, _wsearchenv, __in const wchar_t *, __in const wchar_t *, __out wchar_t)
_CRT_INSECURE_DEPRECATE _CRTIMP void   __cdecl _wsplitpath(__in const wchar_t * _P(_FullPath), __out_opt wchar_t * _P(_Drive), __out_opt wchar_t * _P(_Dir), __out_opt wchar_t * _P(_Filename), __out_opt wchar_t * _P(_Ext));
_CRTIMP_ALTERNATIVE errno_t __cdecl _wsplitpath_s(__in const wchar_t * _P(_FullPath), 
		__out_ecount_opt(_DriveSizeInWords) wchar_t * _P(_Drive), __in size_t _P(_DriveSizeInWords), 
		__out_ecount_opt(_DirSizeInWords) wchar_t * _P(_Dir), __in size_t _P(_DirSizeInWords), 
		__out_ecount_opt(_FilenameSizeInWords) wchar_t * _P(_Filename), __in size_t _P(_FilenameSizeInWords), 
		__out_ecount_opt(_ExtSizeInWords) wchar_t * _P(_Ext), __in size_t _P(_ExtSizeInWords));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_SPLITPATH(errno_t, _wsplitpath_s, wchar_t)

#define _WSTDLIBP_DEFINED
#endif

#ifndef _CRT_SETERRORMODE_BEEP_SLEEP_NO_DEPRECATE
#define _CRT_SETERRORMODE_BEEP_SLEEP_DEPRECATE __declspec(deprecated)
#else
#define _CRT_SETERRORMODE_BEEP_SLEEP_DEPRECATE
#endif

/* The Win32 API SetErrorMode, Beep and Sleep should be used instead. */
_CRT_SETERRORMODE_BEEP_SLEEP_DEPRECATE _CRTIMP void __cdecl _sleep(__in unsigned long _P(_Duration));



#if     !__STDC__
/* --------- The declarations below should not be in stdlib.h --------- */
/* --------- and will be removed in a future release. Include --------- */
/* --------- ctype.h to obtain these declarations.            --------- */
#ifndef tolower     /* tolower has been undefined - use function */
_CRTIMP __checkReturn int __cdecl tolower(__in int _P(_Ch));
_CRTIMP __checkReturn int __cdecl _tolower_l(__in int _P(_Ch), __in_opt _locale_t _P(_PtLoci));
#endif  /* tolower */
#ifndef toupper     /* toupper has been undefined - use function */
_CRTIMP __checkReturn int __cdecl toupper(__in int _P(_Ch));
_CRTIMP __checkReturn int __cdecl _toupper_l(int _P(_Ch), __in_opt _locale_t _P(_PtLoci));
#endif  /* toupper */
/* --------- The declarations above will be removed.          --------- */
#endif

#if     !__STDC__


/* Non-ANSI names for compatibility */

#ifndef __cplusplus
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

#define sys_errlist _sys_errlist
#define sys_nerr    _sys_nerr
#define environ     _environ

_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn char * __cdecl ecvt(__in double _P(_Val), __in int _P(_NumOfDigits), __out int * _P(_PtDec), __out int * _P(_PtSign));
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn char * __cdecl fcvt(__in double _P(_Val), __in int _P(_NumOfDec), __out int * _P(_PtDec), __out int * _P(_PtSign));
_CRT_NONSTDC_DEPRECATE _CRTIMP char * __cdecl gcvt(__in double _P(_Val), __in int _P(_NumOfDigits), __out char * _P(_DstBuf));
#pragma warning(push)
#pragma warning(disable: 4141) /* Using deprecated twice */ 
_CRT_NONSTDC_DEPRECATE _CRTIMP _CRT_INSECURE_DEPRECATE char * __cdecl itoa(__in int _P(_Val), __out char * _P(_DstBuf), __in int _P(_Radix));
_CRT_NONSTDC_DEPRECATE _CRTIMP _CRT_INSECURE_DEPRECATE char * __cdecl ltoa(__in long _P(_Val), __out char * _P(_DstBuf), __in int _P(_Radix));
        onexit_t __cdecl onexit(__in_opt onexit_t _P(_Func));
#ifndef _XBSTRICT
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int    __cdecl putenv(__in const char * _P(_EnvString));
#endif // XBSTRICT
_CRT_NONSTDC_DEPRECATE _CRTIMP void   __cdecl swab(__inout_bcount(_SizeInBytes) char * _P(_Buf1),__inout_bcount(_SizeInBytes) char * _P(_Buf2), __in int _P(_SizeInBytes));
_CRT_NONSTDC_DEPRECATE _CRTIMP _CRT_INSECURE_DEPRECATE char * __cdecl ultoa(__in unsigned long _P(_Val), __out char * _P(_Dstbuf), __in int _P(_Radix));
#pragma warning(pop)


#endif  /* __STDC__ */

#ifdef  __cplusplus
}

#endif

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif  /* _INC_STDLIB */
