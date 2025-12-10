#ifndef EVENTS_EADDSMS_H
#define EVENTS_EADDSMS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EAddSMS : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        int fNumber; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EAddSMS(int pNumber);

    ~EAddSMS() override;

    const char *GetEventName() override;

  private:
    int fNumber; // offset: 0x8, size 0x4
};

void EAddSMS_MakeEvent_Callback(const void *staticData);

#endif
