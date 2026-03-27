#ifndef EAGL4ANIM_STATELESSF3_H
#define EAGL4ANIM_STATELESSF3_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAGL4Anim/AnimMemoryMap.h"
#include "Speed/Indep/Src/EAGL4Anim/AnimUtil.h"

namespace EAGL4Anim {

// total size: 0x18
struct StatelessF3 : public AnimMemoryMap {
    // total size: 0x20
    struct DofInfo {
        float mMin[4];   // offset 0x0, size 0x10
        float mRange[4]; // offset 0x10, size 0x10
    };

    const AttributeBlock *GetAttributeBlock() const {
        return mAttributeBlock;
    }

    static int ComputeSize(int numBones, int numConst, int numKeys, bool useKeyFrames) {}

    unsigned short GetNumFrames() const {
        if (!mTimes) {
            return mNumKeys;
        } else {
            return mTimes[mNumKeys - 2] + 1;
        }
    }

    void GetArrays(DofInfo *&dofInfo, short *&dataBuf) {
        dofInfo = GetDofInfo();
        dataBuf = GetData();
    }

    DofInfo *GetDofInfo() {
        return reinterpret_cast<DofInfo *>(&this[1]);
    }

    short *GetData() {
        return reinterpret_cast<short *>(&reinterpret_cast<unsigned char *>(GetDofInfo())[mNumBones * sizeof(DofInfo)]);
    }

    short *GetFrameData(short *dataBuf, int frameIdx) {
        return &dataBuf[frameIdx * 3 * mNumBones];
    }

    float *GetConstData(short *dataBuf) {
        return reinterpret_cast<float *>(AlignSize4(reinterpret_cast<intptr_t>(&dataBuf[mNumKeys * 3 * mNumBones])));
    }

    unsigned short *GetConstBoneIdx() {
        return &mDofIdxs[mNumBones];
    }

    void UnQuantize(const DofInfo &dofInfo, const short *frameBuf, UMath::Vector3 &result) const {
        result.x = dofInfo.mRange[0] * frameBuf[0];
        result.y = dofInfo.mRange[1] * frameBuf[1];
        result.z = dofInfo.mRange[2] * frameBuf[2];
    }

    static int GetFnOffset() {
        return 24;
    }

    void *GetFnLocation() {
        return reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(this) - GetFnOffset());
    }

    static void InitAnimMemoryMap(AnimMemoryMap *anim);

    AttributeBlock *mAttributeBlock; // offset 0x4, size 0x4
    unsigned short *mTimes;          // offset 0x8, size 0x4
    unsigned short *mDofIdxs;        // offset 0xC, size 0x4
    unsigned short mNumKeys;         // offset 0x10, size 0x2
    unsigned char mNumBones;         // offset 0x12, size 0x1
    unsigned char mNumConstBones;    // offset 0x13, size 0x1
    unsigned char mPadding[4];       // offset 0x14, size 0x4
};

}; // namespace EAGL4Anim

#endif
