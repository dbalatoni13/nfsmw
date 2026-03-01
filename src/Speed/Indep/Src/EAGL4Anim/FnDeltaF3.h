#ifndef EAGL4ANIM_FNDELTAF3_H
#define EAGL4ANIM_FNDELTAF3_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "BoneMask.h"
#include "DeltaF3.h"
#include "FnAnimMemoryMap.h"
#include "MemoryPoolManager.h"
#include "eagl4supportdef.h"

namespace EAGL4Anim {

// total size: 0x30
class FnDeltaF3 : public FnAnimMemoryMap {
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

    FnDeltaF3();

    // Overrides: FnAnimSuper
    ~FnDeltaF3() override {
        if (mPrevValues) {
            MemoryPoolManager::DeleteBlock(mPrevVBlock);
            MemoryPoolManager::DeleteBlock(mNextVBlock);
            MemoryPoolManager::DeleteBlock(mMinRangesf);
        }
    }

    // Overrides: FnAnimMemoryMap
    void SetAnimMemoryMap(AnimMemoryMap *anim) override {
        mpAnim = anim;
        mPrevKey = -1;
        mNextKey = -1;
    }

    // Overrides: FnAnim
    bool GetLength(float &timeLength) const override {
        DeltaF3 *deltaF = reinterpret_cast<DeltaF3 *>(mpAnim);

        timeLength = static_cast<float>(deltaF->GetNumFrames());
        return true;
    }

    // Overrides: FnAnim
    void Eval(float prevTime, float currTime, float *evalBuffer) override;

    // Overrides: FnAnim
    bool EvalSQT(float currTime, float *sqt, const BoneMask *boneMask) override;

    bool EvalSQTMask(float currTime, float *sqt, const BoneMask *boneMask);

    // Overrides: FnAnim
    bool EvalWeights(float currTime, float *weights) override;

    // Overrides: FnAnim
    bool EvalVel2D(float currTime, float *vel) override;

    void InitBuffersAsRequired();

  protected:
    int mPrevKey;                 // offset 0x10, size 0x4
    void *mPrevVBlock;            // offset 0x14, size 0x4
    UMath::Vector4 *mPrevValues;  // offset 0x18, size 0x4 TODO COORD4 typedef
    int mNextKey;                 // offset 0x1C, size 0x4
    void *mNextVBlock;            // offset 0x20, size 0x4
    UMath::Vector4 *mNextValues;  // offset 0x24, size 0x4
    const BoneMask *mBoneMask;    // offset 0x28, size 0x4
    DeltaF3MinRange *mMinRangesf; // offset 0x2C, size 0x4
};

}; // namespace EAGL4Anim

#endif
