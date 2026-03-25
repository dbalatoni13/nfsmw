#ifndef EAGL4ANIM_PHASECHAN_H
#define EAGL4ANIM_PHASECHAN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FnAnimMemoryMap.h"
#include "eagl4supportdef.h"

namespace EAGL4Anim {

// total size: 0xC
struct MatchPhaseInput {
    float mAngle;        // offset 0x0, size 0x4
    float mDAngle;       // offset 0x4, size 0x4
    float mSearchLength; // offset 0x8, size 0x4
};

// total size: 0x4
struct PhaseValue {
    PhaseValue() {}

    float mAngle; // offset 0x0, size 0x4
};

// total size: 0xC
struct PhaseChan : public AnimMemoryMap {
    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    void operator delete(void *ptr, size_t size) {
        EAGL4Internal::EAGL4Free(ptr, size);
    }

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    // void *operator new(size_t, void *ptr) {}

    PhaseChan() {}

    bool IsInvalid() const {
        return (mFlag & 0x4) != 0;
    }

    void SetInvalid(bool s) {
        if (s) {
            mFlag |= 0x4;
        } else {
            mFlag &= ~0x4;
        }
    }

    bool StartWithRight() const {
        return (mFlag & 0x1) != 0;
    }

    void SetStartWithRight(bool s) {
        if (s) {
            mFlag |= 0x1;
        } else {
            mFlag &= ~0x1;
        }
    }

    void SetCycleAnim(bool s) {
        if (s) {
            mFlag |= 0x2;
        } else {
            mFlag &= ~0x2;
        }
    }

    bool IsCycleAnim() const {
        return (mFlag & 0x2) != 0;
    }

    static int ComputeSize(int numCycles, int numFrames, int sampleRateFlag) {}

    int GetSize() const {}

    unsigned char *GetAngles() {
        return &mCycles[mNumCycles > 1 ? mNumCycles : 2];
    }

    const unsigned char *GetAngles() const {
        return &mCycles[mNumCycles > 1 ? mNumCycles : 2];
    }

    void SetAngle(int i, float angle) {}

    float GetAngle(int i) const {}

    int GetAngleSampleRate() const {
        if ((mFlag & 0x8) != 0) {
            return 1;
        }
        if ((mFlag & 0x10) != 0) {
            return 2;
        }
        if ((mFlag & 0x20) != 0) {
            return 4;
        }
        if ((mFlag & 0x40) != 0) {
            return 8;
        }
        return 1;
    }

    int GetNumAngles() const {
        return mNumFrames / GetAngleSampleRate() + 1;
    }

    bool FindMatchTime(const MatchPhaseInput &input, float &time) const;

    unsigned short mNumFrames; // offset 0x4, size 0x2
    unsigned short mStartTime; // offset 0x6, size 0x2
    unsigned char mFlag;       // offset 0x8, size 0x1
    unsigned char mNumCycles;  // offset 0x9, size 0x1
    unsigned char mCycles[2];  // offset 0xA, size 0x2
};

// total size: 0x1C
class FnPhaseChan : public FnAnimMemoryMap {
  public:
    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    // void operator delete(void *ptr, size_t size) {}

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    void *operator new(size_t, void *ptr) {
        return ptr;
    }

    FnPhaseChan()
        : mSampleRate(1), //
          mIdx(0) {
        mType = AnimTypeId::ANIM_PHASE;
    }

    // Overrides: FnAnimSuper
    ~FnPhaseChan() override {}

    // Overrides: FnAnim
    bool GetLength(float &timeLength) const override;

    // Overrides: FnAnim
    void Eval(float, float currTime, float *phaseValue) override;

    // Overrides: FnAnimMemoryMap
    void SetAnimMemoryMap(AnimMemoryMap *anim) override;

    // Overrides: FnAnim
    const PhaseChan *GetPhaseChan() override {
        return reinterpret_cast<const PhaseChan *>(mpAnim);
    }

    // Members
    unsigned short mIdx;          // offset 0x10, size 0x2
    unsigned short mCurrentFrame; // offset 0x12, size 0x2
    bool mRight;                  // offset 0x14, size 0x1
    unsigned char mSampleRate;    // offset 0x18, size 0x1
};

}; // namespace EAGL4Anim

#endif
