#ifndef EVENTS_ENISDETACH_H
#define EVENTS_ENISDETACH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class ENISDetach : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        float fSpeed; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    ENISDetach(float pSpeed, unsigned int phSimable);

    ~ENISDetach() override;

    const char *GetEventName() override;

  private:
    float fSpeed; // offset: 0x8, size 0x4

    unsigned int fhSimable; // offset: 0x10, size 0x4
};

void ENISDetach_MakeEvent_Callback(const void *staticData);

#endif
