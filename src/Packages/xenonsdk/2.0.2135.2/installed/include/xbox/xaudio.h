/**************************************************************************
 *
 *  Copyright (C) 2003 - 2005 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       xaudio.h
 *  Content:    Xbox audio API.
 *
 **************************************************************************/

#ifndef __XAUDIO_INCLUDED__
#define __XAUDIO_INCLUDED__

#include <xaudfx.h>

#pragma warning(push)
#pragma warning(disable:4201)

#pragma pack(push, 4)

/**************************************************************************
 *
 * API
 *
 **************************************************************************/

STDAPI XAudioInitialize(LPCXAUDIOENGINEINIT pInit);
STDAPI_(ULONG) XAudioShutDown(void);
STDAPI XAudioCreateSourceVoice(LPCXAUDIOSOURCEVOICEINIT pInit, IXAudioSourceVoice **ppVoice);
STDAPI XAudioCreateSubmixVoice(LPCXAUDIOSUBMIXVOICEINIT pInit, IXAudioSubmixVoice **ppVoice);
STDAPI XAudioGetMasteringVoice(IXAudioMasteringVoice **ppVoice);
STDAPI XAudioRegisterEngineCallback(XAUDIOENGINECALLBACKTYPE CallbackType, LPFNXAUDIOENGINECALLBACK pfnCallback, LPVOID pContext, LPXAUDIOCALLBACKHANDLE phCallbackHandle);
STDAPI XAudioUnregisterEngineCallback(XAUDIOCALLBACKHANDLE hCallbackHandle);
STDAPI XAudioRegisterEffects(LPCXAUDIOFXREGISTER pEffects);
STDAPI XAudioUnregisterEffects(LPCXAUDIOFXUNREGISTER pEffects);
STDAPI XAudioSynchStart(void);

/**************************************************************************
 *
 * XAudioVoice
 *
 **************************************************************************/

STDAPI_(ULONG) XAudioVoice_AddRef(IXAudioVoice *pVoice);
STDAPI_(ULONG) XAudioVoice_Release(IXAudioVoice *pVoice);
STDAPI XAudioVoice_GetEffectState(IXAudioVoice *pVoice, XAUDIOVOICEFXINDEX EffectIndex, LPXAUDIOVOICEFXSTATE pEffectState);
STDAPI XAudioVoice_SetEffectState(IXAudioVoice *pVoice, XAUDIOVOICEFXINDEX EffectIndex, XAUDIOVOICEFXSTATE EffectState);
STDAPI XAudioVoice_GetEffectParam(IXAudioVoice *pVoice, XAUDIOVOICEFXINDEX EffectIndex, XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPXAUDIOFXPARAM pParam);
STDAPI XAudioVoice_SetEffectParam(IXAudioVoice *pVoice, XAUDIOVOICEFXINDEX EffectIndex, XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPCXAUDIOFXPARAM pParam);
STDAPI XAudioVoice_SetVoiceOutput(IXAudioVoice *pVoice, LPCXAUDIOVOICEOUTPUT pVoiceOutput);
STDAPI XAudioVoice_SetVoiceOutputVolume(IXAudioVoice *pVoice, LPCXAUDIOVOICEOUTPUTVOLUME pVolume);
STDAPI XAudioVoice_GetVoiceContext(IXAudioVoice *pVoice, LPVOID *ppContext);
STDAPI XAudioVoice_GetVoiceState(IXAudioVoice *pVoice, LPXAUDIOSOURCESTATE pState);
STDAPI XAudioVoice_GetVoiceType(IXAudioVoice *pVoice, LPXAUDIOVOICETYPE pType);

#ifdef __cplusplus

struct __declspec(novtable) IXAudioVoice
{
    __forceinline ULONG AddRef(void)
    {
        return XAudioVoice_AddRef(this);
    }

    __forceinline ULONG Release(void)
    {
        return XAudioVoice_Release(this);
    }

    __forceinline HRESULT GetEffectState(XAUDIOVOICEFXINDEX EffectIndex, LPXAUDIOVOICEFXSTATE pEffectState)
    {
        return XAudioVoice_GetEffectState(this, EffectIndex, pEffectState);
    }

    __forceinline HRESULT SetEffectState(XAUDIOVOICEFXINDEX EffectIndex, XAUDIOVOICEFXSTATE EffectState)
    {
        return XAudioVoice_SetEffectState(this, EffectIndex, EffectState);
    }

    __forceinline HRESULT GetEffectParam(XAUDIOVOICEFXINDEX EffectIndex, XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPXAUDIOFXPARAM pParam)
    {
        return XAudioVoice_GetEffectParam(this, EffectIndex, ParamId, ParamType, pParam);
    }

    __forceinline HRESULT SetEffectParam(XAUDIOVOICEFXINDEX EffectIndex, XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPCXAUDIOFXPARAM pParam)
    {
        return XAudioVoice_SetEffectParam(this, EffectIndex, ParamId, ParamType, pParam);
    }

    __forceinline HRESULT SetVoiceOutput(LPCXAUDIOVOICEOUTPUT pVoiceOutput)
    {
        return XAudioVoice_SetVoiceOutput(this, pVoiceOutput);
    }

    __forceinline HRESULT SetVoiceOutputVolume(LPCXAUDIOVOICEOUTPUTVOLUME pVolume)
    {
        return XAudioVoice_SetVoiceOutputVolume(this, pVolume);
    }

    __forceinline HRESULT GetVoiceContext(LPVOID *ppContext)
    {
        return XAudioVoice_GetVoiceContext(this, ppContext);
    }

    __forceinline HRESULT GetVoiceState(LPXAUDIOSOURCESTATE pState)
    {
        return XAudioVoice_GetVoiceState(this, pState);
    }

    __forceinline HRESULT GetVoiceType(LPXAUDIOVOICETYPE pType)
    {
        return XAudioVoice_GetVoiceType(this, pType);
    }
};

#endif // __cplusplus

/**************************************************************************
 *
 * XAudioSourceVoice
 *
 **************************************************************************/

#define XAudioSourceVoice_AddRef                XAudioVoice_AddRef
#define XAudioSourceVoice_Release               XAudioVoice_Release
#define XAudioSourceVoice_GetEffectState        XAudioVoice_GetEffectState
#define XAudioSourceVoice_SetEffectState        XAudioVoice_SetEffectState
#define XAudioSourceVoice_GetEffectParam        XAudioVoice_GetEffectParam
#define XAudioSourceVoice_SetEffectParam        XAudioVoice_SetEffectParam
#define XAudioSourceVoice_SetVoiceOutput        XAudioVoice_SetVoiceOutput
#define XAudioSourceVoice_SetVoiceOutputVolume  XAudioVoice_SetVoiceOutputVolume
#define XAudioSourceVoice_GetVoiceContext       XAudioVoice_GetVoiceContext
#define XAudioSourceVoice_GetVoiceState         XAudioVoice_GetVoiceState
#define XAudioSourceVoice_GetVoiceType          XAudioVoice_GetVoiceType

STDAPI XAudioSourceVoice_GetVoiceFormat(IXAudioSourceVoice *pVoice, LPXAUDIOSOURCEFORMAT pFormat);
STDAPI XAudioSourceVoice_SetVoiceFormat(IXAudioSourceVoice *pVoice, LPCXAUDIOSOURCEFORMAT pFormat, LPCXAUDIOVOICEOUTPUT pVoiceOutput, XAUDIOSOURCEFLAGS Flags);
STDAPI XAudioSourceVoice_SubmitPacket(IXAudioSourceVoice *pVoice, LPCXAUDIOPACKET pPacket, XAUDIOSUBMITPACKET Flags);
STDAPI XAudioSourceVoice_FlushPackets(IXAudioSourceVoice *pVoice);
STDAPI XAudioSourceVoice_Start(IXAudioSourceVoice *pVoice, XAUDIOSTART Flags);
STDAPI XAudioSourceVoice_Stop(IXAudioSourceVoice *pVoice, XAUDIOSTOP Flags);
STDAPI XAudioSourceVoice_Discontinuity(IXAudioSourceVoice *pVoice);
STDAPI XAudioSourceVoice_GetPacketPosition(IXAudioSourceVoice *pVoice, LPXAUDIOPACKETPOSITION pPosition);
STDAPI XAudioSourceVoice_SetPacketPosition(IXAudioSourceVoice *pVoice, LPCXAUDIOPACKETPOSITION pPosition);
STDAPI XAudioSourceVoice_GetStreamPosition(IXAudioSourceVoice *pVoice, LPDWORD pPosition);
STDAPI XAudioSourceVoice_GetPacketLoopCount(IXAudioSourceVoice *pVoice, LPDWORD pLoopCount);
STDAPI XAudioSourceVoice_SetPacketLoopCount(IXAudioSourceVoice *pVoice, DWORD LoopCount);
STDAPI XAudioSourceVoice_GetPacketContext(IXAudioSourceVoice *pVoice, LPVOID *ppContext);
STDAPI XAudioSourceVoice_GetVolume(IXAudioSourceVoice *pVoice, LPXAUDIOVOLUME pVolume);
STDAPI XAudioSourceVoice_SetVolume(IXAudioSourceVoice *pVoice, XAUDIOVOLUME Volume);
STDAPI XAudioSourceVoice_GetPitch(IXAudioSourceVoice *pVoice, LPXAUDIOPITCH pPitch);
STDAPI XAudioSourceVoice_SetPitch(IXAudioSourceVoice *pVoice, XAUDIOPITCH Pitch);

#ifdef __cplusplus

struct __declspec(novtable) IXAudioSourceVoice : public IXAudioVoice
{
    __forceinline HRESULT GetVoiceFormat(LPXAUDIOSOURCEFORMAT pFormat)
    {
        return XAudioSourceVoice_GetVoiceFormat(this, pFormat);
    }

    __forceinline HRESULT SetVoiceFormat(LPCXAUDIOSOURCEFORMAT pFormat, LPCXAUDIOVOICEOUTPUT pVoiceOutput, XAUDIOSOURCEFLAGS Flags)
    {
        return XAudioSourceVoice_SetVoiceFormat(this, pFormat, pVoiceOutput, Flags);
    }

    __forceinline HRESULT SubmitPacket(LPCXAUDIOPACKET pPacket, XAUDIOSUBMITPACKET Flags)
    {
        return XAudioSourceVoice_SubmitPacket(this, pPacket, Flags);
    }

    __forceinline HRESULT FlushPackets(void)
    {
        return XAudioSourceVoice_FlushPackets(this);
    }

    __forceinline HRESULT Start(XAUDIOSTART Flags)
    {
        return XAudioSourceVoice_Start(this, Flags);
    }

    __forceinline HRESULT Stop(XAUDIOSTOP Flags)
    {
        return XAudioSourceVoice_Stop(this, Flags);
    }

    __forceinline HRESULT Discontinuity(void)
    {
        return XAudioSourceVoice_Discontinuity(this);
    }

    __forceinline HRESULT GetPacketPosition(LPXAUDIOPACKETPOSITION pPosition)
    {
        return XAudioSourceVoice_GetPacketPosition(this, pPosition);
    }

    __forceinline HRESULT SetPacketPosition(LPCXAUDIOPACKETPOSITION pPosition)
    {
        return XAudioSourceVoice_SetPacketPosition(this, pPosition);
    }

    __forceinline HRESULT GetStreamPosition(LPDWORD pPosition)
    {
        return XAudioSourceVoice_GetStreamPosition(this, pPosition);
    }

    __forceinline HRESULT GetPacketLoopCount(LPDWORD pLoopCount)
    {
        return XAudioSourceVoice_GetPacketLoopCount(this, pLoopCount);
    }

    __forceinline HRESULT SetPacketLoopCount(DWORD LoopCount)
    {
        return XAudioSourceVoice_SetPacketLoopCount(this, LoopCount);
    }

    __forceinline HRESULT GetPacketContext(LPVOID *ppContext)
    {
        return XAudioSourceVoice_GetPacketContext(this, ppContext);
    }

    __forceinline HRESULT GetVolume(LPXAUDIOVOLUME pVolume)
    {
        return XAudioSourceVoice_GetVolume(this, pVolume);
    }

    __forceinline HRESULT SetVolume(XAUDIOVOLUME Volume)
    {
        return XAudioSourceVoice_SetVolume(this, Volume);
    }

    __forceinline HRESULT GetPitch(LPXAUDIOPITCH pPitch)
    {
        return XAudioSourceVoice_GetPitch(this, pPitch);
    }

    __forceinline HRESULT SetPitch(XAUDIOPITCH Pitch)
    {
        return XAudioSourceVoice_SetPitch(this, Pitch);
    }
};

#endif // __cplusplus

/**************************************************************************
 *
 * XAudioSubmixVoice
 *
 **************************************************************************/

#define XAudioSubmixVoice_AddRef                XAudioVoice_AddRef
#define XAudioSubmixVoice_Release               XAudioVoice_Release
#define XAudioSubmixVoice_GetEffectState        XAudioVoice_GetEffectState
#define XAudioSubmixVoice_SetEffectState        XAudioVoice_SetEffectState
#define XAudioSubmixVoice_GetEffectParam        XAudioVoice_GetEffectParam
#define XAudioSubmixVoice_SetEffectParam        XAudioVoice_SetEffectParam
#define XAudioSubmixVoice_SetVoiceOutput        XAudioVoice_SetVoiceOutput
#define XAudioSubmixVoice_SetVoiceOutputVolume  XAudioVoice_SetVoiceOutputVolume
#define XAudioSubmixVoice_GetVoiceContext       XAudioVoice_GetVoiceContext
#define XAudioSubmixVoice_GetVoiceState         XAudioVoice_GetVoiceState
#define XAudioSubmixVoice_GetVoiceType          XAudioVoice_GetVoiceType

STDAPI XAudioSubmixVoice_GetVoiceFormat(IXAudioSubmixVoice *pVoice, LPXAUDIOFORMAT pFormat);
STDAPI XAudioSubmixVoice_SetVoiceFormat(IXAudioSubmixVoice *pVoice, LPCXAUDIOFORMAT pFormat, LPCXAUDIOVOICEOUTPUT pVoiceOutput);

#ifdef __cplusplus

struct __declspec(novtable) IXAudioSubmixVoice : public IXAudioVoice
{
    __forceinline HRESULT GetVoiceFormat(LPXAUDIOFORMAT pFormat)
    {
        return XAudioSubmixVoice_GetVoiceFormat(this, pFormat);
    }

    __forceinline HRESULT SetVoiceFormat(LPCXAUDIOFORMAT pFormat, LPCXAUDIOVOICEOUTPUT pVoiceOutput)
    {
        return XAudioSubmixVoice_SetVoiceFormat(this, pFormat, pVoiceOutput);
    }
};

#endif // __cplusplus

/**************************************************************************
 *
 * XAudioMasteringVoice
 *
 **************************************************************************/

#define XAudioMasteringVoice_AddRef                 XAudioVoice_AddRef
#define XAudioMasteringVoice_Release                XAudioVoice_Release
#define XAudioMasteringVoice_GetEffectState         XAudioVoice_GetEffectState
#define XAudioMasteringVoice_SetEffectState         XAudioVoice_SetEffectState
#define XAudioMasteringVoice_GetEffectParam         XAudioVoice_GetEffectParam
#define XAudioMasteringVoice_SetEffectParam         XAudioVoice_SetEffectParam
#define XAudioMasteringVoice_SetVoiceOutput         XAudioVoice_SetVoiceOutput
#define XAudioMasteringVoice_SetVoiceOutputVolume   XAudioVoice_SetVoiceOutputVolume
#define XAudioMasteringVoice_GetVoiceContext        XAudioVoice_GetVoiceContext
#define XAudioMasteringVoice_GetVoiceState          XAudioVoice_GetVoiceState
#define XAudioMasteringVoice_GetVoiceType           XAudioVoice_GetVoiceType

STDAPI XAudioMasteringVoice_GetVoiceFormat(IXAudioMasteringVoice *pVoice, LPXAUDIOFORMAT pFormat);

#ifdef __cplusplus

struct __declspec(novtable) IXAudioMasteringVoice : public IXAudioVoice
{
    __forceinline HRESULT GetVoiceFormat(LPXAUDIOFORMAT pFormat)
    {
        return XAudioMasteringVoice_GetVoiceFormat(this, pFormat);
    }
};

#endif // __cplusplus

#pragma pack(pop)
#pragma warning(pop)

#endif // __XAUDIO_INCLUDED__

