#ifndef EVENTS_ESETPLAYERCOLLISIONCACHE_H
#define EVENTS_ESETPLAYERCOLLISIONCACHE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class ESetPlayerCollisionCache : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    ESetPlayerCollisionCache(int pTriggerStimulus);

    ~ESetPlayerCollisionCache() override;

    const char *GetEventName() override {
        return "ESetPlayerCollisionCache";
    }

  private:
    int fTriggerStimulus; // offset: 0xc, size 0x4
};

void ESetPlayerCollisionCache_MakeEvent_Callback(const void *staticData);

#endif
