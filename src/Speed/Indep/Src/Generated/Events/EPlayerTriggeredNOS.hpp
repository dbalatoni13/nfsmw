#ifndef EVENTS_EPLAYERTRIGGEREDNOS_H
#define EVENTS_EPLAYERTRIGGEREDNOS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EPlayerTriggeredNOS : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        HSIMABLE fhSimable; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EPlayerTriggeredNOS(HSIMABLE phSimable);

    ~EPlayerTriggeredNOS() override;

    const char *GetEventName() override {
        return "EPlayerTriggeredNOS";
    }

  private:
    HSIMABLE fhSimable; // offset: 0x8, size 0x4
};

void EPlayerTriggeredNOS_MakeEvent_Callback(const void *staticData);

#endif
