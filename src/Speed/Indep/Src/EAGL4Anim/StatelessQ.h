#ifndef EAGL4ANIM_STATELESSQ_H
#define EAGL4ANIM_STATELESSQ_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAGL4Anim/AnimMemoryMap.h"

namespace EAGL4Anim {

// total size: 0x18
struct StatelessQ : public AnimMemoryMap {
    const AttributeBlock *GetAttributeBlock() const {}

    int GetNumFrames() const {}

    unsigned short *GetData() {}

    unsigned short *GetFrameData(unsigned short *dataBuf, int frameIdx) {}

    unsigned short *GetConstData(unsigned short *dataBuf) {}

    unsigned char *GetConstBoneIdx() {}

    static int ComputeSize(int numKeys, int numBones, int numConstBones, bool useKeyFrames) {}

    static unsigned short Compress2UShort(float val) {}

    static float Uncompress2Float(unsigned short val) {}

    static int GetFnOffset() {}

    void *GetFnLocation() {}

    static void InitAnimMemoryMap(AnimMemoryMap *anim);

    AttributeBlock *mAttributeBlock; // offset 0x4, size 0x4
    unsigned short *mTimes;          // offset 0x8, size 0x4
    unsigned char *mBoneIdxs;        // offset 0xC, size 0x4
    void *mF3Ptr;                    // offset 0x10, size 0x4
    unsigned short mNumKeys;         // offset 0x14, size 0x2
    unsigned char mNumBones;         // offset 0x16, size 0x1
    unsigned char mNumConstBones;    // offset 0x17, size 0x1
};

}; // namespace EAGL4Anim

#endif
