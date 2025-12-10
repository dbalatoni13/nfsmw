#ifndef EVENTS_ERACESHEETON_H
#define EVENTS_ERACESHEETON_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class ERaceSheetOn : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        int fRivalFlow; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    ERaceSheetOn(int pRivalFlow);

    ~ERaceSheetOn() override;

    const char *GetEventName() override;

  private:
    int fRivalFlow; // offset: 0x8, size 0x4
};

void ERaceSheetOn_MakeEvent_Callback(const void *staticData);

#endif
