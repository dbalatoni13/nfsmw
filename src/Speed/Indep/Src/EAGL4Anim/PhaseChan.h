#ifndef EAGL4ANIM_PHASECHAN_H
#define EAGL4ANIM_PHASECHAN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FnAnimMemoryMap.h"

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

    // void operator delete(void *ptr, size_t size) {}

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    // void *operator new(size_t, void *ptr) {}

    PhaseChan() {}

    bool IsInvalid() const {}

    void SetInvalid(bool s) {}

    bool StartWithRight() const {}

    void SetStartWithRight(bool s) {}

    void SetCycleAnim(bool s) {}

    bool IsCycleAnim() const {}

    static int ComputeSize(int numCycles, int numFrames, int sampleRateFlag) {}

    int GetSize() const {}

    unsigned char *GetAngles() {}

    const unsigned char *GetAngles() const {}

    void SetAngle(int i, float angle) {}

    float GetAngle(int i) const {}

    int GetAngleSampleRate() const {}

    int GetNumAngles() const {}

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

    // void *operator new(size_t, void *ptr) {}

    FnPhaseChan() {}

    // Overrides: FnAnimSuper
    ~FnPhaseChan() override {}

    // Overrides: FnAnim
    bool GetLength(float &timeLength) const override;

    // Overrides: FnAnim
    void Eval(float, float currTime, float *phaseValue) override;

    // Overrides: FnAnimMemoryMap
    void SetAnimMemoryMap(AnimMemoryMap *anim) override;

    // Overrides: FnAnim
    const PhaseChan *GetPhaseChan() override {}

    // Members
    unsigned short mIdx;          // offset 0x10, size 0x2
    unsigned short mCurrentFrame; // offset 0x12, size 0x2
    bool mRight;                  // offset 0x14, size 0x1
    unsigned char mSampleRate;    // offset 0x18, size 0x1
};

}; // namespace EAGL4Anim

#endif
