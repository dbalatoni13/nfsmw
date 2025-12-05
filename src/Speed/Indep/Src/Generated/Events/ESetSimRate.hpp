#ifndef EVENTS_ESETSIMRATE_H
#define EVENTS_ESETSIMRATE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class ESetSimRate : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        float fSpeed; // offset: 0x4, size 0x4
        float fTransitionStepSize; // offset: 0x8, size 0x4
    };

    // enum { kEventID = 0 };

    ESetSimRate(float pSpeed, float pTransitionStepSize);

    override virtual ~ESetSimRate();

    override virtual const char *GetEventName() {
        return "ESetSimRate";
    }

  private:
    float fSpeed; // offset: 0x8, size 0x4
    float fTransitionStepSize; // offset: 0xc, size 0x4
};

void ESetSimRate_MakeEvent_Callback(const void *staticData);

#endif
