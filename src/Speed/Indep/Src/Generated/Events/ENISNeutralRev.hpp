#ifndef EVENTS_ENISNEUTRALREV_H
#define EVENTS_ENISNEUTRALREV_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x18
class ENISNeutralRev : public Event {
  public:
    // total size: 0x10
    struct StaticData : public Event::StaticData {
        unsigned int fOn; // offset: 0x4, size 0x4
        float fThrottle;  // offset: 0x8, size 0x4
        float fSpeed;     // offset: 0xc, size 0x4
    };

    // enum { kEventID = 0 };

    ENISNeutralRev(unsigned int pOn, float pThrottle, float pSpeed, unsigned int phSimable);

    ~ENISNeutralRev() override;

    const char *GetEventName() override;

  private:
    unsigned int fOn; // offset: 0x8, size 0x4
    float fThrottle;  // offset: 0xc, size 0x4
    float fSpeed;     // offset: 0x10, size 0x4

    unsigned int fhSimable; // offset: 0x18, size 0x4
};

void ENISNeutralRev_MakeEvent_Callback(const void *staticData);

#endif
