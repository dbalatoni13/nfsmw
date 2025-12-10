#ifndef EVENTS_ENISDETAIL_H
#define EVENTS_ENISDETAIL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class ENISDetail : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        int fSceneryDetail; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    ENISDetail(int pSceneryDetail);

    ~ENISDetail() override;

    const char *GetEventName() override;

  private:
    int fSceneryDetail; // offset: 0x8, size 0x4
};

void ENISDetail_MakeEvent_Callback(const void *staticData);

#endif
