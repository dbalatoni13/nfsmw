#ifndef RCMP_H
#define RCMP_H

#include "types.h"

#include "realgraph/shape.h"

namespace RCMP {

// total size: 0x10
class RCMP_SYSTEM {
  public:
    RCMP_SYSTEM();
    virtual ~RCMP_SYSTEM() {}

    bool IsInited() {}

    // Decl: 282
    void *AllocMem(const char *name, unsigned int size, int alignment, int headersize, int type) {
        return this->AllocMemFunc(name, size, alignment, headersize, type);
    }

    void *AllocMem(const char *name, int size, int alignment, int headersize, int type) {
        return this->AllocMemFunc(name, size, alignment, headersize, type);
    }

    void FreeMem(void *memadr) {
        this->FreeMemFunc(memadr);
    }

    void *(*AllocMemFunc)(const char *, int, int, int, int); // offset 0x0, size 0x4
    void (*FreeMemFunc)(void *);                             // offset 0x4, size 0x4
    int m_DefaultMemDir;                                     // offset 0x8, size 0x4
};

extern RCMP_SYSTEM rcmp_sys;

enum FRAME_TYPE_ENUM {
    FRAME_MPC = 0,
    FRAME_MAD = 1,
    FRAME_PS2_SONY = 2,
    FRAME_VP6 = 3,
};

// total size: 0x8
class FRAME {
  public:
    void *operator new(size_t size) {}

    void *operator new[](size_t size) {}

    void *operator new(size_t size, const char *msg, int alignment, int headersize, int type) {}

    void *operator new[](size_t size, const char *msg, int alignment, int headersize, int type) {}

    void operator delete(void *ptr) {}

    void operator delete[](void *ptr) {}

    void *operator new(size_t, void *ptr) {}

    FRAME() {}

    ~FRAME() {}

    FRAME_TYPE_ENUM GetFrameType() {}

    // Decl: 483
    RealShape::Shape *GetShape() {
        return this->m_Shp;
    }

  protected:
    FRAME_TYPE_ENUM m_FrameType; // offset 0x0, size 0x4
    RealShape::Shape *m_Shp;     // offset 0x4, size 0x4
};

// total size: 0xC
class CHUNK {
  public:
    void *operator new(size_t size) {}

    void *operator new[](size_t size) {}

    void *operator new(size_t size, const char *msg, int alignment, int headersize, int type) {}

    void *operator new[](size_t size, const char *msg, int alignment, int headersize, int type) {}

    void operator delete(void *ptr) {}

    void operator delete[](void *ptr) {}

    void *operator new(size_t, void *ptr) {}

    CHUNK();
    ~CHUNK() {}

    void SetUserChunkData(void *Data) {}

    void SetDataToDecode(void *Data) {}

    void SetSizeOfDataToDecode(unsigned int DataSize) {}

    void *GetUserChunkData() {}

    // Decl: 583
    void *GetDataToDecode() {}

    unsigned int GetSizeOfDataToDecode() {}

  protected:
    void *m_UserChunkData;       // offset 0x0, size 0x4
    void *m_DataToDecode;        // offset 0x4, size 0x4
    uint32_t m_DataToDecodeSize; // offset 0x8, size 0x4
};

// total size: 0x4
class STREAMER {
  public:
    void *operator new(size_t size) {}

    void *operator new[](size_t size) {}

    void *operator new(size_t size, const char *msg, int alignment, int headersize, int type) {}

    void *operator new[](size_t size, const char *msg, int alignment, int headersize, int type) {}

    void operator delete(void *ptr) {}

    void operator delete[](void *ptr) {}

    void *operator new(size_t, void *ptr) {}

    // Decl: 622
    STREAMER(void *Data) {}

    ~STREAMER() {}

    void SetStreamer(void *Data) {}

    void *GetStreamer() {}

  protected:
    void *m_Streamer; // offset 0x0, size 0x4
};

class DECODER;

typedef void (*GETDATACALLBACK)(DECODER *, STREAMER *, CHUNK **);
typedef void (*RELEASEDATACALLBACK)(DECODER *, STREAMER *, CHUNK *);

enum DETECTED_USABILITY_ENUM {
    NOT_USEABLE = 0,
    USABILITY_UNSURE = 1,
    USEABLE = 2,
};

// total size: 0x10
struct CODEC_IDATA {
    void *operator new(size_t size) {}

    void *operator new[](size_t size) {}

    void *operator new(size_t size, const char *msg, int alignment, int headersize, int type) {}

    void *operator new[](size_t size, const char *msg, int alignment, int headersize, int type) {}

    void operator delete(void *ptr) {}

    void operator delete[](void *ptr) {}

    void *operator new(size_t, void *ptr) {}

    CODEC_IDATA();
    CODEC_IDATA(GETDATACALLBACK GetDataFunc, RELEASEDATACALLBACK ReleaseDataFunc, unsigned int NumberOfFramesToBuffer);
    ~CODEC_IDATA() {}

    STREAMER *m_Streamer;                  // offset 0x0, size 0x4
    GETDATACALLBACK m_GetDataFunc;         // offset 0x4, size 0x4
    RELEASEDATACALLBACK m_ReleaseDataFunc; // offset 0x8, size 0x4
    uint32_t m_MaxFramesOutstanding;       // offset 0xC, size 0x4
};

// total size: 0x4
class CODEC {
  public:
    void *operator new(size_t size) {}

    void *operator new[](size_t size) {}

    void *operator new(size_t size, const char *msg, int alignment, int headersize, int type) {}

    void *operator new[](size_t size, const char *msg, int alignment, int headersize, int type) {}

    void operator delete(void *ptr) {}

    void operator delete[](void *ptr) {}

    void *operator new(size_t, void *ptr) {}

    virtual ~CODEC() {}

    CODEC() {}
};

// total size: 0x1C
class DECODER {
  public:
    void *operator new(size_t size) {}

    void *operator new[](size_t size) {}

    void *operator new(size_t size, const char *msg, int alignment, int headersize, int type) {}

    void *operator new[](size_t size, const char *msg, int alignment, int headersize, int type) {}

    void operator delete(void *ptr) {}

    void operator delete[](void *ptr) {}

    void *operator new(size_t, void *ptr) {}

    bool HasCodec() {}

    // Decl: 1065
    CODEC_IDATA *GetCodecIData() {}

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

  private:
    CHUNK *m_FirstChunk; // offset 0x0, size 0x4
    CODEC_IDATA m_IData; // offset 0x4, size 0x10
    CODEC *m_codec;      // offset 0x14, size 0x4
};

} // namespace RCMP

#endif
