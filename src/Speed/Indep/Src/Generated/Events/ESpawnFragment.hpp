#ifndef EVENTS_ESPAWNFRAGMENT_H
#define EVENTS_ESPAWNFRAGMENT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x58
class ESpawnFragment : public Event {
  public:
    // total size: 0x30
    struct StaticData : public Event::StaticData {
        UCrc32 fPartName;               // offset: 0x4, size 0x4
        UCrc32 fCollisionName;          // offset: 0x8, size 0x4
        UCrc32 fAttributeName;          // offset: 0xc, size 0x4
        UCrc32 fJointName;              // offset: 0x10, size 0x4
        unsigned int fAllowDupes;       // offset: 0x14, size 0x4
        float fInheritVelocity;         // offset: 0x18, size 0x4
        unsigned int fHideParent;       // offset: 0x1c, size 0x4
        unsigned int fHidePart;         // offset: 0x20, size 0x4
        unsigned int fAllowHiddenSpawn; // offset: 0x24, size 0x4
        unsigned int fNoPhysics;        // offset: 0x28, size 0x4
        unsigned int fSimplePhysics;    // offset: 0x2c, size 0x4
    };

    // enum { kEventID = 0 };

    ESpawnFragment(UCrc32 pPartName, UCrc32 pCollisionName, UCrc32 pAttributeName, UCrc32 pJointName, unsigned int pAllowDupes,
                   float pInheritVelocity, unsigned int pHideParent, unsigned int pHidePart, unsigned int pAllowHiddenSpawn, unsigned int pNoPhysics,
                   unsigned int pSimplePhysics, unsigned int phModel, UMath::Vector4 pVelocity, UMath::Vector4 pAngularVelocity);

    ~ESpawnFragment() override;

    const char *GetEventName() override;

  private:
    UCrc32 fPartName;               // offset: 0x8, size 0x4
    UCrc32 fCollisionName;          // offset: 0xc, size 0x4
    UCrc32 fAttributeName;          // offset: 0x10, size 0x4
    UCrc32 fJointName;              // offset: 0x14, size 0x4
    unsigned int fAllowDupes;       // offset: 0x18, size 0x4
    float fInheritVelocity;         // offset: 0x1c, size 0x4
    unsigned int fHideParent;       // offset: 0x20, size 0x4
    unsigned int fHidePart;         // offset: 0x24, size 0x4
    unsigned int fAllowHiddenSpawn; // offset: 0x28, size 0x4
    unsigned int fNoPhysics;        // offset: 0x2c, size 0x4
    unsigned int fSimplePhysics;    // offset: 0x30, size 0x4

    unsigned int fhModel;            // offset: 0x38, size 0x4
    UMath::Vector4 fVelocity;        // offset: 0x3c, size 0x10
    UMath::Vector4 fAngularVelocity; // offset: 0x4c, size 0x10
};

void ESpawnFragment_MakeEvent_Callback(const void *staticData);

#endif
