#include "DamageVehicle.h"
#include "Speed/Indep/Src/Generated/Events/EVehicleDestroyed.hpp"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IArticulatedVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngineDamage.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/Damagezones.h"

static const bool Tweak_KillCops = false; // TODO use
BIND_BEHAVIOR_FACTORY(DamageVehicle);

Behavior *DamageVehicle::Construct(const BehaviorParams &params) {
    const DamageParams dp(params.fparams.Fetch<DamageParams>(UCrc32(UCRC32_BASE)));
    return new DamageVehicle(params, dp);
}

void DamageVehicle::ResetParts() {
    IRenderable *irenderable;
    if (this->GetOwner()->QueryInterface(&irenderable)) {
        IModel *instance = irenderable->GetModel();
        if (instance != nullptr) {
            instance->ReleaseChildModels();
        }
    }
}

void DamageVehicle::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        this->GetOwner()->QueryInterface(&this->mIRBComplex);
        this->GetOwner()->QueryInterface(&this->mRB);
    }
    if (mechanic == BEHAVIOR_MECHANIC_DRAW) {
        this->GetOwner()->QueryInterface(&this->mRenderable);
    }
}

static const bool Tweak_EnableDamage = true; // TODO use

void DamageVehicle::OnCollision(const COLLISION_INFO &cinfo) {}

bool DamageVehicle::SetDynamicData(const EventSequencer::System *system, EventDynamicData *data) {
    data->fVelocity = UMath::Vector4Make(this->mLastImpactSpeed, 1.0f);
    return true;
}

const DamageScaleRecord &DamageVehicle::GetDamageRecord(DamageZone::ID zone) const {
    static DamageScaleRecord null_record;
    switch (zone) {
        case DamageZone::DZ_FRONT:
            return this->mSpecs.DZ_FRONT();
        case DamageZone::DZ_REAR:
            return this->mSpecs.DZ_REAR();
        case DamageZone::DZ_LEFT:
            return this->mSpecs.DZ_LEFT();
        case DamageZone::DZ_RIGHT:
            return this->mSpecs.DZ_RIGHT();
        case DamageZone::DZ_LFRONT:
            return this->mSpecs.DZ_LFRONT();
        case DamageZone::DZ_RFRONT:
            return this->mSpecs.DZ_RFRONT();
        case DamageZone::DZ_LREAR:
            return this->mSpecs.DZ_LREAR();
        case DamageZone::DZ_RREAR:
            return this->mSpecs.DZ_RREAR();
        case DamageZone::DZ_TOP:
            return this->mSpecs.DZ_TOP();
        case DamageZone::DZ_BOTTOM:
            return this->mSpecs.DZ_BOTTOM();
        default:
            return null_record;
    }
}

void DamageVehicle::OnTaskSimulate(float dT) {
    if ((this->mShockTimer > 0.0f) && (this->mSpecs.SHOCK_TIME() > 0.0f)) {
        float recover = dT / this->mSpecs.SHOCK_TIME();
        if (this->mShockTimer > recover) {
            this->mShockTimer -= recover;
        } else {
            this->mShockTimer = 0.0f;
        }
    } else {
        this->mShockTimer = 0.0f;
    }
}

void DamageVehicle::Reset() {
    this->mShockTimer = 0.0f;
}

void DamageVehicle::Destroy() {
    if (this->mDamageTotal < 1.0f) {
        if (this->CanDamageVisuals()) {
            for (unsigned int i = 0; i < DamageZone::DZ_MAX; i++) {
                this->mZoneDamage.Set(static_cast<DamageZone::ID>(i), 6);
            }
        }
        this->mDamageTotal = 1.0f;
        new EVehicleDestroyed(GetOwner()->GetInstanceHandle());
    }
}

void DamageVehicle::ResetDamage() {
    this->ResetParts();
    this->mLightDamage = 0;
    this->mShockTimer = 0.0f;
    this->mZoneDamage.Clear();
    this->mDamageTotal = 0.0f;

    EventSequencer::IEngine *es = this->GetOwner()->GetEventSequencer();
    if (es != nullptr) {
        es->ProcessStimulus(UCRC32_RESET_DAMAGE, Sim::GetTime(), this, EventSequencer::QUEUE_ALLOW);
    }
    IEngineDamage *ienginedamage;
    if (this->GetOwner()->QueryInterface(&ienginedamage)) {
        ienginedamage->Repair();
    }
    IArticulatedVehicle *iarticulation;
    if (this->GetOwner()->QueryInterface(&iarticulation)) {
        IVehicle *itrailer = iarticulation->GetTrailer();
        IDamageable *idamage;
        if ((itrailer != nullptr) && itrailer->QueryInterface(&idamage)) {
            idamage->ResetDamage();
        }
    }
}

void DamageVehicle::SetShockForce(float f) {
    if (f > 0.0f && this->mSpecs.SHOCK_FORCE() > 0.0f) {
        float mass = this->GetOwner()->GetRigidBody()->GetMass();
        float impulse = f / mass;
        this->SetInShock(1.0f / this->mSpecs->SHOCK_FORCE() * impulse);
    }
}

void DamageVehicle::SetInShock(float scale) {
    if (this->mSpecs.SHOCK_TIME() <= 0.0f || scale <= 0.2f) {
        return;
    }
    this->mShockTimer = UMath::Min(UMath::Max(this->mShockTimer, scale), 1.0f);
}
