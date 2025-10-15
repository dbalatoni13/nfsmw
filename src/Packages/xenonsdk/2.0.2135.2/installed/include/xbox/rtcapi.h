/***
*rtcapi.h - declarations and definitions for RTC use
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Contains the declarations and definitions for all RunTime Check
*       support.
*
****/

#ifndef _INC_RTCAPI
#define _INC_RTCAPI


#include <crtdefs.h>

#ifdef  _MSC_VER
#pragma pack(push,_CRT_PACKING)
#endif  /* _MSC_VER */

/*
Previous versions of this header included declarations of functions used by RTC but
not intended for use by end-users. These functions are now declared deprecated by default
and may be removed in a future version.
*/
#ifndef _CRT_ENABLE_RTC_INTERNALS
#define _RTCINTERNAL_DEPRECATED __declspec(deprecated) 
#else
#define _RTCINTERNAL_DEPRECATED 
#endif



#ifdef  __cplusplus

extern "C" {

#endif

    /* General User API */

typedef enum _RTC_ErrorNumber {
    _RTC_CHKSTK = 0,
    _RTC_CVRT_LOSS_INFO,
    _RTC_CORRUPT_STACK,
    _RTC_UNINIT_LOCAL_USE,
    _RTC_CORRUPTED_ALLOCA,
    _RTC_ILLEGAL 
} _RTC_ErrorNumber;
 
#   define _RTC_ERRTYPE_IGNORE -1
#   define _RTC_ERRTYPE_ASK    -2

#ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif

    typedef int (__cdecl *_RTC_error_fn)(int, const char *, int, const char *, const char *, ...);
    typedef int (__cdecl *_RTC_error_fnW)(int, const wchar_t *, int, const wchar_t *, const wchar_t *, ...);

    /* User API */
    int           __cdecl _RTC_NumErrors(void);
    const char *  __cdecl _RTC_GetErrDesc(_RTC_ErrorNumber errnum);
    int           __cdecl _RTC_SetErrorType(_RTC_ErrorNumber errnum, int ErrType);
    _RTC_error_fn __cdecl _RTC_SetErrorFunc(_RTC_error_fn);
    _RTC_error_fnW __cdecl _RTC_SetErrorFuncW(_RTC_error_fnW);

    /* Power User/library API */


    /* Init functions */

    /* These functions all call _CRT_RTC_INIT */
    int __cdecl _RTC_Initialize(void);
    void __cdecl _RTC_Terminate(void);

    /*
     * If you're not using the CRT, you have to implement _CRT_RTC_INIT
     * Just return either null, or your error reporting function
     * *** Don't mess with res0/res1/res2/res3/res4 - YOU'VE BEEN WARNED! ***
     */
    _RTC_error_fn _CRT_RTC_INIT(void *res0, void **res1, int res2, int res3, int res4);
    _RTC_error_fnW _CRT_RTC_INITW(void *res0, void **res1, int res2, int res3, int res4);
    
    /* Compiler generated calls (unlikely to be used, even by power users) */
    /* Types */
    typedef struct _RTC_vardesc {
        int addr;
        int size;
        char *name;
    } _RTC_vardesc;

    typedef struct _RTC_framedesc {
        int varCount;
        _RTC_vardesc *variables;
    } _RTC_framedesc;

#pragma pack(push,1)
    /*  Structure padded under 32-bit x86, to get consistent
        execution between 32/64 targets.
    */
    typedef struct _RTC_ALLOCA_NODE {
        __int32 guard1;
        struct _RTC_ALLOCA_NODE *next;
#if !defined(_WIN64)
        __int32 dummypad;
#endif
        size_t allocaSize;
        __int32 dummypad2;
        __int32 guard2[3];
    } _RTC_ALLOCA_NODE;
#pragma pack(pop)


    /* These unsupported functions are deprecated in native mode and not supported at all in /clr mode */

    /* Shortening convert checks - name indicates src bytes to target bytes */
    /* Signedness is NOT checked */
    _RTCINTERNAL_DEPRECATED char   __fastcall _RTC_Check_2_to_1(short src);
    _RTCINTERNAL_DEPRECATED char   __fastcall _RTC_Check_4_to_1(int src);
    _RTCINTERNAL_DEPRECATED char   __fastcall _RTC_Check_8_to_1(__int64 src);
    _RTCINTERNAL_DEPRECATED short  __fastcall _RTC_Check_4_to_2(int src);
    _RTCINTERNAL_DEPRECATED short  __fastcall _RTC_Check_8_to_2(__int64 src);
    _RTCINTERNAL_DEPRECATED int    __fastcall _RTC_Check_8_to_4(__int64 src);

    
    /* Stack Checking Calls */

    /* These unsupported functions are deprecated in native mode and not supported at all in /clr mode */

    _RTCINTERNAL_DEPRECATED  void   __fastcall _RTC_CheckStackVars(void *esp, _RTC_framedesc *fd);
    _RTCINTERNAL_DEPRECATED  void   __fastcall _RTC_CheckStackVars2(void *esp, _RTC_framedesc *fd, _RTC_ALLOCA_NODE *allocaList);
    _RTCINTERNAL_DEPRECATED  void   __fastcall _RTC_AllocaHelper(_RTC_ALLOCA_NODE *pAllocaBase, size_t cbSize, _RTC_ALLOCA_NODE **pAllocaInfoList);
    /* Unintialized Local call */
    void   __cdecl     _RTC_UninitUse(const char *varname);

    /* These unsupported functions are deprecated in native mode and not supported at all in /clr mode */


    /* Subsystem initialization stuff */
    void    __cdecl    _RTC_Shutdown(void);
    void    __cdecl    _RTC_InitBase(void);
    

#ifdef  __cplusplus

    void* _ReturnAddress();
}

#endif

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */


#endif /* _INC_RTCAPI */
