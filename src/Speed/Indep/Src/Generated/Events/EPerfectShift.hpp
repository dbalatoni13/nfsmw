#ifndef EVENTS_EPERFECTSHIFT_H
#define EVENTS_EPERFECTSHIFT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class EPerfectShift : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        HSIMABLE fhSimable; // offset: 0x4, size 0x4
        float fBonus;       // offset: 0x8, size 0x4
    };

    // enum { kEventID = 0 };

    EPerfectShift(HSIMABLE phSimable, float pBonus);

    ~EPerfectShift() override;

    const char *GetEventName() override;

  private:
    HSIMABLE fhSimable; // offset: 0x8, size 0x4
    float fBonus;       // offset: 0xc, size 0x4
};

void EPerfectShift_MakeEvent_Callback(const void *staticData);

#endif
