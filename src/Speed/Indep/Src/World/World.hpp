#ifndef WORLD_WORLD_H
#define WORLD_WORLD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Replay.hpp"
#include "Speed/Indep/Src/World/Car.hpp"
#include "Speed/Indep/Src/World/WCollider.h"
#include "Speed/Indep/Src/World/WCollisionTri.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

struct TrafficTeleporter { // ion even know
};

class World {
public:
    World();
    ~World();

    void DoSnapshot(ReplaySnapshot *snapshot);
    void ResetTimeScale();
    // there's a lot more, but they aren't used
    // too bad!

private:
    // total size: 0xE8
    bTList<Car> CarList; // offset 0x0, size 0x8
    int TotalNumCars; // offset 0x8, size 0x4
    int AICarPos; // offset 0xC, size 0x4
    int CopCarPos; // offset 0x10, size 0x4
    int TrafficCarPos; // offset 0x14, size 0x4
    int ParkedCarPos; // offset 0x18, size 0x4
    Car *CarTable[10]; // offset 0x1C, size 0x28
    bVector3 PlayerPositions[6]; // offset 0x44, size 0x60
    float PlayerDATs[6]; // offset 0xA4, size 0x18
    unsigned int WorldRandomSeed; // offset 0xBC, size 0x4
    float fTimeToRemainPaused; // offset 0xC0, size 0x4
    int OnlinePauseWorld; // offset 0xC4, size 0x4
    float PreviousTimeToWaste; // offset 0xC8, size 0x4
    float fTimeScale; // offset 0xCC, size 0x4
    float fTimeScaleTimer; // offset 0xD0, size 0x4
    void (* TimeScaleCallback)(int); // offset 0xD4, size 0x4
    int TimeScaleCallbackParam; // offset 0xD8, size 0x4
    TrafficTeleporter *Teleporter; // offset 0xDC, size 0x4
    unsigned int CollisionPredictionID; // offset 0xE0, size 0x4
    unsigned int PotentialDriveTargetID; // offset 0xE4, size 0x4
};

extern World *pCurrentWorld;

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

extern int g_tweakIsDriftRace;

void ServiceSpaceNodes();
void ResetWorldTime();
eTimeOfDay GetCurrentTimeOfDay();

#endif
