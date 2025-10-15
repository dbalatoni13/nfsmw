
/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       xuiapp.h
 *  Content:    Xbox 360 UI runtime C++ helpers
 *
 ****************************************************************************/

#pragma once

#ifndef ASSERT
#ifdef _DEBUG
#define ASSERT(expr) \
    (void)((expr) || \
           (XuiGetRuntimeHooks() != NULL && XuiGetRuntimeHooks()->LogMessage(XUI_ERR_INTERNAL, L###expr) == S_FALSE) || \
           (DebugBreak(), 0))
#else
#define ASSERT(expr)
#endif // _DEBUG
#endif // ASSERT

////////////////////////////////////////////////////////////////////////////////
// CXuiStringTable
// a structure that manages XUI string tables
////////////////////////////////////////////////////////////////////////////////
class CXuiStringTable
{
public:
    CXuiStringTable()
    {
        m_hStringTable = NULL;
    }    
    ~CXuiStringTable()
    {
        Clear();
    }   
    void Clear()
    {
        if (m_hStringTable != NULL)
        {
            XuiFreeStringTable(m_hStringTable);
            m_hStringTable = NULL;
        }
    }
    HRESULT Load(LPCWSTR szFile)
    {
        Clear();
        return XuiLoadStringTableFromFile(szFile, &m_hStringTable);
    }
    HRESULT Load(PVOID pData)
    {
        Clear();
        return XuiLoadStringTableFromMemory(pData, &m_hStringTable);
    }    
    LPCWSTR Lookup(LPCWSTR szId)
    {
        return XuiLookupStringTable(m_hStringTable, szId);
    }
    LPCWSTR Lookup(UINT nIndex)
    {
        return XuiLookupStringTableByIndex(m_hStringTable, nIndex);
    }

protected:
    HXUISTRINGTABLE m_hStringTable;
};

////////////////////////////////////////////////////////////////////////////////
// CXuiAnimRunner
// a structure that manages running XUI animations.
////////////////////////////////////////////////////////////////////////////////
class CXuiAnimRunner
{
public:
    CXuiAnimRunner();
    void Reset();
    void Resume();
    void Run();
    void __declspec(deprecated) Run(DWORD dwTime);
    void GetLastFrameTime(LARGE_INTEGER *pliLastFrameTime);
    float GetDeltaTime();
protected:
    LARGE_INTEGER m_liTicksPerSecond;
    LARGE_INTEGER m_liLastFrameTime;
};

////////////////////////////////////////////////////////////////////////////////
// CXuiBase
// a class to simplify using the base APIs
////////////////////////////////////////////////////////////////////////////////
class CXuiBase
{
public:
    HXUIOBJ m_hObj;

    CXuiBase()
    {
        m_hObj = NULL;
    }

    // attaching constructor
    CXuiBase(HXUIOBJ hObj)
    {
        Attach(hObj);
    }
    operator HXUIOBJ()
    {
        ASSERT(IsValid());
        return m_hObj;
    }
    CXuiBase& operator =(HXUIOBJ hObj)
    {
        Attach(hObj);
        return *this;
    }
    void Attach(HXUIOBJ hObj)
    {
        m_hObj = hObj;
        ASSERT(IsValid());
    }

    //
    // IsValid
    // Returns TRUE if the object has a NULL handle or has a valid object handle
    //
    BOOL IsValid()
    {
        return (m_hObj == NULL) || IsValidObject();
    }

    //
    // IsValidObject
    // Returns TRUE if the object has a valid handle
    //
    BOOL IsValidObject()
    {
        return XuiHandleIsValid(m_hObj);
    }

    // base APIs
    
    HXUICLASS GetClass()
    {
        return XuiGetObjectClass(m_hObj);
    }
    HRESULT Destroy()
    {
        HRESULT hr = XuiDestroyObject(m_hObj);
        m_hObj = NULL;
        return hr;
    }
    BOOL IsInstanceOf(HXUICLASS hClass)
    {
        return XuiIsInstanceOf(m_hObj, hClass);
    }
    HXUIOBJ DynamicCast(HXUICLASS hClass)
    {
        return XuiDynamicCast(m_hObj, hClass);
    }
    HRESULT SendMessage(XUIMessage *pMessage)
    {
        return XuiSendMessage(m_hObj, pMessage);
    }
    HRESULT BubbleMessage(XUIMessage *pMessage)
    {
        return XuiBubbleMessage(m_hObj, pMessage);
    }
    HRESULT BroadcastMessage(XUIMessage *pMessage)
    {
        return XuiBroadcastMessage(m_hObj, pMessage);
    }
    HXUIOBJ GetOuter()
    {
        return XuiGetOuter(m_hObj);
    }
    HXUIOBJ GetBase()
    {
        HXUIOBJ hBase = NULL;
        XuiGetBaseObject(m_hObj, &hBase);
        return hBase;
    }
    HRESULT SetTimer(DWORD nId, DWORD nElapse)
    {
        return XuiSetTimer(m_hObj, nId, nElapse);
    }
    HRESULT KillTimer(DWORD nId)
    {
        return XuiKillTimer(m_hObj, nId);
    }
    
};

////////////////////////////////////////////////////////////////////////////////
// CXuiElement
// a class to simplify using the XuiElement* APIs
////////////////////////////////////////////////////////////////////////////////
class CXuiElement: public CXuiBase
{
public:
    CXuiElement()
    {
    }
    CXuiElement(HXUIOBJ hObj) : CXuiBase(hObj)
    {
    }

    // API wrappers
    HXUIOBJ ElementFromPoint(POINT pt)
    {
        return XuiElementFromPoint(m_hObj, pt);
    }
    void ElementFromPoint(POINT pt, CXuiElement *pElem)
    {
        pElem->m_hObj = XuiElementFromPoint(m_hObj, pt);
    }
    HRESULT AddChild(HXUIOBJ hChild)
    {
        return XuiElementAddChild(m_hObj, hChild);
    }
    HRESULT Unlink()
    {
        return XuiElementUnlink(m_hObj);
    }
    HRESULT GetParent(HXUIOBJ *phParent)
    {
        return XuiElementGetParent(m_hObj, phParent);
    }
    HRESULT GetParent(CXuiElement *pParent)
    {
        return XuiElementGetParent(m_hObj, &pParent->m_hObj);
    }
    HRESULT GetFirstChild(HXUIOBJ *phFirstChild)
    {
        return XuiElementGetFirstChild(m_hObj, phFirstChild);
    }
    HRESULT GetFirstChild(CXuiElement *pFirstChild)
    {
        return XuiElementGetFirstChild(m_hObj, &pFirstChild->m_hObj);
    }
    HRESULT GetLastChild(HXUIOBJ *phLastChild)
    {
        return XuiElementGetLastChild(m_hObj, phLastChild);
    }
    HRESULT GetLastChild(CXuiElement *pLastChild)
    {
        return XuiElementGetLastChild(m_hObj, &pLastChild->m_hObj);
    }
    HRESULT GetNext(HXUIOBJ *phNext)
    {
        return XuiElementGetNext(m_hObj, phNext);
    }
    HRESULT GetNext(CXuiElement *pNext)
    {
        return XuiElementGetNext(m_hObj, &pNext->m_hObj);
    }
    HRESULT GetPrev(HXUIOBJ *phPrev)
    {
        return XuiElementGetPrev(m_hObj, phPrev);
    }
    HRESULT GetPrev(CXuiElement *pPrev)
    {
        return XuiElementGetPrev(m_hObj, &pPrev->m_hObj);
    }
    HRESULT InsertChild(HXUIOBJ hChild, HXUIOBJ hChildPrev, HXUIOBJ hChildNext)
    {
        return XuiElementInsertChild(m_hObj, hChild, hChildPrev, hChildNext);
    }
    HRESULT GetXForm(D3DXMATRIX *pmatXForm)
    {
        return XuiElementGetXForm(m_hObj, pmatXForm);
    }
    HRESULT GetFullXForm(D3DXMATRIX *pmatXForm)
    {
        return XuiElementGetFullXForm(m_hObj, pmatXForm);
    }
    HRESULT GetBounds(float *pfWidth, float *pfHeight)
    {
        return XuiElementGetBounds(m_hObj, pfWidth, pfHeight);
    }
    HRESULT SetBounds(float fWidth, float fHeight)
    {
        return XuiElementSetBounds(m_hObj, fWidth, fHeight);
    }
    HRESULT GetPosition(D3DXVECTOR3 *pvPos)
    {
        return XuiElementGetPosition(m_hObj, pvPos);
    }
    HRESULT SetPosition(const D3DXVECTOR3 *pvPos)
    {
        return XuiElementSetPosition(m_hObj, pvPos);
    }
    HRESULT GetScale(D3DXVECTOR3 *pvScale)
    {
        return XuiElementGetScale(m_hObj, pvScale);
    }
    HRESULT SetScale(const D3DXVECTOR3 *pvScale)
    {
        return XuiElementSetScale(m_hObj, pvScale);
    }
    HRESULT GetRotation(D3DXQUATERNION *pquatRot)
    {
        return XuiElementGetRotation(m_hObj, pquatRot);
    }
    HRESULT SetRotation(const D3DXQUATERNION *pquatRot)
    {
        return XuiElementSetRotation(m_hObj, pquatRot);
    }
    HRESULT GetOpacity(float *pfOpacity)
    {
        return XuiElementGetOpacity(m_hObj, pfOpacity);
    }
    HRESULT SetOpacity(float fOpacity)
    {
        return XuiElementSetOpacity(m_hObj, fOpacity);
    }
    HRESULT SetPivot(const D3DXVECTOR3 *pvPivot)
    {
        return XuiElementSetPivot(m_hObj, pvPivot);
    }
    HRESULT GetPivot(D3DXVECTOR3 *pvPivot)
    {
        return XuiElementGetPivot(m_hObj, pvPivot);
    }
    HRESULT GetId(LPCWSTR *ppszId)
    {
        return XuiElementGetId(m_hObj, ppszId);
    }
    HRESULT GetChildById(LPCWSTR szId, HXUIOBJ *phChild)
    {
        return XuiElementGetChildById(m_hObj, szId, phChild);
    }
    HRESULT GetChildById(LPCWSTR szId, CXuiElement *pChild)
    {
        return XuiElementGetChildById(m_hObj, szId, &pChild->m_hObj);
    }
    BOOL IsDescendant(HXUIOBJ hDescendant)
    {
        return XuiElementIsDescendant(m_hObj, hDescendant);
    }
    HRESULT SetUserData(void *pvUserData)
    {
        return XuiElementSetUserData(m_hObj, pvUserData);
    }
    HRESULT GetUserData(void **ppvUserData)
    {
        return XuiElementGetUserData(m_hObj, ppvUserData);
    }
    HRESULT GetAnchor(UINT *puAnchor)
    {
        return XuiElementGetAnchor(m_hObj, puAnchor);
    }
    BOOL IsShown()
    {
        return XuiElementIsShown(m_hObj);
    }
    HRESULT SetShow(BOOL bShow)
    {
        return XuiElementSetShow(m_hObj, bShow);
    }
    HRESULT BeginRender(XUIMessageRender *pRenderData, XUIRenderStruct *pRenderStruct)
    {
        return XuiElementBeginRender(m_hObj, pRenderData, pRenderStruct);
    }
    HRESULT EndRender(XUIMessageRender *pRenderData, XUIRenderStruct *pRenderStruct)
    {
        return XuiElementEndRender(m_hObj, pRenderData, pRenderStruct);
    }
    HRESULT RenderChildren(XUIMessageRender *pRenderData)
    {
        return XuiElementRenderChildren(m_hObj, pRenderData);
    }
    HRESULT SetBlendMode(XUI_BLEND_MODE nBlendMode)
    {
        return XuiElementSetBlendMode(m_hObj, nBlendMode);
    }
    HRESULT GetBlendMode(XUI_BLEND_MODE *pnBlendMode)
    {
        return XuiElementGetBlendMode(m_hObj, pnBlendMode);
    }
    HRESULT SetColorWriteFlags(DWORD dwColorWriteFlags)
    {
        return XuiElementSetColorWriteFlags(m_hObj, dwColorWriteFlags);
    }
    HRESULT GetColorWriteFlags(DWORD *pdwColorWriteFlags)
    {
        return XuiElementGetColorWriteFlags(m_hObj, pdwColorWriteFlags);
    }
    HRESULT EnableInput(BOOL bEnable)
    {
        return XuiElementEnableInput(m_hObj, bEnable);
    }
    BOOL InputEnabled()
    {
        return XuiElementInputEnabled(m_hObj);
    }

    HRESULT DiscardResources(DWORD dwFlags)
    {
        return XuiElementDiscardResources(m_hObj, dwFlags);
    }
    // element timeline APIs
    HRESULT SetTimeline(XUITimeline *pTimeline)
    {
        return XuiElementSetTimeline(m_hObj, pTimeline);
    }
    HRESULT SetSubtimeline(XUISubtimeline *pSubtimeline, int nIndex)
    {
        return XuiElementSetSubtimeline(m_hObj, pSubtimeline, nIndex);
    }
    HRESULT GetTimeline(XUITimeline **ppTimeline)
    {
        return XuiElementGetTimeline(m_hObj, ppTimeline);
    }
    HRESULT StopTimeline(BOOL bStopChildren)
    {
        return XuiElementStopTimeline(m_hObj, bStopChildren);
    }
    HRESULT PlayTimeline(int nStartFrame, int nInitialFrame, int nEndFrame, BOOL bLoop, BOOL bRecurse)
    {
        return XuiElementPlayTimeline(m_hObj, nStartFrame, nInitialFrame, nEndFrame, bLoop, bRecurse);
    }
    HRESULT FindNamedFrame(LPCWSTR szFrameName, int *pnFrame)
    {
        return XuiElementFindNamedFrame(m_hObj, szFrameName, pnFrame);
    }
    HRESULT DisallowRecursiveTimelineControl(BOOL fDisallow)
    {
        return XuiElementDisallowRecursiveTimelineControl(m_hObj, fDisallow);
    }

    // focus APIs
    HRESULT InitFocus(BYTE UserIndex = XUSER_INDEX_ANY, BOOL bPlayInitialFocus = FALSE)
    {
        return XuiElementInitUserFocus(m_hObj, UserIndex, bPlayInitialFocus);
    }
    HRESULT SetFocus(BYTE UserIndex = XUSER_INDEX_ANY)
    {
        return XuiElementSetUserFocus(m_hObj, UserIndex);
    }
    static HXUIOBJ GetFocus(BYTE UserIndex = XUSER_INDEX_ANY)
    {
        return XuiElementGetUserFocus(UserIndex);
    }
    HRESULT WantFocus(BOOL *pbWantFocus)
    {
        return XuiElementWantFocus(m_hObj, pbWantFocus);
    }
    BOOL HasFocus()
    {
        return XuiElementHasFocus(m_hObj);
    }
    BOOL TreeHasFocus()
    {
        return XuiElementTreeHasFocus(m_hObj);
    }
    HXUIOBJ TreeGetFocus()
    {
        return XuiElementTreeGetFocus(m_hObj);
    }
    BYTE GetFocusUser()
    {
        return XuiElementGetFocusUser(m_hObj);
    }
    static BYTE GetFocusUser(HXUIOBJ hObj)
    {
        return XuiElementGetFocusUser(hObj);
    }

    LPCWSTR GetVisualPrefix()
    {
        return XuiElementGetVisualPrefix(m_hObj);
    }
    HRESULT SetVisualPrefix(LPCWSTR szPrefix)
    {
        return XuiElementSetVisualPrefix(m_hObj, szPrefix);
    }
    LPCWSTR GetBasePath()
    {
        return XuiElementGetBasePath(m_hObj);
    }
    HRESULT SetBasePath(LPCWSTR szBasePath)
    {
        return XuiElementSetBasePath(m_hObj, szBasePath);
    }
    static HRESULT SkinChanged(HXUIOBJ hObj)
    {
        return XuiElementSkinChanged(hObj);
    }
    static HRESULT LocaleChanged(HXUIOBJ hObj)
    {
        return XuiElementLocaleChanged(hObj);
    }

    BOOL HitTest(POINT pt)
    {
        return XuiElementHitTest(m_hObj, pt);
    }

    HRESULT ShowMessageBox(LPCWSTR szTitle, LPCWSTR szText, int nButtons, LPCWSTR *pButtonsText)
    {
        return XuiShowMessageBox(m_hObj, szTitle, szText, nButtons, pButtonsText);
    }

    HRESULT ShowMessageBoxEx(LPCWSTR szVisual, HXUIOBJ hParent, LPCWSTR szTitle, LPCWSTR szText, int nButtons, LPCWSTR *pButtonsText, int nFocusButton, DWORD dwFlags)
    {
        return XuiShowMessageBoxEx(szVisual, hParent, m_hObj, szTitle, szText, nButtons, pButtonsText, nFocusButton, dwFlags);
    }

    static HRESULT ShowMessageBox(HXUIOBJ hOwner, LPCWSTR szTitle, LPCWSTR szText, int nButtons, LPCWSTR *pButtonsText)
    {
        return XuiShowMessageBox(hOwner, szTitle, szText, nButtons, pButtonsText);
    }
    CXuiElement& operator =(HXUIOBJ hObj)
    {
        Attach(hObj);
        return *this;
    }
    void Attach(HXUIOBJ hObj)
    {
        m_hObj = hObj;
        ASSERT(IsValid());
        ASSERT(hObj == 0 || XuiDynamicCast(hObj, XuiFindClass(XUI_CLASS_ELEMENT)));
    }    
};

////////////////////////////////////////////////////////////////////////////////
// CXuiTextElement
// a class to simplify using the XuiTextElement* APIs
////////////////////////////////////////////////////////////////////////////////
class CXuiTextElement : public CXuiElement
{
public:
    CXuiTextElement()
    {
    }
    CXuiTextElement(HXUIOBJ hObj) : CXuiElement(hObj)
    {
    }
    HRESULT SetText(LPCWSTR szText)
    {
        return XuiTextElementSetText(m_hObj, szText);
    }
    LPCWSTR GetText()
    {
        return XuiTextElementGetText(m_hObj);
    }
    HRESULT MeasureText(LPCWSTR szText, XUIRect *prct)
    {
        return XuiTextElementMeasureText(m_hObj, szText, prct);
    }
    CXuiTextElement& operator =(HXUIOBJ hObj)
    {
        Attach(hObj);
        return *this;
    }
    void Attach(HXUIOBJ hObj)
    {
        m_hObj = hObj;
        ASSERT(IsValid());
        ASSERT(hObj == 0 || XuiDynamicCast(hObj, XuiFindClass(XUI_CLASS_TEXT)));
    }    
};

////////////////////////////////////////////////////////////////////////////////
// CXuiImageElement
// a class to simplify using the XuiImageElement* APIs
////////////////////////////////////////////////////////////////////////////////
class CXuiImageElement : public CXuiElement
{
public:
    CXuiImageElement()
    {
    }
    CXuiImageElement(HXUIOBJ hObj) : CXuiElement(hObj)
    {
    }
    HRESULT SetImagePath(LPCWSTR szImagePath)
    {
        return XuiImageElementSetImagePath(m_hObj, szImagePath);
    }
    HRESULT GetImagePath(LPCWSTR *pszImagePath)
    {
        return XuiImageElementGetImagePath(m_hObj, pszImagePath);
    }
    HRESULT GetLastError()
    {
        return XuiImageElementGetLastError(m_hObj);
    }
    HRESULT GetContentInfo(XUIContentInfo *pContentInfo)
    {
        return XuiImageElementGetContentInfo(m_hObj, pContentInfo);
    }
    CXuiImageElement& operator =(HXUIOBJ hObj)
    {
        Attach(hObj);
        return *this;
    }
    void Attach(HXUIOBJ hObj)
    {
        m_hObj = hObj;
        ASSERT(IsValid());
        ASSERT(hObj == 0 || XuiDynamicCast(hObj, XuiFindClass(XUI_CLASS_IMAGE)));
    }    
};

////////////////////////////////////////////////////////////////////////////////
// CXuiSound
// a class to simplify using the XuiSound* APIs
////////////////////////////////////////////////////////////////////////////////
class CXuiSound : public CXuiElement
{
public:
    CXuiSound()
    {
    }
    CXuiSound(HXUIOBJ hObj) : CXuiElement(hObj)
    {
    }
    HRESULT Mute(BOOL bMute)
    {
        return XuiSoundMute(m_hObj, bMute);
    }
    BOOL IsInVisual()
    {
        return XuiSoundIsInVisual(m_hObj);
    }
    HRESULT SetDirty()
    {
        return XuiSoundSetDirty(m_hObj);
    }
    BOOL IsDirty()
    {
        return XuiSoundIsDirty(m_hObj);
    }
    HRESULT Play()
    {
        return XuiSoundPlay(m_hObj);
    }
    HRESULT Stop()
    {
        return XuiSoundStop(m_hObj);
    }
    CXuiSound& operator =(HXUIOBJ hObj)
    {
        Attach(hObj);
        return *this;
    }
    void Attach(HXUIOBJ hObj)
    {
        m_hObj = hObj;
        ASSERT(IsValid());
        ASSERT(hObj == 0 || XuiDynamicCast(hObj, XuiFindClass(XUI_CLASS_SOUND)));
    }    
};

////////////////////////////////////////////////////////////////////////////////
// CXuiVideo
// a class to simplify using the XuiVideoElement* APIs
////////////////////////////////////////////////////////////////////////////////
class CXuiVideo : public CXuiElement
{
public:
    CXuiVideo()
    {
    }
    CXuiVideo(HXUIOBJ hObj) : CXuiElement(hObj)
    {
    }
    BOOL IsPaused()
    {
        return XuiVideoIsPaused(m_hObj);
    }
    HRESULT Pause(BOOL bPause)
    {
        return XuiVideoPause(m_hObj, bPause);
    }
    HRESULT Play(LPCWSTR szFile)
    {
        return XuiVideoPlay(m_hObj, szFile);
    }
    HRESULT SetPlayer(IXMediaXmvPlayer* pPlayer)
    {
        return XuiVideoSetPlayer(m_hObj, pPlayer);
    }
    CXuiVideo& operator =(HXUIOBJ hObj)
    {
        Attach(hObj);
        return *this;
    }
    void Attach(HXUIOBJ hObj)
    {
        m_hObj = hObj;
        ASSERT(IsValid());
        ASSERT(hObj == 0 || XuiDynamicCast(hObj, XuiFindClass(XUI_CLASS_VIDEO)));
    }    
};

////////////////////////////////////////////////////////////////////////////////
// CXuiControl
// a class to simplify using the XuiControlElement* APIs
////////////////////////////////////////////////////////////////////////////////
class CXuiControl : public CXuiElement
{
public:
    CXuiControl()
    {
    }
    CXuiControl(HXUIOBJ hObj) : CXuiElement(hObj)
    {
    }

    BOOL IsEnabled()
    {
        return XuiControlIsEnabled(m_hObj);
    }
    HRESULT SetEnable(BOOL bEnable)
    {
        return XuiControlSetEnable(m_hObj, bEnable);
    }
    BOOL IsPressed()
    {
        return XuiControlIsPressed(m_hObj);
    }
    HRESULT SetPress(BOOL bPress)
    {
        return XuiControlSetPress(m_hObj, bPress);
    }
    HRESULT GetVisual(HXUIOBJ* phVisual)
    {
        return XuiControlGetVisual(m_hObj, phVisual);
    }
    HRESULT DestroyVisual()
    {
        return XuiControlDestroyVisual(m_hObj);
    }
    HRESULT AttachVisual()
    {
        return XuiControlAttachVisual(m_hObj);
    }
    HRESULT PlayStandardVisual()
    {
        return XuiControlPlayStandardVisual(m_hObj);
    }
    HRESULT PlayStandardVisualOnFocusChanging(BOOL bFocus)
    {
        return XuiControlPlayStandardVisualOnFocusChanging(m_hObj, bFocus);
    }
    HRESULT PlayVisualRange(LPCWSTR szBegin, LPCWSTR szInitial, LPCWSTR szEnd)
    {
        return XuiControlPlayVisualRange(m_hObj, szBegin, szInitial, szEnd);
    }
    HRESULT PlayOptionalVisual(LPCWSTR szBegin, LPCWSTR szEnd, LPCWSTR szBeginOverrideFallback = NULL, LPCWSTR szEndOverrideFallback = NULL)
    {
        return XuiControlPlayOptionalVisual(m_hObj, szBegin, szEnd, szBeginOverrideFallback, szEndOverrideFallback);
    }
    HRESULT PlayVisual()
    {
        return XuiControlPlayVisual(m_hObj);
    }
    HRESULT PlayVisualOnFocusChanging(BOOL bGettingFocus)
    {
        return XuiControlPlayVisualOnFocusChanging(m_hObj, bGettingFocus);
    }
    LPCWSTR GetText()
    {
        return XuiControlGetText(m_hObj);
    }
    HRESULT SetText(LPCWSTR szText)
    {
        return XuiControlSetText(m_hObj, szText);
    }
    HRESULT SetImage(LPCWSTR szPath)
    {
        return XuiControlSetImagePath(m_hObj, szPath);
    }
    HRESULT SetImage(HXUIBRUSH hBrush)
    {
        return XuiControlSetImageBrush(m_hObj, hBrush);
    }
    HRESULT SetImage(HXUIOBJ hImage)
    {
        return XuiControlSetImageObject(m_hObj, hImage);
    }
    HRESULT GetImage(LPCWSTR *ppszPath)
    {
        return XuiControlGetImagePath(m_hObj, ppszPath);
    }
    HRESULT GetImage(HXUIBRUSH *phBrush)
    {
        return XuiControlGetImageBrush(m_hObj, phBrush);
    }
    HRESULT GetImage(HXUIOBJ *phImage)
    {
        return XuiControlGetImageObject(m_hObj, phImage);
    }
    int GetItemAssociation()
    {
        return XuiControlGetItemAssociation(m_hObj);
    }
    HRESULT SetItemAssociation(int iItem)
    {
        return XuiControlSetItemAssociation(m_hObj, iItem);
    }
    BOOL WantsUnfocusedInput()
    {
        return XuiControlWantsUnfocusedInput(m_hObj);
    }
    HXUIOBJ GetNavigation(XUI_CONTROL_NAVIGATE eDirection, BOOL bSkipNotEnabled = FALSE, BOOL bSkipNotShown = TRUE)
    {
        return XuiControlGetNavigation(m_hObj, eDirection, bSkipNotEnabled, bSkipNotShown);
    }
    BOOL Press(BYTE UserIndex = XUSER_INDEX_ANY)
    {
        return XuiControlPress(m_hObj, UserIndex);
    }
    BOOL IsNavButton()
    {
        return XuiControlIsNavButton(m_hObj);
    }
    BOOL IsBackButton()
    {
        return XuiControlIsBackButton(m_hObj);
    }
    CXuiControl& operator =(HXUIOBJ hObj)
    {
        Attach(hObj);
        return *this;
    }
    void Attach(HXUIOBJ hObj)
    {
        m_hObj = hObj;
        ASSERT(IsValid());
        ASSERT(hObj == 0 || XuiDynamicCast(hObj, XuiFindClass(XUI_CLASS_CONTROL)));
    }    
};

////////////////////////////////////////////////////////////////////////////////
// CXuiNavButton
// a class to simplify using the XuiNavButton* APIs
////////////////////////////////////////////////////////////////////////////////
class CXuiNavButton : public CXuiControl
{
public:
    CXuiNavButton()
    {
    }
    CXuiNavButton(HXUIOBJ hObj) : CXuiControl(hObj)
    {
    }
    LPCWSTR GetPressPath()
    {
        return XuiNavButtonGetPressPath(m_hObj);
    }
    BOOL GetStayVisible()
    {
        return XuiNavButtonGetStayVisible(m_hObj);
    }
    HRESULT GetTransIndices(UINT *puSrcTransIndex, UINT *puDestTransIndex)
    {
        return XuiNavButtonGetTransIndices(m_hObj, puSrcTransIndex, puDestTransIndex);
    }
    CXuiNavButton& operator =(HXUIOBJ hObj)
    {
        Attach(hObj);
        return *this;
    }
    void Attach(HXUIOBJ hObj)
    {
        m_hObj = hObj;
        ASSERT(IsValid());
        ASSERT(hObj == 0 || XuiDynamicCast(hObj, XuiFindClass(XUI_CLASS_NAVBUTTON)));
    }    
};

////////////////////////////////////////////////////////////////////////////////
// CXuiCheckbox
// a class to simplify using the XuiCheckbox* APIs
////////////////////////////////////////////////////////////////////////////////
class CXuiCheckbox : public CXuiControl
{
public:
    CXuiCheckbox()
    {
    }
    CXuiCheckbox(HXUIOBJ hObj) : CXuiControl(hObj)
    {
    }
    BOOL IsChecked()
    {
        return XuiCheckboxIsChecked(m_hObj);
    }
    HRESULT SetCheck(BOOL bCheck)
    {
        return XuiCheckboxSetCheck(m_hObj, bCheck);
    }
    CXuiCheckbox& operator =(HXUIOBJ hObj)
    {
        Attach(hObj);
        return *this;
    }
    void Attach(HXUIOBJ hObj)
    {
        m_hObj = hObj;
        ASSERT(IsValid());
        ASSERT(hObj == 0 || XuiDynamicCast(hObj, XuiFindClass(XUI_CLASS_CHECKBOX)));
    }    
};

////////////////////////////////////////////////////////////////////////////////
// CXuiListItem
// a class to simplify using the XuiListItem* APIs
////////////////////////////////////////////////////////////////////////////////
class CXuiListItem : public CXuiControl
{
public:
    CXuiListItem()
    {
    }
    CXuiListItem(HXUIOBJ hObj) : CXuiControl(hObj)
    {
    }
    HRESULT GetLayout(XUI_LISTITEM_LAYOUT *pnLayout)
    {
        return XuiListItemGetLayout(m_hObj, pnLayout);
    }
    CXuiListItem& operator =(HXUIOBJ hObj)
    {
        Attach(hObj);
        return *this;
    }
    void Attach(HXUIOBJ hObj)
    {
        m_hObj = hObj;
        ASSERT(IsValid());
        ASSERT(hObj == 0 || XuiDynamicCast(hObj, XuiFindClass(XUI_CLASS_LISTITEM)));
    }    
};

////////////////////////////////////////////////////////////////////////////////
// CXuiList
// a class to simplify using the XuiList* APIs
////////////////////////////////////////////////////////////////////////////////
class CXuiList : public CXuiControl
{
public:
    CXuiList()
    {
    }
    CXuiList(HXUIOBJ hObj) : CXuiControl(hObj)
    {
    }
    int GetCurSel()
    {
        return XuiListGetCurSel(m_hObj, NULL);
    }
    int GetCurSel(HXUIOBJ *phItem)
    {
        return XuiListGetCurSel(m_hObj, phItem);
    }
    int GetCurSel(CXuiElement *pItemElem)
    {
        return XuiListGetCurSel(m_hObj, &pItemElem->m_hObj);
    }
    int GetCurSel(CXuiControl *pItemCtrl)
    {
        HXUIOBJ hObj;
        int nRet = XuiListGetCurSel(m_hObj, &hObj);
        if (XuiDynamicCast(hObj, XuiFindClass(XUI_CLASS_CONTROL)) != NULL)
            pItemCtrl->m_hObj = hObj;
        return nRet;
    }
    HRESULT SetCurSel(int iItem)
    {
        return XuiListSetCurSel(m_hObj, iItem);
    }
    HRESULT SetCurSelVisible(int iItem)
    {
        return XuiListSetCurSelVisible(m_hObj, iItem);
    }
    BOOL IsItemVisible(int iItem)
    {
        return XuiListIsItemVisible(m_hObj, iItem);
    }
    HXUIOBJ GetItemControl(int iItem)
    {
        return XuiListGetItemControl(m_hObj, iItem);
    }
    void GetItemControl(int iItem, CXuiControl *pItemCtrl)
    {
        pItemCtrl->m_hObj = GetItemControl(iItem);
    }
    int GetItemCount()
    {
        return XuiListGetItemCount(m_hObj);
    }
    int GetVisibleItemCount()
    {
        return XuiListGetVisibleItemCount(m_hObj);
    }
    int GetCheckedItemCount()
    {
        return XuiListGetCheckedItemCount(m_hObj);
    }
    int GetMaxVisibleItemCount()
    {
        return XuiListGetMaxVisibleItemCount(m_hObj);
    }    
    int GetMaxLinesItemCount()
    {
        return XuiListGetMaxLinesItemCount(m_hObj);
    }
    int GetMaxPerLineItemCount()
    {
        return XuiListGetMaxPerLineItemCount(m_hObj);
    }
    int GetTopItem()
    {
        return XuiListGetTopItem(m_hObj);
    }
    HRESULT SetTopItem(int iItem)
    {
        return XuiListSetTopItem(m_hObj, iItem);
    }
    HRESULT InsertItems(int iAtItem, int cItems)
    {
        return XuiListInsertItems(m_hObj, iAtItem, cItems);
    }
    HRESULT DeleteItems(int iAtItem, int cItems)
    {
        return XuiListDeleteItems(m_hObj, iAtItem, cItems);
    }
    BOOL IsItemChecked(int iItem)
    {
        return XuiListIsItemChecked(m_hObj, iItem);
    }
    HRESULT SetItemCheck(int iItem, BOOL bChecked)
    {
        return XuiListSetItemCheck(m_hObj, iItem, bChecked);
    }
    BOOL IsItemEnabled(int iItem)
    {
        return XuiListIsItemEnabled(m_hObj, iItem);
    }
    HRESULT SetItemEnable(int iItem, BOOL bEnabled)
    {
        return XuiListSetItemEnable(m_hObj, iItem, bEnabled);
    }
    HRESULT SetText(int iItem, LPCWSTR szText)
    {
        return XuiListSetText(m_hObj, iItem, szText);
    }
    LPCWSTR GetText(int iItem)
    {
        return XuiListGetText(m_hObj, iItem);
    }
    HRESULT SetImage(int iItem, LPCWSTR szPath)
    {
        return XuiListSetImagePath(m_hObj, iItem, szPath);
    }
    HRESULT SetImage(int iItem, HXUIBRUSH hBrush)
    {
        return XuiListSetImageBrush(m_hObj, iItem, hBrush);
    }
    HRESULT SetImage(int iItem, HXUIOBJ hImage)
    {
        return XuiListSetImageObject(m_hObj, iItem, hImage);
    }
    HRESULT GetImage(int iItem, LPCWSTR *ppszPath)
    {
        return XuiListGetImagePath(m_hObj, iItem, ppszPath);
    }
    HRESULT GetImage(int iItem, HXUIBRUSH *phBrush)
    {
        return XuiListGetImageBrush(m_hObj, iItem, phBrush);
    }
    HRESULT GetImage(int iItem, HXUIOBJ *phImage)
    {
        return XuiListGetImageObject(m_hObj, iItem, phImage);
    }
    BOOL GetItemCheck(int iItem)
    {
        return XuiListGetItemCheck(m_hObj, iItem);
    }
    CXuiList& operator =(HXUIOBJ hObj)
    {
        Attach(hObj);
        return *this;
    }
    void Attach(HXUIOBJ hObj)
    {
        m_hObj = hObj;
        ASSERT(IsValid());
        ASSERT(hObj == 0 || XuiDynamicCast(hObj, XuiFindClass(XUI_CLASS_LIST)));
    }    
};

////////////////////////////////////////////////////////////////////////////////
// CXuiProgressBar
// a class to simplify using the XuiProgressBar* APIs
////////////////////////////////////////////////////////////////////////////////
class CXuiProgressBar : public CXuiControl
{
public:
    CXuiProgressBar()
    {
    }
    CXuiProgressBar(HXUIOBJ hObj) : CXuiControl(hObj)
    {
    }
    HRESULT SetRange(int nRangeMin, int nRangeMax)
    {
        return XuiProgressBarSetRange(m_hObj, nRangeMin, nRangeMax);
    }
    HRESULT GetRange(int *pnRangeMin, int *pnRangeMax)
    {
        return XuiProgressBarGetRange(m_hObj, pnRangeMin, pnRangeMax);
    }
    HRESULT SetValue(int nValue)
    {
        return XuiProgressBarSetValue(m_hObj, nValue);
    }
    HRESULT GetValue(int *pnValue)
    {
        return XuiProgressBarGetValue(m_hObj, pnValue);
    }
    CXuiProgressBar& operator =(HXUIOBJ hObj)
    {
        Attach(hObj);
        return *this;
    }
    void Attach(HXUIOBJ hObj)
    {
        m_hObj = hObj;
        ASSERT(IsValid());
        ASSERT(hObj == 0 || XuiDynamicCast(hObj, XuiFindClass(XUI_CLASS_PROGRESSBAR)));
    }    
};

////////////////////////////////////////////////////////////////////////////////
// CXuiRadioGroup
// a class to simplify using the XuiRadioGroup* APIs
////////////////////////////////////////////////////////////////////////////////
class CXuiRadioGroup : public CXuiControl
{
public:
    CXuiRadioGroup()
    {
    }
    CXuiRadioGroup(HXUIOBJ hObj) : CXuiControl(hObj)
    {
    }
    int GetCurSel()
    {
        return XuiRadioGroupGetCurSel(m_hObj, NULL);
    }
    int GetCurSel(HXUIOBJ *phItem)
    {
        return XuiRadioGroupGetCurSel(m_hObj, phItem);
    }
    int GetCurSel(CXuiElement *pItemElem)
    {
        return XuiRadioGroupGetCurSel(m_hObj, &pItemElem->m_hObj);
    }
    HRESULT SetCurSel(int iItem)
    {
        return XuiRadioGroupSetCurSel(m_hObj, iItem);
    }
    HXUIOBJ GetItemControl(int iItem)
    {
        return XuiRadioGroupGetItemControl(m_hObj, iItem);
    }
    HXUIOBJ GetChecked()
    {
        return XuiRadioGroupGetChecked(m_hObj);
    }
    HRESULT SetChecked(HXUIOBJ hObjCheck)
    {
        return XuiRadioGroupSetChecked(m_hObj, hObjCheck);
    }
    CXuiRadioGroup& operator =(HXUIOBJ hObj)
    {
        Attach(hObj);
        return *this;
    }
    void Attach(HXUIOBJ hObj)
    {
        m_hObj = hObj;
        ASSERT(IsValid());
        ASSERT(hObj == 0 || XuiDynamicCast(hObj, XuiFindClass(XUI_CLASS_RADIOGROUP)));
    }    
};

////////////////////////////////////////////////////////////////////////////////
// CXuiScene
// a class to simplify using the XuiScene* APIs
////////////////////////////////////////////////////////////////////////////////
class CXuiScene : public CXuiControl
{
public:
    CXuiScene()
    {
    }
    CXuiScene(HXUIOBJ hObj) : CXuiControl(hObj)
    {
    }
    HXUIOBJ GetBackScene()
    {
        return XuiSceneGetBackScene(m_hObj);
    }
    void GetBackScene(CXuiElement *pBackScene)
    {
        pBackScene->Attach(XuiSceneGetBackScene(m_hObj));
    }
    void SetBackScene(HXUIOBJ hBackScene, CXuiElement *pOldBackScene)
    {
        pOldBackScene->Attach(XuiSceneSetBackScene(m_hObj, hBackScene));
    }
    static HRESULT SceneCreate(LPCWSTR szBasePath, LPCWSTR szScenePath, HXUIOBJ* phScene, void* pvInitData = NULL)
    {
        return XuiSceneCreate(szBasePath, szScenePath, pvInitData, phScene);
    }
    static HRESULT SceneCreate(LPCWSTR szBasePath, LPCWSTR szScenePath, CXuiElement *pScene, void* pvInitData = NULL)
    {
        return XuiSceneCreate(szBasePath, szScenePath, pvInitData, &pScene->m_hObj);
    }
    HRESULT SceneCreate(LPCWSTR szScenePath, HXUIOBJ* phScene, void* pvInitData = NULL)
    {
        return XuiSceneCreate(GetBasePath(), szScenePath, pvInitData, phScene);
    }
    HRESULT SceneCreate(LPCWSTR szScenePath, CXuiElement *pScene, void* pvInitData = NULL)
    {
        return XuiSceneCreate(GetBasePath(), szScenePath, pvInitData, &pScene->m_hObj);
    }
    static HRESULT NavigateFirst(HXUIOBJ hParent, HXUIOBJ hFirst, BYTE UserIndex = XUSER_INDEX_FOCUS)
    {
        return XuiSceneNavigateFirst(hParent, hFirst, UserIndex);
    }
    HRESULT NavigateForward(HXUIOBJ hFwd, BOOL bStayVisible = FALSE, BYTE UserIndex = XUSER_INDEX_FOCUS)
    {
        return XuiSceneNavigateForward(m_hObj, bStayVisible, hFwd, UserIndex);
    }
    HRESULT NavigateBack(BYTE UserIndex = XUSER_INDEX_FOCUS)
    {
        HXUIOBJ hBack = GetBackScene();
        if (!hBack)
            return S_FALSE;
        return XuiSceneNavigateBack(m_hObj, hBack, UserIndex);
    }
    HRESULT NavigateBack(HXUIOBJ hBack, BYTE UserIndex = XUSER_INDEX_FOCUS)
    {
        return XuiSceneNavigateBack(m_hObj, hBack, UserIndex);
    }
    HRESULT NavigateBackToFirst(BYTE UserIndex = XUSER_INDEX_FOCUS)
    {
        return XuiSceneNavigateBackToFirst(m_hObj, UserIndex);
    }
    HRESULT InterruptTransitions()
    {
        return XuiSceneInterruptTransitions(m_hObj);
    }
    HRESULT PlayToTransition()
    {
        return XuiScenePlayToTransition(m_hObj);
    }
    HRESULT PlayFromTransition(DWORD dwTransAction)
    {
        return XuiScenePlayFromTransition(m_hObj, dwTransAction);
    }
    HRESULT PlayBackToTransition()
    {
        return XuiScenePlayBackToTransition(m_hObj);
    }
    HRESULT PlayBackFromTransition()
    {
        return XuiScenePlayBackFromTransition(m_hObj);
    }
    HRESULT SetTransIndex(DWORD dwTransType, UINT uTransIndex)
    {
        return XuiSceneSetTransIndex(m_hObj, dwTransType, uTransIndex);
    }
    BOOL IsInTransition()
    {
        return XuiSceneIsInTransition(m_hObj);
    }    
    CXuiScene& operator =(HXUIOBJ hObj)
    {
        Attach(hObj);
        return *this;
    }
    void Attach(HXUIOBJ hObj)
    {
        m_hObj = hObj;
        ASSERT(IsValid());
        ASSERT(hObj == 0 || XuiDynamicCast(hObj, XuiFindClass(XUI_CLASS_SCENE)));
    }
};

////////////////////////////////////////////////////////////////////////////////
// CXuiSlider
// a class to simplify using the XuiSlider* APIs
////////////////////////////////////////////////////////////////////////////////
class CXuiSlider : public CXuiControl
{
public:
    CXuiSlider()
    {
    }
    CXuiSlider(HXUIOBJ hObj) : CXuiControl(hObj)
    {
    }
    HRESULT SetRange(int nRangeMin, int nRangeMax)
    {
        return XuiSliderSetRange(m_hObj, nRangeMin, nRangeMax);
    }
    HRESULT GetRange(int *pnRangeMin, int *pnRangeMax)
    {
        return XuiSliderGetRange(m_hObj, pnRangeMin, pnRangeMax);
    }
    HRESULT SetValue(int nValue)
    {
        return XuiSliderSetValue(m_hObj, nValue);
    }
    HRESULT GetValue(int *pnValue)
    {
        return XuiSliderGetValue(m_hObj, pnValue);
    }
    HRESULT SetStep(int nStep)
    {
        return XuiSliderSetStep(m_hObj, nStep);
    }
    HRESULT GetStep(int *pnStep)
    {
        return XuiSliderGetStep(m_hObj, pnStep);
    }
    HRESULT SetAccel(int nAccelInc, DWORD dwAccelTime)
    {
        return XuiSliderSetAccel(m_hObj, nAccelInc, dwAccelTime);
    }
    HRESULT GetAccel(int *pnAccelInc, DWORD *pdwAccelTime)
    {
        return XuiSliderGetAccel(m_hObj, pnAccelInc, pdwAccelTime);
    }
    CXuiSlider& operator =(HXUIOBJ hObj)
    {
        Attach(hObj);
        return *this;
    }
    void Attach(HXUIOBJ hObj)
    {
        m_hObj = hObj;
        ASSERT(IsValid());
        ASSERT(hObj == 0 || XuiDynamicCast(hObj, XuiFindClass(XUI_CLASS_SLIDER)));
    }
};

////////////////////////////////////////////////////////////////////////////////
// CXuiTabScene
// a class to simplify using the XuiTabScene* APIs
////////////////////////////////////////////////////////////////////////////////
class CXuiTabScene : public CXuiScene
{
public:
    CXuiTabScene()
    {
    }
    CXuiTabScene(HXUIOBJ hObj) : CXuiScene(hObj)
    {
    }
    HRESULT EnableTabbing(BOOL bEnable)
    {
        return XuiTabSceneEnableTabbing(m_hObj, bEnable);
    }
    HRESULT GotoNext()
    {
        return XuiTabSceneGotoNext(m_hObj);
    }
    HRESULT GotoPrev()
    {
        return XuiTabSceneGotoPrev(m_hObj);
    }
    HRESULT Goto(UINT uTab, BOOL bSetFocus=TRUE)
    {
        return XuiTabSceneGoto(m_hObj, uTab, bSetFocus);
    }
    UINT GetCount()
    {
        return XuiTabSceneGetCount(m_hObj);
    }
    BOOL CanUserTab()
    {
        return XuiTabSceneCanUserTab(m_hObj);
    }
    UINT GetCurrentTab(HXUIOBJ* phTab = NULL)
    {
        return XuiTabSceneGetCurrentTab(m_hObj, phTab);
    }
    CXuiTabScene& operator =(HXUIOBJ hObj)
    {
        Attach(hObj);
        return *this;
    }
    void Attach(HXUIOBJ hObj)
    {
        m_hObj = hObj;
        ASSERT(IsValid());
        ASSERT(hObj == 0 || XuiDynamicCast(hObj, XuiFindClass(XUI_CLASS_TABSCENE)));
    }    
};

////////////////////////////////////////////////////////////////////////////////
// CXuiEdit
// a class to simplify using the XuiEdit* APIs
////////////////////////////////////////////////////////////////////////////////
class CXuiEdit : public CXuiControl
{
public:
    CXuiEdit()
    {
    }
    CXuiEdit(HXUIOBJ hObj) : CXuiControl(hObj)
    {
    }
    HRESULT InsertText(UINT nIndex, LPCWSTR pszText)
    {
        return XuiEditInsertText(m_hObj, nIndex, pszText);
    }
    HRESULT DeleteText(UINT nIndex, UINT nCount)
    {
        return XuiEditDeleteText(m_hObj, nIndex, nCount);
    }
    HRESULT GetLineIndex(UINT nLine, UINT* pnIndex)
    {
        return XuiEditGetLineIndex(m_hObj, nLine, pnIndex);
    }   
    HRESULT SetTextFormatInfo(int nTextFormatInfo, XUITextFormatInfo* pTextFormatInfo)
    {
        return XuiEditSetTextFormatInfo(m_hObj, nTextFormatInfo, pTextFormatInfo);
    }
    HRESULT GetTextFormatInfo(int* pnTextFormatInfo, XUITextFormatInfo** ppTextFormatInfo)
    {
        return XuiEditGetTextFormatInfo(m_hObj, pnTextFormatInfo, ppTextFormatInfo);
    }    
    HRESULT SetTextLimit(UINT nLimit)
    {
        return XuiEditSetTextLimit(m_hObj, nLimit);
    }
    UINT GetTextLimit()
    {
        return XuiEditGetTextLimit(m_hObj);
    }
    HRESULT SetReadOnly(BOOL bReadOnly)
    {
        return XuiEditSetReadOnly(m_hObj, bReadOnly);
    }
    UINT GetReadOnly()
    {
        return XuiEditGetReadOnly(m_hObj);
    }    
    HRESULT SetCaretPosition(UINT nPosition)
    {
        return XuiEditSetCaretPosition(m_hObj, nPosition);
    }
    UINT GetCaretPosition()
    {
        return XuiEditGetCaretPosition(m_hObj);
    }
    UINT GetLineCount()
    {
        return XuiEditGetLineCount(m_hObj);
    }
    UINT GetVisibleLineCount(BOOL bSmoothScroll)
    {
        return XuiEditGetVisibleLineCount(m_hObj, bSmoothScroll);
    }
    UINT GetMaxVisibleLineCount()
    {
        return XuiEditGetMaxVisibleLineCount(m_hObj);
    }
    HRESULT SetTopLine(UINT nLineIndex, BOOL bSmoothScroll)
    {
        return XuiEditSetTopLine(m_hObj, nLineIndex, bSmoothScroll);
    }
    UINT GetTopLine(BOOL bSmoothScroll=FALSE)
    {
        return XuiEditGetTopLine(m_hObj, bSmoothScroll);
    }
    HRESULT GetVSmoothScroll(BOOL *pbEnabled, float *pfBaseSpeed, float *pfAccel, float *pfMaxSpeed)
    {
        return XuiEditGetSmoothScroll(m_hObj, XUI_SMOOTHSCROLL_VERTICAL, pbEnabled, pfBaseSpeed, pfAccel, pfMaxSpeed);        
    }
    HRESULT SetVSmoothScroll(BOOL bEnabled, float fBaseSpeed, float fAccel, float fMaxSpeed)
    {
        return XuiEditSetSmoothScroll(m_hObj, XUI_SMOOTHSCROLL_VERTICAL, bEnabled, fBaseSpeed, fAccel, fMaxSpeed);        
    }
    CXuiEdit& operator =(HXUIOBJ hObj)
    {
        Attach(hObj);
        return *this;
    }
    void Attach(HXUIOBJ hObj)
    {
        m_hObj = hObj;
        ASSERT(IsValid());
        ASSERT(hObj == 0 || XuiDynamicCast(hObj, XuiFindClass(XUI_CLASS_EDIT)));
    }    
};

#define XUI_IMPLEMENT_CLASS(className, xuiClassName, xuiBaseClassName)\
    static HRESULT Register()\
    {\
        HXUICLASS hClass;\
        XUIClass cls;\
        memset(&cls, 0x00, sizeof(cls));\
        cls.szClassName = xuiClassName;\
        cls.szBaseClassName = xuiBaseClassName;\
        cls.Methods.CreateInstance = (PFN_CREATEINST) (CreateInstance);\
        cls.Methods.DestroyInstance = (PFN_DESTROYINST) DestroyInstance;\
        cls.Methods.ObjectProc = (PFN_OBJECT_PROC) _ObjectProc;\
        cls.pPropDefs = _GetPropDef(&cls.dwPropDefCount);\
    \
        HRESULT hr = XuiRegisterClass(&cls, &hClass);\
        if (FAILED(hr))\
            return hr;\
        return S_OK;\
    }\
    \
    static HRESULT Unregister()\
    {\
        return XuiUnregisterClass(xuiClassName);\
    }\
    \
    \
    static HRESULT CreateInstance(HXUIOBJ hObj, void **ppvObj)\
    {\
        *ppvObj = NULL;\
        className *pThis = new className();\
        if (!pThis)\
            return E_OUTOFMEMORY;\
        pThis->m_hObj = hObj;\
        HRESULT hr = pThis->OnCreate();\
        if (FAILED(hr))\
        {\
            DestroyInstance(pThis);\
            return hr;\
        }\
        *ppvObj = pThis;\
        return S_OK;\
    }\
    \
    static HRESULT DestroyInstance(void *pvObj)\
    {\
        className *pThis = (className *) pvObj;\
        delete pThis;\
        return S_OK;\
    }

////////////////////////////////////////////////////////////////////////////////
// CXuiElementImplBase
// base class used to simplify implementing a XuiElement ObjectProc from C++
// Note that should not be used directly, but rather through CXuiElementImpl
// and other derived classes
////////////////////////////////////////////////////////////////////////////////
class CXuiElementImplBase
{
protected:
    virtual HRESULT ObjectProc(XUIMessage *pMessage)
    {
        return DispatchMessageMap(pMessage);
    }

    virtual HRESULT DispatchMessageMap(XUIMessage *pMessage)
    {
        return S_OK;
    }

    static HRESULT _ObjectProc(HXUIOBJ hObj, XUIMessage *pMessage, void *pvThis)
    {
        CXuiElementImplBase *pThis = (CXuiElementImplBase *) pvThis;

        return pThis->ObjectProc(pMessage);
    }

    static const XUIElementPropDef *_GetPropDef(DWORD *pdwCount)
    {
        *pdwCount = 0;
        return NULL;
    }

public:
    // OnCreate is called from the CreateInstance implementation
    // after the m_hObj has been set
    HRESULT OnCreate()
    {
        return S_OK;
    }
};

////////////////////////////////////////////////////////////////////////////////
// Helpers for implementing XuiElement derived XUI and message crackers
// The classes derive from CXuiElementImplBase, which provides a virtual
// implementation of the ObjectProc and well as DispatchMessageMap
// The message map handlers make it easier to create the DispatchMessageMap
// function.
// if you override ObjectProc directly, you should call the base class
// implementation to allow dispatching of the message map
// The CXuiElementImplBase dervied classes also derive from the appropriate
// use class (CXuiElement, CXuiControl, etc) this allows the use of m_hObj
// to the XUI C APIs as well as the helper member functions defined in the use
// classes
////////////////////////////////////////////////////////////////////////////////

class CXuiElementImpl : public CXuiElementImplBase, public CXuiElement
{
protected:
};

class CXuiControlImpl : public CXuiElementImplBase, public CXuiControl
{
protected:
};

class CXuiNavButtonImpl : public CXuiElementImplBase, public CXuiNavButton
{
protected:
};

class CXuiListItemImpl : public CXuiElementImplBase, public CXuiListItem
{
protected:
};

class CXuiListImpl : public CXuiElementImplBase, public CXuiList
{
protected:
};

class CXuiSceneImpl : public CXuiElementImplBase, public CXuiScene
{
protected:
};
class CXuiTabSceneImpl : public CXuiElementImplBase, public CXuiTabScene
{
protected:
};



////////////////////////////////////////////////////////////////////////////////
// CXuiModule
// an abstract class to manage the initialization of the XUI runtime, as well as
// initial scene loading, rendering and dispatching input.
////////////////////////////////////////////////////////////////////////////////
#ifdef XUIAPP_AUDITIONING
class CXuiModule : public IXUIAuditionHost
#else
class CXuiModule
#endif
{
public:
    CXuiModule();
    virtual ~CXuiModule();
    
    // These versions of Init and InitShared are deprecated. Xui clients should implement a
    // texture loader function and register it with the new versions of these member functions.
    __declspec(deprecated) HRESULT Init();
    __declspec(deprecated) HRESULT InitShared(IDirect3DDevice9 *pDevice, const D3DPRESENT_PARAMETERS *pPresentParams);

    HRESULT Init(PFN_XUITEXTURELOADER pfnTextureLoader, XUIInitParams *pInitParams=NULL);
    HRESULT InitShared(IDirect3DDevice9 *pDevice, const D3DPRESENT_PARAMETERS *pPresentParams, PFN_XUITEXTURELOADER pfnTextureLoader, XUIInitParams *pInitParams=NULL);
    virtual void Uninit();
    HRESULT RegisterDefaultTypeface(LPCWSTR szTypeface, LPCWSTR szLocator, LPCWSTR szReserved1=NULL);
    HRESULT RegisterTypeface(LPCWSTR szTypeface, LPCWSTR szLocator, LPCWSTR szReserved1=NULL);
    HRESULT LoadSkin(LPCWSTR szSkinFileName, LPCWSTR szVisualPrefix=NULL);
    HRESULT LoadFirstScene(LPCWSTR szBasePath, LPCWSTR szScenePath, void* pvInitData=NULL, HXUIOBJ *phObj=NULL, BYTE UserIndex = XUSER_INDEX_FOCUS);
    int Run();
    virtual void RunFrame();
    virtual void RunFrame(float fDeltaTime);
    virtual HRESULT ProcessInput();
    void Quit();
    void Resume();
    HXUIDC GetDC();
    HXUIOBJ GetRootObj();
    void GetRootObj(CXuiElement *pObj)
    {
        pObj->Attach(GetRootObj());
    }
    virtual HRESULT Render();

#ifdef XUIAPP_AUDITIONING
    // IXUIAuditionHost
    virtual HRESULT ProcessRemoteInput(const XINPUT_KEYSTROKE * pKeystroke);
    virtual HRESULT ChangeLocale();
    virtual HRESULT ChangeSkin(LPCWSTR szSkinResource, LPCWSTR szPrefix = NULL);
    virtual HRESULT ReloadResources();
#endif

protected:
    HXUIOBJ m_hObjRoot;     // root object currently displayed
    HXUIDC m_hDC;           // display context
    BOOL m_bXuiInited;
    BOOL m_bInitRuntime;
    BOOL m_bQuit;
    CXuiAnimRunner m_animRunner;
    BOOL m_bSkinLoaded;
    BOOL m_bDefaultTypefaceRegistered;

    virtual HRESULT RegisterXuiClasses()=0;
    virtual HRESULT UnregisterXuiClasses()=0;
    virtual HRESULT CreateMainCanvas();
    HRESULT InitCommon(XUIInitParams *pInitParams=NULL);
};

// Please note that CXuiApplication has been deprecated
// use CXuiModule instead
typedef __declspec(deprecated) CXuiModule CXuiApplication;


////////////////////////////////////////////////////////////////////////////////
// CXuiSceneCache
// A class that implements scene caching.  This class is used by the
// CXuiCachingModule class to handle most of work of caching scenes and properly
// handling navigation scenarios.
////////////////////////////////////////////////////////////////////////////////
#ifndef XM_REINIT_CACHED_SCENE
#define XM_REINIT_CACHED_SCENE          (XM_USER+0x1000)
#endif

class CXuiSceneCache
{
public:
    CXuiSceneCache();
    HRESULT Init();
    void Uninit();
    void RemoveAll();
    HRESULT CacheScene(LPCWSTR szFullPath, int nCacheLevels=0, HXUIOBJ *phScene=NULL);
    HRESULT CacheNavButtonTargets(HXUIOBJ hObj, int nCacheLevels);
    HXUIOBJ GetSceneFromCache(LPCWSTR szFullPath);
    BOOL ReleaseScene(HXUIOBJ hObj);
    BOOL RemoveScene(HXUIOBJ hScene);
    static void StopTimelines(HXUIOBJ hObj);
    void StartTimelines(HXUIOBJ hObj);
    BOOL HandleNavigation(HXUIOBJ hObjSource, XUINotify *pNotify);
    BOOL FindScene(HXUIOBJ hScene);
    BOOL FindScene(LPCWSTR szFullPath);
    HRESULT OnSkinChanged();
    BOOL IsDescendantOfCachedScene(HXUIOBJ hScene);
    
protected:
    struct SceneCacheEntry
    {
        DECLARE_XUI_ALLOC()

        WCHAR *m_szFileName;
        HXUIOBJ m_hObj;
        BOOL m_bInUse;
        SceneCacheEntry *m_pNext;
        SceneCacheEntry *m_pPrev;
        
        SceneCacheEntry()
        {
            m_szFileName = NULL;
            m_hObj = NULL;
            m_bInUse = FALSE;
            m_pNext = NULL;
            m_pPrev = NULL;
        }
        
        ~SceneCacheEntry()
        {
            if (m_szFileName)
                XuiFree(m_szFileName);
            if (m_hObj)
                XuiDestroyObject(m_hObj);
        }

        HRESULT SetFileName(LPCWSTR szFullPath)
        {
            if (m_szFileName)
                XuiFree(m_szFileName);

            int nSize = (wcslen(szFullPath)+1)*sizeof(WCHAR);
            m_szFileName = (WCHAR *) XuiAlloc(nSize);
            if (!m_szFileName)
                return E_OUTOFMEMORY;
            
            memcpy(m_szFileName, szFullPath, nSize);
            return S_OK;
        }
        
        void DetachObject()
        {
            m_hObj = NULL;
        }
    };

    HXUICLASS m_hClassControl;
    HXUICLASS m_hClassNavButton;
    HXUICLASS m_hClassScene;
    SceneCacheEntry *m_pHead;
    SceneCacheEntry *m_pTail;

    SceneCacheEntry *FindSceneInCache(HXUIOBJ hObj);
};


////////////////////////////////////////////////////////////////////////////////
// CXuiCachingModule
// Adds scene caching support to the CXuiModule class
// Call CacheScene to add a scene to the cache.  This should typically happen
// at load time.
// Nav button PressPath targets are handled automatically.
// When performing scene navigation directly, you can call GetSceneFromCache
// to retrieve a scene handle.  You can then use this handle as hFwd when
// calling XuiSceneNavigateForward.  The scene is released from the cache
// automatically when you navigate back from it.  If you need to release a scene
// at a specific point, you can call ReleaseScene.
// If you set a message filter function in a derived class, be sure to call
// CXuiCachingModule::_SceneCacheMessageFilter in your implementation.
// Note that you shouldn't destroy a scene that is being held in the cache
// directly.  In debug builds the message filter will assert on destruction
// of a cached scene.  You can call RemoveSceneFromCache before destroying
// a cached scene.
// When a cached scene is reused, it receives the XM_REINIT_CACHED_SCENE message
////////////////////////////////////////////////////////////////////////////////
class CXuiCachingModule : public CXuiModule
{
public:
    CXuiCachingModule();
    HRESULT Init(PFN_XUITEXTURELOADER pfnTextureLoader, XUIInitParams *pInitParams=NULL);
    virtual void Uninit();
    HRESULT CacheScene(LPCWSTR szFullPath, int nCacheLevels=0, HXUIOBJ *phScene=NULL);
    BOOL ReleaseScene(HXUIOBJ hObj);
    HXUIOBJ GetSceneFromCache(LPCWSTR szPath);
    BOOL RemoveSceneFromCache(HXUIOBJ hScene);

protected:
    HXUICLASS m_hClassNavButton;
    HXUICLASS m_hClassBackButton;
    HXUICLASS m_hClassScene;
    CXuiSceneCache m_sceneCache;
    static CXuiCachingModule *s_cachingModuelInst;

    HRESULT MessageFilter(HXUIOBJ hObj, XUIMessage *pMessage);
    static HRESULT _SceneCacheMessageFilter(HXUIOBJ hObj, XUIMessage *pMessage);
};


////////////////////////////////////////////////////////////////////////////////
// Message map helpers
// The following macros implement the DispatchMessageMap member function
// for a CXuiElementImplBase derived class.
// Note that all the message handling functions take a reference to a BOOL
// bHandled parameter.  This is a reference to the actual pMessage->bHandled
// field.  Setting this to TRUE indicates that you have fully handled the
// message and that the message must not be dispatched to XUI base class for
// further processing.
////////////////////////////////////////////////////////////////////////////////

#define XUI_BEGIN_MSG_MAP()\
    HRESULT DispatchMessageMap(XUIMessage *pMessage)\
    {

#define XUI_END_MSG_MAP()\
        return __super::DispatchMessageMap(pMessage);\
    }

// Sig: HRESULT OnMessage(XUIMessage *pMessage)
#define XUI_ON_MESSAGE(dwMsg, MemberFunc)\
    if (pMessage->dwMessage == dwMsg)\
    {\
        return MemberFunc(pMessage);\
    }

// Sig: HRESULT OnRender(XUIMessageRender *pRenderData, BOOL& bHandled)
#define XUI_ON_XM_RENDER(MemberFunc)\
    if (pMessage->dwMessage == XM_RENDER)\
    {\
        XUIMessageRender *pData = (XUIMessageRender *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnHittest(XUIMessageHittest *pHittestData, BOOL& bHandled)
#define XUI_ON_XM_HITTEST(MemberFunc)\
    if (pMessage->dwMessage == XM_HITTEST)\
    {\
        XUIMessageHittest *pData = (XUIMessageHittest *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnChildAdded(HXUIOBJ hChild, BOOL& bHandled)
#define XUI_ON_XM_CHILD_ADDED(MemberFunc)\
    if (pMessage->dwMessage == XM_CHILD_ADDED)\
    {\
        XUIMessageChildAdded *pData = (XUIMessageChildAdded *) pMessage->pvData;\
        return MemberFunc(pData->hChild, pMessage->bHandled);\
    }

// Sig: HRESULT OnParentChanged(XUIMessageParentChanged *pParentChangedData, BOOL& bHandled)
#define XUI_ON_XM_PARENT_CHANGED(MemberFunc)\
    if (pMessage->dwMessage == XM_PARENT_CHANGED)\
    {\
        XUIMessageParentChanged *pData = (XUIMessageParentChanged *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnGetBounds(XUIMessageGetBounds *pGetBoundsData, BOOL& bHandled)
#define XUI_ON_XM_GET_BOUNDS(MemberFunc)\
    if (pMessage->dwMessage == XM_GET_BOUNDS)\
    {\
        XUIMessageGetBounds *pData = (XUIMessageGetBounds *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnParentSizeChanging(XUIMessageSizeChanging *pSizeChangingData, BOOL& bHandled)
#define XUI_ON_XM_PARENT_SIZE_CHANGING(MemberFunc)\
    if (pMessage->dwMessage == XM_PARENT_SIZE_CHANGING)\
    {\
        XUIMessageSizeChanging *pData = (XUIMessageSizeChanging *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnSkinChanged(BOOL& bHandled)
#define XUI_ON_XM_SKIN_CHANGED(MemberFunc)\
    if (pMessage->dwMessage == XM_SKIN_CHANGED)\
    {\
        return MemberFunc(pMessage->bHandled);\
    }

// Sig: HRESULT OnDestroy()
#define XUI_ON_XM_DESTROY(MemberFunc)\
    if (pMessage->dwMessage == XM_DESTROY)\
    {\
        return MemberFunc();\
    }

// Sig: HRESULT OnWantFocus(BOOL& bWantFocus, BOOL& bHandled)
#define XUI_ON_XM_WANT_FOCUS(MemberFunc)\
    if (pMessage->dwMessage == XM_WANT_FOCUS)\
    {\
        XUIMessageWantFocus *pData = (XUIMessageWantFocus *) pMessage->pvData;\
        return MemberFunc(pData->bWantFocus, pMessage->bHandled);\
    }

// Sig: HRESULT OnSetFocus(HXUIOBJ hObjLosingFocus, BOOL& bHandled)
#define XUI_ON_XM_SET_FOCUS(MemberFunc)\
    if (pMessage->dwMessage == XM_SET_FOCUS)\
    {\
        XUIMessageSetFocus *pData = (XUIMessageSetFocus *) pMessage->pvData;\
        return MemberFunc(pData->hOther, pMessage->bHandled);\
    }

// Sig: HRESULT OnKillFocus(HXUIOBJ hObjGettingFocus, BOOL& bHandled)
#define XUI_ON_XM_KILL_FOCUS(MemberFunc)\
    if (pMessage->dwMessage == XM_KILL_FOCUS)\
    {\
        XUIMessageKillFocus *pData = (XUIMessageKillFocus *) pMessage->pvData;\
        return MemberFunc(pData->hOther, pMessage->bHandled);\
    }

// Sig: HRESULT OnNotify(XUINotify *pNotifyData, BOOL& bHandled)
#define XUI_ON_XM_NOTIFY(MemberFunc)\
    if (pMessage->dwMessage == XM_NOTIFY)\
    {\
        XUINotify *pData = (XUINotify *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnNotifyPressEx(HXUIOBJ hObjSource, XUINotifyPress *pNotifyPress, BOOL& bHandled)
#define XUI_ON_XM_NOTIFY_PRESS_EX(MemberFunc)\
    if (pMessage->dwMessage == XM_NOTIFY)\
    {\
        XUINotify *pNotify = (XUINotify *) pMessage->pvData;\
        if (pNotify->dwNotify == XN_PRESS)\
            return MemberFunc(pNotify->hObjSource, (XUINotifyPress *) pNotify->pvData, pMessage->bHandled);\
    }

// Sig: HRESULT OnNotifyPress(HXUIOBJ hObjSource, BOOL& bHandled)
#define XUI_ON_XM_NOTIFY_PRESS(MemberFunc)\
    if (pMessage->dwMessage == XM_NOTIFY)\
    {\
        XUINotify *pNotify = (XUINotify *) pMessage->pvData;\
        if (pNotify->dwNotify == XN_PRESS)\
            return MemberFunc(pNotify->hObjSource, pMessage->bHandled);\
    }

// Sig: HRESULT OnNotifyPressing(HXUIOBJ hObjSource, XUINotifyPressing *pNotifyPressingData, BOOL& bHandled)
#define XUI_ON_XM_NOTIFY_PRESSING(MemberFunc)\
    if (pMessage->dwMessage == XM_NOTIFY)\
    {\
        XUINotify *pNotify = (XUINotify *) pMessage->pvData;\
        if (pNotify->dwNotify == XN_PRESSING)\
        {\
            XUINotifyPressing *pNotifyPressing = (XUINotifyPressing *) pNotify->pvData;\
            return MemberFunc(pNotify->hObjSource, pNotifyPressing, pMessage->bHandled);\
        }\
    }

// Sig: HRESULT OnNotifySetFocus(HXUIOBJ hObjSource, XUINotifyFocus *pNotifyFocusData, BOOL& bHandled)
#define XUI_ON_XM_NOTIFY_SET_FOCUS(MemberFunc)\
    if (pMessage->dwMessage == XM_NOTIFY)\
    {\
        XUINotify *pNotify = (XUINotify *) pMessage->pvData;\
        if (pNotify->dwNotify == XN_SET_FOCUS)\
        {\
            XUINotifyFocus *pNotifyFocus = (XUINotifyFocus *) pNotify->pvData;\
            return MemberFunc(pNotify->hObjSource, pNotifyFocus, pMessage->bHandled);\
        }\
    }

// Sig: HRESULT OnNotifyKillFocus(HXUIOBJ hObjSource, XUINotifyFocus *pNotifyFocusData, BOOL& bHandled)
#define XUI_ON_XM_NOTIFY_KILL_FOCUS(MemberFunc)\
    if (pMessage->dwMessage == XM_NOTIFY)\
    {\
        XUINotify *pNotify = (XUINotify *) pMessage->pvData;\
        if (pNotify->dwNotify == XN_KILL_FOCUS)\
        {\
            XUINotifyFocus *pNotifyFocus = (XUINotifyFocus *) pNotify->pvData;\
            return MemberFunc(pNotify->hObjSource, pNotifyFocus, pMessage->bHandled);\
        }\
    }

// Sig: HRESULT OnNotifySelChanged(HXUIOBJ hObjSource, XUINotifySelChanged *pNotifySelChangedData, BOOL& bHandled)
#define XUI_ON_XM_NOTIFY_SELCHANGED(MemberFunc)\
    if (pMessage->dwMessage == XM_NOTIFY)\
    {\
        XUINotify *pNotify = (XUINotify *) pMessage->pvData;\
        if (pNotify->dwNotify == XN_SELCHANGED)\
        {\
            XUINotifySelChanged *pNotifySelChanged = (XUINotifySelChanged *) pNotify->pvData;\
            return MemberFunc(pNotify->hObjSource, pNotifySelChanged, pMessage->bHandled);\
        }\
    }

// Sig: HRESULT OnNotifyItemCheckChanged(HXUIOBJ hObjSource, XUINotifyItemCheckChanged *pNotifyItemCheckChangedData, BOOL& bHandled)
#define XUI_ON_XM_NOTIFY_ITEM_CHECKCHANGED(MemberFunc)\
    if (pMessage->dwMessage == XM_NOTIFY)\
    {\
        XUINotify *pNotify = (XUINotify *) pMessage->pvData;\
        if (pNotify->dwNotify == XN_ITEM_CHECKCHANGED)\
        {\
            XUINotifyItemCheckChanged *pNotifyItemCheckChanged = (XUINotifyItemCheckChanged *) pNotify->pvData;\
            return MemberFunc(pNotify->hObjSource, pNotifyItemCheckChanged, pMessage->bHandled);\
        }\
    }

// Sig: HRESULT OnVideoEnd(HXUIOBJ hObjSource, BOOL& bHandled)
#define XUI_ON_XM_NOTIFY_VIDEO_END(MemberFunc)\
    if (pMessage->dwMessage == XM_NOTIFY)\
    {\
        XUINotify *pNotify = (XUINotify *) pMessage->pvData;\
        if (pNotify->dwNotify == XN_VIDEO_END)\
            return MemberFunc(pNotify->hObjSource, pMessage->bHandled);\
    }

// Sig: HRESULT OnNotifyValueChanging(HXUIOBJ hObjSource, XUINotifyValueChanging *pNotifyValueChangingData, BOOL& bHandled)
#define XUI_ON_XM_NOTIFY_VALUE_CHANGING(MemberFunc)\
    if (pMessage->dwMessage == XM_NOTIFY)\
    {\
        XUINotify *pNotify = (XUINotify *) pMessage->pvData;\
        if (pNotify->dwNotify == XN_VALUE_CHANGING)\
        {\
            XUINotifyValueChanging *pNotifyValueChanging = (XUINotifyValueChanging *) pNotify->pvData;\
            return MemberFunc(pNotify->hObjSource, pNotifyValueChanging, pMessage->bHandled);\
        }\
    }

// Sig: HRESULT OnNotifyValueChanged(HXUIOBJ hObjSource, XUINotifyValueChanged *pNotifyValueChangedData, BOOL& bHandled)
#define XUI_ON_XM_NOTIFY_VALUE_CHANGED(MemberFunc)\
    if (pMessage->dwMessage == XM_NOTIFY)\
    {\
        XUINotify *pNotify = (XUINotify *) pMessage->pvData;\
        if (pNotify->dwNotify == XN_VALUE_CHANGED)\
        {\
            XUINotifyValueChanged *pNotifyValueChanged = (XUINotifyValueChanged *) pNotify->pvData;\
            return MemberFunc(pNotify->hObjSource, pNotifyValueChanged, pMessage->bHandled);\
        }\
    }

// Sig: HRESULT OnTimelineEnd(HXUIOBJ hObjSource, BOOL& bHandled)
#define XUI_ON_XM_TIMELINE_END(MemberFunc)\
    if (pMessage->dwMessage == XM_TIMELINE_END)\
    {\
        XUIMessageTimelineNotify *pData = (XUIMessageTimelineNotify *) pMessage->pvData;\
        return MemberFunc(pData->hObjSource, pMessage->bHandled);\
    }

// Sig: HRESULT OnSize(XUIMessageSizeChanging *pSizeData, BOOL& bHandled)
#define XUI_ON_XM_SIZE(MemberFunc)\
    if (pMessage->dwMessage == XM_SIZE)\
    {\
        XUIMessageSizeChanging *pData = (XUIMessageSizeChanging *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnGetShowState(BOOL& bShown, BOOL& bHandled)
#define XUI_ON_XM_GET_SHOWSTATE(MemberFunc)\
    if (pMessage->dwMessage == XM_GET_SHOWSTATE)\
    {\
        XUIMessageShowstate *pData = (XUIMessageShowstate *) pMessage->pvData;\
        return MemberFunc(pData->bShow, pMessage->bHandled);\
    }

// Sig: HRESULT OnSetShowState(BOOL bShown, BOOL& bHandled)
#define XUI_ON_XM_SET_SHOWSTATE(MemberFunc)\
    if (pMessage->dwMessage == XM_SET_SHOWSTATE)\
    {\
        XUIMessageShowstate *pData = (XUIMessageShowstate *) pMessage->pvData;\
        return MemberFunc(pData->bShow, pMessage->bHandled);\
    }

// Sig: HRESULT OnInit(XUIMessageInit *pInitData, BOOL& bHandled)
#define XUI_ON_XM_INIT(MemberFunc)\
    if (pMessage->dwMessage == XM_INIT)\
    {\
        XUIMessageInit *pData = (XUIMessageInit *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnTimelineStart(HXUIOBJ hObjSource, BOOL& bHandled)
#define XUI_ON_XM_TIMELINE_START(MemberFunc)\
    if (pMessage->dwMessage == XM_TIMELINE_START)\
    {\
        XUIMessageTimelineNotify *pData = (XUIMessageTimelineNotify *) pMessage->pvData;\
        return MemberFunc(pData->hObjSource, pMessage->bHandled);\
    }

// Sig: HRESULT OnGetBasePath(LPCWSTR *pszBasePath, BOOL& bHandled)
#define XUI_ON_XM_GET_BASEPATH(MemberFunc)\
    if (pMessage->dwMessage == XM_GET_BASEPATH)\
    {\
        XUIMessageGetSetBasePath *pData = (XUIMessageGetSetBasePath *) pMessage->pvData;\
        return MemberFunc(&pData->szPath, pMessage->bHandled);\
    }

// Sig: HRESULT OnSetBasePath(LPCWSTR szBasePath, BOOL& bHandled)
#define XUI_ON_XM_SET_BASEPATH(MemberFunc)\
    if (pMessage->dwMessage == XM_SET_BASEPATH)\
    {\
        XUIMessageGetSetBasePath *pData = (XUIMessageGetSetBasePath *) pMessage->pvData;\
        return MemberFunc(pData->szPath, pMessage->bHandled);\
    }

// Sig: HRESULT OnNavForward(XUIMessageNavForward *pNavForwardData, BOOL& bHandled)
#define XUI_ON_XM_NAV_FORWARD(MemberFunc)\
    if (pMessage->dwMessage == XM_NAV_FORWARD)\
    {\
        XUIMessageNavForward *pData = (XUIMessageNavForward *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnNavReturn(HXUIOBJ hSceneFrom, BOOL& bHandled)
#define XUI_ON_XM_NAV_RETURN(MemberFunc)\
    if (pMessage->dwMessage == XM_NAV_RETURN)\
    {\
        XUIMessageNavReturn *pData = (XUIMessageNavReturn *) pMessage->pvData;\
        return MemberFunc(pData->hScene, pMessage->bHandled);\
    }

// Sig: HRESULT OnGetBackScene(XUIMessageGetBackScene *pGetBackSceneData, BOOL& bHandled)
#define XUI_ON_XM_GET_BACKSCENE(MemberFunc)\
    if (pMessage->dwMessage == XM_GET_BACKSCENE)\
    {\
        XUIMessageGetBackScene *pData = (XUIMessageGetBackScene *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnSetBackScene(XUIMessageSetBackScene *pGetBackSceneData, BOOL& bHandled)
#define XUI_ON_XM_SET_BACKSCENE(MemberFunc)\
    if (pMessage->dwMessage == XM_SET_BACKSCENE)\
    {\
        XUIMessageSetBackScene *pData = (XUIMessageSetBackScene *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnTransitionStart(XUIMessageTransition *pTransData, BOOL& bHandled)
#define XUI_ON_XM_TRANSITION_START(MemberFunc)\
    if (pMessage->dwMessage == XM_TRANSITION_START)\
    {\
        XUIMessageTransition *pData = (XUIMessageTransition *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnTransitionEnd(XUIMessageTransition *pTransData, BOOL& bHandled)
#define XUI_ON_XM_TRANSITION_END(MemberFunc)\
    if (pMessage->dwMessage == XM_TRANSITION_END)\
    {\
        XUIMessageTransition *pData = (XUIMessageTransition *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnLocaleChanged(BOOL& bHandled)
#define XUI_ON_XM_LOCALE_CHANGED(MemberFunc)\
    if (pMessage->dwMessage == XM_LOCALE_CHANGED)\
    {\
        return MemberFunc(pMessage->bHandled);\
    }

// Sig: HRESULT OnChildRemoved(HXUIOBJ hChild, BOOL& bHandled)
#define XUI_ON_XM_CHILD_REMOVED(MemberFunc)\
    if (pMessage->dwMessage == XM_CHILD_REMOVED)\
    {\
        XUIMessageChildRemoved *pData = (XUIMessageChildRemoved *) pMessage->pvData;\
        return MemberFunc(pData->hChild, pMessage->bHandled);\
    }

// Sig: HRESULT OnChildSaving(XUIMessageChildSaving *pChildSavingData, BOOL& bHandled)
#define XUI_ON_XM_XM_CHILD_SAVING(MemberFunc)\
    if (pMessage->dwMessage == XM_CHILD_SAVING)\
    {\
        XUIMessageChildSaving *pData = (XUIMessageChildSaving *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnGetVisualPrefix(XUIMessageGetSetVisualPrefix *pVisualPrefixData, BOOL& bHandled)
#define XUI_ON_XM_GET_VISUALPREFIX(MemberFunc)\
    if (pMessage->dwMessage == XM_GET_VISUALPREFIX)\
    {\
        XUIMessageGetSetVisualPrefix *pData = (XUIMessageGetSetVisualPrefix *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnSetVisualPrefix(XUIMessageGetSetVisualPrefix *pVisualPrefixData, BOOL& bHandled)
#define XUI_ON_XM_SET_VISUALPREFIX(MemberFunc)\
    if (pMessage->dwMessage == XM_SET_VISUALPREFIX)\
    {\
        XUIMessageGetSetVisualPrefix *pData = (XUIMessageGetSetVisualPrefix *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnKeyDown(XUIMessageInput *pInputData, BOOL& bHandled)
#define XUI_ON_XM_KEYDOWN(MemberFunc)\
    if (pMessage->dwMessage == XM_KEYDOWN)\
    {\
        XUIMessageInput *pInputData = (XUIMessageInput *) pMessage->pvData;\
        return MemberFunc(pInputData, pMessage->bHandled);\
    }

// Sig: HRESULT OnKeyUp(XUIMessageInput *pInputData, BOOL& bHandled)
#define XUI_ON_XM_KEYUP(MemberFunc)\
    if (pMessage->dwMessage == XM_KEYUP)\
    {\
        XUIMessageInput *pInputData = (XUIMessageInput *) pMessage->pvData;\
        return MemberFunc(pInputData, pMessage->bHandled);\
    }

// Sig: HRESULT OnChar(XUIMessageChar *pInputData, BOOL& bHandled)
#define XUI_ON_XM_CHAR(MemberFunc)\
    if (pMessage->dwMessage == XM_CHAR)\
    {\
        XUIMessageChar *pInputData = (XUIMessageChar *) pMessage->pvData;\
        return MemberFunc(pInputData, pMessage->bHandled);\
    }

// Sig: HRESULT OnGetCheckState(BOOL& bChecked, BOOL& bHandled)
#define XUI_ON_XM_GET_CHECKSTATE(MemberFunc)\
    if (pMessage->dwMessage == XM_GET_CHECKSTATE)\
    {\
        XUIMessageGetCheckState *pData = (XUIMessageGetCheckState *) pMessage->pvData;\
        return MemberFunc(pData->bChecked, pMessage->bHandled);\
    }

// Sig: HRESULT OnSetCheckState(BOOL bChecked, BOOL& bHandled)
#define XUI_ON_XM_SET_CHECKSTATE(MemberFunc)\
    if (pMessage->dwMessage == XM_SET_CHECKSTATE)\
    {\
        XUIMessageSetCheckState *pData = (XUIMessageSetCheckState *) pMessage->pvData;\
        return MemberFunc(pData->bChecked, pMessage->bHandled);\
    }

// Sig: HRESULT OnGetEnableState(BOOL& bEnabled, BOOL& bHandled)
#define XUI_ON_XM_GET_ENABLESTATE(MemberFunc)\
    if (pMessage->dwMessage == XM_GET_ENABLESTATE)\
    {\
        XUIMessageEnableState *pData = (XUIMessageEnableState *) pMessage->pvData;\
        return MemberFunc(pData->bEnabled, pMessage->bHandled);\
    }

// Sig: HRESULT OnSetEnableState(BOOL bEnabled, BOOL& bHandled)
#define XUI_ON_XM_SET_ENABLESTATE(MemberFunc)\
    if (pMessage->dwMessage == XM_SET_ENABLESTATE)\
    {\
        XUIMessageEnableState *pData = (XUIMessageEnableState *) pMessage->pvData;\
        return MemberFunc(pData->bEnabled, pMessage->bHandled);\
    }

// Sig: HRESULT OnGetPressState(BOOL& bPressed, BOOL& bHandled)
#define XUI_ON_XM_GET_PRESSSTATE(MemberFunc)\
    if (pMessage->dwMessage == XM_GET_PRESSSTATE)\
    {\
        XUIMessageGetPressState *pData = (XUIMessageGetPressState *) pMessage->pvData;\
        return MemberFunc(pData->bPressed, pMessage->bHandled);\
    }

// Sig: HRESULT OnSetPressState(BOOL bPressed, BOOL& bHandled)
#define XUI_ON_XM_SET_PRESSSTATE(MemberFunc)\
    if (pMessage->dwMessage == XM_SET_PRESSSTATE)\
    {\
        XUIMessageSetPressState *pData = (XUIMessageSetPressState *) pMessage->pvData;\
        return MemberFunc(pData->bPressed, pMessage->bHandled);\
    }

// radio
/////#define XM_GET_CHECKED              2004
//////#define XM_SET_CHECKED              2005

// Sig: HRESULT OnPlayVisual(XUIMessagePlayVisual *pPlayVisualData, BOOL& bHandled)
#define XUI_ON_XM_PLAY_VISUAL(MemberFunc)\
    if (pMessage->dwMessage == XM_PLAY_VISUAL)\
    {\
        XUIMessagePlayVisual *pData = (XUIMessagePlayVisual *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnGetCurSel(XUIMessageGetCurSel *pGetCurSelData, BOOL& bHandled)
#define XUI_ON_XM_GET_CURSEL(MemberFunc)\
    if (pMessage->dwMessage == XM_GET_CURSEL)\
    {\
        XUIMessageGetCurSel *pData = (XUIMessageGetCurSel *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnSetCurSel(XUIMessageSetCurSel *pSetCurSelData, BOOL& bHandled)
#define XUI_ON_XM_SET_CURSEL(MemberFunc)\
    if (pMessage->dwMessage == XM_SET_CURSEL)\
    {\
        XUIMessageSetCurSel *pData = (XUIMessageSetCurSel *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

/////#define XM_GET_ITEMCONTROL          2009
/////#define XM_GET_TOPITEM              2010

// Sig: HRESULT OnSetTopItem(XUIMessageSetTopItem *pSetTopItemData, BOOL& bHandled)
#define XUI_ON_XM_SET_TOPITEM(MemberFunc)\
    if (pMessage->dwMessage == XM_SET_TOPITEM)\
    {\
        XUIMessageSetTopItem *pData = (XUIMessageSetTopItem *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnGetItemCountAll(XUIMessageGetItemCount *pGetItemCountData, BOOL& bHandled)
#define XUI_ON_XM_GET_ITEMCOUNT_ALL(MemberFunc)\
    if (pMessage->dwMessage == XM_GET_ITEMCOUNT && ((XUIMessageGetItemCount *) pMessage->pvData)->nType == XUI_ITEMCOUNT_ALL)\
    {\
        XUIMessageGetItemCount *pData = (XUIMessageGetItemCount *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnGetItemCountChecked(XUIMessageGetItemCount *pGetItemCountData, BOOL& bHandled)
#define XUI_ON_XM_GET_ITEMCOUNT_CHECKED(MemberFunc)\
    if (pMessage->dwMessage == XM_GET_ITEMCOUNT && ((XUIMessageGetItemCount *) pMessage->pvData)->nType == XUI_ITEMCOUNT_CHECKED)\
    {\
        XUIMessageGetItemCount *pData = (XUIMessageGetItemCount *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnInsertItems(XUIMessageInsertItems *pInsertItemsData, BOOL& bHandled)
#define XUI_ON_XM_INSERT_ITEMS(MemberFunc)\
    if (pMessage->dwMessage == XM_INSERT_ITEMS)\
    {\
        XUIMessageInsertItems *pData = (XUIMessageInsertItems *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnDeleteItems(XUIMessageDeleteItems *pDeleteItemsData, BOOL& bHandled)
#define XUI_ON_XM_DELETE_ITEMS(MemberFunc)\
    if (pMessage->dwMessage == XM_DELETE_ITEMS)\
    {\
        XUIMessageDeleteItems *pData = (XUIMessageDeleteItems *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnGetSourceDataText(XUIMessageGetSourceText *pGetSourceTextData, BOOL& bHandled)
#define XUI_ON_XM_GET_SOURCE_TEXT(MemberFunc)\
    if (pMessage->dwMessage == XM_GET_SOURCE_TEXT)\
    {\
        XUIMessageGetSourceText *pData = (XUIMessageGetSourceText *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnSetSourceDataText(XUIMessageSetSourceText *pSetSourceTextData, BOOL& bHandled)
#define XUI_ON_XM_SET_SOURCE_TEXT(MemberFunc)\
    if (pMessage->dwMessage == XM_SET_SOURCE_TEXT)\
    {\
        XUIMessageSetSourceText *pData = (XUIMessageSetSourceText *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnGetSourceDataImage(XUIMessageGetSourceImage *pGetSourceImageData, BOOL& bHandled)
#define XUI_ON_XM_GET_SOURCE_IMAGE(MemberFunc)\
    if (pMessage->dwMessage == XM_GET_SOURCE_IMAGE)\
    {\
        XUIMessageGetSourceImage *pData = (XUIMessageGetSourceImage *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnSetSourceDataImage(XUIMessageSetSourceImage *pSetSourceImageData, BOOL& bHandled)
#define XUI_ON_XM_SET_SOURCE_IMAGE(MemberFunc)\
    if (pMessage->dwMessage == XM_SET_SOURCE_IMAGE)\
    {\
        XUIMessageSetSourceImage *pData = (XUIMessageSetSourceImage *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnGetItemCheck(XUIMessageGetItemCheck *pGetItemCheckData, BOOL& bHandled)
#define XUI_ON_XM_GET_ITEMCHECK(MemberFunc)\
    if (pMessage->dwMessage == XM_GET_ITEMCHECK)\
    {\
        XUIMessageGetItemCheck *pData = (XUIMessageGetItemCheck *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnSetItemCheck(XUIMessageSetItemCheck *pSetItemCheckData, BOOL& bHandled)
#define XUI_ON_XM_SET_ITEMCHECK(MemberFunc)\
    if (pMessage->dwMessage == XM_SET_ITEMCHECK)\
    {\
        XUIMessageSetItemCheck *pData = (XUIMessageSetItemCheck *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnGetItemEnable(XUIMessageGetItemEnable *pGetItemEnableData, BOOL& bHandled)
#define XUI_ON_XM_GET_ITEMENABLE(MemberFunc)\
    if (pMessage->dwMessage == XM_GET_ITEMENABLE)\
    {\
        XUIMessageGetItemEnable *pData = (XUIMessageGetItemEnable *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnSetItemEnable(XUIMessageSetItemEnable *pSetItemEnableData, BOOL& bHandled)
#define XUI_ON_XM_SET_ITEMENABLE(MemberFunc)\
    if (pMessage->dwMessage == XM_SET_ITEMENABLE)\
    {\
        XUIMessageSetItemEnable *pData = (XUIMessageSetItemEnable *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnPress(XUIMessagePress *pPressData, BOOL& bHandled)
#define XUI_ON_XM_PRESS(MemberFunc)\
    if (pMessage->dwMessage == XM_PRESS)\
    {\
        XUIMessagePress *pData = (XUIMessagePress *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnControlNavigate(XUIMessageControlNavigate *pControlNavigateData, BOOL& bHandled)
#define XUI_ON_XM_CONTROL_NAVIGATE(MemberFunc)\
    if (pMessage->dwMessage == XM_CONTROL_NAVIGATE)\
    {\
        XUIMessageControlNavigate *pData = (XUIMessageControlNavigate *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnTimer(XUIMessageTimer *pXUIMessageTimer, BOOL& bHandled)
#define XUI_ON_XM_TIMER(MemberFunc)\
    if (pMessage->dwMessage == XM_TIMER)\
    {\
        XUIMessageTimer *pData = (XUIMessageTimer *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnDiscardResources(XUIMessageDiscardResources *pDiscardResourcesData, BOOL& bHandled)
#define XUI_ON_XM_DISCARD_RESOURCES(MemberFunc)\
    if (pMessage->dwMessage == XM_DISCARD_RESOURCES)\
    {\
        XUIMessageDiscardResources *pData = (XUIMessageDiscardResources *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnGetDiscardFlags(XUIMessageGetDiscardFlags *pGetDiscardFlagsData, BOOL& bHandled)
#define XUI_ON_XM_GET_DISCARD_FLAGS(MemberFunc)\
    if (pMessage->dwMessage == XM_GET_DISCARD_FLAGS)\
    {\
        XUIMessageGetDiscardFlags *pData = (XUIMessageGetDiscardFlags *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

// Sig: HRESULT OnMsgReturn(XUIMessageMessageBoxReturn *pXUIMessageMessageBoxReturn, BOOL& bHandled)
#define XUI_ON_XM_MSG_RETURN(MemberFunc)\
    if (pMessage->dwMessage == XM_MSG_RETURN)\
    {\
        XUIMessageMessageBoxReturn *pData = (XUIMessageMessageBoxReturn *) pMessage->pvData;\
        return MemberFunc(pData, pMessage->bHandled);\
    }

/////#define XM_SET_SCROLLEND_STATE      2021

// Sig: HRESULT OnEnterTab(BOOL& bHandled)
#define XUI_ON_XM_ENTER_TAB(MemberFunc)\
    if (pMessage->dwMessage == XM_ENTER_TAB)\
    {\
        return MemberFunc(pMessage->bHandled);\
    }

// Sig: HRESULT OnLeaveTab(BOOL& bHandled)
#define XUI_ON_XM_LEAVE_TAB(MemberFunc)\
    if (pMessage->dwMessage == XM_LEAVE_TAB)\
    {\
        return MemberFunc(pMessage->bHandled);\
    }

// Sig: HRESULT OnUser(XUIMessage *pMessage, BOOL& bHandled)
#define XUI_ON_XM_USER(MemberFunc)\
    if (pMessage->dwMessage >= XM_USER)\
    {\
        return MemberFunc(pMessage, pMessage->bHandled);\
    }

#ifndef XUI_DISABLE_INLINES
#include "xuiapp.inl"

#endif // XUI_DISABLE_INLINES
