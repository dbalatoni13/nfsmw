//
//
//
//
//
//
//
//
//
//
//
#ifndef __AISPAWNMANAGER_H
#define __AISPAWNMANAGER_H 1 // Decl: 13

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

// total size: 0xC
// Decl: 19
class AISpawnManager {
  protected:
    void GetBasePosition(UMath::Vector3 &basePos);

    void GetBaseForwardVector(UMath::Vector3 &baseForwardVec);

    bool RespawnAvailable(const UMath::Vector3 &position, float radius);

    bool GetSpawnLocation(short &segInd, char &laneInd, float &timeStep);

    bool CheckSpawnPosition(const UMath::Vector3 &checkPos, bool checkLane, int laneInd, int nodeInd, bool bCheckDist);

    AISpawnManager(float minSpawnDist, float maxSpawnDist);
    virtual ~AISpawnManager();

  private:
    bool GetSpawnPointOnSegment(short &segInd, char &laneInd, float &timeStep);
    void RefreshSpawnData();

    float mMinSpawnDist; // offset 0x0, size 0x4
    float mMaxSpawnDist; // offset 0x4, size 0x4

    static const int kMaxSpawnSegments; // size: 0x4, address: 0xFFFFFFFF
    static float mMaxGatherDist;        // size: 0x4, address: 0x8041547C
    static int mSpawnSegment[50];       // size: 0xC8, address: 0x804F4040
    static int mNumSpawnSegments;       // size: 0x4, address: 0x80415480
};

#endif
