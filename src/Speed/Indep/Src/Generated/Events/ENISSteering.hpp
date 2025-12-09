#ifndef EVENTS_ENISSTEERING_H
#define EVENTS_ENISSTEERING_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x14
class ENISSteering : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        float fDegrees;     // offset: 0x4, size 0x4
        float fBlendWeight; // offset: 0x8, size 0x4
    };

    // enum { kEventID = 0 };

    ENISSteering(float pDegrees, float pBlendWeight, unsigned int phSimable);

    ~ENISSteering() override;

    const char *GetEventName() override {
        return "ENISSteering";
    }

  private:
    float fDegrees;     // offset: 0x8, size 0x4
    float fBlendWeight; // offset: 0xc, size 0x4

    unsigned int fhSimable; // offset: 0x14, size 0x4
};

void ENISSteering_MakeEvent_Callback(const void *staticData);

#endif
