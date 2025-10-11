#ifndef GENERATED_EVENTS_EPLAYERTRIGGEREDNOS_H
#define GENERATED_EVENTS_EPLAYERTRIGGEREDNOS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class EPlayerTriggeredNOS : public Event {
  public:
    // total size: 0xC
    struct StaticData : public Event::StaticData {
        HSIMABLE fhSimable; // offset 0x4, size 0x4
    };

    EPlayerTriggeredNOS(HSIMABLE phSimable);

  private:
    HSIMABLE fhSimable; // offset 0x8, size 0x4
};

#endif
