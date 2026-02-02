#include "Speed/Indep/Src/AI/AIVehicleCopCar.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

AIVehicleCopCar::AIVehicleCopCar(const BehaviorParams &bp)
    : AIVehiclePursuit(bp),  //
      mLOSAngleFront(-0.3f), //
      mPerpHiddenFromMe(false) {}

AIVehicleCopCar::~AIVehicleCopCar() {}

Behavior *AIVehicleCopCar::Construct(const BehaviorParams &bp) {
    return new AIVehicleCopCar(bp);
}

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
