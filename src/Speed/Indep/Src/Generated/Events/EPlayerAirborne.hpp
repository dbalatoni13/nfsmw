#ifndef EVENTS_EPLAYERAIRBORNE_H
#define EVENTS_EPLAYERAIRBORNE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EPlayerAirborne : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        HSIMABLE fhSimable; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EPlayerAirborne(HSIMABLE phSimable);

    ~EPlayerAirborne() override;

    const char *GetEventName() override;

  private:
    HSIMABLE fhSimable; // offset: 0x8, size 0x4
};

void EPlayerAirborne_MakeEvent_Callback(const void *staticData);

#endif
