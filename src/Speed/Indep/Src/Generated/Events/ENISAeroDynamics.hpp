#ifndef EVENTS_ENISAERODYNAMICS_H
#define EVENTS_ENISAERODYNAMICS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class ENISAeroDynamics : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        float fAerodynamics; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    ENISAeroDynamics(float pAerodynamics);

    ~ENISAeroDynamics() override;

    const char *GetEventName() override;

  private:
    float fAerodynamics; // offset: 0x8, size 0x4
};

void ENISAeroDynamics_MakeEvent_Callback(const void *staticData);

#endif
