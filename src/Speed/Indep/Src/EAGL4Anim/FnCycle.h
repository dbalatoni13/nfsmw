#ifndef EAGL4ANIM_FNCYCLE_H
#define EAGL4ANIM_FNCYCLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAGL4Anim/eagl4supportdef.h"
#include "AnimUtil.h"
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
        mLength = endTime - startTime;
    }

    FnAnim *GetAnim() {
        return mpAnim;
    }

    float GetStartTime() const {
        return mStartTime;
    }

    float GetEndTime() const {
        return mEndTime;
    }

  private:
    float GetInRangeTime(float t) const {
        float tmp;
        int n;

        if (t < mStartTime) {
            tmp = t - mStartTime;
            n = FloatToInt(tmp / mLength);
            return mEndTime - (tmp - static_cast<float>(n) * mLength);
        }

        if (t <= mEndTime) {
            return t;
        }

        tmp = t - mEndTime;
        n = FloatToInt(tmp / mLength);
        return mStartTime + (tmp - static_cast<float>(n) * mLength);
    }

  public:
    // Overrides: FnAnim
    void Eval(float previousTime, float currentTime, float *dofs) override {
        mpAnim->Eval(GetInRangeTime(previousTime), GetInRangeTime(currentTime), dofs);
    }

    // Overrides: FnAnim
    bool EvalEvent(float previousTime, float currentTime, EventHandler **eventHandlers, void *extraData) override {
        return mpAnim->EvalEvent(GetInRangeTime(previousTime), GetInRangeTime(currentTime), eventHandlers, extraData);
    }

    // Overrides: FnAnim
    bool EvalSQT(float currentTime, float *sqt, const BoneMask *boneMask) override {
        return mpAnim->EvalSQT(GetInRangeTime(currentTime), sqt, boneMask);
    }

    // Overrides: FnAnim
    bool EvalPhase(float currentTime, PhaseValue &phase) override {
        return mpAnim->EvalPhase(GetInRangeTime(currentTime), phase);
    }

    float mStartTime; // offset 0xC, size 0x4
    float mEndTime;   // offset 0x10, size 0x4
    float mLength;    // offset 0x14, size 0x4
    FnAnim *mpAnim;   // offset 0x18, size 0x4
};

}; // namespace EAGL4Anim

#endif
