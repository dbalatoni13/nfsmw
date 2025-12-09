#ifndef EVENTS_ELOADINGSCREENOFF_H
#define EVENTS_ELOADINGSCREENOFF_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class ELoadingScreenOff : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    ELoadingScreenOff();

    ~ELoadingScreenOff() override;

    const char *GetEventName() override {
        return "ELoadingScreenOff";
    }
};

void ELoadingScreenOff_MakeEvent_Callback(const void *staticData);

#endif
