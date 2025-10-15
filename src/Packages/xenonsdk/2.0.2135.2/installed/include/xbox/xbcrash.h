/*++

Copyright (c) Microsoft Corporation

Module Name:

    xbcrash.h

Abstract:

    This module declares routines to read crash dump files.

--*/

#ifndef _XBCRASH_
#define _XBCRASH

#include <xboxdbg.h>

#ifndef XCRASHAPI
#define XCRASHAPI __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XCRASH_FILE         *PXCRASH_FILE;
typedef struct _XCRASH_WALK_MODSECT *PXCRASH_WALK_MODSECT;
typedef struct _XCRASH_WALK_MODULES *PXCRASH_WALK_MODULES;

XCRASHAPI
BOOL
WINAPI
XcrashOpenFile(
    IN LPCSTR szFileName,
    OUT PXCRASH_FILE *ppCrashFile
);

XCRASHAPI
VOID
WINAPI
XcrashCloseFile(
    IN PXCRASH_FILE pCrashFile
);

XCRASHAPI
BOOL
WINAPI
XcrashGetMemory(
    IN PXCRASH_FILE pCrashFile,
    IN LPCVOID lpAddrToRead,
    IN DWORD dwNumberOfBytesToRead,
    OUT LPVOID lpDataRead,
    OUT LPDWORD lpdwNumberOfBytesRead
);

XCRASHAPI
BOOL
WINAPI
XcrashGetThreadList(
    IN PXCRASH_FILE pCrashFile,
    IN LPDWORD lpdwThreadIdList,
    IN OUT LPDWORD lpdwNumberOfThreads
);

XCRASHAPI
BOOL
WINAPI
XcrashGetThreadContext(
    IN PXCRASH_FILE pCrashFile,
    IN DWORD dwThreadId,
    IN OUT PCONTEXT pThreadContext
);

XCRASHAPI
BOOL
WINAPI
XcrashGetThreadInfo(
  IN  PXCRASH_FILE pCrashFile,
  IN  DWORD dwThreadId, 
  OUT PDM_THREADINFO pDmThreadInfo
  );
  
XCRASHAPI
BOOL
WINAPI
XcrashIsThreadStopped(
  IN  PXCRASH_FILE pCrashFile,
  IN  DWORD dwThreadId, 
  OUT PBOOL pfStopped,
  OUT PDM_THREADSTOP pDmThreadStop
  );

XCRASHAPI
BOOL
WINAPI
XcrashWalkLoadedModules(
    IN     PXCRASH_FILE pCrashFile,
    IN OUT PXCRASH_WALK_MODULES *ppWalkModules,
    OUT    DMN_MODLOAD *pDmLoadedModule
);
    
    
XCRASHAPI
VOID
WINAPI
XcrashCloseLoadedModules(
    PXCRASH_WALK_MODULES pWalkModules
);

XCRASHAPI
BOOL
WINAPI
XcrashWalkModuleSections(
  IN     PXCRASH_FILE         pCrashFile,
  IN OUT PXCRASH_WALK_MODSECT *ppWalkModuleSections, 
  IN     LPCSTR               pszModuleName,
  OUT    PDMN_SECTIONLOAD     pSectionLoad
  );

XCRASHAPI
VOID
WINAPI
XcrashCloseModuleSections(
    PXCRASH_WALK_MODSECT pWalkModuleSections
    );

XCRASHAPI
BOOL
WINAPI
XcrashGetModuleLongName(
  IN     PXCRASH_FILE  pCrashFile,
  IN     LPCSTR        pszShortName,
  OUT    LPSTR         pszLongName,
  IN OUT LPDWORD       pcchLongName
  );
  

XCRASHAPI
BOOL
WINAPI
XcrashGetModuleFunctionTable(
  IN     PXCRASH_FILE  pCrashFile,
  IN     LPCSTR        pszShortName,
  OUT    LPDWORD       pdwFunctionTableAddress,
  OUT    LPDWORD       pdwFunctionTableSize
  );

XCRASHAPI
BOOL
WINAPI
XcrashCopySz(
  IN     PXCRASH_FILE pxcf,
  IN     LPCSTR pszStringVA,
  OUT    LPSTR  pszString,
  IN OUT PDWORD pcchLength
  );
  
XCRASHAPI
BOOL
WINAPI
XcrashGetXtlData(
  IN  PXCRASH_FILE pxcf,
  OUT PDM_XTLDATA pdmxd
  );

#ifdef __cplusplus
} //end of extern "C"
#endif

#endif // _XBCRASH_
