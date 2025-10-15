/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       xuianimate.h
 *  Content:    Xbox 360 UI animation defines, structures, and functions
 *
 ****************************************************************************/

#ifndef __XUIANIMATE_H__
#define __XUIANIMATE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XUISubtimelineProp XUISubtimelineProp;
typedef struct XUIKeyFrame XUIKeyFrame;
typedef struct XUINamedFrame XUINamedFrame;
typedef struct XUITimelineCtl XUITimelineCtl;
typedef struct XUITimeline XUITimeline;
typedef struct XUISubtimeline XUISubtimeline;
typedef enum XUI_NAMEDFRAME_COMMAND XUI_NAMEDFRAME_COMMAND;
typedef enum XUI_INTERPOLATE XUI_INTERPOLATE;
typedef struct XUIObjectData XUIObjectData;

// timeline control commands
// these commands can be set on a named frame to control how the timeline moves
enum XUI_NAMEDFRAME_COMMAND
{
    XUI_COMMAND_PLAY,           // the default command: keeps playing (no parameter required)
    XUI_COMMAND_STOP,           // stops the timeline on this frame (no parameter required)
    XUI_COMMAND_GOTO,           // the next frame is set to the parameter is specified on the named frame
    XUI_COMMAND_GOTOANDPLAY,    // same as XUI_COMMAND_GOTO but plays at the target (parameter)
    XUI_COMMAND_GOTOANDSTOP     // same as XUI_COMMAND_GOTO but stops at the target (parameter)
};

// Keyframe interpolation types
enum XUI_INTERPOLATE
{
    XUI_INTERPOLATE_LINEAR,     // linear interpolation
    XUI_INTERPOLATE_NONE,       // no interpolation
    XUI_INTERPOLATE_EASE,       // ease in/out interpolation
};

void APIENTRY XUIKeyFrame_Construct(XUIKeyFrame* pThis);
void APIENTRY XUIKeyFrame_Destruct(XUIKeyFrame* pThis);
void APIENTRY XUIKeyFrame_SetPropVals(XUIKeyFrame* pThis, XUIElementPropVal *pPropVals);
XUIKeyFrame* APIENTRY XUIKeyFrame_AllocArray(int nNumKeyFrames, int nNumProps, XUIElementPropVal **ppPropVals);
void APIENTRY XUIKeyFrame_DeleteArray(XUIKeyFrame* pArray, int nCount, int nNumProps);

struct XUIKeyFrame
{
    DECLARE_XUI_ALLOC()

    DWORD m_dwFrame;

    XUI_INTERPOLATE m_nInterpolationType;
    // used when m_nInterpolationType==XUI_INTERPOLATE_EASE
    // these 2 points specify the control points of a cubic spline
    // used to smooth out the animation
    D3DXVECTOR2 m_vEaseIn;
    D3DXVECTOR2 m_vEaseOut;

    // array of property values. parent subtimeline contains the 
    // property reference information as well as the number of properties
    XUIElementPropVal *m_pPropVals;
    
#ifdef __cplusplus

    XUIKeyFrame()
    {
        XUIKeyFrame_Construct(this);
    }
    ~XUIKeyFrame()
    {
        XUIKeyFrame_Destruct(this);
    }
    void SetPropVals(XUIElementPropVal *pPropVals)
    {
        XUIKeyFrame_SetPropVals(this, pPropVals);
    }

#endif
};

void APIENTRY XUINamedFrame_Construct(XUINamedFrame* pThis);
void APIENTRY XUINamedFrame_Destruct(XUINamedFrame* pThis);

struct XUINamedFrame
{
    DECLARE_XUI_ALLOC()

    LPCWSTR m_szName;
    DWORD m_dwFrame;
    XUI_NAMEDFRAME_COMMAND m_nCommand;
    int m_nTargetFrame;
    
#ifdef __cplusplus

    XUINamedFrame()
    {
        XUINamedFrame_Construct(this);
    }
    ~XUINamedFrame()
    {
        XUINamedFrame_Destruct(this);
    }
#endif
};

#define TIMELINE_STATE_STOPPED      0x00000001
#define TIMELINE_STATE_START        0x00000002

void APIENTRY XUITimelineCtl_Construct(XUITimelineCtl* pThis);
void APIENTRY XUITimelineCtl_Reset(XUITimelineCtl* pThis);

struct XUITimelineCtl
{
    DECLARE_XUI_ALLOC()

    DWORD m_nState;
    int m_nRangeStart;
    int m_nRangeEnd;
    BOOL m_bLoop;
    int m_nNotifyStart;
    int m_nNotifyEnd;
    
#ifdef __cplusplus

    XUITimelineCtl()
    {
        XUITimelineCtl_Construct(this);
    }
    void Reset()
    {
        XUITimelineCtl_Reset(this);
    }
#endif
};


//
// XUISubtimelineProp:
// This data structure contains the property reference information required to
// to apply a keyframe property value to it's target object
//
struct XUISubtimelineProp
{
    DECLARE_XUI_ALLOC()

    IXUIPropObj *m_pObj;
    DWORD m_dwPropId;
    int m_nIndex;
};

//
// XUISubtimeline:
// This data structure contains the keyframe information and the timeline runtime state for a sub (child) object
//

void APIENTRY XUISubtimeline_Construct(XUISubtimeline* pThis);
void APIENTRY XUISubtimeline_Destruct(XUISubtimeline* pThis);
void APIENTRY XUISubtimeline_Reset(XUISubtimeline* pThis);
void APIENTRY XUISubtimeline_Detach(XUISubtimeline* pThis, XUISubtimeline *pOther);
void APIENTRY XUISubtimeline_SetKeyFrames(XUISubtimeline* pThis, XUIKeyFrame *pKeyFrames, int nCount, BOOL bShareKeyFrames, XUIObjectData *pKeyFrameOwner);
void APIENTRY XUISubtimeline_Play(XUISubtimeline* pThis, int nStartFrame, int nInitialFrame, int nEndFrame, BOOL bLoop, BOOL bNotify);
void APIENTRY XUISubtimeline_Run(XUISubtimeline* pThis, float dt);
void APIENTRY XUISubtimeline_GotoFrame(XUISubtimeline* pThis, int nFrame);
int APIENTRY XUISubtimeline_GetAnimLength(XUISubtimeline* pThis);
void APIENTRY XUISubtimeline_Clear(XUISubtimeline* pThis);
void APIENTRY XUISubtimeline_NotifyOwner(XUISubtimeline* pThis, DWORD dwMessage);

struct XUISubtimeline
{
    DECLARE_XUI_ALLOC()

    XUITimelineCtl m_state;

    HXUIOBJ m_hObj;
    int m_nNumKeyFrames;
    XUIKeyFrame *m_pKeyFrames;
    int m_nAnimLength;
    DWORD m_dwFlags;
    int m_nInitialFrameOffset;
    float m_fCurrTime;

    int m_nNumProps;
    XUISubtimelineProp *m_pProps;

    BOOL m_bSharedKeyFrames;        // TRUE if m_pKeyFrames is owned else where and should not be freed
    XUIObjectData *m_pKeyFrameOwner;
    
#ifdef __cplusplus

    XUISubtimeline()
    {
        XUISubtimeline_Construct(this);
    }
    ~XUISubtimeline()
    {
        XUISubtimeline_Destruct(this);
    }
    void Reset()
    {
        XUISubtimeline_Reset(this);
    }
    void Detach(XUISubtimeline *pOther)
    {
        XUISubtimeline_Detach(this, pOther);
    }
    void SetKeyFrames(XUIKeyFrame *pKeyFrames, int nCount, BOOL bShareKeyFrames, XUIObjectData *pKeyFrameOwner)
    {
        XUISubtimeline_SetKeyFrames(this, pKeyFrames, nCount, bShareKeyFrames, pKeyFrameOwner);
    }
    void Play(int nStartFrame, int nInitialFrame, int nEndFrame, BOOL bLoop=TRUE, BOOL bNotify=TRUE)
    {
        XUISubtimeline_Play(this, nStartFrame, nInitialFrame, nEndFrame, bLoop, bNotify);
    }
    void Run(float dt)
    {
        XUISubtimeline_Run(this, dt);
    }
    void GotoFrame(int nFrame)
    {
        XUISubtimeline_GotoFrame(this, nFrame);
    }
    int GetAnimLength()
    {
        return XUISubtimeline_GetAnimLength(this);
    }
    void Clear()
    {
        XUISubtimeline_Clear(this);
    }
    void NotifyOwner(DWORD dwMessage)
    {
        XUISubtimeline_NotifyOwner(this, dwMessage);
    }
#endif
};

//
// XUITimeline:
// This data structure contains the subtimelines for the children of a container object
//

void APIENTRY XUITimeline_Construct(XUITimeline* pThis);
void APIENTRY XUITimeline_Destruct(XUITimeline* pThis);
XUINamedFrame* APIENTRY XUITimeline_FindNamedFrame(XUITimeline* pThis, LPCWSTR szName);
int APIENTRY XUITimeline_FindNextNamedFrameAtTime(XUITimeline* pThis, int nStartIndex, DWORD dwFrame);
HRESULT APIENTRY XUITimeline_SetSubtimeline(XUITimeline* pThis, XUISubtimeline *pTimeline, int nIndex);
void APIENTRY XUITimeline_SetTimelines(XUITimeline* pThis, XUISubtimeline *pTimelines, int nCount);
void APIENTRY XUITimeline_UpdateAnimLength(XUITimeline* pThis);
void APIENTRY XUITimeline_Stop(XUITimeline* pThis);
void APIENTRY XUITimeline_Play(XUITimeline* pThis, int nStartFrame, int nInitialFrame, int nEndFrame, BOOL bLoop, BOOL bNotify);
void APIENTRY XUITimeline_GotoFrame(XUITimeline* pThis, int nFrame);
int APIENTRY XUITimeline_GetAnimLength(XUITimeline* pThis);
void APIENTRY XUITimeline_Clear(XUITimeline* pThis);
void APIENTRY XUITimeline_ClearTimelines(XUITimeline* pThis);
void APIENTRY XUITimeline_Run(XUITimeline* pThis, float dt);
void APIENTRY XUITimeline_NotifyOwner(XUITimeline* pThis, DWORD dwMessage);

struct XUITimeline
{
    DECLARE_XUI_ALLOC()

    HXUIOBJ m_hObj;     // handle of owning object
    XUITimelineCtl m_state;

    int m_nNumTimelines;
    XUISubtimeline *m_pTimelines;

    XUINamedFrame *m_pNamedFrames;
    int m_nNumNamedFrames;
    BOOL m_bSharedNamedFrames;
    XUIObjectData *m_pNamedFrameOwner;

    int m_nAnimLength;
    DWORD m_dwFlags;
    int m_nInitialFrameOffset;
    
    int m_nCurrNamedFrame;  // the current named frame index (i.e. the last named frame we've hit)
    BOOL m_bStopOnNext;
    BOOL m_bPlayOnNext;
    
    float m_fCurrTime;

#ifdef __cplusplus
    XUITimeline()
    {
        XUITimeline_Construct(this);
    }
    ~XUITimeline()
    {
        XUITimeline_Destruct(this);
    }
    XUINamedFrame *FindNamedFrame(LPCWSTR szName)
    {
        return XUITimeline_FindNamedFrame(this, szName);
    }
    int FindNextNamedFrameAtTime(int nStartIndex, DWORD dwFrame)
    {
        return XUITimeline_FindNextNamedFrameAtTime(this, nStartIndex, dwFrame);
    }
    HRESULT SetSubtimeline(XUISubtimeline *pTimeline, int nIndex)
    {
        return XUITimeline_SetSubtimeline(this, pTimeline, nIndex);
    }
    void SetTimelines(XUISubtimeline *pTimelines, int nCount)
    {
        XUITimeline_SetTimelines(this, pTimelines, nCount);
    }
    void UpdateAnimLength()
    {
        XUITimeline_UpdateAnimLength(this);
    }
    void Stop()
    {
        XUITimeline_Stop(this);
    }
    void Play(int nStartFrame, int nInitialFrame, int nEndFrame, BOOL bLoop=TRUE, BOOL bNotify=TRUE)
    {
        XUITimeline_Play(this, nStartFrame, nInitialFrame, nEndFrame, bLoop, bNotify);
    }
    void GotoFrame(int nFrame)
    {
        XUITimeline_GotoFrame(this, nFrame);
    }
    int GetAnimLength()
    {
        return XUITimeline_GetAnimLength(this);
    }
    void Clear()
    {
        return XUITimeline_Clear(this);
    }
    void ClearTimelines()
    {
        return XUITimeline_ClearTimelines(this);
    }
    void Run(float dt)
    {
        return XUITimeline_Run(this, dt);
    }
    void NotifyOwner(DWORD dwMessage)
    {
        return XUITimeline_NotifyOwner(this, dwMessage);
    }
#endif
};

HRESULT APIENTRY XuiAnimAddTimeline(XUITimeline *pTimeline);
HRESULT APIENTRY XuiAnimRemoveTimeline(XUITimeline *pTimeline);
HRESULT APIENTRY XuiAnimRun(float dt);

#ifdef __cplusplus
}
#endif

#endif // __XUIANIMATE_H__

