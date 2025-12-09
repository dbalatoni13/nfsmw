#ifndef EVENTS_ESHOWRACECOUNTDOWN_H
#define EVENTS_ESHOWRACECOUNTDOWN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class EShowRaceCountdown : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    EShowRaceCountdown();

    ~EShowRaceCountdown() override;

    const char *GetEventName() override {
        return "EShowRaceCountdown";
    }
};

void EShowRaceCountdown_MakeEvent_Callback(const void *staticData);

#endif
