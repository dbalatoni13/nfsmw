#ifndef EVENTS_ENISBRAKELOCK_H
#define EVENTS_ENISBRAKELOCK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x14
class ENISBrakelock : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        unsigned int fFront; // offset: 0x4, size 0x4
        unsigned int fRear;  // offset: 0x8, size 0x4
    };

    // enum { kEventID = 0 };

    ENISBrakelock(unsigned int pFront, unsigned int pRear, unsigned int phSimable);

    ~ENISBrakelock() override;

    const char *GetEventName() override {
        return "ENISBrakelock";
    }

  private:
    unsigned int fFront; // offset: 0x8, size 0x4
    unsigned int fRear;  // offset: 0xc, size 0x4

    unsigned int fhSimable; // offset: 0x14, size 0x4
};

void ENISBrakelock_MakeEvent_Callback(const void *staticData);

#endif
