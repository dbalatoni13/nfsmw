#ifndef EVENTS_ENISCOPLIGHTS_H
#define EVENTS_ENISCOPLIGHTS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x1c
class ENISCopLights : public Event {
  public:
    // total size: 0x18
    struct StaticData : public Event::StaticData {
        int fEnable;           // offset: 0x4, size 0x4
        float fLightIntensity; // offset: 0x8, size 0x4
        float fLightPosX;      // offset: 0xc, size 0x4
        float fLightPosY;      // offset: 0x10, size 0x4
        float fLightPosZ;      // offset: 0x14, size 0x4
    };

    // enum { kEventID = 0 };

    ENISCopLights(int pEnable, float pLightIntensity, float pLightPosX, float pLightPosY, float pLightPosZ);

    ~ENISCopLights() override;

    const char *GetEventName() override {
        return "ENISCopLights";
    }

  private:
    int fEnable;           // offset: 0x8, size 0x4
    float fLightIntensity; // offset: 0xc, size 0x4
    float fLightPosX;      // offset: 0x10, size 0x4
    float fLightPosY;      // offset: 0x14, size 0x4
    float fLightPosZ;      // offset: 0x18, size 0x4
};

void ENISCopLights_MakeEvent_Callback(const void *staticData);

#endif
