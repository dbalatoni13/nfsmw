#ifndef EVENTS_ERELOADGAME_H
#define EVENTS_ERELOADGAME_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

class GRuntimeInstance;

// total size: 0xc
class EReloadGame : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        GRuntimeInstance *fRestartRace; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EReloadGame(GRuntimeInstance *pRestartRace);

    ~EReloadGame() override;

    const char *GetEventName() const override;

  private:
    GRuntimeInstance *fRestartRace; // offset: 0x8, size 0x4
};

void EReloadGame_MakeEvent_Callback(const void *staticData);
int EReloadGame_MakeEvent_LuaBinding(struct lua_State *L);
void EReloadGame_ResolveEvent_Callback(void *event, const struct UGroup *group);

#endif
