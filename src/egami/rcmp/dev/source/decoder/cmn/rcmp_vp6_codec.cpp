// ---------------------------------------------------------------------------
// Lectores de campos de la cabecera de chunk. `getm` es big-endian (carga
// directa en GameCube), `geti` little-endian byte a byte: el arbol de `or` del
// original es (p[3]<<24 | p[2]<<16) | p[1]<<8 | p[0], o sea la forma escrita al
// reves.
// ---------------------------------------------------------------------------
static inline unsigned int getm(const void *src, int bytes) {
    return *(const unsigned int *)src;
}

static inline unsigned int geti(const void *src, int bytes) {
    const unsigned char *p = (const unsigned char *)src;

    switch (bytes) {
    case 1:
        return p[0];
    case 2:
        return (p[1] << 8) | p[0];
    case 3:
        return (p[2] << 16) | (p[1] << 8) | p[0];
    default:
        return (p[3] << 24) | (p[2] << 16) | (p[1] << 8) | p[0];
    }
}

void REAL_abortmessage(const char *msg, ...);
void MAD_initdecode(const unsigned short *src, int motion, int quality);
void MAD_decodemacroblock(const unsigned char *src_y, const unsigned char *src_cb,
                          const unsigned char *src_cr, unsigned char *dest_y,
                          unsigned char *dest_cb, unsigned char *dest_cr, int width);

namespace RealShape {

enum OwnedFlag { OWNED_DISABLED = 0, OWNED_ENABLED = 1 };
enum OwnedDataFlag { OWNED_DATA_DISABLED = 0, OWNED_DATA_ENABLED = 1 };
enum CompressedFlag { COMPRESSED_DISABLED = 0, COMPRESSED_ENABLED = 1 };
enum SwizzledFlag { SWIZZLED_DISABLED = 0, SWIZZLED_ENABLED = 1 };
enum MipmontFlag { MIPMONT_DISABLED = 0, MIPMONT_ENABLED = 1 };
enum CubeMapFlag { CUBE_MAP_DISABLED = 0, CUBE_MAP_ENABLED = 1 };
enum Dot3Flag { DOT3_DISABLED = 0, DOT3_ENABLED = 1 };
enum EmbmFlag { EMBM_DISABLED = 0, EMBM_ENABLED = 1 };
enum HalfAlphaFlag { HALF_ALPHA_DISABLED = 0, HALF_ALPHA_ENABLED = 1 };
enum TransparentFlag { TRANSPARENT_DISABLED = 0, TRANSPARENT_ENABLED = 1 };
enum OpaqueFlag { OPAQUE_DISABLED = 0, OPAQUE_ENABLED = 1 };
enum TransposedFlag { TRANSPOSED_DISABLED = 0, TRANSPOSED_ENABLED = 1 };
enum SharedDataFlag { SHARED_DATA_DISABLED = 0, SHARED_DATA_ENABLED = 1 };

enum TexelType {
    TEXEL_TYPE_INVALID = 0,
    TEXEL_TYPE_GC_YUV = 0x1c,
    TEXEL_TYPE_GC_R8G8B8A8 = 0x6c,
};

// total size: 0x4
struct ElementInfo {
    int mInfo; // offset 0x0, size 0x4
};

// total size: 0x10
class ShapeElement {
  public:
    ElementInfo mElementInfo; // offset 0x0, size 0x4
    int mNextOffset;          // offset 0x4, size 0x4
    int mDataOffset;          // offset 0x8, size 0x4
    int mDataSize;            // offset 0xC, size 0x4

    static void Destroy(ShapeElement *shapeElement);

    void *GetData() {
        if (mDataOffset == 0) {
            return 0;
        }
        return (char *)this + mDataOffset;
    }

    void SetData(void *data, int dataSize) {
        if (data == 0) {
            mDataOffset = 0;
            mDataSize = 0;
        } else {
            mDataSize = dataSize;
            mDataOffset = (char *)data - (char *)this;
        }
    }
};

// total size: 0x20
class TextureElement : public ShapeElement {
  public:
    int mShapeX; // offset 0x10
    int mShapeY; // offset 0x14
    int mWidth;  // offset 0x18
    int mHeight; // offset 0x1C

    void SetWidth(int width) {
        mWidth = (unsigned short)width;
    }
    void SetHeight(int height) {
        mHeight = (unsigned short)height;
    }
};

// total size: 0xC0
struct CreateStruct {
    int mSignature;                  // offset 0x0
    int mShapeElements;              // offset 0x4
    OwnedFlag mOwned;                // offset 0x8
    OwnedDataFlag mOwnedData;        // offset 0xC
    CompressedFlag mCompressed;      // offset 0x10
    TexelType mTextureType;          // offset 0x14
    int mWidth;                      // offset 0x18
    int mHeight;                     // offset 0x1C
    int mShapeX;                     // offset 0x20
    int mShapeY;                     // offset 0x24
    int mNumMipmaps;                 // offset 0x28
    SwizzledFlag mSwizzledTexture;   // offset 0x2C
    MipmontFlag mMipmont;            // offset 0x30
    CubeMapFlag mCubeMap;            // offset 0x34
    Dot3Flag mDot3;                  // offset 0x38
    EmbmFlag mEmbm;                  // offset 0x3C
    HalfAlphaFlag mHalfAlpha;        // offset 0x40
    TransparentFlag mTransparent;    // offset 0x44
    OpaqueFlag mOpaque;              // offset 0x48
    TransposedFlag mTransposed;      // offset 0x4C
    SharedDataFlag mTextureSharedData; // offset 0x50
    TexelType mClutType;             // offset 0x54
    SwizzledFlag mSwizzledClut;      // offset 0x58
    int mNumClutColours;             // offset 0x5C
    SharedDataFlag mClutSharedData;  // offset 0x60
    int mClipX;                      // offset 0x64
    int mClipY;                      // offset 0x68
    int mClipWidth;                  // offset 0x6C
    int mClipHeight;                 // offset 0x70
    int mCenterX;                    // offset 0x74
    int mCenterY;                    // offset 0x78
    int mDimension;                  // offset 0x7C
    int mNumHotSpots;                // offset 0x80
    int mEaglBinSize;                // offset 0x84
    int mUserBinSize;                // offset 0x88
    int mUnicode;                    // offset 0x8C
    int mCommentLength;              // offset 0x90
    int mReserved[11];               // offset 0x94

    CreateStruct();
};

// total size: 0x1
class Shape {
  public:
    static Shape *Create(const CreateStruct &createStruct, int memoryDirection);
    TextureElement *GetTexture() const;

    void SetWidth(int width) {
        GetTexture()->SetWidth(width);
    }
    void SetHeight(int height) {
        GetTexture()->SetHeight(height);
    }

    static void Destroy(Shape *shape) {
        ShapeElement::Destroy((ShapeElement *)shape);
    }
};

} // namespace RealShape

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

enum FRAME_TYPE_ENUM {
    FRAME_MPC = 0,
    FRAME_MAD = 1,
    FRAME_PS2_SONY = 2,
    FRAME_VP6 = 3,
};

class FRAME {
  public:
    FRAME_TYPE_ENUM m_FrameType;
    RealShape::Shape *m_Shp;

    FRAME() {}
    ~FRAME() {}
    static void operator delete(void *ptr) {
        rcmp_sys.FreeMem(ptr);
    }
    FRAME_TYPE_ENUM GetFrameType() {
        return m_FrameType;
    }
    RealShape::Shape *GetShape() {
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
    static void operator delete(void *ptr) {
        rcmp_sys.FreeMem(ptr);
    }

    // VEDA (r32): darle *key method* a CODEC --declarar `virtual ~CODEC();`
    // aqui y en rcmp_mad_codec, con `CODEC::~CODEC() {}` fuera de la clase--
    // SI quita la vtable duplicada (_vt.Q24RCMP5CODEC, 64 B: .rodata de
    // rcmp_mad_codec pasa de +288 a +224), pero cuesta 728 B y CUATRO
    // funciones entre las dos unidades. Medido con measure.py --cmp. No sale.
    virtual ~CODEC() {}
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

CODEC *MAD_CODEC_create();

} // namespace RCMP

// ---------------------------------------------------------------------------
// Listas doblemente enlazadas de RealCore.
// ---------------------------------------------------------------------------
class DLNode {
  public:
    DLNode *Next; // offset 0x0
    DLNode *Prev; // offset 0x4

    DLNode() {
        Next = (DLNode *)3;
        Prev = (DLNode *)3;
    }
    ~DLNode() {
        Next = (DLNode *)7;
        Prev = (DLNode *)7;
    }
    DLNode *GetNext() {
        return Next;
    }
    DLNode *GetPrev() {
        return Prev;
    }
    DLNode *Remove() {
        DLNode *next_node = Next;
        DLNode *prev_node = Prev;

        prev_node->Next = next_node;
        next_node->Prev = prev_node;
        Next = (DLNode *)11;
        Prev = (DLNode *)11;
        return this;
    }
    DLNode *AddAfter(DLNode *insert_point) {
        DLNode *new_prev = insert_point;
        DLNode *new_next = insert_point->Next;

        new_prev->Next = this;
        new_next->Prev = this;
        Next = new_next;
        Prev = new_prev;
        return this;
    }
    DLNode *AddBefore(DLNode *insert_point) {
        DLNode *new_next = insert_point;
        DLNode *new_prev = insert_point->Prev;

        new_prev->Next = this;
        new_next->Prev = this;
        Prev = new_prev;
        Next = new_next;
        return this;
    }
};

class DLList {
  public:
    DLNode HeadNode; // offset 0x0

    DLList() {
        HeadNode.Next = &HeadNode;
        HeadNode.Prev = &HeadNode;
    }
    ~DLList() {}
    void InitList() {
        HeadNode.Next = &HeadNode;
        HeadNode.Prev = &HeadNode;
    }
    int IsEmpty() {
        return HeadNode.GetNext() == &HeadNode;
    }
    DLNode *EndOfList() {
        return &HeadNode;
    }
    DLNode *GetHead() {
        return HeadNode.GetNext();
    }
    DLNode *GetTail() {
        return HeadNode.GetPrev();
    }
    DLNode *AddHead(DLNode *node) {
        return node->AddAfter(&HeadNode);
    }
    DLNode *AddTail(DLNode *node) {
        return node->AddBefore(&HeadNode);
    }
    DLNode *Remove(DLNode *node) {
        return node->Remove();
    }
    DLNode *RemoveHead() {
        return GetHead()->Remove();
    }
    DLNode *RemoveTail() {
        return GetTail()->Remove();
    }
};

template <class T>
class TDLNode : public DLNode {
  public:
    TDLNode() {}
    ~TDLNode() {}
    T *Remove() {
        return (T *)DLNode::Remove();
    }
};

template <class T>
class TDLList : public DLList {
  public:
    typedef int TCOMPARE_FUNC(T *, T *);

    TDLList() {
        InitList();
    }
    ~TDLList() {}
    T *AddHead(DLNode *node) {
        return (T *)DLList::AddHead(node);
    }
    T *AddTail(DLNode *node) {
        return (T *)DLList::AddTail(node);
    }
    T *RemoveHead() {
        return (T *)DLList::RemoveHead();
    }
};

namespace EA {

// total size: 0xC
struct TagValuePair {
    unsigned int mTag;
    union {
        int mInt;
        unsigned int mSize;
        float mFloat;
        const void *mPointer;
    } mValue;
    const TagValuePair *mNext;

    TagValuePair() {}
    TagValuePair(unsigned int tag, int value);
};

namespace Allocator {

enum AllocTvpTag {
    ATT_NULL = 0,
};

// total size: 0x4
class IAllocator {
  public:
    virtual void *Alloc(unsigned int size, const TagValuePair &flags) = 0;
    virtual void Free(void *pBlock, unsigned int size) = 0;
    virtual int AddRef() = 0;
    virtual int Release() = 0;

  protected:
    virtual ~IAllocator() {}
    IAllocator() {}
};

} // namespace Allocator
} // namespace EA

namespace Vp6 {
void SetAllocator(EA::Allocator::IAllocator *pAllocator);
}

struct PB_INSTANCE;

// total size: 0x28
struct YUV_BUFFER_CONFIG {
    int YWidth;         // offset 0x0
    int YHeight;        // offset 0x4
    int YStride;        // offset 0x8
    int UVWidth;        // offset 0xC
    int UVHeight;       // offset 0x10
    int UVStride;       // offset 0x14
    char *YBuffer;      // offset 0x18
    char *UBuffer;      // offset 0x1C
    char *VBuffer;      // offset 0x20
    char *YBufferStart; // offset 0x24
};

extern "C" {
void VP6_VPInitLibrary();
void VP6_VPDeInitLibrary();
int VP6_StartDecoder(PB_INSTANCE **ppbi, int width, int height);
void VP6_StopDecoder(PB_INSTANCE **ppbi);
void VP6_SetPbParam(PB_INSTANCE *pbi, int a, int b);
int VP6_DecodeFrameToYUV(PB_INSTANCE *pbi, char *pBitstrm, unsigned int bitstrmLen, int width, int height);
void VP6_GetYUVConfig(PB_INSTANCE *pbi, YUV_BUFFER_CONFIG *yuv);
}

namespace RCMP {
int VP6_CODEC_is_chunk_for_codec(unsigned int chunkType);
}

int RCMP_global_VP6_skipK;
int RCMP_global_VP6_skipK_frameNo;

// total size: 0x14
struct MyAllocator : public EA::Allocator::IAllocator {
    int mRefcount;   // offset 0x4
    int mHighWater;  // offset 0x8
    int mMemUsage;   // offset 0xC
    int mAllocCount; // offset 0x10

    virtual void *Alloc(unsigned int size, const EA::TagValuePair &flags) {
        int *iblock;

        mMemUsage += size;
        if (mMemUsage > mHighWater) {
            mHighWater = mMemUsage;
        }
        mAllocCount++;
        iblock = (int *)RCMP::rcmp_sys.AllocMem("MyAllocator::Alloc", size + 4, 0, 4,
                                                RCMP::rcmp_sys.m_DefaultMemDir);
        *iblock = size;
        return iblock + 1;
    }

    virtual void Free(void *pBlock, unsigned int size) {
        int *iblock = (int *)pBlock - 1;
        int BlockSize = *iblock;

        mMemUsage -= BlockSize;
        RCMP::rcmp_sys.FreeMem(iblock);
    }

    virtual int AddRef() {
        mRefcount++;
        return mRefcount;
    }

    virtual int Release() {
        mRefcount--;
        if (mRefcount == 0) {
            delete this;
        }
        return mRefcount;
    }

    MyAllocator() {
        mRefcount = 1;
        mHighWater = 0;
        mMemUsage = 0;
        mAllocCount = 0;
    }

    virtual ~MyAllocator() {}
};

MyAllocator myalloc;

// total size: 0x8
struct STREAMCHUNKHDR {
    int type; // offset 0x0
    int size; // offset 0x4
};

static int GetChunkType(RCMP::CHUNK *Chunk) {
    if (Chunk != 0) {
        STREAMCHUNKHDR *chunk = (STREAMCHUNKHDR *)Chunk->GetDataToDecode();
        return getm(&chunk->type, 4);
    }

    return 'MV0K';
}

// total size: 0x14
class VP6_FRAME : public RCMP::FRAME, public TDLNode<VP6_FRAME> {
  public:
    int m_ReferenceCount; // offset 0x10

    static void *operator new(unsigned int size) {
        return RCMP::rcmp_sys.AllocMem("", size, 0, 0, RCMP::rcmp_sys.m_DefaultMemDir);
    }
    static void operator delete(void *ptr) {
        RCMP::rcmp_sys.FreeMem(ptr);
    }

    VP6_FRAME(unsigned int Width, unsigned int Height);
    ~VP6_FRAME();
};

VP6_FRAME::VP6_FRAME(unsigned int Width, unsigned int Height) {
    m_ReferenceCount = 0;
    m_FrameType = RCMP::FRAME_VP6;

    RealShape::CreateStruct createStruct;

    createStruct.mShapeElements = 2;
    createStruct.mTextureType = RealShape::TEXEL_TYPE_GC_YUV;
    createStruct.mWidth = Width;
    createStruct.mHeight = Height;
    createStruct.mTextureSharedData = RealShape::SHARED_DATA_ENABLED;
    m_Shp = RealShape::Shape::Create(createStruct, RCMP::rcmp_sys.m_DefaultMemDir);
}

VP6_FRAME::~VP6_FRAME() {
    if (m_Shp != 0) {
        RealShape::Shape::Destroy(m_Shp);
        m_Shp = 0;
    }
}

// total size: 0x38
class VP6_CODEC_INTERNAL : public RCMP::CODEC {
  public:
    PB_INSTANCE *m_pPB_INST;           // offset 0x4
    int m_Width;                       // offset 0x8
    int m_Height;                      // offset 0xC
    int m_YPlaneSize;                  // offset 0x10
    int m_UVPlaneSize;                 // offset 0x14
    float m_FrameRate;                 // offset 0x18
    int m_FirstTime;                   // offset 0x1C
    unsigned int m_CurrentFrameNumber; // offset 0x20
    TDLList<VP6_FRAME> m_UsedFrames;   // offset 0x24
    TDLList<VP6_FRAME> m_FreeFrames;   // offset 0x2C
    RCMP::DECODER *m_Decoder;          // offset 0x34

    enum LAST_FRAME_ENUM {
        NOT_LASTFRAME = 0,
        SECOND_TO_LAST_FRAME = 1,
        MOVIE_OVER = 2,
    };

    static void *operator new(unsigned int size) {
        return RCMP::rcmp_sys.AllocMem("", size, 0, 0, RCMP::rcmp_sys.m_DefaultMemDir);
    }
    static void operator delete(void *ptr) {
        RCMP::rcmp_sys.FreeMem(ptr);
    }

    void CreateFrame(unsigned int Width, unsigned int Height) {
        m_FreeFrames.AddHead(new VP6_FRAME(Width, Height));
    }

    VP6_CODEC_INTERNAL();
    virtual ~VP6_CODEC_INTERNAL();

    virtual RCMP::DETECTED_USABILITY_ENUM Init(RCMP::DECODER *Decoder, RCMP::CHUNK *FirstChunk);
    virtual RCMP::FRAME *GetFrame(unsigned int GoalFrame);
    virtual unsigned int GetCurrentFrameNumber();
    virtual float GetFrameRate();
    virtual void ReleaseFrame(RCMP::FRAME *_Frame);

    VP6_FRAME *DecodeChunk(RCMP::CHUNK *NextChunk);
    VP6_FRAME *GetFrameFromList();
    void GetNextChunk(RCMP::CHUNK **NextChunk);
    void ReleaseChunk(RCMP::CHUNK *NextChunk);
};

VP6_CODEC_INTERNAL::VP6_CODEC_INTERNAL() {
    m_FirstTime = 1;
    m_FrameRate = 0.0f;
    m_Width = 0;
    m_Height = 0;
    m_YPlaneSize = 0;
    m_UVPlaneSize = 0;
    m_pPB_INST = 0;
    RCMP_global_VP6_skipK = 0;
    RCMP_global_VP6_skipK_frameNo = -1;
    Vp6::SetAllocator(&myalloc);
    VP6_VPInitLibrary();
}

VP6_CODEC_INTERNAL::~VP6_CODEC_INTERNAL() {
    while (!m_UsedFrames.IsEmpty()) {
        delete m_UsedFrames.RemoveHead();
    }
    while (!m_FreeFrames.IsEmpty()) {
        delete m_FreeFrames.RemoveHead();
    }
    VP6_StopDecoder(&m_pPB_INST);
    VP6_VPDeInitLibrary();
}

RCMP::DETECTED_USABILITY_ENUM VP6_CODEC_INTERNAL::Init(RCMP::DECODER *Decoder, RCMP::CHUNK *FirstChunk) {
    m_CurrentFrameNumber = 0;
    m_Decoder = Decoder;
    return RCMP::USEABLE;
}

unsigned int VP6_CODEC_INTERNAL::GetCurrentFrameNumber() {
    return m_CurrentFrameNumber;
}

float VP6_CODEC_INTERNAL::GetFrameRate() {
    return m_FrameRate;
}

RCMP::FRAME *VP6_CODEC_INTERNAL::GetFrame(unsigned int GoalFrame) {
    RCMP::CHUNK *NextChunk;

    GetNextChunk(&NextChunk);

    if (m_FirstTime) {
        unsigned int frameRateNumerator;
        unsigned int frameRateDenominator;
        STREAMCHUNKHDR *chunk;
        int MaxFrames;
        int bRet;
        int i;

        chunk = (STREAMCHUNKHDR *)NextChunk->GetDataToDecode();
        m_Width = geti((char *)chunk + 0xC, 2);
        m_Height = geti((char *)chunk + 0xE, 2);
        frameRateNumerator = geti((char *)chunk + 0x18, 4);
        frameRateDenominator = geti((char *)chunk + 0x1C, 2);
        m_YPlaneSize = m_Width * m_Height;
        m_UVPlaneSize = m_YPlaneSize >> 2;
        m_FrameRate = (float)frameRateNumerator / (float)frameRateDenominator;
        MaxFrames = m_Decoder->GetCodecIData()->m_MaxFramesOutstanding;
        for (i = 0; i < MaxFrames + 1; i++) {
            CreateFrame(m_Width, m_Height);
        }
        m_FirstTime = 0;
        bRet = VP6_StartDecoder(&m_pPB_INST, m_Width, m_Height);
        VP6_SetPbParam(m_pPB_INST, 0, 0);
        ReleaseChunk(NextChunk);
        GetNextChunk(&NextChunk);
    } else {
        int frameslate = GoalFrame - m_CurrentFrameNumber;

        if (frameslate > 16) {
            int CurChunkType;

            RCMP_global_VP6_skipK_frameNo = m_CurrentFrameNumber;
            RCMP_global_VP6_skipK++;
            do {
                ReleaseChunk(NextChunk);
                GetNextChunk(&NextChunk);
                CurChunkType = GetChunkType(NextChunk);
            } while (CurChunkType != 'MV0K');
        } else if (frameslate > 0) {
            if (NextChunk != 0) {
                int CurChunkType = GetChunkType(NextChunk);

                if (RCMP::VP6_CODEC_is_chunk_for_codec(CurChunkType)) {
                    char *pBitstrm;
                    unsigned int bitstrmLen;
                    int iRet;

                    pBitstrm = (char *)NextChunk->GetDataToDecode() + 8;
                    bitstrmLen = NextChunk->GetSizeOfDataToDecode() - 8;
                    iRet = VP6_DecodeFrameToYUV(m_pPB_INST, pBitstrm, bitstrmLen, m_Width, m_Height);
                    ReleaseChunk(NextChunk);
                    GetNextChunk(&NextChunk);
                }
            }
        }
    }

    if (NextChunk != 0) {
        VP6_FRAME *CurFrame = DecodeChunk(NextChunk);

        ReleaseChunk(NextChunk);
        return CurFrame;
    }

    return 0;
}

void VP6_CODEC_INTERNAL::ReleaseFrame(RCMP::FRAME *_Frame) {
    VP6_FRAME *Frame = (VP6_FRAME *)_Frame;

    Frame->m_ReferenceCount--;
    if (Frame->m_ReferenceCount <= 0) {
        Frame->Remove();
        m_FreeFrames.AddTail(Frame);
    }
}

VP6_FRAME *VP6_CODEC_INTERNAL::DecodeChunk(RCMP::CHUNK *NextChunk) {
    VP6_FRAME *CurFrame = GetFrameFromList();
    RealShape::Shape *CurShp = CurFrame->GetShape();
    char *pBitstrm;
    unsigned int bitstrmLen;
    int iRet;

    pBitstrm = (char *)NextChunk->GetDataToDecode() + 8;
    bitstrmLen = NextChunk->GetSizeOfDataToDecode() - 8;
    iRet = VP6_DecodeFrameToYUV(m_pPB_INST, pBitstrm, bitstrmLen, m_Width, m_Height);
    if (iRet == 0) {
        YUV_BUFFER_CONFIG yuv;
        char *pBuf;

        VP6_GetYUVConfig(m_pPB_INST, &yuv);
        pBuf = yuv.YBufferStart;
        CurShp->GetTexture()->SetData(pBuf, (m_Height + 0x60) * (m_Width + 0x60) * 3 / 2);
        CurShp->SetWidth(m_Width);
        CurShp->SetHeight(m_Height);
    }

    return CurFrame;
}

VP6_FRAME *VP6_CODEC_INTERNAL::GetFrameFromList() {
    if (!m_FreeFrames.IsEmpty()) {
        VP6_FRAME *NewFrame = m_FreeFrames.RemoveHead();

        m_UsedFrames.AddTail(NewFrame);
        NewFrame->m_ReferenceCount = 1;
        return NewFrame;
    }

    REAL_abortmessage("VP6_CODEC_INTERNAL::GetFrameFromList() out of frames did you call CODEC::ReleaseFrame( FRAME *Frame )
");
    return 0;
}

void VP6_CODEC_INTERNAL::GetNextChunk(RCMP::CHUNK **NextChunk) {
    *NextChunk = m_Decoder->GetChunk();
    m_CurrentFrameNumber++;
}

void VP6_CODEC_INTERNAL::ReleaseChunk(RCMP::CHUNK *NextChunk) {
    if (NextChunk != 0) {
        m_Decoder->ReleaseChunk(NextChunk);
    }
}

namespace RCMP {

CODEC *VP6_CODEC_create() {
    return new VP6_CODEC_INTERNAL;
}

} // namespace RCMP
