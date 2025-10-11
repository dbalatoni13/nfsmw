#include "DamageHeli.h"
#include "Speed/Indep/Src/Physics/Behaviors/DamageVehicle.h"

Behavior *DamageHeli::Construct(const BehaviorParams &params) {
    const DamageParams dp(params.fparams.Fetch<DamageParams>(UCrc32(0xa6b47fac)));
    return new DamageHeli(params, dp);
}

DamageHeli::DamageHeli(const BehaviorParams &bp, const DamageParams &dp) : DamageVehicle(bp, dp) {
    mAutoDestruct = 0;
    mDestroying = 0;
}

DamageHeli::~DamageHeli() {}

void DamageHeli::DoAutoDestruct() {}

void DamageHeli::Reset() {
    mAutoDestruct = 0;
    mDestroying = 0;
    Behavior::Reset();
}

void DamageHeli::OnTaskSimulate(float dT) {
    DoAutoDestruct();
    Behavior::OnTaskSimulate(dT);
}
