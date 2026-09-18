#ifndef EVENTS_EDISABLETRIGGER_H
#define EVENTS_EDISABLETRIGGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EDisableTrigger : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        CARP::Trigger *fTrigger; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EDisableTrigger(CARP::Trigger *pTrigger);

    ~EDisableTrigger() override;

    const char *GetEventName() const override;

  private:
    CARP::Trigger *fTrigger; // offset: 0x8, size 0x4
};

void EDisableTrigger_MakeEvent_Callback(const void *staticData);
int EDisableTrigger_MakeEvent_LuaBinding(struct lua_State *L);
void EDisableTrigger_ResolveEvent_Callback(void *event, const struct UGroup *group);

#endif
