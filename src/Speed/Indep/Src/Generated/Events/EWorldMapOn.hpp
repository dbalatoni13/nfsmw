#ifndef EVENTS_EWORLDMAPON_H
#define EVENTS_EWORLDMAPON_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class EWorldMapOn : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    EWorldMapOn();

    ~EWorldMapOn() override;

    const char *GetEventName() override {
        return "EWorldMapOn";
    }
};

void EWorldMapOn_MakeEvent_Callback(const void *staticData);

#endif
