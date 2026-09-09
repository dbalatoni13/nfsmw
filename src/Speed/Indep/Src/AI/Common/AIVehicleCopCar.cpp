#include "Speed/Indep/Src/AI/AIVehicleCopCar.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ICopMgr.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

static const bool bCopSimplePhysics = false; // Decl: 28

AIVehicleCopCar::AIVehicleCopCar(const BehaviorParams &bp)
    : AIVehiclePursuit(bp),  //
      mLOSAngleFront(-0.3f), //
      mPerpHiddenFromMe(false) {}

AIVehicleCopCar::~AIVehicleCopCar() {}

Behavior *AIVehicleCopCar::Construct(const BehaviorParams &bp) {
    return new AIVehicleCopCar(bp);
}

BIND_BEHAVIOR_FACTORY(AIVehicleCopCar);

void AIVehicleCopCar::Update(float dT) {
    ProfileNode profile_node("TODO", 0);

    bool have_simple_physics = this->IsSimplePhysicsActive();
    bool want_simple_physics;

    if (this->GetVehicle()->IsOffWorld()) {
        if (!have_simple_physics) {
            this->EnableSimplePhysics();
        }
    } else if (have_simple_physics) {
        UMath::Vector3 forwardVector;
        this->GetSimable()->GetRigidBody()->GetForwardVector(forwardVector);
        UMath::Vector3 position = this->GetSimable()->GetRigidBody()->GetPosition();
        position.y += 1.5f;
        if (!this->GetVehicle()->SetVehicleOnGround(position, forwardVector)) {
            this->GetVehicle()->SetVehicleOnGround(position, forwardVector);
        }
        this->DisableSimplePhysics();
    }

    this->AIVehiclePursuit::Update(dT);
    this->UpdateSpawnTimer(dT);
    this->UpdateReverseOverride(dT);
    this->UpdateTargeting();
    this->UpdateRoadNavInfo();
    if (this->GetGoal() != nullptr) {
        this->GetGoal()->Update(dT);
    }
    this->WatchForPerps();
}

bool AIVehicleCopCar::IsTetheredToTarget(UTL::COM::IUnknown *object) {
    if (!this->GetTarget()->IsValid()) {
        return false;
    }

    if (this->GetTarget()->IsTarget(object)) {
        return false;
    }

    if (!this->GetInPursuit()) {
        return false;
    }

    if (this->GetSimable()->GetRigidBody()->GetSpeedXZ() < MPH2MPS(50.0f)) {
        return false;
    }

    if (this->GetTarget()->GetDistTo() > 50.0f) {
        return false;
    }

    UMath::Vector3 forwardVector;
    this->GetSimable()->GetRigidBody()->GetForwardVector(forwardVector);

    float headingToTarget = UMath::Dot(this->GetTarget()->GetDirTo(), forwardVector);
    return headingToTarget < -0.2f;
}

static const float kCopFrontLOSDistance = 150.0f; // Decl: 141
static const float kCopRearLOSDistance = 50.0f;   // Decl: 142

bool AIVehicleCopCar::CanSeeTarget(AITarget *target) {
    bool isperphidden = false;
    IPerpetrator *iperp;
    target->QueryInterface(&iperp);

    if (iperp != nullptr && iperp->IsHiddenFromCars()) {
        isperphidden = true;
    }

    if (isperphidden && this->mPerpHiddenFromMe) {
        return false;
    }

    this->mPerpHiddenFromMe = false;

    IPursuit *ipursuit = this->GetPursuit();

    float frontDist = -1.0f;
    float rearDist = frontDist;

    if (ipursuit != nullptr && iperp != nullptr) {
        Attrib::Gen::pursuitlevels *pursuitLevels = iperp->GetPursuitLevelAttrib();
        if (pursuitLevels != nullptr) {
            frontDist = pursuitLevels->frontLOSdistance();
            rearDist = pursuitLevels->rearLOSdistance();
        }
    }

    if (frontDist < 0.0f) {
        frontDist = kCopFrontLOSDistance;
        rearDist = kCopRearLOSDistance;
    }

    UMath::Vector3 targetPosition = target->GetPosition();

    UMath::Vector3 forwardVector;
    this->GetOwner()->GetRigidBody()->GetForwardVector(forwardVector);

    UMath::Vector3 position = this->GetOwner()->GetRigidBody()->GetPosition();

    UMath::Vector3 dirToTarget;
    UMath::Sub(targetPosition, position, dirToTarget);
    UMath::Unit(dirToTarget, dirToTarget);

    float distanceToTarget = UMath::Distance(position, targetPosition);

    bool isoutofsight = false;
    isoutofsight = isoutofsight || (distanceToTarget >= frontDist ||
                                    (distanceToTarget >= rearDist && UMath::Dot(forwardVector, dirToTarget) <= this->mLOSAngleFront));

    if (ipursuit != nullptr) {
        isoutofsight = isoutofsight || !ipursuit->PursuitMeterCanShowBusted();
    }

    if (!isoutofsight) {
        UMath::Vector4 posToDest[2];

        posToDest[0] = UMath::Vector4Make(position, 1.0f);
        posToDest[0].y += 0.5f;

        posToDest[1] = UMath::Vector4Make(targetPosition, 1.0f);
        posToDest[1].y += 0.5f;

        WCollisionMgr::WorldCollisionInfo cInfo;
        if (WCollisionMgr(0, 3).CheckHitWorld(posToDest, cInfo, 3) != 0) {
            isoutofsight = true;
        }
    }

    if (isoutofsight) {
        if (isperphidden) {
            this->mPerpHiddenFromMe = true;
        }
        return false;
    }

    return true;
}

void AIVehicleCopCar::WatchForPerps() {
    if (this->GetInPursuit()) {
        return;
    }
    for (IVehicle::List::const_iterator iter = IVehicle::GetList(VEHICLE_PLAYERS).begin(); iter != IVehicle::GetList(VEHICLE_PLAYERS).end(); ++iter) {
        IVehicle *itargetVehicle = *iter;
        if (this->CheckForPursuit(itargetVehicle)) {
            return;
        }
    }
    if (ICopMgr::Exists() && ICopMgr::Get()->CanPursueRacers()) {
        for (IVehicle::List::const_iterator iter = IVehicle::GetList(VEHICLE_RACERS).begin(); iter != IVehicle::GetList(VEHICLE_RACERS).end();
             ++iter) {
            IVehicle *itargetVehicle = *iter;
            DriverClass driverclass = itargetVehicle->GetDriverClass();
            if (driverclass != DRIVER_HUMAN && driverclass != DRIVER_REMOTE && this->CheckForPursuit(itargetVehicle)) {
                return;
            }
        }
    }
}

bool AIVehicleCopCar::CheckForPursuit(IVehicle *itargetVehicle) {
    IPerpetrator *iperp;
    ISimable *itargetSimable = itargetVehicle->GetSimable();
    if (!itargetVehicle->QueryInterface(&iperp)) {
        return false;
    }
    AITarget target(this->GetSimable());
    target.Aquire(itargetSimable);
    if (!this->CanSeeTarget(&target)) {
        return false;
    }

    IVehicleAI *ivehicleai;
    itargetVehicle->QueryInterface(&ivehicleai);

    float lasttraffichittime = iperp->GetLastTrafficHitTime();
    bool hittraffic = false;
    if (lasttraffichittime > 0.0f) {
        hittraffic = (Sim::GetTime() - lasttraffichittime) < 1.0f;
    }
    bool active911 = iperp->Get911CallTime() > 0.0f;

    bool alreadypursuit = false;
    if (ivehicleai != nullptr && ivehicleai->GetPursuit() != nullptr) {
        alreadypursuit = true;
    }

    int heat = (int)iperp->GetHeat();
    // TODO uses the other MPH2MPS inline that uses a slightly different constant....
    if (!alreadypursuit && !active911 && !hittraffic && heat <= 3 && itargetVehicle->GetSpeed() < MPH2MPS(65.0f)) {
        return false;
    }

    this->GetTarget()->Aquire(itargetSimable);
    return true;
}
