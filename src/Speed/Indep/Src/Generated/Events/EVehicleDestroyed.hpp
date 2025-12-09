#ifndef EVENTS_EVEHICLEDESTROYED_H
#define EVENTS_EVEHICLEDESTROYED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EVehicleDestroyed : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        HSIMABLE fhSimable; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EVehicleDestroyed(HSIMABLE phSimable);

    ~EVehicleDestroyed() override;

    const char *GetEventName() override {
        return "EVehicleDestroyed";
    }

  private:
    HSIMABLE fhSimable; // offset: 0x8, size 0x4
};

void EVehicleDestroyed_MakeEvent_Callback(const void *staticData);

#endif
