#ifndef EVENTS_ETERMINATEMUSIC_H
#define EVENTS_ETERMINATEMUSIC_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class ETerminateMusic : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    ETerminateMusic();

    ~ETerminateMusic() override;

    const char *GetEventName() const override;
};

void ETerminateMusic_MakeEvent_Callback(const void *staticData);
int ETerminateMusic_MakeEvent_LuaBinding(struct lua_State *L);
void ETerminateMusic_ResolveEvent_Callback(void *event, const struct UGroup *group);

#endif
