#ifndef EAGL4ANIM_FNEVENTBLENDER_H
#define EAGL4ANIM_FNEVENTBLENDER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FnAnim.h"
#include "eagl4supportdef.h"

namespace EAGL4Anim {

// total size: 0x2C
class FnEventBlender : public FnAnim {
  public:
    enum TriggerType {
        FIRST_ONLY = 0,
        SECOND_ONLY = 1,
        BOTH = 2,
    };

    // Functions
    FnEventBlender() {
        mType = AnimTypeId::ANIM_EVENTBLENDER;
    }

    // Overrides: FnAnimSuper
    ~FnEventBlender() override {}

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

    float GetTimeOffset(int i) const {}

    FnAnim *GetAnim(int i) {}

    void Set(FnAnim *anim0, float timeOffset0, FnAnim *anim1, float timeOffset1, float startTransTime, float duration, enum TriggerType tt);

    // Overrides: FnAnim
    void Eval(float previousTime, float currentTime, float *data) override;

  private:
    FnAnim *mAnim[2];         // offset 0xC, size 0x8
    float mTimeOffset[2];     // offset 0x14, size 0x8
    float mStartTransTime;    // offset 0x1C, size 0x4
    float mEndTransTime;      // offset 0x20, size 0x4
    float mDuration;          // offset 0x24, size 0x4
    TriggerType mTriggerType; // offset 0x28, size 0x4
};
}; // namespace EAGL4Anim

#endif
