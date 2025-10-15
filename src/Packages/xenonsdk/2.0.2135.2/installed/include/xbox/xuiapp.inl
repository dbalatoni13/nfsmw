
/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       xuiapp.inl
 *  Content:    Xbox 360 UI runtime C++ helpers
 *
 ****************************************************************************/

#define XUI_INLINE inline

////////////////////////////////////////////////////////////////////////////////
// CXuiAnimRunner
////////////////////////////////////////////////////////////////////////////////
XUI_INLINE CXuiAnimRunner::CXuiAnimRunner()
{
    Reset();
}

XUI_INLINE void CXuiAnimRunner::Reset()
{
    QueryPerformanceFrequency(&m_liTicksPerSecond);
    QueryPerformanceCounter(&m_liLastFrameTime);
}

XUI_INLINE void CXuiAnimRunner::Resume()
{
    QueryPerformanceCounter(&m_liLastFrameTime);
}

// This function has been deprecated.  Calling Run() with no parameters
// will run animations using the delta time from the last frame
// If you have your own timer, then you can call XuiAnimRun with the delta time
// between frames
XUI_INLINE void __declspec(deprecated) CXuiAnimRunner::Run(DWORD dwTime)
{
    Run();
}

XUI_INLINE void CXuiAnimRunner::Run()
{
    XuiAnimRun(GetDeltaTime());
}

XUI_INLINE float CXuiAnimRunner::GetDeltaTime()
{
    LARGE_INTEGER liCurrFrameTime;
    QueryPerformanceCounter(&liCurrFrameTime);
    unsigned __int64 nDeltaTime = ((unsigned __int64) liCurrFrameTime.QuadPart) - ((unsigned __int64) m_liLastFrameTime.QuadPart);

    float fDeltaTime = (float) ((((double) nDeltaTime) / ((double) m_liTicksPerSecond.QuadPart))*1000.0);
    m_liLastFrameTime = liCurrFrameTime;
    return fDeltaTime;

}

XUI_INLINE void CXuiAnimRunner::GetLastFrameTime(LARGE_INTEGER *pliLastFrameTime)
{
    *pliLastFrameTime = m_liLastFrameTime;
}


////////////////////////////////////////////////////////////////////////////////
// CXuiModule
////////////////////////////////////////////////////////////////////////////////
XUI_INLINE CXuiModule::CXuiModule()
{
    m_hObjRoot = NULL;
    m_hDC = NULL;
    m_bXuiInited = FALSE;
    m_bInitRuntime = TRUE;
    m_bQuit = FALSE;
    m_bSkinLoaded = FALSE;
    m_bDefaultTypefaceRegistered = FALSE;
}

XUI_INLINE CXuiModule::~CXuiModule()
{
    // make sure we haven't been constructed yet
    // or Uninit was called
    // it's OK if the DC is not reset when m_bInitRuntime is FALSE, it means
    // that this DC is shared among many CXuiModule
    ASSERT((m_bXuiInited == FALSE) && (m_hObjRoot == NULL) && ((m_hDC == NULL) || (m_bInitRuntime == FALSE)));
}


XUI_INLINE HRESULT CXuiModule::Init()
{
    // This version of Init is deprecated.
    // In order to reduce dependence on D3DX, XUI clients should implement their own
    // texture loader function and register it with the new version of this member function.
    return Init(XuiD3DXTextureLoader);
}

XUI_INLINE HRESULT CXuiModule::Init(PFN_XUITEXTURELOADER pfnTextureLoader, XUIInitParams *pInitParams/*=NULL*/)
{
    HRESULT hr;
    if (m_bInitRuntime)
    {
        hr = XuiRenderInit(NULL, pfnTextureLoader);
        if (FAILED(hr))
            return hr;
    }

    hr = InitCommon(pInitParams);
    if (FAILED(hr) && m_bInitRuntime)
        XuiRenderUninit();
    return hr;
}

XUI_INLINE HRESULT CXuiModule::InitShared(IDirect3DDevice9 *pDevice, const D3DPRESENT_PARAMETERS *pPresentParams)
{
    // This version of InitShared is deprecated.
    // In order to reduce dependence on D3DX, XUI clients should implement their own
    // texture loader function and register it with the new version of this member function.
    return InitShared(pDevice, pPresentParams, XuiD3DXTextureLoader);
}

XUI_INLINE HRESULT CXuiModule::InitShared(IDirect3DDevice9 *pDevice, const D3DPRESENT_PARAMETERS *pPresentParams, PFN_XUITEXTURELOADER pfnTextureLoader, XUIInitParams *pInitParams/*=NULL*/)
{
    HRESULT hr = XuiRenderInitShared(pDevice, pPresentParams, pfnTextureLoader);
    if (FAILED(hr))
        return hr;
    hr = InitCommon(pInitParams);
    if (FAILED(hr))
        XuiRenderUninit();
    return hr;
}

XUI_INLINE HRESULT CXuiModule::InitCommon(XUIInitParams *pInitParams/*=NULL*/)
{
    HRESULT hr;

    if (m_bInitRuntime)
    {
        hr = XuiRenderCreateDC(&m_hDC);
        if (FAILED(hr))
            return hr;

        XUIInitParams initparams;
        
        if (pInitParams == NULL)
        {
            XUI_INIT_PARAMS(initparams);
            pInitParams = &initparams;
        }
        
        hr = XuiInit(pInitParams);
        if (FAILED(hr))
        {
            XuiRenderDestroyDC(m_hDC);
            m_hDC = NULL;
            return hr;
        }
    }
    m_bXuiInited = TRUE;

#ifdef XUIAPP_AUDITIONING
    XuiAuditioningInit(this);
#endif

    hr = RegisterXuiClasses();
    if (FAILED(hr))
    {
        Uninit();
        return hr;
    }

    // create the main canvas
    hr = CreateMainCanvas();
    if (FAILED(hr))
    {
        Uninit();
        return hr;
    }
    return S_OK;
}

XUI_INLINE void CXuiModule::Uninit()
{
    if (m_hObjRoot)
    {
        XuiDestroyObject(m_hObjRoot);
        m_hObjRoot = NULL;
    }

    UnregisterXuiClasses();

#ifdef XUIAPP_AUDITIONING
    XuiAuditioningUninit();
#endif

    if (m_bInitRuntime)
    {
        if (m_hDC)
        {
            XuiRenderDestroyDC(m_hDC);
            m_hDC = NULL;
        }

        if (m_bXuiInited)
        {
            XuiUninit();
        }

        XuiRenderUninit();
    }
    m_bXuiInited = FALSE;
}

XUI_INLINE int CXuiModule::Run()
{
    while (!m_bQuit)
    {
        RunFrame();
        Render();
        XuiTimersRun();
    }
    return 0;
}

XUI_INLINE void CXuiModule::Resume()
{
    m_animRunner.Resume();
}

XUI_INLINE void CXuiModule::Quit()
{
    m_bQuit = TRUE;
}

XUI_INLINE HXUIDC CXuiModule::GetDC()
{
    return m_hDC;
}

XUI_INLINE HXUIOBJ CXuiModule::GetRootObj()
{
    return m_hObjRoot;
}

//
// RunFrame:
// This function runs animations based on the internal timer
// to use your own timer, call RunFrame(float fDeltaTime)
//
XUI_INLINE void CXuiModule::RunFrame()
{
    RunFrame(m_animRunner.GetDeltaTime());
}

XUI_INLINE void CXuiModule::RunFrame(float fDeltaTime)
{
#ifdef XUIAPP_AUDITIONING
    // Do async auditioning work
    XuiAuditioningProcess();
#endif

    //
    // Check controller input
    //
    ProcessInput();

    // Run animations
    XuiAnimRun(fDeltaTime);
}


XUI_INLINE HRESULT CXuiModule::ProcessInput()
{
#ifdef _XBOX
    XINPUT_KEYSTROKE keyStroke;
    if (ERROR_SUCCESS == XInputGetKeystroke(XUSER_INDEX_ANY, XINPUT_FLAG_ANYDEVICE,
        &keyStroke))
    {
        XuiProcessInput(&keyStroke);
    }
#endif
    return S_OK;
}

XUI_INLINE HRESULT CXuiModule::Render()
{
    ASSERT(m_hDC != NULL);
    HRESULT hr = XuiRenderBegin(m_hDC, D3DCOLOR_ARGB(255, 0, 0, 0));
    if (FAILED(hr))
        return hr;

    XUIMessage msg;
    XUIMessageRender msgRender;
    XuiMessageRender(&msg, &msgRender, m_hDC, 0xffffffff, XUI_BLEND_NORMAL);
    XuiSendMessage(m_hObjRoot, &msg);

    XuiRenderEnd(m_hDC);
    XuiRenderPresent(m_hDC, NULL, NULL, NULL);
    return S_OK;
}

XUI_INLINE HRESULT CXuiModule::RegisterDefaultTypeface(LPCWSTR szTypeface, LPCWSTR szLocator, LPCWSTR szReserved1/*=NULL*/)
{
    TypefaceDescriptor typeface = {szTypeface, szLocator, szReserved1};
    HRESULT hr = XuiRegisterTypeface(&typeface, TRUE);
    if (FAILED(hr))
        return hr;
    m_bDefaultTypefaceRegistered = TRUE;
    return S_OK;
}

XUI_INLINE HRESULT CXuiModule::RegisterTypeface(LPCWSTR szTypeface, LPCWSTR szLocator, LPCWSTR szReserved1/*=NULL*/)
{
    TypefaceDescriptor typeface = {szTypeface, szLocator, szReserved1};
    return XuiRegisterTypeface(&typeface, FALSE);
}

XUI_INLINE HRESULT CXuiModule::LoadSkin(LPCWSTR szSkinFileName, LPCWSTR szVisualPrefix/*=NULL*/)
{
    HRESULT hr = XuiLoadVisualFromBinary(szSkinFileName, szVisualPrefix);
    if (FAILED(hr))
        return hr;
    m_bSkinLoaded = TRUE;
    return S_OK;
}

XUI_INLINE HRESULT CXuiModule::LoadFirstScene(LPCWSTR szBasePath, LPCWSTR szScenePath, void* pvInitData, HXUIOBJ *phObj /*=NULL*/, BYTE Port /*= XUSER_INDEX_FOCUS*/)
{
    if (phObj)
        *phObj = NULL;

    ASSERT(m_bDefaultTypefaceRegistered);
    ASSERT(m_bSkinLoaded);

    // make sure we've created the main canvas already
    ASSERT(m_hObjRoot);
    if (!m_hObjRoot)
        return E_UNEXPECTED;

    HXUIOBJ hScene;
    HRESULT hr = XuiSceneCreate(szBasePath, szScenePath, pvInitData, &hScene);
    if (FAILED(hr))
        return hr;

    hr = XuiSceneNavigateFirst(m_hObjRoot, hScene, Port);
    if (FAILED(hr))
    {
        XuiDestroyObject(hScene);
        return hr;
    }

    if (phObj)
        *phObj = hScene;

    return S_OK;
}

XUI_INLINE HRESULT CXuiModule::CreateMainCanvas()
{
    ASSERT(m_bXuiInited);
    if (!m_bXuiInited)
        return E_UNEXPECTED;

    ASSERT(m_hObjRoot == NULL);
    if (m_hObjRoot)
        return E_UNEXPECTED;


    UINT uWidth, uHeight;
    HRESULT hr = XuiRenderGetBackBufferSize(m_hDC, &uWidth, &uHeight);
    if (FAILED(hr))
        return hr;

    hr = XuiCreateObject(XUI_CLASS_CANVAS, &m_hObjRoot);
    if (FAILED(hr))
        return hr;

    hr = XuiElementSetBounds(m_hObjRoot, (float) uWidth, (float) uHeight);
    if (FAILED(hr))
        return hr;
    return S_OK;
}

#ifdef XUIAPP_AUDITIONING
////////////////////////////////////////////////////////////////////////////////
// IXuiAuditionHost implementation
////////////////////////////////////////////////////////////////////////////////
XUI_INLINE HRESULT CXuiModule::ProcessRemoteInput(const XINPUT_KEYSTROKE * pKeystroke)
{
    return E_NOTIMPL;
}

XUI_INLINE HRESULT CXuiModule::ChangeLocale()
{
    return E_NOTIMPL;
}

XUI_INLINE HRESULT CXuiModule::ChangeSkin(LPCWSTR szSkinResource, LPCWSTR szPrefix /* = NULL */)
{
    XuiFreeVisuals(szPrefix);

    HRESULT hr = XuiLoadVisualFromBinary(szSkinResource, szPrefix);
    if (FAILED(hr))
        return hr;

    XuiElementSkinChanged(m_hObjRoot);

    m_bSkinLoaded = TRUE;
    return S_OK;
}

XUI_INLINE HRESULT CXuiModule::ReloadResources()
{
    XuiElementDiscardResources(m_hObjRoot, XUI_DISCARD_ALL);
    return S_OK;
}

#endif // XUIAPP_AUDITIONING

////////////////////////////////////////////////////////////////////////////////
// CXuiSceneCache implementation
////////////////////////////////////////////////////////////////////////////////
XUI_INLINE CXuiSceneCache::CXuiSceneCache()
{
    m_hClassControl = NULL;
    m_hClassNavButton = NULL;
    m_hClassScene = NULL;
    m_pHead = NULL;
    m_pTail = NULL;
}

XUI_INLINE HRESULT CXuiSceneCache::Init()
{
    m_hClassControl = XuiFindClass(XUI_CLASS_CONTROL);
    m_hClassNavButton = XuiFindClass(XUI_CLASS_NAVBUTTON);
    m_hClassScene = XuiFindClass(XUI_CLASS_SCENE);
    return S_OK;
}
    
XUI_INLINE void CXuiSceneCache::Uninit()
{
    RemoveAll();
    m_hClassControl = NULL;
    m_hClassNavButton = NULL;
    m_hClassScene = NULL;
}
    
XUI_INLINE void CXuiSceneCache::RemoveAll()
{
    SceneCacheEntry *pEntry = m_pHead;
    while (pEntry)
    {
        SceneCacheEntry *pNext = pEntry->m_pNext;
        delete pEntry;
        pEntry = pNext;
    }
    m_pHead = m_pTail = NULL;
}
    
XUI_INLINE HRESULT CXuiSceneCache::CacheScene(LPCWSTR szFullPath, int nCacheLevels/*=0*/, HXUIOBJ *phScene/*=NULL*/)
{
    if (phScene != NULL)
        *phScene = NULL;

    // see if the scene is already in the cache
    if (FindScene(szFullPath))
        return S_OK;
    
    // allocate a new entry
    SceneCacheEntry *pEntry = new SceneCacheEntry();
    if (!pEntry)
        return E_OUTOFMEMORY;

    HXUIOBJ hScene;
    HRESULT hr = XuiSceneCreate(NULL, szFullPath, NULL, &hScene);
    if (FAILED(hr))
    {
        delete pEntry;
        return hr;
    }

    XUIMessage msg;
    XuiMessage(&msg, XM_INIT_RESOURCES);
    XuiBroadcastMessage(hScene, &msg);

    hr = pEntry->SetFileName(szFullPath);
    if (FAILED(hr))
    {
        delete pEntry;
        XuiDestroyObject(hScene);
        return hr;
    }
    
    pEntry->m_hObj = hScene;
    
    // now link it to the list
    if (!m_pTail)
        m_pHead = m_pTail = pEntry;
    else
    {
        pEntry->m_pPrev = m_pTail;
        m_pTail->m_pNext = pEntry;
        m_pTail = pEntry;
    }
    
    StopTimelines(hScene);

    if (phScene)
        *phScene = hScene;
        
    if (nCacheLevels > 0)
    {
        hr = CacheNavButtonTargets(hScene, nCacheLevels-1);
        if (FAILED(hr))
            return hr;
    }
    return S_OK;
}

XUI_INLINE HRESULT CXuiSceneCache::CacheNavButtonTargets(HXUIOBJ hObj, int nCacheLevels)
{
    
    LPCWSTR szBasePath = XuiElementGetBasePath(hObj);
    // loop through the children of the scene
    HXUIOBJ hChild;
    XuiElementGetFirstChild(hObj, &hChild);
    while (hChild)
    {
        HXUIOBJ hNavButton = XuiDynamicCast(hChild, m_hClassNavButton);
        if (hNavButton != NULL)
        {
            LPCWSTR szPath = XuiNavButtonGetPressPath(hNavButton);
            WCHAR *szFullPath = NULL;
            HRESULT hr = XuiResourceComposeLocator(&szFullPath, szBasePath, szPath);
            if (FAILED(hr))
                return hr;

            hr = CacheScene(szFullPath, nCacheLevels, NULL);
            if (FAILED(hr))
                return hr;
            XuiFree(szFullPath);
        }
        else
        {
            HXUIOBJ hChildScene = XuiDynamicCast(hChild, m_hClassScene);
            if (hChildScene)
            {
                HRESULT hr = CacheNavButtonTargets(hChild, nCacheLevels);
                if (FAILED(hr))
                    return hr;
            }
        }
        XuiElementGetNext(hChild, &hChild);
    }
    return S_OK;
}
    
XUI_INLINE HXUIOBJ CXuiSceneCache::GetSceneFromCache(LPCWSTR szFullPath)
{
    SceneCacheEntry *pEntry = m_pHead;
    while (pEntry != NULL)
    {
        if (!_wcsicmp(szFullPath, pEntry->m_szFileName))
        {
            if (!pEntry->m_bInUse)
            {
                pEntry->m_bInUse = TRUE;
                
                // replay the timeline
                StartTimelines(pEntry->m_hObj);
                
                // send the scene the re-init message
                XUIMessage msg;
                XuiMessage(&msg, XM_REINIT_CACHED_SCENE);
                XuiSendMessage(pEntry->m_hObj, &msg);
                return pEntry->m_hObj;
            }
        }
        pEntry = pEntry->m_pNext;
    }
    return NULL;
}
    
XUI_INLINE BOOL CXuiSceneCache::ReleaseScene(HXUIOBJ hObj)
{
    SceneCacheEntry *pEntry = m_pHead;
    while (pEntry != NULL)
    {
        if (pEntry->m_hObj == hObj)
        {
            StopTimelines(pEntry->m_hObj);

            ASSERT(pEntry->m_bInUse == TRUE);
            pEntry->m_bInUse = FALSE;
            return TRUE;
        }
        pEntry = pEntry->m_pNext;
    }
    return FALSE;
}

XUI_INLINE BOOL CXuiSceneCache::RemoveScene(HXUIOBJ hScene)
{
    SceneCacheEntry *pEntry = FindSceneInCache(hScene);
    if (!pEntry)
        return FALSE;

    // unlink from the list
    if (pEntry->m_pPrev)
    {
        pEntry->m_pPrev->m_pNext = pEntry->m_pNext;
    }
    else
    {
        // we're unlinking the head
        m_pHead = pEntry->m_pNext;
    }
    
    if (pEntry->m_pNext)
    {
        pEntry->m_pNext->m_pPrev = pEntry->m_pPrev;
    }
    else
    {
        // we're unlinking the tail
        m_pTail = pEntry->m_pPrev;
    }
    
    // now detach the scene from the Entry
    pEntry->DetachObject();
    
    // finally free the memory
    delete pEntry;
    return TRUE;
}

/*static*/ XUI_INLINE void CXuiSceneCache::StopTimelines(HXUIOBJ hObj)
{
    XuiElementStopTimeline(hObj, FALSE);
    
    // recursively run on the children
    HXUIOBJ hChild;
    XuiElementGetFirstChild(hObj, &hChild);
    while (hChild != NULL)
    {
        StopTimelines(hChild);
        XuiElementGetNext(hChild, &hChild);
    }
}
    
XUI_INLINE void CXuiSceneCache::StartTimelines(HXUIOBJ hObj)
{
    // for non-controls, we just play the timeline
    XuiElementPlayTimeline(hObj, 0, 0, -1, TRUE, FALSE);
    
    // recursively run on the children
    HXUIOBJ hChild;
    XuiElementGetFirstChild(hObj, &hChild);
    while (hChild != NULL)
    {
        StartTimelines(hChild);
        XuiElementGetNext(hChild, &hChild);
    }
    HXUIOBJ hControl = XuiDynamicCast(hObj, m_hClassControl);
    if (hControl != NULL)
    {
        // for controls we send the play visual message
        XuiControlPlayVisual(hObj);
    }
}

XUI_INLINE BOOL CXuiSceneCache::HandleNavigation(HXUIOBJ hObjSource, XUINotify *pNotify)
{
    HXUIOBJ hCurrScene;
    XuiElementGetParent(hObjSource, &hCurrScene);
    if (!hCurrScene)
        return FALSE;

    LPCWSTR szBasePath = XuiElementGetBasePath(hCurrScene);
    LPCWSTR szPath = XuiNavButtonGetPressPath(hObjSource);
    
    WCHAR *szFullPath = NULL;
    
    HRESULT hr = XuiResourceComposeLocator(&szFullPath, szBasePath, szPath);
    if (FAILED(hr))
        return FALSE;
    
    HXUIOBJ hCachedScene = GetSceneFromCache(szFullPath);
    if (!hCachedScene)
    {
        XuiFree(szFullPath);
        return FALSE;
    }
    
    XuiFree(szFullPath);

    // get the transition index overrides from the nav button
    UINT uSrcTransIndex, uDestTransIndex;
    if (SUCCEEDED(XuiNavButtonGetTransIndices(hObjSource, &uSrcTransIndex, &uDestTransIndex)))
    {
        XuiSceneSetTransIndex(hCurrScene, XUI_NAV_FORWARD, uSrcTransIndex);
        XuiSceneSetTransIndex(hCachedScene, XUI_NAV_BACK, uDestTransIndex);
    }
    
    hr = XuiSceneNavigateForward(hCurrScene, XuiNavButtonGetStayVisible(hObjSource), hCachedScene, XUSER_INDEX_FOCUS);
    if (FAILED(hr))
    {
        ReleaseScene(hCachedScene);
        return FALSE;
    }

    return TRUE;
}
    
XUI_INLINE BOOL CXuiSceneCache::FindScene(HXUIOBJ hScene)
{
    SceneCacheEntry *pEntry = FindSceneInCache(hScene);
    return pEntry != NULL;
}
    
XUI_INLINE BOOL CXuiSceneCache::FindScene(LPCWSTR szFullPath)
{
    SceneCacheEntry *pEntry = m_pHead;
    while (pEntry != NULL)
    {
        if (!_wcsicmp(szFullPath, pEntry->m_szFileName))
            return TRUE;
        pEntry = pEntry->m_pNext;
    }
    return FALSE;
}
    
XUI_INLINE HRESULT CXuiSceneCache::OnSkinChanged()
{
    // send the skin change to all cached scenes that are not in use
    // we're assuming that scenes that are in use will get the skin change
    // through their current parent
    SceneCacheEntry *pEntry = m_pHead;
    while (pEntry != NULL)
    {
        if (!pEntry->m_bInUse)
        {
            XuiElementSkinChanged(pEntry->m_hObj);
        }
        pEntry = pEntry->m_pNext;
    }
    return S_OK;
}

XUI_INLINE BOOL CXuiSceneCache::IsDescendantOfCachedScene(HXUIOBJ hScene)
{
    SceneCacheEntry *pEntry = m_pHead;
    while (pEntry != NULL)
    {
        if (XuiElementIsDescendant(pEntry->m_hObj, hScene))
            return TRUE;
        pEntry = pEntry->m_pNext;
    }
    return FALSE;
}
    
XUI_INLINE CXuiSceneCache::SceneCacheEntry *CXuiSceneCache::FindSceneInCache(HXUIOBJ hObj)
{
    SceneCacheEntry *pEntry = m_pHead;
    while (pEntry != NULL)
    {
        if (pEntry->m_hObj == hObj)
            return pEntry;
        pEntry = pEntry->m_pNext;
    }
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////
// CXuiCachingModule implementation
////////////////////////////////////////////////////////////////////////////////
XUI_INLINE CXuiCachingModule::CXuiCachingModule()
{
    m_hClassNavButton = NULL;
    m_hClassBackButton = NULL;
    m_hClassScene = NULL;
}
    
XUI_INLINE HRESULT CXuiCachingModule::Init(PFN_XUITEXTURELOADER pfnTextureLoader, XUIInitParams *pInitParams/*=NULL*/)
{
    HRESULT hr = CXuiModule::Init(pfnTextureLoader, pInitParams);
    if (FAILED(hr))
        return hr;

    m_hClassNavButton = XuiFindClass(XUI_CLASS_NAVBUTTON);
    m_hClassBackButton = XuiFindClass(XUI_CLASS_BACKBUTTON);
    m_hClassScene = XuiFindClass(XUI_CLASS_SCENE);

    hr = m_sceneCache.Init();
    if (FAILED(hr))
        return hr;

    s_cachingModuelInst = this;

    XuiSetMessageFilter(_SceneCacheMessageFilter);
    return S_OK;

}

/*virtual*/ XUI_INLINE void CXuiCachingModule::Uninit()
{
    XuiSetMessageFilter(NULL);
    m_sceneCache.Uninit();
    CXuiModule::Uninit();
}

XUI_INLINE HRESULT CXuiCachingModule::CacheScene(LPCWSTR szFullPath, int nCacheLevels/*=0*/, HXUIOBJ *phScene/*=NULL*/)
{
    return m_sceneCache.CacheScene(szFullPath, nCacheLevels, phScene);
}

XUI_INLINE BOOL CXuiCachingModule::ReleaseScene(HXUIOBJ hObj)
{
    return m_sceneCache.ReleaseScene(hObj);
}

XUI_INLINE BOOL CXuiCachingModule::RemoveSceneFromCache(HXUIOBJ hScene)
{
    return m_sceneCache.RemoveScene(hScene);
}

XUI_INLINE HXUIOBJ CXuiCachingModule::GetSceneFromCache(LPCWSTR szPath)
{
    return m_sceneCache.GetSceneFromCache(szPath);
}
    
XUI_INLINE HRESULT CXuiCachingModule::MessageFilter(HXUIOBJ hObj, XUIMessage *pMessage)
{
    if (pMessage->dwMessage == XM_NOTIFY)
    {
        XUINotify *pNotify = (XUINotify *) pMessage->pvData;
        if (pNotify->dwNotify == XN_PRESS)
        {
            HXUIOBJ hNavButton = XuiDynamicCast(pNotify->hObjSource, m_hClassNavButton);
            if (hNavButton)
            {
                if (m_sceneCache.HandleNavigation(hNavButton, pNotify))
                    pMessage->bHandled = TRUE;
            }
        }
    }
    else if (pMessage->dwMessage == XM_TRANSITION_END || pMessage->dwMessage == XM_TRANSITION_START)
    {
        XUIMessageTransition *pData = (XUIMessageTransition *) pMessage->pvData;
        if (pData->dwTransType == XUI_TRANSITION_BACKFROM)
        {
            if (m_sceneCache.FindScene(hObj))
            {
                pData->dwTransAction = XUI_TRANSITION_ACTION_UNLINK;
                
                if (pMessage->dwMessage == XM_TRANSITION_END)
                    m_sceneCache.ReleaseScene(hObj);
            }
        }
    }
    else if (pMessage->dwMessage == XM_GET_DISCARD_FLAGS)
    {
        XUIMessageDiscardResources *pData = (XUIMessageDiscardResources *) pMessage->pvData;
        if (m_sceneCache.IsDescendantOfCachedScene(hObj))
        {
            pData->dwFlags = 0;
            pMessage->bHandled = TRUE;
        }
    }
#ifdef _DEBUG
    else if (pMessage->dwMessage == XM_DESTROY)
    {
        HXUIOBJ hScene = XuiDynamicCast(hObj, m_hClassScene);
        if (hScene != NULL)
        {
            ASSERT(m_sceneCache.FindScene(hObj)==FALSE);
        }
    }
#endif
    return S_OK;
}

/*static*/ XUI_INLINE HRESULT CXuiCachingModule::_SceneCacheMessageFilter(HXUIOBJ hObj, XUIMessage *pMessage)
{
    return s_cachingModuelInst->MessageFilter(hObj, pMessage);
}
/*static*/ __declspec(selectany) CXuiCachingModule *CXuiCachingModule::s_cachingModuelInst;

