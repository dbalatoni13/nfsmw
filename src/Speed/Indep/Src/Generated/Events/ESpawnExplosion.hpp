#ifndef EVENTS_ESPAWNEXPLOSION_H
#define EVENTS_ESPAWNEXPLOSION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x30
class ESpawnExplosion : public Event {
  public:
    // total size: 0x28
    struct StaticData : public Event::StaticData {
        UCrc32 fPosition;                // offset: 0x4, size 0x4
        float fSpeed;                    // offset: 0x8, size 0x4
        float fRadius;                   // offset: 0xc, size 0x4
        float fStartRadius;              // offset: 0x10, size 0x4
        unsigned int fEffectThis;        // offset: 0x14, size 0x4
        unsigned int fDoesDamage;        // offset: 0x18, size 0x4
        unsigned int fIncludePlayers;    // offset: 0x1c, size 0x4
        unsigned int fExcludeVehicles;   // offset: 0x20, size 0x4
        unsigned int fExcludeSmackables; // offset: 0x24, size 0x4
    };

    // enum { kEventID = 0 };

    ESpawnExplosion(UCrc32 pPosition, float pSpeed, float pRadius, float pStartRadius, unsigned int pEffectThis, unsigned int pDoesDamage,
                    unsigned int pIncludePlayers, unsigned int pExcludeVehicles, unsigned int pExcludeSmackables, unsigned int phModel);

    ~ESpawnExplosion() override;

    const char *GetEventName() override {
        return "ESpawnExplosion";
    }

  private:
    UCrc32 fPosition;                // offset: 0x8, size 0x4
    float fSpeed;                    // offset: 0xc, size 0x4
    float fRadius;                   // offset: 0x10, size 0x4
    float fStartRadius;              // offset: 0x14, size 0x4
    unsigned int fEffectThis;        // offset: 0x18, size 0x4
    unsigned int fDoesDamage;        // offset: 0x1c, size 0x4
    unsigned int fIncludePlayers;    // offset: 0x20, size 0x4
    unsigned int fExcludeVehicles;   // offset: 0x24, size 0x4
    unsigned int fExcludeSmackables; // offset: 0x28, size 0x4

    unsigned int fhModel; // offset: 0x30, size 0x4
};

void ESpawnExplosion_MakeEvent_Callback(const void *staticData);

#endif
