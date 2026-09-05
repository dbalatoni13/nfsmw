#include "rcmp/rcmp.h"
#include "dolphin.h"
#include "snd/sndo.h"
#include "../../../../../realcore/6.24.00/include/common/realcore/file/filesys.h"

struct AV_SUBTITLE_ARRAY;
struct AV_SUBTITLE;
struct AV_CHUNK_PARSER;
struct STREAMCHUNKHDR {
    int type;
    int size;
};

extern void STREAM_release(int stream, STREAMCHUNKHDR *chunk);
extern STREAMCHUNKHDR *STREAM_get(int stream);
extern int STREAM_isendofstream(int stream);
extern void STREAM_destroy(int stream);
extern int STREAM_create(int requests, int filters, int taps, void *buffer, int size);
extern void STREAM_setpriority(int stream, int prioritylow, int priorityhigh);
extern void STREAM_setfilter(int stream, int filternum, int mask, int value, int tapnum);
extern int STREAM_queuefile(int stream, const char *name, int offset, int endchunkid);
extern int STREAM_queuemem(int stream, void *buffer, int size, int endchunkid);
extern int STREAM_taphandle(int stream, int tapnum);
extern int ASYNCFILE_load(const char *name, int memtype);
extern int ASYNCFILE_release(int handle, void **address, int *size);
extern void MEM_fill(void *dest, unsigned int val, int count);
extern void DEBUG_break();
extern char lbl_8040FEE0[];
extern char lbl_8040FEFC[];
extern char lbl_8040FF14[];
extern const double lbl_8040FF30;
extern const double lbl_8040FF40;
extern const float lbl_8040FF48;
extern "C" void SNDSYS_entercritical();
extern "C" void SNDSYS_leavecritical();
extern "C" int SNDSTRM_status(int streamhandle, SNDSTREAMSTATUS *status);
extern "C" int SNDSTRM_requeststatus(int requesthandle, SNDREQUESTSTATUS *status);
extern "C" int SNDPROFILE_outputlatency();
extern void SYNCTASK_run();

namespace Snd {

struct System {
    static bool IsInited();
};

} // namespace Snd

namespace RCMP {

enum CODEC_TYPE {
    NONE_CODEC = 0,
    MPC_CODEC = 1,
    MAD_CODEC = 2,
    PS2_SONY_CODEC = 3,
    VP6_HEAD_CODEC = 4,
    VP6_CODEC = 5,
};

extern bool MAD_CODEC_is_chunk_for_codec(unsigned int chunktype);
extern bool VP6_CODEC_is_chunk_for_codec(unsigned int chunktype);
extern bool VP6_CODEC_is_head_chunk_for_codec(unsigned int chunktype);
extern CODEC *MAD_CODEC_create();
extern CODEC *VP6_CODEC_create();

static inline unsigned int getm(const void *src, int bytes) {
    return *static_cast<const unsigned int *>(src);
}

static inline unsigned int geti(const void *src, int bytes) {
    if (bytes == 2) {
        return (static_cast<const unsigned char *>(src)[5] << 8) |
               static_cast<const unsigned char *>(src)[4];
    }
    return (static_cast<const unsigned char *>(src)[7] << 24) |
           (static_cast<const unsigned char *>(src)[6] << 16) |
           (static_cast<const unsigned char *>(src)[5] << 8) |
           static_cast<const unsigned char *>(src)[4];
}

struct AV_MS_TIMER {
    long long m_Time;
    long long m_Elapsed;
    unsigned int m_TimeBase;

    inline static void *operator new(unsigned int size) {
        return rcmp_sys.AllocMem(lbl_8040FEE0, size, 0, 0, rcmp_sys.m_DefaultMemDir);
    }

    inline static void operator delete(void *ptr) {
        rcmp_sys.FreeMem(ptr);
    }

    inline AV_MS_TIMER() {}
    inline ~AV_MS_TIMER() {}
    inline void Update();
    inline unsigned int GetMS();
    void SetSpeed(unsigned int Speed);
};

inline unsigned int AV_MS_TIMER::GetMS() {
    return static_cast<unsigned int>(
        static_cast<unsigned long long>(this->m_Elapsed) /
        (OS_TIMER_CLOCK / 1000));
}

struct AUDIO_PLAYER {
    float m_Volume;
    unsigned char *m_audiobuff;
    int m_audiotap;
    int m_sndstreamhandle;
    int m_sndrequesthandle;

    inline static void *operator new(unsigned int size) {
        return rcmp_sys.AllocMem(lbl_8040FEE0, size, 0, 0, rcmp_sys.m_DefaultMemDir);
    }

    inline static void operator delete(void *ptr) {
        rcmp_sys.FreeMem(ptr);
    }

    AUDIO_PLAYER(int streamhandle, int requesthandle);
    ~AUDIO_PLAYER();
    void StartSound();
    int SetSpeed(unsigned int Speed);
    bool IsAudioFinished();
    int SetVol(unsigned int volume);
    inline int GetStreamHandle() {
        return this->m_sndstreamhandle;
    }
};

struct AV_PLAYER {
    enum LOAD_ENUM {
        STREAM = 0,
        PRELOAD = 1,
        FROM_MEM = 2,
    };
    enum SOUND_ENUM {
        SOUND_ON = 0,
        SOUND_OFF = 1,
    };

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
    AV_CHUNK_PARSER *m_VideoChunkParser;
    AV_CHUNK_PARSER *m_AudioChunkParser;
    AV_SUBTITLE_ARRAY *m_SubtitleArray;
    FRAME *m_CurRCMPFrame;
    CHUNK m_ChunkPool[2];
    int m_CurChunk;

    inline static void operator delete(void *ptr) {
        rcmp_sys.FreeMem(ptr);
    }

    AV_PLAYER(const char *VideoFileName, int BufferSize, LOAD_ENUM LoadMode, SOUND_ENUM SndMode);
    ~AV_PLAYER();
    void Init(const char *VideoFileName, int SizeOfVideoFile, int VideoBufferSize,
              int VideoStreamOffset, const char *AudioFileName, int SizeOfAudioFile,
              int AudioBufferSize, int AudioStreamOffset, LOAD_ENUM LoadMode,
              SOUND_ENUM SndMode);
    FRAME *GetFirstFrame(unsigned int MaxFramesOutstanding, int VideoLatencyInMs);
    FRAME *GetFrame(float GoalFrame);
    bool IsTimeForDecode();
    unsigned int SyncedAudioTime();

    inline int GetVideoStreamHandle() {
        return this->m_VideoStream;
    }

    int SetSpeed(unsigned int speed);
    int Pause();
    int UnPause();
    bool IsAudioFinished();
    int SetVol(unsigned int Vol);
    void GetRCMPChunk(DECODER *decoder, CHUNK **ppdchunk);
    static void StaticGetRCMPChunk(DECODER *decoder, STREAMER *streamer, CHUNK **ppdchunk);
    void ReleaseRCMPChunk(CHUNK *dchunk);
    static void StaticReleaseRCMPChunk(DECODER *decoder, STREAMER *streamer, CHUNK *dchunk);
};

static inline void FileFree(void *d) {
    FILESYSOPTS fso;

    fso.size = 0x34;
    FILE_getopts(&fso);
    fso.allocator->Free(d, 0);
}

AV_PLAYER::AV_PLAYER(const char *VideoFileName, int BufferSize, LOAD_ENUM LoadMode, SOUND_ENUM SndMode)
: m_data_streamer(0) {
    this->Init(VideoFileName, 0, BufferSize, 0, 0, 0, 0, 0, LoadMode, SndMode);
}

void AV_PLAYER::Init(const char *VideoFileName, int SizeOfVideoFile, int VideoBufferSize,
                     int VideoStreamOffset, const char *AudioFileName, int SizeOfAudioFile,
                     int AudioBufferSize, int AudioStreamOffset, LOAD_ENUM LoadMode,
                     SOUND_ENUM SndMode) {
    if (!Snd::System::IsInited()) {
        SndMode = SOUND_OFF;
    }

    MEM_fill(this, 0, sizeof(AV_PLAYER));
    this->m_SndMode = SndMode;
    this->m_LoadMode = LoadMode;
    this->m_CurChunk = 0;
    this->m_VideoData = 0;
    this->m_AyncVideoFileHandle = 0;
    this->m_AudioData = 0;
    this->m_AyncAudioFileHandle = 0;
    this->m_ap = 0;
    this->m_CurFrame = 0;
    this->m_MSTimer = 0;
    this->m_pdecoder = 0;
    this->m_SubtitleArray = 0;
    this->m_CurRCMPFrame = 0;
    this->m_VideoFileName = VideoFileName;
    this->m_AudioFileName = AudioFileName;

    if (!rcmp_sys.IsInited() || rcmp_sys.FreeMemFunc == 0) {
        DEBUG_break();
        return;
    }

    if (AudioFileName != 0) {
        this->m_SndFromDifferentFile = true;
        this->m_VideoStreamBuff = static_cast<unsigned char *>(rcmp_sys.AllocMem(
            lbl_8040FEFC, VideoBufferSize, 0, 0, rcmp_sys.m_DefaultMemDir));
        this->m_AudioStreambuff = static_cast<unsigned char *>(rcmp_sys.AllocMem(
            lbl_8040FF14, AudioBufferSize, 0, 0, rcmp_sys.m_DefaultMemDir));
        this->m_VideoStream = STREAM_create(2, 2, 1, this->m_VideoStreamBuff, VideoBufferSize);
        this->m_AudioStream = STREAM_create(2, 2, 1, this->m_AudioStreambuff, AudioBufferSize);
        STREAM_setpriority(this->m_AudioStream, 0x98, 0x34);
        STREAM_setpriority(this->m_VideoStream, 0x98, 0x34);
        STREAM_setfilter(this->m_VideoStream, 1, 0xff, 77, 1);
        STREAM_setfilter(this->m_VideoStream, 2, 0, 0, -2);
        if (this->m_SndMode == SOUND_ON) {
            STREAM_setfilter(this->m_AudioStream, 1, 0xffff, 0x4353, 1);
        } else {
            STREAM_setfilter(this->m_AudioStream, 1, 0xffff, 0x4353, -1);
        }
        STREAM_setfilter(this->m_AudioStream, 2, 0, 0, -2);

        if (this->m_LoadMode == PRELOAD) {
            this->m_AyncAudioFileHandle = ASYNCFILE_load(
                AudioFileName, rcmp_sys.m_DefaultMemDir);
            this->m_AyncVideoFileHandle = ASYNCFILE_load(
                VideoFileName, rcmp_sys.m_DefaultMemDir);
        } else if (this->m_LoadMode == STREAM) {
            this->m_AudioStreamRequestID = STREAM_queuefile(
                this->m_AudioStream, AudioFileName, AudioStreamOffset, 0);
            this->m_VideoStreamRequestID = STREAM_queuefile(
                this->m_VideoStream, VideoFileName, VideoStreamOffset, 0);
        } else if (this->m_LoadMode == FROM_MEM) {
            this->m_VideoStreamRequestID = STREAM_queuemem(
                this->m_VideoStream, const_cast<char *>(VideoFileName), SizeOfVideoFile, 0);
            this->m_AudioStreamRequestID = STREAM_queuemem(
                this->m_AudioStream, const_cast<char *>(AudioFileName), SizeOfAudioFile, 0);
        }
    } else {
        this->m_SndFromDifferentFile = false;
        this->m_VideoStreamBuff = static_cast<unsigned char *>(rcmp_sys.AllocMem(
            lbl_8040FEFC, VideoBufferSize, 0, 0, rcmp_sys.m_DefaultMemDir));
        this->m_AudioStreambuff = 0;
        this->m_VideoStream = STREAM_create(2, 3, 2, this->m_VideoStreamBuff, VideoBufferSize);
        this->m_AudioStream = STREAM_taphandle(this->m_VideoStream, 2);
        STREAM_setpriority(this->m_AudioStream, 0x98, 0x34);
        STREAM_setpriority(this->m_VideoStream, 0x98, 0x34);
        STREAM_setfilter(this->m_VideoStream, 1, 0xff, 77, 1);
        STREAM_setfilter(this->m_VideoStream, 3, 0, 0, -2);
        if (this->m_SndMode == SOUND_ON) {
            STREAM_setfilter(this->m_VideoStream, 2, 0xffff, 0x4353, 2);
        } else {
            STREAM_setfilter(this->m_VideoStream, 2, 0xffff, 0x4353, -1);
        }

        if (this->m_LoadMode == PRELOAD) {
            this->m_AyncVideoFileHandle = ASYNCFILE_load(
                VideoFileName, rcmp_sys.m_DefaultMemDir);
        } else if (this->m_LoadMode == STREAM) {
            this->m_VideoStreamRequestID = STREAM_queuefile(
                this->m_VideoStream, VideoFileName, VideoStreamOffset, 0);
        } else if (this->m_LoadMode == FROM_MEM) {
            this->m_VideoStreamRequestID = STREAM_queuemem(
                this->m_VideoStream, const_cast<char *>(this->m_VideoFileName),
                SizeOfVideoFile, 0);
        }
    }
}

FRAME *AV_PLAYER::GetFirstFrame(unsigned int MaxFramesOutstanding, int VideoLatencyInMs) {
    STREAM_setpriority(this->m_AudioStream, 0x97, 0x33);
    STREAM_setpriority(this->m_VideoStream, 0x97, 0x33);

    {
        int size;
        int ret;

        if (this->m_AyncVideoFileHandle != 0) {
            ret = ASYNCFILE_release(this->m_AyncVideoFileHandle,
                                    reinterpret_cast<void **>(&this->m_VideoData), &size);
            this->m_VideoStreamRequestID = STREAM_queuemem(
                this->m_VideoStream, this->m_VideoData, size, 0);
        }
    }

    {
        int size;
        int ret;

        if (this->m_AyncAudioFileHandle != 0) {
            ret = ASYNCFILE_release(this->m_AyncAudioFileHandle,
                                    reinterpret_cast<void **>(&this->m_AudioData), &size);
            this->m_AudioStreamRequestID = STREAM_queuemem(
                this->m_AudioStream, this->m_AudioData, size, 0);
        }
    }

    if (!this->m_SndFromDifferentFile) {
        this->m_AudioStreamRequestID = this->m_VideoStreamRequestID;
    }

    if (this->m_SndMode == SOUND_ON) {
        this->m_ap = new AUDIO_PLAYER(this->m_AudioStream, this->m_AudioStreamRequestID);
        this->m_trackingaudio = this->m_ap->GetStreamHandle() >= 0;
    } else {
        this->m_trackingaudio = 0;
    }

    this->m_pdecoder = 0;
    if (rcmp_sys.IsInited() && rcmp_sys.FreeMemFunc != 0) {
        this->m_data_streamer.SetStreamer(this);
        CODEC_IDATA cidata(&this->m_data_streamer, StaticGetRCMPChunk,
                           StaticReleaseRCMPChunk, MaxFramesOutstanding);
        this->m_pdecoder = new DECODER(&cidata);
    }

    this->m_CurRCMPFrame = this->m_pdecoder->GetFrame(0);
    this->m_MSTimer = new AV_MS_TIMER;
    this->m_MSTimer->m_Elapsed = 0;
    this->m_MSTimer->m_Time = OSGetTime();
    this->m_MSTimer->m_TimeBase = 0x1000;
    this->m_MSTimer->Update();
    this->m_refms = this->m_MSTimer->GetMS();
    this->m_filterederror = 0;
    this->m_VideoLatencyInMs = VideoLatencyInMs;
    this->m_oldaudiotime = 0;
    if (this->m_ap != 0) {
        this->m_ap->StartSound();
    }
    this->SetSpeed(0x1000);
    return this->m_CurRCMPFrame;
}

AV_PLAYER::~AV_PLAYER() {
    if (this->m_pdecoder != 0) {
        delete this->m_pdecoder;
        this->m_pdecoder = 0;
    }
    if (this->m_ap != 0) {
        delete this->m_ap;
        this->m_ap = 0;
    }
    if (this->m_MSTimer != 0) {
        delete this->m_MSTimer;
        this->m_MSTimer = 0;
    }

    STREAM_destroy(this->m_VideoStream);
    this->m_VideoStream = 0;
    if (this->m_SndFromDifferentFile) {
        STREAM_destroy(this->m_AudioStream);
        this->m_AudioStream = 0;
    }
    if (this->m_VideoData != 0) {
        FileFree(this->m_VideoData);
        this->m_VideoData = 0;
    }
    if (this->m_AudioData != 0) {
        FileFree(this->m_AudioData);
        this->m_AudioData = 0;
    }
    if (this->m_VideoStreamBuff != 0) {
        rcmp_sys.FreeMem(this->m_VideoStreamBuff);
        this->m_VideoStreamBuff = 0;
    }
    if (this->m_AudioStreambuff != 0) {
        rcmp_sys.FreeMem(this->m_AudioStreambuff);
        this->m_AudioStreambuff = 0;
    }
}

FRAME *AV_PLAYER::GetFrame(float GoalFrame) {
    this->m_GoalFrame = GoalFrame;
    this->m_CurRCMPFrame = this->m_pdecoder->GetFrame(
        GoalFrame < lbl_8040FF30
            ? static_cast<int>(GoalFrame)
            : static_cast<unsigned int>(static_cast<int>(GoalFrame - lbl_8040FF30) ^ 0x80000000));
    this->m_CurFrame = this->m_pdecoder->GetCurrentFrameNumber();
    return this->m_CurRCMPFrame;
}

unsigned int AV_PLAYER::SyncedAudioTime() {
    AV_MS_TIMER &timer = *this->m_MSTimer;
    SNDREQUESTSTATUS status;
    SNDSTREAMSTATUS sndstrmsstatus;
    int audiotime;
    int error;
    int ellapsed;
    unsigned int ms;

    timer.Update();
    ms = timer.GetMS();
    ellapsed = ms - this->m_refms;
    if (this->m_trackingaudio != 0) {
        SNDSYS_entercritical();
        error = SNDSTRM_status(this->m_ap->GetStreamHandle(), &sndstrmsstatus);
        error = SNDSTRM_requeststatus(sndstrmsstatus.currentrequest, &status);
        SNDSYS_leavecritical();
        error = SNDPROFILE_outputlatency();
        audiotime = status.currenttime - error;
        if (audiotime > this->m_oldaudiotime) {
            this->m_oldaudiotime = audiotime;
            error = audiotime - ellapsed;
            this->m_filterederror = this->m_filterederror - this->m_filterederror / 8;
            this->m_filterederror += error;
            error = -this->m_filterederror;
            if (error < this->m_filterederror) {
                error = this->m_filterederror;
            }
            if (error > 0x108) {
                this->m_refms -= this->m_filterederror / 8;
                this->m_filterederror = 0;
                ellapsed = ms - this->m_refms;
            }
        }
    }
    return ellapsed;
}

bool AV_PLAYER::IsTimeForDecode() {
    unsigned int CurTimeMs;

    CurTimeMs = this->SyncedAudioTime();
    this->m_GoalFrame = static_cast<float>(CurTimeMs += this->m_VideoLatencyInMs) * lbl_8040FF48 *
                        this->m_pdecoder->GetFrameRate();
    return this->m_GoalFrame > static_cast<float>(this->m_CurFrame);
}

inline void AV_MS_TIMER::Update() {
    long long time;

    time = OSGetTime();
    if (this->m_TimeBase == 0x1000) {
        this->m_Elapsed += time - this->m_Time;
    } else {
        this->m_Elapsed += (this->m_TimeBase *
                            static_cast<unsigned long long>(time - this->m_Time)) >> 12;
    }
    this->m_Time = time;
}

inline void AV_MS_TIMER::SetSpeed(unsigned int Speed) {
    this->Update();
    this->m_TimeBase = Speed;
}

int AV_PLAYER::SetSpeed(unsigned int Speed) {
    int err;

    if (Speed > 0x4000)
        Speed = 0x4000;
    if (this->m_ap != 0)
        err = this->m_ap->SetSpeed(Speed);
    this->m_MSTimer->SetSpeed(Speed);
    return 0;
}

int AV_PLAYER::Pause() {
    return this->SetSpeed(0);
}

int AV_PLAYER::UnPause() {
    return this->SetSpeed(0x1000);
}

bool AV_PLAYER::IsAudioFinished() {
    if (this->m_ap != 0) {
        return this->m_ap->IsAudioFinished();
    }
    return true;
}

int AV_PLAYER::SetVol(unsigned int Vol) {
    if (this->m_ap != 0)
        return this->m_ap->SetVol(Vol);
    return 0;
}

void AV_PLAYER::StaticGetRCMPChunk(DECODER *decoder, STREAMER *streamer, CHUNK **ppdchunk) {
    AV_PLAYER *avp;

    avp = static_cast<AV_PLAYER *>(streamer->GetStreamer());
    avp->GetRCMPChunk(decoder, ppdchunk);
}

void AV_PLAYER::ReleaseRCMPChunk(CHUNK *dchunk) {
    STREAMCHUNKHDR *chunk;
    int stream;

    chunk = static_cast<STREAMCHUNKHDR *>(dchunk->GetUserChunkData());
    stream = this->GetVideoStreamHandle();
    STREAM_release(stream, chunk);
}

void AV_PLAYER::StaticReleaseRCMPChunk(DECODER *, STREAMER *streamer, CHUNK *dchunk) {
    AV_PLAYER *avp;

    avp = static_cast<AV_PLAYER *>(streamer->GetStreamer());
    avp->ReleaseRCMPChunk(dchunk);
}

void AV_PLAYER::GetRCMPChunk(DECODER *decoder, CHUNK **ppdchunk) {
    CODEC_TYPE codecType;
    STREAMCHUNKHDR *chunk;
    int stream;
    CHUNK *pdchunk;

    stream = this->GetVideoStreamHandle();
    codecType = NONE_CODEC;
    *ppdchunk = 0;
    for (;;) {
        chunk = STREAM_get(stream);
        SYNCTASK_run();
        if (chunk == 0) {
            return;
        }
        {
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
                ++this->m_CurChunk;
                if (this->m_CurChunk > 1) {
                    this->m_CurChunk = 0;
                }
                pdchunk = &this->m_ChunkPool[this->m_CurChunk];
                *ppdchunk = pdchunk;
                pdchunk->SetUserChunkData(chunk);
                if (codecType == MAD_CODEC) {
                    pdchunk->SetSizeOfDataToDecode(geti(chunk, 4));
                    pdchunk->SetDataToDecode(chunk);
                } else if (codecType == VP6_CODEC || codecType == VP6_HEAD_CODEC) {
                    pdchunk->SetSizeOfDataToDecode(geti(chunk, 4));
                    pdchunk->SetDataToDecode(chunk);
                } else {
                    pdchunk->SetSizeOfDataToDecode(geti(chunk, 4));
                    pdchunk->SetDataToDecode(reinterpret_cast<unsigned char *>(chunk) + 8);
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
            }
            *reinterpret_cast<unsigned int *>(temp) = tmp;
            temp[4] = static_cast<char>(codecType);
            STREAM_release(stream, chunk);
            if (STREAM_isendofstream(stream) != 0) {
                return;
            }
        }
    }
}

}
