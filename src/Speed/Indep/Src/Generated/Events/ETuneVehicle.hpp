#ifndef EVENTS_ETUNEVEHICLE_H
#define EVENTS_ETUNEVEHICLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Physics/PhysicsTunings.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class ETuneVehicle : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        unsigned int fPlayer;             // offset: 0x4, size 0x4
        const Physics::Tunings *fTunings; // offset: 0x8, size 0x4
    };

    // enum { kEventID = 0 };

    ETuneVehicle(unsigned int pPlayer, const Physics::Tunings *pTunings);

    ~ETuneVehicle() override;

    const char *GetEventName() override;

  private:
    unsigned int fPlayer;             // offset: 0x8, size 0x4
    const Physics::Tunings *fTunings; // offset: 0xc, size 0x4
};

void ETuneVehicle_MakeEvent_Callback(const void *staticData);

#endif
