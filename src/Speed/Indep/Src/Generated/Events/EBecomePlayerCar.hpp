#ifndef EVENTS_EBECOMEPLAYERCAR_H
#define EVENTS_EBECOMEPLAYERCAR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class EBecomePlayerCar : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    EBecomePlayerCar();

    ~EBecomePlayerCar() override;

    const char *GetEventName() override {
        return "EBecomePlayerCar";
    }
};

void EBecomePlayerCar_MakeEvent_Callback(const void *staticData);

#endif
