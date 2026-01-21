#ifndef EVENTS_EENABLEAIPHYSICS_H
#define EVENTS_EENABLEAIPHYSICS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x14
class EEnableAIPhysics : public Event {
  public:
    // total size: 0x10
    struct StaticData : public Event::StaticData {
        uintptr_t fhSimable;  // offset: 0x4, size 0x4
        float fSpeed;         // offset: 0x8, size 0x4
        unsigned int fEnable; // offset: 0xc, size 0x4
    };

    // enum { kEventID = 0 };

    EEnableAIPhysics(uintptr_t phSimable, float pSpeed, unsigned int pEnable);

    ~EEnableAIPhysics() override;

    const char *GetEventName() override;

  private:
    uintptr_t fhSimable;  // offset: 0x8, size 0x4
    float fSpeed;         // offset: 0xc, size 0x4
    unsigned int fEnable; // offset: 0x10, size 0x4
};

void EEnableAIPhysics_MakeEvent_Callback(const void *staticData);

#endif
