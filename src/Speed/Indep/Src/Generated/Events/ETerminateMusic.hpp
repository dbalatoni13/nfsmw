#ifndef EVENTS_ETERMINATEMUSIC_H
#define EVENTS_ETERMINATEMUSIC_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class ETerminateMusic : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {
    };

    // enum { kEventID = 0 };

    ETerminateMusic();

    override virtual ~ETerminateMusic();

    override virtual const char *GetEventName() {
        return "ETerminateMusic";
    }
};

void ETerminateMusic_MakeEvent_Callback(const void *staticData);

#endif
