#ifndef EVENTS_EPERFECTLAUNCH_H
#define EVENTS_EPERFECTLAUNCH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class EPerfectLaunch : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        HSIMABLE fhSimable; // offset: 0x4, size 0x4
        float fBonus;       // offset: 0x8, size 0x4
    };

    // enum { kEventID = 0 };

    EPerfectLaunch(HSIMABLE phSimable, float pBonus);

    ~EPerfectLaunch() override;

    const char *GetEventName() override;

  private:
    HSIMABLE fhSimable; // offset: 0x8, size 0x4
    float fBonus;       // offset: 0xc, size 0x4
};

void EPerfectLaunch_MakeEvent_Callback(const void *staticData);

#endif
