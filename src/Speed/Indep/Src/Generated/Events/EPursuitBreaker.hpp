#ifndef GENERATED_EVENTS_EPURSUITBREAKER_H
#define GENERATED_EVENTS_EPURSUITBREAKER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xC
class EPursuitBreaker : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        int fBegin; // offset 0x4, size 0x4
    };

    EPursuitBreaker(int pBegin);

    virtual ~EPursuitBreaker();

    override virtual const char *GetEventName() {
        return "EPursuitBreaker";
    }

  private:
    int fBegin; // offset 0x8, size 0x4
};

void EPursuitBreaker_MakeEvent_Callback(const void *staticData);

#endif
