#ifndef EVENTS_EENABLEMODELING_H
#define EVENTS_EENABLEMODELING_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class EEnableModeling : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        int fEnable; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EEnableModeling(int pEnable, unsigned int phSimable);

    ~EEnableModeling() override;

    const char *GetEventName() override;

  private:
    int fEnable; // offset: 0x8, size 0x4

    unsigned int fhSimable; // offset: 0x10, size 0x4
};

void EEnableModeling_MakeEvent_Callback(const void *staticData);

#endif
