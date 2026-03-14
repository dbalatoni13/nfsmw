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
    mMaxGatherDist = UMath::Max(maxSpawnDist, mMaxGatherDist);
    for (int i = 0; i < 50; i++) {
        mSpawnSegment[i] = -1;
    }
}

AISpawnManager::~AISpawnManager() {
}

void AISpawnManager::GetBasePosition(UMath::Vector3 &basePos) {
    eView *view = eGetView(1, false);
    if (!view->IsActive()) {
        basePos = UMath::Vector3::kZero;
    } else {
        bVector3 cPos(*view->GetCamera()->GetPosition());
        bVector3 posV3;
        eUnSwizzleWorldVector(cPos, posV3);
        basePos.x = posV3.x;
        basePos.y = posV3.y;
        basePos.z = posV3.z;
    }
}

void AISpawnManager::GetBaseForwardVector(UMath::Vector3 &baseForwardVec) {
    eView *view = eGetView(1, false);
    if (!view->IsActive()) {
        baseForwardVec = UMath::Vector3::kZero;
    } else {
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
    float distToBase = UMath::Distancexz(position, basePos);
    if (distToBase > mMinSpawnDist) {
        if (distToBase > (mMaxSpawnDist + mMinSpawnDist) * 0.5f) {
            return true;
        }
        if (distToBase > mMinSpawnDist) {
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

    WRoadNetwork::Get().IncSegmentStamp();

    int numSegments = WRoadNetwork::Get().GetNumSegments();
    int nextSegmentIndex = UMath::Min(numSegments, currentSegmentIndex + 20);
    float maxDistSpawn = mMaxGatherDist;

    UMath::Vector3 basePos;
    GetBasePosition(basePos);

    IPlayer *player = IPlayer::First(PLAYER_LOCAL);
    UMath::Vector3 linearVelocity;

    if (player && player->GetSimable()) {
        const UMath::Vector3 &pos = player->GetSimable()->GetRigidBody()->GetPosition();
        linearVelocity.x = pos.x;
        linearVelocity.y = 0.0f;
        linearVelocity.z = pos.z;
    } else {
        linearVelocity.x = UMath::Vector3::kZero.x;
        linearVelocity.y = 0.0f;
        linearVelocity.z = UMath::Vector3::kZero.z;
    }

    UMath::Add(basePos, linearVelocity, basePos);

    for (int i = currentSegmentIndex; i < nextSegmentIndex; i++) {
        const WRoadSegment *segment = WRoadNetwork::Get().GetSegment(i);
        if (!segment->IsTrafficAllowed()) {
            continue;
        }
        if (segment->IsDecision()) {
            continue;
        }
        if (segment->GetLength() < 10.0f) {
            continue;
        }
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
        float node0Distance = UMath::Distance(basePos, node0pos);
        float node1Distance = UMath::Distance(basePos, node1pos);
        if (node0Distance <= maxDistSpawn + 50.0f || node1Distance <= maxDistSpawn + 50.0f) {
            mSpawnSegment[spawnSegmentIndex] = i;
            int nextSpawnIndex = spawnSegmentIndex + 1;
            mNumSpawnSegments = UMath::Max(nextSpawnIndex, mNumSpawnSegments);
            spawnSegmentIndex = nextSpawnIndex;
            if (spawnSegmentIndex > 49) {
                spawnSegmentIndex = 0;
            }
        }
    }

    currentSegmentIndex = nextSegmentIndex;
    if (currentSegmentIndex >= numSegments) {
        currentSegmentIndex = 0;
    }

    int numToCheck = UMath::Min(currentSpawnIndex + 5, 50);
    for (int i = currentSpawnIndex; i < numToCheck; i++) {
        if (mSpawnSegment[i] >= 0) {
            const WRoadSegment *segment = WRoadNetwork::Get().GetSegment(mSpawnSegment[i]);
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
            float node0Distance = UMath::Distance(basePos, node0pos);
            float node1Distance = UMath::Distance(basePos, node1pos);
            if (node0Distance > maxDistSpawn + 50.0f && node1Distance > maxDistSpawn + 50.0f) {
                mSpawnSegment[i] = -1;
            }
        }
    }

    currentSpawnIndex = numToCheck;
    if (currentSpawnIndex > 49) {
        currentSpawnIndex = 0;
    }
}
