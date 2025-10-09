#include "DamageRacer.h"
#include "Speed/Indep/Src/Generated/Events/ETireBlown.hpp"
#include "Speed/Indep/Src/Generated/Events/ETirePunctured.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISpikeable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Physics/Behaviors/DamageVehicle.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

Behavior *DamageRacer::Construct(const BehaviorParams &params) {
    const DamageParams dp(params.fparams.Fetch<DamageParams>(UCrc32(0xa6b47fac)));
    return new DamageRacer(params, dp);
}

DamageRacer::DamageRacer(const BehaviorParams &bp, const DamageParams &dp) : DamageVehicle(bp, dp), ISpikeable(bp.fowner) {
    GetOwner()->QueryInterface(&mSuspension);
    bMemSet(mBlowOutTimes, 0, sizeof(mBlowOutTimes));
    bMemSet(mDamage, 0, sizeof(mDamage));
}

DamageRacer::~DamageRacer() {
    // TODO
}

eTireDamage DamageRacer::GetTireDamage(unsigned int wheelId) const {
    if (wheelId > 3) {
        return TIRE_DAMAGE_NONE;
    }
    return static_cast<eTireDamage>(mDamage[wheelId]);
}

unsigned int DamageRacer::GetNumBlowouts() const {
    unsigned int count = 0;
    for (unsigned int wheelId = 0; wheelId < 4; ++wheelId) {
        if (mDamage[wheelId] == TIRE_DAMAGE_BLOWN) {
            count++;
        }
    }
    return count;
}

void DamageRacer::Puncture(unsigned int wheelId) {
    if (wheelId < 4 && mDamage[wheelId] == TIRE_DAMAGE_NONE) {
        mDamage[wheelId] = TIRE_DAMAGE_PUNCTURED;

        if (Physics::Info::HasRunflatTires(GetVehicle()->GetVehicleAttributes())) {
            mBlowOutTimes[wheelId] = 180.0f;
        } else {
            mBlowOutTimes[wheelId] = 0.5f;
        }

        new ETirePunctured(GetOwner()->GetInstanceHandle(), wheelId);
    }
}

bool DamageRacer::IsLightDamaged(VehicleFX::ID idx) const {
    if (!CanDamageVisuals()) {
        return false;
    }
    if (IsDestroyed()) {
        return true;
    }
    return DamageVehicle::IsLightDamaged(idx);
}

DamageZone::Info DamageRacer::GetZoneDamage() const {
    if (!CanDamageVisuals()) {
        return DamageZone::Info();
    }
    return DamageVehicle::GetZoneDamage();
}

bool DamageRacer::CanDamageVisuals() const {
    // TODO
}

void DamageRacer::OnTaskSimulate(float dT) {
    if (mSuspension) {
        for (unsigned int wheelId = 0; wheelId < 4; ++wheelId) {
            if ((mDamage[wheelId] == TIRE_DAMAGE_PUNCTURED) && (mBlowOutTimes[wheelId] > 0.0f) && ((mBlowOutTimes[wheelId] -= dT) < 0.0f)) {
                mDamage[wheelId] = TIRE_DAMAGE_BLOWN;
                mBlowOutTimes[wheelId] = 0.0f;
                new ETireBlown(GetOwner()->GetInstanceHandle(), wheelId);
            }
        }
    }
    Behavior::OnTaskSimulate(dT);
}
