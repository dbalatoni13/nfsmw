#ifndef EVENTS_EMISSSHIFT_H
#define EVENTS_EMISSSHIFT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/Event.h"

class EMissShift : public Event {
  public:
    struct StaticData : public Event::StaticData {
        HSIMABLE fhSimable;
        float fBonus;
    };

    enum { kEventID = 1171244884 };

    EMissShift(HSIMABLE phSimable, float pBonus);

    virtual ~EMissShift();

    override virtual const char *GetEventName() {
        return "EMissShift";
    }

  private:
    HSIMABLE fhSimable;
    float fBonus;
};

void EMissShift_MakeEvent_Callback(const void *staticData);

#endif
