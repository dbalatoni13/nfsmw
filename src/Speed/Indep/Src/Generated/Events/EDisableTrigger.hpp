#ifndef EVENTS_EDISABLETRIGGER_H
#define EVENTS_EDISABLETRIGGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EDisableTrigger : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        Trigger * fTrigger; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EDisableTrigger(Trigger * pTrigger);

    override virtual ~EDisableTrigger();

    override virtual const char *GetEventName() {
        return "EDisableTrigger";
    }

  private:
    Trigger * fTrigger; // offset: 0x8, size 0x4
};

void EDisableTrigger_MakeEvent_Callback(const void *staticData);

#endif
