#include "Speed/Indep/Src/AI/AISpawnManager.h"

AISpawnManager::AISpawnManager(float minSpawnDist, float maxSpawnDist)
    : mMinSpawnDist(minSpawnDist) //
    , mMaxSpawnDist(maxSpawnDist) {
    mMaxGatherDist = UMath::Max(mMaxGatherDist, maxSpawnDist);
    for (int i = 0; i < 50; i++) {
        mSpawnSegment[i] = -1;
    }
}

AISpawnManager::~AISpawnManager() {
}
