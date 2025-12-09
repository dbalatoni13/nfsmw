#ifndef EVENTS_EFADESCREENON_H
#define EVENTS_EFADESCREENON_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EFadeScreenOn : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        bool fLoadingBarOnly; // offset: 0x4, size 0x1
    };

    // enum { kEventID = 0 };

    EFadeScreenOn(bool pLoadingBarOnly);

    ~EFadeScreenOn() override;

    const char *GetEventName() override {
        return "EFadeScreenOn";
    }

  private:
    bool fLoadingBarOnly; // offset: 0x8, size 0x1
};

void EFadeScreenOn_MakeEvent_Callback(const void *staticData);

#endif
