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
        EventList *fEventList; // offset: 0x4, size 0x4
        float fTimeDelay;      // offset: 0x8, size 0x4
    };

    // enum { kEventID = 0 };

    EScheduleEvent(EventList *pEventList, float pTimeDelay);

    ~EScheduleEvent() override;

    const char *GetEventName() override {
        return "EScheduleEvent";
    }

  private:
    EventList *fEventList; // offset: 0x8, size 0x4
    float fTimeDelay;      // offset: 0xc, size 0x4
};

void EScheduleEvent_MakeEvent_Callback(const void *staticData);

#endif
