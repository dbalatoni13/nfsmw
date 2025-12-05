#ifndef EVENTS_ERELOADGAME_H
#define EVENTS_ERELOADGAME_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EReloadGame : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        GRuntimeInstance * fRestartRace; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EReloadGame(GRuntimeInstance * pRestartRace);

    override virtual ~EReloadGame();

    override virtual const char *GetEventName() {
        return "EReloadGame";
    }

  private:
    GRuntimeInstance * fRestartRace; // offset: 0x8, size 0x4
};

void EReloadGame_MakeEvent_Callback(const void *staticData);

#endif
