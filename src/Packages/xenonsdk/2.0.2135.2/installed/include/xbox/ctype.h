/***
*ctype.h - character conversion macros and ctype macros
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Defines macros for character classification/conversion.
*       [ANSI/System V]
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_CTYPE
#define _INC_CTYPE

#include <crtdefs.h>

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef WEOF
#define WEOF (wint_t)(0xFFFF)
#endif

#ifndef _CTYPE_DISABLE_MACROS
/* No data exports in pure code */
_CRTIMP extern const unsigned short _wctype[];
_CRTIMP const unsigned short * __cdecl __pctype_func(void);
_CRTIMP const wctype_t * __cdecl __pwctype_func(void);
/* No data exports in pure code */
_CRTIMP extern const unsigned short *_pctype;
_CRTIMP extern const wctype_t *_pwctype;
 #ifndef __PCTYPE_FUNC 
  #if defined(_CRT_DISABLE_PERFCRITICAL_THREADLOCKING) && !defined(_DLL)
   #define __PCTYPE_FUNC  _pctype
  #else
   #define __PCTYPE_FUNC  __pctype_func()
  #endif  /* _MT */
 #endif  /* __PCTYPE_FUNC */
#endif  /* _CTYPE_DISABLE_MACROS */

#define PCTYPE __pctype_func()
#define PWCTYPE __pwctype_func()



/* set bit masks for the possible character types */

#define _UPPER          0x1     /* upper case letter */
#define _LOWER          0x2     /* lower case letter */
#define _DIGIT          0x4     /* digit[0-9] */
#define _SPACE          0x8     /* tab, carriage return, newline, */
                                /* vertical tab or form feed */
#define _PUNCT          0x10    /* punctuation character */
#define _CONTROL        0x20    /* control character */
#define _BLANK          0x40    /* space char */
#define _HEX            0x80    /* hexadecimal digit */

#define _LEADBYTE       0x8000                  /* multibyte leadbyte */
#define _ALPHA          (0x0100|_UPPER|_LOWER)  /* alphabetic character */


/* character classification function prototypes */

#ifndef _CTYPE_DEFINED

_CRTIMP __checkReturn int __cdecl _isctype(__in int _P(_C), __in int _P(_Type));
_CRTIMP __checkReturn int __cdecl _isctype_l(__in int _P(_C), __in int _P(_Type), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl isalpha(__in int _P(_C));
_CRTIMP __checkReturn int __cdecl isupper(__in int _P(_C));
_CRTIMP __checkReturn int __cdecl islower(__in int _P(_C));
_CRTIMP __checkReturn int __cdecl isdigit(__in int _P(_C));
_CRTIMP __checkReturn int __cdecl isxdigit(__in int _P(_C));
_CRTIMP __checkReturn int __cdecl isspace(__in int _P(_C));
_CRTIMP __checkReturn int __cdecl ispunct(__in int _P(_C));
_CRTIMP __checkReturn int __cdecl isalnum(__in int _P(_C));
_CRTIMP __checkReturn int __cdecl isprint(__in int _P(_C));
_CRTIMP __checkReturn int __cdecl isgraph(__in int _P(_C));
_CRTIMP __checkReturn int __cdecl iscntrl(__in int _P(_C));
_CRTIMP __checkReturn int __cdecl toupper(__in int _P(_C));
_CRTIMP __checkReturn int __cdecl tolower(__in int _P(_C));
_CRTIMP __checkReturn int __cdecl _tolower(__in int _P(_C));
_CRTIMP __checkReturn int __cdecl _tolower_l(__in int _P(_C), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl _toupper(__in int _P(_C));
_CRTIMP __checkReturn int __cdecl _toupper_l(__in int _P(_C), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl __isascii(__in int _P(_C));
_CRTIMP __checkReturn int __cdecl __toascii(__in int _P(_C));
_CRTIMP __checkReturn int __cdecl __iscsymf(__in int _P(_C));
_CRTIMP __checkReturn int __cdecl __iscsym(__in int _P(_C));
#define _CTYPE_DEFINED
#endif

#ifndef _WCTYPE_DEFINED

/* wide function prototypes, also declared in wchar.h  */

/* character classification function prototypes */

_CRTIMP __checkReturn int __cdecl iswalpha(__in wint_t _P(_C));
_CRTIMP __checkReturn int __cdecl _iswalpha_l(__in wint_t _P(_C), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl iswupper(__in wint_t _P(_C));
_CRTIMP __checkReturn int __cdecl _iswupper_l(__in wint_t _P(_C), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl iswlower(__in wint_t _P(_C));
_CRTIMP __checkReturn int __cdecl _iswlower_l(__in wint_t _P(_C), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl iswdigit(__in wint_t _P(_C));
_CRTIMP __checkReturn int __cdecl _iswdigit_l(__in wint_t _P(_C), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl iswxdigit(__in wint_t _P(_C));
_CRTIMP __checkReturn int __cdecl _iswxdigit_l(__in wint_t _P(_C), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl iswspace(__in wint_t _P(_C));
_CRTIMP __checkReturn int __cdecl _iswspace_l(__in wint_t _P(_C), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl iswpunct(__in wint_t _P(_C));
_CRTIMP __checkReturn int __cdecl _iswpunct_l(__in wint_t _P(_C), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl iswalnum(__in wint_t _P(_C));
_CRTIMP __checkReturn int __cdecl _iswalnum_l(__in wint_t _P(_C), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl iswprint(__in wint_t _P(_C));
_CRTIMP __checkReturn int __cdecl _iswprint_l(__in wint_t _P(_C), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl iswgraph(__in wint_t _P(_C));
_CRTIMP __checkReturn int __cdecl _iswgraph_l(__in wint_t _P(_C), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl iswcntrl(__in wint_t _P(_C));
_CRTIMP __checkReturn int __cdecl _iswcntrl_l(__in wint_t _P(_C), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl iswascii(__in wint_t _P(_C));
_CRTIMP __checkReturn int __cdecl isleadbyte(__in int _P(_C));
_CRTIMP __checkReturn int __cdecl _isleadbyte_l(__in int _P(_C), __in_opt _locale_t _P(_PtLoci));

_CRTIMP __checkReturn wint_t __cdecl towupper(__in wint_t _P(_C));
_CRTIMP __checkReturn wint_t __cdecl _towupper_l(__in wint_t _P(_C), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn wint_t __cdecl towlower(__in wint_t _P(_C));
_CRTIMP __checkReturn wint_t __cdecl _towlower_l(__in wint_t _P(_C), __in_opt _locale_t _P(_PtLoci)); 
_CRTIMP __checkReturn int __cdecl iswctype(__in wint_t _P(_C), __in wctype_t _P(_Type));
_CRTIMP __checkReturn int __cdecl _iswctype_l(__in wint_t _P(_C), __in wctype_t _P(_Type), __in_opt _locale_t _P(_PtLoci));

_CRTIMP __checkReturn int __cdecl __iswcsymf(__in wint_t _P(_C));
_CRTIMP __checkReturn int __cdecl _iswcsymf_l(__in wint_t _P(_C), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl __iswcsym(__in wint_t _P(_C));
_CRTIMP __checkReturn int __cdecl _iswcsym_l(__in wint_t _P(_C), __in_opt _locale_t _P(_PtLoci));

#if defined(_XBOX) && !defined(_XBOX_CRT_DEPRECATE_IS_WCTYPE) && !defined(_CRT_IS_WCTYPE_NO_DEPRECATE)
#define _CRT_IS_WCTYPE_NO_DEPRECATE
#endif

#ifndef _CRT_IS_WCTYPE_NO_DEPRECATE
#define _CRT_IS_WCTYPE_DEPRECATE __declspec(deprecated)
#else
#define _CRT_IS_WCTYPE_DEPRECATE
#endif

_CRT_IS_WCTYPE_DEPRECATE _CRTIMP int __cdecl is_wctype(__in wint_t _P(_C), __in wctype_t _P(_Type));

#define _WCTYPE_DEFINED
#endif

/* the character classification macro definitions */

#ifndef _CTYPE_DISABLE_MACROS

/*
 * Maximum number of bytes in multi-byte character in the current locale
 * (also defined in stdlib.h).
 */
#ifndef MB_CUR_MAX

#define MB_CUR_MAX (1)
/* No data exports in pure code */
_CRTIMP extern int __mb_cur_max;
_CRTIMP int __cdecl ___mb_cur_max_func(void);


#endif  /* MB_CUR_MAX */

/* Introduced to detect error when character testing functions are called
 * with illegal input of integer.
 */
#ifdef _DEBUG
_CRTIMP int __cdecl _chvalidator(__in int _P(_Ch), __in int _P(_Mask));
#define __chvalidchk(a,b)       _chvalidator(a,b)
#else
#define __chvalidchk(a,b)       (__PCTYPE_FUNC[(a)] & (b))
#endif



#ifndef __cplusplus
#define isalpha(_c)     (MB_CUR_MAX > 1 ? _isctype(_c,_ALPHA) : __chvalidchk(_c, _ALPHA))
#define isupper(_c)     (MB_CUR_MAX > 1 ? _isctype(_c,_UPPER) : __chvalidchk(_c, _UPPER))
#define islower(_c)     (MB_CUR_MAX > 1 ? _isctype(_c,_LOWER) : __chvalidchk(_c, _LOWER))
#define isdigit(_c)     (MB_CUR_MAX > 1 ? _isctype(_c,_DIGIT) : __chvalidchk(_c, _DIGIT))
#define isxdigit(_c)    (MB_CUR_MAX > 1 ? _isctype(_c,_HEX)   : __chvalidchk(_c, _HEX))
#define isspace(_c)     (MB_CUR_MAX > 1 ? _isctype(_c,_SPACE) : __chvalidchk(_c, _SPACE))
#define ispunct(_c)     (MB_CUR_MAX > 1 ? _isctype(_c,_PUNCT) : __chvalidchk(_c, _PUNCT))
#define isalnum(_c)     (MB_CUR_MAX > 1 ? _isctype(_c,_ALPHA|_DIGIT) : __chvalidchk(_c, (_ALPHA|_DIGIT)))
#define isprint(_c)     (MB_CUR_MAX > 1 ? _isctype(_c,_BLANK|_PUNCT|_ALPHA|_DIGIT) : __chvalidchk(_c, (_BLANK|_PUNCT|_ALPHA|_DIGIT)))
#define isgraph(_c)     (MB_CUR_MAX > 1 ? _isctype(_c,_PUNCT|_ALPHA|_DIGIT) : __chvalidchk(_c, (_PUNCT|_ALPHA|_DIGIT)))
#define iscntrl(_c)     (MB_CUR_MAX > 1 ? _isctype(_c,_CONTROL) : __chvalidchk(_c, _CONTROL))
#elif   0         /* Pending ANSI C++ integration */
inline __checkReturn int isalpha(__in int _C)
        {return (MB_CUR_MAX > 1 ? _isctype(_C,_ALPHA) : __chvalidchk(_C, _ALPHA)); }
inline __checkReturn int isupper(__in int _C)
        {return (MB_CUR_MAX > 1 ? _isctype(_C,_UPPER) : __chvalidchk(_C, _UPPER)); }
inline __checkReturn int islower(__in int _C)
        {return (MB_CUR_MAX > 1 ? _isctype(_C,_LOWER) : __chvalidchk(_C, _LOWER)); }
inline __checkReturn int isdigit(__in int _C)
        {return (MB_CUR_MAX > 1 ? _isctype(_C,_DIGIT) : __chvalidchk(_C, _DIGIT)); }
inline __checkReturn int isxdigit(__in int _C)
        {return (MB_CUR_MAX > 1 ? _isctype(_C,_HEX)   : __chvalidchk(_C, _HEX)); }
inline __checkReturn int isspace(__in int _C)
        {return (MB_CUR_MAX > 1 ? _isctype(_C,_SPACE) : __chvalidchk(_C, _SPACE)); }
inline __checkReturn int ispunct(__in int _C)
        {return (MB_CUR_MAX > 1 ? _isctype(_C,_PUNCT) : __chvalidchk(_C, _PUNCT)); }
inline __checkReturn int isalnum(__in int _C)
        {return (MB_CUR_MAX > 1 ? _isctype(_C,_ALPHA|_DIGIT)
                : __chvalidchk(_C) , (_ALPHA|_DIGIT)); }
inline __checkReturn int isprint(__in int _C)
        {return (MB_CUR_MAX > 1 ? _isctype(_C,_BLANK|_PUNCT|_ALPHA|_DIGIT)
                : __chvalidchk(_C , (_BLANK|_PUNCT|_ALPHA|_DIGIT))); }
inline __checkReturn int isgraph(__in int _C)
        {return (MB_CUR_MAX > 1 ? _isctype(_C,_PUNCT|_ALPHA|_DIGIT)
                : __chvalidchk(_C , (_PUNCT|_ALPHA|_DIGIT))); }
inline __checkReturn int iscntrl(__in int _C)
        {return (MB_CUR_MAX > 1 ? _isctype(_C,_CONTROL)
                : __chvalidchk(_C , _CONTROL)); }
#endif  /* __cplusplus */

#ifdef _DEBUG
int __cdecl _chvalidator_l(__in_opt _locale_t, __in int _P(_Ch), __in int _P(_Mask));
#define _chvalidchk_l(_Char, _Flag, _Locale)  _chvalidator_l(_Locale, _Char, _Flag)
#else
#define _chvalidchk_l(_Char, _Flag, _Locale)  (_Locale==NULL ? __chvalidchk(_Char, _Flag) : ((_locale_t)_Locale)->locinfo->pctype[_Char] & (_Flag))
#endif  /* DEBUG */


#define _ischartype_l(_Char, _Flag, _Locale)    ( ((_Locale)!=NULL && (((_locale_t)(_Locale))->locinfo->mb_cur_max) > 1) ? _isctype_l(_Char, (_Flag), _Locale) : _chvalidchk_l(_Char,_Flag,_Locale))
#define _isalpha_l(_Char, _Locale)      _ischartype_l(_Char, _ALPHA, _Locale)
#define _isupper_l(_Char, _Locale)      _ischartype_l(_Char, _UPPER, _Locale)
#define _islower_l(_Char, _Locale)      _ischartype_l(_Char, _LOWER, _Locale)
#define _isdigit_l(_Char, _Locale)      _ischartype_l(_Char, _DIGIT, _Locale)
#define _isxdigit_l(_Char, _Locale)     _ischartype_l(_Char, _HEX, _Locale)
#define _isspace_l(_Char, _Locale)      _ischartype_l(_Char, _SPACE, _Locale)
#define _ispunct_l(_Char, _Locale)      _ischartype_l(_Char, _PUNCT, _Locale)
#define _isalnum_l(_Char, _Locale)      _ischartype_l(_Char, _ALPHA|_DIGIT, _Locale)
#define _isprint_l(_Char, _Locale)      _ischartype_l(_Char, _BLANK|_PUNCT|_ALPHA|_DIGIT, _Locale)
#define _isgraph_l(_Char, _Locale)      _ischartype_l(_Char, _PUNCT|_ALPHA|_DIGIT, _Locale)
#define _iscntrl_l(_Char, _Locale)      _ischartype_l(_Char, _CONTROL, _Locale)

#define _tolower(_Char)    ( (_Char)-'A'+'a' )
#define _toupper(_Char)    ( (_Char)-'a'+'A' )

#define __isascii(_Char)   ( (unsigned)(_Char) < 0x80 )
#define __toascii(_Char)   ( (_Char) & 0x7f )

#ifndef _WCTYPE_INLINE_DEFINED

#undef _CRT_WCTYPE_NOINLINE

#if !defined(__cplusplus) || defined(_M_CEE_PURE) || defined(MRTDLL) || defined(_CRT_WCTYPE_NOINLINE)
#define iswalpha(_c)    ( iswctype(_c,_ALPHA) )
#define iswupper(_c)    ( iswctype(_c,_UPPER) )
#define iswlower(_c)    ( iswctype(_c,_LOWER) )
#define iswdigit(_c)    ( iswctype(_c,_DIGIT) )
#define iswxdigit(_c)   ( iswctype(_c,_HEX) )
#define iswspace(_c)    ( iswctype(_c,_SPACE) )
#define iswpunct(_c)    ( iswctype(_c,_PUNCT) )
#define iswalnum(_c)    ( iswctype(_c,_ALPHA|_DIGIT) )
#define iswprint(_c)    ( iswctype(_c,_BLANK|_PUNCT|_ALPHA|_DIGIT) )
#define iswgraph(_c)    ( iswctype(_c,_PUNCT|_ALPHA|_DIGIT) )
#define iswcntrl(_c)    ( iswctype(_c,_CONTROL) )
#define iswascii(_c)    ( (unsigned)(_c) < 0x80 )

#define _iswalpha_l(_c,_p)    ( _iswctype_l(_c,_ALPHA, _p) )
#define _iswupper_l(_c,_p)    ( _iswctype_l(_c,_UPPER, _p) )
#define _iswlower_l(_c,_p)    ( _iswctype_l(_c,_LOWER, _p) )
#define _iswdigit_l(_c,_p)    ( _iswctype_l(_c,_DIGIT, _p) )
#define _iswxdigit_l(_c,_p)   ( _iswctype_l(_c,_HEX, _p) )
#define _iswspace_l(_c,_p)    ( _iswctype_l(_c,_SPACE, _p) )
#define _iswpunct_l(_c,_p)    ( _iswctype_l(_c,_PUNCT, _p) )
#define _iswalnum_l(_c,_p)    ( _iswctype_l(_c,_ALPHA|_DIGIT, _p) )
#define _iswprint_l(_c,_p)    ( _iswctype_l(_c,_BLANK|_PUNCT|_ALPHA|_DIGIT, _p) )
#define _iswgraph_l(_c,_p)    ( _iswctype_l(_c,_PUNCT|_ALPHA|_DIGIT, _p) )
#define _iswcntrl_l(_c,_p)    ( _iswctype_l(_c,_CONTROL, _p) )
#elif   0         /* __cplusplus */
inline __checkReturn int iswalpha(__in wint_t _C) {return (iswctype(_C,_ALPHA)); }
inline __checkReturn int iswupper(__in wint_t _C) {return (iswctype(_C,_UPPER)); }
inline __checkReturn int iswlower(__in wint_t _C) {return (iswctype(_C,_LOWER)); }
inline __checkReturn int iswdigit(__in wint_t _C) {return (iswctype(_C,_DIGIT)); }
inline __checkReturn int iswxdigit(__in wint_t _C) {return (iswctype(_C,_HEX)); }
inline __checkReturn int iswspace(__in wint_t _C) {return (iswctype(_C,_SPACE)); }
inline __checkReturn int iswpunct(__in wint_t _C) {return (iswctype(_C,_PUNCT)); }
inline __checkReturn int iswalnum(__in wint_t _C) {return (iswctype(_C,_ALPHA|_DIGIT)); }
inline __checkReturn int iswprint(__in wint_t _C) {return (iswctype(_C,_BLANK|_PUNCT|_ALPHA|_DIGIT)); }
inline __checkReturn int iswgraph(__in wint_t _C) {return (iswctype(_C,_PUNCT|_ALPHA|_DIGIT)); }
inline __checkReturn int iswcntrl(__in wint_t _C) {return (iswctype(_C,_CONTROL)); }
inline __checkReturn int iswascii(__in wint_t _C) {return ((unsigned)(_C) < 0x80); }

inline __checkReturn int __CRTDECL _iswalpha_l(__in wint_t _C, __in_opt _locale_t _PtLoci) {return (_iswctype_l(_C,_ALPHA, _PtLoci)); }
inline __checkReturn int __CRTDECL _iswupper_l(__in wint_t _C, __in_opt _locale_t _PtLoci) {return (_iswctype_l(_C,_UPPER, _PtLoci)); }
inline __checkReturn int __CRTDECL _iswlower_l(__in wint_t _C, __in_opt _locale_t _PtLoci) {return (_iswctype_l(_C,_LOWER, _PtLoci)); }
inline __checkReturn int __CRTDECL _iswdigit_l(__in wint_t _C, __in_opt _locale_t _PtLoci) {return (_iswctype_l(_C,_DIGIT, _PtLoci)); }
inline __checkReturn int __CRTDECL _iswxdigit_l(__in wint_t _C, __in_opt _locale_t _PtLoci) {return(_iswctype_l(_C,_HEX, _PtLoci)); }
inline __checkReturn int __CRTDECL _iswspace_l(__in wint_t _C, __in_opt _locale_t _PtLoci) {return (_iswctype_l(_C,_SPACE, _PtLoci)); }
inline __checkReturn int __CRTDECL _iswpunct_l(__in wint_t _C, __in_opt _locale_t _PtLoci) {return (_iswctype_l(_C,_PUNCT, _PtLoci)); }
inline __checkReturn int __CRTDECL _iswalnum_l(__in wint_t _C, __in_opt _locale_t _PtLoci) {return (_iswctype_l(_C,_ALPHA|_DIGIT, _PtLoci)); }
inline __checkReturn int __CRTDECL _iswprint_l(__in wint_t _C, __in_opt _locale_t _PtLoci) {return (_iswctype_l(_C,_BLANK|_PUNCT|_ALPHA|_DIGIT, _PtLoci)); }
inline __checkReturn int __CRTDECL _iswgraph_l(__in wint_t _C, __in_opt _locale_t _PtLoci) {return (_iswctype_l(_C,_PUNCT|_ALPHA|_DIGIT, _PtLoci)); }
inline __checkReturn int __CRTDECL _iswcntrl_l(__in wint_t _C, __in_opt _locale_t _PtLoci) {return (_iswctype_l(_C,_CONTROL, _PtLoci)); }

inline __checkReturn int isleadbyte(int _C) {return (__PCTYPE_FUNC[(unsigned char)(_C)] & _LEADBYTE); }
#endif  /* __cplusplus */
#define _WCTYPE_INLINE_DEFINED
#endif  /* _WCTYPE_INLINE_DEFINED */

/* MS C version 2.0 extended ctype macros */

#define __iscsymf(_c)   (isalpha(_c) || ((_c) == '_'))
#define __iscsym(_c)    (isalnum(_c) || ((_c) == '_'))
#define __iswcsymf(_c)  (iswalpha(_c) || ((_c) == '_'))
#define __iswcsym(_c)   (iswalnum(_c) || ((_c) == '_'))

#define _iscsymf_l(_c, _p)   (_isalpha_l(_c, _p) || ((_c) == '_'))
#define _iscsym_l(_c, _p)    (_isalnum_l(_c, _p) || ((_c) == '_'))
#define _iswcsymf_l(_c, _p)  (_iswalpha_l(_c, _p) || ((_c) == '_'))
#define _iswcsym_l(_c, _p)   (_iswalnum_l(_c, _p) || ((_c) == '_'))

#endif  /* _CTYPE_DISABLE_MACROS */


#if     !__STDC__

/* Non-ANSI names for compatibility */

#ifndef _CTYPE_DEFINED
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl isascii(__in int _P(_C));
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl toascii(__in int _P(_C));
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl iscsymf(__in int _P(_C));
_CRT_NONSTDC_DEPRECATE _CRTIMP __checkReturn int __cdecl iscsym(__in int _P(_C));
#else
#define isascii __isascii
#define toascii __toascii
#define iscsymf __iscsymf
#define iscsym  __iscsym
#endif

#endif  /* __STDC__ */

#ifdef  __cplusplus
}
#endif

#endif  /* _INC_CTYPE */
