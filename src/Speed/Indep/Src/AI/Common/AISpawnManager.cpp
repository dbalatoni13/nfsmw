#include "Speed/Indep/Src/AI/AISpawnManager.h"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/World/TrackPath.hpp"
#include "Speed/Indep/Src/World/WRoadNetwork.h"

float AISpawnManager::mMaxGatherDist = 0.0f;
int AISpawnManager::mSpawnSegment[50];
int AISpawnManager::mNumSpawnSegments = 0;

AISpawnManager::AISpawnManager(float minSpawnDist, float maxSpawnDist) : mMinSpawnDist(minSpawnDist), mMaxSpawnDist(maxSpawnDist) {
    // the gather distance has to cover the widest spawner in the world

    mMaxGatherDist = UMath::Max(maxSpawnDist, mMaxGatherDist);

    for (int i = 0; i < kMaxSpawnSegments; i++) {
        mSpawnSegment[i] = -1;
    }
}

AISpawnManager::~AISpawnManager() {
}

void AISpawnManager::GetBasePosition(UMath::Vector3 &basePos) {
    eView *view = eGetView(1, false);

    if (!view->IsActive()) {
        basePos = UMath::Vector3::kZero;
        return;
    }
    bVector3 cPos(*view->GetCamera()->GetPosition());
    bVector3 posV3;
    eUnSwizzleWorldVector(cPos, posV3);

    basePos = *reinterpret_cast<UMath::Vector3 *>(&posV3);
}

void AISpawnManager::GetBaseForwardVector(UMath::Vector3 &baseForwardVec) {
    eView *view = eGetView(1, false);
    if (!view->IsActive()) {
        baseForwardVec = UMath::Vector3::kZero;
        return;
    }
    bVector3 cPos(*view->GetCamera()->GetDirection());
    bVector3 posV3;
    eUnSwizzleWorldVector(cPos, posV3);

    baseForwardVec = *reinterpret_cast<UMath::Vector3 *>(&posV3);
}

bool AISpawnManager::RespawnAvailable(const UMath::Vector3 &position, float radius) {
    UMath::Vector3 basePos;

    GetBasePosition(basePos);

    float distance = UMath::Distancexz(position, basePos);

    if (distance <= mMinSpawnDist) {
        return false;
    }

    if (distance > (mMaxSpawnDist + mMinSpawnDist) * 0.5f) {
        return true;
    }

    if (distance > mMinSpawnDist) {
        if (!eViews[1].Active) {
            return false;
        }

        UMath::Vector3 baseFwd;

        GetBaseForwardVector(baseFwd);

        UMath::Vector3 toSpawn;

        UMath::Sub(position, basePos, toSpawn);
        VU0_v3unit(toSpawn, toSpawn);

        float facing = UMath::Dot(toSpawn, baseFwd);

        if (facing <= 0.8f && facing >= -0.707f) {
            return true;
        }
    }

    return false;
}

bool AISpawnManager::GetSpawnPointOnSegment(short &segInd, char &laneInd, float &timeStep) {
    const WRoadSegment &segment = *WRoadNetwork::Get().GetSegment(segInd);

    timeStep = Sim::GetRandom()._SimRandom_FloatRange(1.0f);

    static unsigned int lastSpawnLane = 0;

    int lane = lastSpawnLane % UMath::Max(WRoadNetwork::Get().GetSegmentNumTrafficLanes(segment), 1);
    lastSpawnLane++;

    laneInd = WRoadNetwork::Get().GetSegmentTrafficLaneInd(segment, lane);

    return true;
}

bool AISpawnManager::GetSpawnLocation(short &segInd, char &laneInd, float &timeStep) {
    RefreshSpawnData();

    if (mNumSpawnSegments <= 0) {
        return false;
    }

    segInd = mSpawnSegment[Sim::GetRandom()._SimRandom_IntRange(mNumSpawnSegments)];

    if (segInd < 0) {
        return false;
    }

    if (WRoadNetwork::Get().GetSegRoadInd(segInd) == -1) {
        return false;
    }

    if (!GetSpawnPointOnSegment(segInd, laneInd, timeStep)) {
        return false;
    }

    WRoadNav nav;

    nav.SetNavType(WRoadNav::kTypeTraffic);
    nav.SetLaneType(WRoadNav::kLaneTraffic);
    nav.InitAtSegment(segInd, laneInd, timeStep);

    UMath::Vector3 spawnPos = nav.GetPosition();

    if (!nav.IsValid()) {
        return false;
    }

    if (CheckSpawnPosition(spawnPos, true, laneInd, nav.GetNodeInd(), true)) {
        return true;
    } else {
        return false;
    }
}

bool AISpawnManager::CheckSpawnPosition(const UMath::Vector3 &checkPos, bool checkLane, int laneInd, int nodeInd, bool bCheckDist) {
    UMath::Vector3 basePos;

    GetBasePosition(basePos);

    float dist = UMath::Distancexz(checkPos, basePos);

    if (bCheckDist && (dist < mMinSpawnDist || dist > mMaxSpawnDist)) {
        return false;
    }

    bVector2 position2d(checkPos.z, -checkPos.x);

    TrackPathZone *zone = TheTrackPathManager.FindZone(&position2d, TRACK_PACH_ZONE_NO_COP_SPAWN, NULL);

    if (zone) {
        return false;
    }

    for (IVehicle *const *iter = UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::GetList(VEHICLE_ALL).begin();
         iter != UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::GetList(VEHICLE_ALL).end(); ++iter) {

        IVehicle *ivehicle = *iter;

        if (!ivehicle->IsActive()) {
            continue;
        }

        ISimable *isimable = ivehicle->GetSimable();
        IVehicleAI *ivehicleAI = ivehicle->GetAIVehiclePtr();

        if (!isimable || !ivehicleAI) {
            continue;
        }

        WRoadNav *otherNav = ivehicleAI->GetDriveToNav();

        if (otherNav) {
            float distance = UMath::DistanceSquare(checkPos, isimable->GetRigidBody()->GetPosition());

            if (checkLane && otherNav->GetNavType() == WRoadNav::kTypeTraffic) {
                const float sameLaneZoneDist = 15.0f * 15.0f;
                const float diffLaneZoneDist = 5.0f * 5.0f;

                if (ivehicle->IsDestroyed() ||
                    ((otherNav->GetLaneInd() == laneInd || otherNav->GetToLaneInd() == laneInd) && otherNav->GetNodeInd() == nodeInd)) {
                    if (distance < sameLaneZoneDist) {
                        return false;
                    }
                } else {
                    if (distance < diffLaneZoneDist) {
                        return false;
                    }
                }
            } else {
                const float otherCarZoneDist = 40.0f * 40.0f;

                if (distance < otherCarZoneDist) {
                    return false;
                }
            }
        }
    }

    return true;
}

void AISpawnManager::RefreshSpawnData() {
    static int currentSegmentIndex = 0;
    static int spawnSegmentIndex = 0;

    WRoadNetwork &roadNetwork = WRoadNetwork::Get();

    if (!roadNetwork.HasValidTrafficRoads()) {
        return;
    }

    roadNetwork.IncSegmentStamp();

    int nextSegmentIndex = UMath::Min(currentSegmentIndex + 20, static_cast<int>(roadNetwork.GetNumSegments()));

    float maxDistSpawn = mMaxGatherDist;

    UMath::Vector3 basePos;

    GetBasePosition(basePos);

    IPlayer *player = UTL::Collections::ListableSet<IPlayer, 8, ePlayerList, 3>::First(PLAYER_LOCAL);
    UMath::Vector3 linearVelocity =
        player && player->GetSimable() ? player->GetSimable()->GetRigidBody()->GetLinearVelocity() : UMath::Vector3::kZero;

    linearVelocity.y = 0.0f;

    UMath::Add(basePos, linearVelocity, basePos);

    for (int i = currentSegmentIndex; i < nextSegmentIndex; i++) {
        WRoadSegment *segment = roadNetwork.GetSegmentNonConst(i);

        if (segment->fFlags & kRoadSegmentNoTraffic) {
            continue;
        }

        if (segment->fFlags & kRoadSegmentDecision) {
            continue;
        }

        if (segment->GetLength() < 10.0f) {
            continue;
        }

        UMath::Vector3 node0 = roadNetwork.GetNode(segment->fNodeIndex[0])->fPosition;
        UMath::Vector3 node1 = roadNetwork.GetNode(segment->fNodeIndex[1])->fPosition;
        float node0Distance = UMath::Distance(basePos, node0);
        float node1Distance = UMath::Distance(basePos, node1);

        if (node0Distance > maxDistSpawn + 50.0f && node1Distance > maxDistSpawn + 50.0f) {
            continue;
        }

        mSpawnSegment[spawnSegmentIndex] = i;

        spawnSegmentIndex++;

        mNumSpawnSegments = UMath::Max(spawnSegmentIndex, mNumSpawnSegments);

        if (spawnSegmentIndex >= kMaxSpawnSegments)
            spawnSegmentIndex = 0;
    }

    if (nextSegmentIndex >= static_cast<int>(roadNetwork.GetNumSegments())) {
        currentSegmentIndex = 0;
    } else {
        currentSegmentIndex = nextSegmentIndex;
    }

    static int currentSpawnIndex = 0;

    int nextSpawnIndex = UMath::Min(currentSpawnIndex + 5, kMaxSpawnSegments);

    for (int i = currentSpawnIndex; i < nextSpawnIndex; i++) {
        if (mSpawnSegment[i] < 0) {
            continue;
        }

        const WRoadSegment *segment = roadNetwork.GetSegment(mSpawnSegment[i]);
        UMath::Vector3 node0 = roadNetwork.GetNode(segment->fNodeIndex[0])->fPosition;
        UMath::Vector3 node1 = roadNetwork.GetNode(segment->fNodeIndex[1])->fPosition;
        float node0Distance = UMath::Distance(basePos, node0);
        float node1Distance = UMath::Distance(basePos, node1);

        if (node0Distance > maxDistSpawn + 50.0f && node1Distance > maxDistSpawn + 50.0f) {
            mSpawnSegment[i] = -1;
        }
    }

    if (nextSpawnIndex >= kMaxSpawnSegments) {
        currentSpawnIndex = 0;
    } else {
        currentSpawnIndex = nextSpawnIndex;
    }
}
