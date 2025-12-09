#ifndef EVENTS_ENISCARROLL_H
#define EVENTS_ENISCARROLL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x14
class ENISCarRoll : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        float fRoll; // offset: 0x4, size 0x4
        float fTime; // offset: 0x8, size 0x4
    };

    // enum { kEventID = 0 };

    ENISCarRoll(float pRoll, float pTime, unsigned int phSimable);

    ~ENISCarRoll() override;

    const char *GetEventName() override {
        return "ENISCarRoll";
    }

  private:
    float fRoll; // offset: 0x8, size 0x4
    float fTime; // offset: 0xc, size 0x4

    unsigned int fhSimable; // offset: 0x14, size 0x4
};

void ENISCarRoll_MakeEvent_Callback(const void *staticData);

#endif
