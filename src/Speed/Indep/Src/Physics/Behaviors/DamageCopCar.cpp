#include "DamageCopCar.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Physics/Behaviors/DamageVehicle.h"
#include "Speed/Indep/Src/Physics/Dynamics.h"

Behavior *DamageCopCar::Construct(const BehaviorParams &params) {
    const DamageParams dp(params.fparams.Fetch<DamageParams>(UCrc32(0xa6b47fac)));
    return new DamageCopCar(params, dp);
}

DamageCopCar::DamageCopCar(const BehaviorParams &bp, const DamageParams &dp) : DamageVehicle(bp, dp) {
    mFlippedOver = 0.0f;
    mThinkTask = AddTask("Physics", 0.1f, 0.0f, Sim::TASK_FRAME_VARIABLE);
}

DamageCopCar::~DamageCopCar() {
    RemoveTask(mThinkTask);
}

void DamageCopCar::ResetDamage() {
    DamageVehicle::ResetDamage();
    mFlippedOver = 0.0f;
}

bool DamageCopCar::OnTask(HSIMTASK htask, float dT) {
    if (htask == mThinkTask) {
        if (!IsDestroyed() && !GetVehicle()->IsAnimating() && !INIS::Exists() && !IsPaused()) {
            CheckUpright(dT);
        }
        return true;
    }
    Object::OnTask(htask, dT);
    return false;
}

void DamageCopCar::CheckUpright(float dT) {
    bool flippedover = false;
    ISuspension *suspension;
    ICollisionBody *collision_body;

    if (GetOwner()->QueryInterface(&suspension) && GetOwner()->QueryInterface(&collision_body) && collision_body->IsModeling()) {
        if (suspension->GetNumWheelsOnGround() != suspension->GetNumWheels()) {
            const UMath::Vector3 &vup = collision_body->GetUpVector();
            float ground_dot = UMath::Dot(vup, UMath::Vector4To3(collision_body->GetGroundNormal()));
            if ((ground_dot < 0.5f) && (vup.y < 0.5f)) {
                flippedover = true;
            }
        }
        mFlippedOver = !flippedover ? 0.0f : (mFlippedOver + dT);
        if (mFlippedOver > 6.0f) {
            Destroy();
        }
        if (flippedover) {
            ICause *cause = ICause::FindInstance(GetOwner()->GetModel()->GetCausality());
            if (cause) {
                if (ComparePtr(cause, IVehicle::First(VEHICLE_PLAYERS))) {
                    Destroy();
                }
            }
        }
    }
}

void DamageCopCar::OnImpact(const UMath::Vector3 &arm, const UMath::Vector3 &normal, float force, float speed, const SimSurface &mysurface,
                            ISimable *iother) {
    if (iother && iother->GetSimableType() == SIMABLE_SMACKABLE) {
        IDynamicsEntity *body;
        if (iother->QueryInterface(&body) && (body->IsImmobile() || body->GetMass() > 2000.0f)) {
            DamageVehicle::OnImpact(arm, normal, force, speed, mysurface, iother);
        }
    } else {
        DamageVehicle::OnImpact(arm, normal, force, speed, mysurface, iother);
    }
}
