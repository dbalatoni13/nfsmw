#include "Speed/Indep/Src/AI/AISpawnManager.h"

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Math/SimRandom.h"
#include "Speed/Indep/Src/World/TrackPath.hpp"
#include "Speed/Indep/Src/World/WRoadNetwork.h"

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

bool AISpawnManager::GetSpawnPointOnSegment(short &segInd, char &laneInd, float &timeStep) {
    static unsigned int lastSpawnLane;
    const WRoadSegment *segment = WRoadNetwork::Get().GetSegment(segInd);
    timeStep = Sim::GetRandom()._SimRandom_Float();
    int lane = WRoadNetwork::Get().GetSegmentNumTrafficLanes(*segment);
    lane = UMath::Max(lane, 1);
    lastSpawnLane++;
    if (lastSpawnLane >= static_cast< unsigned int >(lane)) {
        lastSpawnLane = 0;
    }
    laneInd = WRoadNetwork::Get().GetSegmentTrafficLaneInd(*segment, lastSpawnLane);
    return true;
}

bool AISpawnManager::GetSpawnLocation(short &segInd, char &laneInd, float &timeStep) {
    int roadInd;
    for (int i = 0; i < mNumSpawnSegments; i++) {
        roadInd = Sim::GetRandom()._SimRandom_IntRange(mNumSpawnSegments);
        segInd = mSpawnSegment[roadInd];
        if (segInd >= 0 && WRoadNetwork::Get().GetSegRoadInd(segInd) >= 0) {
            if (GetSpawnPointOnSegment(segInd, laneInd, timeStep)) {
                WRoadNav testNav;
                testNav.SetNavType(WRoadNav::kTypeTraffic);
                testNav.SetLaneType(WRoadNav::kLaneAny);
                testNav.InitAtSegment(segInd, laneInd, timeStep);
                if (testNav.IsValid()) {
                    UMath::Vector3 checkPos;
                    checkPos = testNav.GetPosition();
                    char nodeInd = testNav.GetNodeInd();
                    if (CheckSpawnPosition(checkPos, true, laneInd, nodeInd, true)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool AISpawnManager::CheckSpawnPosition(const UMath::Vector3 &checkPos, bool checkLane, int laneInd, int nodeInd, bool bCheckDist) {
    UMath::Vector3 basePos;
    GetBasePosition(basePos);
    float dist = UMath::Distancexz(checkPos, basePos);
    if (bCheckDist && dist < mMinSpawnDist) {
        return false;
    }
    if (dist > mMaxSpawnDist) {
        return false;
    }
    bVector2 position2d(checkPos.x, checkPos.z);
    TrackPathZone *zone = TheTrackPathManager.FindZone(&position2d, TRACK_PACH_ZONE_NO_COP_SPAWN, nullptr);
    if (zone != nullptr) {
        return false;
    }

    const IVehicle::List &vehicles = IVehicle::GetList(VEHICLE_ALL);
    for (IVehicle::List::const_iterator iter = vehicles.begin(); iter != vehicles.end(); ++iter) {
        IVehicle *ivehicle = *iter;
        IVehicleAI *ivehicleAI = ivehicle->GetAIVehiclePtr();
        if (ivehicleAI == nullptr) {
            continue;
        }
        WRoadNav *otherNav = ivehicleAI->GetCurrentRoad();
        if (otherNav == nullptr) {
            continue;
        }
        float distance = UMath::DistanceSquare(checkPos, ivehicle->GetPosition());
        if (otherNav->GetNavType() == WRoadNav::kTypeTraffic) {
            const float sameLaneZoneDist = 20000.0f;
            const float diffLaneZoneDist = 5000.0f;
            if (checkLane && otherNav->GetLaneInd() == laneInd) {
                if (distance < sameLaneZoneDist) {
                    return false;
                }
            } else if (otherNav->GetToLaneInd() == laneInd) {
                if (distance < sameLaneZoneDist) {
                    return false;
                }
            } else if (otherNav->GetNodeInd() == nodeInd) {
                if (distance < diffLaneZoneDist) {
                    return false;
                }
            }
        } else {
            const float otherCarZoneDist = 5000.0f;
            if (distance < otherCarZoneDist) {
                return false;
            }
        }
    }
    return true;
}

void AISpawnManager::RefreshSpawnData() {
    static int currentSegmentIndex;
    static int spawnSegmentIndex;
    static int currentSpawnIndex;

    if (!WRoadNetwork::Get().HasValidTrafficRoads()) {
        return;
    }

    UMath::Vector3 basePos;
    GetBasePosition(basePos);

    int numSegments = WRoadNetwork::Get().GetNumSegments();
    int numChecked = UMath::Min(numSegments, 200);

    WRoadNetwork::Get().IncSegmentStamp();

    for (int i = 0; i < numChecked; i++) {
        if (currentSegmentIndex >= numSegments) {
            currentSegmentIndex = 0;
        }
        const WRoadSegment *segment = WRoadNetwork::Get().GetSegment(currentSegmentIndex);
        if (WRoadNetwork::Get().GetSegRoadInd(currentSegmentIndex) >= 0) {
            const WRoadNode *node0 = WRoadNetwork::Get().GetNode(segment->fNodeIndex[0]);
            const WRoadNode *node1 = WRoadNetwork::Get().GetNode(segment->fNodeIndex[1]);
            UMath::Vector3 node0pos;
            node0pos.x = node0->fPosition.x;
            node0pos.y = node0->fPosition.y;
            node0pos.z = node0->fPosition.z;
            UMath::Vector3 node1pos;
            node1pos.x = node1->fPosition.x;
            node1pos.y = node1->fPosition.y;
            node1pos.z = node1->fPosition.z;
            float dist0 = UMath::Distancexz(basePos, node0pos);
            float dist1 = UMath::Distancexz(basePos, node1pos);
            float minDist = UMath::Min(dist0, dist1);
            if (minDist < mMaxGatherDist) {
                if (spawnSegmentIndex >= 50) {
                    spawnSegmentIndex = 0;
                }
                mSpawnSegment[spawnSegmentIndex] = currentSegmentIndex;
                spawnSegmentIndex++;
                if (spawnSegmentIndex > mNumSpawnSegments) {
                    mNumSpawnSegments = spawnSegmentIndex;
                }
            }
        }
        currentSegmentIndex++;
    }
}
