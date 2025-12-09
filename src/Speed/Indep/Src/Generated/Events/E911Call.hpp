#ifndef EVENTS_E911CALL_H
#define EVENTS_E911CALL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class E911Call : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    E911Call();

    ~E911Call() override;

    const char *GetEventName() override {
        return "E911Call";
    }
};

void E911Call_MakeEvent_Callback(const void *staticData);

#endif
