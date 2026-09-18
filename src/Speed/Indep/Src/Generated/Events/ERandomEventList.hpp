#ifndef EVENTS_ERANDOMEVENTLIST_H
#define EVENTS_ERANDOMEVENTLIST_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class ERandomEventList : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        CARP::EventList *fEventList; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    ERandomEventList(CARP::EventList *pEventList);

    ~ERandomEventList() override;

    const char *GetEventName() const override;

  private:
    CARP::EventList *fEventList; // offset: 0x8, size 0x4
};

void ERandomEventList_MakeEvent_Callback(const void *staticData);
int ERandomEventList_MakeEvent_LuaBinding(struct lua_State *L);
void ERandomEventList_ResolveEvent_Callback(void *event, const struct UGroup *group);

#endif
