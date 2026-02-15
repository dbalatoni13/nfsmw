#ifndef AI_AISTEER_H
#define AI_AISTEER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/AI/AIAvoidable.h"

namespace AISteer {

void VehicleSeperation(UMath::Vector3 &separation, IVehicle *myvehicle, const AvoidableList &avoidList, float absolute, float relative);
void Ram(UMath::Vector3 &result, const UMath::Vector3 &myPos, float mySpeed, const UMath::Vector3 &targetPos, const UMath::Vector3 &targetVelocity);
void Pursuit(UMath::Vector3 &result, const UMath::Vector3 &myPos, const UMath::Vector3 &myVel, const UMath::Vector3 &targetPos,
             const UMath::Vector3 &targetVel);
float GetDesiredSpeedToTarget(float distToTarget, float targetSpeed);

}; // namespace AISteer

#endif
