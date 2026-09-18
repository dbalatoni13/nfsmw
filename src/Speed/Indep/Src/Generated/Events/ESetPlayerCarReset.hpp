#ifndef EVENTS_ESETPLAYERCARRESET_H
#define EVENTS_ESETPLAYERCARRESET_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class ESetPlayerCarReset : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        int fUseTrigger;   // offset: 0x4, size 0x4
        CARP::Trigger *fTrigger; // offset: 0x8, size 0x4
    };

    // enum { kEventID = 0 };

    ESetPlayerCarReset(int pUseTrigger, CARP::Trigger *pTrigger);

    ~ESetPlayerCarReset() override;

    const char *GetEventName() const override;

  private:
    int fUseTrigger;   // offset: 0x8, size 0x4
    CARP::Trigger *fTrigger; // offset: 0xc, size 0x4
};

void ESetPlayerCarReset_MakeEvent_Callback(const void *staticData);
int ESetPlayerCarReset_MakeEvent_LuaBinding(struct lua_State *L);
void ESetPlayerCarReset_ResolveEvent_Callback(void *event, const struct UGroup *group);

#endif
