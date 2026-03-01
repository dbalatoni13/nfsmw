#ifndef EAGL4ANIM_RAWEVENTCHANNEL_H
#define EAGL4ANIM_RAWEVENTCHANNEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimMemoryMap.h"
#include "EventHandler.h"

namespace EAGL4Anim {

// total size: 0x8
class RawEventChannel : public AnimMemoryMap {
  public:
    int GetNumEvents() const {}

    void SetNumEvents(int n) {}

    struct Event *GetEvents() {}

    int GetSize() const {}

    static int ComputeSize(int numEvents) {}

    void Eval(float previousTime, float currentTime, int &currentIdx, float &cacheCurrentTime, EventHandler **eventHandlers, void *extraData);

  private:
    int mNumEvents; // offset 0x4, size 0x4
};

}; // namespace EAGL4Anim

#endif
