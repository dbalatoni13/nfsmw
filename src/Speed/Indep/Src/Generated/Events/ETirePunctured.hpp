#ifndef EVENTS_ETIREPUNCTURED_H
#define EVENTS_ETIREPUNCTURED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class ETirePunctured : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        HSIMABLE fhSimable; // offset: 0x4, size 0x4
        unsigned int fIndex; // offset: 0x8, size 0x4
    };

    // enum { kEventID = 0 };

    ETirePunctured(HSIMABLE phSimable, unsigned int pIndex);

    override virtual ~ETirePunctured();

    override virtual const char *GetEventName() {
        return "ETirePunctured";
    }

  private:
    HSIMABLE fhSimable; // offset: 0x8, size 0x4
    unsigned int fIndex; // offset: 0xc, size 0x4
};

void ETirePunctured_MakeEvent_Callback(const void *staticData);

#endif
