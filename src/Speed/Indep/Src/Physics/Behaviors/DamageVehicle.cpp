#include "DamageVehicle.h"
#include "Speed/Indep/Src/Generated/Events/EVehicleDestroyed.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/World/Damagezones.h"

Behavior *DamageVehicle::Construct(const BehaviorParams &params) {
    const DamageParams dp(params.fparams.Fetch<DamageParams>(UCrc32(0xa6b47fac)));
    return new DamageVehicle(params, dp);
}

void DamageVehicle::ResetParts() {
    IRenderable *irenderable;
    if (GetOwner()->QueryInterface(&irenderable)) {
        IModel *instance = irenderable->GetModel();
        if (instance) {
            instance->ReleaseChildModels();
        }
    }
}

void DamageVehicle::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        GetOwner()->QueryInterface(&mIRBComplex);
        GetOwner()->QueryInterface(&mRB);
    }
    if (mechanic == BEHAVIOR_MECHANIC_DRAW) {
        GetOwner()->QueryInterface(&mRenderable);
    }
}

void DamageVehicle::OnCollision(const COLLISION_INFO &cinfo) {}

bool DamageVehicle::SetDynamicData(const EventSequencer::System *system, EventDynamicData *data) {
    data->fVelocity = UMath::Vector4Make(mLastImpactSpeed, 1.0f);
    return true;
}

const DamageScaleRecord &DamageVehicle::GetDamageRecord(DamageZone::ID zone) const {
    static DamageScaleRecord null_record;
    switch (zone) {
        case DamageZone::DZ_FRONT:
            return mSpecs.DZ_FRONT();
        case DamageZone::DZ_REAR:
            return mSpecs.DZ_REAR();
        case DamageZone::DZ_LEFT:
            return mSpecs.DZ_LEFT();
        case DamageZone::DZ_RIGHT:
            return mSpecs.DZ_RIGHT();
        case DamageZone::DZ_LFRONT:
            return mSpecs.DZ_LFRONT();
        case DamageZone::DZ_RFRONT:
            return mSpecs.DZ_RFRONT();
        case DamageZone::DZ_LREAR:
            return mSpecs.DZ_LREAR();
        case DamageZone::DZ_RREAR:
            return mSpecs.DZ_RREAR();
        case DamageZone::DZ_TOP:
            return mSpecs.DZ_TOP();
        case DamageZone::DZ_BOTTOM:
            return mSpecs.DZ_BOTTOM();
        default:
            return null_record;
    }
}

void DamageVehicle::OnTaskSimulate(float dT) {
    if ((mShockTimer > 0.0f) && (mSpecs.SHOCK_TIME() > 0.0f)) {
        float recover = dT / mSpecs.SHOCK_TIME();
        if (mShockTimer > recover) {
            mShockTimer -= recover;
        } else {
            mShockTimer = 0.0f;
        }
    } else {
        mShockTimer = 0.0f;
    }
}

void DamageVehicle::Reset() {
    mShockTimer = 0.0f;
}

void DamageVehicle::Destroy() {
    if (mDamageTotal < 1.0f) {
        if (CanDamageVisuals()) {
            for (unsigned int i = 0; i < DamageZone::DZ_MAX; i++) {
                mZoneDamage.Set(static_cast<DamageZone::ID>(i), 6);
            }
        }
        mDamageTotal = 1.0f;
        new EVehicleDestroyed(GetOwner()->GetInstanceHandle());
    }
}

void DamageVehicle::SetShockForce(float f) {
    if (f > 0.0f && mSpecs.SHOCK_FORCE() > 0.0f) {
        float mass = GetOwner()->GetRigidBody()->GetMass();
        float impulse = f / mass;
        SetInShock(1.0f / mSpecs->SHOCK_FORCE() * impulse);
    }
}

void DamageVehicle::SetInShock(float scale) {
    if (mSpecs.SHOCK_TIME() <= 0.0f || scale <= 0.2f) {
        return;
    }
    mShockTimer = UMath::Min(UMath::Max(mShockTimer, scale), 1.0f);
}
