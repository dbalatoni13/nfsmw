#ifndef EAGL4ANIM_FNDELTAQFAST_H
#define EAGL4ANIM_FNDELTAQFAST_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "BoneMask.h"
#include "DeltaQFast.h"
#include "FnAnimMemoryMap.h"
#include "MemoryPoolManager.h"
#include "eagl4supportdef.h"

namespace EAGL4Anim {

// total size: 0x30
class FnDeltaQFast : public FnAnimMemoryMap {
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

    FnDeltaQFast();

    // Overrides: FnAnimSuper
    ~FnDeltaQFast() override;

    // Overrides: FnAnimMemoryMap
    void SetAnimMemoryMap(AnimMemoryMap *anim) override;

    // Overrides: FnAnim
    bool GetLength(float &timeLength) const override;

    // Overrides: FnAnim
    void Eval(float prevTime, float currTime, float *sqt) override;

    // Overrides: FnAnim
    bool EvalSQT(float currTime, float *sqt, const BoneMask *boneMask) override;

  protected:
    void InitBuffers();

    void AddDelta(DeltaQFastPhysical *floorPhys, DeltaQFast *deltaQ, int prevDeltaIdx, int floorDeltaIdx, UMath::Vector4 *prevQs);

    void SubDelta(DeltaQFastPhysical *floorPhys, DeltaQFast *deltaQ, int prevDeltaIdx, int floorDeltaIdx, UMath::Vector4 *prevQs);

    void UpdateNextQs(DeltaQFast *deltaQ, int ceilKey, int floorBinIdx, int floorDeltaIdx);

    bool EvalSQTMask(float currTime, float *sqt, const BoneMask *boneMask);

    void AddDeltaMask(DeltaQFastPhysical *floorPhys, DeltaQFast *deltaQ, int prevDeltaIdx, int floorDeltaIdx, UMath::Vector4 *prevQs,
                      const BoneMask *boneMask);

    void SubDeltaMask(DeltaQFastPhysical *floorPhys, DeltaQFast *deltaQ, int prevDeltaIdx, int floorDeltaIdx, UMath::Vector4 *prevQs,
                      const BoneMask *boneMask);

    void UpdateNextQsMask(DeltaQFast *deltaQ, int ceilKey, int floorBinIdx, int floorDeltaIdx, const BoneMask *boneMask);

    DeltaQFastMinRangef *mMinRangesf;   // offset 0x10, size 0x4
    unsigned char *mBins;               // offset 0x14, size 0x4
    int mBinSize;                       // offset 0x18, size 0x4
    int mPrevKey;                       // offset 0x1C, size 0x4
    void *mPrevQBlock;                  // offset 0x20, size 0x4
    UMath::Vector4 *mPrevQs;            // offset 0x24, size 0x4
    int mNextKey;                       // offset 0x28, size 0x4
    void *mNextQBlock;                  // offset 0x2C, size 0x4
    UMath::Vector4 *mNextQs;            // offset 0x30, size 0x4
    unsigned char *mConstBoneIdxs;      // offset 0x34, size 0x4
    DeltaQFastPhysical *mConstPhysical; // offset 0x38, size 0x4
    const BoneMask *mBoneMask;          // offset 0x3C, size 0x4
};

}; // namespace EAGL4Anim

#endif
