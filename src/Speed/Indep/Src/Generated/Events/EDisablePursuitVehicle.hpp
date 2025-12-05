#ifndef EVENTS_EDISABLEPURSUITVEHICLE_H
#define EVENTS_EDISABLEPURSUITVEHICLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EDisablePursuitVehicle : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {
    };

    // enum { kEventID = 0 };

    EDisablePursuitVehicle(unsigned int phSimable);

    override virtual ~EDisablePursuitVehicle();

    override virtual const char *GetEventName() {
        return "EDisablePursuitVehicle";
    }

  private:
    unsigned int fhSimable; // offset: 0xc, size 0x4
};

void EDisablePursuitVehicle_MakeEvent_Callback(const void *staticData);

#endif
