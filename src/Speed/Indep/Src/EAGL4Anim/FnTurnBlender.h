#ifndef EAGL4ANIM_FNTURNBLENDER_H
#define EAGL4ANIM_FNTURNBLENDER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FnAnim.h"
#include "Skeleton.h"
#include "eagl4supportdef.h"

namespace EAGL4Anim {

// total size: 0x5C
class FnTurnBlender : public FnAnim {
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

    FnTurnBlender();

    // Overrides: FnAnimSuper
    ~FnTurnBlender() override;

    // Overrides: FnAnim
    bool EvalSQT(float currTime, float *sqtBuffer, const BoneMask *boneMask) override;

    // Overrides: FnAnim
    void Eval(float, float currTime, float *pose) override;

    void SetWeight(float w);

    void SetAnims(Skeleton *s, int numAnims, FnAnim **anims);

    // Overrides: FnAnim
    bool EvalPhase(float currTime, PhaseValue &phase) override;

    // Overrides: FnAnim
    bool EvalVel2D(float currTime, float *vel) override;

    bool BlendVel(float t0, float t1, float *vel) const;

    float GetFrequency() const;

    float GetWeight() const {
        return mWeight;
    }

    void SetSpeed(float s);

  private:
    float CycleTime(float t, float startTime, float endTime) const;
    int ComputeCycleIdx(float t, float startTime, float endTime) const;
    void ComputeAlignQ(float *v1, float *v2, UMath::Vector4 &q) const;
    void AlignCycleBeginEnd(int cIdx);
    void AlignRootQ(float *sqt) const;
    void AlignVel(float *vel) const;
    bool BlendBeginFacing(float *f) const;
    bool BlendEndFacing(float *f) const;

    FnAnim **mAnims;        // offset 0xC, size 0x4
    FnAnim *mFnAnims[2];    // offset 0x10, size 0x8
    float mWeight;          // offset 0x18, size 0x4
    int mNumAnims;          // offset 0x1C, size 0x4
    int mIdx;               // offset 0x20, size 0x4
    Skeleton *mSkeleton;    // offset 0x24, size 0x4
    float mCycles[2];       // offset 0x28, size 0x8
    float mOffsets[2];      // offset 0x30, size 0x8
    float mFreq;            // offset 0x38, size 0x4
    float mPrevTime;        // offset 0x3C, size 0x4
    float mOffset;          // offset 0x40, size 0x4
    int mCycleIdx;          // offset 0x44, size 0x4
    UMath::Vector4 mAlignQ; // offset 0x48, size 0x10
    bool mInit;             // offset 0x58, size 0x1
};

}; // namespace EAGL4Anim

#endif
