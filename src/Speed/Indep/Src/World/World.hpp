#ifndef WORLD_WORLD_H
#define WORLD_WORLD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/World/WCollider.h"
#include "Speed/Indep/Src/World/WCollisionTri.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribCollection.h"

class WWorldPos {
    // total size: 0x3C
  public:
    bool Update(const UMath::Vector3 &pos, UMath::Vector4 &dest, bool usecache, const WCollider *collider, bool keep_valid);

  private:
    WCollisionTri fFace;                // offset 0x0, size 0x30
    unsigned int fFaceValid : 1;        // offset 0x30, size 0x4
    unsigned int fMissCount : 15;       // offset 0x30, size 0x4
    unsigned int fUsageCount : 16;      // offset 0x30, size 0x4
    float fYOffset;                     // offset 0x34, size 0x4
    const Attrib::Collection *fSurface; // offset 0x38, size 0x4
};

enum eTimeOfDay {
    eTOD_NUM_TIMES_OF_DAY = 2,
    eTOD_SUNSET = 1,
    eTOD_MIDDAY = 0,
    eTOD_ERROR = -1,
};

eTimeOfDay GetCurrentTimeOfDay();

#endif
