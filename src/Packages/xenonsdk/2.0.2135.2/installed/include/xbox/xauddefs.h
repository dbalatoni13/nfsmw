/**************************************************************************
 *
 *  Copyright (C) 2003 - 2005 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       xauddefs.h
 *  Content:    Xbox audio type and value definitions.
 *
 **************************************************************************/

#ifndef __XAUDDEFS_INCLUDED__
#define __XAUDDEFS_INCLUDED__

#ifdef _XBOX
#include <xtl.h>
#include <xobjbase.h>
#else // _XBOX
#include <windows.h>
#include <objbase.h>
#endif // _XBOX

#include <math.h>

#pragma warning(push)
#pragma warning(disable:4201)

#pragma pack(push, 4)

/**************************************************************************
 *
 * Interface declarations
 *
 **************************************************************************/

typedef struct IXAudioRefCount          IXAudioRefCount,            *LPXAUDIOREFCOUNT;
typedef struct IXAudioBatchAllocator    IXAudioBatchAllocator,      *LPXAUDIOBATCHALLOCATOR;
typedef struct IXAudioEffect            IXAudioEffect,              *LPXAUDIOEFFECT;
typedef struct IXAudioSourceEffect      IXAudioSourceEffect,        *LPXAUDIOSOURCEFFECT;
typedef struct IXAudioRouterEffect      IXAudioRouterEffect,        *LPXAUDIOROUTEREFFECT;
typedef struct IXAudioRenderEffect      IXAudioRenderEffect,        *LPXAUDIORENDEREFFECT;
typedef struct IXAudioFrameBuffer       IXAudioFrameBuffer,         *LPXAUDIOFRAMEBUFFER;
typedef struct IXAudioEffectManager     IXAudioEffectManager,       *LPXAUDIOEFFECTMANAGER;

#ifdef __cplusplus

typedef struct IXAudioVoice             IXAudioVoice,               *LPXAUDIOVOICE;
typedef struct IXAudioSourceVoice       IXAudioSourceVoice,         *LPXAUDIOSOURCEVOICE;
typedef struct IXAudioSubmixVoice       IXAudioSubmixVoice,         *LPXAUDIOSUBMIXVOICE;
typedef struct IXAudioMasteringVoice    IXAudioMasteringVoice,      *LPXAUDIOMASTERINGVOICE;

#else // __cplusplus

typedef void                            IXAudioVoice,               *LPXAUDIOVOICE;
typedef void                            IXAudioSourceVoice,         *LPXAUDIOSOURCEVOICE;
typedef void                            IXAudioSubmixVoice,         *LPXAUDIOSUBMIXVOICE;
typedef void                            IXAudioMasteringVoice,      *LPXAUDIOMASTERINGVOICE;

#endif // __cplusplus

/**************************************************************************
 *
 * Types, ranges and constants
 *
 **************************************************************************/

typedef FLOAT XAUDIOSAMPLE, *LPXAUDIOSAMPLE;
typedef const XAUDIOSAMPLE *LPCXAUDIOSAMPLE;

#define XAUDIOSAMPLE_MIN        -1.0f
#define XAUDIOSAMPLE_MAX        1.0f

typedef FLOAT XAUDIOVOLUME, *LPXAUDIOVOLUME;
typedef const XAUDIOVOLUME *LPCXAUDIOVOLUME;

#define XAUDIOVOLUME_MIN        0.0f
#define XAUDIOVOLUME_MAX        1.0f
#define XAUDIOVOLUME_DEFAULT    1.0f

typedef FLOAT XAUDIOPITCH, *LPXAUDIOPITCH;
typedef const XAUDIOPITCH *LPCXAUDIOPITCH;

typedef BYTE XAUDIOFREQSCALE, *LPXAUDIOFREQSCALE;
typedef const XAUDIOFREQSCALE *LPCXAUDIOFREQSCALE;

#define XAUDIOPITCH_DEFAULT     0.0f

typedef BYTE XAUDIOCHANNEL, *LPXAUDIOCHANNEL;
typedef const XAUDIOCHANNEL *LPCXAUDIOCHANNEL;

#define XAUDIOCHANNEL_MIN       0
#define XAUDIOCHANNEL_MAX       0xFF
#define XAUDIOCHANNEL_SOURCEMAX 6

typedef BYTE XAUDIOFXID, *LPXAUDIOFXID;
typedef const XAUDIOFXID *LPCXAUDIOFXID;

#define XAUDIOFXID_MIN          0
#define XAUDIOFXID_MAX          0xFF

#define XAUDIOFXID_STATICMIN    XAUDIOFXID_MIN
#define XAUDIOFXID_STATICMAX    0x7F
#define XAUDIOFXID_STATICCOUNT  (XAUDIOFXID_STATICMAX - XAUDIOFXID_STATICMIN + 1)

#define XAUDIOFXID_DYNMIN       0x80
#define XAUDIOFXID_DYNMAX       XAUDIOFXID_MAX
#define XAUDIOFXID_DYNCOUNT     (XAUDIOFXID_DYNMAX - XAUDIOFXID_DYNMIN + 1)

typedef BYTE XAUDIOFXPARAMID, *LPXAUDIOFXPARAMID;
typedef const XAUDIOFXPARAMID *LPCXAUDIOFXPARAMID;

#define XAUDIOFXPARAMID_MIN     0
#define XAUDIOFXPARAMID_MAX     0xFF

typedef BYTE XAUDIOPACKETINDEX, *LPXAUDIOPACKETINDEX;
typedef const XAUDIOPACKETINDEX *LPCXAUDIOPACKETINDEX;

#define XAUDIOPACKETINDEX_MIN   0
#define XAUDIOPACKETINDEX_MAX   0xFF

typedef WORD XAUDIOFRAMESIZE, *LPXAUDIOFRAMESIZE;
typedef const XAUDIOFRAMESIZE *LPCXAUDIOFRAMESIZE;

#define XAUDIOFRAMESIZE_MIN     0
#define XAUDIOFRAMESIZE_MAX     0xFFFF
#define XAUDIOFRAMESIZE_NATIVE  256

typedef DWORD XAUDIOLOOPCOUNT, *LPXAUDIOLOOPCOUNT;
typedef const XAUDIOLOOPCOUNT *LPCXAUDIOLOOPCOUNT;

#define XAUDIOLOOPCOUNT_MIN         0
#define XAUDIOLOOPCOUNT_MAX         0xFFFFFFFE
#define XAUDIOLOOPCOUNT_INFINITE    0xFFFFFFFF

typedef HRESULT XAUDIOPACKETSTATUS, *LPXAUDIOPACKETSTATUS;
typedef const XAUDIOPACKETSTATUS *LPCXAUDIOPACKETSTATUS;

#define XAUDIOPACKETSTATUS_SUCCESS  S_OK
#define XAUDIOPACKETSTATUS_FLUSHED  E_ABORT
#define XAUDIOPACKETSTATUS_ERROR    E_FAIL

typedef DWORD XAUDIOSAMPLERATE, *LPXAUDIOSAMPLERATE;
typedef const XAUDIOSAMPLERATE *LPCXAUDIOSAMPLERATE;

#define XAUDIOSAMPLERATE_MIN    1
#define XAUDIOSAMPLERATE_MAX    0xFFFFFFFF
#define XAUDIOSAMPLERATE_NATIVE 48000

typedef WORD XAUDIOTRAILFRAME, *LPXAUDIOTRAILFRAME;
typedef const XAUDIOTRAILFRAME *LPCXAUDIOTRAILFRAME;

#define XAUDIOTRAILFRAME_MIN    0
#define XAUDIOTRAILFRAME_MAX    0xFFFF

typedef BYTE XAUDIOPRIMEFRAME, *LPXAUDIOPRIMEFRAME;
typedef const XAUDIOPRIMEFRAME *LPCXAUDIOPRIMEFRAME;

#define XAUDIOPRIMEFRAME_MIN    0
#define XAUDIOPRIMEFRAME_MAX    0xFF

typedef BYTE XAUDIOVOICEFXINDEX, *LPXAUDIOVOICEFXINDEX;
typedef const XAUDIOVOICEFXINDEX *LPCXAUDIOVOICEFXINDEX;

#define XAUDIOVOICEFXINDEX_MIN  0
#define XAUDIOVOICEFXINDEX_MAX  0xFF

typedef BYTE XAUDIOENGINECALLBACKINDEX, *LPXAUDIOENGINECALLBACKINDEX;
typedef const XAUDIOENGINECALLBACKINDEX *LPCXAUDIOENGINECALLBACKINDEX;

#define XAUDIOENGINECALLBACKINDEX_MIN   0
#define XAUDIOENGINECALLBACKINDEX_MAX   7
#define XAUDIOENGINECALLBACKINDEX_COUNT (XAUDIOENGINECALLBACKINDEX_MAX + 1)

typedef BYTE XAUDIOVOICEINDEX, *LPXAUDIOVOICEINDEX;
typedef const XAUDIOVOICEINDEX *LPCXAUDIOVOICEINDEX;

#define XAUDIOVOICEINDEX_MIN    0
#define XAUDIOVOICEINDEX_MAX    0xFF

typedef BYTE XAUDIOSUBMIXSTAGE, *LPXAUDIOSUBMIXSTAGE;
typedef const XAUDIOSUBMIXSTAGE *LPCXAUDIOSUBMIXSTAGE;

#define XAUDIOSUBMIXSTAGE_MIN   0
#define XAUDIOSUBMIXSTAGE_MAX   0xFF

typedef LPVOID XAUDIOCALLBACKHANDLE, *LPXAUDIOCALLBACKHANDLE;
typedef const XAUDIOCALLBACKHANDLE *LPCXAUDIOCALLBACKHANDLE;

typedef INT XAUDIODEBUGLEVEL, *LPXAUDIODEBUGLEVEL;
typedef const XAUDIODEBUGLEVEL *LPCXAUDIODEBUGLEVEL;

#define XAUDIODEBUGLEVEL_NONE           -1
#define XAUDIODEBUGLEVEL_FATAL          0
#define XAUDIODEBUGLEVEL_ERROR          1
#define XAUDIODEBUGLEVEL_RESOURCE       2
#define XAUDIODEBUGLEVEL_WARNING        3
#define XAUDIODEBUGLEVEL_INFO           4
#define XAUDIODEBUGLEVEL_VERBOSE        5

typedef DWORD XAUDIODEBUGFLAGS, *LPXAUDIODEBUGFLAGS;
typedef const XAUDIODEBUGFLAGS *LPCXAUDIODEBUGFLAGS;

#define XAUDIODEBUGFLAGS_LIBRARY            0x00000001
#define XAUDIODEBUGFLAGS_FILELINE           0x00000002
#define XAUDIODEBUGFLAGS_PROCESSTHREADID    0x00000004
#define XAUDIODEBUGFLAGS_FUNCTIONNAME       0x00000008
#define XAUDIODEBUGFLAGS_TIME               0x00000010
#define XAUDIODEBUGFLAGS_TIME_COMPLEX       0x00000020
#define XAUDIODEBUGFLAGS_MASK               0x0000003F
#define XAUDIODEBUGFLAGS_DEFAULT            0x00000001

#define XAUDIOBATCHALLOCATOR_ALIGNMENT  4

typedef LPVOID XAUDIOEVENT, *LPXAUDIOEVENT;
typedef const XAUDIOEVENT *LPCXAUDIOEVENT;

typedef BYTE XAUDIOXMASTREAMCOUNT, *LPXAUDIOXMASTREAMCOUNT;
typedef const XAUDIOXMASTREAMCOUNT *LPCXAUDIOXMASTREAMCOUNT;

#define XAUDIOXMASTREAMCOUNT_MIN 1
#define XAUDIOXMASTREAMCOUNT_MAX XAUDIOCHANNEL_SOURCEMAX

/**************************************************************************
 *
 * Enumerations
 *
 **************************************************************************/

typedef BYTE XAUDIOSPEAKER, *LPXAUDIOSPEAKER;
typedef const XAUDIOSPEAKER *LPCXAUDIOSPEAKER;

#define XAUDIOSPEAKER_FRONTLEFT     0
#define XAUDIOSPEAKER_FRONTRIGHT    1
#define XAUDIOSPEAKER_FRONTCENTER   2
#define XAUDIOSPEAKER_LOWFREQUENCY  3
#define XAUDIOSPEAKER_BACKLEFT      4
#define XAUDIOSPEAKER_BACKRIGHT     5
#define XAUDIOSPEAKER_COUNT         6

#define XAUDIOSPEAKER_MIN           0
#define XAUDIOSPEAKER_MAX           (XAUDIOSPEAKER_COUNT - 1)

typedef BYTE XAUDIOSAMPLETYPE, *LPXAUDIOSAMPLETYPE;
typedef const XAUDIOSAMPLETYPE *LPCXAUDIOSAMPLETYPE;

#define XAUDIOSAMPLETYPE_FLOAT      0       // IEEE 32-bit floating point samples between -1.0 and 1.0
#define XAUDIOSAMPLETYPE_16BITPCM   1       // 16-bit big-endian PCM
#define XAUDIOSAMPLETYPE_8BITPCM    2       // 8-bit PCM
#define XAUDIOSAMPLETYPE_XMA        3       // XMA
#define XAUDIOSAMPLETYPE_COUNT      4

#define XAUDIOSAMPLETYPE_NATIVE     XAUDIOSAMPLETYPE_FLOAT
#define XAUDIOSAMPLETYPE_MIN        0
#define XAUDIOSAMPLETYPE_MAX        (XAUDIOSAMPLETYPE_COUNT - 1)
#define XAUDIOSAMPLETYPE_PCMMIN     XAUDIOSAMPLETYPE_FLOAT
#define XAUDIOSAMPLETYPE_PCMMAX     XAUDIOSAMPLETYPE_8BITPCM

typedef BYTE XAUDIOFRAMEBUFTYPE, *LPXAUDIOFRAMEBUFTYPE;
typedef const XAUDIOFRAMEBUFTYPE *LPCXAUDIOFRAMEBUFTYPE;

#define XAUDIOFRAMEBUFTYPE_INTERNAL 0       // The frame buffer allocates its own memory
#define XAUDIOFRAMEBUFTYPE_EXTERNAL 1       // The frame buffer points to memory owned by the title
#define XAUDIOFRAMEBUFTYPE_COUNT    2

#define XAUDIOFRAMEBUFTYPE_MIN      0
#define XAUDIOFRAMEBUFTYPE_MAX      (XAUDIOFRAMEBUFTYPE_COUNT - 1)

typedef BYTE XAUDIOVOICETYPE, *LPXAUDIOVOICETYPE;
typedef const XAUDIOVOICETYPE *LPCXAUDIOVOICETYPE;

#define XAUDIOVOICETYPE_SOURCE      0       // Source voice
#define XAUDIOVOICETYPE_SUBMIX      1       // Submix destination voice
#define XAUDIOVOICETYPE_MASTERING   2       // The final mastering voice
#define XAUDIOVOICETYPE_COUNT       3

#define XAUDIOVOICETYPE_MIN         0
#define XAUDIOVOICETYPE_MAX         (XAUDIOVOICETYPE_COUNT - 1)

typedef BYTE XAUDIOENGINECALLBACKTYPE, *LPXAUDIOENGINECALLBACKTYPE;
typedef const XAUDIOENGINECALLBACKTYPE *LPCXAUDIOENGINECALLBACKTYPE;

#define XAUDIOENGINECALLBACKTYPE_FRAMESTART 0       // Called before the engine processes any voices
#define XAUDIOENGINECALLBACKTYPE_FRAMEEND   1       // Called when the engine has finished processing all voices
#define XAUDIOENGINECALLBACKTYPE_COUNT      2

#define XAUDIOENGINECALLBACKTYPE_MIN        0
#define XAUDIOENGINECALLBACKTYPE_MAX        (XAUDIOENGINECALLBACKTYPE_COUNT - 1)

typedef BYTE XAUDIOROUTERFXMODE, *LPXAUDIOROUTERFXMODE;
typedef const XAUDIOROUTERFXMODE *LPCXAUDIOROUTERFXMODE;

#define XAUDIOROUTERFXMODE_MIX          0       // Mix into the output frame buffer
#define XAUDIOROUTERFXMODE_OVERWRITE    1       // Overwrite any data in the ouput frame buffer
#define XAUDIOROUTERFXMODE_COUNT        2

#define XAUDIOROUTERFXMODE_MIN          0
#define XAUDIOROUTERFXMODE_MAX          (XAUDIOROUTERFXMODE_COUNT - 1)

typedef BYTE XAUDIOVOICECATEGORY, *LPXAUDIOVOICECATEGORY;
typedef const XAUDIOVOICECATEGORY *LPCXAUDIOVOICECATEGORY;

#define XAUDIOVOICECATEGORY_NONE    0       // The voice does not belong to a defined category
#define XAUDIOVOICECATEGORY_BGMUSIC 1       // The voice is playing background music
#define XAUDIOVOICECATEGORY_COUNT   2

#define XAUDIOVOICECATEGORY_MIN     0
#define XAUDIOVOICECATEGORY_MAX     (XAUDIOVOICECATEGORY_COUNT - 1)

typedef BYTE XAUDIOFXPARAMTYPE, *LPXAUDIOFXPARAMTYPE;
typedef const XAUDIOFXPARAMTYPE *LPCXAUDIOFXPARAMTYPE;

#define XAUDIOFXPARAMTYPE_NULL      0       // The parameter does not take or return any data
#define XAUDIOFXPARAMTYPE_NUMERIC   1       // The paremeter uses a floating-point numeric value
#define XAUDIOFXPARAMTYPE_FLAG      2       // The parameter uses a 32-bit bitmask
#define XAUDIOFXPARAMTYPE_DATA      3       // The parameter uses a blob of data
#define XAUDIOFXPARAMTYPE_COUNT     4

#define XAUDIOFXPARAMTYPE_MIN       0
#define XAUDIOFXPARAMTYPE_MAX       (XAUDIOFXPARAMTYPE_COUNT - 1)

/**************************************************************************
 *
 * Bitfields and flags
 *
 **************************************************************************/

typedef BYTE XAUDIODATAFLOW, *LPXAUDIODATAFLOW;
typedef const XAUDIODATAFLOW *LPCXAUDIODATAFLOW;

#define XAUDIODATAFLOW_INPLACE  0x01        // The effect processes data in the input frame buffer in-place
#define XAUDIODATAFLOW_INPUT    0x02        // The effect takes input data only
#define XAUDIODATAFLOW_OUTPUT   0x04        // The effect produces output only
#define XAUDIODATAFLOW_IO       0x06        // The effect reads from an input buffer and writes to an output buffer
#define XAUDIODATAFLOW_MASK     0x07

typedef BYTE XAUDIOSOURCESTATE, *LPXAUDIOSOURCESTATE;
typedef const XAUDIOSOURCESTATE *LPCXAUDIOSOURCESTATE;

#define XAUDIOSOURCESTATE_STARTED       0x01        // The voice or effect is running
#define XAUDIOSOURCESTATE_STARTING      0x02        // The voice or effect will run for the first time on the next frame
#define XAUDIOSOURCESTATE_STOPPING      0x04        // The voice or effect will stop asynchronously
#define XAUDIOSOURCESTATE_DISCONTINUITY 0x08        // The voice or effect will not consider running out of packets as an error
#define XAUDIOSOURCESTATE_STARVED       0x10        // The voice or effect has run out of packets and considers it an error state
#define XAUDIOSOURCESTATE_READYPACKET   0x20        // The voice or effect is ready to receive more packets
#define XAUDIOSOURCESTATE_SYNCHRONIZED  0x40        // The voice is waiting for a pending synchronized start
#define XAUDIOSOURCESTATE_MASK          0x7F

typedef BYTE XAUDIOSOURCEFLAGS, *LPXAUDIOSOURCEFLAGS;
typedef const XAUDIOSOURCEFLAGS *LPCXAUDIOSOURCEFLAGS;

#define XAUDIOSOURCEFLAGS_NOPITCH       0x01        // The title does not require the effect to support pitch shifting
#define XAUDIOSOURCEFLAGS_DEFERHW       0x02        // Hardware resources are allocated and freed with Start and Stop
#define XAUDIOSOURCEFLAGS_MASK          0x03

typedef BYTE XAUDIOSTART, *LPXAUDIOSTART;
typedef const XAUDIOSTART *LPCXAUDIOSTART;

#define XAUDIOSTART_SYNCHRONIZED        0x01        // The voice should defer starting until XAudioSynchStart is called
#define XAUDIOSTART_MASK                0x01

typedef BYTE XAUDIOSTOP, *LPXAUDIOSTOP;
typedef const XAUDIOSTOP *LPCXAUDIOSTOP;

#define XAUDIOSTOP_IMMEDIATE            0x01        // The voice or effect should stop immediately and not gracefully roll off
#define XAUDIOSTOP_MASK                 0x01

typedef BYTE XAUDIOFRAMEBUFPREPARE, *LPXAUDIOFRAMEBUFPREPARE;
typedef const XAUDIOFRAMEBUFPREPARE *LPCXAUDIOFRAMEBUFPREPARE;

#define XAUDIOFRAMEBUFPREPARE_REINIT    0x01        // Force a reinitialization of the frame buffer
#define XAUDIOFRAMEBUFPREPARE_ZERO      0x02        // Set the frame buffer's data to all zeros
#define XAUDIOFRAMEBUFPREPARE_MASK      0x03

typedef BYTE XAUDIOSUBMITPACKET, *LPXAUDIOSUBMITPACKET;
typedef const XAUDIOSUBMITPACKET *LPCXAUDIOSUBMITPACKET;

#define XAUDIOSUBMITPACKET_DISCONTINUITY    0x01        // The effect should not consider running out of packets as an error
#define XAUDIOSUBMITPACKET_MASK             0x01

typedef BYTE XAUDIOVOICEFXSTATE, *LPXAUDIOVOICEFXSTATE;
typedef const XAUDIOVOICEFXSTATE *LPCXAUDIOVOICEFXSTATE;

#define XAUDIOVOICEFXSTATE_ENABLED  0x01        // The effect should run
#define XAUDIOVOICEFXSTATE_MASK     0x01

typedef DWORD XAUDIOLOCKTYPE, *LPXAUDIOLOCKTYPE;
typedef const XAUDIOLOCKTYPE *LPCXAUDIOLOCKTYPE;

#define XAUDIOLOCKTYPE_LOCK             0x00000001  // Block until the lock is taken
#define XAUDIOLOCKTYPE_TRYLOCK          0x00000002  // Attempt to take the lock, but return if held by another thread
#define XAUDIOLOCKTYPE_UNLOCK           0x00000003  // Unlock a previously successful lock
#define XAUDIOLOCKTYPE_OPERATIONMASK    0x0000FFFF

#define XAUDIOLOCKTYPE_SPINLOCK         0x00010000  // High-performance spinlock used for synchronizing data access
#define XAUDIOLOCKTYPE_API              0x00020000  // XAudio API critical section
#define XAUDIOLOCKTYPE_LOCKMASK         0xFFFF0000

typedef DWORD XAUDIOSPEAKERCONFIG, *LPXAUDIOSPEAKERCONFIG;
typedef const XAUDIOSPEAKERCONFIG *LPCXAUDIOSPEAKERCONFIG;

//
// Analog speaker configuration values are a enumeration while digital values
// are a bitmask.  0 is not a valid analog value, but 0 in the digital word 
// indicates that no digital encoding or preencoded output is allowed on the
// digital channel.  In that case, the digital channel will output the same
// data as the analog.
//

#define XAUDIOSPEAKERCONFIG_ANALOG_SURROUND         0x00000001
#define XAUDIOSPEAKERCONFIG_ANALOG_MONO             0x00000002
#define XAUDIOSPEAKERCONFIG_ANALOG_STEREOBYPASS     0x00000003
#define XAUDIOSPEAKERCONFIG_ANALOG_MASK             0x0000FFFF

#define XAUDIOSPEAKERCONFIG_DIGITAL_DOLBYDIGITAL    0x00010000
#define XAUDIOSPEAKERCONFIG_DIGITAL_WMAPRO          0x00020000
#define XAUDIOSPEAKERCONFIG_DIGITAL_MASK            0xFFFF0000

/**************************************************************************
 *
 * Effect identifiers
 *
 **************************************************************************/

#define XAUDIOFXID_SOURCE       0
#define XAUDIOFXID_ROUTER       1
#define XAUDIOFXID_RENDER       2
#define XAUDIOFXID_REVERB       3
#define XAUDIOFXID_COUNT        4

#define XAUDIOFXID_FIRSTCLIENT  XAUDIOFXID_COUNT

/**************************************************************************
 *
 * Source effect parameters
 *
 **************************************************************************/

#define XAUDIOFXPARAMID_SOURCE_VOLUME           0   // Numeric parameter type, get/set
#define XAUDIOFXPARAMID_SOURCE_PITCH            1   // Numeric parameter type, get/set
#define XAUDIOFXPARAMID_SOURCE_FORMAT           2   // Data parameter type, get/set

/**************************************************************************
 *
 * Router effect parameters
 *
 **************************************************************************/

#define XAUDIOFXPARAMID_ROUTER_MODE             0   // Flag parameter type, get/set
#define XAUDIOFXPARAMID_ROUTER_CHANNELMAP       1   // Data parameter type, get/set
#define XAUDIOFXPARAMID_ROUTER_CHANNELVOLUME    2   // Data parameter type, get/set

/**************************************************************************
 *
 * Reverb effect parameters
 *
 **************************************************************************/

#define XAUDIOFXPARAMID_REVERB_ENVIRONMENTSETTINGS  0   // Data parameter type, set only.  Use the native parameter set.
#define XAUDIOFXPARAMID_REVERB_I3DL2SETTINGS        1   // Data parameter type, set only.  Use the I3DL2 parameter set.
#define XAUDIOFXPARAMID_REVERB_WETDRYMIX            2   // Numeric parameter type, get/set.  Range is [0.0, 1.0]

/**************************************************************************
 *
 * XMemAlloc allocation attributes
 *
 **************************************************************************/

#ifdef _XBOX

#define XAUDIOMEMATTR(ObjectType, Alignment, ZeroInitialize, MemoryType) \
    MAKE_XALLOC_ATTRIBUTES \
    ( \
        ObjectType,                             /* ObjectType */ \
        FALSE,                                  /* HeapTracksAttributes */ \
        FALSE,                                  /* MustSucceed */ \
        FALSE,                                  /* FixedSize */ \
        eXALLOCAllocatorId_XAUDIO,              /* AllocatorId */ \
        Alignment,                              /* Alignment */ \
        XALLOC_MEMPROTECT_READWRITE,            /* MemoryProtect */ \
        ZeroInitialize,                         /* ZeroInitialize */ \
        MemoryType                              /* MemoryType */ \
    )

#define XAUDIOMEMATTR_HEAP(ObjectType) \
    XAUDIOMEMATTR \
    ( \
        ObjectType,                             /* ObjectType */ \
        XALLOC_ALIGNMENT_4,                     /* Alignment */ \
        TRUE,                                   /* ZeroInitialize */ \
        XALLOC_MEMTYPE_HEAP                     /* MemoryType */ \
    )

#else // _XBOX

#define XAUDIOMEMATTR_HEAP(type)                type

#endif // XBOX

#define XAUDIOMEMTYPE_INTERNAL          0
#define XAUDIOMEMTYPE_BATCHALLOCATOR    1
#define XAUDIOMEMTYPE_FRAMEBUFFER       2
#define XAUDIOMEMTYPE_EFFECT            3
#define XAUDIOMEMTYPE_EFFECTMANAGER     4
#define XAUDIOMEMTYPE_ENGINE            5
#define XAUDIOMEMTYPE_VOICE             6
#define XAUDIOMEMTYPE_XMADRIVER         7
#define XAUDIOMEMTYPE_EVENT             8

#define XAUDIOMEMATTR_INTERNAL          XAUDIOMEMATTR_HEAP(XAUDIOMEMTYPE_INTERNAL)
#define XAUDIOMEMATTR_BATCHALLOCATOR    XAUDIOMEMATTR_HEAP(XAUDIOMEMTYPE_BATCHALLOCATOR)
#define XAUDIOMEMATTR_FRAMEBUFFER       XAUDIOMEMATTR_HEAP(XAUDIOMEMTYPE_FRAMEBUFFER)
#define XAUDIOMEMATTR_EFFECT            XAUDIOMEMATTR_HEAP(XAUDIOMEMTYPE_EFFECT)
#define XAUDIOMEMATTR_EFFECTMANAGER     XAUDIOMEMATTR_HEAP(XAUDIOMEMTYPE_EFFECTMANAGER)
#define XAUDIOMEMATTR_ENGINE            XAUDIOMEMATTR_HEAP(XAUDIOMEMTYPE_ENGINE)
#define XAUDIOMEMATTR_VOICE             XAUDIOMEMATTR_HEAP(XAUDIOMEMTYPE_VOICE)
#define XAUDIOMEMATTR_EVENT             XAUDIOMEMATTR_HEAP(XAUDIOMEMTYPE_EVENT)

#define XAUDIOMEMATTR_XMADRIVER         XAUDIOMEMATTR(XAUDIOMEMTYPE_XMADRIVER, XALLOC_PHYSICAL_ALIGNMENT_128, FALSE, XALLOC_MEMTYPE_PHYSICAL)

/**************************************************************************
 *
 * Structures
 *
 **************************************************************************/

typedef struct XAUDIOFXINFO
{
    XAUDIODATAFLOW      DataFlow;       // The effect's data flow mask
    XAUDIOTRAILFRAME    TrailFrames;    // The number of frames the effect will process even after no more input data is supplied
    XAUDIOPRIMEFRAME    PrimeFrames;    // The number of frames the effect should be primed with before running
} XAUDIOFXINFO, *LPXAUDIOFXINFO;

typedef const XAUDIOFXINFO *LPCXAUDIOFXINFO;

typedef FLOAT XAUDIOFXNUMERICPARAM, *LPXAUDIOFXNUMERICPARAM;
typedef const XAUDIOFXNUMERICPARAM *LPCXAUDIOFXNUMERICPARAM;

typedef DWORD XAUDIOFXFLAGPARAM, *LPXAUDIOFXFLAGPARAM;
typedef const XAUDIOFXFLAGPARAM *LPCXAUDIOFXFLAGPARAM;

typedef struct XAUDIOFXDATAPARAM
{
    LPVOID  pBuffer;
    DWORD   BufferSize;
} XAUDIOFXDATAPARAM, *LPXAUDIOFXDATAPARAM;

typedef const XAUDIOFXDATAPARAM *LPCXAUDIOFXDATAPARAM;

typedef union XAUDIOFXPARAM
{
    XAUDIOFXNUMERICPARAM    Value;
    XAUDIOFXFLAGPARAM       Flags;
    XAUDIOFXDATAPARAM       Data;
} XAUDIOFXPARAM, *LPXAUDIOFXPARAM;

typedef const XAUDIOFXPARAM *LPCXAUDIOFXPARAM;

typedef struct XAUDIOXMAFORMAT
{
    XAUDIOSAMPLERATE    SampleRate;     // Sample rate, in Hz
    XAUDIOCHANNEL       ChannelCount;   // Number of channels
} XAUDIOXMAFORMAT, *LPXAUDIOXMAFORMAT;

typedef const XAUDIOXMAFORMAT *LPCXAUDIOXMAFORMAT;

typedef struct XAUDIOSOURCEFORMAT
{
    XAUDIOSAMPLETYPE                SampleType;                         // Sample data format

    union 
    {
        struct
        {
            XAUDIOXMASTREAMCOUNT    NumStreams;                         // Number of interleaved XMA streams
            XAUDIOXMAFORMAT         Stream[XAUDIOXMASTREAMCOUNT_MAX];   // XMA streams
        };

        struct
        {
            XAUDIOCHANNEL           ChannelCount;                       // Number of channels
            XAUDIOSAMPLERATE        SampleRate;                         // Sample rate, in Hz
        };
    };
} XAUDIOSOURCEFORMAT, *LPXAUDIOSOURCEFORMAT;

typedef const XAUDIOSOURCEFORMAT *LPCXAUDIOSOURCEFORMAT;

typedef union XAUDIOFORMAT
{
    struct
    {
        XAUDIOSAMPLETYPE    SampleType;         // Sample data format
        XAUDIOCHANNEL       ChannelCount;       // Number of channels
        XAUDIOSAMPLERATE    SampleRate;         // Sample rate, in Hz
    };

    ULONGLONG               UnionValue;         // Unionized value for optimized parameter passing
} XAUDIOFORMAT, *LPXAUDIOFORMAT;

typedef const XAUDIOFORMAT *LPCXAUDIOFORMAT;

typedef struct XMALOOPREGION
{
    DWORD   LoopStart;
    DWORD   LoopEnd;
    BYTE    LoopSubframeEnd;
    BYTE    LoopSubframeSkip;
} XMALOOPREGION, *LPXMALOOPREGION;

typedef const XMALOOPREGION* LPCXMALOOPREGION;

typedef struct XAUDIOPACKET
{
    LPVOID              pBuffer;
    DWORD               BufferSize;
    DWORD               LoopCount;

    union
    {
        struct
        {
            DWORD       LoopStart;
            DWORD       LoopLength;
        };

        XMALOOPREGION   XMALoop[XAUDIOXMASTREAMCOUNT_MAX];
    };

    LPVOID              pContext;
} XAUDIOPACKET, *LPXAUDIOPACKET;

typedef const XAUDIOPACKET *LPCXAUDIOPACKET;

typedef struct XMAPACKETPOSITION
{
    DWORD               ReadOffset;
    WORD                SampleIndex;
} XMAPACKETPOSITION, *LPXMAPACKETPOSITION;

typedef const XMAPACKETPOSITION *LPCXMAPACKETPOSITION;

typedef union XAUDIOPACKETPOSITION
{
    DWORD               Position;
    XMAPACKETPOSITION   XMAPosition[XAUDIOXMASTREAMCOUNT_MAX];
} XAUDIOPACKETPOSITION, *LPXAUDIOPACKETPOSITION;

typedef const XAUDIOPACKETPOSITION *LPCXAUDIOPACKETPOSITION;

typedef struct XAUDIOCHANNELMAPENTRY
{
    XAUDIOCHANNEL   InputChannel;
    XAUDIOCHANNEL   OutputChannel;
    XAUDIOVOLUME    Volume;
} XAUDIOCHANNELMAPENTRY, *LPXAUDIOCHANNELMAPENTRY;

typedef const XAUDIOCHANNELMAPENTRY *LPCXAUDIOCHANNELMAPENTRY;

typedef struct XAUDIOCHANNELMAP
{
    XAUDIOCHANNEL           EntryCount;
    LPXAUDIOCHANNELMAPENTRY paEntries;
} XAUDIOCHANNELMAP, *LPXAUDIOCHANNELMAP;

typedef const XAUDIOCHANNELMAP *LPCXAUDIOCHANNELMAP;

typedef struct XAUDIOCHANNELVOLUMEENTRY
{
    XAUDIOCHANNEL   EntryIndex;
    XAUDIOVOLUME    Volume;
} XAUDIOCHANNELVOLUMEENTRY, *LPXAUDIOCHANNELVOLUMEENTRY;

typedef const XAUDIOCHANNELVOLUMEENTRY *LPCXAUDIOCHANNELVOLUMEENTRY;

typedef struct XAUDIOCHANNELVOLUME
{
    XAUDIOCHANNEL               EntryCount;
    LPXAUDIOCHANNELVOLUMEENTRY  paEntries;
} XAUDIOCHANNELVOLUME, *LPXAUDIOCHANNELVOLUME;

typedef const XAUDIOCHANNELVOLUME *LPCXAUDIOCHANNELVOLUME;

typedef struct XAUDIOFRAMEBUFINIT
{
    XAUDIOFRAMEBUFTYPE      Type;
    XAUDIOFORMAT            MaxFormat;
    LPXAUDIOSAMPLE          pSampleBuffer;
    LPVOID                  pContext;
} XAUDIOFRAMEBUFINIT, *LPXAUDIOFRAMEBUFINIT;

typedef const XAUDIOFRAMEBUFINIT *LPCXAUDIOFRAMEBUFINIT;

typedef struct XAUDIOFRAMEBUFDATA
{
    XAUDIOFORMAT            Format;
    LPXAUDIOSAMPLE          pSampleBuffer;
} XAUDIOFRAMEBUFDATA, *LPXAUDIOFRAMEBUFDATA;

typedef const XAUDIOFRAMEBUFDATA *LPCXAUDIOFRAMEBUFDATA;

typedef struct XAUDIOFXINIT
{
    XAUDIOFXID              EffectId;
    LPVOID                  pContext;
} XAUDIOFXINIT, *LPXAUDIOFXINIT;

typedef const XAUDIOFXINIT *LPCXAUDIOFXINIT;

typedef struct XAUDIOROUTERFXINIT
{
    XAUDIOFXINIT            EffectHeader;
    XAUDIOCHANNEL           ChannelMapEntryCount;
    XAUDIOROUTERFXMODE      Mode;
    LPCXAUDIOCHANNELMAP     pChannelMap;
} XAUDIOROUTERFXINIT, *LPXAUDIOROUTERFXINIT;

typedef const XAUDIOROUTERFXINIT *LPCXAUDIOROUTERFXINIT;

typedef struct XAUDIOSOURCEFXPACKETCALLBACK
{
    LPVOID                  pEffectContext;
    LPVOID                  pPacketContext;
    XAUDIOPACKETSTATUS      Status;
} XAUDIOSOURCEFXPACKETCALLBACK, *LPXAUDIOSOURCEFXPACKETCALLBACK;

typedef const XAUDIOSOURCEFXPACKETCALLBACK *LPCXAUDIOSOURCEFXPACKETCALLBACK;

typedef void (CALLBACK *LPFNXAUDIOSOURCEPACKETCALLBACK)(LPCXAUDIOSOURCEFXPACKETCALLBACK pCallbackData);

typedef struct XAUDIOSOURCEFXINIT
{
    XAUDIOFXINIT                    EffectHeader;
    XAUDIOSOURCEFORMAT              Format;
    XAUDIOPACKETINDEX               MaxPacketCount;
    XAUDIOFREQSCALE                 MaxFreqScale;   // (0|1)= no upward scaling possible, 2 = one octave up, 4 = two octaves...
    XAUDIOSOURCEFLAGS               Flags;
    LPFNXAUDIOSOURCEPACKETCALLBACK  pfnPacketCompletionCallback;
    LPFNXAUDIOSOURCEPACKETCALLBACK  pfnPacketLoopCallback;
} XAUDIOSOURCEFXINIT, *LPXAUDIOSOURCEFXINIT;

typedef const XAUDIOSOURCEFXINIT *LPCXAUDIOSOURCEFXINIT;

typedef struct XAUDIOSOURCEFXFORMATPARAM
{
    XAUDIOSOURCEFORMAT  Format;
    XAUDIOSOURCEFLAGS   Flags;
} XAUDIOSOURCEFXFORMATPARAM, *LPXAUDIOSOURCEFXFORMATPARAM;

typedef const XAUDIOSOURCEFXFORMATPARAM *LPCXAUDIOSOURCEFXFORMATPARAM;

typedef struct XAUDIOREVERBSETTINGS
{
    // Delay times
    DWORD       ReflectionsDelay;           // [0, 300] in ms
    BYTE        ReverbDelay;                // [0, 85] in ms
    BYTE        RearDelay;                  // [0, 5] in ms

    // Indexed parameters
    BYTE        PositionLeft;               // [0, 30] no units
    BYTE        PositionRight;              // [0, 30] no units
    BYTE        PositionMatrixLeft;         // [0, 30] no units
    BYTE        PositionMatrixRight;        // [0, 30] no units
    BYTE        EarlyDiffusion;             // [0, 15] no units
    BYTE        LateDiffusion;              // [0, 15] no units
    BYTE        LowEQGain;                  // [0, 12] no units
    BYTE        LowEQCutoff;                // [0, 9] no units
    BYTE        HighEQGain;                 // [0, 8] no units
    BYTE        HighEQCutoff;               // [0, 14] no units

    // Direct parameters
    FLOAT       RoomFilterFreq;             // [20, 20000] in Hz
    FLOAT       RoomFilterMain;             // [-100, 0] in dB
    FLOAT       RoomFilterHF;               // [-100, 0] in dB
    FLOAT       ReflectionsGain;            // [-100, 20] in dB
    FLOAT       ReverbGain;                 // [-100, 20] in dB
    FLOAT       DecayTime;                  // [0.1, inf] in seconds
    FLOAT       Density;                    // [0, 1] (percentage)
    FLOAT       RoomSize;                   // [1, 12] in meters
} XAUDIOREVERBSETTINGS, *LPXAUDIOREVERBSETTINGS;

typedef const XAUDIOREVERBSETTINGS *LPCXAUDIOREVERBSETTINGS;

typedef struct XAUDIOREVERBI3DL2SETTINGS
{
    LONG        lRoom;                      // [-10000, 0] in mB (hundredths of decibels)
    LONG        lRoomHF;                    // [-10000, 0] in mB (hundredths of decibels)
    FLOAT       flRoomRolloffFactor;        // [0.0, 10.0]
    FLOAT       flDecayTime;                // [0.1, 20.0] in seconds
    FLOAT       flDecayHFRatio;             // [0.1, 2.0]
    LONG        lReflections;               // [-10000, 1000] in mB (hundredths of decibels)
    FLOAT       flReflectionsDelay;         // [0.0, 0.3] in seconds
    LONG        lReverb;                    // [-10000, 2000] in mB (hundredths of decibels)
    FLOAT       flReverbDelay;              // [0.0, 0.1] in seconds
    FLOAT       flDiffusion;                // [0.0, 100.0] (percentage)
    FLOAT       flDensity;                  // [0.0, 100.0] (percentage)
    FLOAT       flHFReference;              // [20.0, 20000.0] in Hz
} XAUDIOREVERBI3DL2SETTINGS, *LPXAUDIOREVERBI3DL2SETTINGS;

typedef const XAUDIOREVERBI3DL2SETTINGS *LPCXAUDIOREVERBI3DL2SETTINGS;

typedef struct XAUDIORENDERFXCALLBACK
{
    LPVOID                      pEffectContext;
} XAUDIORENDERFXCALLBACK, *LPXAUDIORENDERFXCALLBACK;

typedef const XAUDIORENDERFXCALLBACK *LPCXAUDIORENDERFXCALLBACK;

typedef void (CALLBACK *LPFNXAUDIORENDERFXCALLBACK)(LPCXAUDIORENDERFXCALLBACK pCallbackData);

typedef struct XAUDIORENDERFXINIT
{
    XAUDIOFXINIT                EffectHeader;
    LPFNXAUDIORENDERFXCALLBACK  pfnRenderCallback;
} XAUDIORENDERFXINIT, *LPXAUDIORENDERFXINIT;

typedef const XAUDIORENDERFXINIT *LPCXAUDIORENDERFXINIT;

typedef HRESULT (STDAPICALLTYPE *LPFNXAUDIOQUERYFXSIZE)(LPCXAUDIOFXINIT pInit, LPDWORD pEffectSize);
typedef HRESULT (STDAPICALLTYPE *LPFNXAUDIOCREATEFX)(LPCXAUDIOFXINIT pInit, IXAudioBatchAllocator *pAllocator, IXAudioEffect **ppEffect);

typedef struct XAUDIOFXTABLEENTRY
{
    XAUDIOFXID              EffectId;
    LPFNXAUDIOQUERYFXSIZE   pfnQueryEffectSize;
    LPFNXAUDIOCREATEFX      pfnCreateEffect;
} XAUDIOFXTABLEENTRY, *LPXAUDIOFXTABLEENTRY;

typedef const XAUDIOFXTABLEENTRY *LPCXAUDIOFXTABLEENTRY;

typedef struct XAUDIOFXTABLE
{
    XAUDIOFXID              EffectCount;
    LPCXAUDIOFXTABLEENTRY   paEffects;
} XAUDIOFXTABLE, *LPXAUDIOFXTABLE;

typedef const XAUDIOFXTABLE *LPCXAUDIOFXTABLE;

typedef struct XAUDIOFXREGISTER
{
    XAUDIOFXID              EffectCount;
    LPXAUDIOFXTABLEENTRY    paEffects;
} XAUDIOFXREGISTER, *LPXAUDIOFXREGISTER;

typedef const XAUDIOFXREGISTER *LPCXAUDIOFXREGISTER;

typedef struct XAUDIOFXUNREGISTER
{
    XAUDIOFXID              EffectCount;
    LPCXAUDIOFXID           paEffectIds;
} XAUDIOFXUNREGISTER, *LPXAUDIOFXUNREGISTER;

typedef const XAUDIOFXUNREGISTER *LPCXAUDIOFXUNREGISTER;

typedef struct XAUDIOFXMGRINIT
{
    LPCXAUDIOFXTABLE        pEffectTable;
} XAUDIOFXMGRINIT, *LPXAUDIOFXMGRINIT;

typedef const XAUDIOFXMGRINIT *LPCXAUDIOFXMGRINIT;

typedef struct XAUDIOVOICEFXCHAIN
{
    XAUDIOVOICEFXINDEX      EffectCount;
    LPCXAUDIOFXINIT *       papEffects;
} XAUDIOVOICEFXCHAIN, *LPXAUDIOVOICEFXCHAIN;

typedef const XAUDIOVOICEFXCHAIN *LPCXAUDIOVOICEFXCHAIN;

typedef struct XAUDIOVOICEOUTPUTENTRY
{
    IXAudioVoice *      pDestVoice;
    LPXAUDIOCHANNELMAP  pChannelMap;
} XAUDIOVOICEOUTPUTENTRY, *LPXAUDIOVOICEOUTPUTENTRY;

typedef const XAUDIOVOICEOUTPUTENTRY *LPCXAUDIOVOICEOUTPUTENTRY;

typedef struct XAUDIOVOICEOUTPUT
{
    XAUDIOVOICEINDEX            EntryCount;
    LPXAUDIOVOICEOUTPUTENTRY    paEntries;
} XAUDIOVOICEOUTPUT, *LPXAUDIOVOICEOUTPUT;

typedef const XAUDIOVOICEOUTPUT *LPCXAUDIOVOICEOUTPUT;

typedef struct XAUDIOVOICEOUTPUTVOLUMEENTRY
{
    XAUDIOVOICEINDEX        OutputVoiceIndex;
    LPXAUDIOCHANNELVOLUME   pVolume;
} XAUDIOVOICEOUTPUTVOLUMEENTRY, *LPXAUDIOVOICEOUTPUTVOLUMEENTRY;

typedef const XAUDIOVOICEOUTPUTVOLUMEENTRY *LPCXAUDIOVOICEOUTPUTVOLUMEENTRY;

typedef struct XAUDIOVOICEOUTPUTVOLUME
{
    XAUDIOVOICEINDEX                EntryCount;
    LPXAUDIOVOICEOUTPUTVOLUMEENTRY  paEntries;
} XAUDIOVOICEOUTPUTVOLUME, *LPXAUDIOVOICEOUTPUTVOLUME;

typedef const XAUDIOVOICEOUTPUTVOLUME *LPCXAUDIOVOICEOUTPUTVOLUME;

typedef struct XAUDIOVOICEPROCESSCALLBACK
{
    LPVOID                  pVoiceContext;
} XAUDIOVOICEPROCESSCALLBACK, *LPXAUDIOVOICEPROCESSCALLBACK;

typedef const XAUDIOVOICEPROCESSCALLBACK *LPCXAUDIOVOICEPROCESSCALLBACK;

typedef void (CALLBACK *LPFNXAUDIOVOICEPROCESSCALLBACK)(LPCXAUDIOVOICEPROCESSCALLBACK pCallbackData);

typedef struct XAUDIOVOICEPACKETCALLBACK
{
    LPVOID                  pVoiceContext;
    LPVOID                  pPacketContext;
    XAUDIOPACKETSTATUS      Status;
} XAUDIOVOICEPACKETCALLBACK, *LPXAUDIOVOICEPACKETCALLBACK;

typedef const XAUDIOVOICEPACKETCALLBACK *LPCXAUDIOVOICEPACKETCALLBACK;

typedef void (CALLBACK *LPFNXAUDIOVOICEPACKETCALLBACK)(LPCXAUDIOVOICEPACKETCALLBACK pCallbackData);

typedef struct XAUDIOSOURCEVOICEINIT
{
    XAUDIOSOURCEFORMAT              Format;
    XAUDIOVOICECATEGORY             Category;
    XAUDIOVOICEINDEX                MaxOutputVoiceCount;
    XAUDIOCHANNEL                   MaxChannelMapEntryCount;
    XAUDIOPACKETINDEX               MaxPacketCount;
    XAUDIOFREQSCALE                 MaxFreqScale;
    XAUDIOSOURCEFLAGS               Flags;
    LPCXAUDIOVOICEFXCHAIN           pEffectChain;
    LPCXAUDIOVOICEOUTPUT            pVoiceOutput;
    LPFNXAUDIOVOICEPROCESSCALLBACK  pfnProcessCallback;
    LPFNXAUDIOVOICEPACKETCALLBACK   pfnPacketCompletionCallback;
    LPFNXAUDIOVOICEPACKETCALLBACK   pfnPacketLoopCallback;
    LPVOID                          pContext;
} XAUDIOSOURCEVOICEINIT, *LPXAUDIOSOURCEVOICEINIT;

typedef const XAUDIOSOURCEVOICEINIT *LPCXAUDIOSOURCEVOICEINIT;

typedef struct XAUDIOSUBMIXVOICEINIT
{
    XAUDIOFORMAT                    Format;
    XAUDIOVOICEINDEX                MaxOutputVoiceCount;
    XAUDIOCHANNEL                   MaxChannelMapEntryCount;
    XAUDIOSUBMIXSTAGE               SubmixStage;
    LPCXAUDIOVOICEFXCHAIN           pEffectChain;
    LPCXAUDIOVOICEOUTPUT            pVoiceOutput;
    LPFNXAUDIOVOICEPROCESSCALLBACK  pfnProcessCallback;
    LPVOID                          pContext;
} XAUDIOSUBMIXVOICEINIT, *LPXAUDIOSUBMIXVOICEINIT;

typedef const XAUDIOSUBMIXVOICEINIT *LPCXAUDIOSUBMIXVOICEINIT;

typedef struct XAUDIOMASTERINGVOICEINIT
{
    LPCXAUDIOVOICEFXCHAIN           pEffectChain;
    LPFNXAUDIOVOICEPROCESSCALLBACK  pfnProcessCallback;
    LPVOID                          pContext;
} XAUDIOMASTERINGVOICEINIT, *LPXAUDIOMASTERINGVOICEINIT;

typedef const XAUDIOMASTERINGVOICEINIT *LPCXAUDIOMASTERINGVOICEINIT;

typedef struct XAUDIOENGINEINIT
{
    XAUDIOCHANNEL                   MaxVoiceChannelCount;
    XAUDIOSUBMIXSTAGE               SubmixStageCount;
    LPCXAUDIOFXTABLE                pEffectTable;
    LPCXAUDIOMASTERINGVOICEINIT     pMasteringVoiceInit;
} XAUDIOENGINEINIT, *LPXAUDIOENGINEINIT;

typedef const XAUDIOENGINEINIT *LPCXAUDIOENGINEINIT;

typedef void (CALLBACK *LPFNXAUDIOENGINECALLBACK)(LPVOID pContext);

typedef struct XAUDIOPERFCOUNTER
{
    ULONGLONG           MinTicks;       // Shortest time
    ULONGLONG           AvgTicks;       // Average time
    ULONGLONG           MaxTicks;       // Longest time
    ULONGLONG           QueryTicks;     // Total number of CPU ticks since the last query
    ULONGLONG           TicksPerSecond; // Tick count frequency
    ULONGLONG           FrameCount;     // Number of frames processed
} XAUDIOPERFCOUNTER, *LPXAUDIOPERFCOUNTER;

typedef const XAUDIOPERFCOUNTER *LPCXAUDIOPERFCOUNTER;

/**************************************************************************
 *
 * Predefined reverb effects using the I3DL2 parameter set
 *
 **************************************************************************/

#define I3DL2_ENVIRONMENT_PRESET_DEFAULT \
-10000,    0,0.0f, 1.00f,0.50f,-10000,0.020f,-10000,0.040f,100.0f,100.0f,5000.0f

#define I3DL2_ENVIRONMENT_PRESET_GENERIC \
 -1000, -100,0.0f, 1.49f,0.83f, -2602,0.007f,   200,0.011f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_PADDEDCELL \
 -1000,-6000,0.0f, 0.17f,0.10f, -1204,0.001f,   207,0.002f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_ROOM \
 -1000, -454,0.0f, 0.40f,0.83f, -1646,0.002f,    53,0.003f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_BATHROOM \
 -1000,-1200,0.0f, 1.49f,0.54f,  -370,0.007f,  1030,0.011f,100.0f, 60.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_LIVINGROOM \
 -1000,-6000,0.0f, 0.50f,0.10f, -1376,0.003f, -1104,0.004f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_STONEROOM \
 -1000, -300,0.0f, 2.31f,0.64f,  -711,0.012f,    83,0.017f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_AUDITORIUM \
 -1000, -476,0.0f, 4.32f,0.59f,  -789,0.020f,  -289,0.030f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_CONCERTHALL \
 -1000, -500,0.0f, 3.92f,0.70f, -1230,0.020f,    -2,0.029f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_CAVE \
 -1000,    0,0.0f, 2.91f,1.30f,  -602,0.015f,  -302,0.022f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_ARENA \
 -1000, -698,0.0f, 7.24f,0.33f, -1166,0.020f,    16,0.030f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_HANGAR \
 -1000,-1000,0.0f,10.05f,0.23f,  -602,0.020f,   198,0.030f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_CARPETEDHALLWAY \
 -1000,-4000,0.0f, 0.30f,0.10f, -1831,0.002f, -1630,0.030f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_HALLWAY \
 -1000, -300,0.0f, 1.49f,0.59f, -1219,0.007f,   441,0.011f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR \
 -1000, -237,0.0f, 2.70f,0.79f, -1214,0.013f,   395,0.020f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_ALLEY \
 -1000, -270,0.0f, 1.49f,0.86f, -1204,0.007f,    -4,0.011f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_FOREST \
 -1000,-3300,0.0f, 1.49f,0.54f, -2560,0.162f,  -613,0.088f, 79.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_CITY \
 -1000, -800,0.0f, 1.49f,0.67f, -2273,0.007f, -2217,0.011f, 50.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_MOUNTAINS \
 -1000,-2500,0.0f, 1.49f,0.21f, -2780,0.300f, -2014,0.100f, 27.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_QUARRY \
 -1000,-1000,0.0f, 1.49f,0.83f,-10000,0.061f,   500,0.025f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_PLAIN \
 -1000,-2000,0.0f, 1.49f,0.50f, -2466,0.179f, -2514,0.100f, 21.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_PARKINGLOT \
 -1000,    0,0.0f, 1.65f,1.50f, -1363,0.008f, -1153,0.012f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_SEWERPIPE \
 -1000,-1000,0.0f, 2.81f,0.14f,   429,0.014f,   648,0.021f, 80.0f, 60.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_UNDERWATER \
 -1000,-4000,0.0f, 1.49f,0.10f,  -449,0.007f,  1700,0.011f,100.0f,100.0f,5000.0f

#define I3DL2_ENVIRONMENT_PRESET_SMALLROOM \
 -1000, -600,0.0f, 1.10f,0.83f,  -400,0.005f,   500,0.010f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_MEDIUMROOM \
 -1000, -600,0.0f, 1.30f,0.83f, -1000,0.010f,  -200,0.020f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_LARGEROOM \
 -1000, -600,0.0f, 1.50f,0.83f, -1600,0.020f, -1000,0.040f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_MEDIUMHALL \
 -1000, -600,0.0f, 1.80f,0.70f, -1300,0.015f,  -800,0.030f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_LARGEHALL \
 -1000, -600,0.0f, 1.80f,0.70f, -2000,0.030f, -1400,0.060f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_PLATE \
 -1000, -200,0.0f, 1.30f,0.90f,     0,0.002f,     0,0.010f,100.0f, 75.0f,5000.0f

/**************************************************************************
 *
 * Legacy types
 *
 **************************************************************************/

#ifndef WAVE_FORMAT_PCM
#define WAVE_FORMAT_PCM 1

#pragma pack(push, 1)

typedef struct waveformat_tag
{
    WORD    wFormatTag;
    WORD    nChannels;
    DWORD   nSamplesPerSec;
    DWORD   nAvgBytesPerSec;
    WORD    nBlockAlign;
} WAVEFORMAT, *PWAVEFORMAT, *NPWAVEFORMAT, *LPWAVEFORMAT;

typedef const WAVEFORMAT *LPCWAVEFORMAT;

typedef struct pcmwaveformat_tag
{
    WAVEFORMAT  wf;
    WORD        wBitsPerSample;
} PCMWAVEFORMAT, *PPCMWAVEFORMAT, *NPPCMWAVEFORMAT, *LPPCMWAVEFORMAT;

typedef const PCMWAVEFORMAT *LPCPCMWAVEFORMAT;

#pragma pack(pop)

#endif // WAVE_FORMAT_PCM

#ifndef _WAVEFORMATEX_
#define _WAVEFORMATEX_

#pragma pack(push, 1)

typedef struct tWAVEFORMATEX
{
    WORD    wFormatTag;
    WORD    nChannels;
    DWORD   nSamplesPerSec;
    DWORD   nAvgBytesPerSec;
    WORD    nBlockAlign;
    WORD    wBitsPerSample;
    WORD    cbSize;
} WAVEFORMATEX, *PWAVEFORMATEX, *NPWAVEFORMATEX, *LPWAVEFORMATEX;

typedef const WAVEFORMATEX *LPCWAVEFORMATEX;

#pragma pack(pop)

#endif // _WAVEFORMATEX_

/**************************************************************************
 *
 * XMA Data
 *
 * The seek table is in the RIFF chunk "seek"
 *
 * Format of seek table:
 *
 * DWORD    Number of streams
 * DWORD    Stream 0: Number of table entries
 * DWORD    Stream 0: Entry 0
 * DWORD    Stream 0: Entry 1
 * ...
 * DWORD    Stream 0: Entry n-1
 * ...
 * DWORD    Stream n-1: Number of table entries
 * DWORD    Stream n-1: Entry 0
 * DWORD    Stream n-1: Entry 1
 * ...
 * DWORD    Stream n-1: Entry n-1
 *
 **************************************************************************/

#define XMA_INVALIDSAMPLECOUNT      384     // Count of invalid decoded samples at the beginning of the stream
#define XMA_PACKETSIZEINBYTES       2048    // XMA packet size in bytes
#define XMA_PACKETSIZEINBITS        (XMA_PACKETSIZEINBYTES * 8)
#define XMA_FRAMESIZEINSAMPLES      512     // XMA frame size in decoded samples

#ifndef WAVE_FORMAT_XMA
#define  WAVE_FORMAT_XMA 0x0165

//
// ChannelMask values
//

#define XMA_SPEAKER_LEFT            0x01
#define XMA_SPEAKER_RIGHT           0x02
#define XMA_SPEAKER_CENTER          0x04
#define XMA_SPEAKER_LFE             0x08
#define XMA_SPEAKER_LEFT_SURROUND   0x10
#define XMA_SPEAKER_RIGHT_SURROUND  0x20
#define XMA_SPEAKER_LEFT_BACK       0x40
#define XMA_SPEAKER_RIGHT_BACK      0x80

typedef struct XMASTREAMFORMAT
{
    DWORD   PsuedoBytesPerSec;  // Used by encoder
    DWORD   SampleRate;         // Sample rate for the stream.
    DWORD   LoopStart;          // Loop start offset (in bits).
    DWORD   LoopEnd;            // Loop end offset (in bits).

    // Format for SubframeData: eeee ssss.
    // e: Subframe number of loop end point [0,3].
    // s: Number of subframes to skip before decoding and outputting at the loop start point [1,4].

    BYTE    SubframeData;       // Data for decoding subframes.  See above.
    BYTE    Channels;           // Number of channels in the stream (1 or 2).
    WORD    ChannelMask;        // Channel assignments for the channels in the stream (same as
                                // lower 16 bits of dwChannelMask in WAVEFORMATEXTENSIBLE).
} XMASTREAMFORMAT, *PXMASTREAMFORMAT, *NPXMASTREAMFORMAT, *LPXMASTREAMFORMAT;
typedef const XMASTREAMFORMAT *LPCXMASTREAMFORMAT;

typedef struct XMAWAVEFORMAT
{
    WORD            FormatTag;     // Audio format type (always WAVE_FORMAT_XMA).
    WORD            BitsPerSample; // Bit depth (currently required to be 16).
    WORD            EncodeOptions; // Options for XMA encoder/decoder.
    WORD            LargestSkip;   // Largest skip used in interleaving streams.
    WORD            NumStreams;    // Number of interleaved audio streams.
    BYTE            LoopCount;     // Number of loop repetitions (255 == infinite).
    BYTE            Version;       // Version of the encoder that generated this.
    XMASTREAMFORMAT XmaStreams[1]; // Format info for each stream (can grow based on wNumStreams).
} XMAWAVEFORMAT, *PXMAWAVEFORMAT, *NPXMAWAVEFORMAT, *LPXMAWAVEFORMAT;
typedef XMAWAVEFORMAT *LPCXMAWAVEFORMAT;

#endif // #ifndef WAVE_FORMAT_XMA

STDAPI XAudioGetXMAPacketPosition(DWORD SampleIndex, const DWORD* pSeekTable, const VOID* pvBuffer, XMAPACKETPOSITION* pPosition);

/**************************************************************************
 *
 * IXAudioRefCount
 *
 **************************************************************************/

#undef INTERFACE
#define INTERFACE IXAudioRefCount

DECLARE_INTERFACE(IXAudioRefCount)
{
    // IXAudioRefCount
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;
};

/**************************************************************************
 *
 * IXAudioBatchAllocator
 *
 **************************************************************************/

#undef INTERFACE
#define INTERFACE IXAudioBatchAllocator

DECLARE_INTERFACE_(IXAudioBatchAllocator, IXAudioRefCount)
{
    // IXAudioRefCount
    STDMETHOD_(DWORD, AddRef)(THIS) PURE;
    STDMETHOD_(DWORD, Release)(THIS) PURE;

    // IXAudioBatchAllocator
    STDMETHOD_(void, GrowHeap)(THIS_ DWORD BufferSize) PURE;
    STDMETHOD(CreateHeap)(THIS_ DWORD AllocAttributes) PURE;
    STDMETHOD_(DWORD, GetFreeHeapSize)(THIS) PURE;
    STDMETHOD_(LPVOID, Alloc)(THIS_ DWORD BufferSize) PURE;
};

STDAPI XAudioCreateBatchAllocator(DWORD AllocAttributes, DWORD BufferSize, IXAudioBatchAllocator **ppBatchAllocator);

/**************************************************************************
 *
 * Global XAudio synchronization lock
 *
 **************************************************************************/

STDAPI XAudioLock(XAUDIOLOCKTYPE LockType);

/**************************************************************************
 *
 * Synchronization events
 *
 **************************************************************************/

STDAPI_(XAUDIOEVENT) XAudioCreateEvent(BOOL ManualReset, BOOL InitialState);
STDAPI_(void) XAudioFreeEvent(XAUDIOEVENT Event);
STDAPI_(void) XAudioSetEvent(XAUDIOEVENT Event);
STDAPI_(void) XAudioResetEvent(XAUDIOEVENT Event);
STDAPI_(void) XAudioPulseEvent(XAUDIOEVENT Event);
STDAPI_(DWORD) XAudioWaitForSingleObject(XAUDIOEVENT Event, DWORD Milliseconds, BOOL Alertable);
STDAPI_(DWORD) XAudioWaitForMultipleObjects(DWORD Count, LPCXAUDIOEVENT paEvents, BOOL WaitForAll, DWORD Milliseconds, BOOL Alertable);

/**************************************************************************
 *
 * Speaker configuration
 *
 **************************************************************************/

#ifdef _XBOX

STDAPI XAudioGetSpeakerConfig(LPXAUDIOSPEAKERCONFIG pConfig);

#endif // _XBOX

/**************************************************************************
 *
 * Miscelaneous helpers
 *
 **************************************************************************/

__forceinline XAUDIOVOLUME XAudioDecibelsToVolume(FLOAT Volume)
{
    return (XAUDIOVOLUME)pow(10.0, (DOUBLE)Volume / 20.0);
}

__forceinline FLOAT XAudioVolumeToDecibels(XAUDIOVOLUME Volume)
{
    return (FLOAT)(20.0 * log10((DOUBLE)Volume));
}

__forceinline XAUDIOVOLUME XAudioMillibelsToVolume(LONG Volume)
{
    return (XAUDIOVOLUME)pow(10.0, (DOUBLE)Volume / 2000.0);
}

__forceinline LONG XAudioVolumeToMillibels(XAUDIOVOLUME Volume)
{
    return (LONG)(2000.0 * log10((DOUBLE)Volume));
}

__forceinline XAUDIOSAMPLERATE XAudioPitchToSampleRate(XAUDIOSAMPLERATE BaseSampleRate, XAUDIOPITCH Pitch)
{
    return (XAUDIOSAMPLERATE)((DOUBLE)BaseSampleRate * pow(2.0, (DOUBLE)Pitch));
}

__forceinline XAUDIOPITCH XAudioSampleRateToPitch(XAUDIOSAMPLERATE BaseSampleRate, XAUDIOSAMPLERATE SampleRate)
{
    return (XAUDIOPITCH)(log((DOUBLE)BaseSampleRate / (DOUBLE)SampleRate) / 0.69314718055994529);
}

__forceinline void XAudioConvertSampleShortToNative(LPXAUDIOSAMPLE pOutputSample, SHORT InputSample)
{
    *pOutputSample = (XAUDIOSAMPLE)InputSample / 32768.0f;
}

__forceinline void XAudioConvertSampleNativeToShort(SHORT *pOutputSample, XAUDIOSAMPLE InputSample)
{
    const LONG              LongSample = (LONG)(InputSample * 32768.0f);

    if(LongSample >= 32767)
    {
        *pOutputSample = 32767;
    }
    else if(LongSample <= -32768)
    {
        *pOutputSample = -32738;
    }
    else
    {
        *pOutputSample = (SHORT)LongSample;
    }
}

__forceinline void XAudioConvertSampleByteToNative(LPXAUDIOSAMPLE pOutputSample, BYTE InputSample)
{
    *pOutputSample = ((XAUDIOSAMPLE)InputSample - 128.0f) / 128.0f;
}

__forceinline void XAudioConvertSampleNativeToByte(BYTE *pOutputSample, XAUDIOSAMPLE InputSample)
{
    const LONG              LongSample = (LONG)(InputSample * 128.0f) + 128;

    if(LongSample >= 255)
    {
        *pOutputSample = 255;
    }
    else if(LongSample <= 0)
    {
        *pOutputSample = 0;
    }
    else
    {
        *pOutputSample = (BYTE)LongSample;
    }
}

#ifdef __cplusplus

__forceinline void XAudioConvertSample(LPXAUDIOSAMPLE pOutputSample, XAUDIOSAMPLE InputSample)
{
    *pOutputSample = InputSample;
}

__forceinline void XAudioConvertSample(LPXAUDIOSAMPLE pOutputSample, SHORT InputSample)
{
    XAudioConvertSampleShortToNative(pOutputSample, InputSample);
}

__forceinline void XAudioConvertSample(SHORT *pOutputSample, XAUDIOSAMPLE InputSample)
{
    XAudioConvertSampleNativeToShort(pOutputSample, InputSample);
}

__forceinline void XAudioConvertSample(LPXAUDIOSAMPLE pOutputSample, BYTE InputSample)
{
    XAudioConvertSampleByteToNative(pOutputSample, InputSample);
}

__forceinline void XAudioConvertSample(BYTE *pOutputSample, XAUDIOSAMPLE InputSample)
{
    XAudioConvertSampleNativeToByte(pOutputSample, InputSample);
}

#endif // __cplusplus

/**************************************************************************
 *
 * These macros are included to aid effect developers in properly
 * allocating and freeing their effect objects.  Include the
 * XAUDIO_USE_BATCHALLOC_NEWDELETE macro in your class declaration and use
 * XAUDIO_BATCHALLOC_NEW to allocate instances of your effect class.
 *
 * When doing size calculations, remember that using new[] (and therefore,
 * XAUDIO_BATCHALLOC_NEWA) adds a size_t value to the front of the
 * allocation.  If you plan to use XAUDIO_BATCHALLOC_NEWA to allocate an
 * array of objects, add sizeof(size_t) to the batch allocator's heap size
 * before calling IXAudioBatchAllocator::Alloc.
 *
 **************************************************************************/

#define XAUDIO_BATCHALLOC_ALLOC(Type, Count, pAllocator) \
    ((Type *)pAllocator->Alloc(sizeof(Type) * (Count)))

#define XAUDIO_BATCHALLOC_FREE(pBuffer) \
    pBuffer = NULL

#ifdef __cplusplus

#define XAUDIO_USE_BATCHALLOC_NEWDELETE() \
    public: \
        static void *__cdecl operator new(size_t Size) \
        { \
            ASSERT(FALSE); \
            return NULL; \
        } \
        static void *__cdecl operator new[](size_t Size) \
        { \
            ASSERT(FALSE); \
            return NULL; \
        } \
        static void *__cdecl operator new(size_t Size, IXAudioBatchAllocator *pAllocator) \
        { \
            return pAllocator->Alloc(Size); \
        } \
        static void *__cdecl operator new[](size_t Size, IXAudioBatchAllocator *pAllocator) \
        { \
            return pAllocator->Alloc(Size); \
        } \
        static void __cdecl operator delete(void *pBuffer) \
        { \
        } \
        static void __cdecl operator delete[](void *pBuffer) \
        { \
        } \
        static void __cdecl operator delete(void *pBuffer, IXAudioBatchAllocator *pAllocator) \
        { \
        } \
        static void __cdecl operator delete[](void *pBuffer, IXAudioBatchAllocator *pAllocator) \
        { \
        }

#define XAUDIO_BATCHALLOC_NEW(Type, pAllocator) \
    new(pAllocator) Type

#define XAUDIO_BATCHALLOC_NEWA(Type, Count, pAllocator) \
    new(pAllocator) Type[Count]

#define XAUDIO_BATCHALLOC_DELETE(pObject) \
    { \
        if(pObject) \
        { \
            delete pObject; \
            pObject = NULL; \
        } \
    }

#define XAUDIO_BATCHALLOC_DELETEA(pObject) \
    { \
        if(pObject) \
        { \
            delete [] pObject; \
            pObject = NULL; \
        } \
    }

#endif // __cplusplus

/**************************************************************************
 *
 * Debugging helpers
 *
 **************************************************************************/

STDAPI_(void) XAudioSetDebugPrintLevel(XAUDIODEBUGLEVEL Level);
STDAPI_(void) XAudioSetDebugBreakLevel(XAUDIODEBUGLEVEL Level);
STDAPI_(void) XAudioSetDebugFlags(XAUDIODEBUGFLAGS Flags);
STDAPI_(void) XAudioDumpMemoryUsage(BOOL AssertNone);
STDAPI_(void) XAudioQueryPerformance(LPXAUDIOPERFCOUNTER pCounter);

/**************************************************************************
 *
 * Compatibility wrappers
 *
 **************************************************************************/

#ifdef __cplusplus

__forceinline ULONG XAudioRefCount_AddRef(IXAudioRefCount *pRefCount)
{
    return pRefCount->AddRef();
}

__forceinline ULONG XAudioRefCount_Release(IXAudioRefCount *pRefCount)
{
    return pRefCount->Release();
}

__forceinline ULONG XAudioBatchAllocator_AddRef(IXAudioBatchAllocator *pBatchAllocator)
{
    return pBatchAllocator->AddRef();
}

__forceinline ULONG XAudioBatchAllocator_Release(IXAudioBatchAllocator *pBatchAllocator)
{
    return pBatchAllocator->Release();
}

__forceinline void XAudioBatchAllocator_GrowHeap(IXAudioBatchAllocator *pBatchAllocator, DWORD BufferSize)
{
    pBatchAllocator->GrowHeap(BufferSize);
}

__forceinline HRESULT XAudioBatchAllocator_CreateHeap(IXAudioBatchAllocator *pBatchAllocator, DWORD AllocAttributes)
{
    return pBatchAllocator->CreateHeap(AllocAttributes);
}

__forceinline DWORD XAudioBatchAllocator_GetFreeHeapSize(IXAudioBatchAllocator *pBatchAllocator)
{
    return pBatchAllocator->GetFreeHeapSize();
}

__forceinline LPVOID XAudioBatchAllocator_Alloc(IXAudioBatchAllocator *pBatchAllocator, DWORD BufferSize)
{
    return pBatchAllocator->Alloc(BufferSize);
}

#else // __cplusplus

__forceinline ULONG XAudioRefCount_AddRef(IXAudioRefCount *pRefCount)
{
    return pRefCount->lpVtbl->AddRef(pRefCount);
}

__forceinline ULONG XAudioRefCount_Release(IXAudioRefCount *pRefCount)
{
    return pRefCount->lpVtbl->Release(pRefCount);
}

__forceinline ULONG XAudioBatchAllocator_AddRef(IXAudioBatchAllocator *pBatchAllocator)
{
    return pBatchAllocator->lpVtbl->AddRef(pBatchAllocator);
}

__forceinline ULONG XAudioBatchAllocator_Release(IXAudioBatchAllocator *pBatchAllocator)
{
    return pBatchAllocator->lpVtbl->Release(pBatchAllocator);
}

__forceinline void XAudioBatchAllocator_GrowHeap(IXAudioBatchAllocator *pBatchAllocator, DWORD BufferSize)
{
    pBatchAllocator->lpVtbl->GrowHeap(pBatchAllocator, BufferSize);
}

__forceinline HRESULT XAudioBatchAllocator_CreateHeap(IXAudioBatchAllocator *pBatchAllocator, DWORD AllocAttributes)
{
    return pBatchAllocator->lpVtbl->CreateHeap(pBatchAllocator, AllocAttributes);
}

__forceinline DWORD XAudioBatchAllocator_GetFreeHeapSize(IXAudioBatchAllocator *pBatchAllocator)
{
    return pBatchAllocator->lpVtbl->GetFreeHeapSize(pBatchAllocator);
}

__forceinline LPVOID XAudioBatchAllocator_Alloc(IXAudioBatchAllocator *pBatchAllocator, DWORD BufferSize)
{
    return pBatchAllocator->lpVtbl->Alloc(pBatchAllocator, BufferSize);
}

#endif // __cplusplus

#pragma pack(pop)
#pragma warning(pop)
#endif // __XAUDDEFS_INCLUDED__

