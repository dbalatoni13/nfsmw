#ifndef EVENTS_ERESTARTRACE_H
#define EVENTS_ERESTARTRACE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class ERestartRace : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    ERestartRace();

    ~ERestartRace() override;

    const char *GetEventName() override;
};

void ERestartRace_MakeEvent_Callback(const void *staticData);

#endif
