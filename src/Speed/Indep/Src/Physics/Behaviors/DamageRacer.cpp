#include "DamageRacer.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Generated/Events/ETireBlown.hpp"
#include "Speed/Indep/Src/Generated/Events/ETirePunctured.hpp"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Physics/Behaviors/DamageVehicle.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

static const float Tweak_SpikedTireBlowTime = 0.5f;
static const float Tweak_RunflatTireBlowTime = 180.0f;
static const bool Tweak_EnableBlowouts = true;
// TODO use these
static const bool Tweak_ResetBlowouts = false;
static const bool Tweak_TestBlowOuts = false;
static const bool Tweak_TestBlowOutIdx[4] = {false, false, false, false};

BIND_BEHAVIOR_FACTORY(DamageRacer);

Behavior *DamageRacer::Construct(const BehaviorParams &params) {
    const DamageParams dp(params.fparams.Fetch<DamageParams>(UCrc32(UCRC32_BASE)));
    return new DamageRacer(params, dp);
}

DamageRacer::DamageRacer(const BehaviorParams &bp, const DamageParams &dp)
    : DamageVehicle(bp, dp), //
      ISpikeable(bp.fowner) {
    this->GetOwner()->QueryInterface(&this->mSuspension);
    bMemSet(this->mBlowOutTimes, 0, sizeof(this->mBlowOutTimes));
    bMemSet(this->mDamage, 0, sizeof(this->mDamage));
}

DamageRacer::~DamageRacer() {}

eTireDamage DamageRacer::GetTireDamage(unsigned int wheelId) const {
    if (wheelId > 3) {
        return TIRE_DAMAGE_NONE;
    }
    return static_cast<eTireDamage>(this->mDamage[wheelId]);
}

unsigned int DamageRacer::GetNumBlowouts() const {
    unsigned int count = 0;
    for (unsigned int wheelId = 0; wheelId < 4; ++wheelId) {
        if (this->mDamage[wheelId] == TIRE_DAMAGE_BLOWN) {
            count++;
        }
    }
    return count;
}

void DamageRacer::Puncture(unsigned int wheelId) {
    if (wheelId < 4 && Tweak_EnableBlowouts && this->mDamage[wheelId] == TIRE_DAMAGE_NONE) {
        this->mDamage[wheelId] = TIRE_DAMAGE_PUNCTURED;

        if (Physics::Info::HasRunflatTires(this->GetVehicle()->GetVehicleAttributes())) {
            this->mBlowOutTimes[wheelId] = Tweak_RunflatTireBlowTime;
        } else {
            this->mBlowOutTimes[wheelId] = Tweak_SpikedTireBlowTime;
        }

        new ETirePunctured(this->GetOwner()->GetInstanceHandle(), wheelId);
    }
}

bool DamageRacer::IsLightDamaged(VehicleFX::ID idx) const {
    if (!this->CanDamageVisuals()) {
        return false;
    }
    return DamageVehicle::IsLightDamaged(idx);
}

DamageZone::Info DamageRacer::GetZoneDamage() const {
    if (!this->CanDamageVisuals()) {
        return DamageZone::Info();
    }
    return DamageVehicle::GetZoneDamage();
}

bool DamageRacer::CanDamageVisuals() const {
    if (FEDatabase != nullptr) {
        // TODO
        // if (!FEDatabase->GetGameplaySettings()->Damage) {
        //     return false;
        // }
    }
    return DamageVehicle::CanDamageVisuals();
}

void DamageRacer::OnTaskSimulate(float dT) {
    if (this->mSuspension != nullptr) {
        for (unsigned int wheelId = 0; wheelId < 4; ++wheelId) {
            if ((this->mDamage[wheelId] == TIRE_DAMAGE_PUNCTURED) && (this->mBlowOutTimes[wheelId] > 0.0f) &&
                ((this->mBlowOutTimes[wheelId] -= dT) < 0.0f)) {
                this->mDamage[wheelId] = TIRE_DAMAGE_BLOWN;
                this->mBlowOutTimes[wheelId] = 0.0f;
                new ETireBlown(this->GetOwner()->GetInstanceHandle(), wheelId);
            }
        }
    }
    DamageVehicle::OnTaskSimulate(dT);
}

void DamageRacer::OnBehaviorChange(const UCrc32 &mechanic) {
    DamageVehicle::OnBehaviorChange(mechanic);
    if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
        this->GetOwner()->QueryInterface(&this->mSuspension);
    }
}

void DamageRacer::ResetDamage() {
    DamageVehicle::ResetDamage();
    bMemSet(this->mBlowOutTimes, 0, sizeof(this->mBlowOutTimes));
    bMemSet(this->mDamage, 0, sizeof(this->mDamage));
}
