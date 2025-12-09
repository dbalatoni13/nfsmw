#ifndef EVENTS_ESPAWNSMACKABLE_H
#define EVENTS_ESPAWNSMACKABLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x34
class ESpawnSmackable : public Event {
  public:
    // total size: 0x28
    struct StaticData : public Event::StaticData {
        UMath::Vector3 fPosition;    // offset: 0x4, size 0xc
        HMODEL fScenery;             // offset: 0x10, size 0x4
        UMath::Vector4 fOrientation; // offset: 0x14, size 0x10
        bool fVirginSpawn;           // offset: 0x24, size 0x1
    };

    // enum { kEventID = 0 };

    ESpawnSmackable(UMath::Vector3 pPosition, HMODEL pScenery, UMath::Vector4 pOrientation, bool pVirginSpawn, WTrigger *pTrigger,
                    unsigned int phSimable);

    ~ESpawnSmackable() override;

    const char *GetEventName() override {
        return "ESpawnSmackable";
    }

  private:
    UMath::Vector3 fPosition;    // offset: 0x8, size 0xc
    HMODEL fScenery;             // offset: 0x14, size 0x4
    UMath::Vector4 fOrientation; // offset: 0x18, size 0x10
    bool fVirginSpawn;           // offset: 0x28, size 0x1

    WTrigger *fTrigger;     // offset: 0x30, size 0x4
    unsigned int fhSimable; // offset: 0x34, size 0x4
};

void ESpawnSmackable_MakeEvent_Callback(const void *staticData);

#endif
