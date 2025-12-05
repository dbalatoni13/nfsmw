#ifndef EVENTS_EKNOCKOUTRACER_H
#define EVENTS_EKNOCKOUTRACER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EKnockoutRacer : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        GRacerInfo * fRacer; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EKnockoutRacer(GRacerInfo * pRacer);

    override virtual ~EKnockoutRacer();

    override virtual const char *GetEventName() {
        return "EKnockoutRacer";
    }

  private:
    GRacerInfo * fRacer; // offset: 0x8, size 0x4
};

void EKnockoutRacer_MakeEvent_Callback(const void *staticData);

#endif
