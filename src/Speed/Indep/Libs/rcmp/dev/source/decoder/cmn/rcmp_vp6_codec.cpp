#include "rcmp_codec_internal.h"

MyAllocator myalloc;
extern "C" {
int RCMP_global_VP6_skipK;
int RCMP_global_VP6_skipK_frameNo;
}

RCMP::CODEC::~CODEC() {
}

struct STREAMCHUNKHDR {
    unsigned int type;
    unsigned char data[0x1c];
};

static inline unsigned int getm(const void *src, int bytes) {
    return *static_cast<const unsigned int *>(src);
}

static inline unsigned int geti(const void *src, int bytes) {
    if (bytes == 2) {
        return (static_cast<const unsigned char *>(src)[1] << 8) |
               static_cast<const unsigned char *>(src)[0];
    }
    return (static_cast<const unsigned char *>(src)[0] << 24) |
           (static_cast<const unsigned char *>(src)[1] << 16) |
           (static_cast<const unsigned char *>(src)[2] << 8) |
           static_cast<const unsigned char *>(src)[3];
}

static int GetChunkType(RCMP::CHUNK *Chunk) {
    if (Chunk != 0) {
        {
            STREAMCHUNKHDR *chunk;

            chunk = static_cast<STREAMCHUNKHDR *>(Chunk->GetDataToDecode());
            return getm(chunk, 4);
        }
    }
    return 0x4D56304B;
}

VP6_FRAME::VP6_FRAME(unsigned int Width, unsigned int Height)
    : RCMP::FRAME()
    , RCMP::TDLNode<VP6_FRAME>()
    , m_ReferenceCount(0) {
    this->m_FrameType = FRAME_VP6;

    RealShape::CreateStruct createStruct;

    createStruct.mShapeElements = 2;
    createStruct.mTextureType = 0x1c;
    createStruct.mWidth = Width;
    createStruct.mHeight = Height;
    createStruct.mTextureSharedData = 1;
    this->m_Shp = RealShape::Shape::Create(createStruct, RCMP::rcmp_sys.m_DefaultMemDir);
}

VP6_FRAME::~VP6_FRAME() {
    if (this->m_Shp != 0) {
        RealShape::Shape::Destroy(this->m_Shp);
        this->m_Shp = 0;
    }
}

VP6_CODEC_INTERNAL::VP6_CODEC_INTERNAL()
    : RCMP::CODEC()
    , m_UsedFrames()
    , m_FreeFrames() {
    this->m_FrameRate = lbl_804101B8;
    this->m_FirstTime = 1;
    this->m_Width = 0;
    this->m_Height = 0;
    this->m_YPlaneSize = 0;
    this->m_UVPlaneSize = 0;
    this->m_pPB_INST = 0;
    RCMP_global_VP6_skipK = 0;
    RCMP_global_VP6_skipK_frameNo = -1;
    Vp6::SetAllocator(&myalloc);
    VP6_VPInitLibrary();
}

VP6_CODEC_INTERNAL::~VP6_CODEC_INTERNAL() {
    int bRet;

    while (!this->m_UsedFrames.IsEmpty()) {
        VP6_FRAME *CurFrame;

        CurFrame = this->m_UsedFrames.RemoveHead();
        if (CurFrame != 0) {
            delete CurFrame;
        }
    }
    while (!this->m_FreeFrames.IsEmpty()) {
        VP6_FRAME *CurFrame;

        CurFrame = this->m_FreeFrames.RemoveHead();
        if (CurFrame != 0) {
            delete CurFrame;
        }
    }
    bRet = VP6_StopDecoder(&this->m_pPB_INST);
    VP6_VPDeInitLibrary();
}

void VP6_CODEC_INTERNAL::ReleaseFrame(RCMP::FRAME *_Frame) {
    VP6_FRAME *Frame;

    Frame = static_cast<VP6_FRAME *>(_Frame);
    Frame->m_ReferenceCount--;
    if (Frame->m_ReferenceCount > 0) {
        return;
    }
    this->m_FreeFrames.AddTail(Frame->Remove());
}

VP6_FRAME *VP6_CODEC_INTERNAL::GetFrameFromList() {
    if (!this->m_FreeFrames.IsEmpty()) {
        VP6_FRAME *NewFrame;

        NewFrame = this->m_FreeFrames.RemoveHead();
        this->m_UsedFrames.AddTail(NewFrame);
        NewFrame->m_ReferenceCount = 1;
        return NewFrame;
    }
    REAL_abortmessage(lbl_804101C8);
    return 0;
}

VP6_FRAME *VP6_CODEC_INTERNAL::DecodeChunk(RCMP::CHUNK *NextChunk) {
    VP6_FRAME *CurFrame;
    RealShape::Shape *CurShp;
    char *pBitstrm;
    unsigned int bitstrmLen;
    int iRet;

    CurFrame = this->GetFrameFromList();
    CurShp = CurFrame->GetShape();
    pBitstrm = static_cast<char *>(NextChunk->GetDataToDecode()) + 8;
    bitstrmLen = NextChunk->GetSizeOfDataToDecode() - 8;
    iRet = VP6_DecodeFrameToYUV(this->m_pPB_INST, pBitstrm, bitstrmLen,
                                this->m_Width, this->m_Height);
    if (iRet == 0) {
        YUV_BUFFER_CONFIG yuv;
        char *pBuf;

        VP6_GetYUVConfig(this->m_pPB_INST, &yuv);
        pBuf = yuv.YBufferStart;
        CurShp->GetTexture()->SetData(
            pBuf, ((this->m_Height + 0x60) * (this->m_Width + 0x60) * 3) / 2);
        CurShp->SetWidth(this->m_Width);
        CurShp->SetHeight(this->m_Height);
    }
    return CurFrame;
}

RCMP::FRAME *VP6_CODEC_INTERNAL::GetFrame(unsigned int GoalFrame) {
    RCMP::CHUNK *NextChunk;

    this->GetNextChunk(&NextChunk);
    if (this->m_FirstTime != 0) {
        unsigned int frameRateNumerator;
        unsigned int frameRateDenominator;
        STREAMCHUNKHDR *chunk;
        int MaxFrames;
        int bRet;

        chunk = static_cast<STREAMCHUNKHDR *>(NextChunk->GetDataToDecode());
        this->m_Width = geti(reinterpret_cast<unsigned char *>(chunk) + 0xc, 2);
        this->m_Height = geti(reinterpret_cast<unsigned char *>(chunk) + 0xe, 2);
        frameRateNumerator = geti(reinterpret_cast<unsigned char *>(chunk) + 0x18, 4);
        frameRateDenominator = geti(reinterpret_cast<unsigned char *>(chunk) + 0x1c, 2);
        this->m_YPlaneSize = this->m_Width * this->m_Height;
        this->m_UVPlaneSize = this->m_YPlaneSize >> 2;
        this->m_FrameRate = static_cast<float>(frameRateNumerator) /
                            static_cast<float>(frameRateDenominator);

        MaxFrames = this->m_Decoder->GetCodecIData()->m_MaxFramesOutstanding;
        {
            int i;

            i = MaxFrames + 1;
            if (i > 0) {
                do {
                    this->CreateFrame(this->m_Height, this->m_Width);
                } while (--i != 0);
            }
        }
        this->m_FirstTime = 0;
        bRet = VP6_StartDecoder(&this->m_pPB_INST, this->m_Width, this->m_Height);
        VP6_SetPbParam(this->m_pPB_INST, 0, 0);
        this->ReleaseChunk(NextChunk);
        this->GetNextChunk(&NextChunk);
    }

    {
        int frameslate;

        frameslate = GoalFrame - this->m_CurrentFrameNumber;
        if (frameslate > 0x10) {
            int CurChunkType;

            do {
                RCMP_global_VP6_skipK_frameNo = this->m_CurrentFrameNumber;
                RCMP_global_VP6_skipK++;
                this->ReleaseChunk(NextChunk);
                this->GetNextChunk(&NextChunk);
                CurChunkType = GetChunkType(NextChunk);
            } while (CurChunkType != 0x4D56304B);
        }
        if (frameslate > 0) {
            int CurChunkType;

            if (NextChunk != 0) {
                CurChunkType = GetChunkType(NextChunk);
                if (RCMP::VP6_CODEC_is_chunk_for_codec(CurChunkType)) {
                    char *pBitstrm;
                    unsigned int bitstrmLen;
                    int iRet;

                    pBitstrm = static_cast<char *>(NextChunk->GetDataToDecode()) + 8;
                    bitstrmLen = NextChunk->GetSizeOfDataToDecode() - 8;
                    iRet = VP6_DecodeFrameToYUV(this->m_pPB_INST, pBitstrm, bitstrmLen,
                                                this->m_Width, this->m_Height);
                    this->ReleaseChunk(NextChunk);
                    this->GetNextChunk(&NextChunk);
                }
            }
        }
    }

    if (NextChunk != 0) {
        VP6_FRAME *CurFrame;

        CurFrame = this->DecodeChunk(NextChunk);
        this->ReleaseChunk(NextChunk);
        return CurFrame;
    }
    return 0;
}

unsigned int VP6_CODEC_INTERNAL::GetCurrentFrameNumber() {
    return this->m_CurrentFrameNumber;
}

DETECTED_USABILITY_ENUM VP6_CODEC_INTERNAL::Init(RCMP::DECODER *Decoder, RCMP::CHUNK *) {
    this->m_CurrentFrameNumber = 0;
    this->m_Decoder = Decoder;
    return USEABLE;
}

float VP6_CODEC_INTERNAL::GetFrameRate() {
    return this->m_FrameRate;
}

void VP6_CODEC_INTERNAL::GetNextChunk(RCMP::CHUNK **NextChunk) {
    *NextChunk = this->m_Decoder->GetChunk();
    this->m_CurrentFrameNumber++;
}

void VP6_CODEC_INTERNAL::ReleaseChunk(RCMP::CHUNK *NextChunk) {
    if (NextChunk != 0) {
        this->m_Decoder->ReleaseChunk(NextChunk);
    }
}

namespace RCMP {

CODEC *VP6_CODEC_create() {
    return new VP6_CODEC_INTERNAL;
}

} // namespace RCMP
