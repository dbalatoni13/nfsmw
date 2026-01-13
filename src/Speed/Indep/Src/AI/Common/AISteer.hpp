#ifndef AI_COMMON_AISTEER_H
#define AI_COMMON_AISTEER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/AI/AIAvoidable.h"

void VehicleSeperation(UMath::Vector3 &separation, IVehicle *myvehicle, const AvoidableList &avoidList, float absolute, float relative);

#endif
