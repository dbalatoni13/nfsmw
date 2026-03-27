#ifndef EAGL4ANIM_FNRUNBLENDER_H
#define EAGL4ANIM_FNRUNBLENDER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimMemoryMap.h"
#include "FnAnim.h"
#include "Skeleton.h"
#include "eagl4supportdef.h"

namespace EAGL4Anim {

// total size: 0x80
class FnRunBlender : public FnAnim {
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

    inline float GetOffset() const {
        return mOffset;
    }

    inline void GetAnims(int &numAnims, const AnimMemoryMap **&anims, const PhaseChan **&phases, const AnimMemoryMap **&vels) const {
        numAnims = mNumAnims;
        anims = mAnims;
        phases = mPhases;
        vels = mVels;
    }

    inline float GetWeight() const {
        return mWeight;
    }

    FnRunBlender();

    // Overrides: FnAnimSuper
    ~FnRunBlender() override;

    // Overrides: FnAnim
    bool EvalSQT(float currTime, float *sqtBuffer, const BoneMask *boneMask) override;

    // Overrides: FnAnim
    void Eval(float, float currTime, float *pose) override;

    void SetWeight(float w);

    void SetAnims(Skeleton *s, int numAnims, const AnimMemoryMap **anims, const AnimMemoryMap **phases, const AnimMemoryMap **vels);

    // Overrides: FnAnim
    bool EvalPhase(float currTime, PhaseValue &phase) override;

    // Overrides: FnAnim
    bool EvalVel2D(float currTime, float *vel) override;

    void SetSpeed(float s);

    void GetMinMaxSpeed(float &min, float &max) const;

    float GetFrequency() const;

    void ComputeBeginRootQ(UMath::Vector4 &q) const;

    void ComputeEndRootQ(UMath::Vector4 &q) const;

  private:
    void ComputeRootQ(float t0, float t1, UMath::Vector4 &q) const;
    float CycleTime(float t, float startTime, float endTime) const;
    int ComputeCycleIdx(float t, float startTime, float endTime) const;
    void ComputeAlignQ(float *v1, float *v2, UMath::Vector4 &q) const;
    void AlignCycleBeginEnd(int cIdx);
    void AlignRootQ(float *sqt) const;
    void AlignVel(float *vel) const;
    bool BlendVel(float t0, float t1, float *vel) const;
    bool BlendFacing(float t0, float t1, float *f) const;

    // Overrides: FnAnim
    bool FindMatchTime(const MatchPhaseInput &input, float &time) const override;

    const AnimMemoryMap **mAnims; // offset 0xC, size 0x4
    const PhaseChan **mPhases;    // offset 0x10, size 0x4
    const AnimMemoryMap **mVels;  // offset 0x14, size 0x4
    FnAnim *mFnAnims[2];          // offset 0x18, size 0x8
    FnAnim *mFnVelAnims[2];       // offset 0x20, size 0x8
    float mWeight;                // offset 0x28, size 0x4
    int mNumAnims;                // offset 0x2C, size 0x4
    int mIdx;                     // offset 0x30, size 0x4
    Skeleton *mSkeleton;          // offset 0x34, size 0x4
    float mAlignFrame[2];         // offset 0x38, size 0x8
    float mCycles[2];             // offset 0x40, size 0x8
    float mFreq;                  // offset 0x48, size 0x4
    float mPrevTime;              // offset 0x4C, size 0x4
    float mOffset;                // offset 0x50, size 0x4
    int mCycleIdx;                // offset 0x54, size 0x4
    UMath::Vector4 mAlignQ;       // offset 0x58, size 0x10
    bool mInit;                   // offset 0x68, size 0x1
    UMath::Vector4 mRootQ;        // offset 0x6C, size 0x10
    float *mSpeed;                // offset 0x7C, size 0x4
};

}; // namespace EAGL4Anim

#endif
