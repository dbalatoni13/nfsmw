#ifndef EVENTS_EBECOMEPURSUITCAR_H
#define EVENTS_EBECOMEPURSUITCAR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EBecomePursuitCar : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {
    };

    // enum { kEventID = 0 };

    EBecomePursuitCar(unsigned int phSimable);

    override virtual ~EBecomePursuitCar();

    override virtual const char *GetEventName() {
        return "EBecomePursuitCar";
    }

  private:
    unsigned int fhSimable; // offset: 0xc, size 0x4
};

void EBecomePursuitCar_MakeEvent_Callback(const void *staticData);

#endif
