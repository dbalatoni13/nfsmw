#ifndef EVENTS_EEXITENGAGABLETRIGGER_H
#define EVENTS_EEXITENGAGABLETRIGGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EExitEngagableTrigger : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        GRuntimeInstance * fRaceActivity; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EExitEngagableTrigger(GRuntimeInstance * pRaceActivity);

    override virtual ~EExitEngagableTrigger();

    override virtual const char *GetEventName() {
        return "EExitEngagableTrigger";
    }

  private:
    GRuntimeInstance * fRaceActivity; // offset: 0x8, size 0x4
};

void EExitEngagableTrigger_MakeEvent_Callback(const void *staticData);

#endif
