#ifndef EVENTS_ENISCARSHAKE_H
#define EVENTS_ENISCARSHAKE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x1c
class ENISCarShake : public Event {
  public:
    // total size: 0x14
    struct StaticData : public Event::StaticData {
        float fShake; // offset: 0x4, size 0x4
        float fRate; // offset: 0x8, size 0x4
        float fRamp; // offset: 0xc, size 0x4
        float fTime; // offset: 0x10, size 0x4
    };

    // enum { kEventID = 0 };

    ENISCarShake(float pShake, float pRate, float pRamp, float pTime, unsigned int phSimable);

    override virtual ~ENISCarShake();

    override virtual const char *GetEventName() {
        return "ENISCarShake";
    }

  private:
    float fShake; // offset: 0x8, size 0x4
    float fRate; // offset: 0xc, size 0x4
    float fRamp; // offset: 0x10, size 0x4
    float fTime; // offset: 0x14, size 0x4

    unsigned int fhSimable; // offset: 0x1c, size 0x4
};

void ENISCarShake_MakeEvent_Callback(const void *staticData);

#endif
