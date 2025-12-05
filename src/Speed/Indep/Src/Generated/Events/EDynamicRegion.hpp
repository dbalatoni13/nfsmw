#ifndef EVENTS_EDYNAMICREGION_H
#define EVENTS_EDYNAMICREGION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x40
class EDynamicRegion : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        int fOn; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EDynamicRegion(int pOn, UMath::Vector4 pPosition, UMath::Vector4 pVector, UMath::Vector4 pVelocity, unsigned int phSimable);

    override virtual ~EDynamicRegion();

    override virtual const char *GetEventName() {
        return "EDynamicRegion";
    }

  private:
    int fOn; // offset: 0x8, size 0x4

    UMath::Vector4 fPosition; // offset: 0x10, size 0x10
    UMath::Vector4 fVector; // offset: 0x20, size 0x10
    UMath::Vector4 fVelocity; // offset: 0x30, size 0x10
    unsigned int fhSimable; // offset: 0x40, size 0x4
};

void EDynamicRegion_MakeEvent_Callback(const void *staticData);

#endif
