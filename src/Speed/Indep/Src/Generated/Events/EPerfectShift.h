#ifndef EVENTS_EPERFECTSHIFT_H
#define EVENTS_EPERFECTSHIFT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/Event.h"

class EPerfectShift : public Event {
  public:
    struct StaticData : public Event::StaticData {
        HSIMABLE fhSimable;
        float fBonus;
    };

    enum { kEventID = 1171244884 };

    EPerfectShift(HSIMABLE phSimable, float pBonus);

    virtual ~EPerfectShift();

    const char *GetEventName() {
        return "EPerfectShift";
    }

  private:
    HSIMABLE fhSimable;
    float fBonus;
};

void EPerfectShift_MakeEvent_Callback(const void *staticData);

#endif
