#ifndef AI_COMMON_AISPAWNMANAGER_H
#define AI_COMMON_AISPAWNMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

// total size: 0xC
class AISpawnManager {
  public:
    AISpawnManager(float minSpawnDist, float maxSpawnDist);

    void GetBasePosition(UMath::Vector3 &basePos);

    void GetBaseForwardVector(UMath::Vector3 &baseForwardVec);

    bool RespawnAvailable(const UMath::Vector3 &position, float radius);

    bool GetSpawnPointOnSegment(short &segInd, char &laneInd, float &timeStep);

    bool GetSpawnLocation(short &segInd, char &laneInd, float &timeStep);

    bool CheckSpawnPosition(const UMath::Vector3 &checkPos, bool checkLane, int laneInd, int nodeInd, bool bCheckDist);

    void RefreshSpawnData();

    // Virtual methods
    virtual ~AISpawnManager();

    // Static members
    static const int kMaxSpawnSegments; // size: 0x4, address: 0xFFFFFFFF
    static float mMaxGatherDist;        // size: 0x4, address: 0x8041547C
    static int mSpawnSegment[50];       // size: 0xC8, address: 0x804F4040
    static int mNumSpawnSegments;       // size: 0x4, address: 0x80415480

  private:
    float mMinSpawnDist; // offset 0x0, size 0x4
    float mMaxSpawnDist; // offset 0x4, size 0x4
};

#endif
