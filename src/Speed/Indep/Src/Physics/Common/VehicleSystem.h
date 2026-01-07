#ifndef PHYSICS_COMMON_VEHICLESYSTEM_H
#define PHYSICS_COMMON_VEHICLESYSTEM_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"

namespace VehicleSystem {

extern float ENABLE_ROLL_STOPS_THRESHOLD;
extern float PAD_DEAD_ZONE;

}; // namespace VehicleSystem

// TODO where to put this?
namespace VehicleClass {

extern const UCrc32 CAR;
extern const UCrc32 SUBMARINE;
extern const UCrc32 CHOPPER;
extern const UCrc32 BIKE;
extern const UCrc32 BOAT;
extern const UCrc32 SNOWMOBILE;
extern const UCrc32 HOVER;
extern const UCrc32 PLANE;
extern const UCrc32 TANK;
extern const UCrc32 TRAILER;
extern const UCrc32 TRAIN;
extern const UCrc32 TRANSPORT;
extern const UCrc32 RC;
extern const UCrc32 TRACTOR;

}; // namespace VehicleClass

#endif
