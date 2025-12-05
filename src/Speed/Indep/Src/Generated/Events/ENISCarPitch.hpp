#ifndef EVENTS_ENISCARPITCH_H
#define EVENTS_ENISCARPITCH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x14
class ENISCarPitch : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        float fPitch; // offset: 0x4, size 0x4
        float fTime; // offset: 0x8, size 0x4
    };

    // enum { kEventID = 0 };

    ENISCarPitch(float pPitch, float pTime, unsigned int phSimable);

    override virtual ~ENISCarPitch();

    override virtual const char *GetEventName() {
        return "ENISCarPitch";
    }

  private:
    float fPitch; // offset: 0x8, size 0x4
    float fTime; // offset: 0xc, size 0x4

    unsigned int fhSimable; // offset: 0x14, size 0x4
};

void ENISCarPitch_MakeEvent_Callback(const void *staticData);

#endif
