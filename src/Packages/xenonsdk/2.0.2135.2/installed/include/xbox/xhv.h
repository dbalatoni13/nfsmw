/***************************************************************************
 *
 *  xhv.h -- This module defines the XBox High-Level Voice APIs
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 ***************************************************************************/

#ifndef __XHV_H__
#define __XHV_H__


#ifndef __XAUDIO_INCLUDED__
#error You must include xaudio.h before xhv.h
#endif

#ifndef __XONLINE__
#error You must include xonline.h before xhv.h
#endif


#ifdef __cplusplus
extern "C" {
#endif


//
// Constants
//

#define XHVINLINE                               FORCEINLINE

//
// The XHV worker thread will initially run on this processor
//

#define XHV_WORKER_THREAD_PROCESSOR             (1)

#define XHV_MAX_REMOTE_TALKERS                  (30)
#define XHV_MAX_LOCAL_TALKERS                   (4)

//
// At the moment, you may only have up to this many enabled processing modes
// for local and remote talkers (separately).
//

#define XHV_MAX_PROCESSING_MODES                (2)

//
// When setting playback priorities, talker pairs between
// XHV_PLAYBACK_PRIORITY_MAX and XHV_PLAYBACK_PRIORITY_MIN, inclusive, will be
// heard, while any other value will result in muting that pair.
//

#define XHV_PLAYBACK_PRIORITY_MAX               (0)
#define XHV_PLAYBACK_PRIORITY_MIN               (0xFFFF)
#define XHV_PLAYBACK_PRIORITY_NEVER             (0xFFFFFFFF)

//
// Each packet reported by voice chat mode is the following size (including the
// XHV_CODEC_HEADER)
//

#define XHV_VOICECHAT_MODE_PACKET_SIZE          (10)

//
// When supplying a buffer to GetLocalChatData, you won't have to supply a
// buffer any larger than the following number of packets (or
// XHV_MAX_VOICECHAT_PACKETS * XHV_VOICECHAT_MODE_PACKET_SIZE bytes)
//

#define XHV_MAX_VOICECHAT_PACKETS               (10)

//
// XHV's final output submix voice consists of one channel per local talker.
// Each channel contains the mixed voice for its user index.  The data can be
// mapped to speakers or headsets.
//

#define XHV_XAUDIO_OUTPUT_CHANNEL_COUNT         (XHV_MAX_LOCAL_TALKERS)

//
// XHV uses three stages of submix voices.  Hence, XAudio's maximum stage count
// must be set to at least this value.
//

#define XHV_XAUDIO_SUBMIX_STAGE_COUNT           (3)

//
// XHV requires the use of a custom XAudio effect.  This effect has the
// following Effect ID.  Title effects should not use this ID.
//

#define XHV_XAUDIO_OUTPUT_EFFECTID              (XAUDIOFXID_STATICMAX)

//
// The number of effects in any chain passed into XHV is limited to this many.
//

#define XHV_MAX_FXCHAIN_EFFECTS                 (1)

//
// The microphone callback is given PCM data in this format.  Note that XAudio
// effect chains will process the data in the native XAudio format (see XAudio
// for details).
//

#define XHV_PCM_BYTES_PER_SAMPLE                (2)
#define XHV_PCM_SAMPLE_RATE                     (16000)


//
// Data Ready Flags.  These flags are set when there is local data waiting to be
// consumed (e.g. through GetLocalChatData).  GetLocalDataFlags() allows you to
// get the current state of these flags without entering XHV's critical section.
// Each mask is 4 bits, one for each local talker.  The least significant bit in
// each section indicates data is available for user index 0, while the most
// significant bit indicates user index 3.
//

#define XHV_VOICECHAT_DATA_READY_MASK           (0xF)
#define XHV_VOICECHAT_DATA_READY_OFFSET         (0)


//
// Interfaces
//

typedef struct IXHVEngine                       *LPIXHVENGINE, *PIXHVENGINE;

//
// Typedefs, Enums and Structures
//

typedef CONST LPVOID                            XHV_PROCESSING_MODE, *PXHV_PROCESSING_MODE;
typedef DWORD                                   XHV_PLAYBACK_PRIORITY;
typedef VOID(*PFNMICRAWDATAREADY)(
    IN  DWORD                                   dwUserIndex,
    IN  PVOID                                   pvData,
    IN  DWORD                                   dwSize,
    IN  PBOOL                                   pVoiceDetected
    );

typedef DWORD                                   XHV_LOCK_TYPE;

#define XHV_LOCK_TYPE_LOCK                      0
#define XHV_LOCK_TYPE_TRYLOCK                   1
#define XHV_LOCK_TYPE_UNLOCK                    2
#define XHV_LOCK_TYPE_COUNT                     3

//
// Supported processing modes
//

extern  CONST LPVOID                            _xhv_loopback_mode;
extern  CONST LPVOID                            _xhv_voicechat_mode;

#define XHV_LOOPBACK_MODE                       _xhv_loopback_mode
#define XHV_VOICECHAT_MODE                      _xhv_voicechat_mode

//
// You must specify the following initialization parameters at creation time
//

typedef struct XHV_INIT_PARAMS
{
    DWORD                                       dwMaxRemoteTalkers;
    DWORD                                       dwMaxLocalTalkers;
    PXHV_PROCESSING_MODE                        localTalkerEnabledModes;
    DWORD                                       dwNumLocalTalkerEnabledModes;
    PXHV_PROCESSING_MODE                        remoteTalkerEnabledModes;
    DWORD                                       dwNumRemoteTalkerEnabledModes;
    BOOL                                        bCustomVADProvided;
    BOOL                                        bRelaxPrivileges;
    PFNMICRAWDATAREADY                          pfnMicrophoneRawDataReady;
    LPCXAUDIOVOICEFXCHAIN                       pfxDefaultRemoteTalkerFX;
    LPCXAUDIOVOICEFXCHAIN                       pfxDefaultTalkerPairFX;
    LPCXAUDIOVOICEFXCHAIN                       pfxOutputFX;
} XHV_INIT_PARAMS, *PXHV_INIT_PARAMS;

//
// This header appears at the beginning of each blob of data reported by voice
// chat mode
//

#pragma pack(push, 1)
typedef struct XHV_CODEC_HEADER
{
    WORD                                        bMsgNo :  4;
    WORD                                        wSeqNo : 12;
} XHV_CODEC_HEADER, *PXHV_CODEC_HEADER;
#pragma pack (pop)

//
//  IXHVEngine interface
//

#undef  INTERFACE
#define INTERFACE IXHVEngine

DECLARE_INTERFACE(IXHVEngine)
{
    STDMETHOD_(ULONG, AddRef)(
        IN                                      THIS
        )   PURE;

    STDMETHOD_(ULONG, Release)(
        IN                                      THIS
        )   PURE;

    //
    // Locking
    //

    STDMETHOD(Lock)(
        IN                                      THIS_
        IN  XHV_LOCK_TYPE                       lockType
        );

    //
    // Processing mode management
    //

    STDMETHOD(StartLocalProcessingModes)(
        IN                                      THIS_
        IN  DWORD                               dwUserIndex,
        IN  CONST PXHV_PROCESSING_MODE          processingModes,
        IN  DWORD                               dwNumProcessingModes
        )   PURE;

    STDMETHOD(StopLocalProcessingModes)(
        IN                                      THIS_
        IN  DWORD                               dwUserIndex,
        IN  CONST PXHV_PROCESSING_MODE          processingModes,
        IN  DWORD                               dwNumProcessingModes
        )   PURE;

    STDMETHOD(StartRemoteProcessingModes)(
        IN                                      THIS_
        IN  XUID                                xuidRemoteTalker,
        IN  CONST PXHV_PROCESSING_MODE          processingModes,
        IN  DWORD                               dwNumProcessingModes
        )   PURE;

    STDMETHOD(StopRemoteProcessingModes)(
        IN                                      THIS_
        IN  XUID                                xuidRemoteTalker,
        IN  CONST PXHV_PROCESSING_MODE          processingModes,
        IN  DWORD                               dwNumProcessingModes
        )   PURE;

    STDMETHOD(SetMaxDecodePackets)(
        IN                                      THIS_
        IN  DWORD                               dwMaxDecodePackets
        )   PURE;

    //
    // Manage talkers
    //

    STDMETHOD(RegisterLocalTalker)(
        IN                                      THIS_
        IN  DWORD                               dwUserIndex
        )   PURE;

    STDMETHOD(UnregisterLocalTalker)(
        IN                                      THIS_
        IN  DWORD                               dwUserIndex
        )   PURE;

    STDMETHOD(RegisterRemoteTalker)(
        IN                                      THIS_
        IN  XUID                                xuidRemoteTalker,
        IN  LPCXAUDIOVOICEFXCHAIN               pfxRemoteTalkerFX,
        IN  LPCXAUDIOVOICEFXCHAIN               pfxTalkerPairFX,
        IN  LPXAUDIOSUBMIXVOICE                 pOutputVoice
        )   PURE;

    STDMETHOD(UnregisterRemoteTalker)(
        IN                                      THIS_
        IN  XUID                                xuidRemoteTalker
        )   PURE;

    STDMETHOD(GetRemoteTalkers)(
        IN                                      THIS_
        OUT PDWORD                              pdwRemoteTalkersCount,
        OUT PXUID                               pxuidRemoteTalkers
        )   PURE;

    STDMETHOD_(BOOL, IsHeadsetPresent)(
        IN                                      THIS_
        IN  DWORD                               dwUserIndex
        )   PURE;

    STDMETHOD_(BOOL, IsLocalTalking)(
        IN                                      THIS_
        IN  DWORD                               dwUserIndex
        )   PURE;

    STDMETHOD_(BOOL, IsRemoteTalking)(
        IN                                      THIS_
        IN  XUID                                xuidRemoteTalker
        )   PURE;

    STDMETHOD(SetRemoteTalkerOutputVoice)(
        IN                                      THIS_
        IN  XUID                                xuidRemoteTalker,
        IN  LPXAUDIOSUBMIXVOICE                 pOutputVoice
        )   PURE;

    STDMETHOD(SetRemoteTalkerEffectParam)(
        IN                                      THIS_
        IN  XUID                                xuidRemoteTalker,
        IN  BOOL                                fPairEffect,
        IN  XAUDIOVOICEFXINDEX                  bEffectIndex,
        IN  XAUDIOFXPARAMID                     bParamId,
        IN  XAUDIOFXPARAMTYPE                   bParamType,
        IN  LPCXAUDIOFXPARAM                    pParam
        )   PURE;

    STDMETHOD_(DWORD, GetDataReadyFlags)(
        IN                                      THIS
        )   PURE;

    //
    // Voice chat mode functions
    //

    STDMETHOD(GetLocalChatData)(
        IN                                      THIS_
        IN      DWORD                           dwUserIndex,
        OUT     PBYTE                           pbData,
        IN OUT  PDWORD                          pdwSize,
        OUT     PDWORD                          pdwPackets
        )   PURE;

    STDMETHOD(SetPlaybackPriority)(
        IN                                      THIS_
        IN  XUID                                xuidRemoteTalker,
        IN  DWORD                               dwUserIndex,
        IN  XHV_PLAYBACK_PRIORITY               playbackPriority
        )   PURE;

    STDMETHOD(SubmitIncomingChatData)(
        IN                                      THIS_
        IN      XUID                            xuidRemoteTalker,
        IN      CONST BYTE*                     pbData,
        IN OUT  PDWORD                          pdwSize
        )   PURE;
};

#pragma push_macro("VTBL")
#pragma push_macro("THIS")
#pragma push_macro("THIS_")

#undef  VTBL
#undef  THIS
#undef  THIS_

#if defined(__cplusplus) && !defined(CINTERFACE)
#define VTBL(p) (p)
#define THIS
#define THIS_
#else // defined(__cplusplus) && !defined(CINTERFACE)
#define VTBL(p) ((p)->lpVtbl)
#define THIS    pThis
#define THIS_   pThis,
#endif // defined(__cplusplus) && !defined(CINTERFACE)

XHVINLINE
STDMETHODIMP_(ULONG)
IXHVEngine_AddRef(
    IN  PIXHVENGINE                             pThis
    )
{
    return VTBL(pThis)->AddRef(THIS);
}

XHVINLINE
STDMETHODIMP_(ULONG)
IXHVEngine_Release(
    IN  PIXHVENGINE                             pThis
    )
{
    return VTBL(pThis)->Release(THIS);
}

XHVINLINE
STDMETHODIMP
IXHVEngine_Lock(
    IN  PIXHVENGINE                             pThis,
    IN  XHV_LOCK_TYPE                           lockType
    )
{
    return VTBL(pThis)->Lock(THIS_ lockType);
}

XHVINLINE
STDMETHODIMP
IXHVEngine_StartLocalProcessingModes(
    IN  PIXHVENGINE                             pThis,
    IN  DWORD                                   dwUserIndex,
    IN  CONST PXHV_PROCESSING_MODE              processingModes,
    IN  DWORD                                   dwNumProcessingModes
    )
{
    return VTBL(pThis)->StartLocalProcessingModes(THIS_ dwUserIndex, processingModes, dwNumProcessingModes);
}

XHVINLINE
STDMETHODIMP
IXHVEngine_StopLocalProcessingModes(
    IN  PIXHVENGINE                             pThis,
    IN  DWORD                                   dwUserIndex,
    IN  CONST PXHV_PROCESSING_MODE              processingModes,
    IN  DWORD                                   dwNumProcessingModes
    )
{
    return VTBL(pThis)->StopLocalProcessingModes(THIS_ dwUserIndex, processingModes, dwNumProcessingModes);
}

XHVINLINE
STDMETHODIMP
IXHVEngine_StartRemoteProcessingModes(
    IN  PIXHVENGINE                             pThis,
    IN  XUID                                    Xuid,
    IN  CONST PXHV_PROCESSING_MODE              processingModes,
    IN  DWORD                                   dwNumProcessingModes
    )
{
    return VTBL(pThis)->StartRemoteProcessingModes(THIS_ Xuid, processingModes, dwNumProcessingModes);
}

XHVINLINE
STDMETHODIMP
IXHVEngine_StopRemoteProcessingModes(
    IN  PIXHVENGINE                             pThis,
    IN  XUID                                    Xuid,
    IN  CONST PXHV_PROCESSING_MODE              processingModes,
    IN  DWORD                                   dwNumProcessingModes
    )
{
    return VTBL(pThis)->StopRemoteProcessingModes(THIS_ Xuid, processingModes, dwNumProcessingModes);
}

XHVINLINE
STDMETHODIMP
IXHVEngine_SetMaxDecodePackets(
    IN  PIXHVENGINE                             pThis,
    IN  DWORD                                   dwMaxDecodePackets
    )
{
    return VTBL(pThis)->SetMaxDecodePackets(THIS_ dwMaxDecodePackets);
}

XHVINLINE
STDMETHODIMP
IXHVEngine_RegisterLocalTalker(
    IN  PIXHVENGINE                             pThis,
    IN  DWORD                                   dwUserIndex
    )
{
    return VTBL(pThis)->RegisterLocalTalker(THIS_ dwUserIndex);
}

XHVINLINE
STDMETHODIMP
IXHVEngine_UnregisterLocalTalker(
    IN  PIXHVENGINE                             pThis,
    IN  DWORD                                   dwUserIndex
    )
{
    return VTBL(pThis)->UnregisterLocalTalker(THIS_ dwUserIndex);
}

XHVINLINE
STDMETHODIMP
IXHVEngine_RegisterRemoteTalker(
    IN  PIXHVENGINE                             pThis,
    IN  XUID                                    xuidRemoteTalker,
    IN  LPCXAUDIOVOICEFXCHAIN                   pfxRemoteTalkerFX,
    IN  LPCXAUDIOVOICEFXCHAIN                   pfxTalkerPairFX,
    IN  LPXAUDIOSUBMIXVOICE                     pOutputVoice
    )
{
    return VTBL(pThis)->RegisterRemoteTalker(THIS_ xuidRemoteTalker, pfxRemoteTalkerFX, pfxTalkerPairFX, pOutputVoice);
}

XHVINLINE
STDMETHODIMP
IXHVEngine_UnregisterRemoteTalker(
    IN  PIXHVENGINE                             pThis,
    IN  XUID                                    xuidRemoteTalker
    )
{
    return VTBL(pThis)->UnregisterRemoteTalker(THIS_ xuidRemoteTalker);
}

XHVINLINE
STDMETHODIMP
IXHVEngine_GetRemoteTalkers(
    IN  PIXHVENGINE                             pThis,
    OUT PDWORD                                  pdwRemoteTalkersCount,
    OUT PXUID                                   pxuidRemoteTalkers
    )
{
    return VTBL(pThis)->GetRemoteTalkers(THIS_ pdwRemoteTalkersCount, pxuidRemoteTalkers);
}

XHVINLINE
STDMETHODIMP_(BOOL)
IXHVEngine_IsHeadsetPresent(
    IN  PIXHVENGINE                             pThis,
    IN  DWORD                                   dwUserIndex
    )
{
    return VTBL(pThis)->IsHeadsetPresent(THIS_ dwUserIndex);
}

XHVINLINE
STDMETHODIMP_(BOOL)
IXHVEngine_IsLocalTalking(
    IN  PIXHVENGINE                             pThis,
    IN  DWORD                                   dwUserIndex
    )
{
    return VTBL(pThis)->IsLocalTalking(THIS_ dwUserIndex);
}

XHVINLINE
STDMETHODIMP_(BOOL)
IXHVEngine_IsRemoteTalking(
    IN  PIXHVENGINE                             pThis,
    IN  XUID                                    xuidRemoteTalker
    )
{
    return VTBL(pThis)->IsRemoteTalking(THIS_ xuidRemoteTalker);
}

XHVINLINE
STDMETHODIMP
IXHVEngine_SetRemoteTalkerOutputVoice(
    IN  PIXHVENGINE                             pThis,
    IN  XUID                                    xuidRemoteTalker,
    IN  LPXAUDIOSUBMIXVOICE                     pOutputVoice
    )
{
    return VTBL(pThis)->SetRemoteTalkerOutputVoice(THIS_ xuidRemoteTalker, pOutputVoice);
}

XHVINLINE
STDMETHODIMP
IXHVEngine_SetRemoteTalkerEffectParam(
    IN  PIXHVENGINE                             pThis,
    IN  XUID                                    xuidRemoteTalker,
    IN  BOOL                                    fPairEffect,
    IN  XAUDIOVOICEFXINDEX                      bEffectIndex,
    IN  XAUDIOFXPARAMID                         bParamId,
    IN  XAUDIOFXPARAMTYPE                       bParamType,
    IN  LPCXAUDIOFXPARAM                        pParam
    )
{
    return VTBL(pThis)->SetRemoteTalkerEffectParam(THIS_ xuidRemoteTalker, fPairEffect, bEffectIndex, bParamId, bParamType, pParam);
}

XHVINLINE
STDMETHODIMP_(DWORD)
IXHVEngine_GetDataReadyFlags(
    IN  PIXHVENGINE                             pThis
    )
{
    return VTBL(pThis)->GetDataReadyFlags(THIS);
}

XHVINLINE
STDMETHODIMP
IXHVEngine_GetLocalChatData(
    IN      PIXHVENGINE                         pThis,
    IN      DWORD                               dwUserIndex,
    OUT     PBYTE                               pbData,
    IN OUT  PDWORD                              pdwSize,
    OUT     PDWORD                              pdwPackets
    )
{
    return VTBL(pThis)->GetLocalChatData(THIS_ dwUserIndex, pbData, pdwSize, pdwPackets);
}

XHVINLINE
STDMETHODIMP
IXHVEngine_SetPlaybackPriority(
    IN  PIXHVENGINE                             pThis,
    IN  XUID                                    xuidRemoteTalker,
    IN  DWORD                                   dwUserIndex,
    IN  XHV_PLAYBACK_PRIORITY                   playbackPriority
    )
{
    return VTBL(pThis)->SetPlaybackPriority(THIS_ xuidRemoteTalker, dwUserIndex, playbackPriority);
}

XHVINLINE
STDMETHODIMP
IXHVEngine_SubmitIncomingChatData(
    IN      PIXHVENGINE                         pThis,
    IN      XUID                                xuidRemoteTalker,
    IN      CONST BYTE*                         pbData,
    IN OUT  PDWORD                              pdwSize
    )
{
    return VTBL(pThis)->SubmitIncomingChatData(THIS_ xuidRemoteTalker, pbData, pdwSize);
}

#pragma pop_macro("VTBL")
#pragma pop_macro("THIS")
#pragma pop_macro("THIS_")

//
// Use the following function to create an instance of the XHV engine
//

XBOXAPI
HRESULT
WINAPI
XHVCreateEngine(
    IN  PXHV_INIT_PARAMS                        pParams,
    OUT PHANDLE                                 phWorkerThread,
    OUT PIXHVENGINE*                            ppEngine
    );



#ifdef __cplusplus
}
#endif



#endif // __XHV_H__

