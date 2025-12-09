#ifndef EVENTS_ESNDGAMESTATE_H
#define EVENTS_ESNDGAMESTATE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class ESndGameState : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        int fState;      // offset: 0x4, size 0x4
        bool fTurningOn; // offset: 0x8, size 0x1
    };

    // enum { kEventID = 0 };

    ESndGameState(int pState, bool pTurningOn);

    ~ESndGameState() override;

    const char *GetEventName() override {
        return "ESndGameState";
    }

  private:
    int fState;      // offset: 0x8, size 0x4
    bool fTurningOn; // offset: 0xc, size 0x1
};

void ESndGameState_MakeEvent_Callback(const void *staticData);

#endif
