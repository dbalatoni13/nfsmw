#ifndef EAGL4ANIM_FNCYCLE_H
#define EAGL4ANIM_FNCYCLE_H

#include "Speed/Indep/Src/EAGL4Anim/eagl4supportdef.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FnAnim.h"

namespace EAGL4Anim {

// total size: 0x1C
class FnCycle : public FnAnim {
  public:
    FnCycle() {
        mType = AnimTypeId::ANIM_CYCLE;
    }

    // Overrides: FnAnimSuper
    ~FnCycle() override {}

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

    void Set(FnAnim *anim, float startTime, float endTime) {
        mpAnim = anim;
        mStartTime = startTime;
        mEndTime = endTime;
    }

    FnAnim *GetAnim() {
        return mpAnim;
    }

    float GetStartTime() const {}

    float GetEndTime() const {}

    // Overrides: FnAnim
    void Eval(float previousTime, float currentTime, float *dofs) override {}

    // Overrides: FnAnim
    bool EvalEvent(float previousTime, float currentTime, EventHandler **eventHandlers, void *extraData) override {}

    // Overrides: FnAnim
    bool EvalSQT(float currentTime, float *sqt, const BoneMask *boneMask) override {}

    // Overrides: FnAnim
    bool EvalPhase(float currentTime, PhaseValue &phase) override {}

  private:
    float GetInRangeTime(float t) const {}

    float mStartTime; // offset 0xC, size 0x4
    float mEndTime;   // offset 0x10, size 0x4
    float mLength;    // offset 0x14, size 0x4
    FnAnim *mpAnim;   // offset 0x18, size 0x4
};

}; // namespace EAGL4Anim

#endif
