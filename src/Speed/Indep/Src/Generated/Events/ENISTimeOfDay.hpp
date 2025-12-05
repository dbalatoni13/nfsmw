#ifndef EVENTS_ENISTIMEOFDAY_H
#define EVENTS_ENISTIMEOFDAY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x18
class ENISTimeOfDay : public Event {
  public:
    // total size: 0x14
    struct StaticData : public Event::StaticData {
        float fTimeOfDay; // offset: 0x4, size 0x4
        float fUpdateRate; // offset: 0x8, size 0x4
        float fSunAzimuth; // offset: 0xc, size 0x4
        float fTimeOfDayOvercast; // offset: 0x10, size 0x4
    };

    // enum { kEventID = 0 };

    ENISTimeOfDay(float pTimeOfDay, float pUpdateRate, float pSunAzimuth, float pTimeOfDayOvercast);

    override virtual ~ENISTimeOfDay();

    override virtual const char *GetEventName() {
        return "ENISTimeOfDay";
    }

  private:
    float fTimeOfDay; // offset: 0x8, size 0x4
    float fUpdateRate; // offset: 0xc, size 0x4
    float fSunAzimuth; // offset: 0x10, size 0x4
    float fTimeOfDayOvercast; // offset: 0x14, size 0x4
};

void ENISTimeOfDay_MakeEvent_Callback(const void *staticData);

#endif
