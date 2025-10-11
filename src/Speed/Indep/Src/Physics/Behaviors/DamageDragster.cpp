#include "DamageDragster.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Physics/Behaviors/DamageVehicle.h"
#include "Speed/Indep/Src/Sim/Collision.h"

Behavior *DamageDragster::Construct(const BehaviorParams &params) {
    const DamageParams dp(params.fparams.Fetch<DamageParams>(UCrc32(0xa6b47fac)));
    return new DamageDragster(params, dp);
}

DamageDragster::DamageDragster(const BehaviorParams &bp, const DamageParams &dp) : DamageRacer(bp, dp) {}

void DamageDragster::CheckTotaling(const COLLISION_INFO &cinfo) {
    if ((cinfo.type == Sim::Collision::Info::GROUND) || IsDestroyed()) {
        return;
    }
    IVehicle *vehicle = GetVehicle();
    if (vehicle->IsAnimating()) {
        return;
    }
    float old_speed = 0.0f;
    HSIMABLE my_handle = GetOwner()->GetInstanceHandle();
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
    if ((old_speed > 0.0f) && (old_speed > MPH2MPS(100.0f))) {
        float speed_change = old_speed - GetOwner()->GetRigidBody()->GetSpeed();
        if (speed_change > MPH2MPS(40.0f)) {
            Destroy();
        }
    }
}

void DamageDragster::OnCollision(const COLLISION_INFO &cinfo) {
    DamageVehicle::OnCollision(cinfo);
    CheckTotaling(cinfo);
}
