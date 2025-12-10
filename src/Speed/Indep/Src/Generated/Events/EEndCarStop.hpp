#ifndef EVENTS_EENDCARSTOP_H
#define EVENTS_EENDCARSTOP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class EEndCarStop : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        int fDontRetestInput; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EEndCarStop(int pDontRetestInput, unsigned int phSimable);

    ~EEndCarStop() override;

    const char *GetEventName() override;

  private:
    int fDontRetestInput; // offset: 0x8, size 0x4

    unsigned int fhSimable; // offset: 0x10, size 0x4
};

void EEndCarStop_MakeEvent_Callback(const void *staticData);

#endif
