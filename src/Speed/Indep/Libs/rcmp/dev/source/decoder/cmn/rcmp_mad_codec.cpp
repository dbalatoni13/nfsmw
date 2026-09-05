#include "rcmp_codec_internal.h"

struct STREAMCHUNKHDR {
    unsigned int type;
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
    return 0x4D41446B;
}

MAD_FRAME::MAD_FRAME(unsigned int Width, unsigned int Height)
    : RCMP::FRAME()
    , RCMP::TDLNode<MAD_FRAME>()
    , m_ReferenceCount(0) {
    this->m_FrameType = FRAME_MAD;

    RealShape::CreateStruct createStruct;

    createStruct.mShapeElements = 2;
    createStruct.mTextureType = 0x6c;
    createStruct.mWidth = Width;
    createStruct.mHeight = Height;
    this->m_Shp = RealShape::Shape::Create(createStruct, RCMP::rcmp_sys.m_DefaultMemDir);
}

MAD_FRAME::~MAD_FRAME() {
    if (this->m_Shp != 0) {
        RealShape::Shape::Destroy(this->m_Shp);
        this->m_Shp = 0;
    }
}

MAD_CODEC_INTERNAL::MAD_CODEC_INTERNAL()
    : RCMP::CODEC()
    , m_UsedFrames()
    , m_FreeFrames() {
    this->m_FirstTime = 1;
}

MAD_CODEC_INTERNAL::~MAD_CODEC_INTERNAL() {
    while (!this->m_UsedFrames.IsEmpty()) {
        MAD_FRAME *CurFrame;

        CurFrame = this->m_UsedFrames.RemoveHead();
        if (CurFrame != 0) {
            delete CurFrame;
        }
    }
    while (!this->m_FreeFrames.IsEmpty()) {
        MAD_FRAME *CurFrame;

        CurFrame = this->m_FreeFrames.RemoveHead();
        if (CurFrame != 0) {
            delete CurFrame;
        }
    }
}

unsigned int MAD_CODEC_INTERNAL::GetCurrentFrameNumber() {
    return this->m_CurrentFrameNumber;
}

DETECTED_USABILITY_ENUM MAD_CODEC_INTERNAL::Init(RCMP::DECODER *Decoder, RCMP::CHUNK *) {
    this->m_CurrentFrameNumber = 0;
    this->m_Decoder = Decoder;
    this->m_RefFrame = 0;
    return USEABLE;
}

float MAD_CODEC_INTERNAL::GetFrameRate() {
    return this->m_FrameRate;
}

void MAD_CODEC_INTERNAL::GetNextChunk(RCMP::CHUNK **NextChunk) {
    *NextChunk = this->m_Decoder->GetChunk();
    this->m_CurrentFrameNumber++;
}

void MAD_CODEC_INTERNAL::ReleaseChunk(RCMP::CHUNK *NextChunk) {
    if (NextChunk != 0) {
        this->m_Decoder->ReleaseChunk(NextChunk);
    }
}

MAD_FRAME *MAD_CODEC_INTERNAL::DecodeChunk(RCMP::CHUNK *NextChunk) {
    MAD_FRAME *CurFrame;
    STREAMCHUNKHDR *chunk;
    RCMP::Shape *CurShp;
    unsigned char *CurY;
    unsigned char *CurCb;
    unsigned char *CurCr;
    RCMP::Shape *RefShp;
    unsigned char *RefY;
    unsigned char *RefCb;
    unsigned char *RefCr;
    int i;
    int j;
    int iluma;
    int ichroma;

    CurFrame = 0;
    RefY = 0;
    RefCb = 0;
    RefCr = 0;
    chunk = static_cast<STREAMCHUNKHDR *>(NextChunk->GetDataToDecode());

    if (this->m_RefFrame != 0) {
        RefShp = this->m_RefFrame->GetShape();
        RefY = static_cast<unsigned char *>(RefShp->GetTexture()->GetData());
        RefCb = RefY + this->m_Width * this->m_Height;
        RefCr = RefY + (this->m_Width * (this->m_Height * 4 + this->m_Height)) / 4;
    }

    if (this->m_FirstTime != 0) {
        int MaxFrames;

        this->m_VideoPeriod = geti(reinterpret_cast<unsigned char *>(chunk) + 0xc, 4);
        this->m_FrameRate = lbl_80410334 /
                            (static_cast<float>(this->m_VideoPeriod) * lbl_80410330);
        this->m_Width = geti(reinterpret_cast<unsigned char *>(chunk) + 0x10, 2);
        this->m_Height = geti(reinterpret_cast<unsigned char *>(chunk) + 0x12, 2);
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
    }

    if (GetChunkType(NextChunk) == 0x4D41446B) {
        if (this->m_RefFrame != 0) {
            this->ReleaseFrame(this->m_RefFrame);
        }
        this->m_RefFrame = 0;
        CurFrame = this->CreateKorM();
        MAD_initdecode(reinterpret_cast<const unsigned short *>(reinterpret_cast<char *>(chunk) + 0x18),
                       0, static_cast<signed char>(reinterpret_cast<unsigned char *>(chunk)[0x15]));
    } else if (GetChunkType(NextChunk) == 0x4D41446D) {
        CurFrame = this->CreateKorM();
        MAD_initdecode(reinterpret_cast<const unsigned short *>(reinterpret_cast<char *>(chunk) + 0x18),
                       1, static_cast<signed char>(reinterpret_cast<unsigned char *>(chunk)[0x15]));
    } else if (GetChunkType(NextChunk) == 0x4D414465) {
        CurFrame = this->CreateE();
        MAD_initdecode(reinterpret_cast<const unsigned short *>(reinterpret_cast<char *>(chunk) + 0x18),
                       1, static_cast<signed char>(reinterpret_cast<unsigned char *>(chunk)[0x15]));
    }

    CurShp = CurFrame->GetShape();
    CurY = static_cast<unsigned char *>(CurShp->GetTexture()->GetData());
    CurCb = CurY + this->m_Width * this->m_Height;
    CurCr = CurY + (this->m_Width * (this->m_Height * 4 + this->m_Height)) / 4;

    for (i = 0; i < this->m_Height; i += 0x10) {
        for (j = 0; j < this->m_Width; j += 0x10) {
            iluma = i * this->m_Width + j;
            ichroma = i * this->m_Width / 4 + j / 2;
            MAD_decodemacroblock(RefY + iluma, RefCb + ichroma, RefCr + ichroma,
                                 CurY + iluma, CurCb + ichroma, CurCr + ichroma,
                                 this->m_Width);
        }
    }

    if (GetChunkType(NextChunk) != 0x4D41446D) {
        if (GetChunkType(NextChunk) != 0x4D41446B) {
            return CurFrame;
        }
    }
    if (this->m_RefFrame != 0) {
        this->ReleaseFrame(this->m_RefFrame);
    }
    this->m_RefFrame = CurFrame;
    return CurFrame;
}

RCMP::FRAME *MAD_CODEC_INTERNAL::GetFrame(unsigned int GoalFrame) {
    RCMP::CHUNK *NextChunk;

    this->GetNextChunk(&NextChunk);
    if (this->m_FirstTime == 0) {
        int frameslate;

        frameslate = GoalFrame - this->m_CurrentFrameNumber;
        if (frameslate > 2) {
            int CurChunkType;

            do {
                this->ReleaseChunk(NextChunk);
                this->GetNextChunk(&NextChunk);
                CurChunkType = GetChunkType(NextChunk);
            } while (CurChunkType != 0x4D41446B);
        } else if (frameslate > 0) {
            int i;

            i = frameslate;
            if (i > 0) {
                do {
                if (GetChunkType(NextChunk) == 0x4D414465) {
                    this->ReleaseChunk(NextChunk);
                    this->GetNextChunk(&NextChunk);
                }
                } while (--i != 0);
            }
        }
    }

    {
        MAD_FRAME *CurFrame;

        if (NextChunk != 0) {
            CurFrame = this->DecodeChunk(NextChunk);
            this->ReleaseChunk(NextChunk);
            return CurFrame;
        }
        if (this->m_RefFrame != 0) {
            this->ReleaseFrame(this->m_RefFrame);
            this->m_RefFrame = CurFrame;
        }
    }
    return 0;
}

MAD_FRAME *MAD_CODEC_INTERNAL::CreateKorM() {
    if (!this->m_FreeFrames.IsEmpty()) {
        MAD_FRAME *NewFrame;

        NewFrame = this->m_FreeFrames.RemoveHead();
        this->m_UsedFrames.AddTail(NewFrame);
        NewFrame->m_ReferenceCount = 2;
        return NewFrame;
    }
    REAL_abortmessage(lbl_80410338);
    return 0;
}

MAD_FRAME *MAD_CODEC_INTERNAL::CreateE() {
    if (!this->m_FreeFrames.IsEmpty()) {
        MAD_FRAME *NewFrame;

        NewFrame = this->m_FreeFrames.RemoveHead();
        this->m_UsedFrames.AddTail(NewFrame);
        NewFrame->m_ReferenceCount = 1;
        return NewFrame;
    }
    REAL_abortmessage(lbl_8041039C);
    return 0;
}

void MAD_CODEC_INTERNAL::ReleaseFrame(RCMP::FRAME *_Frame) {
    MAD_FRAME *Frame;

    Frame = static_cast<MAD_FRAME *>(_Frame);
    Frame->m_ReferenceCount--;
    if (Frame->m_ReferenceCount > 0) {
        return;
    }
    this->m_FreeFrames.AddTail(Frame->Remove());
}

namespace RCMP {

CODEC *MAD_CODEC_create() {
    return new MAD_CODEC_INTERNAL;
}

} // namespace RCMP
