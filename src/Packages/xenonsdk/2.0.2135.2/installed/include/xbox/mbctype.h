/***
*mbctype.h - MBCS character conversion macros
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Defines macros for MBCS character classification/conversion.
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_MBCTYPE
#define _INC_MBCTYPE

#include <crtdefs.h>
#include <ctype.h>

#ifdef  __cplusplus
extern "C" {
#endif


/*
 * MBCS - Multi-Byte Character Set
 */

/*
 * This declaration allows the user access the _mbctype[] look-up array.
 */
/* No data exports in pure code */
_CRTIMP extern unsigned char _mbctype[];
_CRTIMP extern unsigned char _mbcasemap[];


/* bit masks for MBCS character types */

#define _MS     0x01    /* MBCS single-byte symbol */
#define _MP     0x02    /* MBCS punct */
#define _M1     0x04    /* MBCS 1st (lead) byte */
#define _M2     0x08    /* MBCS 2nd byte*/

#define _SBUP   0x10    /* SBCS upper char */
#define _SBLOW  0x20    /* SBCS lower char */

/* byte types  */

#define _MBC_SINGLE     0       /* valid single byte char */
#define _MBC_LEAD       1       /* lead byte */
#define _MBC_TRAIL      2       /* trailing byte */
#define _MBC_ILLEGAL    (-1)    /* illegal byte */

#define _KANJI_CP   932

/* _setmbcp parameter defines */
#define _MB_CP_SBCS     0
#define _MB_CP_OEM      -2
#define _MB_CP_ANSI     -3
#define _MB_CP_LOCALE   -4


#ifndef _MBCTYPE_DEFINED

/* MB control routines */

_CRTIMP int __cdecl _setmbcp(__in int _P(_CodePage));
_CRTIMP int __cdecl _getmbcp(void);


/* MBCS character classification function prototypes */


/* byte routines */
_CRTIMP __checkReturn int __cdecl _ismbbkalnum( __in unsigned int _P(_C) );
_CRTIMP __checkReturn int __cdecl _ismbbkalnum_l(__in unsigned int _P(_C), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl _ismbbkana( __in unsigned int _P(_C) );
_CRTIMP __checkReturn int __cdecl _ismbbkana_l(__in unsigned int _P(_C), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl _ismbbkpunct( __in unsigned int _P(_C) );
_CRTIMP __checkReturn int __cdecl _ismbbkpunct_l(__in unsigned int _P(_C), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl _ismbbkprint( __in unsigned int _P(_C) );
_CRTIMP __checkReturn int __cdecl _ismbbkprint_l(__in unsigned int _P(_C), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl _ismbbalpha( __in unsigned int _P(_C) );
_CRTIMP __checkReturn int __cdecl _ismbbalpha_l(__in unsigned int _P(_C), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl _ismbbpunct( __in unsigned int _P(_C) );
_CRTIMP __checkReturn int __cdecl _ismbbpunct_l(__in unsigned int _P(_C), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl _ismbbalnum( __in unsigned int _P(_C) );
_CRTIMP __checkReturn int __cdecl _ismbbalnum_l(__in unsigned int _P(_C), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl _ismbbprint( __in unsigned int _P(_C) );
_CRTIMP __checkReturn int __cdecl _ismbbprint_l(__in unsigned int _P(_C), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl _ismbbgraph( __in unsigned int _P(_C) );
_CRTIMP __checkReturn int __cdecl _ismbbgraph_l(__in unsigned int _P(_C), __in_opt _locale_t _P(_PtLoci));

#ifndef _MBLEADTRAIL_DEFINED
_CRTIMP __checkReturn int __cdecl _ismbblead( __in unsigned int _P(_C));
_CRTIMP __checkReturn int __cdecl _ismbblead_l(__in unsigned int _P(_C), __in_opt _locale_t _P(_PtLoci) );
_CRTIMP __checkReturn int __cdecl _ismbbtrail( __in unsigned int _P(_C));
_CRTIMP __checkReturn int __cdecl _ismbbtrail_l(__in unsigned int _P(_C), __in_opt _locale_t _P(_PtLoci) );
_CRTIMP __checkReturn int __cdecl _ismbslead(__in_bcount(_Pos - _Str+1) const unsigned char * _P(_Str), __in const unsigned char * _P(_Pos));
_CRTIMP __checkReturn int __cdecl _ismbslead_l(__in_bcount(_Pos - _Str+1) const unsigned char * _P(_Str), __in const unsigned char * _P(_Pos), __in_opt _locale_t _P(_PtLoci));
_CRTIMP __checkReturn int __cdecl _ismbstrail(__in_bcount(_Pos - _Str+1) const unsigned char * _P(_Str), __in const unsigned char * _P(_Pos));
_CRTIMP __checkReturn int __cdecl _ismbstrail_l(__in_bcount(_Pos - _Str+1) const unsigned char * _P(_Str), __in const unsigned char * _P(_Pos), __in_opt _locale_t _P(_PtLoci));

#define _MBLEADTRAIL_DEFINED
#endif

#define _MBCTYPE_DEFINED
#endif

/*
 * char byte classification macros
 */

#if defined(_CRT_DISABLE_PERFCRITICAL_THREADLOCKING) && !defined(_DLL)
#define _ismbbkalnum(_c)    ((_mbctype+1)[(unsigned char)(_c)] & _MS)
#define _ismbbkprint(_c)    ((_mbctype+1)[(unsigned char)(_c)] & (_MS|_MP))
#define _ismbbkpunct(_c)    ((_mbctype+1)[(unsigned char)(_c)] & _MP)

#define _ismbbalnum(_c) (((_pctype)[(unsigned char)(_c)] & (_ALPHA|_DIGIT))||_ismbbkalnum(_c))
#define _ismbbalpha(_c) (((_pctype)[(unsigned char)(_c)] & (_ALPHA))||_ismbbkalnum(_c))
#define _ismbbgraph(_c) (((_pctype)[(unsigned char)(_c)] & (_PUNCT|_ALPHA|_DIGIT))||_ismbbkprint(_c))
#define _ismbbprint(_c) (((_pctype)[(unsigned char)(_c)] & (_BLANK|_PUNCT|_ALPHA|_DIGIT))||_ismbbkprint(_c))
#define _ismbbpunct(_c) (((_pctype)[(unsigned char)(_c)] & _PUNCT)||_ismbbkpunct(_c))

#define _ismbblead(_c)  ((_mbctype+1)[(unsigned char)(_c)] & _M1)
#define _ismbbtrail(_c) ((_mbctype+1)[(unsigned char)(_c)] & _M2)

#define _ismbbkana(_c)  ((_mbctype+1)[(unsigned char)(_c)] & (_MS|_MP))
#endif


#ifdef  __cplusplus
}
#endif

#endif  /* _INC_MBCTYPE */
