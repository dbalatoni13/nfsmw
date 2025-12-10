#ifndef EVENTS_EENABLETRIGGER_H
#define EVENTS_EENABLETRIGGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EEnableTrigger : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        Trigger *fTrigger; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EEnableTrigger(Trigger *pTrigger);

    ~EEnableTrigger() override;

    const char *GetEventName() override;

  private:
    Trigger *fTrigger; // offset: 0x8, size 0x4
};

void EEnableTrigger_MakeEvent_Callback(const void *staticData);

#endif
