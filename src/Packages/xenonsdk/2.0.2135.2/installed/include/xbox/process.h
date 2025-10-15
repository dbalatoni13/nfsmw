/***
*process.h - definition and declarations for process control functions
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file defines the modeflag values for spawnxx calls.
*       Also contains the function argument declarations for all
*       process control related routines.
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_PROCESS
#define _INC_PROCESS

#include <crtdefs.h>


#ifdef __cplusplus
extern "C" {
#endif

/* modeflag values for _spawnxx routines */

#define _P_WAIT         0
#define _P_NOWAIT       1
#define _OLD_P_OVERLAY  2
#define _P_NOWAITO      3
#define _P_DETACH       4

#define _P_OVERLAY      2

/* Action codes for _cwait(). The action code argument to _cwait is ignored
   on Win32 though it is accepted for compatibilty with old MS CRT libs */
#define _WAIT_CHILD      0
#define _WAIT_GRANDCHILD 1


/* function prototypes */

_CRTIMP uintptr_t __cdecl _beginthread (__in void (__cdecl * _P(_StartAddress)) (void *),
        __in unsigned _P(_StackSize), __in_opt void * _P(_ArgList));
_CRTIMP void __cdecl _endthread(void);
_CRTIMP uintptr_t __cdecl _beginthreadex(__in_opt void * _P(_Security), __in unsigned _P(_StackSize),
        __in unsigned (__stdcall * _P(_StartAddress)) (void *), __in_opt void * _P(_ArgList), 
        __in unsigned _P(_InitFlag), __in_opt unsigned * _P(_ThrdAddr));
_CRTIMP void __cdecl _endthreadex(__in unsigned _P(_Retval));



#if     _MSC_VER >= 1200
_CRTIMP __declspec(noreturn) void __cdecl exit(__in int _P(_Code));
_CRTIMP __declspec(noreturn) void __cdecl _exit(__in int _P(_Code));
#else
_CRTIMP void __cdecl exit(__in int _P(_Code));
_CRTIMP void __cdecl _exit(__in int _P(_Code));
#endif
_CRTIMP void __cdecl abort(void);
_CRTIMP void __cdecl _cexit(void);
_CRTIMP void __cdecl _c_exit(void);
#ifndef _XBSTRICT
_CRTIMP int __cdecl _getpid(void);

_CRTIMP intptr_t __cdecl _cwait(__out_opt int * _P(_TermStat), __in intptr_t _P(_ProcHandle), __in int _P(_Action));
_CRTIMP intptr_t __cdecl _execl(__in const char * _P(_Filename), __in const char * _P(_ArgList), ...);
_CRTIMP intptr_t __cdecl _execle(__in const char * _P(_Filename), __in const char * _P(_ArgList), ...);
_CRTIMP intptr_t __cdecl _execlp(__in const char * _P(_Filename), __in const char * _P(_ArgList), ...);
_CRTIMP intptr_t __cdecl _execlpe(__in const char * _P(_Filename), __in const char * _P(_ArgList), ...);
_CRTIMP intptr_t __cdecl _execv(__in const char * _P(_Filename), __in const char * const * _P(_ArgList));
_CRTIMP intptr_t __cdecl _execve(__in const char * _P(_Filename), __in const char * const * _P(_ArgList), __in_opt const char * const * _P(_Env));
_CRTIMP intptr_t __cdecl _execvp(__in const char * _P(_Filename), __in const char * const * _P(_ArgList));
_CRTIMP intptr_t __cdecl _execvpe(__in const char * _P(_Filename), __in const char * const * _P(_ArgList), __in_opt const char * const * _P(_Env));
_CRTIMP intptr_t __cdecl _spawnl(__in int _P(_Mode), __in const char * _P(_Filename), __in const char * _P(_ArgList), ...);
_CRTIMP intptr_t __cdecl _spawnle(__in int _P(_Mode), __in const char * _P(_Filename), __in const char * _P(_ArgList), ...);
_CRTIMP intptr_t __cdecl _spawnlp(__in int _P(_Mode), __in const char * _P(_Filename), __in const char * _P(_ArgList), ...);
_CRTIMP intptr_t __cdecl _spawnlpe(__in int _P(_Mode), __in const char * _P(_Filename), __in const char * _P(_ArgList), ...);
_CRTIMP intptr_t __cdecl _spawnv(__in int _P(_Mode), __in const char * _P(_Filename), __in const char * const * _P(_ArgList));
_CRTIMP intptr_t __cdecl _spawnve(__in int _P(_Mode), __in const char * _P(_Filename), __in const char * const * _P(_ArgList),
        __in_opt const char * const * _P(_Env));
_CRTIMP intptr_t __cdecl _spawnvp(__in int _P(_Mode), __in const char * _P(_Filename), __in const char * const * _P(_ArgList));
_CRTIMP intptr_t __cdecl _spawnvpe(__in int _P(_Mode), __in const char * _P(_Filename), __in const char * const * _P(_ArgList),
        __in_opt const char * const * _P(_Env));
_CRTIMP int __cdecl system(__in_opt const char * _P(_Command));

#ifndef _WPROCESS_DEFINED
/* wide function prototypes, also declared in wchar.h  */
_CRTIMP intptr_t __cdecl _wexecl(__in const wchar_t * _P(_Filename), __in const wchar_t * _P(_ArgList), ...);
_CRTIMP intptr_t __cdecl _wexecle(__in const wchar_t * _P(_Filename), __in const wchar_t * _P(_ArgList), ...);
_CRTIMP intptr_t __cdecl _wexeclp(__in const wchar_t * _P(_Filename), __in const wchar_t * _P(_ArgList), ...);
_CRTIMP intptr_t __cdecl _wexeclpe(__in const wchar_t * _P(_Filename), __in const wchar_t * _P(_ArgList), ...);
_CRTIMP intptr_t __cdecl _wexecv(__in const wchar_t * _P(_Filename), __in const wchar_t * const * _P(_ArgList));
_CRTIMP intptr_t __cdecl _wexecve(__in const wchar_t * _P(_Filename), __in const wchar_t * const * _P(_ArgList),
        __in_opt const wchar_t * const * _P(_Env));
_CRTIMP intptr_t __cdecl _wexecvp(__in const wchar_t * _P(_Filename), __in const wchar_t * const * _P(_ArgList));
_CRTIMP intptr_t __cdecl _wexecvpe(__in const wchar_t * _P(_Filename), __in const wchar_t * const * _P(_ArgList), 
        __in_opt const wchar_t * const * _P(_Env));
_CRTIMP intptr_t __cdecl _wspawnl(__in int _P(_Mode), __in const wchar_t * _P(_Filename), __in const wchar_t * _P(_ArgList), ...);
_CRTIMP intptr_t __cdecl _wspawnle(__in int _P(_Mode), __in const wchar_t * _P(_Filename), __in const wchar_t * _P(_ArgList), ...);
_CRTIMP intptr_t __cdecl _wspawnlp(__in int _P(_Mode), __in const wchar_t * _P(_Filename), __in const wchar_t * _P(_ArgList), ...);
_CRTIMP intptr_t __cdecl _wspawnlpe(__in int _P(_Mode), __in const wchar_t * _P(_Filename), __in const wchar_t * _P(_ArgList), ...);
_CRTIMP intptr_t __cdecl _wspawnv(__in int _P(_Mode), __in const wchar_t * _P(_Filename), __in const wchar_t * const * _P(_ArgList));
_CRTIMP intptr_t __cdecl _wspawnve(__in int _P(_Mode), __in const wchar_t * _P(_Filename), __in const wchar_t * const * _P(_ArgList),
        __in_opt const wchar_t * const * _P(_Env));
_CRTIMP intptr_t __cdecl _wspawnvp(__in int _P(_Mode), __in const wchar_t * _P(_Filename), __in const wchar_t * const * _P(_ArgList));
_CRTIMP intptr_t __cdecl _wspawnvpe(__in int _P(_Mode), __in const wchar_t * _P(_Filename), __in const wchar_t * const * _P(_ArgList),
        __in_opt const wchar_t * const * _P(_Env));
_CRTIMP int __cdecl _wsystem(__in_opt const wchar_t * _P(_Command));

#define _WPROCESS_DEFINED
#endif
#endif // XBSTRICT



#ifdef  _DECL_DLLMAIN
/*
 * Declare DLL notification (initialization/termination) routines
 *      The preferred method is for the user to provide DllMain() which will
 *      be called automatically by the DLL entry point defined by the C run-
 *      time library code.  If the user wants to define the DLL entry point
 *      routine, the user's entry point must call _CRT_INIT on all types of
 *      notifications, as the very first thing on attach notifications and
 *      as the very last thing on detach notifications.
 */
#ifdef  _WINDOWS_       /* Use types from WINDOWS.H */
BOOL WINAPI DllMain(__in HANDLE _P(_HDllHandle), __in DWORD _P(_Reason), __in_opt LPVOID _P(_Reserved));
BOOL WINAPI _CRT_INIT(__in HANDLE _P(_HDllHandle), __in DWORD _P(_Reason), __in_opt LPVOID _P(_Reserved));
BOOL WINAPI _wCRT_INIT(__in HANDLE _P(_HDllHandle), __in DWORD _P(_Reason), __in_opt LPVOID _P(_Reserved));
extern BOOL (WINAPI *_pRawDllMain)(HANDLE, DWORD, LPVOID);
#else
int __stdcall DllMain(__in void * _P(_HDllHandle), __in unsigned _P(_Reason), __in_opt void * _P(_Reserved));
int __stdcall _CRT_INIT(__in void * _P(_HDllHandle), __in unsigned _P(_Reason), __in_opt void * _P(_Reserved));
int __stdcall _wCRT_INIT(__in void * _P(_HDllHandle), __in unsigned _P(_Reason), __in_opt void * _P(_Reserved));
extern int (__stdcall *_pRawDllMain)(void *, unsigned, void *);
#endif  /* _WINDOWS_ */
#endif

#if     !__STDC__

/* Non-ANSI names for compatibility */

#define P_WAIT          _P_WAIT
#define P_NOWAIT        _P_NOWAIT
#define P_OVERLAY       _P_OVERLAY
#define OLD_P_OVERLAY   _OLD_P_OVERLAY
#define P_NOWAITO       _P_NOWAITO
#define P_DETACH        _P_DETACH
#define WAIT_CHILD      _WAIT_CHILD
#define WAIT_GRANDCHILD _WAIT_GRANDCHILD

/* current declarations */
_CRT_NONSTDC_DEPRECATE _CRTIMP intptr_t __cdecl cwait(__out_opt int * _P(_TermStat), __in intptr_t _P(_ProcHandle), __in int _P(_Action));
_CRT_NONSTDC_DEPRECATE _CRTIMP intptr_t __cdecl execl(__in const char * _P(_Filename), __in const char * _P(_ArgList), ...);
_CRT_NONSTDC_DEPRECATE _CRTIMP intptr_t __cdecl execle(__in const char * _P(_Filename), __in const char * _P(_ArgList), ...);
_CRT_NONSTDC_DEPRECATE _CRTIMP intptr_t __cdecl execlp(__in const char * _P(_Filename), __in const char * _P(_ArgList), ...);
_CRT_NONSTDC_DEPRECATE _CRTIMP intptr_t __cdecl execlpe(__in const char * _P(_Filename), __in const char * _P(_ArgList), ...);
_CRT_NONSTDC_DEPRECATE _CRTIMP intptr_t __cdecl execv(__in const char * _P(_Filename), __in const char * const * _P(_ArgList));
_CRT_NONSTDC_DEPRECATE _CRTIMP intptr_t __cdecl execve(__in const char * _P(_Filename), __in const char * const * _P(_ArgList), __in_opt const char * const * _P(_Env));
_CRT_NONSTDC_DEPRECATE _CRTIMP intptr_t __cdecl execvp(__in const char * _P(_Filename), __in const char * const * _P(_ArgList));
_CRT_NONSTDC_DEPRECATE _CRTIMP intptr_t __cdecl execvpe(__in const char * _P(_Filename), __in const char * const * _P(_ArgList), __in_opt const char * const * _P(_Env));
_CRT_NONSTDC_DEPRECATE _CRTIMP intptr_t __cdecl spawnl(__in int, __in const char * _P(_Filename), __in const char * _P(_ArgList), ...);
_CRT_NONSTDC_DEPRECATE _CRTIMP intptr_t __cdecl spawnle(__in int, __in const char * _P(_Filename), __in const char * _P(_ArgList), ...);
_CRT_NONSTDC_DEPRECATE _CRTIMP intptr_t __cdecl spawnlp(__in int, __in const char * _P(_Filename), __in const char * _P(_ArgList), ...);
_CRT_NONSTDC_DEPRECATE _CRTIMP intptr_t __cdecl spawnlpe(__in int, __in const char * _P(_Filename), __in const char * _P(_ArgList), ...);
_CRT_NONSTDC_DEPRECATE _CRTIMP intptr_t __cdecl spawnv(__in int, __in const char * _P(_Filename), __in const char * const * _P(_ArgList));
_CRT_NONSTDC_DEPRECATE _CRTIMP intptr_t __cdecl spawnve(__in int, __in const char * _P(_Filename), __in const char * const * _P(_ArgList),
        __in_opt const char * const * _P(_Env));
_CRT_NONSTDC_DEPRECATE _CRTIMP intptr_t __cdecl spawnvp(__in int, __in const char * _P(_Filename), __in const char * const * _P(_ArgList));
_CRT_NONSTDC_DEPRECATE _CRTIMP intptr_t __cdecl spawnvpe(__in int, __in const char * _P(_Filename), __in const char * const * _P(_ArgList),
        __in_opt const char * const * _P(_Env));

_CRT_NONSTDC_DEPRECATE _CRTIMP int __cdecl getpid(void);

#endif  /* __STDC__ */

#ifdef  __cplusplus
}
#endif


#endif  /* _INC_PROCESS */
