#ifndef EVENTS_EDDAYSPEECH_H
#define EVENTS_EDDAYSPEECH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class EDDaySpeech : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {
    };

    // enum { kEventID = 0 };

    EDDaySpeech();

    override virtual ~EDDaySpeech();

    override virtual const char *GetEventName() {
        return "EDDaySpeech";
    }
};

void EDDaySpeech_MakeEvent_Callback(const void *staticData);

#endif
