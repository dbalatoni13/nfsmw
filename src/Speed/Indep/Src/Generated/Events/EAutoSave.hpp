#ifndef EVENTS_EAUTOSAVE_H
#define EVENTS_EAUTOSAVE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class EAutoSave : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {
    };

    // enum { kEventID = 0 };

    EAutoSave();

    override virtual ~EAutoSave();

    override virtual const char *GetEventName() {
        return "EAutoSave";
    }
};

void EAutoSave_MakeEvent_Callback(const void *staticData);

#endif
