#ifndef EAGL4ANIM_SKELETONDATA_H
#define EAGL4ANIM_SKELETONDATA_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimMemoryMap.h"
#include "AnimationTarget.h"
#include "BoneData.h"
#include "FnAnim.h"

namespace EAGL4Anim {

// total size: 0x10
class SkeletonData : public AnimationTarget {
  public:
    unsigned short GetCoreSkelCheckSum() const {}

    bool IsAnimCompatible(const AnimMemoryMap *a) {}

    bool IsAnimCompatible(const FnAnim *a) {}

    void SetCoreSkelCheckSum(unsigned short cs) {}

    int GetNumBones() const {}

    int GetSize() const {}

    static int ComputeSize(int numBones) {}

    void SetNumBones(int n) {}

    BoneData *GetBoneData() {}

    const BoneData *GetBoneData() const {}

    BoneData &GetBoneData(int i) {}

    const BoneData &GetBoneData(int i) const {}

    const BoneData *GetParentBoneData(const BoneData *b) const {}

    BoneData *GetParentBoneData(const BoneData *b) {}

    int GetBoneIdx(const BoneData *b) {}

    void SetInvBoneScales(float *p) {
        mInvBoneScales = p;
    }

    const float *GetInvBoneScales() const {
        return mInvBoneScales;
    }

    float *GetInvBoneScales() {
        return mInvBoneScales;
    }

    unsigned short GetFlags() const {
        return mFlags;
    }

    void SetFlags(unsigned short f) {
        mFlags = f;
    }

  private:
    unsigned short mCoreSkelCheckSum; // offset 0x4, size 0x2
    unsigned short mFlags;            // offset 0x6, size 0x2
    int mNumBones;                    // offset 0x8, size 0x4
    float *mInvBoneScales;            // offset 0xC, size 0x4
};

}; // namespace EAGL4Anim

#endif
