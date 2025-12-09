#ifndef EVENTS_ESHOWSMS_H
#define EVENTS_ESHOWSMS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EShowSMS : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        int fShowMsg; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EShowSMS(int pShowMsg);

    ~EShowSMS() override;

    const char *GetEventName() override {
        return "EShowSMS";
    }

  private:
    int fShowMsg; // offset: 0x8, size 0x4
};

void EShowSMS_MakeEvent_Callback(const void *staticData);

#endif
