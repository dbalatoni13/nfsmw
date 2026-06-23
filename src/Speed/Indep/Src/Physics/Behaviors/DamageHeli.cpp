#include "DamageHeli.h"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Physics/Behaviors/DamageVehicle.h"

BIND_BEHAVIOR_FACTORY(DamageHeli);

Behavior *DamageHeli::Construct(const BehaviorParams &params) {
    const DamageParams dp(params.fparams.Fetch<DamageParams>(UCrc32(UCRC32_BASE)));
    return new DamageHeli(params, dp);
}

DamageHeli::DamageHeli(const BehaviorParams &bp, const DamageParams &dp) : DamageVehicle(bp, dp) {
    this->mAutoDestruct = 0;
    this->mDestroying = 0;
}

DamageHeli::~DamageHeli() {}

void DamageHeli::DoAutoDestruct() {}

void DamageHeli::Reset() {
    this->mAutoDestruct = 0;
    this->mDestroying = 0;
    DamageVehicle::Reset();
}

void DamageHeli::OnTaskSimulate(float dT) {
    this->DoAutoDestruct();
    DamageVehicle::OnTaskSimulate(dT);
}
