#ifndef RCMP_RCMP_H
#define RCMP_RCMP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

extern char lbl_8040FEE0[];

enum FRAME_TYPE_ENUM {
    FRAME_MPC = 0,
    FRAME_MAD = 1,
    FRAME_PS2_SONY = 2,
    FRAME_VP6 = 3,
};

enum DETECTED_USABILITY_ENUM {
    NOT_USEABLE = 0,
    USABILITY_UNSURE = 1,
    USEABLE = 2,
};

namespace RCMP {

struct RCMP_SYSTEM;
struct DECODER;
struct CHUNK;
struct STREAMER;
struct FRAME;

} // namespace RCMP

namespace RealShape {
struct Shape;
}

namespace RCMP {

typedef RealShape::Shape Shape;

extern RCMP_SYSTEM rcmp_sys;

typedef void (*GETDATACALLBACK)(DECODER *, STREAMER *, CHUNK **);
typedef void (*RELEASEDATACALLBACK)(DECODER *, STREAMER *, CHUNK *);

struct RCMP_SYSTEM {
    RCMP_SYSTEM();
    inline virtual ~RCMP_SYSTEM() {}

    inline bool IsInited() {
        return this->AllocMemFunc != 0;
    }

    inline void *AllocMem(const char *name, unsigned int size, int alignment, int headersize, int type) {
        return this->AllocMemFunc(name, size, alignment, headersize, type);
    }

    inline void *AllocMem(const char *name, int size, int alignment, int headersize, int type) {
        return this->AllocMemFunc(name, size, alignment, headersize, type);
    }

    inline void FreeMem(void *memadr) {
        this->FreeMemFunc(memadr);
    }

    void *(*AllocMemFunc)(const char *, int, int, int, int);
    void (*FreeMemFunc)(void *);
    int m_DefaultMemDir;
};

struct CODEC_IDATA {
    inline static void operator delete(void *ptr) {
        rcmp_sys.FreeMem(ptr);
    }

    inline ~CODEC_IDATA() {}

    CODEC_IDATA();
    CODEC_IDATA(
        STREAMER *Streamer,
        GETDATACALLBACK GetDataFunc,
        RELEASEDATACALLBACK ReleaseDataFunc,
        unsigned int NumberOfFramesToBuffer
    );

    STREAMER *m_Streamer;
    GETDATACALLBACK m_GetDataFunc;
    RELEASEDATACALLBACK m_ReleaseDataFunc;
    unsigned int m_MaxFramesOutstanding;
};

struct CODEC {
    inline static void operator delete(void *ptr) {
        rcmp_sys.FreeMem(ptr);
    }

    inline CODEC() {}
    virtual ~CODEC();

    virtual DETECTED_USABILITY_ENUM Init(DECODER *Decoder, CHUNK *FirstChunk);
    virtual FRAME *GetFrame(unsigned int GoalFrame);
    virtual unsigned int GetCurrentFrameNumber();
    virtual float GetFrameRate();
    virtual void ReleaseFrame(FRAME *Frame);
};

class DECODER {
  public:
    inline static void *operator new(unsigned int size) {
        return rcmp_sys.AllocMem(lbl_8040FEE0, size, 0, 0, rcmp_sys.m_DefaultMemDir);
    }

    inline static void operator delete(void *ptr) {
        rcmp_sys.FreeMem(ptr);
    }

    DECODER(const CODEC_IDATA *IData);
    virtual ~DECODER();

    DETECTED_USABILITY_ENUM ChooseCodec(CODEC *codec, CHUNK *FirstChunk);
    void FreeChosenCodec();
    unsigned int GetCurrentFrameNumber();
    float GetFrameRate();
    FRAME *GetFrame(unsigned int GoalFrame);
    void ReleaseFrame(FRAME *Frame);
    CHUNK *GetChunk();
    void ReleaseChunk(CHUNK *Data);

    inline bool HasCodec() {
        return this->m_codec != 0;
    }

    inline CODEC_IDATA *GetCodecIData() {
        return &this->m_IData;
    }

  private:
    CHUNK *m_FirstChunk;
    CODEC_IDATA m_IData;
    CODEC *m_codec;
};

class CHUNK {
  public:
    CHUNK();

    inline static void operator delete(void *ptr) {
        rcmp_sys.FreeMem(ptr);
    }

    inline static void operator delete[](void *ptr) {
        rcmp_sys.FreeMem(ptr);
    }

    inline ~CHUNK() {}

    inline void SetUserChunkData(void *Data) {
        this->m_UserChunkData = Data;
    }

    inline void SetDataToDecode(void *Data) {
        this->m_DataToDecode = Data;
    }

    inline void SetSizeOfDataToDecode(unsigned int DataSize) {
        this->m_DataToDecodeSize = DataSize;
    }

    inline void *GetUserChunkData() {
        return this->m_UserChunkData;
    }

    inline void *GetDataToDecode() {
        return this->m_DataToDecode;
    }

    inline unsigned int GetSizeOfDataToDecode() {
        return this->m_DataToDecodeSize;
    }

  protected:
    void *m_UserChunkData;
    void *m_DataToDecode;
    unsigned int m_DataToDecodeSize;
};

class FRAME {
  public:
    inline static void operator delete(void *ptr) {
        rcmp_sys.FreeMem(ptr);
    }

    inline FRAME() {}
    inline ~FRAME() {}

    inline FRAME_TYPE_ENUM GetFrameType() {
        return this->m_FrameType;
    }

    inline Shape *GetShape() {
        return this->m_Shp;
    }

  protected:
    FRAME_TYPE_ENUM m_FrameType;
    Shape *m_Shp;
};

class STREAMER {
  public:
    inline static void operator delete(void *ptr) {
        rcmp_sys.FreeMem(ptr);
    }

    inline STREAMER(void *Data) {
        this->m_Streamer = Data;
    }

    inline ~STREAMER() {}

    inline void SetStreamer(void *Data) {
        this->m_Streamer = Data;
    }

    inline void *GetStreamer() {
        return this->m_Streamer;
    }

  protected:
    void *m_Streamer;
};

} // namespace RCMP

#endif
