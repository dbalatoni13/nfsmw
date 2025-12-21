/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.7
 */
/*
 *                      Emotion Engine Library
 *                          Version 0.01
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         eetypes.h
 *                         develop library
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.1.0
 *       0.3.0		June.05		horikawa    required of M.W.
 */

#ifndef _eetypes_h_
#define _eetypes_h_

#ifndef	_SYS_TYPES_H
typedef unsigned char		u_char;
typedef unsigned short		u_short;
typedef unsigned int		u_int;
typedef unsigned long		u_long;
#endif
#ifdef	__GNUC__
typedef int long128 __attribute__ ((mode (TI)));
typedef unsigned int u_long128 __attribute__ ((mode (TI)));
#endif

#ifndef FALSE
#define	FALSE	0
#endif

#ifndef TRUE
#define	TRUE	(!FALSE)
#endif

#ifndef NULL
#define	NULL	0
#endif

#endif /* _eetypes_h_ */
