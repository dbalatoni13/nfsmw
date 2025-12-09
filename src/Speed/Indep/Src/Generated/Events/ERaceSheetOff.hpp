#ifndef EVENTS_ERACESHEETOFF_H
#define EVENTS_ERACESHEETOFF_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class ERaceSheetOff : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    ERaceSheetOff();

    ~ERaceSheetOff() override;

    const char *GetEventName() override {
        return "ERaceSheetOff";
    }
};

void ERaceSheetOff_MakeEvent_Callback(const void *staticData);

#endif
