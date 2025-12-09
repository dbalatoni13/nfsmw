#ifndef EVENTS_ENISFREEZE_H
#define EVENTS_ENISFREEZE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class ENISFreeze : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        int fFreezeAnimation; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    ENISFreeze(int pFreezeAnimation);

    ~ENISFreeze() override;

    const char *GetEventName() override {
        return "ENISFreeze";
    }

  private:
    int fFreezeAnimation; // offset: 0x8, size 0x4
};

void ENISFreeze_MakeEvent_Callback(const void *staticData);

#endif
