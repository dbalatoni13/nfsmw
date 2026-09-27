#ifndef AVPLAYER_H
#define AVPLAYER_H

#include "types.h"

#include "rcmp/rcmp.h"

namespace RCMP {

// total size: 0x34
class AV_SUBTITLE {
  public:
    enum JUSTIFY_ENUM {
        JUST_RIGHT = 0,
        JUST_TOP = 1,
        JUST_CENTER = 2,
        JUST_LEFT = 3,
        JUST_BOTTOM = 4,
    };

    void *operator new(size_t size) {}

    void *operator new[](size_t size) {}

    void *operator new(size_t size, const char *msg, int alignment, int headersize, int type) {}

    void *operator new[](size_t size, const char *msg, int alignment, int headersize, int type) {}

    void operator delete(void *ptr) {}

    void operator delete[](void *ptr) {}

    void *operator new(size_t, void *ptr) {}

    AV_SUBTITLE();
    ~AV_SUBTITLE() {}

    void SetUserData0(void *Data0) {}

    void SetUserData1(void *Data1) {}

    void SetUserData2(void *Data2) {}

    int32_t GetPosX() {}

    int32_t GetPosY() {}

    uint32_t GetWidth() {}

    uint32_t GetHeight() {}

    uint32_t GetStrID() {}

    uint32_t GetFrameNumber() {}

    uint8_t GetA() {}

    uint8_t GetR() {}

    uint8_t GetG() {}

    uint8_t GetB() {}

    JUSTIFY_ENUM GetJustifyX() {}

    JUSTIFY_ENUM GetJustifyY() {}

    uint32_t GetShapeIndex() {}

    void *GetUserData0() {}

    void *GetUserData1() {}

    void *GetUserData2() {}

    void SetPosX(int32_t PosX);

    void SetPosY(int32_t PosY);

    void SetWidth(uint32_t Width);

    void SetHeight(uint32_t Height);

    void SetStrID(uint32_t StrID);

    void SetFrameNumber(uint32_t FrameNumber);

    void SetJustifyX(JUSTIFY_ENUM Justify);

    void SetJustifyY(JUSTIFY_ENUM Justify);

    void SetShapeIndex(uint32_t i);

    void SetA(uint32_t a);

    void SetR(uint32_t r);

    void SetG(uint32_t g);

    void SetB(uint32_t b);

  private:
    int32_t mPosX;          // offset 0x0, size 0x4
    int32_t mPosY;          // offset 0x4, size 0x4
    uint32_t mWidth;        // offset 0x8, size 0x4
    uint32_t mHeight;       // offset 0xC, size 0x4
    uint32_t mStrID;        // offset 0x10, size 0x4
    uint32_t mFrameNumber;  // offset 0x14, size 0x4
    JUSTIFY_ENUM mJustifyX; // offset 0x18, size 0x4
    JUSTIFY_ENUM mJustifyY; // offset 0x1C, size 0x4
    uint32_t mShapeIndex;   // offset 0x20, size 0x4
    uint8_t mA;             // offset 0x24, size 0x1
    uint8_t mR;             // offset 0x25, size 0x1
    uint8_t mG;             // offset 0x26, size 0x1
    uint8_t mB;             // offset 0x27, size 0x1
    void *mTexturePtr;      // offset 0x28, size 0x4
    void *mStringPtr;       // offset 0x2C, size 0x4
    void *mWrapStringPtr;   // offset 0x30, size 0x4
};

// total size: 0x8
class AV_SUBTITLE_ARRAY {
  public:
    void *operator new(size_t size) {}

    void *operator new[](size_t size) {}

    void *operator new(size_t size, const char *msg, int alignment, int headersize, int type) {}

    void *operator new[](size_t size, const char *msg, int alignment, int headersize, int type) {}

    void operator delete(void *ptr) {}

    void operator delete[](void *ptr) {}

    void *operator new(size_t, void *ptr) {}

    AV_SUBTITLE_ARRAY();
    ~AV_SUBTITLE_ARRAY();

    void Init(uint32_t NumberOfSubtitle, int32_t x, int32_t y, uint32_t w, uint32_t h);

    AV_SUBTITLE *GetSubtitle(uint32_t Subtitle);

    AV_SUBTITLE *FindSubtitle(uint32_t Frame);

    uint32_t GetNumberOfSubtitle();

    void Init(void *Data, uint32_t DataSize);

  private:
    AV_SUBTITLE *mSubTitle;     // offset 0x0, size 0x4
    uint32_t mNumberOfSubtitle; // offset 0x4, size 0x4
};

//  total size: 0x94
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

    void *operator new(size_t size) {
        return RCMP::rcmp_sys.AllocMem("", static_cast<unsigned int>(sizeof(AV_PLAYER)), 0, 0, RCMP::rcmp_sys.m_DefaultMemDir);
    }

    void *operator new[](size_t size) {}

    void *operator new(size_t size, const char *msg, int alignment, int headersize, int type) {}

    void *operator new[](size_t size, const char *msg, int alignment, int headersize, int type) {}

    void operator delete(void *ptr) {}

    void operator delete[](void *ptr) {}

    void *operator new(size_t, void *ptr) {}

    unsigned int GetCurFrame() {
        return m_CurFrame;
    }

    struct DECODER *GetDecoder() {
        return m_pdecoder;
    }

    float GetGoalFrame() {
        return m_GoalFrame;
    }

    int GetVideoStreamHandle() {}

    AV_PLAYER(const char *VideoFileName, int BufferSize, LOAD_ENUM LoadMode, SOUND_ENUM SndMode);

    AV_PLAYER(const char *VideoFileName, int VideoBufferSize, const char *AudioFileName, int AudioBufferSize, LOAD_ENUM LoadMode, SOUND_ENUM SndMode);

    AV_PLAYER(const char *VideoFileName, int VideoBufferSize, int VideoStreamOffset, const char *AudioFileName, int AudioBufferSize,
              int AudioStreamOffset, LOAD_ENUM LoadMode, SOUND_ENUM SndMode);

    AV_PLAYER(const void *VideoFileData, int SizeOfVideoFile, int VideoBufferSize, const void *AudioFileData, int SizeOfAudioFile,
              int AudioBufferSize, LOAD_ENUM LoadMode, SOUND_ENUM SndMode);

    AV_PLAYER(const void *VideoFileData, int SizeOfVideoFile, int VideoBufferSize, const void *AudioFileData, int SizeOfAudioFile,
              int AudioBufferSize, SOUND_ENUM SndMode);

    void Init(const char *VideoFileName, int SizeOfVideoFile, int VideoBufferSize, int VideoStreamOffset, const char *AudioFileName,
              int SizeOfAudioFile, int AudioBufferSize, int AudioStreamOffset, LOAD_ENUM LoadMode, SOUND_ENUM SndMode);

    void SetSubtitle(struct AV_SUBTITLE_ARRAY *SubtitleArray);

    ~AV_PLAYER();

    struct FRAME *GetFirstFrame(unsigned int MaxFramesOutstanding, int VideoLatencyInMs);

    struct FRAME *GetFrame(float GoalFrame);

    struct AV_SUBTITLE *GetSubtitle();

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

    static void StaticReleaseRCMPChunk(STREAMER *streamer, CHUNK *dchunk);

  private:
    struct AUDIO_PLAYER *m_ap;                  // offset 0x0, size 0x4
    uint8_t *m_VideoStreamBuff;                 // offset 0x4, size 0x4
    uint8_t *m_AudioStreambuff;                 // offset 0x8, size 0x4
    const char *m_VideoFileName;                // offset 0xC, size 0x4
    const char *m_AudioFileName;                // offset 0x10, size 0x4
    bool m_SndFromDifferentFile;                // offset 0x14, size 0x1
    LOAD_ENUM m_LoadMode;                       // offset 0x18, size 0x4
    SOUND_ENUM m_SndMode;                       // offset 0x1C, size 0x4
    uint8_t *m_VideoData;                       // offset 0x20, size 0x4
    int m_AyncVideoFileHandle;                  // offset 0x24, size 0x4
    uint8_t *m_AudioData;                       // offset 0x28, size 0x4
    int m_AyncAudioFileHandle;                  // offset 0x2C, size 0x4
    int m_VideoStream;                          // offset 0x30, size 0x4
    int m_AudioStream;                          // offset 0x34, size 0x4
    int m_VideoStreamRequestID;                 // offset 0x38, size 0x4
    int m_AudioStreamRequestID;                 // offset 0x3C, size 0x4
    int32_t m_VideoLatencyInMs;                 // offset 0x40, size 0x4
    uint32_t m_CurFrame;                        // offset 0x44, size 0x4
    float m_GoalFrame;                          // offset 0x48, size 0x4
    uint32_t m_refms;                           // offset 0x4C, size 0x4
    int32_t m_oldaudiotime;                     // offset 0x50, size 0x4
    int32_t m_trackingaudio;                    // offset 0x54, size 0x4
    int32_t m_filterederror;                    // offset 0x58, size 0x4
    struct AV_MS_TIMER *m_MSTimer;              // offset 0x5C, size 0x4
    STREAMER m_data_streamer;                   // offset 0x60, size 0x4
    DECODER *m_pdecoder;                        // offset 0x64, size 0x4
    struct AV_CHUNK_PARSER *m_VideoChunkParser; // offset 0x68, size 0x4
    struct AV_CHUNK_PARSER *m_AudioChunkParser; // offset 0x6C, size 0x4
    AV_SUBTITLE_ARRAY *m_SubtitleArray;         // offset 0x70, size 0x4
    FRAME *m_CurRCMPFrame;                      // offset 0x74, size 0x4
    CHUNK m_ChunkPool[2];                       // offset 0x78, size 0x18
    int m_CurChunk;                             // offset 0x90, size 0x4
};

}; // namespace RCMP

#endif
