#include "OnlineManager.hpp"

#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"

float ExtrapolatedCar::State::SquaredDistanceTo(State &target) const {
    return VU0_v3distancesquare(mPosition, target.mPosition);
}

bool ExtrapolatedCar::State::IsBlending() const { return mBlend > 0.0f; }

void ExtrapolatedCar::ExtractExtrapolatedPosition(UMath::Vector3 &position) const {
    position = mBlended.mPosition;
}

void ExtrapolatedCar::ExtractExtrapolatedDirection(UMath::Vector3 &direction) const {
    mBlended.ExtractDirection(direction);
}

bool ExtrapolatedCar::IsAbleToSee(ExtrapolatedCar &target) {
    float fVar1;

    fVar1 = mBlended.SquaredDistanceTo(target.mBlended);
    return fVar1 < 40000.0f;
}
