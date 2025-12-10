#ifndef EVENTS_EPLAYOBJECTEFFECT_H
#define EVENTS_EPLAYOBJECTEFFECT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x34
class EPlayObjectEffect : public Event {
  public:
    // total size: 0x1c
    struct StaticData : public Event::StaticData {
        const char *fEffect;            // offset: 0x4, size 0x4
        UCrc32 fPosition;               // offset: 0x8, size 0x4
        float fIntensity;               // offset: 0xc, size 0x4
        UCrc32 fID;                     // offset: 0x10, size 0x4
        unsigned int fTracking;         // offset: 0x14, size 0x4
        unsigned int fAllowHiddenSpawn; // offset: 0x18, size 0x4
    };

    // enum { kEventID = 0 };

    EPlayObjectEffect(const char *pEffect, UCrc32 pPosition, float pIntensity, UCrc32 pID, unsigned int pTracking, unsigned int pAllowHiddenSpawn,
                      unsigned int phModel, UMath::Vector4 pVelocity);

    ~EPlayObjectEffect() override;

    const char *GetEventName() override;

  private:
    const char *fEffect;            // offset: 0x8, size 0x4
    UCrc32 fPosition;               // offset: 0xc, size 0x4
    float fIntensity;               // offset: 0x10, size 0x4
    UCrc32 fID;                     // offset: 0x14, size 0x4
    unsigned int fTracking;         // offset: 0x18, size 0x4
    unsigned int fAllowHiddenSpawn; // offset: 0x1c, size 0x4

    unsigned int fhModel;     // offset: 0x24, size 0x4
    UMath::Vector4 fVelocity; // offset: 0x28, size 0x10
};

void EPlayObjectEffect_MakeEvent_Callback(const void *staticData);

#endif
