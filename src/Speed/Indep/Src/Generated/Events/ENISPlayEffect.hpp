#ifndef EVENTS_ENISPLAYEFFECT_H
#define EVENTS_ENISPLAYEFFECT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x24
class ENISPlayEffect : public Event {
  public:
    // total size: 0x20
    struct StaticData : public Event::StaticData {
        const char * fEffect; // offset: 0x4, size 0x4
        float fPositionX; // offset: 0x8, size 0x4
        float fPositionY; // offset: 0xc, size 0x4
        float fPositionZ; // offset: 0x10, size 0x4
        float fMagnitudeX; // offset: 0x14, size 0x4
        float fMagnitudeY; // offset: 0x18, size 0x4
        float fMagnitudeZ; // offset: 0x1c, size 0x4
    };

    // enum { kEventID = 0 };

    ENISPlayEffect(const char * pEffect, float pPositionX, float pPositionY, float pPositionZ, float pMagnitudeX, float pMagnitudeY, float pMagnitudeZ);

    override virtual ~ENISPlayEffect();

    override virtual const char *GetEventName() {
        return "ENISPlayEffect";
    }

  private:
    const char * fEffect; // offset: 0x8, size 0x4
    float fPositionX; // offset: 0xc, size 0x4
    float fPositionY; // offset: 0x10, size 0x4
    float fPositionZ; // offset: 0x14, size 0x4
    float fMagnitudeX; // offset: 0x18, size 0x4
    float fMagnitudeY; // offset: 0x1c, size 0x4
    float fMagnitudeZ; // offset: 0x20, size 0x4
};

void ENISPlayEffect_MakeEvent_Callback(const void *staticData);

#endif
