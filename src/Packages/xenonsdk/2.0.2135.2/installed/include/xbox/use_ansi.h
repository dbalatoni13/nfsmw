/***
*use_ansi.h - pragmas for ANSI Standard C++ libraries
*
*	Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This header is intended to force the use of the appropriate ANSI
*       Standard C++ libraries whenever it is included.
*
*       [Public]
*
****/


#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _USE_ANSI_CPP
#define _USE_ANSI_CPP

#undef _CRT_NOPRAGMA_LIBS

#ifndef _CRT_NOPRAGMA_LIBS

#if _XBOX_VER < 200

#if defined(_DLL) && !defined(_STATIC_CPPLIB)
#ifdef _DEBUG
#pragma comment(lib,"msvcprtd")
#else	// _DEBUG
#pragma comment(lib,"msvcprt")
#endif	// _DEBUG

#if !defined(_STL_NOFORCE_MANIFEST) && !defined(_VC_NODEFAULTLIB)
#if _MSC_FULL_VER >= 140040130
#include <crtassem.h>




#endif	// _MSC_FULL_VER

#endif	// !defined(_STL_NOFORCE_MANIFEST) && !defined(_VC_NODEFAULTLIB)

#else	// _DLL && !STATIC_CPPLIB
#ifdef _DEBUG
#pragma comment(lib,"libcpmtd")
#else	// _DEBUG
#pragma comment(lib,"libcpmt")
#endif	// _DEBUG
#endif	// _DLL && !STATIC_CPPLIB


#else // _XBOX_VER >= 200
#ifdef _DEBUG
#pragma comment(lib,"libcpmtd")
#else	// _DEBUG
#pragma comment(lib,"libcpmt")
#endif	// _DEBUG
#endif // _XBOX_VER >= 200

#endif  // _CRT_NOPRAGMA_LIBS

#endif	// _USE_ANSI_CPP
