#ifndef EVENTS_ERELOADHUD_H
#define EVENTS_ERELOADHUD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class EReloadHud : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    EReloadHud();

    ~EReloadHud() override;

    const char *GetEventName() override {
        return "EReloadHud";
    }
};

void EReloadHud_MakeEvent_Callback(const void *staticData);

#endif
