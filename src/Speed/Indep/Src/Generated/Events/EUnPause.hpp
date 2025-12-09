#ifndef EVENTS_EUNPAUSE_H
#define EVENTS_EUNPAUSE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class EUnPause : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    EUnPause();

    ~EUnPause() override;

    const char *GetEventName() override {
        return "EUnPause";
    }
};

void EUnPause_MakeEvent_Callback(const void *staticData);

#endif
