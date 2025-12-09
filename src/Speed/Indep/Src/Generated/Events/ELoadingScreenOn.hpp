#ifndef EVENTS_ELOADINGSCREENON_H
#define EVENTS_ELOADINGSCREENON_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class ELoadingScreenOn : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        LoadingScreenTypes fLoadingType; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    ELoadingScreenOn(LoadingScreenTypes pLoadingType);

    ~ELoadingScreenOn() override;

    const char *GetEventName() override {
        return "ELoadingScreenOn";
    }

  private:
    LoadingScreenTypes fLoadingType; // offset: 0x8, size 0x4
};

void ELoadingScreenOn_MakeEvent_Callback(const void *staticData);

#endif
