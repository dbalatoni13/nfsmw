#include "Speed/Indep/Src/AI/AIVehicleCopCar.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pursuitlevels.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ICopMgr.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

// El ELF carga aqui 1/2.237f y lo multiplica por 65.0f SIN plegar las dos
// constantes: es una inline, no una expresion constante. MPH2MPS de
// ConversionUtil.hpp da 0.44703001f y es el bueno para las otras 23
// llamadas del juego, asi que este sitio lleva su propia conversion.
static inline float CopCarMPH2MPS(float mph) {
    return mph / 2.237f;
}


AIVehicleCopCar::AIVehicleCopCar(const BehaviorParams &bp)
    : AIVehiclePursuit(bp),  //
      mLOSAngleFront(-0.3f), //
      mPerpHiddenFromMe(false) {}

AIVehicleCopCar::~AIVehicleCopCar() {}

Behavior *AIVehicleCopCar::Construct(const BehaviorParams &bp) {
    return new AIVehicleCopCar(bp);
}

BIND_BEHAVIOR_FACTORY(AIVehicleCopCar)


void AIVehicleCopCar::Update(float dT) {
    ProfileNode profile_node;

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
    IPerpetrator *iperp;
    bool hidden = false;
    target->QueryInterface(&iperp);
    if (iperp && iperp->IsHiddenFromCars()) {
        hidden = true;
    }
    if (hidden && mPerpHiddenFromMe) {
        return false;
    }

    mPerpHiddenFromMe = false;
    IPursuit *pursuit = GetPursuit();
    float frontLOS = -1.0f;
    float rearLOS = -1.0f;
    if (pursuit && iperp) {
        Attrib::Gen::pursuitlevels *pl = iperp->GetPursuitLevelAttrib();
        if (pl) {
            frontLOS = pl->frontLOSdistance();
            rearLOS = pl->rearLOSdistance();
        }
    }
    if (frontLOS < 0.0f) {
        frontLOS = 150.0f;
        rearLOS = 50.0f;
    }

    UMath::Vector3 targetPos = target->GetPosition();
    UMath::Vector3 forward;
    GetOwner()->GetRigidBody()->GetForwardVector(forward);
    UMath::Vector3 myPos = GetOwner()->GetRigidBody()->GetPosition();

    UMath::Vector3 dirTo;
    UMath::Sub(targetPos, myPos, dirTo);
    UMath::Unit(dirTo, dirTo);
    float dist = UMath::Distance(myPos, targetPos);

    bool blocked = dist >= frontLOS || (dist >= rearLOS && UMath::Dot(forward, dirTo) <= mLOSAngleFront);
    bool outOfSight = blocked;

    if (pursuit) {
        outOfSight = outOfSight || !pursuit->PursuitMeterCanShowBusted();
    }

    if (!outOfSight) {
        UMath::Vector4 segs[2];
        segs[0] = UMath::Vector4Make(myPos, 1.0f);
        segs[0].y += 0.5f;
        segs[1] = UMath::Vector4Make(targetPos, 1.0f);
        segs[1].y += 0.5f;
        WCollisionMgr::WorldCollisionInfo cinfo;
        if (WCollisionMgr(0, 3).CheckHitWorld(segs, cinfo, 3)) {
            outOfSight = true;
        }
    }

    if (outOfSight) {
        if (hidden) {
            mPerpHiddenFromMe = true;
        }
        return false;
    }
    return true;
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
    if (!alreadypursuit && !active911 && !hittraffic && heat <= 3 && itargetVehicle->GetSpeed() < CopCarMPH2MPS(65.0f)) {
        return false;
    }

    GetTarget()->Aquire(itargetSimable);
    return true;
}
