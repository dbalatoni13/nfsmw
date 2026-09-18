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
    int GetNumEvents() const {
        return mNumEvents;
    }

    void SetNumEvents(int n) {
        mNumEvents = n;
    }

    struct Event *GetEvents() {
        return reinterpret_cast<Event *>(reinterpret_cast<unsigned char *>(this) + 8);
    }

    int GetSize() const {
        return ComputeSize(mNumEvents);
    }

    static int ComputeSize(int numEvents) {
        return 0x8 + numEvents * 0x10;
    }

    void Eval(float previousTime, float currentTime, int &currentIdx, float &cacheCurrentTime, EventHandler **eventHandlers, void *extraData);

  private:
    int mNumEvents; // offset 0x4, size 0x4
};

}; // namespace EAGL4Anim

#endif
