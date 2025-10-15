
/************************************************************************
*                                                                       *
*   XCam.h -- This module defines the Xbox Camera APIs                  *
*                                                                       *
*   Copyright (c) Microsoft Corp. All rights reserved.                  *
*                                                                       *
************************************************************************/

#pragma once
#ifndef __XCAM_H__
#define __XCAM_H__

#include <xonline.h>
#include <xaudio.h>
#include <xhv.h>

#ifdef __cplusplus
extern "C" {
#endif


//------------------------------------------------------------------------------
// XCam Interfaces
//------------------------------------------------------------------------------
typedef struct IXCamVideo          *PIXCAMVIDEO;
typedef struct IXCamStreamEngine   *PIXCAMSTREAMENGINE;

//------------------------------------------------------------------------------
// XCam Enum Typedefs
//------------------------------------------------------------------------------
typedef enum _XCAMDEVICESTATE {
    XCAMDEVICESTATE_DISCONNECTED    = 0,
    XCAMDEVICESTATE_CONNECTED       = 1,
    XCAMDEVICESTATE_INITIALIZED     = 2,
    XCAMDEVICESTATE_IN_ERROR        = 3,
    XCAMDEVICESTATE_ERROR_FULLSPEED = 4,
    XCAMDEVICESTATE_FORCE_DWORD  = 0x7fffffff
} XCAMDEVICESTATE;

typedef enum _XCAMRESOLUTION {
    XCAMRESOLUTION_160x120      = 0,
    XCAMRESOLUTION_176x144      = 1,
    XCAMRESOLUTION_320x240      = 2,
    XCAMRESOLUTION_352x288      = 3,
    XCAMRESOLUTION_640x480      = 4,
    XCAMRESOLUTION_960x720      = 5,
    XCAMRESOLUTION_1280x960     = 6,
    XCAMRESOLUTION_FORCE_DWORD  = 0x7fffffff
} XCAMRESOLUTION;

typedef enum _XCAMFRAMERATE {
    XCAMFRAMERATE_FASTEST       = 0,
    XCAMFRAMERATE_5             = 5,
    XCAMFRAMERATE_10            = 10,
    XCAMFRAMERATE_15            = 15,
    XCAMFRAMERATE_20            = 20,
    XCAMFRAMERATE_25            = 25,
    XCAMFRAMERATE_30            = 30,
    XCAMFRAMERATE_60            = 60,
    XCAMFRAMERATE_FORCE_DWORD   = 0x7fffffff
} XCAMFRAMERATE;

typedef enum _XCAMCONFIGIDS {
    XCAMCONFIGID_AE_MODE         = 2,
    XCAMCONFIGID_AE_PRIORITY     = 3,
    XCAMCONFIGID_EXPOSURE_TIME   = 4,
    XCAMCONFIGID_BACKLIGHT_COMP  = 101,
    XCAMCONFIGID_BRIGHTNESS      = 102,
    XCAMCONFIGID_CONTRAST        = 103,
    XCAMCONFIGID_GAIN            = 104,
    XCAMCONFIGID_FLICKER         = 105,
    XCAMCONFIGID_HUE             = 106,
    XCAMCONFIGID_SATURATION      = 107,
    XCAMCONFIGID_SHARPNESS       = 108,
    XCAMCONFIGID_GAMMA           = 109,
    XCAMCONFIGID_WHITE_BALANCE   = 110,
    XCAMCONFIGID_AUTO_WHITE_BAL  = 111,
    XCAMCONFIGID_LOWLIGHT        = 202,
    XCAMCONFIGID_LIGHTSOURCE     = 204,
    XCAMCONFIGID_COMP_ROOM_LIGHT = 300,
    XCAMCONFIGID_COMP_CORRECTED_LIGHTSOURCE = 301,
    XCAMCONFIGCONTROLID_FORCE_DWORD  = 0x7fffffff
} XCAMCONFIGCONTROLID;

typedef enum _XCAMZOOMFACTOR {
    XCAMZOOMFACTOR_1X            = 1,
    XCAMZOOMFACTOR_2X            = 2,
    XCAMZOOMFACTOR_4X            = 4,
    XCAMZOOMFACTOR_FORCE_DWORD   = 0x7fffffff
} XCAMZOOMFACTOR;

static const LONG XCAMCONFIG_LIGHTSOURCE_INCANDESCENT = 0;
static const LONG XCAMCONFIG_LIGHTSOURCE_FLOURESCENT = 1;
static const LONG XCAMCONFIG_LIGHTSOURCE_DAYLIGHT = 2;


//------------------------------------------------------------------------------
// XCam Callback Functions
//------------------------------------------------------------------------------
typedef VOID (*PFNCAMERADATAREADY)(DWORD UserData, PBYTE pVideoFrame, DWORD dwSize);

//------------------------------------------------------------------------------
// XCam Structures
//------------------------------------------------------------------------------
typedef struct _XCAM_STREAM_ENGINE_INIT_PARAMS {
    XCAMRESOLUTION                 VideoResolution;
    DWORD                          MaximumBitrate;
    DWORD                          MinimumBitrate;
    DWORD                          InitialTargetBitrate;
    XCAMFRAMERATE                  InitialFramerate;
    DWORD                          MaxRemoteConsoles;
    DWORD                          NetworkDataPacketSize;
    DWORD                          ThreadProcessorID;
    BOOL                           GenerateLocalPreview;
    BOOL                           GetDataDirectFromCamera;
    PFNCAMERADATAREADY             CameraDataReadyCallback;
    DWORD                          CameraDataReadyCallbackUserdata;
    IDirect3DDevice9              *pD3DDevice;
} XCAM_STREAM_ENGINE_INIT_PARAMS, *PXCAM_STREAM_ENGINE_INIT_PARAMS;


//------------------------------------------------------------------------------
// XCam Initialization and Shutdown APIs
//------------------------------------------------------------------------------

XBOXAPI
DWORD
WINAPI
XCamInitialize();

XBOXAPI
VOID
WINAPI
XCamShutdown();


//------------------------------------------------------------------------------
// XCam Basic Camera Access API's
//------------------------------------------------------------------------------


XBOXAPI
XCAMDEVICESTATE
WINAPI
XCamGetStatus();

XBOXAPI
DWORD
WINAPI
XCamGetConfig(
    IN     XCAMCONFIGCONTROLID     ControlID,
    OUT    LONG                   *pValue);

XBOXAPI
DWORD
WINAPI
XCamSetConfig(
    IN     XCAMCONFIGCONTROLID     ControlID,
    IN     LONG                    Value,
    IN OUT PXOVERLAPPED            pOverlapped OPTIONAL);

XBOXAPI
DWORD
WINAPI
XCamSetView(
    IN     XCAMZOOMFACTOR          ZoomFactor, 
    IN     LONG                    XCenter,
    IN     LONG                    YCenter,
    IN OUT PXOVERLAPPED            pOverlapped OPTIONAL);

XBOXAPI
DWORD
WINAPI
XCamGetView(
    OUT XCAMZOOMFACTOR            *pZoomFactor OPTIONAL, 
    OUT LONG                      *pXCenter OPTIONAL,
    OUT LONG                      *pYCenter OPTIONAL);

XBOXAPI
DWORD
WINAPI
XCamSnapshot(
    IN       XCAMRESOLUTION        Resolution,
    IN OUT   D3DLOCKED_RECT       *pBuffer,
    IN OUT   PXOVERLAPPED          pOverlapped);



//------------------------------------------------------------------------------
// XCam Interface Creation Functions
//------------------------------------------------------------------------------
XBOXAPI
DWORD
WINAPI
XCamCreateVideo(
    OUT      PIXCAMVIDEO                    *ppVideo
    );

XBOXAPI
DWORD
WINAPI
XCamCreateStreamEngine(
    IN CONST XCAM_STREAM_ENGINE_INIT_PARAMS *pParams,
    OUT      PIXCAMSTREAMENGINE             *ppEngine
    );


//------------------------------------------------------------------------------
// IXCamVideo Interface
//------------------------------------------------------------------------------

#undef  INTERFACE
#define INTERFACE   IXCamVideo

DECLARE_INTERFACE(IXCamVideo)
{
    STDMETHOD_(ULONG, AddRef)( IN THIS ) PURE;
    STDMETHOD_(ULONG, Release)( IN THIS ) PURE;

    STDMETHOD_(DWORD, SetCaptureMode)(
        IN                          THIS_
        IN     XCAMRESOLUTION       Resolution,
        IN     XCAMFRAMERATE        Framerate,
        IN OUT PXOVERLAPPED         pOverlapped OPTIONAL
        ) PURE;

    STDMETHOD_(DWORD, ReadFrame)(
        IN                         THIS_
        IN OUT D3DLOCKED_RECT     *pBuffer,
        IN OUT PXOVERLAPPED        pOverlapped
        ) PURE;
};


//------------------------------------------------------------------------------
// IXCamStreamEngine Interface
//------------------------------------------------------------------------------

#undef  INTERFACE
#define INTERFACE   IXCamStreamEngine

DECLARE_INTERFACE(IXCamStreamEngine)
{
    STDMETHOD_(ULONG, AddRef)( IN THIS ) PURE;
    STDMETHOD_(ULONG, Release)( IN THIS ) PURE;

    //
    // Finds a free remote console object and registers it with the specified XNADDR.
    // Use the "SendVideoToConsole" and "ReceiveVideoFromConsole" flags to specify
    // whether video data should flow in a single direction or bi-directionally
    // between the local machine and the remote machine.  Subsequent calls to
    // this function with the same XNADDR can be used to enable or disable
    // sending/receiving of video data, and/or update the list of XUIDs signed
    // on to that remote machine.
    //
    // If, because of parental control privileges, a video connection cannot be
    // established with this RemoteConsole, this function will return an error code.
    //
    // NOTE: At least one of the two flags must be TRUE - you cannot register a
    // remote console and neither send video to nor receive video from them!
    //
    STDMETHOD_(DWORD, RegisterRemoteConsole)(
        IN                         THIS_
        IN  XNADDR                 RemoteAddress,
        IN  CONST XUID            *RemoteXUIDList,
        IN  DWORD                  RemoteXUIDListLength,
        IN  BOOL                   SendVideoToConsole,
        IN  BOOL                   ReceiveVideoFromConsole
        ) PURE;

    //
    // De-registers a previously registered remote XNADDR.  This will terminate
    // the outgoing/incoming video stream(s) for that XNADDR.
    //
    STDMETHOD_(DWORD, UnregisterRemoteConsole)(
        IN                         THIS_
        IN  XNADDR                 RemoteAddress
        ) PURE;

    //
    // Retrieve the current list of registered remote consoles.
    //
    STDMETHOD_(VOID, GetRemoteConsoles)(
        IN                         THIS_
        OUT    XNADDR              *pRemoteAddresses,
        IN OUT PDWORD              pNumAddresses
        ) PURE;

    //
    // Retrieve the list of remote consoles whose video streams have been
    // disabled as a result of family settings.  This should be called
    // immediately following a user sign on event to check on the XCAM engine. 
    // Internally, as soon as the sign-in has occurred, the XCAM engine will
    // re-examine the parental control privileges and determine if video
    // communication can continue.  RemoteConsoles that are disabled as a
    // result of family settings will not produce any new outbound video
    // packets, nor will they generate new frames of video.
    //
    STDMETHOD_(VOID, GetDisallowedRemoteConsoles)(
        IN                         THIS_
        OUT    XNADDR              *pDisallowedRemoteAddresses,
        IN OUT PDWORD              pNumAddresses
        ) PURE;

    //
    // Push a frame of video into the "encode queue".  XOVERLAPPED is signaled once
    // the encode is complete
    //
    STDMETHOD_(DWORD, SubmitLocalVideoFrame)(
        IN                         THIS_
        IN  PBYTE                  pVideoFrame,
        IN  PXOVERLAPPED           pOverlapped
        ) PURE;

    //
    // Retrieves a chunk of data to be sent to the specified XNADDR (which was
    // previously registered via "RegisterRemoteConsole").  This blob can contain
    // both encoded video data, as well as QOS data and necessary feedback 
    // communication related to an incoming video stream.  This should be called
    // for _ALL_ registered remote consoles, regardless of whether they are sending
    // video, receiving video, or both.
    //
    STDMETHOD_(DWORD, GetNetworkData)(
        IN                         THIS_
        IN     XNADDR              RemoteAddress,
        OUT    PBYTE               pBuffer,
        IN OUT PDWORD              pBufferSize
        ) PURE;

    //
    // Submits a chunk of data that has been received on the network from the 
    // specified XNADDR (which was previously registered via "RegisterRemoteConsole").
    // See "GetNetworkData" for more information.
    //
    STDMETHOD_(DWORD, SubmitNetworkData)(
        IN                         THIS_
        IN  XNADDR                 RemoteAddress,
        IN  PBYTE                  pBuffer,
        IN  DWORD                  BufferSize,
        IN  PXOVERLAPPED           pOverlapped
        ) PURE;

    //
    // Specifies the screenspace rectangle in which the incoming video stream
    // corresponding with the RemoteAddress should be rendered.  This function should
    // be used in conjunction with the "RenderRemoteConsole" function.
    //
    STDMETHOD_(DWORD, SetRemoteConsoleRect)(
        IN                         THIS_
        IN       XNADDR            RemoteAddress,
        IN CONST D3DRECT          *pRect
        ) PURE;

    //
    // Specifies the default texture that should be used as the "fallback" texture for
    // this remote console during calls to "RenderRemoteConsole".  If no new frames have
    // been decoded in the last 5 seconds, the engine will switch over to rendering this
    // fallback texture (if one has been provided).  To get rid of a previously set
    // default texture, simply calls this function and pass in NULL for the
    // pDefaultTexture.  This fallback texture has no effect on the data returned by 
    // "GetNextRemoteConsoleFrame", and should only be used in conjunction with the
    // "RenderRemoteConsole" function.
    //
    STDMETHOD_(DWORD, SetRemoteConsoleDefaultTexture)(
        IN                         THIS_
        IN  XNADDR                 RemoteAddress,
        IN  D3DTexture            *pDefaultTexture
        ) PURE;

    //
    // Renders the current frame of the incoming video stream corresponding
    // with the specified RemoteAddress in the screenspace rectangle previously
    // specified in "SetRemoteConsoleRect".
    //
    STDMETHOD_(DWORD, RenderRemoteConsole)(
        IN                         THIS_
        IN  XNADDR                 RemoteAddress
        ) PURE;

    //
    // Obtains the next frame of video for the specified RemoteAddress. Use
    // this function if you want to handle displaying the incoming stream
    // yourself, rather than use the "RenderRemoteConsole" function. If no
    // new frames are available, this will return ERROR_IO_PENDING.
    //
    STDMETHOD_(DWORD, GetNextRemoteConsoleFrame)(
        IN                         THIS_
        IN     XNADDR              RemoteAddress,
        IN OUT D3DLOCKED_RECT     *pYBuffer,
        IN OUT D3DLOCKED_RECT     *pUBuffer,
        IN OUT D3DLOCKED_RECT     *pVBuffer
        ) PURE;

    //
    // Obtains the next frame of video for the specified RemoteAddress, and
    // converts it to D3DFMT_LIN_X8R8G8B8 directly within this function using
    // the CPU.  This function is _MUCH_ more CPU intensive than 
    // GetNextRemoteConsoleFrame(), and should only be used if you actually want
    // the color conversion to occur on the CPU instead of the GPU. If no new
    // frames are available, this will return ERROR_IO_PENDING.
    //
    STDMETHOD_(DWORD, GetNextRemoteConsoleFrameRGB)(
        IN                         THIS_
        IN     XNADDR              RemoteAddress,
        IN OUT D3DLOCKED_RECT     *pBuffer
        ) PURE;


    //
    // Does for the local preview video what "SetRemoteConsoleRect"
    // does for a remote console's incoming video stream.
    //
    STDMETHOD_(VOID, SetLocalPreviewRect)(
        IN                         THIS_
        IN CONST D3DRECT          *pRect
        ) PURE;

    //
    // Does for the local preview video what "RenderRemoteConsole" does
    // for a remote console's incoming video stream.
    //
    STDMETHOD_(VOID, RenderLocalPreview)(
        IN                         THIS
        ) PURE;

    //
    // Does for the local preview video what "GetNextRemoteConsoleFrame" does
    // for a remote console's incoming video stream.
    //
    STDMETHOD_(DWORD, GetNextLocalPreviewFrame)(
        IN                         THIS_
        IN OUT D3DLOCKED_RECT     *pBuffer
        ) PURE;

    //
    // Does for the local preview video what "GetNextRemoteConsoleFrameRGB"
    // does for a remote console's incoming video stream
    //
    STDMETHOD_(DWORD, GetNextLocalPreviewFrameRGB)(
        IN                         THIS_
        IN OUT D3DLOCKED_RECT     *pBuffer
        ) PURE;

    //
    // Encoder controls
    //
    STDMETHOD_(VOID, SetEncoderTargetBitrate)(
        IN                         THIS_
        IN  DWORD                  TargetBitrate
        ) PURE;

    STDMETHOD_(DWORD, GetEncoderTargetBitrate)(
        IN                         THIS
        ) PURE;

    STDMETHOD_(VOID, SetEncoderFramerate)(
        IN                         THIS_
        IN  XCAMFRAMERATE          Framerate
        ) PURE;

    STDMETHOD_(XCAMFRAMERATE, GetEncoderFramerate)(
        IN                         THIS
        ) PURE;

    STDMETHOD_(DWORD, GetEncoderActualBitrate)(
        IN                         THIS
        ) PURE;


};


//------------------------------------------------------------------------------
// C Function Wrappers for XCAM Interfaces
//------------------------------------------------------------------------------

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

FORCEINLINE
STDMETHODIMP_(ULONG)
IXCamVideo_AddRef(
    IN  PIXCAMVIDEO                pThis
    )
{
    return VTBL(pThis)->AddRef(THIS);
}

FORCEINLINE
STDMETHODIMP_(ULONG)
IXCamVideo_Release(
    IN  PIXCAMVIDEO                pThis
    )
{
    return VTBL(pThis)->Release(THIS);
}

FORCEINLINE
STDMETHODIMP_(DWORD)
IXCamVideo_SetCaptureMode(
    IN  PIXCAMVIDEO                pThis,
    IN  XCAMRESOLUTION             Resolution,
    IN  XCAMFRAMERATE              Framerate,
    IN  PXOVERLAPPED               pOverlapped
    )
{
    return VTBL(pThis)->SetCaptureMode(THIS_ Resolution, Framerate, pOverlapped);
}

FORCEINLINE
STDMETHODIMP_(DWORD)
IXCamVideo_ReadFrame(
    IN  PIXCAMVIDEO                pThis,
    IN OUT D3DLOCKED_RECT         *pBuffer,
    IN  PXOVERLAPPED               pOverlapped
    )
{
    return VTBL(pThis)->ReadFrame(THIS_ pBuffer, pOverlapped);
}




FORCEINLINE
STDMETHODIMP_(ULONG)
IXCamStreamEngine_AddRef(
    IN  PIXCAMSTREAMENGINE         pThis
    )
{
    return VTBL(pThis)->AddRef(THIS);
}

FORCEINLINE
STDMETHODIMP_(ULONG)
IXCamStreamEngine_Release(
    IN  PIXCAMSTREAMENGINE         pThis
    )
{
    return VTBL(pThis)->Release(THIS);
}

FORCEINLINE
STDMETHODIMP_(DWORD)
IXCamStreamEngine_RegisterRemoteConsole(
    IN  PIXCAMSTREAMENGINE         pThis,
    IN  XNADDR                     RemoteAddress,
    IN  CONST XUID                *RemoteXUIDList,
    IN  DWORD                      RemoteXUIDListLength,
    IN  BOOL                       SendVideoToConsole,
    IN  BOOL                       ReceiveVideoFromConsole
    )
{
    return VTBL(pThis)->RegisterRemoteConsole(THIS_ RemoteAddress, RemoteXUIDList, RemoteXUIDListLength, SendVideoToConsole, ReceiveVideoFromConsole);
}

FORCEINLINE
STDMETHODIMP_(DWORD)
IXCamStreamEngine_UnregisterRemoteConsole(
    IN  PIXCAMSTREAMENGINE         pThis,
    IN  XNADDR                     RemoteAddress
    )
{
    return VTBL(pThis)->UnregisterRemoteConsole(THIS_ RemoteAddress);
}

FORCEINLINE
STDMETHODIMP_(VOID)
IXCamStreamEngine_GetRemoteConsoles(
    IN     PIXCAMSTREAMENGINE      pThis,
    IN     XNADDR                 *pRemoteAddresses,
    IN OUT PDWORD                  pNumAddresses
    )
{
    return VTBL(pThis)->GetRemoteConsoles(THIS_ pRemoteAddresses, pNumAddresses);
}

FORCEINLINE
STDMETHODIMP_(VOID)
IXCamStreamEngine_GetDisallowedRemoteConsoles(
    IN     PIXCAMSTREAMENGINE      pThis,
    IN     XNADDR                 *pDisallowedRemoteAddresses,
    IN OUT PDWORD                  pNumAddresses
    )
{
    return VTBL(pThis)->GetDisallowedRemoteConsoles(THIS_ pDisallowedRemoteAddresses, pNumAddresses);
}

FORCEINLINE
STDMETHODIMP_(DWORD)
IXCamStreamEngine_SubmitLocalVideoFrame(
    IN  PIXCAMSTREAMENGINE         pThis,
    IN  PBYTE                      pVideoFrame,
    IN  PXOVERLAPPED               pOverlapped
    )
{
    return VTBL(pThis)->SubmitLocalVideoFrame(THIS_ pVideoFrame, pOverlapped);
}

FORCEINLINE
STDMETHODIMP_(DWORD)
IXCamStreamEngine_GetNetworkData(
    IN     PIXCAMSTREAMENGINE      pThis,
    IN     XNADDR                  RemoteAddress,
    OUT    PBYTE                   pBuffer,
    IN OUT PDWORD                  pBufferSize
    )
{
    return VTBL(pThis)->GetNetworkData(THIS_ RemoteAddress, pBuffer, pBufferSize);
}

FORCEINLINE
STDMETHODIMP_(DWORD)
IXCamStreamEngine_SubmitNetworkData(
    IN  PIXCAMSTREAMENGINE         pThis,
    IN  XNADDR                     RemoteAddress,
    IN  PBYTE                      pBuffer,
    IN  DWORD                      BufferSize,
    IN  PXOVERLAPPED               pOverlapped
    )
{
    return VTBL(pThis)->SubmitNetworkData(THIS_ RemoteAddress, pBuffer, BufferSize, pOverlapped);
}

FORCEINLINE
STDMETHODIMP_(DWORD)
IXCamStreamEngine_SetRemoteConsoleRect(
    IN       PIXCAMSTREAMENGINE    pThis,
    IN       XNADDR                RemoteAddress,
    IN CONST D3DRECT              *pRect
    )
{
    return VTBL(pThis)->SetRemoteConsoleRect(THIS_ RemoteAddress, pRect);
}

FORCEINLINE
STDMETHODIMP_(DWORD)
IXCamStreamEngine_SetRemoteConsoleDefaultTexture(
    IN  PIXCAMSTREAMENGINE         pThis,
    IN  XNADDR                     RemoteAddress,
    IN  D3DTexture                *pDefaultTexture
    )
{
    return VTBL(pThis)->SetRemoteConsoleDefaultTexture(THIS_ RemoteAddress, pDefaultTexture);
}

FORCEINLINE
STDMETHODIMP_(DWORD)
IXCamStreamEngine_RenderRemoteConsole(
    IN  PIXCAMSTREAMENGINE         pThis,
    IN  XNADDR                     RemoteAddress
    )
{
    return VTBL(pThis)->RenderRemoteConsole(THIS_ RemoteAddress);
}

FORCEINLINE
STDMETHODIMP_(DWORD)
IXCamStreamEngine_GetNextRemoteConsoleFrame(
    IN     PIXCAMSTREAMENGINE      pThis,
    IN     XNADDR                  RemoteAddress,
    IN OUT D3DLOCKED_RECT         *pYBuffer,
    IN OUT D3DLOCKED_RECT         *pUBuffer,
    IN OUT D3DLOCKED_RECT         *pVBuffer
    )
{
    return VTBL(pThis)->GetNextRemoteConsoleFrame(THIS_ RemoteAddress, pYBuffer, pUBuffer, pVBuffer);
}

FORCEINLINE
STDMETHODIMP_(DWORD)
IXCamStreamEngine_GetNextRemoteConsoleFrameRGB(
    IN     PIXCAMSTREAMENGINE      pThis,
    IN     XNADDR                  RemoteAddress,
    IN OUT D3DLOCKED_RECT         *pBuffer
    )
{
    return VTBL(pThis)->GetNextRemoteConsoleFrameRGB(THIS_ RemoteAddress, pBuffer);
}

FORCEINLINE
STDMETHODIMP_(VOID)
IXCamStreamEngine_SetLocalPreviewRect(
    IN       PIXCAMSTREAMENGINE    pThis,
    IN CONST D3DRECT              *pRect
    )
{
    return VTBL(pThis)->SetLocalPreviewRect(THIS_ pRect);
}

FORCEINLINE
STDMETHODIMP_(VOID)
IXCamStreamEngine_RenderLocalPreview(
    IN  PIXCAMSTREAMENGINE         pThis
    )
{
    return VTBL(pThis)->RenderLocalPreview(THIS);
}

FORCEINLINE
STDMETHODIMP_(DWORD)
IXCamStreamEngine_GetNextLocalPreviewFrame(
    IN     PIXCAMSTREAMENGINE      pThis,
    IN OUT D3DLOCKED_RECT         *pBuffer
    )
{
    return VTBL(pThis)->GetNextLocalPreviewFrame(THIS_ pBuffer);
}

FORCEINLINE
STDMETHODIMP_(DWORD)
IXCamStreamEngine_GetNextLocalPreviewFrameRGB(
    IN     PIXCAMSTREAMENGINE      pThis,
    IN OUT D3DLOCKED_RECT         *pBuffer
    )
{
    return VTBL(pThis)->GetNextLocalPreviewFrameRGB(THIS_ pBuffer);
}

FORCEINLINE
STDMETHODIMP_(VOID)
IXCamStreamEngine_SetEncoderTargetBitrate(
    IN  PIXCAMSTREAMENGINE         pThis,
    IN  DWORD                      TargetBitrate
    )
{
    return VTBL(pThis)->SetEncoderTargetBitrate(THIS_ TargetBitrate);
}

FORCEINLINE
STDMETHODIMP_(DWORD)
IXCamStreamEngine_GetEncoderTargetBitrate(
    IN  PIXCAMSTREAMENGINE         pThis
    )
{
    return VTBL(pThis)->GetEncoderTargetBitrate(THIS);
}

FORCEINLINE
STDMETHODIMP_(VOID)
IXCamStreamEngine_SetEncoderFramerate(
    IN  PIXCAMSTREAMENGINE         pThis,
    IN  XCAMFRAMERATE              Framerate
    )
{
    return VTBL(pThis)->SetEncoderFramerate(THIS_ Framerate);
}

FORCEINLINE
STDMETHODIMP_(XCAMFRAMERATE)
IXCamStreamEngine_GetEncoderFramerate(
    IN  PIXCAMSTREAMENGINE         pThis
    )
{
    return VTBL(pThis)->GetEncoderFramerate(THIS);
}

FORCEINLINE
STDMETHODIMP_(DWORD)
IXCamStreamEngine_GetEncoderActualBitrate(
    IN  PIXCAMSTREAMENGINE         pThis
    )
{
    return VTBL(pThis)->GetEncoderActualBitrate(THIS);
}



#pragma pop_macro("VTBL")
#pragma pop_macro("THIS")
#pragma pop_macro("THIS_")

#ifdef __cplusplus
}
#endif

#endif // __XCAM_H__
