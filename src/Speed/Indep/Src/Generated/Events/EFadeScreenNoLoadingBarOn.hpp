#ifndef EVENTS_EFADESCREENNOLOADINGBARON_H
#define EVENTS_EFADESCREENNOLOADINGBARON_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class EFadeScreenNoLoadingBarOn : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    EFadeScreenNoLoadingBarOn();

    ~EFadeScreenNoLoadingBarOn() override;

    const char *GetEventName() override;
};

void EFadeScreenNoLoadingBarOn_MakeEvent_Callback(const void *staticData);

#endif
