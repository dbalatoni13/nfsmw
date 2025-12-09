#ifndef EVENTS_ESHOWRESULTS_H
#define EVENTS_ESHOWRESULTS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class EShowResults : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        FERESULTTYPE fResultType; // offset: 0x4, size 0x4
        bool fCalledWhileDriving; // offset: 0x8, size 0x1
    };

    // enum { kEventID = 0 };

    EShowResults(FERESULTTYPE pResultType, bool pCalledWhileDriving);

    ~EShowResults() override;

    const char *GetEventName() override {
        return "EShowResults";
    }

  private:
    FERESULTTYPE fResultType; // offset: 0x8, size 0x4
    bool fCalledWhileDriving; // offset: 0xc, size 0x1
};

void EShowResults_MakeEvent_Callback(const void *staticData);

#endif
