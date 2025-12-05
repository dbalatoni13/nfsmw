#ifndef EVENTS_EENTERENGAGABLETRIGGER_H
#define EVENTS_EENTERENGAGABLETRIGGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EEnterEngagableTrigger : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        GRuntimeInstance * fRaceActivity; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EEnterEngagableTrigger(GRuntimeInstance * pRaceActivity);

    override virtual ~EEnterEngagableTrigger();

    override virtual const char *GetEventName() {
        return "EEnterEngagableTrigger";
    }

  private:
    GRuntimeInstance * fRaceActivity; // offset: 0x8, size 0x4
};

void EEnterEngagableTrigger_MakeEvent_Callback(const void *staticData);

#endif
