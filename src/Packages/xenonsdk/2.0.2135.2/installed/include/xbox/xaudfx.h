/**************************************************************************
 *
 *  Copyright (C) 2003 - 2005 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       xaudfx.h
 *  Content:    Xbox audio effects.
 *
 **************************************************************************/

#ifndef __XAUDFX_INCLUDED__
#define __XAUDFX_INCLUDED__

#include <xauddefs.h>

#pragma warning(push)
#pragma warning(disable:4201)

#pragma pack(push, 4)

/**************************************************************************
 *
 * IXAudioEffect
 *
 **************************************************************************/

#undef INTERFACE
#define INTERFACE IXAudioEffect

DECLARE_INTERFACE_(IXAudioEffect, IXAudioRefCount)
{
    // IXAudioRefCount
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    // IXAudioEffect
    STDMETHOD(GetInfo)(THIS_ LPXAUDIOFXINFO pInfo) PURE;
    STDMETHOD(GetParam)(THIS_ XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPXAUDIOFXPARAM pParam) PURE;
    STDMETHOD(SetParam)(THIS_ XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPCXAUDIOFXPARAM pParam) PURE;
    STDMETHOD(GetContext)(THIS_ LPVOID *ppContext) PURE;
    STDMETHOD(Process)(THIS_ IXAudioFrameBuffer *pInputBuffer, IXAudioFrameBuffer *pOutputBuffer) PURE;
};

/**************************************************************************
 *
 * IXAudioSourceEffect
 *
 **************************************************************************/

#undef INTERFACE
#define INTERFACE IXAudioSourceEffect

DECLARE_INTERFACE_(IXAudioSourceEffect, IXAudioEffect)
{
    // IXAudioRefCount
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    // IXAudioEffect
    STDMETHOD(GetInfo)(THIS_ LPXAUDIOFXINFO pInfo) PURE;
    STDMETHOD(GetParam)(THIS_ XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPXAUDIOFXPARAM pParam) PURE;
    STDMETHOD(SetParam)(THIS_ XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPCXAUDIOFXPARAM pParam) PURE;
    STDMETHOD(GetContext)(THIS_ LPVOID *ppContext) PURE;
    STDMETHOD(Process)(THIS_ IXAudioFrameBuffer *pInputBuffer, IXAudioFrameBuffer *pOutputBuffer) PURE;

    // IXAudioSourceEffect
    STDMETHOD(SubmitPacket)(THIS_ LPCXAUDIOPACKET pPacket, XAUDIOSUBMITPACKET Flags) PURE;
    STDMETHOD(FlushPackets)(THIS) PURE;
    STDMETHOD(GetState)(THIS_ LPXAUDIOSOURCESTATE pState) PURE;
    STDMETHOD(Start)(THIS) PURE;
    STDMETHOD(Stop)(THIS_ XAUDIOSTOP Flags) PURE;
    STDMETHOD(Discontinuity)(THIS) PURE;
    STDMETHOD(GetPacketPosition)(THIS_ LPXAUDIOPACKETPOSITION pPosition) PURE;
    STDMETHOD(SetPacketPosition)(THIS_ LPCXAUDIOPACKETPOSITION pPosition) PURE;
    STDMETHOD(GetPacketLoopCount)(THIS_ LPDWORD pLoopCount) PURE;
    STDMETHOD(SetPacketLoopCount)(THIS_ DWORD LoopCount) PURE;
    STDMETHOD(GetPacketContext)(THIS_ LPVOID *ppContext) PURE;
    STDMETHOD(GetStreamPosition)(THIS_ LPDWORD pPosition) PURE;
    STDMETHOD(GetVolume)(THIS_ LPXAUDIOVOLUME pVolume) PURE;
    STDMETHOD(SetVolume)(THIS_ XAUDIOVOLUME Volume) PURE;
    STDMETHOD(GetPitch)(THIS_ LPXAUDIOPITCH pPitch) PURE;
    STDMETHOD(SetPitch)(THIS_ XAUDIOPITCH Pitch) PURE;
    STDMETHOD(GetFormat)(THIS_ LPXAUDIOSOURCEFORMAT pFormat, LPXAUDIOSOURCEFLAGS pFlags) PURE;
    STDMETHOD(SetFormat)(THIS_ LPCXAUDIOSOURCEFORMAT pFormat, XAUDIOSOURCEFLAGS Flags) PURE;
};

/**************************************************************************
 *
 * IXAudioRouterEffect
 *
 **************************************************************************/

#undef INTERFACE
#define INTERFACE IXAudioRouterEffect

DECLARE_INTERFACE_(IXAudioRouterEffect, IXAudioEffect)
{
    // IXAudioRefCount
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    // IXAudioEffect
    STDMETHOD(GetInfo)(THIS_ LPXAUDIOFXINFO pInfo) PURE;
    STDMETHOD(GetParam)(THIS_ XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPXAUDIOFXPARAM pParam) PURE;
    STDMETHOD(SetParam)(THIS_ XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPCXAUDIOFXPARAM pParam) PURE;
    STDMETHOD(GetContext)(THIS_ LPVOID *ppContext) PURE;
    STDMETHOD(Process)(THIS_ IXAudioFrameBuffer *pInputBuffer, IXAudioFrameBuffer *pOutputBuffer) PURE;

    // IXAudioRouterEffect
    STDMETHOD(GetMode)(THIS_ LPXAUDIOROUTERFXMODE pMode) PURE;
    STDMETHOD(SetMode)(THIS_ XAUDIOROUTERFXMODE Mode) PURE;
    STDMETHOD(GetChannelMap)(THIS_ LPXAUDIOCHANNELMAP pChannelMap, DWORD BufferSize, LPDWORD pRequiredSize) PURE;
    STDMETHOD(SetChannelMap)(THIS_ LPCXAUDIOCHANNELMAP pChannelMap) PURE;
    STDMETHOD(GetChannelVolume)(THIS_ LPXAUDIOCHANNELVOLUME pVolume) PURE;
    STDMETHOD(SetChannelVolume)(THIS_ LPCXAUDIOCHANNELVOLUME pVolume) PURE;
};

/**************************************************************************
 *
 * IXAudioRenderEffect
 *
 **************************************************************************/

#undef INTERFACE
#define INTERFACE IXAudioRenderEffect

DECLARE_INTERFACE_(IXAudioRenderEffect, IXAudioEffect)
{
    // IXAudioRefCount
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    // IXAudioEffect
    STDMETHOD(GetInfo)(THIS_ LPXAUDIOFXINFO pInfo) PURE;
    STDMETHOD(GetParam)(THIS_ XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPXAUDIOFXPARAM pParam) PURE;
    STDMETHOD(SetParam)(THIS_ XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPCXAUDIOFXPARAM pParam) PURE;
    STDMETHOD(GetContext)(THIS_ LPVOID *ppContext) PURE;
    STDMETHOD(Process)(THIS_ IXAudioFrameBuffer *pInputBuffer, IXAudioFrameBuffer *pOutputBuffer) PURE;

    // IXAudioRenderEffect
    STDMETHOD(SetCallback)(THIS_ LPFNXAUDIORENDERFXCALLBACK pfnCallback) PURE;
};

/**************************************************************************
 *
 * IXAudioFrameBuffer
 *
 **************************************************************************/

STDAPI_(ULONG) XAudioFrameBuffer_AddRef(IXAudioFrameBuffer *pFrameBuffer);
STDAPI_(ULONG) XAudioFrameBuffer_Release(IXAudioFrameBuffer *pFrameBuffer);
STDAPI XAudioFrameBuffer_GetMaxFormat(IXAudioFrameBuffer *pFrameBuffer, LPXAUDIOFORMAT pFormat);
STDAPI XAudioFrameBuffer_GetProcessingData(IXAudioFrameBuffer *pFrameBuffer, LPXAUDIOFRAMEBUFDATA pProcessData);
STDAPI XAudioFrameBuffer_GetContext(IXAudioFrameBuffer *pFrameBuffer, LPVOID *ppContext);
STDAPI XAudioFrameBuffer_Prepare(IXAudioFrameBuffer *pFrameBuffer, LPCXAUDIOFORMAT pFormat, XAUDIOFRAMEBUFPREPARE Flags);
STDAPI XAudioFrameBuffer_Unprepare(IXAudioFrameBuffer *pFrameBuffer);
STDAPI XAudioFrameBuffer_SetExternalBuffer(IXAudioFrameBuffer *pFrameBuffer, LPCXAUDIOFORMAT pMaxFormat, LPXAUDIOSAMPLE pSampleBuffer);

#ifdef __cplusplus

struct IXAudioFrameBuffer
{
    __forceinline ULONG AddRef(void)
    {
        return XAudioFrameBuffer_AddRef(this);
    }

    __forceinline ULONG Release(void)
    {
        return XAudioFrameBuffer_Release(this);
    }

    __forceinline HRESULT GetMaxFormat(LPXAUDIOFORMAT pFormat)
    {
        return XAudioFrameBuffer_GetMaxFormat(this, pFormat);
    }

    __forceinline HRESULT GetProcessingData(LPXAUDIOFRAMEBUFDATA pProcessData)
    {
        return XAudioFrameBuffer_GetProcessingData(this, pProcessData);
    }

    __forceinline HRESULT GetContext(LPVOID *ppContext)
    {
        return XAudioFrameBuffer_GetContext(this, ppContext);
    }

    __forceinline HRESULT Prepare(LPCXAUDIOFORMAT pFormat, XAUDIOFRAMEBUFPREPARE Flags)
    {
        return XAudioFrameBuffer_Prepare(this, pFormat, Flags);
    }

    __forceinline HRESULT Unprepare(void)
    {
        return XAudioFrameBuffer_Unprepare(this);
    }

    __forceinline HRESULT SetExternalBuffer(LPCXAUDIOFORMAT pMaxFormat, LPXAUDIOSAMPLE pSampleBuffer)
    {
        return XAudioFrameBuffer_SetExternalBuffer(this, pMaxFormat, pSampleBuffer);
    }
};

#endif // __cplusplus

/**************************************************************************
 *
 * IXAudioEffectManager
 *
 **************************************************************************/

STDAPI_(ULONG) XAudioEffectManager_AddRef(IXAudioEffectManager *pEffectManager);
STDAPI_(ULONG) XAudioEffectManager_Release(IXAudioEffectManager *pEffectManager);
STDAPI XAudioEffectManager_QueryEffectSize(IXAudioEffectManager *pEffectManager, LPCXAUDIOFXINIT pInit, LPDWORD pEffectSize);
STDAPI XAudioEffectManager_CreateEffect(IXAudioEffectManager *pEffectManager, LPCXAUDIOFXINIT pInit, IXAudioBatchAllocator *pAllocator, IXAudioEffect **ppEffect);
STDAPI XAudioEffectManager_RegisterEffects(IXAudioEffectManager *pEffectManager, LPCXAUDIOFXREGISTER pEffects);
STDAPI XAudioEffectManager_UnregisterEffects(IXAudioEffectManager *pEffectManager, LPCXAUDIOFXUNREGISTER pEffects);

#ifdef __cplusplus

struct IXAudioEffectManager
{
    __forceinline ULONG AddRef(void)
    {
        return XAudioEffectManager_AddRef(this);
    }

    __forceinline ULONG Release(void)
    {
        return XAudioEffectManager_Release(this);
    }

    __forceinline HRESULT QueryEffectSize(LPCXAUDIOFXINIT pInit, LPDWORD pEffectSize)
    {
        return XAudioEffectManager_QueryEffectSize(this, pInit, pEffectSize);
    }

    __forceinline HRESULT CreateEffect(LPCXAUDIOFXINIT pInit, IXAudioBatchAllocator *pAllocator, IXAudioEffect **ppEffect)
    {
        return XAudioEffectManager_CreateEffect(this, pInit, pAllocator, ppEffect);
    }

    __forceinline HRESULT RegisterEffects(LPCXAUDIOFXREGISTER pEffects)
    {
        return XAudioEffectManager_RegisterEffects(this, pEffects);
    }

    __forceinline HRESULT UnregisterEffects(LPCXAUDIOFXUNREGISTER pEffects)
    {
        return XAudioEffectManager_UnregisterEffects(this, pEffects);
    }
};

#endif // __cplusplus

/**************************************************************************
 *
 * API
 *
 **************************************************************************/

STDAPI XAudioQueryFrameBufferSize(LPCXAUDIOFRAMEBUFINIT pInit, LPDWORD pObjectSize);
STDAPI XAudioCreateFrameBuffer(LPCXAUDIOFRAMEBUFINIT pInit, IXAudioBatchAllocator *pAllocator, IXAudioFrameBuffer **ppFrameBuffer);

STDAPI XAudioQueryEffectManagerSize(LPCXAUDIOFXMGRINIT pInit, LPDWORD pObjectSize);
STDAPI XAudioCreateEffectManager(LPCXAUDIOFXMGRINIT pInit, IXAudioBatchAllocator *pAllocator, IXAudioEffectManager **ppEffectManager);

STDAPI_(LPXAUDIOCHANNELMAP) XAudioGetDefaultChannelMap(XAUDIOCHANNEL InputChannelCount, XAUDIOCHANNEL OutputChannelCount, LPXAUDIOCHANNELMAP pChannelMap);

/**************************************************************************
 *
 * Effect creation routines
 *
 **************************************************************************/

STDAPI XAudioQuerySourceEffectSize(LPCXAUDIOFXINIT pInit, LPDWORD pObjectSize);
STDAPI XAudioCreateSourceEffect(LPCXAUDIOFXINIT pInit, IXAudioBatchAllocator *pAllocator, IXAudioEffect **ppEffect);

STDAPI XAudioQueryPCMSourceEffectSize(LPCXAUDIOFXINIT pInit, LPDWORD pObjectSize);
STDAPI XAudioCreatePCMSourceEffect(LPCXAUDIOFXINIT pInit, IXAudioBatchAllocator *pAllocator, IXAudioEffect **ppEffect);

STDAPI XAudioQueryXMASourceEffectSize(LPCXAUDIOFXINIT pInit, LPDWORD pObjectSize);
STDAPI XAudioCreateXMASourceEffect(LPCXAUDIOFXINIT pInit, IXAudioBatchAllocator *pAllocator, IXAudioEffect **ppEffect);

STDAPI XAudioQueryRouterEffectSize(LPCXAUDIOFXINIT pInit, LPDWORD pObjectSize);
STDAPI XAudioCreateRouterEffect(LPCXAUDIOFXINIT pInit, IXAudioBatchAllocator *pAllocator, IXAudioEffect **ppEffect);

STDAPI XAudioQueryNativeRenderEffectSize(LPCXAUDIOFXINIT pInit, LPDWORD pObjectSize);
STDAPI XAudioCreateNativeRenderEffect(LPCXAUDIOFXINIT pInit, IXAudioBatchAllocator *pAllocator, IXAudioEffect **ppEffect);

#define XAudioQueryRenderEffectSize XAudioQueryNativeRenderEffectSize
#define XAudioCreateRenderEffect    XAudioCreateNativeRenderEffect

STDAPI XAudioQueryReverbEffectSize(LPCXAUDIOFXINIT pInit, LPDWORD pObjectSize);
STDAPI XAudioCreateReverbEffect(LPCXAUDIOFXINIT pInit, IXAudioBatchAllocator *pAllocator, IXAudioEffect **ppEffect);

/**************************************************************************
 *
 * Global variables
 *
 **************************************************************************/

EXTERN_C const XAUDIOFXTABLE XAudioDefaultEffectTable;

/**************************************************************************
 *
 * Compatibility wrappers
 *
 **************************************************************************/

#ifdef __cplusplus

__forceinline ULONG XAudioEffect_AddRef(IXAudioEffect *pEffect)
{
    return pEffect->AddRef();
}

__forceinline ULONG XAudioEffect_Release(IXAudioEffect *pEffect)
{
    return pEffect->Release();
}

__forceinline HRESULT XAudioEffect_GetInfo(IXAudioEffect *pEffect, LPXAUDIOFXINFO pInfo)
{
    return pEffect->GetInfo(pInfo);
}

__forceinline HRESULT XAudioEffect_GetParam(IXAudioEffect *pEffect, XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPXAUDIOFXPARAM pParam)
{
    return pEffect->GetParam(ParamId, ParamType, pParam);
}

__forceinline HRESULT XAudioEffect_SetParam(IXAudioEffect *pEffect, XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPCXAUDIOFXPARAM pParam)
{
    return pEffect->SetParam(ParamId, ParamType, pParam);
}

__forceinline HRESULT XAudioEffect_GetContext(IXAudioEffect *pEffect, LPVOID *ppContext)
{
    return pEffect->GetContext(ppContext);
}

__forceinline HRESULT XAudioEffect_Process(IXAudioEffect *pEffect, IXAudioFrameBuffer *pInputBuffer, IXAudioFrameBuffer *pOutputBuffer)
{
    return pEffect->Process(pInputBuffer, pOutputBuffer);
}

__forceinline ULONG XAudioSourceEffect_AddRef(IXAudioSourceEffect *pSourceEffect)
{
    return pSourceEffect->AddRef();
}

__forceinline ULONG XAudioSourceEffect_Release(IXAudioSourceEffect *pSourceEffect)
{
    return pSourceEffect->Release();
}

__forceinline HRESULT XAudioSourceEffect_GetInfo(IXAudioSourceEffect *pSourceEffect, LPXAUDIOFXINFO pInfo)
{
    return pSourceEffect->GetInfo(pInfo);
}

__forceinline HRESULT XAudioSourceEffect_GetParam(IXAudioSourceEffect *pSourceEffect, XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPXAUDIOFXPARAM pParam)
{
    return pSourceEffect->GetParam(ParamId, ParamType, pParam);
}

__forceinline HRESULT XAudioSourceEffect_SetParam(IXAudioSourceEffect *pSourceEffect, XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPCXAUDIOFXPARAM pParam)
{
    return pSourceEffect->SetParam(ParamId, ParamType, pParam);
}

__forceinline HRESULT XAudioSourceEffect_GetContext(IXAudioSourceEffect *pSourceEffect, LPVOID *ppContext)
{
    return pSourceEffect->GetContext(ppContext);
}

__forceinline HRESULT XAudioSourceEffect_Process(IXAudioSourceEffect *pSourceEffect, IXAudioFrameBuffer *pInputBuffer, IXAudioFrameBuffer *pOutputBuffer)
{
    return pSourceEffect->Process(pInputBuffer, pOutputBuffer);
}

__forceinline HRESULT XAudioSourceEffect_SubmitPacket(IXAudioSourceEffect *pEffect, LPCXAUDIOPACKET pPacket, XAUDIOSUBMITPACKET Flags)
{
    return pEffect->SubmitPacket(pPacket, Flags);
}

__forceinline HRESULT XAudioSourceEffect_FlushPackets(IXAudioSourceEffect *pEffect)
{
    return pEffect->FlushPackets();
}

__forceinline HRESULT XAudioSourceEffect_GetState(IXAudioSourceEffect *pEffect, LPXAUDIOSOURCESTATE pState)
{
    return pEffect->GetState(pState);
}

__forceinline HRESULT XAudioSourceEffect_Start(IXAudioSourceEffect *pEffect)
{
    return pEffect->Start();
}

__forceinline HRESULT XAudioSourceEffect_Stop(IXAudioSourceEffect *pEffect, XAUDIOSTOP Flags)
{
    return pEffect->Stop(Flags);
}

__forceinline HRESULT XAudioSourceEffect_Discontinuity(IXAudioSourceEffect *pEffect)
{
    return pEffect->Discontinuity();
}

__forceinline HRESULT XAudioSourceEffect_GetPacketPosition(IXAudioSourceEffect *pEffect, LPXAUDIOPACKETPOSITION pPosition)
{
    return pEffect->GetPacketPosition(pPosition);
}

__forceinline HRESULT XAudioSourceEffect_SetPacketPosition(IXAudioSourceEffect *pEffect, LPCXAUDIOPACKETPOSITION pPosition)
{
    return pEffect->SetPacketPosition(pPosition);
}

__forceinline HRESULT XAudioSourceEffect_GetPacketLoopCount(IXAudioSourceEffect *pEffect, LPDWORD pLoopCount)
{
    return pEffect->GetPacketLoopCount(pLoopCount);
}

__forceinline HRESULT XAudioSourceEffect_SetPacketLoopCount(IXAudioSourceEffect *pEffect, DWORD LoopCount)
{
    return pEffect->SetPacketLoopCount(LoopCount);
}

__forceinline HRESULT XAudioSourceEffect_GetPacketContext(IXAudioSourceEffect *pEffect, LPVOID *ppContext)
{
    return pEffect->GetPacketContext(ppContext);
}

__forceinline HRESULT XAudioSourceEffect_GetVolume(IXAudioSourceEffect *pEffect, LPXAUDIOVOLUME pVolume)
{
    return pEffect->GetVolume(pVolume);
}

__forceinline HRESULT XAudioSourceEffect_SetVolume(IXAudioSourceEffect *pEffect, XAUDIOVOLUME Volume)
{
    return pEffect->SetVolume(Volume);
}

__forceinline HRESULT XAudioSourceEffect_GetPitch(IXAudioSourceEffect *pEffect, LPXAUDIOPITCH pPitch)
{
    return pEffect->GetPitch(pPitch);
}

__forceinline HRESULT XAudioSourceEffect_SetPitch(IXAudioSourceEffect *pEffect, XAUDIOPITCH Pitch)
{
    return pEffect->SetPitch(Pitch);
}

__forceinline HRESULT XAudioSourceEffect_GetFormat(IXAudioSourceEffect *pEffect, LPXAUDIOSOURCEFORMAT pFormat, LPXAUDIOSOURCEFLAGS pFlags)
{
    return pEffect->GetFormat(pFormat, pFlags);
}

__forceinline HRESULT XAudioSourceEffect_SetFormat(IXAudioSourceEffect *pEffect, LPCXAUDIOSOURCEFORMAT pFormat, XAUDIOSOURCEFLAGS Flags)
{
    return pEffect->SetFormat(pFormat, Flags);
}

__forceinline ULONG XAudioRouterEffect_AddRef(IXAudioRouterEffect *pRouterEffect)
{
    return pRouterEffect->AddRef();
}

__forceinline ULONG XAudioRouterEffect_Release(IXAudioRouterEffect *pRouterEffect)
{
    return pRouterEffect->Release();
}

__forceinline HRESULT XAudioRouterEffect_GetInfo(IXAudioRouterEffect *pRouterEffect, LPXAUDIOFXINFO pInfo)
{
    return pRouterEffect->GetInfo(pInfo);
}

__forceinline HRESULT XAudioRouterEffect_GetParam(IXAudioRouterEffect *pRouterEffect, XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPXAUDIOFXPARAM pParam)
{
    return pRouterEffect->GetParam(ParamId, ParamType, pParam);
}

__forceinline HRESULT XAudioRouterEffect_SetParam(IXAudioRouterEffect *pRouterEffect, XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPCXAUDIOFXPARAM pParam)
{
    return pRouterEffect->SetParam(ParamId, ParamType, pParam);
}

__forceinline HRESULT XAudioRouterEffect_GetContext(IXAudioRouterEffect *pRouterEffect, LPVOID *ppContext)
{
    return pRouterEffect->GetContext(ppContext);
}

__forceinline HRESULT XAudioRouterEffect_Process(IXAudioRouterEffect *pRouterEffect, IXAudioFrameBuffer *pInputBuffer, IXAudioFrameBuffer *pOutputBuffer)
{
    return pRouterEffect->Process(pInputBuffer, pOutputBuffer);
}

__forceinline HRESULT XAudioRouterEffect_GetChannelMap(IXAudioRouterEffect *pEffect, LPXAUDIOCHANNELMAP pChannelMap, DWORD BufferSize, LPDWORD pRequiredSize)
{
    return pEffect->GetChannelMap(pChannelMap, BufferSize, pRequiredSize);
}

__forceinline HRESULT XAudioRouterEffect_SetChannelMap(IXAudioRouterEffect *pEffect, LPCXAUDIOCHANNELMAP pChannelMap)
{
    return pEffect->SetChannelMap(pChannelMap);
}

__forceinline HRESULT XAudioRouterEffect_GetChannelVolume(IXAudioRouterEffect *pEffect, LPXAUDIOCHANNELVOLUME pVolume)
{
    return pEffect->GetChannelVolume(pVolume);
}

__forceinline HRESULT XAudioRouterEffect_SetChannelVolume(IXAudioRouterEffect *pEffect, LPCXAUDIOCHANNELVOLUME pVolume)
{
    return pEffect->SetChannelVolume(pVolume);
}

__forceinline ULONG XAudioRenderEffect_AddRef(IXAudioRenderEffect *pRenderEffect)
{
    return pRenderEffect->AddRef();
}

__forceinline ULONG XAudioRenderEffect_Release(IXAudioRenderEffect *pRenderEffect)
{
    return pRenderEffect->Release();
}

__forceinline HRESULT XAudioRenderEffect_GetInfo(IXAudioRenderEffect *pRenderEffect, LPXAUDIOFXINFO pInfo)
{
    return pRenderEffect->GetInfo(pInfo);
}

__forceinline HRESULT XAudioRenderEffect_GetParam(IXAudioRenderEffect *pRenderEffect, XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPXAUDIOFXPARAM pParam)
{
    return pRenderEffect->GetParam(ParamId, ParamType, pParam);
}

__forceinline HRESULT XAudioRenderEffect_SetParam(IXAudioRenderEffect *pRenderEffect, XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPCXAUDIOFXPARAM pParam)
{
    return pRenderEffect->SetParam(ParamId, ParamType, pParam);
}

__forceinline HRESULT XAudioRenderEffect_GetContext(IXAudioRenderEffect *pRenderEffect, LPVOID *ppContext)
{
    return pRenderEffect->GetContext(ppContext);
}

__forceinline HRESULT XAudioRenderEffect_Process(IXAudioRenderEffect *pRenderEffect, IXAudioFrameBuffer *pInputBuffer, IXAudioFrameBuffer *pOutputBuffer)
{
    return pRenderEffect->Process(pInputBuffer, pOutputBuffer);
}

__forceinline HRESULT XAudioRenderEffect_SetCallback(IXAudioRenderEffect *pEffect, LPFNXAUDIORENDERFXCALLBACK pfnCallback)
{
    return pEffect->SetCallback(pfnCallback);
}

#else // __cplusplus

__forceinline ULONG XAudioEffect_AddRef(IXAudioEffect *pEffect)
{
    return pEffect->lpVtbl->AddRef(pEffect);
}

__forceinline ULONG XAudioEffect_Release(IXAudioEffect *pEffect)
{
    return pEffect->lpVtbl->Release(pEffect);
}

__forceinline HRESULT XAudioEffect_GetInfo(IXAudioEffect *pEffect, LPXAUDIOFXINFO pInfo)
{
    return pEffect->lpVtbl->GetInfo(pEffect, pInfo);
}

__forceinline HRESULT XAudioEffect_GetParam(IXAudioEffect *pEffect, XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPXAUDIOFXPARAM pParam)
{
    return pEffect->lpVtbl->GetParam(pEffect, ParamId, ParamType, pParam);
}

__forceinline HRESULT XAudioEffect_SetParam(IXAudioEffect *pEffect, XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPCXAUDIOFXPARAM pParam)
{
    return pEffect->lpVtbl->SetParam(pEffect, ParamId, ParamType, pParam);
}

__forceinline HRESULT XAudioEffect_GetContext(IXAudioEffect *pEffect, LPVOID *ppContext)
{
    return pEffect->lpVtbl->GetContext(pEffect, ppContext);
}

__forceinline HRESULT XAudioEffect_Process(IXAudioEffect *pEffect, IXAudioFrameBuffer *pInputBuffer, IXAudioFrameBuffer *pOutputBuffer)
{
    return pEffect->lpVtbl->Process(pEffect, pInputBuffer, pOutputBuffer);
}

__forceinline ULONG XAudioSourceEffect_AddRef(IXAudioSourceEffect *pSourceEffect)
{
    return pSourceEffect->lpVtbl->AddRef(pSourceEffect);
}

__forceinline ULONG XAudioSourceEffect_Release(IXAudioSourceEffect *pSourceEffect)
{
    return pSourceEffect->lpVtbl->Release(pSourceEffect);
}

__forceinline HRESULT XAudioSourceEffect_GetInfo(IXAudioSourceEffect *pSourceEffect, LPXAUDIOFXINFO pInfo)
{
    return pSourceEffect->lpVtbl->GetInfo(pSourceEffect, pInfo);
}

__forceinline HRESULT XAudioSourceEffect_GetParam(IXAudioSourceEffect *pSourceEffect, XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPXAUDIOFXPARAM pParam)
{
    return pSourceEffect->lpVtbl->GetParam(pSourceEffect, ParamId, ParamType, pParam);
}

__forceinline HRESULT XAudioSourceEffect_SetParam(IXAudioSourceEffect *pSourceEffect, XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPCXAUDIOFXPARAM pParam)
{
    return pSourceEffect->lpVtbl->SetParam(pSourceEffect, ParamId, ParamType, pParam);
}

__forceinline HRESULT XAudioSourceEffect_GetContext(IXAudioSourceEffect *pSourceEffect, LPVOID *ppContext)
{
    return pSourceEffect->lpVtbl->GetContext(pSourceEffect, ppContext);
}

__forceinline HRESULT XAudioSourceEffect_Process(IXAudioSourceEffect *pSourceEffect, IXAudioFrameBuffer *pInputBuffer, IXAudioFrameBuffer *pOutputBuffer)
{
    return pSourceEffect->lpVtbl->Process(pSourceEffect, pInputBuffer, pOutputBuffer);
}

__forceinline HRESULT XAudioSourceEffect_SubmitPacket(IXAudioSourceEffect *pEffect, LPCXAUDIOPACKET pPacket, XAUDIOSUBMITPACKET Flags)
{
    return pEffect->lpVtbl->SubmitPacket(pEffect, pPacket, Flags);
}

__forceinline HRESULT XAudioSourceEffect_FlushPackets(IXAudioSourceEffect *pEffect)
{
    return pEffect->lpVtbl->FlushPackets(pEffect);
}

__forceinline HRESULT XAudioSourceEffect_GetState(IXAudioSourceEffect *pEffect, LPXAUDIOSOURCESTATE pState)
{
    return pEffect->lpVtbl->GetState(pEffect, pState);
}

__forceinline HRESULT XAudioSourceEffect_Start(IXAudioSourceEffect *pEffect)
{
    return pEffect->lpVtbl->Start(pEffect);
}

__forceinline HRESULT XAudioSourceEffect_Stop(IXAudioSourceEffect *pEffect, XAUDIOSTOP Flags)
{
    return pEffect->lpVtbl->Stop(pEffect, Flags);
}

__forceinline HRESULT XAudioSourceEffect_Discontinuity(IXAudioSourceEffect *pEffect)
{
    return pEffect->lpVtbl->Discontinuity(pEffect);
}

__forceinline HRESULT XAudioSourceEffect_GetPacketPosition(IXAudioSourceEffect *pEffect, LPXAUDIOPACKETPOSITION pPosition)
{
    return pEffect->lpVtbl->GetPacketPosition(pEffect, pPosition);
}

__forceinline HRESULT XAudioSourceEffect_SetPacketPosition(IXAudioSourceEffect *pEffect, LPCXAUDIOPACKETPOSITION Position)
{
    return pEffect->lpVtbl->SetPacketPosition(pEffect, Position);
}

__forceinline HRESULT XAudioSourceEffect_GetPacketLoopCount(IXAudioSourceEffect *pEffect, LPDWORD pLoopCount)
{
    return pEffect->lpVtbl->GetPacketLoopCount(pEffect, pLoopCount);
}

__forceinline HRESULT XAudioSourceEffect_SetPacketLoopCount(IXAudioSourceEffect *pEffect, DWORD LoopCount)
{
    return pEffect->lpVtbl->SetPacketLoopCount(pEffect, LoopCount);
}

__forceinline HRESULT XAudioSourceEffect_GetPacketContext(IXAudioSourceEffect *pEffect, LPVOID *ppContext)
{
    return pEffect->lpVtbl->GetPacketContext(pEffect, ppContext);
}

__forceinline HRESULT XAudioSourceEffect_GetVolume(IXAudioSourceEffect *pEffect, LPXAUDIOVOLUME pVolume)
{
    return pEffect->lpVtbl->GetVolume(pEffect, pVolume);
}

__forceinline HRESULT XAudioSourceEffect_SetVolume(IXAudioSourceEffect *pEffect, XAUDIOVOLUME Volume)
{
    return pEffect->lpVtbl->SetVolume(pEffect, Volume);
}

__forceinline HRESULT XAudioSourceEffect_GetPitch(IXAudioSourceEffect *pEffect, LPXAUDIOPITCH pPitch)
{
    return pEffect->lpVtbl->GetPitch(pEffect, pPitch);
}

__forceinline HRESULT XAudioSourceEffect_SetPitch(IXAudioSourceEffect *pEffect, XAUDIOPITCH Pitch)
{
    return pEffect->lpVtbl->SetPitch(pEffect, Pitch);
}

__forceinline HRESULT XAudioSourceEffect_GetFormat(IXAudioSourceEffect *pEffect, LPXAUDIOSOURCEFORMAT pFormat, LPXAUDIOSOURCEFLAGS pFlags)
{
    return pEffect->lpVtbl->GetFormat(pEffect, pFormat, pFlags);
}

__forceinline HRESULT XAudioSourceEffect_SetFormat(IXAudioSourceEffect *pEffect, LPCXAUDIOSOURCEFORMAT pFormat, XAUDIOSOURCEFLAGS Flags)
{
    return pEffect->lpVtbl->SetFormat(pEffect, pFormat, Flags);
}

__forceinline ULONG XAudioRouterEffect_AddRef(IXAudioRouterEffect *pRouterEffect)
{
    return pRouterEffect->lpVtbl->AddRef(pRouterEffect);
}

__forceinline ULONG XAudioRouterEffect_Release(IXAudioRouterEffect *pRouterEffect)
{
    return pRouterEffect->lpVtbl->Release(pRouterEffect);
}

__forceinline HRESULT XAudioRouterEffect_GetInfo(IXAudioRouterEffect *pRouterEffect, LPXAUDIOFXINFO pInfo)
{
    return pRouterEffect->lpVtbl->GetInfo(pRouterEffect, pInfo);
}

__forceinline HRESULT XAudioRouterEffect_GetParam(IXAudioRouterEffect *pRouterEffect, XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPXAUDIOFXPARAM pParam)
{
    return pRouterEffect->lpVtbl->GetParam(pRouterEffect, ParamId, ParamType, pParam);
}

__forceinline HRESULT XAudioRouterEffect_SetParam(IXAudioRouterEffect *pRouterEffect, XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPCXAUDIOFXPARAM pParam)
{
    return pRouterEffect->lpVtbl->SetParam(pRouterEffect, ParamId, ParamType, pParam);
}

__forceinline HRESULT XAudioRouterEffect_GetContext(IXAudioRouterEffect *pRouterEffect, LPVOID *ppContext)
{
    return pRouterEffect->lpVtbl->GetContext(pRouterEffect, ppContext);
}

__forceinline HRESULT XAudioRouterEffect_Process(IXAudioRouterEffect *pRouterEffect, IXAudioFrameBuffer *pInputBuffer, IXAudioFrameBuffer *pOutputBuffer)
{
    return pRouterEffect->lpVtbl->Process(pRouterEffect, pInputBuffer, pOutputBuffer);
}

__forceinline HRESULT XAudioRouterEffect_GetChannelMap(IXAudioRouterEffect *pEffect, LPXAUDIOCHANNELMAP pChannelMap, DWORD BufferSize, LPDWORD pRequiredSize)
{
    return pEffect->lpVtbl->GetChannelMap(pEffect, pChannelMap, BufferSize, pRequiredSize);
}

__forceinline HRESULT XAudioRouterEffect_SetChannelMap(IXAudioRouterEffect *pEffect, LPCXAUDIOCHANNELMAP pChannelMap)
{
    return pEffect->lpVtbl->SetChannelMap(pEffect, pChannelMap);
}

__forceinline HRESULT XAudioRouterEffect_GetChannelVolume(IXAudioRouterEffect *pEffect, LPXAUDIOCHANNELVOLUME pVolume)
{
    return pEffect->lpVtbl->GetChannelVolume(pEffect, pVolume);
}

__forceinline HRESULT XAudioRouterEffect_SetChannelVolume(IXAudioRouterEffect *pEffect, LPCXAUDIOCHANNELVOLUME pVolume)
{
    return pEffect->lpVtbl->SetChannelVolume(pEffect, pVolume);
}

__forceinline ULONG XAudioRenderEffect_AddRef(IXAudioRenderEffect *pRenderEffect)
{
    return pRenderEffect->lpVtbl->AddRef(pRenderEffect);
}

__forceinline ULONG XAudioRenderEffect_Release(IXAudioRenderEffect *pRenderEffect)
{
    return pRenderEffect->lpVtbl->Release(pRenderEffect);
}

__forceinline HRESULT XAudioRenderEffect_GetInfo(IXAudioRenderEffect *pRenderEffect, LPXAUDIOFXINFO pInfo)
{
    return pRenderEffect->lpVtbl->GetInfo(pRenderEffect, pInfo);
}

__forceinline HRESULT XAudioRenderEffect_GetParam(IXAudioRenderEffect *pRenderEffect, XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPXAUDIOFXPARAM pParam)
{
    return pRenderEffect->lpVtbl->GetParam(pRenderEffect, ParamId, ParamType, pParam);
}

__forceinline HRESULT XAudioRenderEffect_SetParam(IXAudioRenderEffect *pRenderEffect, XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPCXAUDIOFXPARAM pParam)
{
    return pRenderEffect->lpVtbl->SetParam(pRenderEffect, ParamId, ParamType, pParam);
}

__forceinline HRESULT XAudioRenderEffect_GetContext(IXAudioRenderEffect *pRenderEffect, LPVOID *ppContext)
{
    return pRenderEffect->lpVtbl->GetContext(pRenderEffect, ppContext);
}

__forceinline HRESULT XAudioRenderEffect_Process(IXAudioRenderEffect *pRenderEffect, IXAudioFrameBuffer *pInputBuffer, IXAudioFrameBuffer *pOutputBuffer)
{
    return pRenderEffect->lpVtbl->Process(pRenderEffect, pInputBuffer, pOutputBuffer);
}

__forceinline HRESULT XAudioRenderEffect_SetCallback(IXAudioRenderEffect *pEffect, LPFNXAUDIORENDERFXCALLBACK pfnCallback)
{
    return pEffect->lpVtbl->SetCallback(pEffect, pfnCallback);
}

#endif // __cplusplus

#pragma pack(pop)
#pragma warning(pop)

#endif // __XAUDFX_INCLUDED__

