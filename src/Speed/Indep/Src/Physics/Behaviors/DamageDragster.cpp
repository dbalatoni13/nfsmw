#include "DamageDragster.h"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Physics/Behaviors/DamageVehicle.h"
#include "Speed/Indep/Src/Sim/Collision.h"

BIND_BEHAVIOR_FACTORY(DamageDragster);

static const float Tweak_DragKillSpeed = 100.0f;
static const float Tweak_DragKillSpeedChange = 40.0f;
static const bool Tweak_DragDisableKill = false;

Behavior *DamageDragster::Construct(const BehaviorParams &params) {
    const DamageParams dp(params.fparams.Fetch<DamageParams>(UCrc32(UCRC32_BASE)));
    return new DamageDragster(params, dp);
}

DamageDragster::DamageDragster(const BehaviorParams &bp, const DamageParams &dp) : DamageRacer(bp, dp) {}

void DamageDragster::CheckTotaling(const COLLISION_INFO &cinfo) {
    if ((cinfo.type == Sim::Collision::Info::GROUND) || this->IsDestroyed()) {
        return;
    }
    IVehicle *vehicle = this->GetVehicle();
    if (vehicle->IsAnimating()) {
        return;
    }
    float old_speed = 0.0f;
    HSIMABLE my_handle = this->GetOwner()->GetInstanceHandle();
    if (cinfo.objA == my_handle) {
        if (cinfo.objAImmobile) {
            return;
        }
        old_speed = UMath::Length(cinfo.objAVel);
    } else if (cinfo.objB == my_handle) {
        if (cinfo.objBImmobile) {
            return;
        }
        old_speed = UMath::Length(cinfo.objBVel);
    }
    if (!Tweak_DragDisableKill && (old_speed > 0.0f) && (old_speed > MPH2MPS(Tweak_DragKillSpeed))) {
        float speed_change = old_speed - this->GetOwner()->GetRigidBody()->GetSpeed();
        if (speed_change > MPH2MPS(Tweak_DragKillSpeedChange)) {
            this->Destroy();
        }
    }
}

void DamageDragster::OnCollision(const COLLISION_INFO &cinfo) {
    DamageVehicle::OnCollision(cinfo);
    this->CheckTotaling(cinfo);
}
