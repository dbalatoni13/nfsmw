#ifndef EVENTS_EBECOMEPURSUITCAR_H
#define EVENTS_EBECOMEPURSUITCAR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EBecomePursuitCar : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    EBecomePursuitCar(unsigned int phSimable);

    ~EBecomePursuitCar() override;

    const char *GetEventName() const override;

  private:
    unsigned int fhSimable; // offset: 0x8, size 0x4
};

void EBecomePursuitCar_MakeEvent_Callback(const void *staticData);
int EBecomePursuitCar_MakeEvent_LuaBinding(struct lua_State *L);
void EBecomePursuitCar_ResolveEvent_Callback(void *event, const struct UGroup *group);

#endif
