/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       xuiresource.h
 *  Content:    Xbox 360 UI resource defines, structures, and functions
 *
 ****************************************************************************/
 
#ifndef __XUIRESOURCE_H__
#define __XUIRESOURCE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XUIRESOURCE* HXUIRESOURCE;
typedef struct _XUIPACKAGE*  HXUIPACKAGE;
typedef enum XUI_SEEK_OPTIONS XUI_SEEK_OPTIONS;

enum XUI_SEEK_OPTIONS
{
    XUI_SEEK_FROM_START   = 0x1,
    XUI_SEEK_FROM_CURRENT = 0x2,
    XUI_SEEK_FROM_END     = 0x4
};


typedef struct IXuiResourceManager IXuiResourceManager;
typedef struct IXuiResourceReader IXuiResourceReader;

struct IXuiResourceReaderVTBL
{
    VOID                 (*AddRef)(IXuiResourceReader* pThis);
    UINT                 (*Release)(IXuiResourceReader* pThis);
    IXuiResourceManager* (*GetManager)(IXuiResourceReader* pThis);
    HRESULT              (*SetManager)(IXuiResourceReader* pThis, IXuiResourceManager* pManager);
    UINT                 (*GetTotalSize)(IXuiResourceReader* pThis);
    UINT                 (*GetRemainingBytes)(IXuiResourceReader* pThis);
    HRESULT              (*Read)(IXuiResourceReader* pThis, BYTE * pbDst, UINT cb, UINT * cbRead);
    HRESULT              (*Seek)(IXuiResourceReader* pThis, UINT fFlags, INT iOffset);
    HRESULT              (*GetBuffer)(IXuiResourceReader* pThis, BYTE **ppBuffer);
    HRESULT              (*ReleaseBuffer)(IXuiResourceReader* pThis);
};

struct IXuiResourceReader
{
#ifdef __cplusplus

    virtual VOID                  AddRef()=0;
    virtual UINT                  Release()=0;
    virtual IXuiResourceManager * GetManager()=0;
    virtual HRESULT               SetManager(IXuiResourceManager* pManager)=0;
    virtual UINT                  GetTotalSize()=0;
    virtual UINT                  GetRemainingBytes()=0;
    virtual HRESULT               Read(BYTE * pbDst, UINT cb, UINT * cbRead)=0;
    virtual HRESULT               Seek(UINT fFlags, INT iOffset)=0;
    virtual HRESULT               GetBuffer(BYTE **ppBuffer) = 0;
    virtual HRESULT               ReleaseBuffer() = 0;

#else
 
    struct IXuiResourceReaderVTBL * pVtbl;

#endif
};



struct IXuiResourceManagerVTBL
{
    VOID    (*AddRef)(IXuiResourceManager* pThis);
    UINT    (*Release)(IXuiResourceManager* pThis);
    LPCWSTR (*GetName)(IXuiResourceManager* pThis);
    BOOL    (*IsTransport)(IXuiResourceManager* pThis);
    HRESULT (*Init)(IXuiResourceManager* pThis, IXuiResourceReader * pBaseReader);
    HRESULT (*Open)(IXuiResourceManager* pThis, LPCWSTR szPath, IXuiResourceReader ** pReader);
    HRESULT (*GetFileAttributes)(IXuiResourceManager* pThis, LPCWSTR szPath, WIN32_FILE_ATTRIBUTE_DATA * pAttributes);
};

struct IXuiResourceManager
{
#ifdef __cplusplus

    virtual VOID    AddRef()=0;
    virtual UINT    Release()=0;
    virtual LPCWSTR GetName()=0;
    virtual BOOL    IsTransport()=0;
    virtual HRESULT Init(IXuiResourceReader * pBaseReader)=0;
    virtual HRESULT Open(LPCWSTR szPath, IXuiResourceReader ** pReader)=0; 
    virtual HRESULT GetFileAttributes(LPCWSTR szPath, WIN32_FILE_ATTRIBUTE_DATA * pAttributes) = 0;

#else
 
    struct IXuiResourceManagerVTBL * pVtbl;

#endif
};


BOOL    APIENTRY XuiResourceLocatorIsAbsolute(LPCWSTR szLocator);
BOOL    APIENTRY XuiResourceLocatorIsFile(LPCWSTR szLocator);
HRESULT APIENTRY XuiResourceComposeLocator(LPWSTR * pszComposedPath, LPCWSTR szBase, LPCWSTR szRelative);
HRESULT APIENTRY XuiResourceOpen(LPCWSTR szLocator, HXUIRESOURCE * phResource, BOOL *pbIsMemoryResource);
HRESULT APIENTRY XuiResourceOpenNoLoc(LPCWSTR szLocator, HXUIRESOURCE * phResource, BOOL *pbIsMemoryResource);
HRESULT APIENTRY XuiResourceGetBuffer(HXUIRESOURCE hResource, BYTE **ppBuffer);
HRESULT APIENTRY XuiResourceReleaseBuffer(HXUIRESOURCE hResource, BYTE *pBuffer);
HRESULT APIENTRY XuiResourceRead(HXUIRESOURCE hResource, BYTE * pbDest, UINT cb, UINT * pcbRead);
UINT    APIENTRY XuiResourceGetTotalSize(HXUIRESOURCE hResource);
HRESULT APIENTRY XuiResourceSeek(HXUIRESOURCE hResource, DWORD dwSeekOptions, INT iOffset);
VOID    APIENTRY XuiResourceClose(HXUIRESOURCE hResource);
HRESULT APIENTRY XuiResourceLoadAll(LPCWSTR szLocator, BYTE ** ppb, UINT * pcb);
HRESULT APIENTRY XuiResourceLoadAllNoLoc(LPCWSTR szLocator, BYTE ** ppb, UINT * pcb);
HRESULT APIENTRY XuiRegisterResourceTransport(IXuiResourceManager * pManager, IXuiResourceManager ** ppPreviousManager);
VOID    APIENTRY XuiUnregisterResourceTransport(LPCWSTR szTransport);

VOID    APIENTRY XuiResourcePackageExpiration(BOOL bEnable);
HRESULT APIENTRY XuiResourceOpenPackage(LPWSTR szLocator, HXUIPACKAGE *phPackage, BOOL fLocalizePath);
UINT    APIENTRY XuiResourceGetPackageEntryCount(HXUIPACKAGE hPackage);
HRESULT APIENTRY XuiResourceGetPackageEntryInfo(HXUIPACKAGE hPackage, UINT index, LPWSTR szLocator, UINT *pLocatorSize, 
                                                UINT *pFileOffset, UINT *pLength, FILETIME *pFileTime);
VOID    APIENTRY XuiResourceReleasePackage(HXUIPACKAGE hPackage);

#ifdef __cplusplus
}
#endif

#endif // __XUIRESOURCE_H__

