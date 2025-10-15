/***
*io.h - declarations for low-level file handling and I/O functions
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file contains the function declarations for the low-level
*       file handling and I/O functions.
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_IO
#define _INC_IO

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

#ifndef _FSIZE_T_DEFINED
typedef unsigned long _fsize_t; /* Could be 64 bits for Win32 */
#define _FSIZE_T_DEFINED
#endif

#ifndef _FINDDATA_T_DEFINED

struct _finddata32_t {
        unsigned    attrib;
        __time32_t  time_create;    /* -1 for FAT file systems */
        __time32_t  time_access;    /* -1 for FAT file systems */
        __time32_t  time_write;
        _fsize_t    size;
        char        name[260];
};

#if     _INTEGRAL_MAX_BITS >= 64

struct _finddata32i64_t {
        unsigned    attrib;
        __time32_t  time_create;    /* -1 for FAT file systems */
        __time32_t  time_access;    /* -1 for FAT file systems */
        __time32_t  time_write;
        __int64     size;
        char        name[260];
};

struct _finddata64i32_t {
        unsigned    attrib;
        __time64_t  time_create;    /* -1 for FAT file systems */
        __time64_t  time_access;    /* -1 for FAT file systems */
        __time64_t  time_write;
        _fsize_t    size;
        char        name[260];
};

struct __finddata64_t {
        unsigned    attrib;
        __time64_t  time_create;    /* -1 for FAT file systems */
        __time64_t  time_access;    /* -1 for FAT file systems */
        __time64_t  time_write;
        __int64     size;
        char        name[260];
};

#endif

#ifdef _USE_32BIT_TIME_T
#define _finddata_t     _finddata32_t
#define _finddatai64_t  _finddata32i64_t

#define _findfirst      _findfirst32
#define _findnext       _findnext32
#define _findfirsti64   _findfirst32i64
#define _findnexti64     _findnext32i64

#else
#define _finddata_t     _finddata64i32_t
#define _finddatai64_t  __finddata64_t

#define _findfirst      _findfirst64i32
#define _findnext       _findnext64i32
#define _findfirsti64   _findfirst64
#define _findnexti64    _findnext64

#endif


#define _FINDDATA_T_DEFINED
#endif

#ifndef _WFINDDATA_T_DEFINED

struct _wfinddata32_t {
        unsigned    attrib;
        __time32_t  time_create;    /* -1 for FAT file systems */
        __time32_t  time_access;    /* -1 for FAT file systems */
        __time32_t  time_write;
        _fsize_t    size;
        wchar_t     name[260];
};

#if     _INTEGRAL_MAX_BITS >= 64

struct _wfinddata32i64_t {
        unsigned    attrib;
        __time32_t  time_create;    /* -1 for FAT file systems */
        __time32_t  time_access;    /* -1 for FAT file systems */
        __time32_t  time_write;
        __int64     size;
        wchar_t     name[260];
};

struct _wfinddata64i32_t {
        unsigned    attrib;
        __time64_t  time_create;    /* -1 for FAT file systems */
        __time64_t  time_access;    /* -1 for FAT file systems */
        __time64_t  time_write;
        _fsize_t    size;
        wchar_t     name[260];
};

struct _wfinddata64_t {
        unsigned    attrib;
        __time64_t  time_create;    /* -1 for FAT file systems */
        __time64_t  time_access;    /* -1 for FAT file systems */
        __time64_t  time_write;
        __int64     size;
        wchar_t     name[260];
};

#endif

#ifdef _USE_32BIT_TIME_T
#define _wfinddata_t    _wfinddata32_t
#define _wfinddatai64_t _wfinddata32i64_t

#define _wfindfirst     _wfindfirst32
#define _wfindnext      _wfindnext32
#define _wfindfirsti64  _wfindfirst32i64
#define _wfindnexti64   _wfindnext32i64

#else                  
#define _wfinddata_t    _wfinddata64i32_t
#define _wfinddatai64_t _wfinddata64_t

#define _wfindfirst     _wfindfirst64i32
#define _wfindnext      _wfindnext64i32
#define _wfindfirsti64  _wfindfirst64
#define _wfindnexti64   _wfindnext64

#endif

#define _WFINDDATA_T_DEFINED
#endif

/* File attribute constants for _findfirst() */

#define _A_NORMAL       0x00    /* Normal file - No read/write restrictions */
#define _A_RDONLY       0x01    /* Read only file */
#define _A_HIDDEN       0x02    /* Hidden file */
#define _A_SYSTEM       0x04    /* System file */
#define _A_SUBDIR       0x10    /* Subdirectory */
#define _A_ARCH         0x20    /* Archive file */

/* function prototypes */

#ifndef _SIZE_T_DEFINED
#ifdef  _WIN64
typedef unsigned __int64    size_t;
#else
typedef _W64 unsigned int   size_t;
#endif
#define _SIZE_T_DEFINED
#endif

#include <crtdefs.h>

_CRTIMP __checkReturn int __cdecl _access(__in const char * _P(_Filename), __in int _P(_AccessMode));
_CRTIMP __checkReturn_wat errno_t __cdecl _access_s(__in const char * _P(_Filename), __in int _P(_AccessMode));
_CRTIMP __checkReturn int __cdecl _chmod(__in const char * _P(_Filename), __in int _P(_Mode));
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn int __cdecl _chsize(__in int _P(_FileHandle), __in long _P(_Size));
_CRTIMP __checkReturn_wat errno_t __cdecl _chsize_s(__in int _P(_FileHandle),__in __int64 _P(_Size));
_CRTIMP __checkReturn_opt int __cdecl _close(__in int _P(_FileHandle));
_CRTIMP __checkReturn_opt int __cdecl _commit(__in int _P(_FileHandle));
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn int __cdecl _creat(__in const char * _P(_Filename), __in int _P(_PermissionMode));
_CRTIMP __checkReturn int __cdecl _dup(__in int _P(_FileHandle));
_CRTIMP __checkReturn int __cdecl _dup2(__in int _P(_FileHandleSrc), __in int _P(_FileHandleDst));
_CRTIMP __checkReturn int __cdecl _eof(__in int _P(_FileHandle));
_CRTIMP __checkReturn long __cdecl _filelength(__in int _P(_FileHandle));
_CRTIMP __checkReturn intptr_t __cdecl _findfirst32(__in const char * _P(_Filename), __out struct _finddata32_t * _P(_FindData));
_CRTIMP __checkReturn int __cdecl _findnext32(__in intptr_t _P(_FindHandle), __out struct _finddata32_t * _P(_FindData));
_CRTIMP __checkReturn_opt int __cdecl _findclose(__in intptr_t _P(_FindHandle));
_CRTIMP __checkReturn int __cdecl _isatty(__in int _P(_FileHandle));
#ifndef _XBSTRICT
_CRTIMP int __cdecl _locking(__in int _P(_FileHandle), __in int _P(_LockMode), __in long _P(_NumOfBytes));
#endif // XBSTRICT
_CRTIMP __checkReturn_opt long __cdecl _lseek(__in int _P(_FileHandle), __in long _P(_Offset), __in int _P(_Origin));
_CRTIMP __checkReturn_wat errno_t __cdecl _mktemp_s(__inout_ecount(_Size) char * _P(_TemplateName), __in size_t _P(_Size));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_0(errno_t, _mktemp_s, __inout_ecount(_Size) char)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_0(__checkReturn char *, __RETURN_POLICY_DST, _CRTIMP, _mktemp, __inout char)
#ifndef _XBSTRICT
_CRTIMP __checkReturn int __cdecl _pipe(__inout_ecount(2) int * _P(_PtHandles), __in unsigned int _P(_PipeSize), __in int _P(_TextMode));
#endif // XBSTRICT
_CRTIMP __checkReturn int __cdecl _read(__in int _P(_FileHandle), __out_bcount(_MaxCharCount) void * _P(_DstBuf), __in unsigned int _P(_MaxCharCount));
_CRTIMP __checkReturn int __cdecl remove(__in const char * _P(_Filename));
_CRTIMP __checkReturn int __cdecl rename(__in const char * _P(_OldFilename), __in const char * _P(_NewFilename));
_CRTIMP __checkReturn int __cdecl _setmode(__in int _P(_FileHandle), __in int _P(_Mode));
_CRTIMP __checkReturn long __cdecl _tell(__in int _P(_FileHandle));
_CRT_INSECURE_DEPRECATE _CRTIMP int __cdecl _umask(__in int _P(_Mode));
_CRTIMP __checkReturn_wat errno_t __cdecl _umask_s(__in int _P(_NewMode), __out int * _P(_OldMode));
_CRTIMP __checkReturn int __cdecl _unlink(__in const char * _P(_Filename));
_CRTIMP int __cdecl _write(__in int _P(_FileHandle), __in_bcount(_MaxCharCount) const void * _P(_Buf), __in unsigned int _P(_MaxCharCount));

#if     _INTEGRAL_MAX_BITS >= 64
_CRTIMP __checkReturn __int64 __cdecl _filelengthi64(__in int _P(_FileHandle));
_CRTIMP __checkReturn intptr_t __cdecl _findfirst32i64(__in const char * _P(_Filename), __out struct _finddata32i64_t * _P(_FindData));
_CRTIMP __checkReturn intptr_t __cdecl _findfirst64i32(__in const char * _P(_Filename), __out struct _finddata64i32_t * _P(_FindData));
_CRTIMP __checkReturn intptr_t __cdecl _findfirst64(__in const char * _P(_Filename), __out struct __finddata64_t * _P(_FindData));
_CRTIMP __checkReturn int __cdecl _findnext32i64(__in intptr_t _P(_FindHandle), __out struct _finddata32i64_t * _P(_FindData));
_CRTIMP __checkReturn int __cdecl _findnext64i32(__in intptr_t _P(_FindHandle), __out struct _finddata64i32_t * _P(_FindData));
_CRTIMP __checkReturn int __cdecl _findnext64(__in intptr_t _P(_FindHandle), __out struct __finddata64_t * _P(_FindData));
_CRTIMP __checkReturn_opt __int64 __cdecl _lseeki64(__in int _P(_FileHandle), __in __int64 _P(_Offset), __in int _P(_Origin));
_CRTIMP __checkReturn __int64 __cdecl _telli64(__in int _P(_FileHandle));
#endif

_CRTIMP __checkReturn_wat errno_t __cdecl _sopen_s(__out int * _P(_FileHandle), __in const char * _P(_Filename),__in int _P(_OpenFlag), __in int _P(_ShareFlag), __in int _P(_PermissionMode));

#if !defined(__cplusplus)
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn int __cdecl _open(__in const char * _P(_Filename), __in int _P(_OpenFlag), ...);
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn int __cdecl _sopen(__in const char * _P(_Filename), __in int _P(_OpenFlag), int _P(_ShareFlag), ...);
#else

/* these function do not validate pmode; use _sopen_s */
extern "C++" _CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn int __cdecl _open(__in const char * _Filename, __in int _Openflag, __in int _PermissionMode = 0);
extern "C++" _CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn int __cdecl _sopen(__in const char * _Filename, __in int _Openflag, __in int _ShareFlag, __in int _PermissionMode = 0);

#endif

#ifndef _WIO_DEFINED

/* wide function prototypes, also declared in wchar.h  */

_CRTIMP __checkReturn int __cdecl _waccess(__in const wchar_t * _P(_Filename), __in int _P(_AccessMode));
_CRTIMP __checkReturn_wat errno_t __cdecl _waccess_s(__in const wchar_t * _P(_Filename), __in int _P(_AccessMode));
_CRTIMP __checkReturn int __cdecl _wchmod(__in const wchar_t * _P(_Filename), __in int _P(_Mode));
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn int __cdecl _wcreat(__in const wchar_t * _P(_Filename), __in int _P(_PermissionMode));
_CRTIMP __checkReturn intptr_t __cdecl _wfindfirst32(__in const wchar_t * _P(_Filename), __out struct _wfinddata32_t * _P(_FindData));
_CRTIMP __checkReturn int __cdecl _wfindnext32(__in intptr_t _P(_FindHandle), __out struct _wfinddata32_t * _P(_FindData));
_CRTIMP __checkReturn int __cdecl _wunlink(__in const wchar_t * _P(_Filename));
_CRTIMP __checkReturn int __cdecl _wrename(__in const wchar_t * _P(_NewFilename), __in const wchar_t * _P(_OldFilename));
_CRTIMP errno_t __cdecl _wmktemp_s(__inout_ecount(_SizeInWords) wchar_t * _P(_TemplateName), __in size_t _P(_SizeInWords));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_0(errno_t, _wmktemp_s, __inout_ecount(_Size) wchar_t)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_0(wchar_t *, __RETURN_POLICY_DST, _CRTIMP, _wmktemp, __inout wchar_t)

#if     _INTEGRAL_MAX_BITS >= 64
_CRTIMP __checkReturn intptr_t __cdecl _wfindfirst32i64(__in const wchar_t * _P(_Filename), __out struct _wfinddata32i64_t * _P(_FindData));
_CRTIMP __checkReturn intptr_t __cdecl _wfindfirst64i32(__in const wchar_t * _P(_Filename), __out struct _wfinddata64i32_t * _P(_FindData));
_CRTIMP __checkReturn intptr_t __cdecl _wfindfirst64(__in const wchar_t * _P(_Filename), __out struct _wfinddata64_t * _P(_FindData));
_CRTIMP __checkReturn int __cdecl _wfindnext32i64(__in intptr_t _P(_FindHandle), __out struct _wfinddata32i64_t * _P(_FindData));
_CRTIMP __checkReturn int __cdecl _wfindnext64i32(__in intptr_t _P(_FindHandle), __out struct _wfinddata64i32_t * _P(_FindData));
_CRTIMP __checkReturn int __cdecl _wfindnext64(__in intptr_t _P(_FindHandle), __out struct _wfinddata64_t * _P(_FindData));
#endif

_CRTIMP __checkReturn_wat errno_t __cdecl _wsopen_s(__out int * _P(_FileHandle), __in const wchar_t * _P(_Filename), __in int _P(_OpenFlag), __in int _P(_ShareFlag), __in int _P(_PermissionFlag));


_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn int __cdecl _wopen(__in const wchar_t * _P(_Filename), __in int _P(_OpenFlag), ...);
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn int __cdecl _wsopen(__in const wchar_t * _P(_Filename), __in int _P(_OpenFlag), int _P(_ShareFlag), ...);


#define _WIO_DEFINED
#endif

int  __cdecl __lock_fhandle(__in int _P(_Filehandle));
void __cdecl _unlock_fhandle(__in int _P(_Filehandle));


_CRTIMP intptr_t __cdecl _get_osfhandle(__in int _P(_FileHandle));
_CRTIMP int __cdecl _open_osfhandle(__in intptr_t _P(_OSFileHandle), __in int _P(_Flags));

#if     !__STDC__

/* Non-ANSI names for compatibility */

#pragma warning(push)
#pragma warning(disable: 4141) /* Using deprecated twice */ 
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl access(__in const char * _P(_Filename), __in int _P(_AccessMode));
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl chmod(__in const char * _P(_Filename), int _P(_AccessMode));
_CRT_NONSTDC_DEPRECATE _CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn int __cdecl chsize(__in int _P(_FileHandle), __in long _P(_Size));
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl close(__in int _P(_FileHandle));
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl creat(__in const char * _P(_Filename), __in int _P(_PermissionMode));
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl dup(__in int _P(_FileHandle));
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl dup2(__in int _P(_FileHandleSrc), __in int _P(_FileHandleDst));
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl eof(__in int _P(_FileHandle));
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn long __cdecl filelength(__in int _P(_FileHandle));
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl isatty(__in int _P(_FileHandle));
#ifndef _XBSTRICT
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl locking(__in int _P(_FileHandle), __in int _P(_LockMode), __in long _P(_NumOfBytes));
#endif
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn_opt long __cdecl lseek(__in int _P(_FileHandle), __in long _P(_Offset), __in int _P(_Origin));
_CRT_NONSTDC_DEPRECATE _CRT_INSECURE_DEPRECATE _CRTIMP char * __cdecl mktemp(__inout char * _P(_TemplateName));
_CRT_NONSTDC_DEPRECATE _CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn int __cdecl open(__in const char * _P(_Filename), __in int _P(_OpenFlag), ...);
_CRT_NONSTDC_DEPRECATE _CRTIMP int __cdecl read(int _P(_FileHandle), __out_bcount(_MaxCharCount) void * _P(_DstBuf), __in unsigned int _P(_MaxCharCount));
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl setmode(__in int _P(_FileHandle), __in int _P(_Mode));
_CRT_NONSTDC_DEPRECATE _CRT_INSECURE_DEPRECATE _CRTIMP int __cdecl sopen(const char * _P(_Filename), __in int _P(_OpenFlag), __in int _P(_ShareFlag), ...);
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn long __cdecl tell(__in int _P(_FileHandle));
_CRT_NONSTDC_DEPRECATE _CRT_INSECURE_DEPRECATE _CRTIMP int __cdecl umask(__in int _P(_Mode));
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl unlink(__in const char * _P(_Filename));
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl write(__in int _P(_Filehandle), __in_bcount(_MaxCharCount) const void * _P(_Buf), __in unsigned int _P(_MaxCharCount));
#pragma warning(pop)

#endif  /* __STDC__ */

#ifdef  __cplusplus
}
#endif


#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif  /* _INC_IO */
