#ifndef EVENTS_ECHANGESTATE_H
#define EVENTS_ECHANGESTATE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class EChangeState : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        unsigned int fActivityKey; // offset: 0x4, size 0x4
        unsigned int fStateKey;    // offset: 0x8, size 0x4
    };

    // enum { kEventID = 0 };

    EChangeState(unsigned int pActivityKey, unsigned int pStateKey);

    ~EChangeState() override;

    const char *GetEventName() override;

  private:
    unsigned int fActivityKey; // offset: 0x8, size 0x4
    unsigned int fStateKey;    // offset: 0xc, size 0x4
};

void EChangeState_MakeEvent_Callback(const void *staticData);

#endif
