//
//
//
//
//

#ifndef PHYSICS_TYPE_H
#define PHYSICS_TYPE_H

typedef float Rpm;
typedef float Mps;
typedef float Mps2;
typedef float Mph;
typedef float Kph;
typedef float Meters;
typedef float Kg;
typedef float FtLbs;
typedef float Hp;
typedef float Nm;
typedef float Newtons;
typedef float Seconds;
typedef float Psi;
typedef float Lbs;
typedef float Kgm2;

enum GearID {
    G_REVERSE = 0,
    G_NEUTRAL = 1,
    G_FIRST = 2,
    G_SECOND = 3,
    G_THIRD = 4,
    G_FOURTH = 5,
    G_FIFTH = 6,
    G_SIXTH = 7,
    G_SEVENTH = 8,
    G_EIGHTH = 9,
    G_MAX = 10,
};

enum ShiftStatus {
    SHIFT_STATUS_NONE = 0,
    SHIFT_STATUS_NORMAL = 1,
    SHIFT_STATUS_GOOD = 2,
    SHIFT_STATUS_PERFECT = 3,
    SHIFT_STATUS_MISSED = 4,
};

enum eTireDamage {
    TIRE_DAMAGE_NONE = 0,
    TIRE_DAMAGE_PUNCTURED = 1,
    TIRE_DAMAGE_BLOWN = 2,
};

enum ShiftPotential {
    SHIFT_POTENTIAL_MISS = 5,
    SHIFT_POTENTIAL_PERFECT = 4,
    SHIFT_POTENTIAL_GOOD = 3,
    SHIFT_POTENTIAL_UP = 2,
    SHIFT_POTENTIAL_DOWN = 1,
    SHIFT_POTENTIAL_NONE = 0,
};

namespace Physics {

namespace Wheels {

inline bool IsLeft(const unsigned int i) {
    return (i & 1) == 0;
}
inline bool IsRight(const unsigned int i) {
    return (i & 1) != 0;
}
inline bool IsFront(const unsigned int i) {
    return i < 2;
}
inline bool IsRear(const unsigned int i) {
    return i > 1;
}

}; // namespace Wheels

}; // namespace Physics

#endif
