#ifndef EVENTS_EBECOMEAICAR_H
#define EVENTS_EBECOMEAICAR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class EBecomeAiCar : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    EBecomeAiCar();

    ~EBecomeAiCar() override;

    const char *GetEventName() const override;
};

void EBecomeAiCar_MakeEvent_Callback(const void *staticData);
int EBecomeAiCar_MakeEvent_LuaBinding(struct lua_State *L);
void EBecomeAiCar_ResolveEvent_Callback(void *event, const struct UGroup *group);

#endif
