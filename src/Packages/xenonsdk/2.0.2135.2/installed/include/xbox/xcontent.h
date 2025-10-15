/*++

Copyright (c) 2002-2002  Microsoft Corporation

Module Name:

    xcontent.h

Abstract:

    This module defines the routines used to build content signatures

--*/

#ifndef _XCONTENT_
#define _XCONTENT_

#ifndef XCONTENTAPI
#define XCONTENTAPI __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

XCONTENTAPI
BOOL
WINAPI
XRegisterSignature(
    IN DWORD IndexNumber,
    IN DWORD Flags,
    IN LPVOID SignatureData,
    IN DWORD SignatureSize,
    IN LPCSTR FileName,
    IN DWORD FileOffset,
    IN DWORD DataLength
);
#define XSIG_FLAG_NAMED             0x00000001
#define XSIG_FLAG_AUTOMATIC         0x00000002

typedef BOOL (CALLBACK *CALCSIGPROC)(
    IN LPCSTR FileName,
    IN LPBYTE FileContents,
    IN DWORD FileSize
);

#ifdef __cplusplus
}
#endif

#endif // XCONTENT
