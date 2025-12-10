#ifndef EVENTS_EPLAYERSHIFT_H
#define EVENTS_EPLAYERSHIFT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/Event.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"

// total size: 0x1c
class EPlayerShift : public Event {
  public:
    // total size: 0x18
    struct StaticData : public Event::StaticData {
        HSIMABLE fhSimable;  // offset: 0x4, size 0x4
        ShiftStatus fStatus; // offset: 0x8, size 0x4
        bool fAutomatic;     // offset: 0xc, size 0x1
        GearID fFrom;        // offset: 0x10, size 0x4
        GearID fTo;          // offset: 0x14, size 0x4
    };

    // enum { kEventID = 0 };

    EPlayerShift(HSIMABLE phSimable, ShiftStatus pStatus, bool pAutomatic, GearID pFrom, GearID pTo);

    ~EPlayerShift() override;

    const char *GetEventName() override;

  private:
    HSIMABLE fhSimable;  // offset: 0x8, size 0x4
    ShiftStatus fStatus; // offset: 0xc, size 0x4
    bool fAutomatic;     // offset: 0x10, size 0x1
    GearID fFrom;        // offset: 0x14, size 0x4
    GearID fTo;          // offset: 0x18, size 0x4
};

void EPlayerShift_MakeEvent_Callback(const void *staticData);

#endif
