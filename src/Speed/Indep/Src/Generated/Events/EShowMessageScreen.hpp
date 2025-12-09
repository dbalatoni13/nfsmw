#ifndef EVENTS_ESHOWMESSAGESCREEN_H
#define EVENTS_ESHOWMESSAGESCREEN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EShowMessageScreen : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        unsigned int fID; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EShowMessageScreen(unsigned int pID);

    ~EShowMessageScreen() override;

    const char *GetEventName() override {
        return "EShowMessageScreen";
    }

  private:
    unsigned int fID; // offset: 0x8, size 0x4
};

void EShowMessageScreen_MakeEvent_Callback(const void *staticData);

#endif
