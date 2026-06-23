#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Behaviors/DamageVehicle.h"
#include "Speed/Indep/Src/Physics/Behaviors/RigidBody.h"

// total size: 0xC0
class DamageCopCar : public DamageVehicle {
  public:
    typedef DamageVehicle Base;

  private:
    Seconds mFlippedOver; // offset 0xB8, size 0x4
    HSIMTASK mThinkTask;  // offset 0xBC, size 0x4

  public:
    static Behavior *Construct(const BehaviorParams &params);

    // ITaskable
    bool OnTask(HSIMTASK htask, float dT) override;

    // IDamageable
    void ResetDamage() override;

  protected:
    DamageCopCar(const BehaviorParams &bp, const DamageParams &dp);
    // IUnknown
    ~DamageCopCar() override;

    // DamageVehicle
    void OnImpact(const UMath::Vector3 &arm, const UMath::Vector3 &normal, float force, float speed, const SimSurface &mysurface,
                  ISimable *iother) override;

  private:
    void CheckUpright(float dT);
};

BIND_BEHAVIOR_FACTORY(DamageCopCar);

Behavior *DamageCopCar::Construct(const BehaviorParams &params) {
    const DamageParams dp(params.fparams.Fetch<DamageParams>(UCrc32(UCRC32_BASE)));
    return new DamageCopCar(params, dp);
}

DamageCopCar::DamageCopCar(const BehaviorParams &bp, const DamageParams &dp) : DamageVehicle(bp, dp) {
    this->mFlippedOver = 0.0f;
    this->mThinkTask = this->AddTask("Physics", 0.1f, 0.0f, Sim::TASK_FRAME_VARIABLE);
}

DamageCopCar::~DamageCopCar() {
    this->RemoveTask(this->mThinkTask);
}

void DamageCopCar::ResetDamage() {
    DamageVehicle::ResetDamage();
    this->mFlippedOver = 0.0f;
}

bool DamageCopCar::OnTask(HSIMTASK htask, float dT) {
    if (htask == this->mThinkTask) {
        if (!this->IsDestroyed() && !this->GetVehicle()->IsAnimating() && !INIS::Exists() && !this->IsPaused()) {
            this->CheckUpright(dT);
        }
        return true;
    }
    Object::OnTask(htask, dT);
    return false;
}

static const float Tweak_DamageCop_UprightDot = 0.5f;
static const float Tweak_DamageCop_GroundDot = 0.5f;
static const float Tweak_DamageCop_FlippedOverDeathTime = 6.0f;

void DamageCopCar::CheckUpright(float dT) {
    bool flippedover = false;
    ISuspension *suspension;
    ICollisionBody *collision_body;

    if (GetOwner()->QueryInterface(&suspension) && GetOwner()->QueryInterface(&collision_body) && collision_body->IsModeling()) {
        if (suspension->GetNumWheelsOnGround() != suspension->GetNumWheels()) {
            const UMath::Vector3 &vup = collision_body->GetUpVector();
            float ground_dot = UMath::Dot(vup, UMath::Vector4To3(collision_body->GetGroundNormal()));
            if ((ground_dot < Tweak_DamageCop_GroundDot) && (vup.y < Tweak_DamageCop_UprightDot)) {
                flippedover = true;
            }
        }
        this->mFlippedOver = !flippedover ? 0.0f : (this->mFlippedOver + dT);
        if (this->mFlippedOver > Tweak_DamageCop_FlippedOverDeathTime) {
            this->Destroy();
        }
        if (flippedover) {
            ICause *cause = ICause::FindInstance(GetOwner()->GetModel()->GetCausality());
            if (cause != nullptr) {
                if (ComparePtr(cause, IVehicle::First(VEHICLE_PLAYERS))) {
                    Destroy();
                }
            }
        }
    }
}

static const float Tweak_MinMassSmackableForCopDamage = 2000.0f;

void DamageCopCar::OnImpact(const UMath::Vector3 &arm, const UMath::Vector3 &normal, float force, float speed, const SimSurface &mysurface,
                            ISimable *iother) {
    if ((iother != nullptr) && iother->GetSimableType() == SIMABLE_SMACKABLE) {
        IDynamicsEntity *body;
        if (iother->QueryInterface(&body) && (body->IsImmobile() || body->GetMass() > Tweak_MinMassSmackableForCopDamage)) {
            DamageVehicle::OnImpact(arm, normal, force, speed, mysurface, iother);
        }
    } else {
        DamageVehicle::OnImpact(arm, normal, force, speed, mysurface, iother);
    }
}
