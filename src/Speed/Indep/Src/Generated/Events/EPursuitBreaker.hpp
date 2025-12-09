#ifndef EVENTS_EPURSUITBREAKER_H
#define EVENTS_EPURSUITBREAKER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EPursuitBreaker : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        int fBegin; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EPursuitBreaker(int pBegin);

    ~EPursuitBreaker() override;

    const char *GetEventName() override {
        return "EPursuitBreaker";
    }

  private:
    int fBegin; // offset: 0x8, size 0x4
};

void EPursuitBreaker_MakeEvent_Callback(const void *staticData);

#endif
