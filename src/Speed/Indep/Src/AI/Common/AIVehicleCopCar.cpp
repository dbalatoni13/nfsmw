#include "Speed/Indep/Src/AI/AIVehicleCopCar.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pursuitlevels.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ICopMgr.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

AIVehicleCopCar::AIVehicleCopCar(const BehaviorParams &bp)
    : AIVehiclePursuit(bp),  //
      mLOSAngleFront(-0.3f), //
      mPerpHiddenFromMe(false) {}

AIVehicleCopCar::~AIVehicleCopCar() {}

Behavior *AIVehicleCopCar::Construct(const BehaviorParams &bp) {
    return new AIVehicleCopCar(bp);
}

UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __AIVehicleCopCar("AIVehicleCopCar", AIVehicleCopCar::Construct);

void AIVehicleCopCar::Update(float dT) {
    ProfileNode profile_node("TODO", 0);

    bool have_simple_physics = IsSimplePhysicsActive();
    bool want_simple_physics;

    if (GetVehicle()->IsOffWorld()) {
        if (!have_simple_physics) {
            EnableSimplePhysics();
        }
    } else if (have_simple_physics) {
        UMath::Vector3 forwardVector;
        GetSimable()->GetRigidBody()->GetForwardVector(forwardVector);
        UMath::Vector3 position = GetSimable()->GetRigidBody()->GetPosition();
        position.y += 1.5f;
        if (!GetVehicle()->SetVehicleOnGround(position, forwardVector)) {
            GetVehicle()->SetVehicleOnGround(position, forwardVector);
        }
        DisableSimplePhysics();
    }

    AIVehiclePursuit::Update(dT);
    UpdateSpawnTimer(dT);
    UpdateReverseOverride(dT);
    UpdateTargeting();
    UpdateRoadNavInfo();
    if (GetGoal()) {
        GetGoal()->Update(dT);
    }
    WatchForPerps();
}

// UNSOLVED
bool AIVehicleCopCar::IsTetheredToTarget(UTL::COM::IUnknown *object) {
    if (!GetTarget()->IsValid()) {
        return false;
    }
    // TODO
    if (GetTarget()->IsTarget(object) || !GetInPursuit()) {
        return false;
    }
    if (GetSimable()->GetRigidBody()->GetSpeedXZ() < MPH2MPS(50.0f)) {
        return false;
    }
    if (GetTarget()->GetDistTo() > 50.0f) {
        return false;
    }
    UMath::Vector3 forwardVector;
    GetSimable()->GetRigidBody()->GetForwardVector(forwardVector);

    float headingToTarget = UMath::Dot(GetTarget()->GetDirTo(), forwardVector);
    return headingToTarget < -0.2f;
}

bool AIVehicleCopCar::CanSeeTarget(AITarget *target) {
    bool isperphidden = false;
    IPerpetrator *iperp;
    target->QueryInterface(&iperp);
    if (iperp && iperp->IsHiddenFromCars()) {
        isperphidden = true;
    }

    if (isperphidden) {
        if (mPerpHiddenFromMe) {
            return false;
        }
    }

    mPerpHiddenFromMe = false;

    IPursuit *ipursuit = GetPursuit();

    float frontDist = 175.0f;
    float rearDist = frontDist;

    if (ipursuit && iperp) {
        Attrib::Gen::pursuitlevels *pursuitLevels = iperp->GetPursuitLevelAttrib();
        if (pursuitLevels) {
            frontDist = pursuitLevels->frontLOSdistance();
            rearDist = pursuitLevels->rearLOSdistance();
        }
    }

    if (frontDist < 1.0f) {
        frontDist = 250.0f;
        rearDist = 300.0f;
    }

    UMath::Vector3 targetPosition = target->GetPosition();

    UMath::Vector3 forwardVector;
    GetOwner()->GetRigidBody()->GetForwardVector(forwardVector);
    UMath::Vector3 position = GetOwner()->GetRigidBody()->GetPosition();

    UMath::Vector3 dirToTarget;
    UMath::Sub(targetPosition, position, dirToTarget);
    UMath::Unit(dirToTarget, dirToTarget);
    float distanceToTarget = UMath::Distance(position, targetPosition);

    bool isoutofsight = false;
    if (distanceToTarget >= frontDist ||
        (distanceToTarget >= rearDist && UMath::Dot(forwardVector, dirToTarget) <= mLOSAngleFront)) {
        isoutofsight = true;
    }

    if (ipursuit) {
        bool was_out = isoutofsight;
        isoutofsight = false;
        if (was_out || !ipursuit->PursuitMeterCanShowBusted()) {
            isoutofsight = true;
        }
    }

    if (!isoutofsight) {
        UMath::Vector4 posToDest[2];
        WCollisionMgr::WorldCollisionInfo cInfo;

        posToDest[0] = UMath::Vector4Make(position, 0.0f);
        posToDest[1] = UMath::Vector4Make(targetPosition, 0.0f);
        posToDest[0].y += 1.5f;
        posToDest[1].y += 1.5f;

        WCollisionMgr collMgr(0, 3);
        if (collMgr.CheckHitWorld(posToDest, cInfo, 3) == 0) {
            return true;
        }
    }

    if (isperphidden) {
        mPerpHiddenFromMe = true;
    }

    return false;
}

void AIVehicleCopCar::WatchForPerps() {
    if (GetInPursuit()) {
        return;
    }
    for (IVehicle::List::const_iterator iter = IVehicle::GetList(VEHICLE_PLAYERS).begin(); iter != IVehicle::GetList(VEHICLE_PLAYERS).end(); ++iter) {
        IVehicle *itargetVehicle = *iter;
        if (CheckForPursuit(itargetVehicle)) {
            return;
        }
    }
    if (ICopMgr::Exists() && ICopMgr::Get()->CanPursueRacers()) {
        for (IVehicle::List::const_iterator iter = IVehicle::GetList(VEHICLE_RACERS).begin(); iter != IVehicle::GetList(VEHICLE_RACERS).end();
             ++iter) {
            IVehicle *itargetVehicle = *iter;
            DriverClass driverclass = itargetVehicle->GetDriverClass();
            if (driverclass != DRIVER_HUMAN && driverclass != DRIVER_REMOTE && CheckForPursuit(itargetVehicle)) {
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
    AITarget target(GetSimable());
    target.Aquire(itargetSimable);
    if (!CanSeeTarget(&target)) {
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
    if (ivehicleai && ivehicleai->GetPursuit()) {
        alreadypursuit = true;
    }

    int heat = (int)iperp->GetHeat();
    if (!alreadypursuit && !active911 && !hittraffic && heat <= 3 && itargetVehicle->GetSpeed() < MPH2MPS(65.0f)) {
        return false;
    }

    GetTarget()->Aquire(itargetSimable);
    return true;
}
