#ifndef EVENTS_EDISPINTRORACE_H
#define EVENTS_EDISPINTRORACE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class EDispIntroRace : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    EDispIntroRace();

    ~EDispIntroRace() override;

    const char *GetEventName() const override;
};

void EDispIntroRace_MakeEvent_Callback(const void *staticData);
int EDispIntroRace_MakeEvent_LuaBinding(struct lua_State *L);
void EDispIntroRace_ResolveEvent_Callback(void *event, const struct UGroup *group);

#endif
