#ifndef EAGL4ANIM_FNCSISEVENTCHANNEL_H
#define EAGL4ANIM_FNCSISEVENTCHANNEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "CsisEventChannel.h"
#include "FnAnimMemoryMap.h"

namespace EAGL4Anim {

// total size: 0x18
class FnCsisEventChannel : public FnAnimMemoryMap {
  public:
    FnCsisEventChannel() {}

    // Overrides: FnAnimSuper
    ~FnCsisEventChannel() override {}

    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    // void operator delete(void *ptr, size_t size) {}

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    // void *operator new(size_t, void *ptr) {}

    CsisEventChannel *GetCsisEventChannel() {}

    const CsisEventChannel *GetCsisEventChannel() const {}

    int GetNumEvents() const {}

    // Overrides: FnAnimMemoryMap
    void SetAnimMemoryMap(AnimMemoryMap *anim) override {}

    // Overrides: FnAnim
    bool EvalEvent(float previousTime, float currentTime, EventHandler **eventHandlers, void *extraData) override {}

    // Overrides: FnAnim
    void Eval(float previousTime, float currentTime, float *eventHandlers) override {}

  private:
    int mCurrentIdx;    // offset 0x10, size 0x4
    float mCurrentTime; // offset 0x14, size 0x4
};

}; // namespace EAGL4Anim

#endif
