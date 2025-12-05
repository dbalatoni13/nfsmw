#ifndef EVENTS_ELOADLOST_H
#define EVENTS_ELOADLOST_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class ELoadLost : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {
    };

    // enum { kEventID = 0 };

    ELoadLost(unsigned int phSimable);

    override virtual ~ELoadLost();

    override virtual const char *GetEventName() {
        return "ELoadLost";
    }

  private:
    unsigned int fhSimable; // offset: 0xc, size 0x4
};

void ELoadLost_MakeEvent_Callback(const void *staticData);

#endif
