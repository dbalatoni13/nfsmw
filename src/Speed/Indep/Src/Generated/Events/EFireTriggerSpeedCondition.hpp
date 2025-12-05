#ifndef EVENTS_EFIRETRIGGERSPEEDCONDITION_H
#define EVENTS_EFIRETRIGGERSPEEDCONDITION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x18
class EFireTriggerSpeedCondition : public Event {
  public:
    // total size: 0x10
    struct StaticData : public Event::StaticData {
        Trigger * fTrigger; // offset: 0x4, size 0x4
        float fSpeed; // offset: 0x8, size 0x4
        int fTriggerIfSpeedBelow; // offset: 0xc, size 0x4
    };

    // enum { kEventID = 0 };

    EFireTriggerSpeedCondition(Trigger * pTrigger, float pSpeed, int pTriggerIfSpeedBelow, unsigned int phSimable);

    override virtual ~EFireTriggerSpeedCondition();

    override virtual const char *GetEventName() {
        return "EFireTriggerSpeedCondition";
    }

  private:
    Trigger * fTrigger; // offset: 0x8, size 0x4
    float fSpeed; // offset: 0xc, size 0x4
    int fTriggerIfSpeedBelow; // offset: 0x10, size 0x4

    unsigned int fhSimable; // offset: 0x18, size 0x4
};

void EFireTriggerSpeedCondition_MakeEvent_Callback(const void *staticData);

#endif
