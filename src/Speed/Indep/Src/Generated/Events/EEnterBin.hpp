#ifndef EVENTS_EENTERBIN_H
#define EVENTS_EENTERBIN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EEnterBin : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        int fBinIndex; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EEnterBin(int pBinIndex);

    ~EEnterBin() override;

    const char *GetEventName() override {
        return "EEnterBin";
    }

  private:
    int fBinIndex; // offset: 0x8, size 0x4
};

void EEnterBin_MakeEvent_Callback(const void *staticData);

#endif
