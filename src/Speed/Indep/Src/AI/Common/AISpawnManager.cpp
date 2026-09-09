#include "Speed/Indep/Src/AI/AISpawnManager.h"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Math/SimRandom.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/WRoadElem.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"

float AISpawnManager::mMaxGatherDist = 0.0f; // Decl: 23

int AISpawnManager::mSpawnSegment[50];     // Decl: 25
int AISpawnManager::mNumSpawnSegments = 0; // Decl: 26

AISpawnManager::AISpawnManager(float minSpawnDist, float maxSpawnDist)
    : mMinSpawnDist(minSpawnDist), //
      mMaxSpawnDist(maxSpawnDist) {
    this->mMaxGatherDist = UMath::Max(maxSpawnDist, this->mMaxGatherDist);
    for (int i = 0; i < kMaxSpawnSegments; i++) {
        this->mSpawnSegment[i] = -1;
    }
}

AISpawnManager::~AISpawnManager() {}

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
    this->GetBasePosition(basePos);
    float distToBase = UMath::Distancexz(basePos, position);
    if (distToBase > this->mMinSpawnDist) {
        if (distToBase > (this->mMaxSpawnDist + this->mMinSpawnDist) * 0.5f) {
            return true;
        }
        if (distToBase > mMinSpawnDist) {
            eView *view = eGetView(1, false);
            if (view->IsActive()) {
                UMath::Vector3 baseForwardVec;
                this->GetBaseForwardVector(baseForwardVec);

                UMath::Vector3 baseToPos;
                UMath::Sub(position, basePos, baseToPos);
                UMath::Unit(baseToPos, baseToPos);
                float dot = UMath::Dot(baseToPos, baseForwardVec);
                if (dot <= 0.8f && dot >= -0.707f) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool AISpawnManager::GetSpawnPointOnSegment(short &segInd, char &laneInd, float &timeStep) {
    const WRoadSegment *segment = WRoadNetwork::Get().GetSegment(segInd);
    static unsigned int lastSpawnLane = 0;

    timeStep = Sim::GetRandom().SimRandom_Float();

    int lane = UMath::Max(WRoadNetwork::Get().GetSegmentNumTrafficLanes(*segment), 1);

    lastSpawnLane++;
    laneInd = WRoadNetwork::Get().GetSegmentTrafficLaneInd(*segment, (lastSpawnLane - 1) % lane);

    return true;
}

bool AISpawnManager::GetSpawnLocation(short &segInd, char &laneInd, float &timeStep) {
    int roadInd;

    this->RefreshSpawnData();

    if (mNumSpawnSegments <= 0) {
        return false;
    }

    roadInd = Sim::GetRandom().SimRandom_IntRange(mNumSpawnSegments);
    segInd = mSpawnSegment[roadInd];

    if (segInd < 0) {
        return false;
    }

    if (WRoadNetwork::Get().GetSegRoadInd(segInd) == -1) {
        return false;
    }

    if (!this->GetSpawnPointOnSegment(segInd, laneInd, timeStep)) {
        return false;
    }

    WRoadNav testNav;
    testNav.SetNavType(WRoadNav::kTypeTraffic);
    testNav.SetLaneType(static_cast<WRoadNav::ELaneType>(1));
    testNav.InitAtSegment(segInd, laneInd, timeStep);

    UMath::Vector3 checkPos = testNav.GetPosition();

    if (!testNav.IsValid()) {
        return false;
    }
    if (this->CheckSpawnPosition(checkPos, true, laneInd, testNav.GetNodeInd(), true)) {
        return true;
    }

    return false;
}

bool AISpawnManager::CheckSpawnPosition(const UMath::Vector3 &checkPos, bool checkLane, int laneInd, int nodeInd, bool bCheckDist) {
    UMath::Vector3 basePos;
    this->GetBasePosition(basePos);

    float dist = UMath::Distancexz(basePos, checkPos);
    if (bCheckDist && (dist < this->mMinSpawnDist || dist > this->mMaxSpawnDist)) {
        return false;
    }

    bVector2 position2d(checkPos.z, -checkPos.x);
    TrackPathZone *zone = TheTrackPathManager.FindZone(&position2d, TRACK_PACH_ZONE_NO_COP_SPAWN, nullptr);
    if (zone != nullptr) {
        return false;
    }

    for (IVehicle::List::const_iterator iter = IVehicle::GetList(VEHICLE_ALL).begin(); iter != IVehicle::GetList(VEHICLE_ALL).end(); ++iter) {
        IVehicle *ivehicle = *iter;

        if (!ivehicle->IsActive()) {
            continue;
        }

        ISimable *isimable = ivehicle->GetSimable();
        IVehicleAI *ivehicleAI = ivehicle->GetAIVehiclePtr();

        if (isimable == nullptr || ivehicleAI == nullptr) {
            continue;
        }

        WRoadNav *otherNav = ivehicleAI->GetDriveToNav();
        if (otherNav != nullptr) {
            float distance = UMath::DistanceSquare(checkPos, isimable->GetRigidBody()->GetPosition());

            if (checkLane && otherNav->GetNavType() == WRoadNav::kTypeTraffic) {
                const float sameLaneZoneDist = 225.0f;
                const float diffLaneZoneDist = 25.0f;

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
                const float otherCarZoneDist = 1600.0f;

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
    float maxDistSpawn = this->mMaxGatherDist;

    UMath::Vector3 basePos;
    this->GetBasePosition(basePos);

    IPlayer *player = IPlayer::First(PLAYER_LOCAL);
    UMath::Vector3 linearVelocity =
        player != nullptr && player->GetSimable() != nullptr ? player->GetSimable()->GetRigidBody()->GetLinearVelocity() : UMath::Vector3::kZero;

    linearVelocity.y = 0.0f;

    static int currentSpawnIndex = 0;
    int nextSpawnIndex;

    UMath::Add(basePos, linearVelocity, basePos);

    for (int i = currentSegmentIndex; i < nextSegmentIndex; i++) {
        WRoadSegment *segment = const_cast<WRoadSegment *>(roadNetwork.GetSegment(i));

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

        if (node0Distance <= maxDistSpawn + 50.0f || node1Distance <= maxDistSpawn + 50.0f) {
            this->mSpawnSegment[spawnSegmentIndex] = i;

            spawnSegmentIndex = spawnSegmentIndex + 1;
            this->mNumSpawnSegments = UMath::Max(spawnSegmentIndex, this->mNumSpawnSegments);

            if (spawnSegmentIndex > 49) {
                spawnSegmentIndex = 0;
            }
        }
    }

    if (nextSegmentIndex >= static_cast<int>(roadNetwork.GetNumSegments())) {
        currentSegmentIndex = 0;
    } else {
        currentSegmentIndex = nextSegmentIndex;
    }

    nextSpawnIndex = UMath::Min(currentSpawnIndex + 5, 50);

    for (int i = currentSpawnIndex; i < nextSpawnIndex; i++) {
        if (this->mSpawnSegment[i] >= 0) {
            const WRoadSegment *segment = roadNetwork.GetSegment(this->mSpawnSegment[i]);

            UMath::Vector3 node0 = roadNetwork.GetNode(segment->fNodeIndex[0])->fPosition;
            UMath::Vector3 node1 = roadNetwork.GetNode(segment->fNodeIndex[1])->fPosition;

            float node0Distance = UMath::Distance(basePos, node0);
            float node1Distance = UMath::Distance(basePos, node1);

            if (node0Distance > maxDistSpawn + 50.0f && node1Distance > maxDistSpawn + 50.0f) {
                this->mSpawnSegment[i] = -1;
            }
        }
    }

    if (nextSpawnIndex > 49) {
        currentSpawnIndex = 0;
    } else {
        currentSpawnIndex = nextSpawnIndex;
    }
}
