#ifndef EAGL4ANIM_STATELESSF3_H
#define EAGL4ANIM_STATELESSF3_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAGL4Anim/AnimMemoryMap.h"

namespace EAGL4Anim {

// total size: 0x18
struct StatelessF3 : public AnimMemoryMap {
    // total size: 0x20
    struct DofInfo {
        float mMin[4];   // offset 0x0, size 0x10
        float mRange[4]; // offset 0x10, size 0x10
    };

    const AttributeBlock *GetAttributeBlock() const {}

    static int ComputeSize(int numBones, int numConst, int numKeys, bool useKeyFrames) {}

    unsigned short GetNumFrames() const {}

    void GetArrays(DofInfo *&dofInfo, short *&dataBuf) {}

    DofInfo *GetDofInfo() {}

    short *GetData() {}

    short *GetFrameData(short *dataBuf, int frameIdx) {}

    float *GetConstData(short *dataBuf) {}

    unsigned short *GetConstBoneIdx() {}

    void UnQuantize(const DofInfo &dofInfo, const short *frameBuf, UMath::Vector3 &result) const {}

    static int GetFnOffset() {}

    void *GetFnLocation() {}

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
