#ifndef EVENTS_ETIPS_H
#define EVENTS_ETIPS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class ETips : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        int fPlayerIndex; // offset: 0x4, size 0x4
        int fTipNum;      // offset: 0x8, size 0x4
    };

    // enum { kEventID = 0 };

    ETips(int pPlayerIndex, int pTipNum);

    ~ETips() override;

    const char *GetEventName() override {
        return "ETips";
    }

  private:
    int fPlayerIndex; // offset: 0x8, size 0x4
    int fTipNum;      // offset: 0xc, size 0x4
};

void ETips_MakeEvent_Callback(const void *staticData);

#endif
