#ifndef EVENTS_EGPSFINISHED_H
#define EVENTS_EGPSFINISHED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class EGPSFinished : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {
    };

    // enum { kEventID = 0 };

    EGPSFinished();

    override virtual ~EGPSFinished();

    override virtual const char *GetEventName() {
        return "EGPSFinished";
    }
};

void EGPSFinished_MakeEvent_Callback(const void *staticData);

#endif
