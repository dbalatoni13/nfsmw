#ifndef EVENTS_ECAMERASHAKE_H
#define EVENTS_ECAMERASHAKE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class ECameraShake : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {
    };

    // enum { kEventID = 0 };

    ECameraShake();

    override virtual ~ECameraShake();

    override virtual const char *GetEventName() {
        return "ECameraShake";
    }
};

void ECameraShake_MakeEvent_Callback(const void *staticData);

#endif
