#ifndef GENERATED_EVENTS_EJOINTDETACHED_H
#define GENERATED_EVENTS_EJOINTDETACHED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xC
class EJointDetached : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        HSIMABLE fhObject; // offset 0x4, size 0x4
    };
    EJointDetached(HSIMABLE phObject);

  private:
    HSIMABLE fhObject; // offset 0x8, size 0x4
};

#endif
