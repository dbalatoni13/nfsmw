/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       xuielement.h
 *  Content:    Xbox 360 UI element and controls defines, structures and
 *              functions
 *
 ****************************************************************************/

#ifndef __XUIELEMENT_H__
#define __XUIELEMENT_H__

#pragma warning(push)
#pragma warning(disable: 4201) // unnamed structure / union

#ifdef __cplusplus
extern "C" {
#endif

// C type definitions
typedef struct XUIObjectData XUIObjectData;
typedef struct XUINamedframeData XUINamedframeData;
typedef struct XUITimelineData XUITimelineData;
typedef struct XUISubtimelineData XUISubtimelineData;
typedef struct XUITimelinePropPath XUITimelinePropPath;
typedef struct XUIKeyframeData XUIKeyframeData;
typedef struct XUIPropertyData XUIPropertyData;
typedef struct XUIAllocatedArray XUIAllocatedArray;
typedef struct XUIContentInfo XUIContentInfo;
typedef struct XUIRenderStruct XUIRenderStruct;
typedef enum XUI_FILL_TYPE XUI_FILL_TYPE;
typedef enum XUI_LISTITEM_LAYOUT XUI_LISTITEM_LAYOUT;
typedef enum XUI_SCROLLEND_DIRECTION XUI_SCROLLEND_DIRECTION;
typedef enum XUI_CONTENT_TYPE XUI_CONTENT_TYPE;

// Class names
#define XUI_CLASS_ELEMENT          L"XuiElement"
#define XUI_CLASS_CANVAS           L"XuiCanvas"
#define XUI_CLASS_FIGURE           L"XuiFigure"
#define XUI_CLASS_CONTROL          L"XuiControl"
#define XUI_CLASS_SCENE            L"XuiScene"
#define XUI_CLASS_BUTTON           L"XuiButton"
#define XUI_CLASS_NAVBUTTON        L"XuiNavButton"
#define XUI_CLASS_BACKBUTTON       L"XuiBackButton"
#define XUI_CLASS_CHECKBOX         L"XuiCheckbox"
#define XUI_CLASS_RADIOBUTTON      L"XuiRadioButton"
#define XUI_CLASS_RADIOGROUP       L"XuiRadioGroup"
#define XUI_CLASS_TEXT             L"XuiText"
#define XUI_CLASS_TEXT_PRESENTER   L"XuiTextPresenter"
#define XUI_CLASS_VISUAL           L"XuiVisual"
#define XUI_CLASS_GROUP            L"XuiGroup"
#define XUI_CLASS_SOUND            L"XuiSound"
#define XUI_CLASS_SOUND_XACT       L"XuiSoundXACT"
#define XUI_CLASS_SOUND_XAUDIO     L"XuiSoundXAudio"
#define XUI_CLASS_VIDEO            L"XuiVideo"
#define XUI_CLASS_TRANSITION       L"XuiTransition"
#define XUI_CLASS_LIST             L"XuiList"
#define XUI_CLASS_COMMONLIST       L"XuiCommonList"
#define XUI_CLASS_LISTITEM         L"XuiListItem"
#define XUI_CLASS_SCROLLEND        L"XuiScrollEnd"
#define XUI_CLASS_PROGRESSBAR      L"XuiProgressBar"
#define XUI_CLASS_SLIDER           L"XuiSlider"
#define XUI_CLASS_CARET            L"XuiCaret"
#define XUI_CLASS_EDIT             L"XuiEdit"
#define XUI_CLASS_IMAGE            L"XuiImage"
#define XUI_CLASS_IMAGE_PRESENTER  L"XuiImagePresenter"
#define XUI_CLASS_MESSAGEBOX       L"XuiMessageBox"
#define XUI_CLASS_LABEL            L"XuiLabel"
#define XUI_CLASS_TABSCENE         L"XuiTabScene"
#define XUI_CLASS_MESSAGEBOXBUTTON L"XuiMessageBoxButton"
#define XUI_CLASS_NINE_GRID        L"XuiNineGrid"

// Allocated array:
struct XUIAllocatedArray
{
    int m_nCount;           // number of elements in the array
    DWORD m_dwElemSize;     // size of each element in bytes
    // this structure is followed in memory by the actual elements

#ifdef __cplusplus
    void *GetElement(UINT nIndex)
    {
        BYTE *pbData = (BYTE *) (this+1);
        return pbData + nIndex*m_dwElemSize;
    }
#endif
};

typedef void (APIENTRY *PFN_XUI_CONSTRUCT_ELEM)(void *pv);
typedef void (APIENTRY *PFN_XUI_DESTRUCT_ELEM)(void *pv);

XUIAllocatedArray * APIENTRY XUIAllocatedArray_Allocate(int nCount, DWORD dwElemSize, PFN_XUI_CONSTRUCT_ELEM pfnConstruct);
void APIENTRY XUIAllocatedArray_Free(XUIAllocatedArray *pArray, PFN_XUI_DESTRUCT_ELEM pfnDestruct);

// Properties: property data
void APIENTRY XUIPropertyData_Construct(XUIPropertyData* pThis);
void APIENTRY XUIPropertyData_Destruct(XUIPropertyData* pThis);
void APIENTRY XUIPropertyData_Reset(XUIPropertyData* pThis);

#define XUI_PROPERTY_DATA_CUSTOM_STR        1
#define XUI_PROPERTY_DATA_BIN               2
#define XUI_PROPERTY_DATA_COMPOUND          4
#define XUI_PROPERTY_DATA_ALLOC_VAL         8
#define XUI_PROPERTY_DATA_NOFREE            16



struct XUIPropertyData
{
    DECLARE_XUI_ALLOC()

    USHORT m_uFlags;
    USHORT m_uIndex;

    const XUIElementPropDef *m_pDef;

    union
    {
        LPCWSTR m_szCustomValString;

        struct XUICustomValBin
        {
            BYTE * _pb;                     // the first 4 bytes are the length of the data
        } m_CustomValBin;


        XUIAllocatedArray *m_pCompoundPropArray;    // allocated array of XUIPropertyData for our compound props

        DWORD m_dwVal;
        float m_fVal;

        DWORD m_dwIndexIntoTable;           // index into appropriate table

        XUIElementPropVal *m_pVal;          // used if XUI_PROPERTY_DATA_ALLOC_VAL is set
    };

    void *m_pLoadData;                      // private load data

#ifdef __cplusplus
    XUIPropertyData()
    {
        XUIPropertyData_Construct(this);
    }
    ~XUIPropertyData()
    {
        XUIPropertyData_Destruct(this);
    }
    void Reset()
    {
        XUIPropertyData_Reset(this);
    }
#endif
};

// Properties: property value defaults
const XUIElementPropVal* APIENTRY XuiDefault_EmptyString();
const XUIElementPropVal* APIENTRY XuiDefault_False();
const XUIElementPropVal* APIENTRY XuiDefault_True();
const XUIElementPropVal* APIENTRY XuiDefault_Vector();
const XUIElementPropVal* APIENTRY XuiDefault_ScaleVector();
const XUIElementPropVal* APIENTRY XuiDefault_Quaternion();
const XUIElementPropVal* APIENTRY XuiDefault_FloatOne();
const XUIElementPropVal* APIENTRY XuiDefault_UnsignedZero();
const XUIElementPropVal* APIENTRY XuiDefault_IntegerZero();

// Object data
void APIENTRY XUIObjectData_Construct(XUIObjectData* pThis);
void APIENTRY XUIObjectData_Destruct(XUIObjectData* pThis);
DWORD APIENTRY XUIObjectData_AddRef(XUIObjectData* pThis);
DWORD APIENTRY XUIObjectData_Release(XUIObjectData* pThis);
void APIENTRY XUIObjectData_GetId(XUIObjectData *pThis, LPCWSTR *ppszId);

#define XUI_OBJDATA_F_NONE      0
#define XUI_OBJDATA_F_INCREATE  1
#define XUI_OBJDATA_F_NOFREE    2
#define XUI_OBJDATA_F_OWNED     4
#define XUI_OBJDATA_F_RELCHILD  8
#define XUI_OBJDATA_F_FREEPROPS 16

struct XUIObjectData
{
    DECLARE_XUI_ALLOC()

    LPCWSTR m_szClassName;
    XUIAllocatedArray *m_pPropArray;

    XUIAllocatedArray *m_pChildArray;

    XUINamedframeData *m_pNamedFrames;
    int m_nNumNamedFrames;
    XUINamedFrame *m_pRuntimeNamedFrames;   // cached runtime version
    DWORD m_dwRefs;

    int m_nNumTimelines;
    XUISubtimelineData *m_pTimelines;
    DWORD m_dwFlags;

    void *m_pLoadData;

#ifdef __cplusplus

    XUIObjectData()
    {
        XUIObjectData_Construct(this);
    }
    ~XUIObjectData()
    {
        XUIObjectData_Destruct(this);
    }
    DWORD AddRef()
    {
        return XUIObjectData_AddRef(this);
    }
    DWORD Release()
    {
        return XUIObjectData_Release(this);
    }

    int GetNumProps()
    {
        if (!m_pPropArray)
            return 0;
        return m_pPropArray->m_nCount;
    }

    XUIPropertyData *GetPropAddr(int nProp)
    {
        return (XUIPropertyData *) m_pPropArray->GetElement(nProp);
    }

    XUIObjectData *GetChild(int nChild)
    {
        return (XUIObjectData *) m_pChildArray->GetElement(nChild);
    }
    int GetNumChildren()
    {
        if (!m_pChildArray)
            return 0;
        return m_pChildArray->m_nCount;
    }
#endif
};

HRESULT APIENTRY XuiLoadObjectDataFromBinary(LPCWSTR szFIleName, XUIObjectData **ppObjData);

// Animations: key frames
void APIENTRY XUIKeyframeData_Construct(XUIKeyframeData* pThis);
void APIENTRY XUIKeyframeData_Destruct(XUIKeyframeData* pThis);
void APIENTRY XUIKeyframeData_Reset(XUIKeyframeData* pThis);
void APIENTRY XUIKeyframeData_Detach(XUIKeyframeData* pThis, XUIKeyframeData *pOther);
HRESULT APIENTRY XUIKeyframeData_CopyTo(XUIKeyframeData* pThis, XUIKeyframeData *pOther);

#define XUI_KEYFRAMEDATA_F_NONE         0x0
#define XUI_KEYFRAMEDATA_F_NOFREE       0x1

#define XUI_KEYFRAMEDATA_F_INTERP_MASK  0x6
#define XUI_KEYFRAMEDATA_F_INTERP_SHIFT 0x1
#define XUI_KEYFRAMEDATA_F_INTERP_NONE  0x2     // no interpolation
#define XUI_KEYFRAMEDATA_F_INTERP_EASE  0x4     // ease interpolation

struct XUIKeyframeData
{
    DECLARE_XUI_ALLOC()

    int m_nTime;

    BYTE m_bFlags;
    char m_nEaseIn;     // value from -100 to 100 inclusive
    char m_nEaseOut;     // value from -100 to 100 inclusive
    char m_nEaseScale;  // value from 0 to 100 inclusive

    int m_nNumPropVals;
    XUIElementPropVal *m_pPropVals;

#ifdef __cplusplus

    XUIKeyframeData()
    {
        XUIKeyframeData_Construct(this);
    }
    ~XUIKeyframeData()
    {
        XUIKeyframeData_Destruct(this);
    }
    void Reset()
    {
        XUIKeyframeData_Reset(this);
    }
    void Detach(XUIKeyframeData *pOther)
    {
        XUIKeyframeData_Detach(this, pOther);
    }
    HRESULT CopyTo(XUIKeyframeData *pOther)
    {
        return XUIKeyframeData_CopyTo(this, pOther);
    }
    XUI_INTERPOLATE GetInterpolationType()
    {
        return (XUI_INTERPOLATE) ((m_bFlags & XUI_KEYFRAMEDATA_F_INTERP_MASK) >> XUI_KEYFRAMEDATA_F_INTERP_SHIFT);
    }
    void SetInterpolationType(XUI_INTERPOLATE nInterpType)
    {
        m_bFlags = (BYTE) ((m_bFlags & ~XUI_KEYFRAMEDATA_F_INTERP_MASK) | (nInterpType << XUI_KEYFRAMEDATA_F_INTERP_SHIFT));
    }
#endif
};

// Animations: timeline property paths
void APIENTRY XUITimelinePropPath_Construct(XUITimelinePropPath* pThis);
void APIENTRY XUITimelinePropPath_Destruct(XUITimelinePropPath* pThis);
void APIENTRY XUITimelinePropPath_Clear(XUITimelinePropPath* pThis);
HRESULT APIENTRY XUITimelinePropPath_CopyTo(XUITimelinePropPath* pThis, XUITimelinePropPath* pOther);

#define XUI_PROPPATH_F_NONE         0
#define XUI_PROPPATH_F_NOFREE       1

struct XUITimelinePropPath
{
    DECLARE_XUI_ALLOC()

    DWORD m_dwFlags;
    int m_nPathDepth;
    XUIElementPropDef **m_ppPropDefs;

#ifdef __cplusplus

    XUITimelinePropPath()
    {
        XUITimelinePropPath_Construct(this);
    }
    ~XUITimelinePropPath()
    {
        XUITimelinePropPath_Destruct(this);
    }
    void Clear()
    {
        XUITimelinePropPath_Clear(this);
    }
    HRESULT CopyTo(XUITimelinePropPath *pOther) const
    {
        return XUITimelinePropPath_CopyTo(const_cast<XUITimelinePropPath*>(this), pOther);
    }
#endif
};

// Animations: sub-timelines
void APIENTRY XUISubtimelineData_Construct(XUISubtimelineData* pThis);
void APIENTRY XUISubtimelineData_Destruct(XUISubtimelineData* pThis);
void APIENTRY XUISubtimelineData_Clear(XUISubtimelineData* pThis);
HRESULT APIENTRY XUISubtimelineData_SetId(XUISubtimelineData* pThis, LPCWSTR szId);
HRESULT APIENTRY XUISubtimelineData_CopyTo(XUISubtimelineData* pThis, XUISubtimelineData *pOther);

#define XUI_SUBTIMELINEDATA_F_NONE      0
#define XUI_SUBTIMELINEDATA_F_NOFREE    1

struct XUISubtimelineData
{
    DECLARE_XUI_ALLOC()

    DWORD m_dwFlags;

    LPCWSTR m_szId;
    int m_nNumProps;

    XUITimelinePropPath *m_pPropPaths;
    int *m_pnIndices;

    int m_nNumKeyFrames;
    XUIKeyframeData *m_pKeyFrames;
    XUIKeyFrame *m_pRuntimeKeyFrames;

#ifdef __cplusplus

    XUISubtimelineData()
    {
        XUISubtimelineData_Construct(this);
    }
    ~XUISubtimelineData()
    {
        XUISubtimelineData_Destruct(this);
    }
    void Clear()
    {
        XUISubtimelineData_Clear(this);
    }
    HRESULT SetId(LPCWSTR szId)
    {
        return XUISubtimelineData_SetId(this, szId);
    }
    HRESULT CopyTo(XUISubtimelineData *pOther)
    {
        return XUISubtimelineData_CopyTo(this, pOther);
    }
#endif
};

// Animations: named frames
void APIENTRY XUINamedframeData_Construct(XUINamedframeData* pThis);
void APIENTRY XUINamedframeData_Destruct(XUINamedframeData* pThis);
void APIENTRY XUINamedframeData_Reset(XUINamedframeData* pThis);
HRESULT APIENTRY XUINamedframeData_CopyTo(XUINamedframeData* pThis, XUINamedframeData *pOther);
void APIENTRY XUINamedframeData_DetachInto(XUINamedframeData* pThis, XUINamedframeData *pOther);
void APIENTRY XUINamedframeData_Clear(XUINamedframeData* pThis);

struct XUINamedframeData
{
    DECLARE_XUI_ALLOC()

    LPCWSTR m_szName;
    DWORD m_dwFrame;
    XUI_NAMEDFRAME_COMMAND m_nCommand;
    LPCWSTR m_szCommandParams;

#ifdef __cplusplus

    XUINamedframeData()
    {
        XUINamedframeData_Construct(this);
    }
    ~XUINamedframeData()
    {
        XUINamedframeData_Destruct(this);
    }
    void Reset()
    {
        XUINamedframeData_Reset(this);
    }
    HRESULT CopyTo(XUINamedframeData *pOther)
    {
        return XUINamedframeData_CopyTo(this, pOther);
    }
    void DetachInto(XUINamedframeData *pOther)
    {
        XUINamedframeData_DetachInto(this, pOther);
    }
    void Clear()
    {
        XUINamedframeData_Clear(this);
    }
#endif
};

// Creation from data APIs
#define XUI_INIT_NOW    TRUE
#define XUI_INIT_LATER  FALSE
#define XUI_RUN_NOW     TRUE
#define XUI_RUN_LATER   FALSE
HRESULT APIENTRY XuiCreateObjectFromData(XUIObjectData *pObjData, HXUIOBJ *phObj, BOOL bInitNow, BOOL bRunNow, BOOL bCacheKeyFrames);
HRESULT APIENTRY XuiCreateTimelineFromData(HXUIOBJ hObj, XUIObjectData *pObjData, XUITimeline *pTimeline, BOOL bCacheKeyFrames, XUIObjectData *pObjDataOwner);
HRESULT APIENTRY XuiCreateSubtimelineFromData(HXUIOBJ hOwnerObj, XUISubtimelineData *pSubtimelineData, XUISubtimeline *pSubtimeline, BOOL bCacheKeyFrames, XUIObjectData *pOwner);

// XuiElement property names
#define XUI_ELEM_PROP_ID        L"Id"
#define XUI_ELEM_PROP_WIDTH     L"Width"
#define XUI_ELEM_PROP_HEIGHT    L"Height"
#define XUI_ELEM_PROP_POSITION  L"Position"
#define XUI_ELEM_PROP_SCALE     L"Scale"
#define XUI_ELEM_PROP_ROTATION  L"Rotation"

// XuiElement APIs
HXUIOBJ APIENTRY XuiElementFromPoint(HXUIOBJ hObj, POINT pt);
HRESULT APIENTRY XuiElementAddChild(HXUIOBJ hObj, HXUIOBJ hChild);
HRESULT APIENTRY XuiElementUnlink(HXUIOBJ hObj);
HRESULT APIENTRY XuiElementGetParent(HXUIOBJ hObj, HXUIOBJ *phParent);
HRESULT APIENTRY XuiElementGetFirstChild(HXUIOBJ hObj, HXUIOBJ *phFirstChild);
HRESULT APIENTRY XuiElementGetLastChild(HXUIOBJ hObj, HXUIOBJ *phLastChild);
HRESULT APIENTRY XuiElementGetNext(HXUIOBJ hObj, HXUIOBJ *phNext);
HRESULT APIENTRY XuiElementGetPrev(HXUIOBJ hObj, HXUIOBJ *phPrev);
HRESULT APIENTRY XuiElementInsertChild(HXUIOBJ hObj, HXUIOBJ hChild, HXUIOBJ hChildPrev, HXUIOBJ hChildNext);
HRESULT APIENTRY XuiElementGetXForm(HXUIOBJ hObj, D3DXMATRIX *pmatXForm);
HRESULT APIENTRY XuiElementGetFullXForm(HXUIOBJ hObj, D3DXMATRIX *pmatXForm);
HRESULT APIENTRY XuiElementSetBounds(HXUIOBJ hObj, float fWidth, float fHeight);
HRESULT APIENTRY XuiElementGetPosition(HXUIOBJ hObj, D3DXVECTOR3 *pvPos);
HRESULT APIENTRY XuiElementSetPosition(HXUIOBJ hObj, const D3DXVECTOR3 *pvPos);
HRESULT APIENTRY XuiElementGetScale(HXUIOBJ hObj, D3DXVECTOR3 *pvScale);
HRESULT APIENTRY XuiElementSetScale(HXUIOBJ hObj, const D3DXVECTOR3 *pvScale);
HRESULT APIENTRY XuiElementGetRotation(HXUIOBJ hObj, D3DXQUATERNION *pquatRot);
HRESULT APIENTRY XuiElementSetRotation(HXUIOBJ hObj, const D3DXQUATERNION *pquatRot);
HRESULT APIENTRY XuiElementGetOpacity(HXUIOBJ hObj, float *pfOpacity);
HRESULT APIENTRY XuiElementSetOpacity(HXUIOBJ hObj, float fOpacity);
HRESULT APIENTRY XuiElementSetPivot(HXUIOBJ hObj, const D3DXVECTOR3 *pvPivot);
HRESULT APIENTRY XuiElementGetPivot(HXUIOBJ hObj, D3DXVECTOR3 *pvPivot);
HRESULT APIENTRY XuiElementGetId(HXUIOBJ hObj, LPCWSTR *pszId);
HRESULT APIENTRY XuiElementGetChildById(HXUIOBJ hObj, LPCWSTR szId, HXUIOBJ *phChild);
BOOL    APIENTRY XuiElementIsDescendant(HXUIOBJ hObj, HXUIOBJ hDescendant);
HRESULT APIENTRY XuiElementSetUserData(HXUIOBJ hObj, void *pvUserData);
HRESULT APIENTRY XuiElementGetUserData(HXUIOBJ hObj, void **ppvUserData);
HRESULT APIENTRY XuiElementGetLoadId(HXUIOBJ hObj, UINT *puLoadId);

#define XUI_ANCHOR_NONE     0
#define XUI_ANCHOR_LEFT     1
#define XUI_ANCHOR_TOP      2
#define XUI_ANCHOR_RIGHT    4
#define XUI_ANCHOR_BOTTOM   8
#define XUI_ANCHOR_HCENTER  16
#define XUI_ANCHOR_VCENTER  32

HRESULT APIENTRY XuiElementGetAnchor(HXUIOBJ hObj, UINT *puAnchor);

struct XUIRenderStruct
{
    DWORD dwOrigColorFactor;
    XUI_BLEND_MODE nOrigBlendMode;
    DWORD dwOrigColorWriteFlags;
};

HRESULT APIENTRY XuiElementBeginRender(HXUIOBJ hObj, XUIMessageRender *pRenderData, XUIRenderStruct *pRenderStruct);
HRESULT APIENTRY XuiElementEndRender(HXUIOBJ hObj, XUIMessageRender *pRenderData, XUIRenderStruct *pRenderStruct);
HRESULT APIENTRY XuiElementRenderChildren(HXUIOBJ hObj, XUIMessageRender *pRenderData);

HRESULT APIENTRY XuiElementSetBlendMode(HXUIOBJ hObj, XUI_BLEND_MODE nBlendMode);
HRESULT APIENTRY XuiElementGetBlendMode(HXUIOBJ hObj, XUI_BLEND_MODE *pnBlendMode);
HRESULT APIENTRY XuiElementSetColorWriteFlags(HXUIOBJ hObj, DWORD dwColorWriteFlags);
HRESULT APIENTRY XuiElementGetColorWriteFlags(HXUIOBJ hObj, DWORD *pdwColorWriteFlags);
void    APIENTRY XuiLockTree(BOOL bLock);
BOOL    APIENTRY XuiIsTreeLocked();
HRESULT APIENTRY XuiElementEnableInput(HXUIOBJ hObj, BOOL bEnable);
BOOL    APIENTRY XuiElementInputEnabled(HXUIOBJ hObj);
HRESULT APIENTRY XuiElementDiscardResources(HXUIOBJ hObj, DWORD dwFlags);
HRESULT APIENTRY XuiElementTreeDelayedInitialization(HXUIOBJ hObj, void* pvInitData);


// XuiElement APIs: timelines
HRESULT APIENTRY XuiElementSetTimeline(HXUIOBJ hObj, XUITimeline *pTimeline);
HRESULT APIENTRY XuiElementSetSubtimeline(HXUIOBJ hObj, XUISubtimeline *pSubtimeline, int nIndex);
HRESULT APIENTRY XuiElementGetTimeline(HXUIOBJ hObj, XUITimeline **ppTimeline);
HRESULT APIENTRY XuiElementStopTimeline(HXUIOBJ hObj, BOOL bStopChildren);
HRESULT APIENTRY XuiElementPlayTimeline(HXUIOBJ hObj, int nStartFrame, int nInitialFrame, int nEndFrame, BOOL bLoop, BOOL bRecurse);
HRESULT APIENTRY XuiElementFindNamedFrame(HXUIOBJ hObj, LPCWSTR szFrameName, int *pnFrame);
HRESULT APIENTRY XuiElementDisallowRecursiveTimelineControl(HXUIOBJ hObj, BOOL fDisallow);

// XuiElement APIs: input

#define VK_PAD_A_OR_START       0x5840
#define VK_PAD_B_OR_BACK        0x5841
#define VK_NONE                 0x5842

HRESULT APIENTRY XuiElementInitFocus(HXUIOBJ hObj, BOOL bPlayInitialFocus);
HRESULT APIENTRY XuiElementSetFocus(HXUIOBJ hObj);
HXUIOBJ APIENTRY XuiElementGetFocus();

HRESULT APIENTRY XuiElementWantFocus(HXUIOBJ hObj, BOOL *pbWantFocus);
BOOL    APIENTRY XuiElementHasFocus(HXUIOBJ hObj);
BOOL    APIENTRY XuiElementTreeHasFocus(HXUIOBJ hObj);
HXUIOBJ APIENTRY XuiElementTreeGetFocus(HXUIOBJ hObj);
BYTE APIENTRY XuiElementGetFocusUser(HXUIOBJ hObj);

HRESULT APIENTRY XuiElementInitUserFocus(HXUIOBJ hObj, BYTE UserIndex, BOOL bPlayInitialFocus);
HRESULT APIENTRY XuiElementSetUserFocus(HXUIOBJ hObj, BYTE UserIndex);
HXUIOBJ APIENTRY XuiElementGetUserFocus(BYTE UserIndex);

BOOL APIENTRY XuiPlayingInitialFocus(void);

HRESULT APIENTRY XuiProcessInput(XINPUT_KEYSTROKE * pInputKeystroke);
DWORD   APIENTRY XuiMapKeyboardToGamepadInput(DWORD dwKeyCode);

static __declspec(noinline) LPCWSTR XuiElementGetVisualPrefix(HXUIOBJ hObj)
{
    HRESULT hr;
    XUIMessage msg;
    XUIMessageGetSetVisualPrefix msgExt;
    XuiMessageGetVisualPrefix(&msg, &msgExt);

    hr = XuiBubbleMessage(XuiGetOuter(hObj), &msg);
    if (FAILED(hr))
        return NULL;

    return msgExt.szPrefix;
}

static __declspec(noinline) HRESULT XuiElementSetVisualPrefix(HXUIOBJ hObj, LPCWSTR szPrefix)
{
    XUIMessage msg;
    XUIMessageGetSetVisualPrefix msgExt;
    XuiMessageSetVisualPrefix(&msg, &msgExt);
    msgExt.szPrefix = szPrefix;

    return XuiSendMessage(XuiGetOuter(hObj), &msg);
}

static __declspec(noinline) LPCWSTR XuiElementGetBasePath(HXUIOBJ hObj)
{
    HRESULT hr;
    XUIMessage msg;
    XUIMessageGetSetBasePath msgExt;
    XuiMessageGetBasePath(&msg, &msgExt);

    hr = XuiBubbleMessage(XuiGetOuter(hObj), &msg);
    if (FAILED(hr))
        return NULL;

    return msgExt.szPath;
}

static __declspec(noinline) HRESULT XuiElementSetBasePath(HXUIOBJ hObj, LPCWSTR szBasePath)
{
    XUIMessage msg;
    XUIMessageGetSetBasePath msgExt;
    XuiMessageSetBasePath(&msg, &msgExt);
    msgExt.szPath = szBasePath;

    return XuiSendMessage(XuiGetOuter(hObj), &msg);
}

static __declspec(noinline) BOOL XuiElementHitTest(HXUIOBJ hObj, POINT pt)
{
    HRESULT hr;
    XUIMessage msg;
    XUIMessageHittest msgData;
    XuiMessageHittest(&msg,&msgData,pt);
    hr = XuiSendMessage(XuiGetOuter(hObj), &msg);
    if (FAILED(hr))
        return FALSE;
    return msgData.bHit;
}

static __declspec(noinline) HRESULT XuiElementGetBounds(HXUIOBJ hObj, float *pfWidth, float *pfHeight)
{
    HRESULT hr;
    XUIMessage msg;
    XUIMessageGetBounds msgGetBounds;
    XuiMessageGetBounds(&msg,&msgGetBounds);
    hr = XuiSendMessage(hObj, &msg);
    if (FAILED(hr))
        return hr;
    if (!msg.bHandled)
        return XUI_ERR_MESSAGE_NOT_HANDLED;
    *pfWidth = msgGetBounds.fWidth;
    *pfHeight = msgGetBounds.fHeight;
    return S_OK;
}

static __declspec(noinline) BOOL XuiElementIsShown(HXUIOBJ hObj)
{
    XUIMessage msg;
    XUIMessageShowstate msgData;
    XuiMessageGetShowstate(&msg, &msgData);
    XuiSendMessage(hObj, &msg);
    return msgData.bShow;
}

static __declspec(noinline) HRESULT XuiElementSetShow(HXUIOBJ hObj, BOOL bShow)
{
    XUIMessage msg;
    XUIMessageShowstate msgData;
    XuiMessageSetShowstate(&msg, &msgData, bShow);
    return XuiSendMessage(hObj, &msg);
}

static __declspec(noinline) HRESULT XuiElementSkinChanged(HXUIOBJ hObj)
{
    XUIMessage msg;
    XuiMessage(&msg, XM_SKIN_CHANGED);
    return XuiBroadcastMessage(hObj, &msg);
}

static __declspec(noinline) HRESULT XuiElementLocaleChanged(HXUIOBJ hObj)
{
    XUIMessage msg;
    XuiMessage(&msg, XM_LOCALE_CHANGED);
    return XuiBroadcastMessage(hObj, &msg);
}

// XuiFigure APIs
enum XUI_FILL_TYPE
{
    XUI_FILL_NONE = 0,
    XUI_FILL_SOLID = 1,
    XUI_FILL_LINEAR_GRADIENT = 2,
    XUI_FILL_RADIAL_GRADIENT = 3,
    XUI_FILL_TEXTURE = 4
};

HRESULT APIENTRY XuiFigureClose(HXUIOBJ hObj, BOOL bClose);
HRESULT APIENTRY XuiFigureDeletePoint(HXUIOBJ hObj, int nPoint);
HRESULT APIENTRY XuiFigureSetFill(HXUIOBJ hObj, XUI_FILL_TYPE nFillType, DWORD dwFillColor, XUIGradientStop *pStops, int nNumStops, float fGradientAngle, const D3DXVECTOR2 *pvScale, const D3DXVECTOR2 *pvTrans);
HRESULT APIENTRY XuiFigureSetStroke(HXUIOBJ hObj, float fStrokeWidth, DWORD dwStrokeColor);
HRESULT APIENTRY XuiFigureSetTexture(HXUIOBJ hObj, LPCWSTR wszFileName);
HRESULT APIENTRY XuiFigureGetFillXForm(HXUIOBJ hObj, D3DXMATRIX *pmatFillXForm);
HRESULT APIENTRY XuiFigureSetShape(HXUIOBJ hObj, const XUIFigurePoint *pPoints, int nNumPoints);
HRESULT APIENTRY XuiFigureGetScalingFactor(HXUIOBJ hObj, D3DXVECTOR2 *pvScalingFactor);
BOOL APIENTRY XuiFigureIsClosed(HXUIOBJ hObj);
HRESULT APIENTRY XuiFigureGetShape(HXUIOBJ hObj, HXUISHAPE *phShape);

static __declspec(noinline) BOOL XuiFigureGetPointCount(HXUIOBJ hObj, int *pnCount)
{
    HRESULT hr;

    XUIMessage msg;
    XUIMessageFigureGetCount msgData;
    XuiMessageFigureGetCount(&msg, &msgData);
    hr = XuiSendMessage(hObj, &msg);
    if (FAILED(hr))
        return FALSE;
    *pnCount = msgData.nCount;
    return TRUE;
}

static __declspec(noinline) BOOL XuiFigureAddPoint(HXUIOBJ hObj, const XUIFigurePoint *pPoint)
{
    XUIMessage msg;
    XUIMessageFigurePoint msgData;
    XuiMessageFigureAddPoint(&msg, &msgData, 0, pPoint);
    XuiSendMessage(hObj, &msg);
    return TRUE;
}

static __declspec(noinline) BOOL XuiFigureSetPoints(HXUIOBJ hObj, int nPointIndex, const XUIFigurePoint *pPoint, int nCount)
{
    XUIMessage msg;
    XUIMessageFigurePoint msgData;
    XuiMessageFigureSetPoints(&msg, &msgData, nPointIndex, pPoint, nCount);
    XuiSendMessage(hObj, &msg);
    return TRUE;
}

static __declspec(noinline) HRESULT XuiFigureGetPoints(HXUIOBJ hObj, int nPointIndex, XUIFigurePoint *pPoint, int nCount)
{
    XUIMessage msg;
    XUIMessageFigurePoint msgData;
    XuiMessageFigureGetPoints(&msg, &msgData, nPointIndex, pPoint, nCount);
    return XuiSendMessage(hObj, &msg);
}

static __declspec(noinline) BOOL XuiFigureGetNextPoint(HXUIOBJ hObj, int nPointIndex, int *pnNextPoint)
{
    HRESULT hr;
    XUIMessage msg;
    XUIMessageFigurePointIndex msgData;
    XuiMessageFigureGetNextPoint(&msg, &msgData, nPointIndex);
    hr = XuiSendMessage(hObj, &msg);
    if (FAILED(hr))
        return FALSE;
    *pnNextPoint = msgData.nOtherIndex;
    return TRUE;
}

static __declspec(noinline) BOOL XuiFigureGetPrevPoint(HXUIOBJ hObj, int nPointIndex, int *pnPrevPoint)
{
    HRESULT hr;
    XUIMessage msg;
    XUIMessageFigurePointIndex msgData;
    XuiMessageFigureGetPrevPoint(&msg, &msgData, nPointIndex);
    hr = XuiSendMessage(hObj, &msg);
    if (FAILED(hr))
        return FALSE;
    *pnPrevPoint = msgData.nOtherIndex;
    return TRUE;
}

static __declspec(noinline) BOOL XuiFigureVertexFromPoint(HXUIOBJ hObj, const D3DXVECTOR2 *pt, int *pnVertex)
{
    HRESULT hr;
    XUIMessage msg;
    XUIMessageFigureVertexPoint msgData;
    XuiMessageFigureVertexFromPoint(&msg, &msgData, pt);
    hr = XuiSendMessage(hObj, &msg);
    if (FAILED(hr))
        return FALSE;
    *pnVertex = msgData.nVertex;
    return TRUE;
}

// XuiTextElement APIs
HRESULT APIENTRY XuiTextElementSetText(HXUIOBJ hObj, LPCWSTR szText);
LPCWSTR APIENTRY XuiTextElementGetText(HXUIOBJ hObj);
HRESULT APIENTRY XuiTextElementMeasureText(HXUIOBJ hObj, LPCWSTR szText, XUIRect *prct);

// XuiTextPresenterElement APIs
HXUIFONT APIENTRY XuiTextPresenterGetFont(HXUIOBJ hObj);
DWORD APIENTRY XuiTextPresenterGetStyle(HXUIOBJ hObj);
HRESULT APIENTRY XuiTextPresenterSetStyle(HXUIOBJ hObj, DWORD dwStyle);
int APIENTRY XuiTextPresenterGetLineSpacing(HXUIOBJ hObj);
HRESULT APIENTRY XuiTextPresenterMeasureText(HXUIOBJ hObj, LPCWSTR szText, XUIRect *prct);

// XuiImageElement APIs
enum XUI_CONTENT_TYPE
{
    XUI_CONTENT_NONE,       // there is no content
    XUI_CONTENT_OBJ,        // content is a XUI object
    XUI_CONTENT_BRUSH,      // content is a XUI brush
};
struct XUIContentInfo
{
    DWORD cbSize;
    XUI_CONTENT_TYPE nType;
    union
    {
        HXUIOBJ hObj;
        HXUIBRUSH hBrush;
    };
};

HRESULT APIENTRY XuiImageElementSetImagePath(HXUIOBJ hObj, LPCWSTR szImagePath);
HRESULT APIENTRY XuiImageElementGetImagePath(HXUIOBJ hObj, LPCWSTR *pszImagePath);
HRESULT APIENTRY XuiImageElementGetLastError(HXUIOBJ hObj);
HRESULT APIENTRY XuiImageElementGetContentInfo(HXUIOBJ hObj, XUIContentInfo *pInfo);

// XuiVisual APIs
HRESULT APIENTRY XuiLoadVisualFromBinary(LPCWSTR szResourcePath, LPCWSTR szPrefix);
HRESULT APIENTRY XuiVisualRegister(LPCWSTR szVisualId, XUIObjectData *pObjData, XUIObjectData *pObjDataOwner);
HRESULT APIENTRY XuiVisualCreateInstance(LPCWSTR szVisualId, LPCWSTR szPrefix, HXUIOBJ *phObj);
HRESULT APIENTRY XuiVisualFind(LPCWSTR szVisualId, LPCWSTR szPrefix, XUIObjectData **ppObjData, XUIObjectData **ppObjOwner);
HRESULT APIENTRY XuiVisualUnregister(LPCWSTR szVisualId);
void    APIENTRY XuiFreeVisuals(LPCWSTR szPrefix);
HRESULT APIENTRY XuiVisualSetBasePath(LPCWSTR szBasePath, LPCWSTR szPrefix);
LPCWSTR APIENTRY XuiVisualGetBasePath(LPCWSTR szPrefix);

// Control visual states: normal/focus
#define XUI_NORMAL                  L"Normal"                   // required for all control visuals
#define XUI_ENDNORMAL               L"EndNormal"
#define XUI_FOCUS                   L"Focus"                    // when not specified by the visual, try XUI_NORMAL
#define XUI_ENDFOCUS                L"EndFocus"
#define XUI_INITFOCUS               L"InitFocus"                // when not specified by the visual, try XUI_FOCUS
#define XUI_ENDINITFOCUS            L"EndInitFocus"
#define XUI_KILLFOCUS               L"KillFocus"                // changing from focus to normal:
                                                                // define KillFocus Normal EndNormal sequentially;
                                                                // normal from any state but focus will play XUI_NORMAL to XUI_ENDNORMAL
                                                                // normal from focus will play XUI_KILLFOCUS to XUI_ENDNORMAL
                                                                // when not specified by the visual, try XUI_NORMAL

#define XUI_NORMALDISABLE           L"NormalDisable"            // normal, disabled: when not specified by the visual, try XUI_NORMAL
#define XUI_ENDNORMALDISABLE        L"EndNormalDisable"
#define XUI_FOCUSDISABLE            L"FocusDisable"             // focused, disabled: when not specified by the visual, try XUI_FOCUS
#define XUI_ENDFOCUSDISABLE         L"EndFocusDisable"
#define XUI_INITFOCUSDISABLE        L"InitFocusDisable"         // initial focus, disabled: when not specified by the visual, try XUI_FOCUSDISABLE
#define XUI_ENDINITFOCUSDISABLE     L"EndInitFocusDisable"

// Control visual states: press
#define XUI_PRESS                   L"Press"                    // focused, pressed: when not specified by the visual, try XUI_FOCUS
#define XUI_ENDPRESS                L"EndPress"
#define XUI_NORMALPRESS             L"NormalPress"              // normal, pressed: when not specified by the visual, try XUI_PRESS
#define XUI_ENDNORMALPRESS          L"EndNormalPress"
#define XUI_PRESSDISABLE            L"PressDisable"             // pressed, disabled: when not specified by the visual, try XUI_NORMALPRESS
#define XUI_ENDPRESSDISABLE         L"EndPressDisable"

// Control visual states: checked
#define XUI_NORMALCHECK             L"NormalCheck"              // normal, checked: when not specified by the visual, try XUI_NORMAL
#define XUI_ENDNORMALCHECK          L"EndNormalCheck"
#define XUI_FOCUSCHECK              L"FocusCheck"               // focused, checked: when not specified by the visual, try XUI_NORMALCHECK
#define XUI_ENDFOCUSCHECK           L"EndFocusCheck"
#define XUI_INITFOCUSCHECK          L"InitFocusCheck"           // initial focus, checked: when not specified by the visual, try XUI_FOCUSCHECK
#define XUI_ENDINITFOCUSCHECK       L"EndInitFocusCheck"

#define XUI_NORMALCHECKDISABLE      L"NormalCheckDisable"       // normal, checked, disabled: when not specified by the visual, try XUI_NORMALCHECK
#define XUI_ENDNORMALCHECKDISABLE   L"EndNormalCheckDisable"
#define XUI_FOCUSCHECKDISABLE       L"FocusCheckDisable"        // focused, checked, disabled: when not specified by the visual, try XUI_FOCUSCHECK
#define XUI_ENDFOCUSCHECKDISABLE    L"EndFocusCheckDisable"
#define XUI_INITFOCUSCHECKDISABLE   L"InitFocusCheckDisable"    // initial focus, checked, disabled: when not specified by the visual, try XUI_FOCUSCHECKDISABLE
#define XUI_ENDINITFOCUSCHECKDISABLE L"EndInitFocusCheckDisable"

// Control visual states: selected
// NOTE: selected applies only to normal; focused and selected is the same as focused
#define XUI_NORMALSEL               L"NormalSel"                // normal, selected: when not specified by the visual, try XUI_NORMAL
#define XUI_ENDNORMALSEL            L"EndNormalSel"
#define XUI_NORMALSELCHECK          L"NormalSelCheck"           // normal, checked, selected: when not specified by the visual, try XUI_NORMALSEL
#define XUI_ENDNORMALSELCHECK       L"EndNormalSelCheck"

#define XUI_NORMALSELDISABLE        L"NormalSelDisable"         // normal, selected, disabled: when not specified by the visual, try XUI_NORMALSEL
#define XUI_ENDNORMALSELDISABLE     L"EndNormalSelDisable"
#define XUI_NORMALSELCHECKDISABLE   L"NormalSelCheckDisable"    // normal, checked, selected, disabled: when not specified by the visual, try XUI_NORMALSELDISABLE
#define XUI_ENDNORMALSELCHECKDISABLE L"EndNormalSelCheckDisable"

// Control visual states: scroll ends
// NOTE: scrolling applies only when more is available to scroll
//       scrolling is not supported on a disabled scroll end
//       focus is not supported on a scroll end
#define XUI_SCROLLMORE              L"ScrollMore"               // more available to scroll: when not specified by the visual, try XUI_NORMAL
#define XUI_ENDSCROLLMORE           L"EndScrollMore"
#define XUI_SCROLLING               L"Scrolling"                // scrolling: when not specified by the visual, try XUI_MORESCROLL
#define XUI_ENDSCROLLING            L"EndScrolling"

#define XUI_SCROLLMOREDISABLE       L"ScrollMoreDisable"        // when not specified by the visual, try XUI_SCROLLMORE
#define XUI_ENDSCROLLMOREDISABLE    L"EndScrollMoreDisable"

// Control visual elements: edit
#define XUI_EDITCARET               L"Caret"
#define XUI_EDITTEXT                L"Text"
#define XUI_EDITSCROLLLEFT          L"ScrollLeft"
#define XUI_EDITSCROLLRIGHT         L"ScrollRight"
#define XUI_EDITSCROLLUP            L"ScrollUp"
#define XUI_EDITSCROLLDOWN          L"ScrollDown"

// Control visual elements: progress bars and sliders

#define XUI_PROGRESSBODY            L"ProgressBody"             // ID of visual element to be used as progress bar body
#define XUI_PROGRESSVALUE           1                           // DataAssociation for text presenters of progress value
#define XUI_SLIDERBODY              L"SliderBody"               // ID of visual element to be used as slider body
#define XUI_SLIDERVALUE             1                           // DataAssociation for text presenters of slider value
#define XUI_SLIDERTHUMB             L"SliderThumb"              // ID of visual element to be used as slider thumb
#define XUI_SLIDERSCROLLMIN         L"ScrollMin"                // ID of visual element that indicates whether slider is at min position or not
#define XUI_SLIDERSCROLLMAX         L"ScrollMax"                // ID of visual element that indicates whether slider is at max position or not

// Control visual elements: message box

#define XUI_MESSAGEBOX_BUTTON       L"Button"
#define XUI_MESSAGEBOX_MESSAGETEXT  L"MessageText"

// Scene transition namedframes
#define XUI_SCENE_TRANS_FROM        L"TransFrom"
#define XUI_SCENE_END_TRANS_FROM    L"EndTransFrom"
#define XUI_SCENE_TRANS_TO          L"TransTo"
#define XUI_SCENE_END_TRANS_TO      L"EndTransTo"
#define XUI_SCENE_TRANS_BACKTO      L"TransBackTo"
#define XUI_SCENE_END_TRANS_BACKTO  L"EndTransBackTo"
#define XUI_SCENE_TRANS_BACKFROM      L"TransBackFrom"
#define XUI_SCENE_END_TRANS_BACKFROM  L"EndTransBackFrom"

// A container is not an actual element class, but rather
// a logical container such as a radio group or a list.

static __declspec(noinline) int XuiContainerGetCurSel(HXUIOBJ hObj, HXUIOBJ* phItem)
{
    XUIMessage msg;
    XUIMessageGetCurSel msgData;
    XuiMessageGetCurSel(&msg, &msgData);
    XuiSendMessage(hObj,&msg);
    if (phItem)
        *phItem = msgData.hItem;
    return msgData.iItem;
}
static __declspec(noinline) HRESULT XuiContainerSetCurSel(HXUIOBJ hObj, int iItem)
{
    XUIMessage msg;
    XUIMessageSetCurSel msgData;
    XuiMessageSetCurSel(&msg, &msgData, iItem);
    return XuiSendMessage(hObj,&msg);
}
static __declspec(noinline) HRESULT XuiContainerBubbleSelChanged(HXUIOBJ hObj, int iItem, int iOldItem)
{
    XUIMessage msg;
    XUINotify nm;
    XUINotifySelChanged nmData;
    XuiNotifySelChanged(&msg, &nm, &nmData, hObj, iItem, iOldItem);
    return XuiBubbleMessage(hObj, &msg);
}
static __declspec(noinline) HXUIOBJ XuiContainerGetItemControl(HXUIOBJ hObj, int iItem)
{
    XUIMessage msg;
    XUIMessageGetItemControl msgData;
    XuiMessageGetItemControl(&msg, &msgData, iItem);
    XuiSendMessage(hObj,&msg);
    return msgData.hItem;
}
static __declspec(noinline) int XuiContainerGetItemCount(HXUIOBJ hObj)
{
    HRESULT hr;
    XUIMessage msg;
    XUIMessageGetItemCount msgData;
    XuiMessageGetItemCount(&msg, &msgData, XUI_ITEMCOUNT_ALL);
    hr = XuiSendMessage(hObj, &msg);
    if (FAILED(hr))
        return 0;
    if (msgData.cItems < 0)
        msgData.cItems = 0;
    return msgData.cItems;
}
static __declspec(noinline) int XuiContainerGetVisibleItemCount(HXUIOBJ hObj)
{
    HRESULT hr;
    XUIMessage msg;
    XUIMessageGetItemCount msgData;
    XuiMessageGetItemCount(&msg, &msgData, XUI_ITEMCOUNT_VISIBLE);
    hr = XuiSendMessage(hObj, &msg);
    if (FAILED(hr))
        return 0;
    if (msgData.cItems < 0)
        msgData.cItems = 0;
    return msgData.cItems;
}
static __declspec(noinline) int XuiContainerGetCheckedItemCount(HXUIOBJ hObj)
{
    HRESULT hr;
    XUIMessage msg;
    XUIMessageGetItemCount msgData;
    XuiMessageGetItemCount(&msg, &msgData, XUI_ITEMCOUNT_CHECKED);
    hr = XuiSendMessage(hObj, &msg);
    if (FAILED(hr))
        return 0;
    if (msgData.cItems < 0)
        msgData.cItems = 0;
    return msgData.cItems;
}
static __declspec(noinline) int XuiContainerGetMaxVisibleItemCount(HXUIOBJ hObj)
{
    HRESULT hr;
    XUIMessage msg;
    XUIMessageGetItemCount msgData;
    XuiMessageGetItemCount(&msg, &msgData, XUI_ITEMCOUNT_MAX_VISIBLE);
    hr = XuiSendMessage(hObj, &msg);
    if (FAILED(hr))
        return 0;
    if (msgData.cItems < 0)
        msgData.cItems = 0;
    return msgData.cItems;
}
static __declspec(noinline) int XuiContainerGetMaxLinesItemCount(HXUIOBJ hObj)
{
    HRESULT hr;
    XUIMessage msg;
    XUIMessageGetItemCount msgData;
    XuiMessageGetItemCount(&msg, &msgData, XUI_ITEMCOUNT_MAX_LINES);
    hr = XuiSendMessage(hObj, &msg);
    if (FAILED(hr))
        return 0;
    if (msgData.cItems < 0)
        msgData.cItems = 0;
    return msgData.cItems;
}
static __declspec(noinline) int XuiContainerGetMaxPerLineItemCount(HXUIOBJ hObj)
{
    HRESULT hr;
    XUIMessage msg;
    XUIMessageGetItemCount msgData;
    XuiMessageGetItemCount(&msg, &msgData, XUI_ITEMCOUNT_MAX_PER_LINE);
    hr = XuiSendMessage(hObj, &msg);
    if (FAILED(hr))
        return 0;
    if (msgData.cItems < 0)
        msgData.cItems = 0;
    return msgData.cItems;
}
static __declspec(noinline) int XuiContainerGetTopItem(HXUIOBJ hObj)
{
    HRESULT hr;
    XUIMessage msg;
    XUIMessageGetTopItem msgData;
    XuiMessageGetTopItem(&msg, &msgData);
    hr = XuiSendMessage(hObj, &msg);
    if (FAILED(hr))
        return -1;
    return msgData.iTopItem;
}
static __declspec(noinline) HRESULT XuiContainerSetTopItem(HXUIOBJ hObj, int iTopItem)
{
    XUIMessage msg;
   XUIMessageSetTopItem msgData;
    XuiMessageSetTopItem(&msg, &msgData, iTopItem);
    return XuiSendMessage(hObj,&msg);
}
static __declspec(noinline) BOOL XuiContainerGetItemCheck(HXUIOBJ hObj, int iItem)
{
    XUIMessage msg;
    XUIMessageGetItemCheck msgData;
    XuiMessageGetItemCheck(&msg, &msgData, iItem);
    XuiSendMessage(hObj, &msg);
    return msgData.bChecked;
}
static __declspec(noinline) HRESULT XuiContainerSetItemCheck(HXUIOBJ hObj, int iItem, BOOL bChecked)
{
    XUIMessage msg;
    XUIMessageSetItemCheck msgData;
    XuiMessageSetItemCheck(&msg, &msgData, iItem, bChecked);
    return XuiSendMessage(hObj, &msg);
}

// XuiControl property names
#define XUI_CONTROL_CLASS_OVERRIDE  L"ClassOverride"

// XuiControl APIs
int     APIENTRY XuiControlGetItemAssociation(HXUIOBJ hObj);
HRESULT APIENTRY XuiControlSetItemAssociation(HXUIOBJ hObj, int iItem);
BOOL    APIENTRY XuiControlWantsUnfocusedInput(HXUIOBJ hObj);
HXUIOBJ APIENTRY XuiControlGetNavigation(HXUIOBJ hObj, XUI_CONTROL_NAVIGATE eDirection, BOOL bSkipNotEnabled, BOOL bSkipNotShown);

static __declspec(noinline) LPCWSTR XuiControlGetText(HXUIOBJ hObj)
{
    XUIMessage msg;
    XUIMessageGetSourceText msgExt;
    XuiMessageGetSourceText(&msg, &msgExt, 0, 0, FALSE);
    XuiSendMessage(XuiGetOuter(hObj), &msg);
    return msgExt.szText;
}

static __declspec(noinline) HRESULT XuiControlSetText(HXUIOBJ hObj, LPCWSTR szText)
{
    XUIMessage msg;
    XUIMessageSetSourceText msgExt;
    XuiMessageSetSourceText(&msg, &msgExt, 0, 0, FALSE, szText);
    return XuiSendMessage(XuiGetOuter(hObj), &msg);
}

static __declspec(noinline) HRESULT XuiControlSetImagePath(HXUIOBJ hObj, LPCWSTR szPath)
{
    XUIMessage msg;
    XUIMessageSetSourceImage msgExt;
    XuiMessageSetSourceImage(&msg, &msgExt, 0, 0, FALSE, szPath, NULL, NULL);
    return XuiSendMessage(XuiGetOuter(hObj), &msg);
}

static __declspec(noinline) HRESULT XuiControlGetImagePath(HXUIOBJ hObj, LPCWSTR *ppszPath)
{
    HRESULT hr;
    XUIMessage msg;
    XUIMessageGetSourceImage msgExt;
    XuiMessageGetSourceImage(&msg, &msgExt, 0, 0, FALSE);
    hr = XuiSendMessage(XuiGetOuter(hObj), &msg);
    *ppszPath = msgExt.szPath;
    return hr;
}

static __declspec(noinline) HRESULT XuiControlSetImageBrush(HXUIOBJ hObj, HXUIBRUSH hBrush)
{
    XUIMessage msg;
    XUIMessageSetSourceImage msgExt;
    XuiMessageSetSourceImage(&msg, &msgExt, 0, 0, FALSE, NULL, hBrush, NULL);
    return XuiSendMessage(XuiGetOuter(hObj), &msg);
}

static __declspec(noinline) HRESULT XuiControlGetImageBrush(HXUIOBJ hObj, HXUIBRUSH *phBrush)
{
    HRESULT hr;
    XUIMessage msg;
    XUIMessageGetSourceImage msgExt;
    XuiMessageGetSourceImage(&msg, &msgExt, 0, 0, FALSE);
    hr = XuiSendMessage(XuiGetOuter(hObj), &msg);
    *phBrush = msgExt.hBrush;
    return hr;
}

static __declspec(noinline) HRESULT XuiControlSetImageObject(HXUIOBJ hObj, HXUIOBJ hImage)
{
    XUIMessage msg;
    XUIMessageSetSourceImage msgExt;
    XuiMessageSetSourceImage(&msg, &msgExt, 0, 0, FALSE, NULL, NULL, hImage);
    return XuiSendMessage(XuiGetOuter(hObj), &msg);
}

static __declspec(noinline) HRESULT XuiControlGetImageObject(HXUIOBJ hObj, HXUIOBJ *phImage)
{
    HRESULT hr;
    XUIMessage msg;
    XUIMessageGetSourceImage msgExt;
    XuiMessageGetSourceImage(&msg, &msgExt, 0, 0, FALSE);
    hr = XuiSendMessage(XuiGetOuter(hObj), &msg);
    *phImage = msgExt.hObj;
    return hr;
}

static __declspec(noinline) BOOL XuiControlIsEnabled(HXUIOBJ hObj)
{
    XUIMessage msg;
    XUIMessageEnableState msgData;
    XuiMessageGetEnablestate(&msg, &msgData);
    XuiSendMessage(hObj, &msg);
    return msgData.bEnabled;
}
static __declspec(noinline) HRESULT XuiControlSetEnable(HXUIOBJ hObj, BOOL bEnable)
{
    XUIMessage msg;
    XUIMessageEnableState msgData;
    XuiMessageSetEnablestate(&msg, &msgData, bEnable);
    return XuiSendMessage(hObj, &msg);
}
static __declspec(noinline) BOOL XuiControlIsPressed(HXUIOBJ hObj)
{
    XUIMessage msg;
    XUIMessagePressState msgData;
    XuiMessageGetPressState(&msg, &msgData);
    XuiSendMessage(hObj, &msg);
    return msgData.bPressed;
}
static __declspec(noinline) HRESULT XuiControlSetPress(HXUIOBJ hObj, BOOL bPressed)
{
    XUIMessage msg;
    XUIMessagePressState msgData;
    XuiMessageSetPressState(&msg, &msgData, bPressed);
    return XuiSendMessage(hObj, &msg);
}
static __declspec(noinline) BOOL XuiControlPress(HXUIOBJ hObj, BYTE UserIndex)
{
    HRESULT hr;
    XUIMessage msg;
    XUIMessagePress msgData;
    XuiMessagePress(&msg, &msgData, UserIndex);
    hr = XuiSendMessage(hObj, &msg);
    if (FAILED(hr))
        return FALSE;
    return msgData.bPressOccurred;
}

// XuiControl APIs: for visuals
HRESULT APIENTRY XuiControlGetVisual(HXUIOBJ hObj, HXUIOBJ* phVisual);
HRESULT APIENTRY XuiControlDestroyVisual(HXUIOBJ hObj);
HRESULT APIENTRY XuiControlAttachVisual(HXUIOBJ hObj);
HRESULT APIENTRY XuiControlPlayStandardVisual(HXUIOBJ hObj);
#define XUI_KILLING_FOCUS    FALSE
#define XUI_SETTING_FOCUS   TRUE
HRESULT APIENTRY XuiControlPlayStandardVisualOnFocusChanging(HXUIOBJ hObj, BOOL bFocus);
HRESULT APIENTRY XuiControlPlayVisualRange(HXUIOBJ hObj, LPCWSTR szBegin, LPCWSTR szInitial, LPCWSTR szEnd);
HRESULT APIENTRY XuiControlPlayOptionalVisual(HXUIOBJ hObj, LPCWSTR szBegin, LPCWSTR szEnd, LPCWSTR szBeginOverrideFallback, LPCWSTR szEndOverrideFallback);

static __declspec(noinline) HRESULT XuiControlPlayVisual(HXUIOBJ hObj)
{
    XUIMessage msg;
    XUIMessagePlayVisual msgData;
    XuiMessagePlayVisual(&msg, &msgData);
    return XuiSendMessage(hObj, &msg);
}
static __declspec(noinline) HRESULT XuiControlPlayVisualOnFocusChanging(HXUIOBJ hObj, BOOL bGettingFocus)
{
    XUIMessage msg;
    XUIMessagePlayVisual msgData;
    XuiMessagePlayVisualOnFocusChanging(&msg, &msgData, bGettingFocus);
    return XuiSendMessage(hObj, &msg);
}

// XuiControl APIs: XuiCheckbox
static __declspec(noinline) BOOL XuiCheckboxIsChecked(HXUIOBJ hObj)
{
    XUIMessage msg;
    XUIMessageGetCheckState msgData;
    XuiMessageGetCheckstate(&msg, &msgData);
    XuiSendMessage(hObj, &msg);
    return msgData.bChecked;
}
static __declspec(noinline) HRESULT XuiCheckboxSetCheck(HXUIOBJ hObj, BOOL bCheck)
{
    XUIMessage msg;
    XUIMessageSetCheckState msgData;
    XuiMessageSetCheckstate(&msg, &msgData, bCheck);
    return XuiSendMessage(hObj, &msg);
}

// XuiControl APIs: XuiEdit
HRESULT APIENTRY XuiEditSetReadOnly(HXUIOBJ hObj, BOOL bReadOnly);
BOOL APIENTRY XuiEditGetReadOnly(HXUIOBJ hObj);
HRESULT APIENTRY XuiEditSetTextLimit(HXUIOBJ hObj, UINT nLimit);
UINT APIENTRY XuiEditGetTextLimit(HXUIOBJ hObj);
HRESULT APIENTRY XuiEditSetTextFormatInfo(HXUIOBJ hObj, int nTextFormatInfo, XUITextFormatInfo* pTextFormatInfo);
HRESULT APIENTRY XuiEditGetTextFormatInfo(HXUIOBJ hObj, int* pnTextFormatInfo, XUITextFormatInfo** ppTextFormatInfo);

static __declspec(noinline) HRESULT XuiEditInsertText(HXUIOBJ hObj, UINT nIndex, LPCWSTR pszText)
{
    XUIMessage msg;
    XUIMessageInsertText msgData;
    XuiMessageInsertText(&msg, &msgData, nIndex, pszText);
    return XuiSendMessage(hObj, &msg);
}

static __declspec(noinline) HRESULT XuiEditDeleteText(HXUIOBJ hObj, UINT nIndex, UINT nCount)
{
    XUIMessage msg;
    XUIMessageDeleteText msgData;
    XuiMessageDeleteText(&msg, &msgData, nIndex, nCount);
    return XuiSendMessage(hObj, &msg);
}

static __declspec(noinline) HRESULT XuiEditSetCaretPosition(HXUIOBJ hObj, UINT nPosition)
{
    XUIMessage msg;
    XUIMessageSetCaretPosition msgData;
    XuiMessageSetCaretPosition(&msg, &msgData, nPosition);
    return XuiSendMessage(hObj, &msg);
}

static __declspec(noinline) UINT XuiEditGetCaretPosition(HXUIOBJ hObj)
{
    HRESULT hr;
    XUIMessage msg;
    XUIMessageGetCaretPosition msgData;
    XuiMessageGetCaretPosition(&msg, &msgData);

    hr = XuiSendMessage(hObj, &msg);
    if (SUCCEEDED(hr))
    {
        return msgData.nPosition;
    }

    return 0;
}

static __declspec(noinline) UINT XuiEditGetLineCount(HXUIOBJ hObj)
{
    HRESULT hr;
    XUIMessage msg;
    XUIMessageGetLineCount msgData;
    XuiMessageGetLineCount(&msg, &msgData, XUI_LINECOUNT_ALL, FALSE);

    hr = XuiSendMessage(hObj, &msg);
    if (SUCCEEDED(hr))
    {
        return msgData.nCount;
    }

    return 0;
}

static __declspec(noinline) UINT XuiEditGetVisibleLineCount(HXUIOBJ hObj, BOOL bSmoothScroll)
{
    HRESULT hr;
    XUIMessage msg;
    XUIMessageGetLineCount msgData;
    XuiMessageGetLineCount(&msg, &msgData, XUI_LINECOUNT_VISIBLE, bSmoothScroll);

    hr = XuiSendMessage(hObj, &msg);
    if (SUCCEEDED(hr))
    {
        return msgData.nCount;
    }

    return 0;
}

static __declspec(noinline) UINT XuiEditGetMaxVisibleLineCount(HXUIOBJ hObj)
{
    HRESULT hr;
    XUIMessage msg;
    XUIMessageGetLineCount msgData;
    XuiMessageGetLineCount(&msg, &msgData, XUI_LINECOUNT_MAX_VISIBLE, FALSE);

    hr = XuiSendMessage(hObj, &msg);
    if (SUCCEEDED(hr))
    {
        return msgData.nCount;
    }

    return 0;
}

static __declspec(noinline) UINT XuiEditGetTopLine(HXUIOBJ hObj, BOOL bSmoothScroll)
{
    HRESULT hr;
    XUIMessage msg;
    XUIMessageGetTopLine msgData;
    XuiMessageGetTopLine(&msg, &msgData, bSmoothScroll);

    hr = XuiSendMessage(hObj, &msg);
    if (SUCCEEDED(hr))
    {
        return msgData.nLineIndex;
    }

    return 0;
}

static __declspec(noinline) HRESULT XuiEditSetTopLine(HXUIOBJ hObj, UINT nLineIndex, BOOL bSmoothScroll)
{
    XUIMessage msg;
    XUIMessageSetTopLine msgData;
    XuiMessageSetTopLine(&msg, &msgData, nLineIndex, bSmoothScroll);
    return XuiSendMessage(hObj, &msg);
}

static __declspec(noinline) HRESULT XuiEditGetLineIndex(HXUIOBJ hObj, UINT nLine, UINT* pnIndex)
{
    HRESULT hr;
    XUIMessage msg;
    XUIMessageGetLineIndex msgData;
    XuiMessageGetLineIndex(&msg, &msgData, nLine);
    hr = XuiSendMessage(hObj, &msg);
    if (FAILED(hr))
        return hr;

    if (pnIndex == NULL)
        return E_INVALIDARG;

    *pnIndex = msgData.nIndex;
    return S_OK;
}

static __declspec(noinline) HRESULT XuiEditSetSmoothScroll(HXUIOBJ hObj,
                                                           XUI_SMOOTHSCROLL_ORIENTATION orientation,
                                                           BOOL bEnabled,
                                                           float fBaseSpeed,
                                                           float fAccel,
                                                           float fMaxSpeed)
{
    XUIMessage msg;
    XUIMessageSmoothScroll msgData;
    XuiMessageSetSmoothScroll(&msg, &msgData, orientation, bEnabled, fBaseSpeed, fAccel, fMaxSpeed);
    return XuiSendMessage(hObj, &msg);
}

static __declspec(noinline) HRESULT XuiEditGetSmoothScroll(HXUIOBJ hObj,
                                                           XUI_SMOOTHSCROLL_ORIENTATION orientation,
                                                           BOOL *pbEnabled,
                                                           float *pfBaseSpeed,
                                                           float *pfAccel,
                                                           float *pfMaxSpeed)
{
    HRESULT hr = S_OK;
    XUIMessage msg;
    XUIMessageSmoothScroll msgData;
    XuiMessageGetSmoothScroll(&msg, &msgData, orientation);
    hr = XuiSendMessage(hObj, &msg);
    if (SUCCEEDED(hr))
    {
        if (pbEnabled)
            *pbEnabled = msgData.bEnabled;
        if (pfBaseSpeed)
            *pfBaseSpeed = msgData.fBaseSpeed;
        if (pfAccel)
            *pfAccel = msgData.fAccel;
        if (pfMaxSpeed)
            *pfMaxSpeed = msgData.fMaxSpeed;
    }
    return hr;
}

// XuiControl APIs: XuiList:
// Scrolling past the last page of data is disallowed.
// Setting the top item changes the list's view to include that item (last page rule enforced).
// Setting the current item does *not* place that item in view if not already in view.
// Navigating when the current item is not in view places it in view (last page rule enforced).
// The count of list items may only change via caller insertion and deletion; the count must not
// change during the processing of other messages.
//
// Only the following messages should be overridden by derivations supplying list data:
//
// A derivation whose count of item data is static __declspec(noinline) must call XM_INSERT_ITEMS once on XuiList to
// inform it of its data; it must implement and handle XM_GET_SOURCEDATA and also
// XM_GET_ITEMCOUNT for XUI_ITEMCOUNT_ALL only.
//
// A derivation with dynamic data must also implement and handle XM_SET_SOURCEDATA.
// XM_INSERT_ITEMS and XM_DELETE_ITEMS must also be implemented but left unhandled.
//
// A derivation supporting checkable items must also implement and handle XM_GET_ITEMCHECK and
// XM_GET_ITEMCOUNT for XUI_ITEMCOUNT_CHECKED.  XM_SET_ITEMCHECK must be implemented but
// left unhandled.
//
// A derivation supporting disable-able items must also implement and handle XM_GET_ITEMENABLE,
// and implement XM_SET_ITEMENABLE but leave it unhandled.
//

#define XuiListGetCurSel XuiContainerGetCurSel
#define XuiListSetCurSel XuiContainerSetCurSel
#define XuiListBubbleSelChanged XuiContainerBubbleSelChanged
#define XuiListGetItemControl XuiContainerGetItemControl
#define XuiListGetItemCount XuiContainerGetItemCount
#define XuiListGetVisibleItemCount XuiContainerGetVisibleItemCount
#define XuiListGetCheckedItemCount XuiContainerGetCheckedItemCount
#define XuiListGetMaxVisibleItemCount XuiContainerGetMaxVisibleItemCount
#define XuiListGetMaxLinesItemCount XuiContainerGetMaxLinesItemCount
#define XuiListGetMaxPerLineItemCount XuiContainerGetMaxPerLineItemCount
#define XuiListGetTopItem XuiContainerGetTopItem
#define XuiListSetTopItem XuiContainerSetTopItem
#define XuiListGetItemCheck XuiContainerGetItemCheck
#define XuiListSetItemCheck XuiContainerSetItemCheck

static __declspec(noinline) HRESULT XuiListInsertItems(HXUIOBJ hObj, int iAtItem, int cItems)
{
    XUIMessage msg;
    XUIMessageInsertItems msgData;
    XuiMessageInsertItems(&msg, &msgData, iAtItem, cItems);
    return XuiSendMessage(hObj, &msg);
}
static __declspec(noinline) HRESULT XuiListDeleteItems(HXUIOBJ hObj, int iAtItem, int cItems)
{
    XUIMessage msg;
    XUIMessageDeleteItems msgData;
    XuiMessageDeleteItems(&msg, &msgData, iAtItem, cItems);
    return XuiSendMessage(hObj, &msg);
}
static __declspec(noinline) BOOL XuiListIsItemChecked(HXUIOBJ hObj, int iItem)
{
    XUIMessage msg;
    XUIMessageGetItemCheck msgData;
    XuiMessageGetItemCheck(&msg, &msgData, iItem);
    XuiSendMessage(hObj, &msg);
    return msgData.bChecked;
}
static __declspec(noinline) BOOL XuiListIsItemEnabled(HXUIOBJ hObj, int iItem)
{
    XUIMessage msg;
    XUIMessageGetItemEnable msgData;
    XuiMessageGetItemEnable(&msg, &msgData, iItem);
    XuiSendMessage(hObj, &msg);
    return msgData.bEnabled;
}
static __declspec(noinline) HRESULT XuiListSetItemEnable(HXUIOBJ hObj, int iItem, BOOL bEnabled)
{
    XUIMessage msg;
    XUIMessageSetItemEnable msgData;
    XuiMessageSetItemEnable(&msg, &msgData, iItem, bEnabled);
    return XuiSendMessage(hObj, &msg);
}
static __declspec(noinline) HRESULT XuiListSetText(HXUIOBJ hObj, int iItem, LPCWSTR szText)
{
    XUIMessage msg;
    XUIMessageSetSourceText msgExt;
    XuiMessageSetSourceText(&msg, &msgExt, iItem, 0, TRUE, szText);
    return XuiSendMessage(XuiGetOuter(hObj), &msg);
}
static __declspec(noinline) LPCWSTR XuiListGetText(HXUIOBJ hObj, int iItem)
{
    XUIMessage msg;
    XUIMessageGetSourceText msgExt;
    XuiMessageGetSourceText(&msg, &msgExt, iItem, 0, TRUE);
    XuiSendMessage(XuiGetOuter(hObj), &msg);
    return msgExt.szText;
}
static __declspec(noinline) HRESULT XuiListSetImagePath(HXUIOBJ hObj, int iItem, LPCWSTR szPath)
{
    XUIMessage msg;
    XUIMessageSetSourceImage msgExt;
    XuiMessageSetSourceImage(&msg, &msgExt, iItem, 0, TRUE, szPath, NULL, NULL);
    return XuiSendMessage(XuiGetOuter(hObj), &msg);
}
static __declspec(noinline) HRESULT XuiListSetImageBrush(HXUIOBJ hObj, int iItem, HXUIBRUSH hBrush)
{
    XUIMessage msg;
    XUIMessageSetSourceImage msgExt;
    XuiMessageSetSourceImage(&msg, &msgExt, iItem, 0, TRUE, NULL, hBrush, NULL);
    return XuiSendMessage(XuiGetOuter(hObj), &msg);
}
static __declspec(noinline) HRESULT XuiListSetImageObject(HXUIOBJ hObj, int iItem, HXUIOBJ hImage)
{
    XUIMessage msg;
    XUIMessageSetSourceImage msgExt;
    XuiMessageSetSourceImage(&msg, &msgExt, iItem, 0, TRUE, NULL, NULL, hImage);
    return XuiSendMessage(XuiGetOuter(hObj), &msg);
}
static __declspec(noinline) HRESULT XuiListGetImagePath(HXUIOBJ hObj, int iItem, LPCWSTR *ppszPath)
{
    HRESULT hr;
    XUIMessage msg;
    XUIMessageGetSourceImage msgExt;
    XuiMessageGetSourceImage(&msg, &msgExt, iItem, 0, TRUE);
    hr = XuiSendMessage(XuiGetOuter(hObj), &msg);
    *ppszPath = msgExt.szPath;
    return hr;
}
static __declspec(noinline) HRESULT XuiListGetImageBrush(HXUIOBJ hObj, int iItem, HXUIBRUSH *phBrush)
{
    HRESULT hr;
    XUIMessage msg;
    XUIMessageGetSourceImage msgExt;
    XuiMessageGetSourceImage(&msg, &msgExt, iItem, 0, TRUE);
    hr = XuiSendMessage(XuiGetOuter(hObj), &msg);
    *phBrush = msgExt.hBrush;
    return hr;
}
static __declspec(noinline) HRESULT XuiListGetImageObject(HXUIOBJ hObj, int iItem, HXUIOBJ *phObj)
{
    HRESULT hr;
    XUIMessage msg;
    XUIMessageGetSourceImage msgExt;
    XuiMessageGetSourceImage(&msg, &msgExt, iItem, 0, TRUE);
    hr = XuiSendMessage(XuiGetOuter(hObj), &msg);
    *phObj = msgExt.hObj;
    return hr;
}
static __declspec(noinline) BOOL XuiListIsItemVisible(HXUIOBJ hObj, int iItem)
{
    int iTopItem = XuiListGetTopItem(hObj);
    int cVisible = XuiListGetVisibleItemCount(hObj);
    if (iItem >= 0 && iTopItem >= 0 && cVisible > 0 && iItem >= iTopItem && iItem < iTopItem + cVisible)
        return TRUE;
    return FALSE;
}
static __declspec(noinline) HRESULT XuiListSetCurSelVisible(HXUIOBJ hObj, int iItem)
{
    HRESULT hr = XuiListSetCurSel(hObj, iItem);
    if (FAILED(hr)) { return hr; }
    if (!XuiListIsItemVisible(hObj, iItem))
        return XuiListSetTopItem(hObj, iItem);
    return hr;
}

// XuiControl APIs: XuiListItem
enum XUI_LISTITEM_LAYOUT
{
    XUI_LISTITEM_LAYOUT_MULTI_VERTICAL,
    XUI_LISTITEM_LAYOUT_MULTI_HORIZONTAL,
    XUI_LISTITEM_LAYOUT_SINGLE_VERTICAL,
    XUI_LISTITEM_LAYOUT_SINGLE_HORIZONTAL,
    XUI_LISTITEM_LAYOUT_GRID_VERTICAL,
    XUI_LISTITEM_LAYOUT_GRID_HORIZONTAL
};
HRESULT APIENTRY XuiListItemGetLayout(HXUIOBJ hObj, XUI_LISTITEM_LAYOUT *pnLayout);

// XuiControl APIs: XuiNavButton
BOOL    APIENTRY XuiControlIsNavButton(HXUIOBJ hObj);
BOOL    APIENTRY XuiControlIsBackButton(HXUIOBJ hObj);
LPCWSTR APIENTRY XuiNavButtonGetPressPath(HXUIOBJ hObj);
BOOL    APIENTRY XuiNavButtonGetStayVisible(HXUIOBJ hObj);
HRESULT APIENTRY XuiNavButtonGetTransIndices(HXUIOBJ hObj, UINT *puSrcTransIndex, UINT *puDestTransIndex);

// XuiControl APIs: XuiProgressBar
HRESULT APIENTRY XuiProgressBarSetRange(HXUIOBJ hObj, int nRangeMin, int nRangeMax);
HRESULT APIENTRY XuiProgressBarGetRange(HXUIOBJ hObj, int *pnRangeMin, int *pnRangeMax);
HRESULT APIENTRY XuiProgressBarSetValue(HXUIOBJ hObj, int nValue);
HRESULT APIENTRY XuiProgressBarGetValue(HXUIOBJ hObj, int *pnValue);

// XuiControl APIs: XuiRadioGroup
#define XuiRadioGroupGetCurSel XuiContainerGetCurSel
#define XuiRadioGroupSetCurSel XuiContainerSetCurSel
#define XuiRadioGroupBubbleSelChanged XuiContainerBubbleSelChanged
#define XuiRadioGroupGetItemControl XuiContainerGetItemControl

static __declspec(noinline) HXUIOBJ XuiRadioGroupGetChecked(HXUIOBJ hObj)
{
    XUIMessage msg;
    XUIMessageChecked msgData;
    XuiMessageGetChecked(&msg, &msgData);
    XuiSendMessage(hObj,&msg);
    return msgData.hChecked;
}

static __declspec(noinline) HRESULT XuiRadioGroupSetChecked(HXUIOBJ hObj, HXUIOBJ hCheck)
{
    XUIMessage msg;
    XUIMessageChecked msgData;
    XuiMessageSetChecked(&msg, &msgData, hCheck);
    return XuiSendMessage(hObj,&msg);
}

// XuiControl APIs: XuiScene
HRESULT APIENTRY XuiSceneCreate(LPCWSTR szBasePath, LPCWSTR szScenePath, void* pvInitData, HXUIOBJ* phScene);
HRESULT APIENTRY XuiSceneNavigateFirst(HXUIOBJ hParent, HXUIOBJ hFirst, BYTE UserIndex);
HRESULT APIENTRY XuiSceneNavigateForward(HXUIOBJ hCur, BOOL bStayVisible, HXUIOBJ hFwd, BYTE UserIndex);
HRESULT APIENTRY XuiSceneNavigateBack(HXUIOBJ hCur, HXUIOBJ hBack, BYTE UserIndex);
HRESULT APIENTRY XuiSceneNavigateBackToFirst(HXUIOBJ hCur, BYTE UserIndex);
HRESULT APIENTRY XuiSceneInterruptTransitions(HXUIOBJ hCur);
BOOL APIENTRY XuiSceneIsInTransition(HXUIOBJ hScene);

HRESULT APIENTRY XuiSetInterruptTransitionsDefault(BOOL bInterrupt);
BOOL APIENTRY XuiGetInterruptTransitionsDefault(void);

#define XUI_INTERRUPT_TRANSITIONS_DEFAULT   0
#define XUI_INTERRUPT_TRANSITIONS_DISABLE   1
#define XUI_INTERRUPT_TRANSITIONS_ENABLE    2

#define XUI_TIMER_TRANSITION_ENABLE_INPUT 0xAABBCCDD

#define XUI_NAV_FORWARD    0
#define XUI_NAV_BACK       1
HRESULT APIENTRY XuiSceneSetTransIndex(HXUIOBJ hScene, DWORD dwTransType, UINT uTransIndex);

static __declspec(noinline) HXUIOBJ XuiSceneGetBackScene(HXUIOBJ hScene)
{
    XUIMessage msg;
    XUIMessageGetBackScene msgData;
    XuiMessageGetBackScene(&msg, &msgData);
    XuiSendMessage(hScene, &msg);
    return msgData.hBackScene;
}
static __declspec(noinline) HXUIOBJ XuiSceneSetBackScene(HXUIOBJ hScene, HXUIOBJ hBackScene)
{
    XUIMessage msg;
    XUIMessageSetBackScene msgData;
    XuiMessageSetBackScene(&msg, &msgData, hBackScene);
    XuiSendMessage(hScene, &msg);
    return msgData.hOldBackScene;
}

#define XUI_TRANSITION_ACTION_RELINK    0   // relink target of transition when complete
#define XUI_TRANSITION_ACTION_UNLINK    1   // unlink target of transition when complete
#define XUI_TRANSITION_ACTION_DESTROY   2   // destroy target of transition when complete
HRESULT APIENTRY XuiScenePlayToTransition(HXUIOBJ hScene);                          // -> scene
HRESULT APIENTRY XuiScenePlayFromTransition(HXUIOBJ hScene, DWORD dwTransAction);   //    scene ->
HRESULT APIENTRY XuiScenePlayBackToTransition(HXUIOBJ hScene);                      //    scene <-
HRESULT APIENTRY XuiScenePlayBackFromTransition(HXUIOBJ hScene);                    // <- scene

// XuiControl APIs: XuiScrollEnd
enum XUI_SCROLLEND_DIRECTION
{
    XUI_SCROLLEND_DIRECTION_UP,
    XUI_SCROLLEND_DIRECTION_DOWN,
    XUI_SCROLLEND_DIRECTION_LEFT,
    XUI_SCROLLEND_DIRECTION_RIGHT
};
HRESULT APIENTRY XuiScrollEndGetDirection(HXUIOBJ hObj, XUI_SCROLLEND_DIRECTION *pnDirection);

static __declspec(noinline) HRESULT XuiControlSetScrollEndState(HXUIOBJ hObj, XUI_SCROLLEND_STATE nState)
{
    XUIMessage msg;
    XUIMessageSetScrollEndState msgData;
    XuiMessageSetScrollEndState(&msg, &msgData, nState);
    return XuiSendMessage(hObj,&msg);
}

// XuiControl APIs: XuiSlider
HRESULT APIENTRY XuiSliderSetRange(HXUIOBJ hObj, int nRangeMin, int nRangeMax);
HRESULT APIENTRY XuiSliderGetRange(HXUIOBJ hObj, int *pnRangeMin, int *pnRangeMax);
HRESULT APIENTRY XuiSliderSetValue(HXUIOBJ hObj, int nValue);
HRESULT APIENTRY XuiSliderGetValue(HXUIOBJ hObj, int *pnValue);
HRESULT APIENTRY XuiSliderSetStep(HXUIOBJ hObj, int nStep);
HRESULT APIENTRY XuiSliderGetStep(HXUIOBJ hObj, int *pnStep);
HRESULT APIENTRY XuiSliderSetAccel(HXUIOBJ hObj, int nAccelInc, DWORD dwAccelTime);
HRESULT APIENTRY XuiSliderGetAccel(HXUIOBJ hObj, int *pnAccelInc, DWORD *pdwAccelTime);

// XuiControl APIs: XuiTabScene
HRESULT APIENTRY XuiTabSceneEnableTabbing(HXUIOBJ hObj, BOOL bEnable);
HRESULT APIENTRY XuiTabSceneGotoNext(HXUIOBJ hObj);
HRESULT APIENTRY XuiTabSceneGotoPrev(HXUIOBJ hObj);
HRESULT APIENTRY XuiTabSceneGoto(HXUIOBJ hObj, UINT uTab, BOOL bSetFocus);
UINT APIENTRY XuiTabSceneGetCount(HXUIOBJ hObj);
BOOL APIENTRY XuiTabSceneCanUserTab(HXUIOBJ hObj);
UINT APIENTRY XuiTabSceneGetCurrentTab(HXUIOBJ hObj, HXUIOBJ* phCurTab);

// XuiImagePresenter mode flags
#define XUI_IMAGE_PRESENTER_MODE_NORMAL                         0x00000000
#define XUI_IMAGE_PRESENTER_MODE_AUTOSIZE                       0x00000001
#define XUI_IMAGE_PRESENTER_MODE_CENTER                         0x00000002
#define XUI_IMAGE_PRESENTER_MODE_STRETCH                        0x00000004
#define XUI_IMAGE_PRESENTER_MODE_STRETCH_MAINTAIN_ASPECT        0x00000008
#define XUI_IMAGE_PRESENTER_MODE_STRETCH_CENTER_MAINTAIN_ASPECT 0x00000010

// XuiVideo apis
HRESULT APIENTRY XuiVideoRegister(void);
HRESULT APIENTRY XuiVideoUnregister(void);
BOOL APIENTRY XuiVideoIsPaused(HXUIOBJ hObj);
HRESULT APIENTRY XuiVideoPause(HXUIOBJ hObj, BOOL bPause);
HRESULT APIENTRY XuiVideoPlay(HXUIOBJ hObj, LPCWSTR szFile);

typedef struct IXMediaXmvPlayer IXMediaXmvPlayer;
HRESULT APIENTRY XuiVideoSetPlayer(HXUIOBJ hObj, IXMediaXmvPlayer* pPlayer);

// XuiMessageBox apis

HRESULT APIENTRY XuiMessageBoxButtonRegister(void);
HRESULT APIENTRY XuiMessageBoxButtonUnregister(void);

// Center the message box on the parent window
#define XUI_MB_CENTER_ON_PARENT                         0x00000001

HRESULT APIENTRY XuiShowMessageBox(HXUIOBJ hOwner, LPCWSTR szTitle, LPCWSTR szText, int nButtons, LPCWSTR *pButtonsText);
HRESULT APIENTRY XuiShowMessageBoxEx(LPCWSTR szVisual, HXUIOBJ hParent, HXUIOBJ hOwner, LPCWSTR szTitle, LPCWSTR szText, int nButtons, LPCWSTR *pButtonsText, int nFocusButton, DWORD dwFlags);

// XuiSound apis
HRESULT APIENTRY XuiSoundMute(HXUIOBJ hObj, BOOL bMute);
BOOL APIENTRY XuiSoundIsInVisual(HXUIOBJ hObj);
HRESULT APIENTRY XuiSoundSetDirty(HXUIOBJ hObj);
BOOL APIENTRY XuiSoundIsDirty(HXUIOBJ hObj);
HRESULT APIENTRY XuiSoundXACTRegister(void);
HRESULT APIENTRY XuiSoundXACTUnregister(void);
HRESULT APIENTRY XuiSoundXAudioRegister(void);
HRESULT APIENTRY XuiSoundXAudioUnregister(void);

static __declspec(noinline) HRESULT XuiSoundPlay(HXUIOBJ hObj)
{
    XUIMessage msg;
    XuiMessage(&msg, XM_SOUND_PLAY);
    return XuiSendMessage(XuiGetOuter(hObj), &msg);
}

static __declspec(noinline) HRESULT XuiSoundStop(HXUIOBJ hObj)
{
    XUIMessage msg;
    XuiMessage(&msg, XM_SOUND_STOP);
    return XuiSendMessage(XuiGetOuter(hObj), &msg);
}

HRESULT APIENTRY XuiRegisterTypeface(TypefaceDescriptor * pDescriptor, BOOL bMakeDefault);
HRESULT APIENTRY XuiUnregisterTypeface(LPCWSTR szTypeface);
HRESULT APIENTRY XuiEnumerateTypefaces(TypefaceDescriptor ** ppaDescriptors, DWORD * pdwCount);
VOID    APIENTRY XuiDestroyTypefaceList(TypefaceDescriptor * pDescriptors, DWORD dwCount);

#ifdef __cplusplus
}
#endif

#pragma warning(pop)

#endif // __XUIELEMENT_H__

