#ifndef EVENTS_ECAMERASHAKE_H
#define EVENTS_ECAMERASHAKE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class ECameraShake : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    ECameraShake();

    ~ECameraShake() override;

    const char *GetEventName() const override;
};

void ECameraShake_MakeEvent_Callback(const void *staticData);
int ECameraShake_MakeEvent_LuaBinding(struct lua_State *L);
void ECameraShake_ResolveEvent_Callback(void *event, const struct UGroup *group);

#endif
