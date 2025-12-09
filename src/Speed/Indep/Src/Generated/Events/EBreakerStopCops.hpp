#ifndef EVENTS_EBREAKERSTOPCOPS_H
#define EVENTS_EBREAKERSTOPCOPS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x20
class EBreakerStopCops : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        float fEventRadius; // offset: 0x4, size 0x4
        float fDuration;    // offset: 0x8, size 0x4
    };

    // enum { kEventID = 0 };

    EBreakerStopCops(float pEventRadius, float pDuration, UMath::Vector4 pPosition);

    ~EBreakerStopCops() override;

    const char *GetEventName() override {
        return "EBreakerStopCops";
    }

  private:
    float fEventRadius; // offset: 0x8, size 0x4
    float fDuration;    // offset: 0xc, size 0x4

    UMath::Vector4 fPosition; // offset: 0x14, size 0x10
};

void EBreakerStopCops_MakeEvent_Callback(const void *staticData);

#endif
