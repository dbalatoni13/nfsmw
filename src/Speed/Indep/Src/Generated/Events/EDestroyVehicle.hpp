#ifndef EVENTS_EDESTROYVEHICLE_H
#define EVENTS_EDESTROYVEHICLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EDestroyVehicle : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    EDestroyVehicle(unsigned int phSimable);

    ~EDestroyVehicle() override;

    const char *GetEventName() const override;

  private:
    unsigned int fhSimable; // offset: 0xc, size 0x4
};

void EDestroyVehicle_MakeEvent_Callback(const void *staticData);
int EDestroyVehicle_MakeEvent_LuaBinding(struct lua_State *L);
void EDestroyVehicle_ResolveEvent_Callback(void *event, const struct UGroup *group);

#endif
