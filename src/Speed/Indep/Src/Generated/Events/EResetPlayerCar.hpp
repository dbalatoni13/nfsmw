#ifndef EVENTS_ERESETPLAYERCAR_H
#define EVENTS_ERESETPLAYERCAR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class EResetPlayerCar : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    EResetPlayerCar();

    ~EResetPlayerCar() override;

    const char *GetEventName() override;
};

void EResetPlayerCar_MakeEvent_Callback(const void *staticData);

#endif
