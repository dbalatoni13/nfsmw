/***
*typeinfo.h - Defines the type_info structure and exceptions used for RTTI
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Defines the type_info structure and exceptions used for
*       Runtime Type Identification.
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#include <crtdefs.h>

#ifndef _INC_TYPEINFO
#define _INC_TYPEINFO

#ifdef  _MSC_VER
#pragma pack(push,_CRT_PACKING)
#endif  /* _MSC_VER */

#ifndef RC_INVOKED

#ifndef __cplusplus
#error This header requires a C++ compiler ...
#endif


#include <typeinfo>


#ifdef  __RTTI_OLDNAMES
/* Some synonyms for folks using older standard */
using std::bad_cast;
using std::bad_typeid;

typedef type_info Type_info;
typedef bad_cast Bad_cast;
typedef bad_typeid Bad_typeid;
#endif  /* __RTTI_OLDNAMES */



#endif /* RC_INVOKED */

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif  /* _INC_TYPEINFO */
