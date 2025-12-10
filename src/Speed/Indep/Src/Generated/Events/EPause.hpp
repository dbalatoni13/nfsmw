#ifndef EVENTS_EPAUSE_H
#define EVENTS_EPAUSE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x14
class EPause : public Event {
  public:
    // total size: 0x10
    struct StaticData : public Event::StaticData {
        int fPlayerIndex;        // offset: 0x4, size 0x4
        int fCalledFromPostRace; // offset: 0x8, size 0x4
        int fShowFlatPause;      // offset: 0xc, size 0x4
    };

    // enum { kEventID = 0 };

    EPause(int pPlayerIndex, int pCalledFromPostRace, int pShowFlatPause);

    ~EPause() override;

    const char *GetEventName() override;

  private:
    int fPlayerIndex;        // offset: 0x8, size 0x4
    int fCalledFromPostRace; // offset: 0xc, size 0x4
    int fShowFlatPause;      // offset: 0x10, size 0x4
};

void EPause_MakeEvent_Callback(const void *staticData);

#endif
