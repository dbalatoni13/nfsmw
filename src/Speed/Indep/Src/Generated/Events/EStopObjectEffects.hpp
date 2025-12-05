#ifndef EVENTS_ESTOPOBJECTEFFECTS_H
#define EVENTS_ESTOPOBJECTEFFECTS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EStopObjectEffects : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {
    };

    // enum { kEventID = 0 };

    EStopObjectEffects(unsigned int phModel);

    override virtual ~EStopObjectEffects();

    override virtual const char *GetEventName() {
        return "EStopObjectEffects";
    }

  private:
    unsigned int fhModel; // offset: 0xc, size 0x4
};

void EStopObjectEffects_MakeEvent_Callback(const void *staticData);

#endif
