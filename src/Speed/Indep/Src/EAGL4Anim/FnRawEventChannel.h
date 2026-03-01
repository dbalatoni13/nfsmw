#ifndef EAGL4ANIM_FNRAWEVENTCHANNEL_H
#define EAGL4ANIM_FNRAWEVENTCHANNEL_H

#include "Speed/Indep/Src/EAGL4Anim/EventHandler.h"
#include "Speed/Indep/Src/EAGL4Anim/eagl4supportdef.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FnAnimMemoryMap.h"
#include "RawEventChannel.h"

namespace EAGL4Anim {

// total size: 0x18
class FnRawEventChannel : public FnAnimMemoryMap {
  public:
    FnRawEventChannel()
        : mCurrentIdx(0), //
          mCurrentTime(0.0f) {
        mType = AnimTypeId::ANIM_RAWEVENT;
    }

    // Overrides: FnAnimSuper
    ~FnRawEventChannel() override {}

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

    RawEventChannel *GetRawEventChannel() {
        return reinterpret_cast<RawEventChannel *>(mpAnim);
    }

    const RawEventChannel *GetRawEventChannel() const {
        return reinterpret_cast<RawEventChannel *>(mpAnim);
    }

    int GetNumEvents() const {}

    // Overrides: FnAnimMemoryMap
    void SetAnimMemoryMap(AnimMemoryMap *anim) override {
        mpAnim = anim;
        mCurrentIdx = 0;
        mCurrentTime = 0.0f;
    }

    // Overrides: FnAnim
    bool EvalEvent(float previousTime, float currentTime, EventHandler **eventHandlers, void *extraData) override {
        GetRawEventChannel()->Eval(previousTime, currentTime, mCurrentIdx, mCurrentTime, eventHandlers, extraData);
        return true;
    }

    // Overrides: FnAnim
    void Eval(float previousTime, float currentTime, float *eventHandlers) override {
        EvalEvent(previousTime, currentTime, reinterpret_cast<EventHandler **>(eventHandlers), nullptr);
    }

  private:
    int mCurrentIdx;    // offset 0x10, size 0x4
    float mCurrentTime; // offset 0x14, size 0x4
};

}; // namespace EAGL4Anim

#endif
