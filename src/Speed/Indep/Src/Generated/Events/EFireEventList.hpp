#ifndef EVENTS_EFIREEVENTLIST_H
#define EVENTS_EFIREEVENTLIST_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x14
class EFireEventList : public Event {
  public:
    // total size: 0x10
    struct StaticData : public Event::StaticData {
        EventList *fEventList; // offset: 0x4, size 0x4
        int fDelayed;          // offset: 0x8, size 0x4
        int fVerbose;          // offset: 0xc, size 0x4
    };

    // enum { kEventID = 0 };

    EFireEventList(EventList *pEventList, int pDelayed, int pVerbose);

    ~EFireEventList() override;

    const char *GetEventName() override {
        return "EFireEventList";
    }

  private:
    EventList *fEventList; // offset: 0x8, size 0x4
    int fDelayed;          // offset: 0xc, size 0x4
    int fVerbose;          // offset: 0x10, size 0x4
};

void EFireEventList_MakeEvent_Callback(const void *staticData);

#endif
