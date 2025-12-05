#ifndef EVENTS_EJUMPTOSTRATEGYFLOW_H
#define EVENTS_EJUMPTOSTRATEGYFLOW_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class EJumpToStrategyFlow : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {
    };

    // enum { kEventID = 0 };

    EJumpToStrategyFlow();

    override virtual ~EJumpToStrategyFlow();

    override virtual const char *GetEventName() {
        return "EJumpToStrategyFlow";
    }
};

void EJumpToStrategyFlow_MakeEvent_Callback(const void *staticData);

#endif
