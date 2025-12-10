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
        EventList *fEventList; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    ERandomEventList(EventList *pEventList);

    ~ERandomEventList() override;

    const char *GetEventName() override;

  private:
    EventList *fEventList; // offset: 0x8, size 0x4
};

void ERandomEventList_MakeEvent_Callback(const void *staticData);

#endif
