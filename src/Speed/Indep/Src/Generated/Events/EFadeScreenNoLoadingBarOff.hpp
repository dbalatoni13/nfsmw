#ifndef EVENTS_EFADESCREENNOLOADINGBAROFF_H
#define EVENTS_EFADESCREENNOLOADINGBAROFF_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class EFadeScreenNoLoadingBarOff : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {
    };

    // enum { kEventID = 0 };

    EFadeScreenNoLoadingBarOff();

    override virtual ~EFadeScreenNoLoadingBarOff();

    override virtual const char *GetEventName() {
        return "EFadeScreenNoLoadingBarOff";
    }
};

void EFadeScreenNoLoadingBarOff_MakeEvent_Callback(const void *staticData);

#endif
