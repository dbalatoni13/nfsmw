#include "DamageRacer.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISpikeable.h"
#include "Speed/Indep/Src/Physics/Behaviors/DamageVehicle.h"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
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
