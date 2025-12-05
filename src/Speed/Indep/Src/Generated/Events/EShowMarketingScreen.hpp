#ifndef EVENTS_ESHOWMARKETINGSCREEN_H
#define EVENTS_ESHOWMARKETINGSCREEN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EShowMarketingScreen : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        int fShowFor; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EShowMarketingScreen(int pShowFor);

    override virtual ~EShowMarketingScreen();

    override virtual const char *GetEventName() {
        return "EShowMarketingScreen";
    }

  private:
    int fShowFor; // offset: 0x8, size 0x4
};

void EShowMarketingScreen_MakeEvent_Callback(const void *staticData);

#endif
