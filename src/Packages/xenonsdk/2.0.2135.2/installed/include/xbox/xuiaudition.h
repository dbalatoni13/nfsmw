/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       xuiaudition.h
 *  Content:    Xbox 360 UI auditioning structures and functions
 *
 ****************************************************************************/

#ifndef __XUIAUDITION_H__
#define __XUIAUDITION_H__

#ifdef __cplusplus
extern "C" {
#endif

//
// Audition Host functions should return E_NOTIMPL where appropriate 
// if they choose not to provide any override the default behavior.
//

typedef struct IXUIAuditionHost IXUIAuditionHost;
typedef struct IXUIAuditionHostVTBL IXUIAuditionHostVTBL;

struct IXUIAuditionHostVTBL
{   
    HRESULT (*ProcessRemoteInput)(IXUIAuditionHost* pThis, const XINPUT_KEYSTROKE * pKeystroke);
    HRESULT (*ChangeLocale)(IXUIAuditionHost* pThis);
    HRESULT (*ChangeSkin)(IXUIAuditionHost* pThis, LPCWSTR szSkinResource, LPCWSTR szPrefix);
    HRESULT (*ReloadResources)(IXUIAuditionHost* pThis);
};

struct IXUIAuditionHost
{
#ifdef __cplusplus    
       
    virtual HRESULT ProcessRemoteInput(const XINPUT_KEYSTROKE * pKeystroke) = 0;
    virtual HRESULT ChangeLocale() = 0;
    virtual HRESULT ChangeSkin(LPCWSTR szSkinResource, LPCWSTR szPrefix) = 0;
    virtual HRESULT ReloadResources() = 0;

#else
    
    struct IXUIAuditionHostVTBL * pVtbl;

#endif
};

//
// Auditioning API
//

HRESULT APIENTRY XuiAuditioningInit(IXUIAuditionHost * pHost);
HRESULT APIENTRY XuiAuditioningProcess();
HRESULT APIENTRY XuiAuditioningEnableCaching(BOOL fEnable);
HRESULT APIENTRY XuiAuditionLogMessage(HRESULT hr, LPCWSTR szMessage);
VOID    APIENTRY XuiAuditioningUninit();

#ifdef __cplusplus
}
#endif

#endif // __XUIAUDITION_H__

