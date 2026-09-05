#ifndef RCMP_CODEC_INTERNAL_H
#define RCMP_CODEC_INTERNAL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "rcmp/rcmp.h"
#include <Allocator/iallocator.h>

extern "C" char lbl_80410188[];
extern "C" char lbl_804101A4[];
extern "C" float lbl_804101B8;
extern "C" char lbl_804101C8[];
extern "C" char lbl_80410308[];
extern "C" char lbl_80410338[];
extern "C" char lbl_8041039C[];
extern "C" double lbl_80410328;
extern "C" float lbl_80410330;
extern "C" float lbl_80410334;

extern "C" int RCMP_global_VP6_skipK;
extern "C" int RCMP_global_VP6_skipK_frameNo;

namespace Vp6 {
void SetAllocator(EA::Allocator::IAllocator *allocator);
}

extern "C" void VP6_VPInitLibrary();
extern "C" void VP6_VPDeInitLibrary();
extern "C" int VP6_StopDecoder(void **pbi);
extern "C" int VP6_StartDecoder(void **pbi, unsigned int ImageWidth, unsigned int ImageHeight);
extern "C" void VP6_SetPbParam(void *pbi, int Command, unsigned int Parameter);
extern "C" void VP6_GetYUVConfig(void *pbi, void *yuv);
extern "C" int VP6_DecodeFrameToYUV(void *pbi, char *VideoBufferPtr, unsigned int ByteCount,
                                    unsigned int ImageWidth, unsigned int ImageHeight);
extern "C" void REAL_abortmessage(const char *message, ...);
void MAD_initdecode(const unsigned short *src, int motion, int quality);
void MAD_decodemacroblock(const unsigned char *src_y, const unsigned char *src_cb,
                          const unsigned char *src_cr, unsigned char *dest_y,
                          unsigned char *dest_cb, unsigned char *dest_cr, int width);

namespace RealShape {

struct CreateStruct {
    CreateStruct();

    int mSignature;
    int mShapeElements;
    int mOwned;
    int mOwnedData;
    int mCompressed;
    int mTextureType;
    int mWidth;
    int mHeight;
    int mShapeX;
    int mShapeY;
    int mNumMipmaps;
    int mSwizzledTexture;
    int mMipmont;
    int mCubeMap;
    int mDot3;
    int mEmbm;
    int mHalfAlpha;
    int mTransparent;
    int mOpaque;
    int mTransposed;
    int mTextureSharedData;
    int mClutType;
    int mSwizzledClut;
    int mNumClutColours;
    int mClutSharedData;
    int mClipX;
    int mClipY;
    int mClipWidth;
    int mClipHeight;
    int mCenterX;
    int mCenterY;
    int mDimension;
    int mNumHotSpots;
    int mEaglBinSize;
    int mUserBinSize;
    int mUnicode;
    int mCommentLength;
    int mReserved[11];
};

struct ShapeElement {
    static void Destroy(ShapeElement *shape);

    inline void *GetData() {
        if (this->mDataOffset == 0) {
            return 0;
        }
        return reinterpret_cast<char *>(this) + this->mDataOffset;
    }

    inline void SetData(void *data, int dataSize) {
        if (data == 0) {
            this->mDataOffset = 0;
            this->mDataSize = 0;
        } else {
            this->mDataSize = dataSize;
            this->mDataOffset = static_cast<char *>(data) - reinterpret_cast<char *>(this);
        }
    }

    int mElementInfo;
    int mNextOffset;
    int mDataOffset;
    int mDataSize;
};

struct TextureElement : public ShapeElement {
    inline void SetWidth(int width) {
        this->mWidth = width & 0xffff;
    }

    inline void SetHeight(int height) {
        this->mHeight = height & 0xffff;
    }

    int mShapeX;
    int mShapeY;
    int mWidth;
    int mHeight;
};

struct Shape {
    static Shape *Create(const CreateStruct &createStruct, int memdir);
    TextureElement *GetTexture() const;

    inline void SetWidth(int width) {
        this->GetTexture()->SetWidth(width);
    }

    inline void SetHeight(int height) {
        this->GetTexture()->SetHeight(height);
    }

    static void Destroy(Shape *shape) {
        if (shape != 0) {
            ShapeElement::Destroy(reinterpret_cast<ShapeElement *>(shape));
        }
    }
};

} // namespace RealShape

struct YUV_BUFFER_CONFIG {
    int YWidth;
    int YHeight;
    int YStride;
    int UVWidth;
    int UVHeight;
    int UVStride;
    char *YBuffer;
    char *UBuffer;
    char *VBuffer;
    char *YBufferStart;
};

namespace RCMP {

bool VP6_CODEC_is_chunk_for_codec(unsigned int chunktype);

struct DLNode {
    inline DLNode()
        : Next(reinterpret_cast<DLNode *>(3))
        , Prev(reinterpret_cast<DLNode *>(3)) {
    }

    inline ~DLNode() {
        this->Next = reinterpret_cast<DLNode *>(7);
        this->Prev = reinterpret_cast<DLNode *>(7);
    }

    inline DLNode *GetNext() {
        return this->Next;
    }

    inline DLNode *GetPrev() {
        return this->Prev;
    }

    inline DLNode *Remove() {
        DLNode *next_node;
        DLNode *prev_node;

        next_node = this->Next;
        prev_node = this->Prev;
        prev_node->Next = next_node;
        next_node->Prev = prev_node;
        this->Next = reinterpret_cast<DLNode *>(11);
        this->Prev = reinterpret_cast<DLNode *>(11);
        return this;
    }

    inline DLNode *AddAfter(DLNode *insert_point) {
        DLNode *new_prev;
        DLNode *new_next;

        new_prev = insert_point;
        new_next = insert_point->Next;
        new_prev->Next = this;
        new_next->Prev = this;
        this->Next = new_next;
        this->Prev = new_prev;
        return this;
    }

    inline DLNode *AddBefore(DLNode *insert_point) {
        DLNode *new_next;
        DLNode *new_prev;

        new_next = insert_point;
        new_prev = new_next->Prev;
        new_prev->Next = this;
        this->Next = insert_point;
        this->Prev = new_prev;
        return this;
    }

    DLNode *Next;
    DLNode *Prev;
};

struct DLList {
    inline void InitList() {
        this->HeadNode.Next = &this->HeadNode;
        this->HeadNode.Prev = &this->HeadNode;
    }

    inline DLList()
        : HeadNode() {
        this->InitList();
    }

    inline ~DLList() {
    }

    inline DLNode *GetHead() {
        return this->HeadNode.GetNext();
    }

    inline int IsEmpty() {
        return this->HeadNode.GetNext() == &this->HeadNode;
    }

    inline DLNode *RemoveHead() {
        return this->GetHead()->Remove();
    }

    inline DLNode *AddHead(DLNode *node) {
        return node->AddAfter(&this->HeadNode);
    }

    inline DLNode *AddTail(DLNode *node) {
        return node->AddBefore(&this->HeadNode);
    }

    DLNode HeadNode;
};

template <class T> struct TDLNode : public DLNode {
    inline TDLNode()
        : DLNode() {
    }

    inline T *Remove() {
        return static_cast<T *>(DLNode::Remove());
    }
};

template <class T> struct TDLList : public DLList {
    inline TDLList()
        : DLList() {
    }

    inline T *AddHead(DLNode *node) {
        return static_cast<T *>(DLList::AddHead(node));
    }

    inline T *RemoveHead() {
        return static_cast<T *>(DLList::RemoveHead());
    }

    inline T *AddTail(DLNode *node) {
        return static_cast<T *>(DLList::AddTail(node));
    }
};

} // namespace RCMP

struct VP6_FRAME : public RCMP::FRAME, public RCMP::TDLNode<VP6_FRAME> {
    inline static void *operator new(unsigned int size) {
        return RCMP::rcmp_sys.AllocMem(lbl_80410188, size, 0, 0, RCMP::rcmp_sys.m_DefaultMemDir);
    }

    inline static void operator delete(void *ptr) {
        RCMP::rcmp_sys.FreeMem(ptr);
    }

    VP6_FRAME(unsigned int Width, unsigned int Height);
    ~VP6_FRAME();

    int m_ReferenceCount;
};

struct MAD_FRAME : public RCMP::FRAME, public RCMP::TDLNode<MAD_FRAME> {
    inline static void *operator new(unsigned int size) {
        return RCMP::rcmp_sys.AllocMem(lbl_80410308, size, 0, 0, RCMP::rcmp_sys.m_DefaultMemDir);
    }

    inline static void operator delete(void *ptr) {
        RCMP::rcmp_sys.FreeMem(ptr);
    }

    MAD_FRAME(unsigned int Width, unsigned int Height);
    ~MAD_FRAME();

    int m_ReferenceCount;
};

class MyAllocator : public EA::Allocator::IAllocator {
  public:
    inline virtual void *Alloc(unsigned int size, const EA::TagValuePair &flags) {
        int *iblock;

        this->mMemUsage += size;
        if (this->mMemUsage > this->mHighWater) {
            this->mHighWater = this->mMemUsage;
        }
        this->mAllocCount++;
        iblock = static_cast<int *>(RCMP::rcmp_sys.AllocMem(lbl_804101A4, size + 4, 0, 4,
                                                            RCMP::rcmp_sys.m_DefaultMemDir));
        *iblock = size;
        return iblock + 1;
    }
    inline virtual void Free(void *pBlock, unsigned int size) {
        int *iblock;
        int BlockSize;

        iblock = static_cast<int *>(pBlock) - 1;
        BlockSize = *iblock;
        this->mMemUsage -= BlockSize;
        RCMP::rcmp_sys.FreeMem(iblock);
    }
    inline virtual int AddRef() {
        this->mRefcount++;
        return this->mRefcount;
    }
    inline virtual int Release() {
        this->mRefcount--;
        if (this->mRefcount == 0) {
            if (this != 0) {
                delete this;
            }
        }
        return this->mRefcount;
    }
    inline virtual ~MyAllocator() {
    }

    int mRefcount;
    int mHighWater;
    int mMemUsage;
    int mAllocCount;
};

class VP6_CODEC_INTERNAL : public RCMP::CODEC {
  public:
    inline static void *operator new(unsigned int size) {
        return RCMP::rcmp_sys.AllocMem(lbl_80410188, size, 0, 0, RCMP::rcmp_sys.m_DefaultMemDir);
    }

    VP6_CODEC_INTERNAL();
    virtual ~VP6_CODEC_INTERNAL();
    virtual DETECTED_USABILITY_ENUM Init(RCMP::DECODER *Decoder, RCMP::CHUNK *);
    virtual unsigned int GetCurrentFrameNumber();
    virtual float GetFrameRate();
    virtual RCMP::FRAME *GetFrame(unsigned int GoalFrame);
    virtual void ReleaseFrame(RCMP::FRAME *Frame);

    inline void CreateFrame(unsigned int Height, unsigned int Width) {
        this->m_FreeFrames.AddHead(new VP6_FRAME(Width, Height));
    }

    VP6_FRAME *DecodeChunk(RCMP::CHUNK *NextChunk);
    VP6_FRAME *GetFrameFromList();
    void GetNextChunk(RCMP::CHUNK **NextChunk);
    void ReleaseChunk(RCMP::CHUNK *NextChunk);

    void *m_pPB_INST;
    int m_Width;
    int m_Height;
    int m_YPlaneSize;
    int m_UVPlaneSize;
    float m_FrameRate;
    int m_FirstTime;
    unsigned int m_CurrentFrameNumber;
    RCMP::TDLList<VP6_FRAME> m_UsedFrames;
    RCMP::TDLList<VP6_FRAME> m_FreeFrames;
    RCMP::DECODER *m_Decoder;
};

class MAD_CODEC_INTERNAL : public RCMP::CODEC {
  public:
    inline static void *operator new(unsigned int size) {
        return RCMP::rcmp_sys.AllocMem(lbl_80410308, size, 0, 0, RCMP::rcmp_sys.m_DefaultMemDir);
    }

    MAD_CODEC_INTERNAL();
    virtual ~MAD_CODEC_INTERNAL();
    virtual DETECTED_USABILITY_ENUM Init(RCMP::DECODER *Decoder, RCMP::CHUNK *);
    virtual unsigned int GetCurrentFrameNumber();
    virtual float GetFrameRate();
    virtual RCMP::FRAME *GetFrame(unsigned int GoalFrame);
    virtual void ReleaseFrame(RCMP::FRAME *Frame);

    void GetNextChunk(RCMP::CHUNK **NextChunk);
    void ReleaseChunk(RCMP::CHUNK *NextChunk);
    MAD_FRAME *DecodeChunk(RCMP::CHUNK *NextChunk);
    MAD_FRAME *CreateKorM();
    MAD_FRAME *CreateE();

    inline void CreateFrame(unsigned int Height, unsigned int Width) {
        this->m_FreeFrames.AddHead(new MAD_FRAME(Width, Height));
    }

    inline void MarkFrameInUse(MAD_FRAME *Frame) {
        this->m_UsedFrames.AddTail(Frame->Remove());
    }

    float m_FrameRate;
    int m_VideoPeriod;
    int m_FirstTime;
    int m_Width;
    int m_Height;
    RCMP::FRAME *m_RefFrame;
    unsigned int m_CurrentFrameNumber;
    RCMP::TDLList<MAD_FRAME> m_UsedFrames;
    RCMP::TDLList<MAD_FRAME> m_FreeFrames;
    RCMP::DECODER *m_Decoder;
};

#endif
