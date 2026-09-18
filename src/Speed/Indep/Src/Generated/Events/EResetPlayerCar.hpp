#ifndef EVENTS_ERESETPLAYERCAR_H
#define EVENTS_ERESETPLAYERCAR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class EResetPlayerCar : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    EResetPlayerCar();

    ~EResetPlayerCar() override;

    const char *GetEventName() const override;
};

void EResetPlayerCar_MakeEvent_Callback(const void *staticData);
int EResetPlayerCar_MakeEvent_LuaBinding(struct lua_State *L);
void EResetPlayerCar_ResolveEvent_Callback(void *event, const struct UGroup *group);

#endif
