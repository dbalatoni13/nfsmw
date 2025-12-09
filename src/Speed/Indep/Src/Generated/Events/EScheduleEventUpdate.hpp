#ifndef EVENTS_ESCHEDULEEVENTUPDATE_H
#define EVENTS_ESCHEDULEEVENTUPDATE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x1c
class EScheduleEventUpdate : public Event {
  public:
    // total size: 0x18
    struct StaticData : public Event::StaticData {
        void *fStaticData;       // offset: 0x4, size 0x4
        EventList *fEventList;   // offset: 0x8, size 0x4
        int fTaskHandle;         // offset: 0xc, size 0x4
        float fExecuteOnSimTime; // offset: 0x10, size 0x4
        int fResetCount;         // offset: 0x14, size 0x4
    };

    // enum { kEventID = 0 };

    EScheduleEventUpdate(void *pStaticData, EventList *pEventList, int pTaskHandle, float pExecuteOnSimTime, int pResetCount);

    ~EScheduleEventUpdate() override;

    const char *GetEventName() override {
        return "EScheduleEventUpdate";
    }

  private:
    void *fStaticData;       // offset: 0x8, size 0x4
    EventList *fEventList;   // offset: 0xc, size 0x4
    int fTaskHandle;         // offset: 0x10, size 0x4
    float fExecuteOnSimTime; // offset: 0x14, size 0x4
    int fResetCount;         // offset: 0x18, size 0x4
};

void EScheduleEventUpdate_MakeEvent_Callback(const void *staticData);

#endif
