#include "Speed/Indep/Src/AI/AIVehicleHelicopter.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pursuitlevels.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"

bool HeliVehicleActive() {
    if (gHeliVehicle) {
        return true;
    } else {
        return false;
    }
}

AIVehicleHelicopter::AIVehicleHelicopter(const BehaviorParams &bp)
    : AIVehiclePursuit(bp),          //
      IAIHelicopter(bp.fowner),      //
      mPerpHiddenFromMe(false),      //
      mHeliFuelTimeRemaining(30.0f), //
      mDustStormIntensity(0.0f),     //
      mShadowScale(0.0f) {
    static float height = 0.0f;

    height += 10.0f;

    GetOwner()->QueryInterface(&mISimpleChopper);
    mStrafeToDest = false;

    mLookAtPosition = UMath::Vector3::kZero;
    mLastPlaceHeliSawPerp = UMath::Vector3::kZero;
    mDestinationVelocity = UMath::Vector3::kZero;
}

AIVehicleHelicopter::~AIVehicleHelicopter() {
    gHeliVehicle = nullptr;
}

Behavior *AIVehicleHelicopter::Construct(const BehaviorParams &bp) {
    return new AIVehicleHelicopter(bp);
}

void AIVehicleHelicopter::SetFuelFull() {
    gHeliVehicle = this;
    ISimable *simable = IPlayer::First(PLAYER_LOCAL)->GetSimable();
    IPerpetrator *perp;
    if (simable && simable->QueryInterface(&perp)) {
        Attrib::Gen::pursuitlevels *pursuitLevelAttrib = perp->GetPursuitLevelAttrib();
        if (pursuitLevelAttrib) {
            mHeliFuelTimeRemaining = pursuitLevelAttrib->HeliFuelTime();
        }
    }
}

void AIVehicleHelicopter::SetDestinationVelocity(const UMath::Vector3 &v) {
    mDestinationVelocity = v;
    UMath::Scale(mDestinationVelocity, 9.0f, mDestinationVelocity);
    UMath::AddScale(v, mDestinationVelocity, 0.1f, mDestinationVelocity);
}
