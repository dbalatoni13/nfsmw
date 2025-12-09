#ifndef EVENTS_EGPSLOST_H
#define EVENTS_EGPSLOST_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class EGPSLost : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    EGPSLost();

    ~EGPSLost() override;

    const char *GetEventName() override {
        return "EGPSLost";
    }
};

void EGPSLost_MakeEvent_Callback(const void *staticData);

#endif
