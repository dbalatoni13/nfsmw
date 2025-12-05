#ifndef EVENTS_ENISSTOPEFFECTS_H
#define EVENTS_ENISSTOPEFFECTS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class ENISStopEffects : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {
    };

    // enum { kEventID = 0 };

    ENISStopEffects();

    override virtual ~ENISStopEffects();

    override virtual const char *GetEventName() {
        return "ENISStopEffects";
    }
};

void ENISStopEffects_MakeEvent_Callback(const void *staticData);

#endif
