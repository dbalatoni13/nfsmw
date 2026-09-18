namespace RCMP {

class RCMP_SYSTEM {
  public:
    void *(*AllocMemFunc)(const char *, int, int, int, int);
    void (*FreeMemFunc)(void *);
    int m_DefaultMemDir;

    RCMP_SYSTEM();
    virtual ~RCMP_SYSTEM() {}

    bool IsInited() {
        return AllocMemFunc != 0;
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

RCMP_SYSTEM rcmp_sys;

RCMP_SYSTEM::RCMP_SYSTEM() {
    AllocMemFunc = 0;
    FreeMemFunc = 0;
}

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
        return rcmp_sys.AllocMem(0, size, 0, 0, rcmp_sys.m_DefaultMemDir);
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

DECODER::DECODER(const CODEC_IDATA *IData) {
    m_IData = *IData;
    m_codec = 0;
    m_FirstChunk = 0;
}

DECODER::~DECODER() {
    FreeChosenCodec();
}

DETECTED_USABILITY_ENUM DECODER::ChooseCodec(CODEC *codec, CHUNK *FirstChunk) {
    DETECTED_USABILITY_ENUM ret;

    m_codec = codec;
    m_FirstChunk = FirstChunk;
    ret = codec->Init(this, FirstChunk);
    return ret;
}

void DECODER::FreeChosenCodec() {
    if (m_codec != 0) {
        delete m_codec;
        m_codec = 0;
    }
}

unsigned int DECODER::GetCurrentFrameNumber() {
    if (m_codec == 0) {
        return 0;
    }

    return m_codec->GetCurrentFrameNumber();
}

float DECODER::GetFrameRate() {
    if (m_codec == 0) {
        return 0.0f;
    }

    return m_codec->GetFrameRate();
}

FRAME *DECODER::GetFrame(unsigned int GoalFrame) {
    if (m_codec == 0 && m_FirstChunk == 0) {
        GetCodecIData()->m_GetDataFunc(this, GetCodecIData()->m_Streamer, &m_FirstChunk);
    }

    if (m_codec == 0) {
        return 0;
    }
    return m_codec->GetFrame(GoalFrame);
}

void DECODER::ReleaseFrame(FRAME *Frame) {
    m_codec->ReleaseFrame(Frame);
}

CHUNK *DECODER::GetChunk() {
    CHUNK *NextChunk;

    if (m_FirstChunk != 0) {
        NextChunk = m_FirstChunk;
        m_FirstChunk = 0;
        return NextChunk;
    }

    GetCodecIData()->m_GetDataFunc(this, GetCodecIData()->m_Streamer, &NextChunk);
    return NextChunk;
}

void DECODER::ReleaseChunk(CHUNK *Data) {
    GetCodecIData()->m_ReleaseDataFunc(this, GetCodecIData()->m_Streamer, Data);
}

CHUNK::CHUNK() {
    m_DataToDecode = 0;
    m_DataToDecodeSize = 0;
    m_UserChunkData = 0;
}

CODEC_IDATA::CODEC_IDATA() {
    m_MaxFramesOutstanding = 2;
    m_Streamer = 0;
    m_GetDataFunc = 0;
    m_ReleaseDataFunc = 0;
}

CODEC_IDATA::CODEC_IDATA(STREAMER *Streamer, GETDATACALLBACK GetDataFunc, RELEASEDATACALLBACK ReleaseDataFunc, unsigned int NumberOfFramesToBuffer) {
    m_MaxFramesOutstanding = NumberOfFramesToBuffer;
    m_Streamer = Streamer;
    m_GetDataFunc = GetDataFunc;
    m_ReleaseDataFunc = ReleaseDataFunc;
}

} // namespace RCMP
