#ifndef EVENTS_ELOADINGSCREENON_H
#define EVENTS_ELOADINGSCREENON_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingScreen.hpp"

// total size: 0xc
class ELoadingScreenOn : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        LoadingScreen::LoadingScreenTypes fLoadingType; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    ELoadingScreenOn(LoadingScreen::LoadingScreenTypes pLoadingType);

    ~ELoadingScreenOn() override;

    const char *GetEventName() override;

  private:
    LoadingScreen::LoadingScreenTypes fLoadingType; // offset: 0x8, size 0x4
};

void ELoadingScreenOn_MakeEvent_Callback(const void *staticData);

#endif
