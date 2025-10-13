#ifndef EVENTS_EPLAYERSHIFT_H
#define EVENTS_EPLAYERSHIFT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/Event.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"

class EPlayerShift : public Event {
  public:
    struct StaticData : public Event::StaticData {
        HSIMABLE fhSimable;
        ShiftStatus fStatus;
        bool fAutomatic;
        GearID fFrom;
        GearID fTo;
    };

    enum { kEventID = 1171244884 };

    EPlayerShift(HSIMABLE phSimable, ShiftStatus pStatus, bool pAutomatic, GearID pFrom, GearID pTo);

    virtual ~EPlayerShift();

    const char *GetEventName() {
        return "EPlayerShift";
    }

  private:
    HSIMABLE fhSimable;
    ShiftStatus fStatus;
    bool fAutomatic;
    GearID fFrom;
    GearID fTo;
};

void EPlayerShift_MakeEvent_Callback(const void *staticData);

#endif
