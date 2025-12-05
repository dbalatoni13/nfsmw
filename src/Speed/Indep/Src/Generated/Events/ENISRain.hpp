#ifndef EVENTS_ENISRAIN_H
#define EVENTS_ENISRAIN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class ENISRain : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        float fRainIntensity; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    ENISRain(float pRainIntensity);

    override virtual ~ENISRain();

    override virtual const char *GetEventName() {
        return "ENISRain";
    }

  private:
    float fRainIntensity; // offset: 0x8, size 0x4
};

void ENISRain_MakeEvent_Callback(const void *staticData);

#endif
