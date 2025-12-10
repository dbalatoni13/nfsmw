#ifndef EVENTS_ENISLIGHTS_H
#define EVENTS_ENISLIGHTS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x14
class ENISLights : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        unsigned int fCopLights;  // offset: 0x4, size 0x4
        unsigned int fHeadLights; // offset: 0x8, size 0x4
    };

    // enum { kEventID = 0 };

    ENISLights(unsigned int pCopLights, unsigned int pHeadLights, unsigned int phSimable);

    ~ENISLights() override;

    const char *GetEventName() override;

  private:
    unsigned int fCopLights;  // offset: 0x8, size 0x4
    unsigned int fHeadLights; // offset: 0xc, size 0x4

    unsigned int fhSimable; // offset: 0x14, size 0x4
};

void ENISLights_MakeEvent_Callback(const void *staticData);

#endif
