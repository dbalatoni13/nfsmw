#ifndef EVENTS_ESCHEDULEEVENT_H
#define EVENTS_ESCHEDULEEVENT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class EScheduleEvent : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        CARP::EventList *fEventList; // offset: 0x4, size 0x4
        float fTimeDelay;      // offset: 0x8, size 0x4
    };

    // enum { kEventID = 0 };

    EScheduleEvent(CARP::EventList *pEventList, float pTimeDelay);

    ~EScheduleEvent() override;

    const char *GetEventName() const override;

  private:
    CARP::EventList *fEventList; // offset: 0x8, size 0x4
    float fTimeDelay;      // offset: 0xc, size 0x4
};

void EScheduleEvent_MakeEvent_Callback(const void *staticData);
int EScheduleEvent_MakeEvent_LuaBinding(struct lua_State *L);
void EScheduleEvent_ResolveEvent_Callback(void *event, const struct UGroup *group);

#endif
