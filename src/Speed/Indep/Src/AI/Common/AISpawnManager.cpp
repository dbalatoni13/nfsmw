#include "Speed/Indep/Src/AI/AISpawnManager.h"

#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"

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

void AISpawnManager::GetBasePosition(UMath::Vector3 &basePos) {
    eView *view = eGetView(0, false);
    if (view->IsActive()) {
        bVector3 cPos(*view->GetCamera()->GetPosition());
        bVector3 posV3;
        eUnSwizzleWorldVector(cPos, posV3);
        basePos.x = posV3.x;
        basePos.y = posV3.y;
        basePos.z = posV3.z;
    }
}

void AISpawnManager::GetBaseForwardVector(UMath::Vector3 &baseForwardVec) {
    eView *view = eGetView(0, false);
    if (view->IsActive()) {
        bVector3 cPos(*view->GetCamera()->GetDirection());
        bVector3 posV3;
        eUnSwizzleWorldVector(cPos, posV3);
        baseForwardVec.x = posV3.x;
        baseForwardVec.y = posV3.y;
        baseForwardVec.z = posV3.z;
    }
}

bool AISpawnManager::RespawnAvailable(const UMath::Vector3 &position, float radius) {
    UMath::Vector3 basePos;
    GetBasePosition(basePos);
    float distToBase = UMath::Distancexz(basePos, position);
    if (mMinSpawnDist < distToBase) {
        if ((mMinSpawnDist + mMaxSpawnDist) * 0.5f < distToBase) {
            return true;
        }
        if (mMinSpawnDist < distToBase) {
            eView *view = eGetView(0, false);
            if (view->IsActive()) {
                UMath::Vector3 baseForwardVec;
                GetBaseForwardVector(baseForwardVec);
                UMath::Vector3 baseToPos;
                UMath::Sub(position, basePos, baseToPos);
                UMath::Unit(baseToPos, baseToPos);
                float dot = UMath::Dot(baseToPos, baseForwardVec);
                if (dot <= 0.0f && dot >= -1.0f) {
                    return true;
                }
            }
        }
    }
    return false;
}
