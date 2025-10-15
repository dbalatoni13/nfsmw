/***
*stdio.h - definitions/declarations for standard I/O routines
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file defines the structures, values, macros, and functions
*       used by the level 2 I/O ("standard I/O") routines.
*       [ANSI/System V]
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_STDIO
#define _INC_STDIO

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


/* Buffered I/O macros */

#define BUFSIZ  512


/*
 * Default number of supported streams. _NFILE is confusing and obsolete, but
 * supported anyway for backwards compatibility.
 */
#define _NFILE      _NSTREAM_

#define _NSTREAM_   512

/*
 * Number of entries in _iob[] (declared below). Note that _NSTREAM_ must be
 * greater than or equal to _IOB_ENTRIES.
 */
#define _IOB_ENTRIES 20

#define EOF     (-1)


#ifndef _FILE_DEFINED
struct _iobuf {
        char *_ptr;
        int   _cnt;
        char *_base;
        int   _flag;
        int   _file;
        int   _charbuf;
        int   _bufsiz;
        char *_tmpfname;
        };
typedef struct _iobuf FILE;
#define _FILE_DEFINED
#endif


/* Directory where temporary files may be created. */

#define _P_tmpdir   "D:\\"
#define _wP_tmpdir  L"D:\\"

/* L_tmpnam = length of string _P_tmpdir
 *            + 1 if _P_tmpdir does not end in "/" or "\", else 0
 *            + 12 (for the filename string)
 *            + 1 (for the null terminator)
 * L_tmpnam_s = length of string _P_tmpdir
 *            + 1 if _P_tmpdir does not end in "/" or "\", else 0
 *            + 16 (for the filename string)
 *            + 1 (for the null terminator)
 */
#define L_tmpnam   (sizeof(_P_tmpdir) + 12)
#define L_tmpnam_s (sizeof(_P_tmpdir) + 16)



/* Seek method constants */

#define SEEK_CUR    1
#define SEEK_END    2
#define SEEK_SET    0


#define FILENAME_MAX    260
#define FOPEN_MAX       20
#define _SYS_OPEN       20
#define TMP_MAX         32767  /* SHRT_MAX */
#define _TMP_MAX_S      2147483647 /* INT_MAX */


/* Define NULL pointer value */

#ifndef NULL
#ifdef  __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif


/* Declare _iob[] array */

#ifndef _STDIO_DEFINED
/* These functions are for enabling STATIC_CPPLIB functionality */
_CRTIMP FILE * __cdecl __iob_func(void);
#endif  /* _STDIO_DEFINED */


/* Define file position type */

#ifndef _FPOS_T_DEFINED
#undef _FPOSOFF


#if     !__STDC__ && _INTEGRAL_MAX_BITS >= 64
typedef __int64 fpos_t;
#define _FPOSOFF(fp) ((long)(fp))
#else
typedef struct fpos_t {
        unsigned int lopart;
        int          hipart;
        } fpos_t;
#define _FPOSOFF(fp) ((long)(fp).lopart)
#endif

#define _FPOS_T_DEFINED
#endif


#define stdin  (&__iob_func()[0])
#define stdout (&__iob_func()[1])
#define stderr (&__iob_func()[2])


#define _IOREAD         0x0001
#define _IOWRT          0x0002

#define _IOFBF          0x0000
#define _IOLBF          0x0040
#define _IONBF          0x0004

#define _IOMYBUF        0x0008
#define _IOEOF          0x0010
#define _IOERR          0x0020
#define _IOSTRG         0x0040
#define _IORW           0x0080

/* constants used by _set_output_format */
#define _TWO_DIGIT_EXPONENT 0x1

/* Function prototypes */

#ifndef _STDIO_DEFINED

_CRTIMP __checkReturn int __cdecl _filbuf(__inout FILE * _P(_File) );
_CRTIMP __checkReturn_opt int __cdecl _flsbuf(__in int _P(_Ch), __inout FILE * _P(_File));

_CRTIMP __checkReturn FILE * __cdecl _fsopen(__in const char * _P(_Filename), __in const char * _P(_Mode), __in int _P(_ShFlag));

_CRTIMP void __cdecl clearerr(__inout FILE * _P(_File));
_CRTIMP __checkReturn_wat errno_t __cdecl clearerr_s(__inout FILE * _P(_File) );
_CRTIMP __checkReturn_opt int __cdecl fclose(__inout FILE * _P(_File));
_CRTIMP __checkReturn_opt int __cdecl _fcloseall(void);

_CRTIMP __checkReturn FILE * __cdecl _fdopen(__in int _P(_FileHandle), __in const char * _P(_Mode));

_CRTIMP __checkReturn int __cdecl feof(__in FILE * _P(_File));
_CRTIMP __checkReturn int __cdecl ferror(__in FILE * _P(_File));
_CRTIMP __checkReturn_opt int __cdecl fflush(__in_opt FILE * _P(_File));
_CRTIMP __checkReturn_opt int __cdecl fgetc(__inout FILE * _P(_File));
_CRTIMP __checkReturn_opt int __cdecl _fgetchar(void);
_CRTIMP __checkReturn_opt int __cdecl fgetpos(__in FILE * _P(_File) , __out fpos_t * _P(_Pos));
_CRTIMP __checkReturn_opt char * __cdecl fgets(__out_ecount(_MaxCount) char * _P(_Buf), __in int _P(_MaxCount), __inout FILE * _P(_File));

_CRTIMP __checkReturn int __cdecl _fileno(__in FILE * _P(_File));

#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
#pragma push_macro("_tempnam")
#undef _tempnam
#endif

_CRTIMP __checkReturn char * __cdecl _tempnam(__in_opt const char * _P(_DirName), __in_opt const char * _P(_FilePrefix));

#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
#pragma pop_macro("_tempnam")
#endif

_CRTIMP __checkReturn_opt int __cdecl _flushall(void);
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn FILE * __cdecl fopen(__in const char * _P(_Filename), __in const char * _P(_Mode));
_CRTIMP __checkReturn_wat errno_t __cdecl fopen_s(__deref_out_opt FILE ** _P(_File), __in const char * _P(_Filename), __in const char * _P(_Mode));
_CRTIMP __checkReturn_opt int __cdecl fprintf(__inout FILE * _P(_File), __in __format_string const char * _P(_Format), ...);
_CRTIMP __checkReturn_opt int __cdecl fprintf_s(__inout FILE * _P(_File), __in __format_string const char * _P(_Format), ...);
_CRTIMP __checkReturn_opt int __cdecl fputc(__in int _P(_Ch), __inout FILE * _P(_File));
_CRTIMP __checkReturn_opt int __cdecl _fputchar(__in int _P(_Ch));
_CRTIMP __checkReturn_opt int __cdecl fputs(__in const char * _P(_Str), __inout FILE * _P(_File));
_CRTIMP __checkReturn_opt size_t __cdecl fread(__out_bcount(_Size*_Count) void * _P(_DstBuf), __in size_t _P(_Size), __in size_t _P(_Count), __inout FILE * _P(_File));
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn FILE * __cdecl freopen(__in const char * _P(_Filename), __in const char * _P(_Mode), __inout FILE * _P(_File));
_CRTIMP __checkReturn_wat errno_t __cdecl freopen_s(__deref_out_opt FILE ** _P(_File), __in const char * _P(_Filename), __in const char * _P(_Mode), __inout FILE * _P(_OldFile));
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl fscanf(__inout FILE * _P(_File), __in __format_string const char * _P(_Format), ...);
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl _fscanf_l(__inout FILE * _P(_File), __in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn_opt int __cdecl fscanf_s(__inout FILE * _P(_File), __in __format_string const char * _P(_Format), ...);
_CRTIMP __checkReturn_opt int __cdecl _fscanf_s_l(__inout FILE * _P(_File), __in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn_opt int __cdecl fsetpos(__inout FILE * _P(_File), __in const fpos_t * _P(_Pos));
_CRTIMP __checkReturn_opt int __cdecl fseek(__inout FILE * _P(_File), __in long _P(_Offset), __in int _P(_Origin));
_CRTIMP __checkReturn long __cdecl ftell(__inout FILE * _P(_File));

_CRTIMP __checkReturn_opt int __cdecl _fseeki64(__inout FILE * _P(_File), __in __int64 _P(_Offset), __in int _P(_Origin));
_CRTIMP __checkReturn __int64 __cdecl _ftelli64(__inout FILE * _P(_File));

_CRTIMP __checkReturn_opt size_t __cdecl fwrite(__in_ecount(_Size*_Count) const void * _P(_Str), __in size_t _P(_Size), __in size_t _P(_Count), __inout FILE * _P(_File));
_CRTIMP __checkReturn int __cdecl getc(__inout FILE * _P(_File));
_CRTIMP __checkReturn int __cdecl getchar(void);
_CRTIMP __checkReturn int __cdecl _getmaxstdio(void);
_CRTIMP char * __cdecl gets_s(__out_ecount(_Size) char * _P(_Buf), __in size_t _P(_Size));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_0(char *, gets_s, __out_ecount(_Size) char)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_0(char *, __RETURN_POLICY_SAME, _CRTIMP, gets, __out char)
__checkReturn int __cdecl _getw(__inout FILE * _P(_File));
_CRTIMP void __cdecl perror(__in_opt const char * _P(_ErrMsg));
#ifndef _XBSTRICT
_CRTIMP __checkReturn_opt int __cdecl _pclose(__inout FILE * _P(_File));
_CRTIMP __checkReturn FILE * __cdecl _popen(__in const char * _P(_Command), __in const char * _P(_Mode));
#endif // XBSTRICT
_CRTIMP __checkReturn_opt int __cdecl printf(__in __format_string const char * _P(_Format), ...);
_CRTIMP __checkReturn_opt int __cdecl printf_s(__in __format_string const char * _P(_Format), ...);
_CRTIMP __checkReturn_opt int __cdecl putc(__in int _P(_Ch), __inout FILE * _P(_File));
_CRTIMP __checkReturn_opt int __cdecl putchar(__in int _P(_Ch));
_CRTIMP __checkReturn_opt int __cdecl puts(__in const char * _P(_Str));
_CRTIMP __checkReturn_opt int __cdecl _putw(__in int _P(_Word), __inout FILE * _P(_File));
_CRTIMP __checkReturn int __cdecl remove(__in const char * _P(_Filename));
_CRTIMP __checkReturn int __cdecl rename(__in const char *_P(_OldName), const char * _P(_NewName));
_CRTIMP void __cdecl rewind(__inout FILE * _P(_File));
_CRTIMP __checkReturn_opt int __cdecl _rmtmp(void);
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl scanf(__in __format_string const char * _P(_Format), ...);
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl _scanf_l(__in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP_ALTERNATIVE __checkReturn_opt int __cdecl scanf_s(__in __format_string const char * _P(_Format), ...);
_CRTIMP_ALTERNATIVE __checkReturn_opt int __cdecl _scanf_s_l(__in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRT_INSECURE_DEPRECATE _CRTIMP void __cdecl setbuf(__inout FILE * _P(_File), __inout_ecount_part_opt(BUFSIZ, 0) char * _P(_Buffer));
_CRTIMP __checkReturn_opt int __cdecl _setmaxstdio(__in int _P(_Max));
_CRTIMP __checkReturn_opt unsigned int __cdecl _set_output_format(__in unsigned int _P(_Format));
_CRTIMP __checkReturn_opt unsigned int __cdecl _get_output_format(void);
_CRTIMP __checkReturn_opt int __cdecl setvbuf(__inout FILE * _P(_File), __inout_bcount_opt(_Size) char * _P(_Buf), __in int _P(_Mode), __in size_t _P(_Size));
_CRTIMP_ALTERNATIVE __checkReturn_opt int __cdecl _snprintf_s(__out_bcount(_DstSize) char * _P(_DstBuf), __in size_t _P(_DstSize), __in size_t _P(_MaxCount), __in __format_string const char * _P(_Format), ...);
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_2_ARGLIST(int, _snprintf_s, _vsnprintf_s, __out_bcount(_Size) char, __in size_t, __in __format_string const char *)
_CRTIMP_ALTERNATIVE __checkReturn_opt int __cdecl sprintf_s(__out_bcount(_DstSize) char * _P(_DstBuf), __in size_t _P(_DstSize), __in __format_string const char * _P(_Format), ...);
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_1_ARGLIST(int, sprintf_s, vsprintf_s, __out_bcount(_Size) char, __in __format_string const char *)
_CRTIMP __checkReturn int __cdecl _scprintf(__in __format_string const char * _P(_Format), ...);
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl sscanf(__in const char * _P(_Src), __in __format_string const char * _P(_Format), ...);
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl _sscanf_l(__in const char * _P(_Src), __in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP_ALTERNATIVE __checkReturn_opt int __cdecl sscanf_s(__in const char * _P(_Src), __in __format_string const char * _P(_Format), ...);
_CRTIMP_ALTERNATIVE __checkReturn_opt int __cdecl _sscanf_s_l(__in const char * _P(_Src), __in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl _snscanf(__in_bcount(_MaxCount) const char * _P(_Src), __in size_t _P(_MaxCount), __in __format_string const char * _P(_Format), ...);
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl _snscanf_l(__in_bcount(_MaxCount) const char * _P(_Src), __in size_t _P(_MaxCount), __in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP_ALTERNATIVE __checkReturn_opt int __cdecl _snscanf_s(__in_bcount(_MaxCount) const char * _P(_Src), __in size_t _P(_MaxCount), __in __format_string const char * _P(_Format), ...);
_CRTIMP_ALTERNATIVE __checkReturn_opt int __cdecl _snscanf_s_l(__in_bcount(_MaxCount) const char * _P(_Src), __in size_t _P(_MaxCount), __in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn FILE * __cdecl tmpfile(void);
_CRTIMP __checkReturn_wat errno_t __cdecl tmpfile_s(__deref_opt_out FILE ** _P(_File));
_CRTIMP __checkReturn_wat errno_t __cdecl tmpnam_s(__out_ecount(_Size) char * _P(_Buf), __in size_t _P(_Size));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_0(errno_t, tmpnam_s, __out_ecount(_Size) char)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_0(char *, __RETURN_POLICY_DST, _CRTIMP, tmpnam, __out_opt char)
_CRTIMP __checkReturn_opt int __cdecl ungetc(__in int _P(_Ch), __inout FILE * _P(_File));
_CRTIMP __checkReturn_opt int __cdecl _unlink(__in const char * _P(_Filename));
_CRTIMP __checkReturn_opt int __cdecl vfprintf(__inout FILE * _P(_File), __in __format_string const char * _P(_Format), va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl vfprintf_s(__inout FILE * _P(_File), __in __format_string const char * _P(_Format), va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl vprintf(__in __format_string const char * _P(_Format), va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl vprintf_s(__in __format_string const char * _P(_Format), va_list _P(_ArgList));
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl vsnprintf(__out_ecount(_MaxCount) char * _P(_DstBuf), __in size_t _P(_MaxCount), __in __format_string const char * _P(_Format), va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl vsnprintf_s(__out_ecount(_DstSize) char * _P(_DstBuf), __in size_t _P(_DstSize), __in size_t _P(_MaxCount), __in __format_string const char * _P(_Format), va_list _P(_ArgList));
_CRTIMP_ALTERNATIVE __checkReturn_opt int __cdecl _vsnprintf_s(__out_ecount(_DstSize) char * _P(_DstBuf), __in size_t _P(_DstSize), __in size_t _P(_MaxCount), __in __format_string const char * _P(_Format), va_list _P(_ArgList));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_3(int, _vsnprintf_s, __out_ecount(_Size) char, __in size_t, __in __format_string const char *, va_list)
__DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_2_ARGLIST(int, __RETURN_POLICY_SAME, _CRTIMP, _snprintf, _vsnprintf, __out char, __in size_t, __in __format_string const char *)
_CRTIMP_ALTERNATIVE int __cdecl vsprintf_s(__out_ecount(_Size) char * _P(_DstBuf), __in size_t _P(_Size), __in __format_string const char * _P(_Format), va_list _P(_ArgList));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_2(int, vsprintf_s, __out_ecount(_Size) char, __in __format_string const char *, va_list)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_1_ARGLIST(int, __RETURN_POLICY_SAME, _CRTIMP, sprintf, vsprintf, __out char, __in __format_string const char *)
_CRTIMP __checkReturn int __cdecl _vscprintf(__in __format_string const char * _P(_Format), va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl _snprintf_c(__out_ecount(_MaxCount) char * _P(_DstBuf), __in size_t _P(_MaxCount), __in __format_string const char * _P(_Format), ...);
_CRTIMP __checkReturn_opt int __cdecl _vsnprintf_c(__out_ecount(_MaxCount) char *_P(_DstBuf), __in size_t _P(_MaxCount), __in __format_string const char * _P(_Format), va_list _P(_ArgList));

_CRTIMP __checkReturn_opt int __cdecl _fprintf_p(__inout FILE * _P(_File), __in __format_string const char * _P(_Format), ...);
_CRTIMP __checkReturn_opt int __cdecl _printf_p(__in __format_string const char * _P(_Format), ...);
_CRTIMP __checkReturn_opt int __cdecl _sprintf_p(__out_ecount(_MaxCount) char * _P(_Dst), __in size_t _P(_MaxCount), __in __format_string const char * _P(_Format), ...);
_CRTIMP __checkReturn_opt int __cdecl _vfprintf_p(__inout FILE * _P(_File), __in __format_string const char * _P(_Format), va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl _vprintf_p(__in __format_string const char * _P(_Format), va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl _vsprintf_p(__out_ecount(_MaxCount) char * _P(_Dst), __in size_t _P(_MaxCount), __in __format_string const char * _P(_Format), va_list _P(_ArgList));
_CRTIMP __checkReturn int __cdecl _scprintf_p(__in __format_string const char * _P(_Format), ...);
_CRTIMP __checkReturn int __cdecl _vscprintf_p(__in __format_string const char * _P(_Format), va_list _P(_ArgList));
_CRTIMP int __cdecl _set_printf_count_output(__in int _P(_Value));
_CRTIMP int __cdecl _get_printf_count_output();

_CRTIMP __checkReturn_opt int __cdecl _printf_l(__in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn_opt int __cdecl _printf_p_l(__in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn_opt int __cdecl _printf_s_l(__in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn_opt int __cdecl _vprintf_l(__in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl _vprintf_p_l(__in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl _vprintf_s_l(__in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));

_CRTIMP __checkReturn_opt int __cdecl _fprintf_l(__inout FILE * _P(_File), __in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn_opt int __cdecl _fprintf_p_l(__inout FILE * _P(_File), __in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn_opt int __cdecl _fprintf_s_l(__inout FILE * _P(_File), __in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn_opt int __cdecl _vfprintf_l(__inout FILE * _P(_File), __in const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl _vfprintf_p_l(__inout FILE * _P(_File), __in const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl _vfprintf_s_l(__inout FILE * _P(_File), __in const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));

_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl _sprintf_l(__out char * _P(_DstBuf), __in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn_opt int __cdecl _sprintf_p_l(__out_ecount(_MaxCount) char * _P(_DstBuf), __in size_t _P(_MaxCount), __in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP_ALTERNATIVE __checkReturn_opt int __cdecl _sprintf_s_l(__out_bcount(_DstSize) char * _P(_DstBuf), __in size_t _P(_DstSize), __in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl _vsprintf_l(__out char * _P(_DstBuf), __in const char * _P(_Format), __in_opt _locale_t, va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl _vsprintf_p_l(__out_ecount(_MaxCount) char * _P(_DstBuf), __in size_t _P(_MaxCount), __in __format_string const char* _P(_Format), __in_opt _locale_t _P(_PtLoci),  va_list _P(_ArgList));
_CRTIMP_ALTERNATIVE __checkReturn_opt int __cdecl _vsprintf_s_l(__out_ecount(_DstSize) char * _P(_DstBuf), __in size_t _P(_DstSize), __in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));

_CRTIMP __checkReturn_opt int __cdecl _scprintf_l(__in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn_opt int __cdecl _scprintf_p_l(__in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn_opt int __cdecl _vscprintf_l(__in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl _vscprintf_p_l(__in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));

_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl _snprintf_l(__out_ecount(_MaxCount) char * _P(_DstBuf), __in size_t _P(_MaxCount), __in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn_opt int __cdecl _snprintf_c_l(__out_ecount(_MaxCount) char * _P(_DstBuf), __in size_t _P(_MaxCount), __in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP_ALTERNATIVE __checkReturn_opt int __cdecl _snprintf_s_l(__out_ecount(_DstSize) char * _P(_DstBuf), __in size_t _P(_DstSize), __in size_t _P(_MaxCount), __in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl _vsnprintf_l(__out_ecount(_MaxCount) char * _P(_DstBuf), __in size_t _P(_MaxCount), __in __format_string const char * _P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl _vsnprintf_c_l(__out_ecount(_MaxCount) char * _P(_DstBuf), __in size_t _P(_MaxCount), const char *, __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl _vsnprintf_s_l(__out_ecount(_DstSize) char * _P(_DstBuf), __in size_t _P(_DstSize), __in size_t _P(_MaxCount), __in __format_string const char* _P(_Format),__in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));

#ifndef _WSTDIO_DEFINED

/* wide function prototypes, also declared in wchar.h  */

#ifndef WEOF
#define WEOF (wint_t)(0xFFFF)
#endif

_CRTIMP __checkReturn FILE * __cdecl _wfsopen(__in const wchar_t * _P(_Filename), __in const wchar_t * _P(_Mode), __in int _P(_ShFlag));

_CRTIMP __checkReturn_opt wint_t __cdecl fgetwc(__inout FILE * _P(_File));
_CRTIMP __checkReturn_opt wint_t __cdecl _fgetwchar(void);
_CRTIMP __checkReturn_opt wint_t __cdecl fputwc(__in wchar_t _P(_Ch), __inout FILE * _P(_File));
_CRTIMP __checkReturn_opt wint_t __cdecl _fputwchar(__in wchar_t _P(_Ch));
_CRTIMP __checkReturn wint_t __cdecl getwc(__inout FILE * _P(_File));
_CRTIMP __checkReturn wint_t __cdecl getwchar(void);
_CRTIMP __checkReturn_opt wint_t __cdecl putwc(__in wchar_t _P(_Ch), __inout FILE * _P(_File));
_CRTIMP __checkReturn_opt wint_t __cdecl putwchar(__in wchar_t _P(_Ch));
_CRTIMP __checkReturn_opt wint_t __cdecl ungetwc(__in wint_t _P(_Ch), __inout FILE * _P(_File));

_CRTIMP __checkReturn_opt wchar_t * __cdecl fgetws(__out_ecount(_SizeInWords) wchar_t * _P(_Dst), __in int _P(_SizeInWords), __inout FILE * _P(_File));
_CRTIMP __checkReturn_opt int __cdecl fputws(__in const wchar_t * _P(_Str), __inout FILE * _P(_File));
_CRTIMP __checkReturn_opt wchar_t * __cdecl _getws_s(__out_ecount(_SizeInWords) wchar_t * _P(_Str), __in size_t _P(_SizeInWords));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_0(wchar_t *, _getws_s, __out_ecount(_Size) wchar_t)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_0(wchar_t *, __RETURN_POLICY_SAME, _CRTIMP, _getws, __out wchar_t)
_CRTIMP __checkReturn_opt int __cdecl _putws(__in const wchar_t * _P(_Str));

_CRTIMP __checkReturn_opt int __cdecl fwprintf(__inout FILE * _P(_File), __in __format_string const wchar_t * _P(_Format), ...);
_CRTIMP __checkReturn_opt int __cdecl fwprintf_s(__inout FILE * _P(_File), __in __format_string const wchar_t * _P(_Format), ...);
_CRTIMP __checkReturn_opt int __cdecl wprintf(__in __format_string const wchar_t * _P(_Format), ...);
_CRTIMP __checkReturn_opt int __cdecl wprintf_s(__in __format_string const wchar_t * _P(_Format), ...);
_CRTIMP __checkReturn int __cdecl _scwprintf(__in __format_string const wchar_t * _P(_Format), ...);
_CRTIMP __checkReturn_opt int __cdecl vfwprintf(__inout FILE * _P(_File), __in __format_string const wchar_t * _P(_Format), va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl vfwprintf_s(__inout FILE * _P(_File), __in __format_string const wchar_t * _P(_Format), va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl vwprintf(__in __format_string const wchar_t * _P(_Format), va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl vwprintf_s(__in __format_string const wchar_t * _P(_Format), va_list _P(_ArgList));

_CRTIMP_ALTERNATIVE int __cdecl swprintf_s(__out_ecount(_SizeInWords) wchar_t * _P(_Dst), __in size_t _P(_SizeInWords), __in __format_string const wchar_t * _P(_Format), ...);
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_1_ARGLIST(int, swprintf_s, vswprintf_s, __out_ecount(_Size) wchar_t, __in __format_string const wchar_t *)
_CRTIMP_ALTERNATIVE int __cdecl vswprintf_s(__out_ecount(_SizeInWords) wchar_t * _P(_Dst), __in size_t _P(_SizeInWords), __in __format_string const wchar_t * _P(_Format), va_list _P(_ArgList));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_2(int, vswprintf_s, __out_ecount(_Size) wchar_t, __in __format_string const wchar_t *, va_list)

_CRTIMP __checkReturn_opt int __cdecl _swprintf_c(__out_ecount(_SizeInWords) wchar_t * _P(_DstBuf), __in size_t _P(_SizeInWords), __in __format_string const wchar_t * _P(_Format), ...);
_CRTIMP __checkReturn_opt int __cdecl _vswprintf_c(__out_ecount(_SizeInWords) wchar_t * _P(_DstBuf), __in size_t, const wchar_t *, va_list _P(_ArgList));

_CRTIMP_ALTERNATIVE __checkReturn_opt int __cdecl _snwprintf_s(__out_ecount(_DstSizeInWords) wchar_t * _P(_DstBuf), __in size_t _P(_DstSizeInWords), __in size_t _P(_MaxCount), __in __format_string const wchar_t * _P(_Format), ...);
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_2_ARGLIST(int, _snwprintf_s, _vsnwprintf_s, __out_ecount(_Size) wchar_t, __in size_t, __in __format_string const wchar_t *)
_CRTIMP_ALTERNATIVE __checkReturn_opt int __cdecl _vsnwprintf_s(__out_ecount(_DstSizeInWords) wchar_t * _P(_DstBuf), __in size_t _P(_SizeInWords), __in size_t _P(_MaxCount), __in __format_string const wchar_t * _P(_Format), va_list _P(_ArgList));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_3(int, _vsnwprintf_s, __out_ecount(_Size) wchar_t, __in size_t, __in __format_string const wchar_t *, va_list)
__DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_2_ARGLIST(int, __RETURN_POLICY_SAME, _CRTIMP, _snwprintf, _vsnwprintf, __out wchar_t, __in size_t, __in __format_string const wchar_t *)

_CRTIMP __checkReturn_opt int __cdecl _fwprintf_p(__inout FILE * _P(_File), __in __format_string const wchar_t * _P(_Format), ...);
_CRTIMP __checkReturn_opt int __cdecl _wprintf_p(__in __format_string const wchar_t * _P(_Format), ...);
_CRTIMP __checkReturn_opt int __cdecl _vfwprintf_p(__inout FILE * _P(_File), __in __format_string const wchar_t * _P(_Format), va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl _vwprintf_p(__in __format_string const wchar_t * _P(_Format), va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl _swprintf_p(__out_ecount(_MaxCount) wchar_t * _P(_DstBuf), __in size_t _P(_MaxCount), __in __format_string const wchar_t * _P(_Format), ...);
_CRTIMP __checkReturn_opt int __cdecl _vswprintf_p(__out_ecount(_MaxCount) wchar_t * _P(_DstBuf), __in size_t _P(_MaxCount), __in __format_string const wchar_t * _P(_Format), va_list _P(_ArgList));
_CRTIMP __checkReturn int __cdecl _scwprintf_p(__in __format_string const wchar_t * _P(_Format), ...);
_CRTIMP __checkReturn int __cdecl _vscwprintf_p(__in __format_string const wchar_t * _P(_Format), va_list _P(_ArgList));

_CRTIMP __checkReturn_opt int __cdecl _wprintf_l(__in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn_opt int __cdecl _wprintf_p_l(__in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn_opt int __cdecl _wprintf_s_l(__in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn_opt int __cdecl _vwprintf_l(__in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl _vwprintf_p_l(__in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl _vwprintf_s_l(__in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));

_CRTIMP __checkReturn_opt int __cdecl _fwprintf_l(__inout FILE * _P(_File), __in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn_opt int __cdecl _fwprintf_p_l(__inout FILE * _P(_File), __in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn_opt int __cdecl _fwprintf_s_l(__inout FILE * _P(_File), __in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn_opt int __cdecl _vfwprintf_l(__inout FILE * _P(_File), __in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl _vfwprintf_p_l(__inout FILE * _P(_File), __in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl _vfwprintf_s_l(__inout FILE * _P(_File), __in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));

_CRTIMP __checkReturn_opt int __cdecl _swprintf_c_l(__out_ecount(_MaxCount) wchar_t * _P(_DstBuf), __in size_t _P(_MaxCount), __in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn_opt int __cdecl _swprintf_p_l(__out_ecount(_MaxCount) wchar_t * _P(_DstBuf), __in size_t _P(_MaxCount), __in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP_ALTERNATIVE __checkReturn_opt int __cdecl _swprintf_s_l(__out_ecount(_DstSize) wchar_t * _P(_DstBuf), __in size_t _P(_DstSize), __in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn_opt int __cdecl _vswprintf_c_l(__out_ecount(_MaxCount) wchar_t * _P(_DstBuf), __in size_t _P(_MaxCount), __in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));
_CRTIMP __checkReturn_opt int __cdecl _vswprintf_p_l(__out_ecount(_MaxCount) wchar_t * _P(_DstBuf), __in size_t _P(_MaxCount), __in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));
_CRTIMP_ALTERNATIVE __checkReturn_opt int __cdecl _vswprintf_s_l(__out_ecount(_DstSize) wchar_t * _P(_DstBuf), __in size_t _P(_DstSize), __in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));

_CRTIMP __checkReturn int __cdecl _scwprintf_l(__in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn int __cdecl _scwprintf_p_l(__in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn int __cdecl _vscwprintf_p_l(__in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));

_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl _snwprintf_l(__out_ecount(_MaxCount) wchar_t * _P(_DstBuf), __in size_t _P(_MaxCount), __in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP_ALTERNATIVE __checkReturn_opt int __cdecl _snwprintf_s_l(__out_ecount(_DstSize) wchar_t * _P(_DstBuf), __in size_t _P(_DstSize), __in size_t _P(_MaxCount), __in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl _vsnwprintf_l(__out_ecount(_MaxCount) wchar_t * _P(_DstBuf), __in size_t _P(_MaxCount), __in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));
_CRTIMP_ALTERNATIVE __checkReturn_opt int __cdecl _vsnwprintf_s_l(__out_ecount(_DstSize) wchar_t * _P(_DstBuf), __in size_t _P(_DstSize), __in size_t _P(_MaxCount), __in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));



#define _SWPRINTFS_DEPRECATED 


/* we could end up with a double deprecation, disable warnings 4141 and 4996 */
#pragma warning(push)
#pragma warning(disable:4141 4996)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_1_ARGLIST_EX(int, __RETURN_POLICY_SAME, _SWPRINTFS_DEPRECATED _CRTIMP, _swprintf, _vswprintf, vswprintf_s, __out wchar_t, __in __format_string const wchar_t *)
#pragma warning(pop)

#if !defined(RC_INVOKED) && !defined(__midl)
#include <swprintf.inl>
#endif

#ifndef __cplusplus
#define swprintf _swprintf
#define vswprintf _vswprintf
#endif

#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
#pragma push_macro("_wtempnam")
#undef _wtempnam
#endif

_CRTIMP __checkReturn wchar_t * __cdecl _wtempnam(__in_opt const wchar_t * _P(_Directory), __in_opt const wchar_t * _P(_FilePrefix));

#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
#pragma pop_macro("_wtempnam")
#endif

_CRTIMP __checkReturn int __cdecl _vscwprintf(__in __format_string const wchar_t * _P(_Format), va_list _P(_ArgList));
_CRTIMP __checkReturn int __cdecl _vscwprintf_l(__in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), va_list _P(_ArgList));
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl fwscanf(__inout FILE * _P(_File), __in __format_string const wchar_t * _P(_Format), ...);
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl _fwscanf_l(__inout FILE * _P(_File), __in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP __checkReturn_opt int __cdecl fwscanf_s(__inout FILE * _P(_File), __in __format_string const wchar_t * _P(_Format), ...);
_CRTIMP __checkReturn_opt int __cdecl _fwscanf_s_l(__inout FILE * _P(_File), __in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl swscanf(__in const wchar_t * _P(_Src), __in __format_string const wchar_t * _P(_Format), ...);
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl _swscanf_l(__in const wchar_t * _P(_Src), __in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP_ALTERNATIVE __checkReturn_opt int __cdecl swscanf_s(__in const wchar_t *_P(_Src), __in __format_string const wchar_t * _P(_Format), ...);
_CRTIMP_ALTERNATIVE __checkReturn_opt int __cdecl _swscanf_s_l(__in const wchar_t * _P(_Src), __in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl _snwscanf(__in_ecount(_MaxCount) const wchar_t * _P(_Src), __in size_t _P(_MaxCount), __in __format_string const wchar_t * _P(_Format), ...);
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl _snwscanf_l(__in_ecount(_MaxCount) const wchar_t * _P(_Src), __in size_t _P(_MaxCount), __in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP_ALTERNATIVE __checkReturn_opt int __cdecl _snwscanf_s(__in_ecount(_MaxCount) const wchar_t * _P(_Src), __in size_t _P(_MaxCount), __in __format_string const wchar_t * _P(_Format), ...);
_CRTIMP_ALTERNATIVE __checkReturn_opt int __cdecl _snwscanf_s_l(__in_ecount(_MaxCount) const wchar_t * _P(_Src), __in size_t _P(_MaxCount), __in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl wscanf(__in __format_string const wchar_t * _P(_Format), ...);
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl _wscanf_l(__in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);
_CRTIMP_ALTERNATIVE __checkReturn_opt int __cdecl wscanf_s(__in __format_string const wchar_t * _P(_Format), ...);
_CRTIMP_ALTERNATIVE __checkReturn_opt int __cdecl _wscanf_s_l(__in __format_string const wchar_t * _P(_Format), __in_opt _locale_t _P(_PtLoci), ...);

_CRTIMP __checkReturn FILE * __cdecl _wfdopen(__in int _P(_FileHandle) , __in const wchar_t * _P(_Mode));
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn FILE * __cdecl _wfopen(__in const wchar_t * _P(_Filename), __in const wchar_t * _P(_Mode));
_CRTIMP __checkReturn_wat errno_t __cdecl _wfopen_s(__deref_out_opt FILE ** _P(_File), __in const wchar_t * _P(_Filename), __in const wchar_t * _P(_Mode));
_CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn FILE * __cdecl _wfreopen(__in const wchar_t * _P(_Filename), __in const wchar_t * _P(_Mode), __inout FILE * _P(_OldFile));
_CRTIMP __checkReturn_wat errno_t __cdecl _wfreopen_s(__deref_out_opt FILE ** _P(_File), __in const wchar_t * _P(_Filename), __in const wchar_t * _P(_Mode), __inout FILE * _P(_OldFile));

_CRTIMP void __cdecl _wperror(__in_opt const wchar_t * _P(_ErrMsg));
#ifndef _XBSTRICT
_CRTIMP __checkReturn FILE * __cdecl _wpopen(__in const wchar_t *_P(_Command), __in const wchar_t * _P(_Mode));
#endif // XBSTRICT
_CRTIMP __checkReturn int __cdecl _wremove(__in const wchar_t * _P(_Filename));
_CRTIMP __checkReturn_wat errno_t __cdecl _wtmpnam_s(__out_ecount(_SizeInWords) wchar_t * _P(_DstBuf), __in size_t _P(_SizeInWords));
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_0(errno_t, _wtmpnam_s, __out_ecount(_Size) wchar_t)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_0(wchar_t *, __RETURN_POLICY_DST, _CRTIMP, _wtmpnam, __out_opt wchar_t)

__checkReturn_opt wint_t __cdecl _fgetwc_nolock(__inout FILE * _P(_File));
__checkReturn_opt wint_t __cdecl _fputwc_nolock(__in wchar_t _P(_Ch), __inout FILE * _P(_File));
__checkReturn_opt wint_t __cdecl _ungetwc_nolock(__in wint_t _P(_Ch), __inout FILE * _P(_File));

#undef _CRT_GETPUTWCHAR_NOINLINE

#if !defined(__cplusplus) || defined(_M_CEE_PURE) || defined(_CRT_GETPUTWCHAR_NOINLINE)
#define getwchar()      fgetwc(stdin)
#define putwchar(_c)    fputwc((_c),stdout)
#else   /* __cplusplus */
inline wint_t __CRTDECL getwchar()
        {return (fgetwc(stdin)); }   /* stdin */
inline wint_t __CRTDECL putwchar(wchar_t _C)
        {return (fputwc(_C, stdout)); }       /* stdout */
#endif  /* __cplusplus */

#define getwc(_stm)             fgetwc(_stm)
#define putwc(_c,_stm)          fputwc(_c,_stm)
#define _putwc_nolock(_c,_stm)     _fputwc_nolock(_c,_stm)
#define _getwc_nolock(_c)          _fgetwc_nolock(_c)

#if defined(_CRT_DISABLE_PERFCRITICAL_THREADLOCKING) && !defined(_DLL)
#define fgetwc(_stm)            _getwc_nolock(_stm)
#define fputwc(_c,_stm)         _putwc_nolock(_c,_stm)
#define ungetwc(_c,_stm)        _ungetwc_nolock(_c,_stm)
#endif

#define _WSTDIO_DEFINED
#endif  /* _WSTDIO_DEFINED */

#define _STDIO_DEFINED
#endif  /* _STDIO_DEFINED */


/* Macro definitions */

#if defined(_CRT_DISABLE_PERFCRITICAL_THREADLOCKING) && !defined(_DLL)
#define feof(_stream)     ((_stream)->_flag & _IOEOF)
#define ferror(_stream)   ((_stream)->_flag & _IOERR)
#define _fileno(_stream)  ((_stream)->_file)
#define fgetc(_stream)     (--(_stream)->_cnt >= 0 \
                ? 0xff & *(_stream)->_ptr++ : _filbuf(_stream))
#define putc(_c,_stream)  (--(_stream)->_cnt >= 0 \
                ? 0xff & (*(_stream)->_ptr++ = (char)(_c)) :  _flsbuf((_c),(_stream)))
#define getc(_stream)    fgetc(_stream)
#define getchar()         getc(stdin)
#define putchar(_c)       putc((_c),stdout)
#endif


#define _fgetc_nolock(_stream)       (--(_stream)->_cnt >= 0 ? 0xff & *(_stream)->_ptr++ : _filbuf(_stream))
#define _fputc_nolock(_c,_stream)    (--(_stream)->_cnt >= 0 ? 0xff & (*(_stream)->_ptr++ = (char)(_c)) :  _flsbuf((_c),(_stream)))
#define _getc_nolock(_stream)       _fgetc_nolock(_stream)
#define _putc_nolock(_c, _stream)   _fputc_nolock(_c, _stream)
#define _getchar_nolock()           _getc_nolock(stdin)
#define _putchar_nolock(_c)         _putc_nolock((_c),stdout)
#define _getwchar_nolock()          _getwc_nolock(stdin)
#define _putwchar_nolock(_c)        _putwc_nolock((_c),stdout)

_CRTIMP void __cdecl _lock_file(__inout FILE * _P(_File));
_CRTIMP void __cdecl _unlock_file(__inout FILE * _P(_File));


_CRTIMP __checkReturn_opt int __cdecl _fclose_nolock(__inout FILE * _P(_File));
_CRTIMP __checkReturn_opt int __cdecl _fflush_nolock(__inout_opt FILE * _P(_File));
_CRTIMP __checkReturn_opt size_t __cdecl _fread_nolock(__out_bcount(_Size*_Count) void * _P(_DstBuf), __in size_t _P(_Size), __in size_t _P(_Count), __inout FILE * _P(_File));
_CRTIMP __checkReturn_opt int __cdecl _fseek_nolock(__inout FILE * _P(_File), __in long _P(_Offset), __in int _P(_Origin));
_CRTIMP __checkReturn long __cdecl _ftell_nolock(__inout FILE * _P(_File));
_CRTIMP __checkReturn_opt int __cdecl _fseeki64_nolock(__inout FILE * _P(_File), __in __int64 _P(_Offset), __in int _P(_Origin));
_CRTIMP __checkReturn __int64 __cdecl _ftelli64_nolock(__inout FILE * _P(_File));
_CRTIMP __checkReturn_opt size_t __cdecl _fwrite_nolock(__in_bcount(_Size*_Count) const void * _P(_DstBuf), __in size_t _P(_Size), __in size_t _P(_Count), __inout FILE * _P(_File));
_CRTIMP __checkReturn_opt int __cdecl _ungetc_nolock(__in int _P(_Ch), __inout FILE * _P(_File));

#if defined(_CRT_DISABLE_PERFCRITICAL_THREADLOCKING) && !defined(_DLL)
#define fclose(_stm)                            _fclose_nolock(_stm)
#define fflush(_stm)                            _fflush_nolock(_stm)
#define fread(_buf,_siz,_cnt,_stm)              _fread_nolock(_buf,_siz,_cnt,_stm)
#define fseek(_stm,_offset,_origin)             _fseek_nolock(_stm,_offset,_origin)
#define ftell(_stm)                             _ftell_nolock(_stm)
#define _fseeki64(_stm)                         _fseeki64_nolock(_stm)
#define _ftelli64(_stm)                         _ftelli64_nolock(_stm)
#define fwrite(_buf,_siz,_cnt,_stm)             _fwrite_nolock(_buf,_siz,_cnt,_stm)
#define ungetc(_c,_stm)                         _ungetc_nolock(_c,_stm)
#endif

#if     !__STDC__ && !defined(_POSIX_)

/* Non-ANSI names for compatibility */

#define P_tmpdir  _P_tmpdir
#define SYS_OPEN  _SYS_OPEN

#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
#pragma push_macro("tempnam")
#undef tempnam
#endif

_CRT_NONSTDC_DEPRECATE _CRTIMP char * __cdecl tempnam(__in_opt const char * _P(_Directory), __in_opt const char * _P(_FilePrefix));

#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
#pragma pop_macro("tempnam")
#endif

_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl fcloseall(void);
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn FILE * __cdecl fdopen(__in int _P(_FileHandle), __in __format_string const char * _P(_Format));
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl fgetchar(void);
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl fileno(__in FILE * _P(_File));
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl flushall(void);
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl fputchar(__in int _P(_Ch));
#pragma warning(push)
#pragma warning(disable: 4141) /* Using deprecated twice */ 
_CRT_NONSTDC_DEPRECATE _CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn int __cdecl getw(__inout FILE * _P(_File));
_CRT_NONSTDC_DEPRECATE _CRT_INSECURE_DEPRECATE _CRTIMP __checkReturn_opt int __cdecl putw(__in int _P(_Ch), __inout FILE * _P(_File));
#pragma warning(pop)
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl rmtmp(void);
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl unlink(__in const char * _P(_Filename));

#endif  /* __STDC__ */

#ifdef  __cplusplus
}
#endif

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif  /* _INC_STDIO */
