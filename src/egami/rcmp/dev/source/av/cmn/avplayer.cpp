typedef unsigned int u32;

#include "snd/sndo.h"

unsigned int __OSBusClock __attribute__((address((0x800000F8))));

#define OS_BUS_CLOCK (u32)__OSBusClock
#define OS_TIMER_CLOCK (OS_BUS_CLOCK / 4)
#define OSTicksToMilliseconds(ticks) ((ticks) / (OS_TIMER_CLOCK / 1000))

// total size: 0x8
struct STREAMCHUNKHDR {
    int type;
    int size;
};

struct TagValuePair;

struct IAllocator {
    virtual void *Alloc(unsigned int size, const TagValuePair &flags);
    virtual void Free(void *pBlock, unsigned int size);
    virtual int AddRef();
    virtual int Release();
    virtual ~IAllocator();
};

// total size: 0x34
struct FILESYSOPTS {
    int size;
    IAllocator *allocator;
    int MaxOpenFiles;
    int MaxFileOps;
    int nSearchLocs;
    int nSearchPathLength;
    int MaxDevices;
    int ThreadStackSize;
    int (*decompresssize)(const void *);
    int (*decompress)(const void *, void *);
    unsigned int LargeReadSliceSize;
    unsigned int AllocAlignBoundary;
    int DiscType;
};

extern "C" {
long long OSGetTime();
int SNDPROFILE_outputlatency();
}

void DEBUG_break();
void MEM_fill(void *dst, unsigned int val, int size);
void SYNCTASK_run();
void FILE_getopts(FILESYSOPTS *opts);

int STREAM_create(int type, int nbuffers, int nfilters, void *mem, int memsize);
int STREAM_taphandle(int stream, int tap);
void STREAM_destroy(int stream);
int STREAM_setpriority(int stream, int priority, int subpriority);
int STREAM_setfilter(int stream, int filter, int mask, int match, int tap);
int STREAM_queuefile(int stream, const char *name, int offset, int flags);
int STREAM_queuemem(int stream, void *data, int size, int flags);
STREAMCHUNKHDR *STREAM_get(int stream);
void STREAM_release(int stream, STREAMCHUNKHDR *chunk);
int STREAM_isendofstream(int stream);

int ASYNCFILE_load(const char *name, int memdir);
int ASYNCFILE_release(int handle, void **data, int *size);

static inline unsigned int getm(const void *src, int bytes) {
    return *(const unsigned int *)src;
}

static inline unsigned int geti(const void *src, int bytes) {
    const unsigned char *s = (const unsigned char *)src;
    return (s[3] << 24) | (s[2] << 16) | (s[1] << 8) | s[0];
}

static inline void FileFree(void *d) {
    FILESYSOPTS fso;

    fso.size = sizeof(FILESYSOPTS);
    FILE_getopts(&fso);
    fso.allocator->Free(d, 0);
}

namespace RCMP {

class RCMP_SYSTEM {
  public:
    void *(*AllocMemFunc)(const char *, int, int, int, int);
    void (*FreeMemFunc)(void *);
    int m_DefaultMemDir;

    bool IsInited() {
        return AllocMemFunc != 0 && FreeMemFunc != 0;
    }
    void *AllocMem(const char *name, unsigned int size, int alignment, int headersize, int type) {
        return AllocMemFunc(name, size, alignment, headersize, type);
    }
    void *AllocMem(const char *name, int size, int alignment, int headersize, int type) {
        return AllocMemFunc(name, size, alignment, headersize, type);
    }
    void FreeMem(void *memadr) {
        FreeMemFunc(memadr);
    }
};

extern RCMP_SYSTEM rcmp_sys;

struct Shape;

enum FRAME_TYPE_ENUM {
    FRAME_MPC = 0,
    FRAME_MAD = 1,
    FRAME_PS2_SONY = 2,
    FRAME_VP6 = 3,
};

class FRAME {
  public:
    FRAME_TYPE_ENUM m_FrameType;
    Shape *m_Shp;

    FRAME() {}
    ~FRAME() {}
    FRAME_TYPE_ENUM GetFrameType() {
        return m_FrameType;
    }
    Shape *GetShape() {
        return m_Shp;
    }
};

class CHUNK {
  public:
    void *m_UserChunkData;
    void *m_DataToDecode;
    unsigned int m_DataToDecodeSize;

    CHUNK();
    ~CHUNK() {}
    static void *operator new(unsigned int size) {
        return rcmp_sys.AllocMem("", size, 0, 0, rcmp_sys.m_DefaultMemDir);
    }
    static void operator delete(void *ptr) {
        rcmp_sys.FreeMem(ptr);
    }
    void SetUserChunkData(void *Data) {
        m_UserChunkData = Data;
    }
    void SetDataToDecode(void *Data) {
        m_DataToDecode = Data;
    }
    void SetSizeOfDataToDecode(unsigned int DataSize) {
        m_DataToDecodeSize = DataSize;
    }
    void *GetUserChunkData() {
        return m_UserChunkData;
    }
    void *GetDataToDecode() {
        return m_DataToDecode;
    }
    unsigned int GetSizeOfDataToDecode() {
        return m_DataToDecodeSize;
    }
};

/* "Assert: %s:%i %s (%s)": literal MUERTO del objetivo, entre "" y "AV::VideoStreamBuffer". Sin simbolo para que el enlazador no lo estripe. */
class STREAMER {
  public:
    void *m_Streamer;

    STREAMER(void *Data) {
        m_Streamer = Data;
    }
    ~STREAMER() {}
    void SetStreamer(void *Data) {
        m_Streamer = Data;
    }
    void *GetStreamer() {
        return m_Streamer;
    }
};

class DECODER;

typedef void (*GETDATACALLBACK)(DECODER *, STREAMER *, CHUNK **);
typedef void (*RELEASEDATACALLBACK)(DECODER *, STREAMER *, CHUNK *);

class CODEC_IDATA {
  public:
    STREAMER *m_Streamer;
    GETDATACALLBACK m_GetDataFunc;
    RELEASEDATACALLBACK m_ReleaseDataFunc;
    unsigned int m_MaxFramesOutstanding;

    CODEC_IDATA();
    CODEC_IDATA(STREAMER *Streamer, GETDATACALLBACK GetDataFunc, RELEASEDATACALLBACK ReleaseDataFunc, unsigned int NumberOfFramesToBuffer);
    ~CODEC_IDATA() {}
    static void operator delete(void *ptr) {
        rcmp_sys.FreeMem(ptr);
    }
};

enum DETECTED_USABILITY_ENUM {
    NOT_USEABLE = 0,
    USABILITY_UNSURE = 1,
    USEABLE = 2,
};

class CODEC {
  public:
    virtual ~CODEC();
    virtual DETECTED_USABILITY_ENUM Init(DECODER *Decoder, CHUNK *FirstChunk) = 0;
    virtual FRAME *GetFrame(unsigned int GoalFrame) = 0;
    virtual unsigned int GetCurrentFrameNumber() = 0;
    virtual float GetFrameRate() = 0;
    virtual void ReleaseFrame(FRAME *Frame) = 0;
};

class DECODER {
  public:
    CHUNK *m_FirstChunk;
    CODEC_IDATA m_IData;
    CODEC *m_codec;

    DECODER(const CODEC_IDATA *IData);
    virtual ~DECODER();

    static void *operator new(unsigned int size) {
        return rcmp_sys.AllocMem("", size, 0, 0, rcmp_sys.m_DefaultMemDir);
    }
    static void operator delete(void *ptr) {
        rcmp_sys.FreeMem(ptr);
    }

    bool HasCodec() {
        return m_codec != 0;
    }
    CODEC_IDATA *GetCodecIData() {
        return &m_IData;
    }

    DETECTED_USABILITY_ENUM ChooseCodec(CODEC *codec, CHUNK *FirstChunk);
    void FreeChosenCodec();
    unsigned int GetCurrentFrameNumber();
    float GetFrameRate();
    FRAME *GetFrame(unsigned int GoalFrame);
    void ReleaseFrame(FRAME *Frame);
    CHUNK *GetChunk();
    void ReleaseChunk(CHUNK *Data);
};

enum CODEC_TYPE {
    NONE_CODEC = 0,
    MPC_CODEC = 1,
    MAD_CODEC = 2,
    PS2_SONY_CODEC = 3,
    VP6_HEAD_CODEC = 4,
    VP6_CODEC = 5,
};

int MAD_CODEC_is_chunk_for_codec(unsigned int chunk_type);
int VP6_CODEC_is_chunk_for_codec(unsigned int chunk_type);
int VP6_CODEC_is_head_chunk_for_codec(unsigned int chunk_type);
CODEC *MAD_CODEC_create();
CODEC *VP6_CODEC_create();

class AUDIO_PLAYER {
  public:
    float mSpeed;
    void *mMemBlock;
    int mPad;
    int mTap;
    int mRequestId;

    static void *operator new(unsigned int size) {
        return rcmp_sys.AllocMem("", size, 0, 0, rcmp_sys.m_DefaultMemDir);
    }
    static void operator delete(void *p) {
        rcmp_sys.FreeMem(p);
    }

    int GetStreamHandle() {
        return mTap;
    }

    AUDIO_PLAYER(int channel, int param);
    ~AUDIO_PLAYER();
    int SetSpeed(unsigned int speed);
    int StartSound();
    int SetVol(unsigned int vol);
    bool IsAudioFinished();
};

class AV_SUBTITLE;
class AV_SUBTITLE_ARRAY;
class AV_MS_TIMER;

// total size: 0x94
class AV_PLAYER {
  public:
    enum LOAD_ENUM {
        STREAM = 0,
        PRELOAD = 1,
        FROM_MEM = 2,
    };
    enum SOUND_ENUM {
        SOUND_ON = 0,
        SOUND_OFF = 1,
    };

    static void *operator new(unsigned int size) {
        return rcmp_sys.AllocMem("", size, 0, 0, rcmp_sys.m_DefaultMemDir);
    }
    static void operator delete(void *ptr) {
        rcmp_sys.FreeMem(ptr);
    }

    unsigned int GetCurFrame() {
        return m_CurFrame;
    }
    DECODER *GetDecoder() {
        return m_pdecoder;
    }
    float GetGoalFrame() {
        return m_GoalFrame;
    }
    int GetVideoStreamHandle() {
        return m_VideoStream;
    }

    AV_PLAYER(const char *VideoFileName, int BufferSize, LOAD_ENUM LoadMode, SOUND_ENUM SndMode);

    void Init(const char *VideoFileName, int SizeOfVideoFile, int VideoBufferSize, int VideoStreamOffset, const char *AudioFileName, int SizeOfAudioFile, int AudioBufferSize, int AudioStreamOffset, LOAD_ENUM LoadMode, SOUND_ENUM SndMode);

    ~AV_PLAYER();

    FRAME *GetFirstFrame(unsigned int MaxFramesOutstanding, int VideoLatencyInMs);

    FRAME *GetFrame(float GoalFrame);

    bool IsTimeForDecode();

    bool IsAudioFinished();

    int SetSpeed(unsigned int Speed);

    int Pause();

    int UnPause();

    int SetVol(unsigned int Vol);

    unsigned int SyncedAudioTime();

    void GetRCMPChunk(DECODER *decoder, CHUNK **ppdchunk);

    static void StaticGetRCMPChunk(DECODER *decoder, STREAMER *streamer, CHUNK **ppdchunk);

    void ReleaseRCMPChunk(CHUNK *dchunk);

    static void StaticReleaseRCMPChunk(DECODER *decoder, STREAMER *streamer, CHUNK *dchunk);

    AUDIO_PLAYER *m_ap;
    unsigned char *m_VideoStreamBuff;
    unsigned char *m_AudioStreambuff;
    const char *m_VideoFileName;
    const char *m_AudioFileName;
    bool m_SndFromDifferentFile;
    LOAD_ENUM m_LoadMode;
    SOUND_ENUM m_SndMode;
    unsigned char *m_VideoData;
    int m_AyncVideoFileHandle;
    unsigned char *m_AudioData;
    int m_AyncAudioFileHandle;
    int m_VideoStream;
    int m_AudioStream;
    int m_VideoStreamRequestID;
    int m_AudioStreamRequestID;
    int m_VideoLatencyInMs;
    unsigned int m_CurFrame;
    float m_GoalFrame;
    unsigned int m_refms;
    int m_oldaudiotime;
    int m_trackingaudio;
    int m_filterederror;
    AV_MS_TIMER *m_MSTimer;
    STREAMER m_data_streamer;
    DECODER *m_pdecoder;
    void *m_VideoChunkParser;
    void *m_AudioChunkParser;
    AV_SUBTITLE_ARRAY *m_SubtitleArray;
    FRAME *m_CurRCMPFrame;
    CHUNK m_ChunkPool[2];
    int m_CurChunk;
};

// total size: 0x18
class AV_MS_TIMER {
  public:
    long long m_OldTime;
    unsigned long long m_AccTime;
    unsigned int m_Speed;

    static void *operator new(unsigned int size) {
        return rcmp_sys.AllocMem("", size, 0, 0, rcmp_sys.m_DefaultMemDir);
    }
    static void operator delete(void *ptr) {
        rcmp_sys.FreeMem(ptr);
    }

    AV_MS_TIMER() {
        m_AccTime = 0;
        m_OldTime = OSGetTime();
        m_Speed = 0x1000;
    }
    ~AV_MS_TIMER() {}
    unsigned int GetMS() {
        Update();
        return (unsigned int)OSTicksToMilliseconds(m_AccTime);
    }
    void Update() {
        long long time;

        time = OSGetTime();
        if (m_Speed == 0x1000) {
            m_AccTime += time - m_OldTime;
        } else {
            m_AccTime += ((unsigned long long)m_Speed * (time - m_OldTime)) >> 12;
        }
        m_OldTime = time;
    }
    void SetSpeed(unsigned int Speed) {
        Update();
        m_Speed = Speed;
    }
    unsigned int GetSpeed() {
        return m_Speed;
    }
};

AV_PLAYER::AV_PLAYER(const char *VideoFileName, int BufferSize, LOAD_ENUM LoadMode, SOUND_ENUM SndMode)
    : m_data_streamer(0) {
    Init(VideoFileName, 0, BufferSize, 0, 0, 0, 0, 0, LoadMode, SndMode);
}

void AV_PLAYER::Init(const char *VideoFileName, int SizeOfVideoFile, int VideoBufferSize, int VideoStreamOffset, const char *AudioFileName, int SizeOfAudioFile, int AudioBufferSize, int AudioStreamOffset, LOAD_ENUM LoadMode, SOUND_ENUM SndMode) {
    if (!Snd::System::IsInited()) {
        SndMode = SOUND_OFF;
    }

    MEM_fill(this, 0, sizeof(AV_PLAYER));

    m_LoadMode = LoadMode;
    m_SndMode = SndMode;
    m_VideoData = 0;
    m_AudioData = 0;
    m_AyncVideoFileHandle = 0;
    m_AyncAudioFileHandle = 0;
    m_CurFrame = 0;
    m_MSTimer = 0;
    m_pdecoder = 0;
    m_ap = 0;
    m_SubtitleArray = 0;
    m_CurRCMPFrame = 0;
    m_CurChunk = 0;
    m_VideoFileName = VideoFileName;
    m_AudioFileName = AudioFileName;

    if (!rcmp_sys.IsInited()) {
        DEBUG_break();
        return;
    }

    if (AudioFileName != 0) {
        m_SndFromDifferentFile = true;
        m_VideoStreamBuff = (unsigned char *)rcmp_sys.AllocMem("AV::VideoStreamBuffer", VideoBufferSize, 0, 0, rcmp_sys.m_DefaultMemDir);
        m_AudioStreambuff = (unsigned char *)rcmp_sys.AllocMem("AV::AudioStreamBuffer", AudioBufferSize, 0, 0, rcmp_sys.m_DefaultMemDir);
        m_VideoStream = STREAM_create(2, 2, 1, m_VideoStreamBuff, VideoBufferSize);
        m_AudioStream = STREAM_create(2, 2, 1, m_AudioStreambuff, AudioBufferSize);
        STREAM_setpriority(m_AudioStream, 0x98, 0x34);
        STREAM_setpriority(m_VideoStream, 0x98, 0x34);
        STREAM_setfilter(m_VideoStream, 1, 0xFF, 0x4D, 1);
        STREAM_setfilter(m_VideoStream, 2, 0, 0, -2);
        if (m_SndMode == SOUND_ON) {
            STREAM_setfilter(m_AudioStream, 1, 0xFFFF, 0x4353, 1);
        } else {
            STREAM_setfilter(m_AudioStream, 1, 0xFFFF, 0x4353, -1);
        }
        STREAM_setfilter(m_AudioStream, 2, 0, 0, -2);
        if (m_LoadMode == PRELOAD) {
            m_AyncAudioFileHandle = ASYNCFILE_load(AudioFileName, rcmp_sys.m_DefaultMemDir);
            m_AyncVideoFileHandle = ASYNCFILE_load(VideoFileName, rcmp_sys.m_DefaultMemDir);
        } else if (m_LoadMode == STREAM) {
            m_AudioStreamRequestID = STREAM_queuefile(m_AudioStream, AudioFileName, AudioStreamOffset, 0);
            m_VideoStreamRequestID = STREAM_queuefile(m_VideoStream, VideoFileName, VideoStreamOffset, 0);
        } else if (m_LoadMode == FROM_MEM) {
            m_VideoStreamRequestID = STREAM_queuemem(m_VideoStream, (void *)m_VideoFileName, SizeOfVideoFile, 0);
            m_AudioStreamRequestID = STREAM_queuemem(m_AudioStream, (void *)m_AudioFileName, SizeOfAudioFile, 0);
        }
    } else {
        m_SndFromDifferentFile = false;
        m_VideoStreamBuff = (unsigned char *)rcmp_sys.AllocMem("AV::VideoStreamBuffer", VideoBufferSize, 0, 0, rcmp_sys.m_DefaultMemDir);
        m_AudioStreambuff = 0;
        m_VideoStream = STREAM_create(2, 3, 2, m_VideoStreamBuff, VideoBufferSize);
        m_AudioStream = STREAM_taphandle(m_VideoStream, 2);
        STREAM_setpriority(m_AudioStream, 0x98, 0x34);
        STREAM_setpriority(m_VideoStream, 0x98, 0x34);
        STREAM_setfilter(m_VideoStream, 1, 0xFF, 0x4D, 1);
        STREAM_setfilter(m_VideoStream, 3, 0, 0, -2);
        if (m_SndMode == SOUND_ON) {
            STREAM_setfilter(m_VideoStream, 2, 0xFFFF, 0x4353, 2);
        } else {
            STREAM_setfilter(m_VideoStream, 2, 0xFFFF, 0x4353, -1);
        }
        if (m_LoadMode == PRELOAD) {
            m_AyncVideoFileHandle = ASYNCFILE_load(VideoFileName, rcmp_sys.m_DefaultMemDir);
        } else if (m_LoadMode == STREAM) {
            m_VideoStreamRequestID = STREAM_queuefile(m_VideoStream, VideoFileName, VideoStreamOffset, 0);
        } else if (m_LoadMode == FROM_MEM) {
            m_VideoStreamRequestID = STREAM_queuemem(m_VideoStream, (void *)m_VideoFileName, SizeOfVideoFile, 0);
        }
    }
}

AV_PLAYER::~AV_PLAYER() {
    if (m_pdecoder != 0) {
        delete m_pdecoder;
        m_pdecoder = 0;
    }
    if (m_ap != 0) {
        delete m_ap;
        m_ap = 0;
    }
    if (m_MSTimer != 0) {
        delete m_MSTimer;
        m_MSTimer = 0;
    }
    STREAM_destroy(m_VideoStream);
    m_VideoStream = 0;
    if (m_SndFromDifferentFile) {
        STREAM_destroy(m_AudioStream);
        m_AudioStream = 0;
    }
    if (m_VideoData != 0) {
        FileFree(m_VideoData);
        m_VideoData = 0;
    }
    if (m_AudioData != 0) {
        FileFree(m_AudioData);
        m_AudioData = 0;
    }
    if (m_VideoStreamBuff != 0) {
        rcmp_sys.FreeMem(m_VideoStreamBuff);
        m_VideoStreamBuff = 0;
    }
    if (m_AudioStreambuff != 0) {
        rcmp_sys.FreeMem(m_AudioStreambuff);
        m_AudioStreambuff = 0;
    }
}

FRAME *AV_PLAYER::GetFirstFrame(unsigned int MaxFramesOutstanding, int VideoLatencyInMs) {
    int audio_stream = m_AudioStream;
    int video_latency = VideoLatencyInMs;
    STREAM_setpriority(audio_stream, 0x97, 0x33);
    STREAM_setpriority(m_VideoStream, 0x97, 0x33);

    if (m_AyncVideoFileHandle != 0) {
        int size;
        int ret;

        ret = ASYNCFILE_release(m_AyncVideoFileHandle, (void **)&m_VideoData, &size);
        m_VideoStreamRequestID = STREAM_queuemem(m_VideoStream, m_VideoData, size, 0);
    }

    if (m_AyncAudioFileHandle != 0) {
        int size;
        int ret;

        ret = ASYNCFILE_release(m_AyncAudioFileHandle, (void **)&m_AudioData, &size);
        m_AudioStreamRequestID = STREAM_queuemem(m_AudioStream, m_AudioData, size, 0);
    }

    if (!m_SndFromDifferentFile) {
        m_AudioStreamRequestID = m_VideoStreamRequestID;
    }

    if (m_SndMode == SOUND_ON) {
        m_ap = new AUDIO_PLAYER(m_AudioStream, m_AudioStreamRequestID);
        m_trackingaudio = (m_ap->GetStreamHandle() >= 0);
    } else {
        m_trackingaudio = 0;
    }

    m_pdecoder = 0;
    if (rcmp_sys.IsInited()) {
        m_data_streamer.SetStreamer(this);
        CODEC_IDATA cidata(&m_data_streamer, StaticGetRCMPChunk, StaticReleaseRCMPChunk, MaxFramesOutstanding);

        m_pdecoder = new DECODER(&cidata);
    }

    m_CurRCMPFrame = m_pdecoder->GetFrame(0);

    m_MSTimer = new AV_MS_TIMER();
    m_refms = m_MSTimer->GetMS();
    m_oldaudiotime = 0;
    m_filterederror = 0;
    m_VideoLatencyInMs = video_latency;
    if (m_ap != 0) {
        m_ap->StartSound();
    }
    SetSpeed(0x1000);
    return m_CurRCMPFrame;
}

FRAME *AV_PLAYER::GetFrame(float GoalFrame) {
    m_GoalFrame = GoalFrame;
    m_CurRCMPFrame = m_pdecoder->GetFrame((unsigned int)GoalFrame);
    m_CurFrame = m_pdecoder->GetCurrentFrameNumber();
    return m_CurRCMPFrame;
}

/* segundo 0x41E0000000000000 (2^31) del pool: el objetivo lo tiene por duplicado y nadie lo referencia. */
bool AV_PLAYER::IsTimeForDecode() {
    unsigned int CurTimeMs;

    CurTimeMs = SyncedAudioTime();
    m_GoalFrame = (float)(CurTimeMs + m_VideoLatencyInMs) * 0.001f * m_pdecoder->GetFrameRate();
    return m_GoalFrame > (float)m_CurFrame;
}

bool AV_PLAYER::IsAudioFinished() {
    if (m_ap != 0) {
        return m_ap->IsAudioFinished();
    }
    return true;
}

int AV_PLAYER::SetSpeed(unsigned int Speed) {
    int err = 0;

    if (Speed > 0x4000) {
        Speed = 0x4000;
    }
    if (m_ap != 0) {
        m_ap->SetSpeed(Speed);
    }
    m_MSTimer->SetSpeed(Speed);
    return err;
}

int AV_PLAYER::Pause() {
    return SetSpeed(0);
}

int AV_PLAYER::UnPause() {
    return SetSpeed(0x1000);
}

int AV_PLAYER::SetVol(unsigned int Vol) {
    if (m_ap != 0) {
        return m_ap->SetVol(Vol);
    }
    return 0;
}

unsigned int AV_PLAYER::SyncedAudioTime() {
    SNDREQUESTSTATUS status;
    SNDSTREAMSTATUS sndstrmsstatus;
    int audiotime;
    int error;
    int ellapsed;
    unsigned int ms;

    ms = m_MSTimer->GetMS();
    ellapsed = ms - m_refms;
    if (m_trackingaudio) {
        SNDSYS_entercritical();
        SNDSTRM_status(m_ap->GetStreamHandle(), &sndstrmsstatus);
        SNDSTRM_requeststatus(sndstrmsstatus.currentrequest, &status);
        SNDSYS_leavecritical();
        audiotime = status.currenttime - SNDPROFILE_outputlatency();
        if (audiotime > m_oldaudiotime) {
            m_oldaudiotime = audiotime;
            error = audiotime - ellapsed;
            m_filterederror = m_filterederror - m_filterederror / 8 + error;
            if ((-m_filterederror < m_filterederror ? m_filterederror : -m_filterederror) > 264) {
                m_refms = m_refms - m_filterederror / 8;
                m_filterederror = 0;
                ellapsed = ms - m_refms;
            }
        }
    }
    return ellapsed;
}

void AV_PLAYER::GetRCMPChunk(DECODER *decoder, CHUNK **ppdchunk) {
    CODEC_TYPE codecType = NONE_CODEC;
    STREAMCHUNKHDR *chunk;
    int stream;
    CHUNK *pdchunk;

    stream = GetVideoStreamHandle();
    *ppdchunk = 0;

    do {
        chunk = STREAM_get(stream);
        SYNCTASK_run();
        if (chunk != 0) {
            unsigned int tmp;
            char temp[5];

            tmp = getm(chunk, 4);
            if (MAD_CODEC_is_chunk_for_codec(tmp)) {
                codecType = MAD_CODEC;
            }
            if (VP6_CODEC_is_chunk_for_codec(tmp)) {
                codecType = VP6_CODEC;
            }
            if (VP6_CODEC_is_head_chunk_for_codec(tmp)) {
                codecType = VP6_HEAD_CODEC;
            }
            if (codecType != NONE_CODEC) {
                m_CurChunk++;
                if (m_CurChunk > 1) {
                    m_CurChunk = 0;
                }
                pdchunk = &m_ChunkPool[m_CurChunk];
                *ppdchunk = pdchunk;
                pdchunk->SetUserChunkData(chunk);
                if (codecType == MAD_CODEC) {
                    pdchunk->SetSizeOfDataToDecode(geti(&chunk->size, 4));
                    pdchunk->SetDataToDecode(chunk);
                } else if (codecType == VP6_HEAD_CODEC || codecType == VP6_CODEC) {
                    pdchunk->SetSizeOfDataToDecode(geti(&chunk->size, 4));
                    pdchunk->SetDataToDecode(chunk);
                } else {
                    pdchunk->SetSizeOfDataToDecode(geti(&chunk->size, 4));
                    pdchunk->SetDataToDecode(chunk + 1);
                }
                if (!decoder->HasCodec()) {
                    if (codecType == MAD_CODEC) {
                        CODEC *codec = MAD_CODEC_create();
                        if (decoder->ChooseCodec(codec, pdchunk) != USEABLE) {
                            decoder->FreeChosenCodec();
                        }
                    }
                    if (codecType == VP6_HEAD_CODEC) {
                        CODEC *codec = VP6_CODEC_create();
                        if (decoder->ChooseCodec(codec, pdchunk) != USEABLE) {
                            decoder->FreeChosenCodec();
                        }
                    }
                }
                return;
            } else {
                *(unsigned int *)temp = tmp;
                temp[4] = 0;
                STREAM_release(stream, chunk);
            }
        }
    } while (!STREAM_isendofstream(stream));
}

void AV_PLAYER::StaticGetRCMPChunk(DECODER *decoder, STREAMER *streamer, CHUNK **ppdchunk) {
    AV_PLAYER *avp;

    avp = (AV_PLAYER *)streamer->GetStreamer();
    avp->GetRCMPChunk(decoder, ppdchunk);
}

void AV_PLAYER::ReleaseRCMPChunk(CHUNK *dchunk) {
    STREAMCHUNKHDR *chunk;
    int stream;

    chunk = (STREAMCHUNKHDR *)dchunk->GetUserChunkData();
    stream = GetVideoStreamHandle();
    STREAM_release(stream, chunk);
}

void AV_PLAYER::StaticReleaseRCMPChunk(DECODER *decoder, STREAMER *streamer, CHUNK *dchunk) {
    AV_PLAYER *avp;

    avp = (AV_PLAYER *)streamer->GetStreamer();
    avp->ReleaseRCMPChunk(dchunk);
}

} // namespace RCMP
