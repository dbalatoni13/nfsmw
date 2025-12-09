#ifndef EVENTS_ENISVISUALLOOK_H
#define EVENTS_ENISVISUALLOOK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x20
class ENISVisualLook : public Event {
  public:
    // total size: 0x1c
    struct StaticData : public Event::StaticData {
        float fDesaturation; // offset: 0x4, size 0x4
        float fColourBloom;  // offset: 0x8, size 0x4
        float fBleachFactor; // offset: 0xc, size 0x4
        int fUseHDR;         // offset: 0x10, size 0x4
        float fFacingScale;  // offset: 0x14, size 0x4
        float fGrazingScale; // offset: 0x18, size 0x4
    };

    // enum { kEventID = 0 };

    ENISVisualLook(float pDesaturation, float pColourBloom, float pBleachFactor, int pUseHDR, float pFacingScale, float pGrazingScale);

    ~ENISVisualLook() override;

    const char *GetEventName() override {
        return "ENISVisualLook";
    }

  private:
    float fDesaturation; // offset: 0x8, size 0x4
    float fColourBloom;  // offset: 0xc, size 0x4
    float fBleachFactor; // offset: 0x10, size 0x4
    int fUseHDR;         // offset: 0x14, size 0x4
    float fFacingScale;  // offset: 0x18, size 0x4
    float fGrazingScale; // offset: 0x1c, size 0x4
};

void ENISVisualLook_MakeEvent_Callback(const void *staticData);

#endif
