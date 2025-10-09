#ifndef GENERATED_EVENTS_ETIREPUNCTURED_H
#define GENERATED_EVENTS_ETIREPUNCTURED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class ETirePunctured : public Event {
  public:
    // total size: 0xC
    struct StaticData : public Event::StaticData {
        HSIMABLE fhSimable;  // offset 0x4, size 0x4
        unsigned int fIndex; // offset 0x8, size 0x4
    };

    ETirePunctured(HSIMABLE phSimable, unsigned int pIndex);

    HSIMABLE fhSimable;  // offset 0x8, size 0x4
    unsigned int fIndex; // offset 0xC, size 0x4
};

#endif
