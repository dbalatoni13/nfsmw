#ifndef EVENTS_ERESETPROPS_H
#define EVENTS_ERESETPROPS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class EResetProps : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    EResetProps();

    ~EResetProps() override;

    const char *GetEventName() override {
        return "EResetProps";
    }
};

void EResetProps_MakeEvent_Callback(const void *staticData);

#endif
