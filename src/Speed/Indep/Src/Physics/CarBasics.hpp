#ifndef CARBASICS_HPP
#define CARBASICS_HPP

#include "types.h"

#define NUM_WHEELS 4
#define MAX_SPEED 160.0f
#define CAR_RADIUS 3.0f
#define ABSOLUTE_MAX_STEERING 45.0f

enum Gear {
    SPORT_SHIFT = -2,
    AUTOMATIC = -1,
    REVERSE = 0,
    NEUTRAL = 1,
    FIRST_GEAR = 2,
    SECOND_GEAR = 3,
    THIRD_GEAR = 4,
    FOURTH_GEAR = 5,
    FIFTH_GEAR = 6,
    SIXTH_GEAR = 7,
    SEVENTH_GEAR = 8,
};

typedef int32 CarLoaderHandle;

// inline float MPH2MPS(float x) {
//     return x * 0.44703001f;
// }

#endif
