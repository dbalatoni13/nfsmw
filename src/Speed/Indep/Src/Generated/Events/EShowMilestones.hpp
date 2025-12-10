#ifndef EVENTS_ESHOWMILESTONES_H
#define EVENTS_ESHOWMILESTONES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EShowMilestones : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        int fEnterSafehouseOnExit; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EShowMilestones(int pEnterSafehouseOnExit);

    ~EShowMilestones() override;

    const char *GetEventName() override;

  private:
    int fEnterSafehouseOnExit; // offset: 0x8, size 0x4
};

void EShowMilestones_MakeEvent_Callback(const void *staticData);

#endif
