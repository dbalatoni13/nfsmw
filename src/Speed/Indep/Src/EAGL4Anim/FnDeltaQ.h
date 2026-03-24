#ifndef EAGL4ANIM_FNDELTAQ_H
#define EAGL4ANIM_FNDELTAQ_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "BoneMask.h"
#include "DeltaQ.h"
#include "FnAnimMemoryMap.h"
#include "MemoryPoolManager.h"
#include "eagl4supportdef.h"

namespace EAGL4Anim {

// total size: 0x30
class FnDeltaQ : public FnAnimMemoryMap {
  public:
    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    void operator delete(void *ptr, size_t size) {
        EAGL4Internal::EAGL4Free(ptr, size);
    }

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    void *operator new(size_t, void *ptr) {
        return ptr;
    }

    FnDeltaQ();

    // Overrides: FnAnimSuper
    ~FnDeltaQ() override {
        if (mPrevQBlock) {
            MemoryPoolManager::DeleteBlock(mPrevQBlock);
        }
    }

    // Overrides: FnAnimMemoryMap
    void SetAnimMemoryMap(AnimMemoryMap *anim) override {
        mpAnim = anim;
    }

    // Overrides: FnAnim
    bool GetLength(float &timeLength) const override {
        DeltaQ *deltaQ = reinterpret_cast<DeltaQ *>(mpAnim);

        timeLength = static_cast<float>(deltaQ->GetNumFrames());
        return true;
    }

    // Overrides: FnAnim
    void Eval(float prevTime, float currTime, float *sqt) override;

    // Overrides: FnAnim
    bool EvalSQT(float currTime, float *sqt, const BoneMask *boneMask) override;

    bool EvalSQTMask(float currTime, float *sqt, const BoneMask *boneMask);

    // Overrides: FnAnim
    bool EvalWeights(float currTime, float *weights) override;

    // Overrides: FnAnim
    bool EvalVel2D(float currTime, float *vel) override;

  protected:
    virtual bool EvalSQTMasked(float currTime, const BoneMask *boneMask, float *sqt);

    void InitBuffersAsRequired() {
        DeltaQ *deltaQ = reinterpret_cast<DeltaQ *>(mpAnim);
        DeltaQMinRange *minRanges;

        deltaQ->GetArrays(minRanges, mBins, mConstBoneIdxs, mConstPhysical);
        mBinSize = deltaQ->GetBinSize();

        if (deltaQ->mNumBones != 0) {
            mPrevQBlock = MemoryPoolManager::NewBlock(deltaQ->mNumBones * sizeof(*mPrevQs));
            mPrevQs = reinterpret_cast<UMath::Vector4 *>(mPrevQBlock);
            mMinRanges = minRanges;
        } else {
            mMinRanges = minRanges;
        }
    }

    DeltaQMinRange *mMinRanges;     // offset 0x10, size 0x4
    unsigned char *mBins;           // offset 0x14, size 0x4
    int mBinSize;                   // offset 0x18, size 0x4
    int mPrevKey;                   // offset 0x1C, size 0x4
    void *mPrevQBlock;              // offset 0x20, size 0x4
    UMath::Vector4 *mPrevQs;        // offset 0x24, size 0x4
    unsigned char *mConstBoneIdxs;  // offset 0x28, size 0x4
    DeltaQPhysical *mConstPhysical; // offset 0x2C, size 0x4
};

}; // namespace EAGL4Anim

#endif
