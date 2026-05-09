#ifndef FRONTEND_MOVIEPLAYER_MOVIEPLAYER_H
#define FRONTEND_MOVIEPLAYER_MOVIEPLAYER_H

#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/bWare/Inc/bMemory.hpp"

// TODO move to D:/env/egami/rcmp/dev/source/av/cmn/avplayer.cpp
namespace RCMP {

// total size: 0x1
struct Shape {
    // Functions
    static inline void Destroy(struct Shape *shape) {}

    // inline enum TexelType GetType() const {}

    inline int GetDepth() const {}

    // inline void (*GetMover(enum TexelType targetType))(unsigned char *, const unsigned char *, int) const {}

    inline bool GetConverterList(const struct ConvertStruct &convertStruct, struct Converter *converterList, int *converterListLength) const {}

    inline int GetWidth() const {}

    inline int GetHeight() const {}

    // inline enum CubeMapFlag GetCubeMapFlag() const {}

    // inline enum Dot3Flag GetDot3Flag() const {}

    // inline enum EmbmFlag GetEmbmFlag() const {}

    // inline enum TransparentFlag GetTransparentFlag() const {}

    // inline enum OpaqueFlag GetOpaqueFlag() const {}

    // inline enum CompressedFlag GetCompressedFlag() const {}

    // inline enum TransposedFlag GetTransposedFlag() const {}

    // inline enum MipmontFlag GetMipmontFlag() const {}

    // inline enum SwizzledFlag GetSwizzledFlag() const {}

    inline int GetShapeX() const {}

    inline int GetShapeY() const {}

    inline int GetNumMipmaps() const {}

    // inline enum MipmappedFlag GetMipmappedFlag() const {}

    inline void SetWidth(int width) {}

    inline void SetHeight(int height) {}

    // inline void SetCubeMapFlag(enum CubeMapFlag cubeMap) {}

    // inline void SetDot3Flag(enum Dot3Flag dot3) {}

    // inline void SetEmbmFlag(enum EmbmFlag embm) {}

    // inline void SetTransparentFlag(enum TransparentFlag transparent) {}

    // inline void SetOpaqueFlag(enum OpaqueFlag opaque) {}

    // inline void SetCompressedFlag(enum CompressedFlag compressed) {}

    // inline void SetTransposedFlag(enum TransposedFlag transposed) {}

    // inline void SetMipmontFlag(enum MipmontFlag mipmont) {}

    // inline void SetSwizzledFlag(enum SwizzledFlag swizzled) {}

    inline void SetShapeX(int shapeX) {}

    inline void SetShapeY(int shapeY) {}

    inline void SetNumMipmaps(int numMipmaps) {}

    inline int GetNumColours() const {}

    inline void SetNumColours(int numColours) {}

    inline void GetClipRect(int *x, int *y, int *width, int *height) const {}

    inline void SetClipRect(int x, int y, int width, int height) {}

    inline void *GetEaglBin() const {}

    inline int GetEaglBinSize() const {}

    inline void SetEaglBin(const void *eaglBin, int size) {}

    inline int GetNumHotSpots() const {}

    inline int GetDimension() const {}

    inline int GetCenterX() const {}

    inline int GetCenterY() const {}

    inline void SetCenterX(int centerX) const {}

    inline void SetCenterY(int centerY) const {}

    inline const char *GetCommentString() const {}

    inline void SetCommentString(const char *string) {}
};

// total size: 0x10
class RCMP_SYSTEM {
  public:
    // Functions
    inline virtual ~RCMP_SYSTEM() {}

    inline bool IsInited() {}

    inline void *AllocMem(const char *name, unsigned int size, int alignment, int headersize, int type) {}

    inline void *AllocMem(const char *name, int size, int alignment, int headersize, int type) {}

    inline void FreeMem(void *memadr) {}

    RCMP_SYSTEM();

    // Members
    void *(*AllocMemFunc)(const char *, int, int, int, int); // offset 0x0, size 0x4
    void (*FreeMemFunc)(void *);                             // offset 0x4, size 0x4
    int m_DefaultMemDir;                                     // offset 0x8, size 0x4
};

static RCMP_SYSTEM rcmp_sys;

typedef enum { FRAME_MPC = 0, FRAME_MAD = 1, FRAME_PS2_SONY = 2 } FRAME_TYPE_ENUM;

// total size: 0x8
struct FRAME {
    // Functions
    static inline void *operator new(size_t size) {}

    static inline void *operator new[](size_t size) {}

    static inline void *operator new(size_t size, const char *msg, int alignment, int headersize, int type) {}

    static inline void *operator new[](size_t size, const char *msg, int alignment, int headersize, int type) {}

    static inline void operator delete(void *ptr) {}

    static inline void operator delete[](void *ptr) {}

    static inline void *operator new(size_t, void *ptr) {}

    inline FRAME() {}

    inline ~FRAME() {}

    inline FRAME_TYPE_ENUM GetFrameType() {}

    inline Shape *GetShape() {
        return m_Shp;
    }

    // Members
    FRAME_TYPE_ENUM m_FrameType; // offset 0x0, size 0x4
    Shape *m_Shp;                // offset 0x4, size 0x4
};

// total size: 0xC
struct CHUNK {
    // Functions
    static inline void *operator new(size_t size) {}

    static inline void *operator new[](size_t size) {}

    static inline void *operator new(size_t size, const char *msg, int alignment, int headersize, int type) {}

    static inline void *operator new[](size_t size, const char *msg, int alignment, int headersize, int type) {}

    static inline void operator delete(void *ptr) {}

    static inline void operator delete[](void *ptr) {}

    static inline void *operator new(size_t, void *ptr) {}

    inline ~CHUNK() {}

    inline void SetUserChunkData(void *Data) {}

    inline void SetDataToDecode(void *Data) {}

    inline void SetSizeOfDataToDecode(unsigned int DataSize) {}

    inline void *GetUserChunkData() {}

    inline void *GetDataToDecode() {}

    inline unsigned int GetSizeOfDataToDecode() {}

    CHUNK();

    // Members
    void *m_UserChunkData;           // offset 0x0, size 0x4
    void *m_DataToDecode;            // offset 0x4, size 0x4
    unsigned int m_DataToDecodeSize; // offset 0x8, size 0x4
};

// total size: 0x4
struct STREAMER {
    // Functions
    static inline void *operator new(size_t size) {}

    static inline void *operator new[](size_t size) {}

    static inline void *operator new(size_t size, const char *msg, int alignment, int headersize, int type) {}

    static inline void *operator new[](size_t size, const char *msg, int alignment, int headersize, int type) {}

    static inline void operator delete(void *ptr) {}

    static inline void operator delete[](void *ptr) {}

    static inline void *operator new(size_t, void *ptr) {}

    inline STREAMER(void *Data) {}

    inline ~STREAMER() {}

    inline void SetStreamer(void *Data) {}

    inline void *GetStreamer() {}

    // Members
    void *m_Streamer; // offset 0x0, size 0x4
};

typedef void (*GETDATACALLBACK)(/* parameters unknown */);
typedef void (*RELEASEDATACALLBACK)(/* parameters unknown */);

typedef enum { NOT_USEABLE = 0, USABILITY_UNSURE = 1, USEABLE = 2 } DETECTED_USABILITY_ENUM;

// total size: 0x10
struct CODEC_IDATA {
    // Functions
    static inline void *operator new(size_t size) {}

    static inline void *operator new[](size_t size) {}

    static inline void *operator new(size_t size, const char *msg, int alignment, int headersize, int type) {}

    static inline void *operator new[](size_t size, const char *msg, int alignment, int headersize, int type) {}

    static inline void operator delete(void *ptr) {}

    static inline void operator delete[](void *ptr) {}

    static inline void *operator new(size_t, void *ptr) {}

    inline ~CODEC_IDATA() {}

    CODEC_IDATA();

    CODEC_IDATA(struct STREAMER *Streamer, void (*GetDataFunc)(struct DECODER *, struct STREAMER *, struct CHUNK **),
                void (*ReleaseDataFunc)(struct DECODER *, struct STREAMER *, struct CHUNK *), unsigned int NumberOfFramesToBuffer);

    // Members
    struct STREAMER *m_Streamer;                               // offset 0x0, size 0x4
    void (*m_GetDataFunc)(DECODER *, STREAMER *, CHUNK **);    // offset 0x4, size 0x4
    void (*m_ReleaseDataFunc)(DECODER *, STREAMER *, CHUNK *); // offset 0x8, size 0x4
    unsigned int m_MaxFramesOutstanding;                       // offset 0xC, size 0x4
};

// total size: 0x4
struct CODEC {
    // Functions
    static inline void *operator new(size_t size) {}

    static inline void *operator new[](size_t size) {}

    static inline void *operator new(size_t size, const char *msg, int alignment, int headersize, int type) {}

    static inline void *operator new[](size_t size, const char *msg, int alignment, int headersize, int type) {}

    static inline void operator delete(void *ptr) {}

    static inline void operator delete[](void *ptr) {}

    static inline void *operator new(size_t, void *ptr) {}

    inline virtual ~CODEC() {}

    inline CODEC() {}
};

// total size: 0x1C
struct DECODER {
    // Functions
    static inline void *operator new(size_t size) {}

    static inline void *operator new[](size_t size) {}

    static inline void *operator new(size_t size, const char *msg, int alignment, int headersize, int type) {}

    static inline void *operator new[](size_t size, const char *msg, int alignment, int headersize, int type) {}

    static inline void operator delete(void *ptr) {}

    static inline void operator delete[](void *ptr) {}

    static inline void *operator new(size_t, void *ptr) {}

    inline bool HasCodec() {}

    inline struct CODEC_IDATA *GetCodecIData() {}

    DECODER(const struct CODEC_IDATA *IData);

    virtual ~DECODER();

    DETECTED_USABILITY_ENUM ChooseCodec(struct CODEC *codec, struct CHUNK *FirstChunk);

    void FreeChosenCodec();

    unsigned int GetCurrentFrameNumber();

    float GetFrameRate();

    struct FRAME *GetFrame(unsigned int GoalFrame);

    void ReleaseFrame(struct FRAME *Frame);

    struct CHUNK *GetChunk();

    void ReleaseChunk(struct CHUNK *Data);

    // Members
    struct CHUNK *m_FirstChunk; // offset 0x0, size 0x4
    struct CODEC_IDATA m_IData; // offset 0x4, size 0x10
    struct CODEC *m_codec;      // offset 0x14, size 0x4
};

// total size: 0x34
struct AV_SUBTITLE {
    // Inner declarations
    enum JUSTIFY_ENUM {
        JUST_RIGHT = 0,
        JUST_TOP = 1,
        JUST_CENTER = 2,
        JUST_LEFT = 3,
        JUST_BOTTOM = 4,
    };

    // Functions
    static inline void *operator new(size_t size) {}

    static inline void *operator new[](size_t size) {}

    static inline void *operator new(size_t size, const char *msg, int alignment, int headersize, int type) {}

    static inline void *operator new[](size_t size, const char *msg, int alignment, int headersize, int type) {}

    static inline void operator delete(void *ptr) {}

    static inline void operator delete[](void *ptr) {}

    static inline void *operator new(size_t, void *ptr) {}

    inline ~AV_SUBTITLE() {}

    inline void SetUserData0(void *Data0) {}

    inline void SetUserData1(void *Data1) {}

    inline void SetUserData2(void *Data2) {}

    inline int GetPosX() {}

    inline int GetPosY() {}

    inline unsigned int GetWidth() {}

    inline unsigned int GetHeight() {}

    inline unsigned int GetStrID() {}

    inline unsigned int GetFrameNumber() {}

    inline unsigned char GetA() {}

    inline unsigned char GetR() {}

    inline unsigned char GetG() {}

    inline unsigned char GetB() {}

    inline enum JUSTIFY_ENUM GetJustifyX() {}

    inline enum JUSTIFY_ENUM GetJustifyY() {}

    inline unsigned int GetShapeIndex() {}

    inline void *GetUserData0() {}

    inline void *GetUserData1() {}

    inline void *GetUserData2() {}

    AV_SUBTITLE();

    void SetPosX(int PosX);

    void SetPosY(int PosY);

    void SetWidth(unsigned int Width);

    void SetHeight(unsigned int Height);

    void SetStrID(unsigned int StrID);

    void SetFrameNumber(unsigned int FrameNumber);

    void SetJustifyX(enum JUSTIFY_ENUM Justify);

    void SetJustifyY(enum JUSTIFY_ENUM Justify);

    void SetShapeIndex(unsigned int i);

    void SetA(unsigned int a);

    void SetR(unsigned int r);

    void SetG(unsigned int g);

    void SetB(unsigned int b);

    // Members
    int mPosX;                   // offset 0x0, size 0x4
    int mPosY;                   // offset 0x4, size 0x4
    unsigned int mWidth;         // offset 0x8, size 0x4
    unsigned int mHeight;        // offset 0xC, size 0x4
    unsigned int mStrID;         // offset 0x10, size 0x4
    unsigned int mFrameNumber;   // offset 0x14, size 0x4
    enum JUSTIFY_ENUM mJustifyX; // offset 0x18, size 0x4
    enum JUSTIFY_ENUM mJustifyY; // offset 0x1C, size 0x4
    unsigned int mShapeIndex;    // offset 0x20, size 0x4
    unsigned char mA;            // offset 0x24, size 0x1
    unsigned char mR;            // offset 0x25, size 0x1
    unsigned char mG;            // offset 0x26, size 0x1
    unsigned char mB;            // offset 0x27, size 0x1
    void *mTexturePtr;           // offset 0x28, size 0x4
    void *mStringPtr;            // offset 0x2C, size 0x4
    void *mWrapStringPtr;        // offset 0x30, size 0x4
};

// total size: 0x8
struct AV_SUBTITLE_ARRAY {
    // Functions
    static inline void *operator new(size_t size) {}

    static inline void *operator new[](size_t size) {}

    static inline void *operator new(size_t size, const char *msg, int alignment, int headersize, int type) {}

    static inline void *operator new[](size_t size, const char *msg, int alignment, int headersize, int type) {}

    static inline void operator delete(void *ptr) {}

    static inline void operator delete[](void *ptr) {}

    static inline void *operator new(size_t, void *ptr) {}

    AV_SUBTITLE_ARRAY();

    ~AV_SUBTITLE_ARRAY();

    void Init(unsigned int NumberOfSubtitle, int x, int y, unsigned int w, unsigned int h);

    struct AV_SUBTITLE *GetSubtitle(unsigned int Subtitle);

    struct AV_SUBTITLE *FindSubtitle(unsigned int Frame);

    unsigned int GetNumberOfSubtitle();

    void Init(void *Data, unsigned int DataSize);

    // Members
    struct AV_SUBTITLE *mSubTitle;  // offset 0x0, size 0x4
    unsigned int mNumberOfSubtitle; // offset 0x4, size 0x4
};

//  total size: 0x94
struct AV_PLAYER {
    // Inner declarations
    enum LOAD_ENUM {
        STREAM = 0,
        PRELOAD = 1,
        FROM_MEM = 2,
    };
    enum SOUND_ENUM {
        SOUND_ON = 0,
        SOUND_OFF = 1,
    };

    // Functions
    static inline void *operator new(size_t size) {
        RCMP::rcmp_sys.AllocMem("avplayer", 0x94, 0, 0, RCMP::rcmp_sys.m_DefaultMemDir);
    }

    static inline void *operator new[](size_t size) {}

    static inline void *operator new(size_t size, const char *msg, int alignment, int headersize, int type) {}

    static inline void *operator new[](size_t size, const char *msg, int alignment, int headersize, int type) {}

    static inline void operator delete(void *ptr) {}

    static inline void operator delete[](void *ptr) {}

    static inline void *operator new(size_t, void *ptr) {}

    inline unsigned int GetCurFrame() {}

    inline struct DECODER *GetDecoder() {}

    inline float GetGoalFrame() {
        return m_GoalFrame;
    }

    inline int GetVideoStreamHandle() {}

    AV_PLAYER(const char *VideoFileName, int BufferSize, enum LOAD_ENUM LoadMode, enum SOUND_ENUM SndMode);

    AV_PLAYER(const char *VideoFileName, int VideoBufferSize, const char *AudioFileName, int AudioBufferSize, enum LOAD_ENUM LoadMode,
              enum SOUND_ENUM SndMode);

    AV_PLAYER(const char *VideoFileName, int VideoBufferSize, int VideoStreamOffset, const char *AudioFileName, int AudioBufferSize,
              int AudioStreamOffset, enum LOAD_ENUM LoadMode, enum SOUND_ENUM SndMode);

    AV_PLAYER(const void *VideoFileData, int SizeOfVideoFile, int VideoBufferSize, const void *AudioFileData, int SizeOfAudioFile,
              int AudioBufferSize, enum LOAD_ENUM LoadMode, enum SOUND_ENUM SndMode);

    AV_PLAYER(const void *VideoFileData, int SizeOfVideoFile, int VideoBufferSize, const void *AudioFileData, int SizeOfAudioFile,
              int AudioBufferSize, enum SOUND_ENUM SndMode);

    void Init(const char *VideoFileName, int SizeOfVideoFile, int VideoBufferSize, int VideoStreamOffset, const char *AudioFileName,
              int SizeOfAudioFile, int AudioBufferSize, int AudioStreamOffset, enum LOAD_ENUM LoadMode, enum SOUND_ENUM SndMode);

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

    void GetRCMPChunk(struct DECODER *decoder, struct CHUNK **ppdchunk);

    static void StaticGetRCMPChunk(struct DECODER *decoder, struct STREAMER *streamer, struct CHUNK **ppdchunk);

    void ReleaseRCMPChunk(struct CHUNK *dchunk);

    static void StaticReleaseRCMPChunk(struct STREAMER *streamer, struct CHUNK *dchunk);

    // Members
    struct AUDIO_PLAYER *m_ap;                  // offset 0x0, size 0x4
    unsigned char *m_VideoStreamBuff;           // offset 0x4, size 0x4
    unsigned char *m_AudioStreambuff;           // offset 0x8, size 0x4
    const char *m_VideoFileName;                // offset 0xC, size 0x4
    const char *m_AudioFileName;                // offset 0x10, size 0x4
    bool m_SndFromDifferentFile;                // offset 0x14, size 0x1
    enum LOAD_ENUM m_LoadMode;                  // offset 0x18, size 0x4
    enum SOUND_ENUM m_SndMode;                  // offset 0x1C, size 0x4
    unsigned char *m_VideoData;                 // offset 0x20, size 0x4
    int m_AyncVideoFileHandle;                  // offset 0x24, size 0x4
    unsigned char *m_AudioData;                 // offset 0x28, size 0x4
    int m_AyncAudioFileHandle;                  // offset 0x2C, size 0x4
    int m_VideoStream;                          // offset 0x30, size 0x4
    int m_AudioStream;                          // offset 0x34, size 0x4
    int m_VideoStreamRequestID;                 // offset 0x38, size 0x4
    int m_AudioStreamRequestID;                 // offset 0x3C, size 0x4
    int m_VideoLatencyInMs;                     // offset 0x40, size 0x4
    unsigned int m_CurFrame;                    // offset 0x44, size 0x4
    float m_GoalFrame;                          // offset 0x48, size 0x4
    unsigned int m_refms;                       // offset 0x4C, size 0x4
    int m_oldaudiotime;                         // offset 0x50, size 0x4
    int m_trackingaudio;                        // offset 0x54, size 0x4
    int m_filterederror;                        // offset 0x58, size 0x4
    struct AV_MS_TIMER *m_MSTimer;              // offset 0x5C, size 0x4
    struct STREAMER m_data_streamer;            // offset 0x60, size 0x4
    struct DECODER *m_pdecoder;                 // offset 0x64, size 0x4
    struct AV_CHUNK_PARSER *m_VideoChunkParser; // offset 0x68, size 0x4
    struct AV_CHUNK_PARSER *m_AudioChunkParser; // offset 0x6C, size 0x4
    struct AV_SUBTITLE_ARRAY *m_SubtitleArray;  // offset 0x70, size 0x4
    struct FRAME *m_CurRCMPFrame;               // offset 0x74, size 0x4
    struct CHUNK m_ChunkPool[2];                // offset 0x78, size 0x18
    int m_CurChunk;                             // offset 0x90, size 0x4
};

} // namespace RCMP

// total size: 0x158
class MoviePlayer {
  public:
    // total size: 0x124
    struct Settings {
        unsigned int volume;           // offset 0x0
        unsigned int bufferSize;       // offset 0x4
        unsigned int activeController; // offset 0x8
        int type;                      // offset 0xC
        int movieId;                   // offset 0x10
        bool preload;                  // offset 0x14
        bool sound;                    // offset 0x18
        bool loop;                     // offset 0x1C
        bool pal;                      // offset 0x20
        char filename[256];            // offset 0x24
    };

    Settings mSettings;        // offset 0x0
    unsigned int fCurFrameNum; // offset 0x124
    int fStatus;               // offset 0x128
    int fLiveStatus;           // offset 0x12C
    unsigned int mTicker;      // offset 0x130
    bool mTickerFirstTime;     // offset 0x134
    bool mMoviePaused;         // offset 0x138
    int mili_seconds;          // offset 0x13C
    int seconds;               // offset 0x140
    int minutes;               // offset 0x144
    float milliseconds;        // offset 0x148
    float prevMilliseconds;    // offset 0x14C
    RCMP::AV_PLAYER *fPlayer;  // offset 0x150
    RCMP::FRAME *CurFrame;     // offset 0x154

    RCMP::AV_PLAYER *GetPlayer() {
        return fPlayer;
    }
    bool IsMoviePaused() {
        return mMoviePaused;
    }
    Settings GetSettings() {
        return mSettings;
    }
    int GetStatus() {
        return fStatus;
    }
    int GetLiveStatus() {
        return fLiveStatus;
    }
    bool IsMoviePlaying();
    void FillInTextureInfo(uint32 *framer_address, TextureInfo *texture_info, RCMP::Shape *yuv_shape);

    MoviePlayer(int memClass);
    ~MoviePlayer();
    void Init(Settings &newSettings);
    void ResetTimer();
    void Play();
    void Stop();
    void Pause();
    void UnPause();
    char *const GetMovieFilename();
    int GetMovieCategoryVolume();
    void GetFirstFrame();
    void DisplayTime();
    void Update();
    void UpdateFunction();
    uint32 GetMillisecondsPerFrame();
    void HandleFatalError();
};

extern MoviePlayer *gMoviePlayer;
extern unsigned int gMovieStartTime;

bool MoviePlayer_Bypass();
void MoviePlayer_Play();
void MoviePlayer_StartUp();
void MoviePlayer_ShutDown();
bool GiveTheMoviePlayerBandwidth();

class ShapeMemoryAllocator : public EA::Allocator::IAllocator {
  public:
    ShapeMemoryAllocator() : mRefcount(1) {}
    ~ShapeMemoryAllocator() override {}
    void *Alloc(size_t size, const EA::TagValuePair &flags) override;
    void *Alloc(size_t size);
    void Free(void *pBlock, size_t size) override;
    int AddRef() override;
    int Release() override;

  private:
    int mRefcount; // offset 0x4, size 0x4
};

#endif
