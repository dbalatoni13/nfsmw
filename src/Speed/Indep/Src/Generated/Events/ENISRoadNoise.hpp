#ifndef EVENTS_ENISROADNOISE_H
#define EVENTS_ENISROADNOISE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class ENISRoadNoise : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        int fDisableRoadNoise; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    ENISRoadNoise(int pDisableRoadNoise);

    override virtual ~ENISRoadNoise();

    override virtual const char *GetEventName() {
        return "ENISRoadNoise";
    }

  private:
    int fDisableRoadNoise; // offset: 0x8, size 0x4
};

void ENISRoadNoise_MakeEvent_Callback(const void *staticData);

#endif
