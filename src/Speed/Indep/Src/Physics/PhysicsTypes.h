#ifndef PHYSICS_PHYSICSTYPES_H
#define PHYSICS_PHYSICSTYPES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

enum GearID {
    G_MAX = 10,
    G_EIGHTH = 9,
    G_SEVENTH = 8,
    G_SIXTH = 7,
    G_FIFTH = 6,
    G_FOURTH = 5,
    G_THIRD = 4,
    G_SECOND = 3,
    G_FIRST = 2,
    G_NEUTRAL = 1,
    G_REVERSE = 0,
};

enum eTireDamage {
    TIRE_DAMAGE_BLOWN = 2,
    TIRE_DAMAGE_PUNCTURED = 1,
    TIRE_DAMAGE_NONE = 0,
};

enum ShiftStatus {
    SHIFT_STATUS_MISSED = 4,
    SHIFT_STATUS_PERFECT = 3,
    SHIFT_STATUS_GOOD = 2,
    SHIFT_STATUS_NORMAL = 1,
    SHIFT_STATUS_NONE = 0,
};

struct AxlePair {
    // total size: 0x8
    float Front; // offset 0x0, size 0x4
    float Rear;  // offset 0x4, size 0x4

    float At(int index) const {
        return (&Front)[index];
    }
};

enum DriverStyle {
    STYLE_DRAG = 1,
    STYLE_RACING = 0,
};

enum PhysicsMode {
    PHYSICS_MODE_EMULATED = 2,
    PHYSICS_MODE_SIMULATED = 1,
    PHYSICS_MODE_INACTIVE = 0,
};

enum ShiftPotential {
    SHIFT_POTENTIAL_MISS = 5,
    SHIFT_POTENTIAL_PERFECT = 4,
    SHIFT_POTENTIAL_GOOD = 3,
    SHIFT_POTENTIAL_UP = 2,
    SHIFT_POTENTIAL_DOWN = 1,
    SHIFT_POTENTIAL_NONE = 0,
};

enum eTireIdx {
    // The front left wheel
    TIRE_FL,
    // The front right wheel
    TIRE_FR,
    // The number of wheels on the front axle
    TIRE_MAX_FRONT = 2,
    // The rear right wheel
    TIRE_RR = 2,
    // The rear left wheel
    TIRE_RL,
    // The total number of wheels
    TIRE_MAX,
};

enum eInvulnerablitiy {
    INVULNERABLE_FROM_PHYSICS_SWITCH = 4,
    INVULNERABLE_FROM_CONTROL_SWITCH = 3,
    INVULNERABLE_FROM_RESET = 2,
    INVULNERABLE_FROM_MANUAL_RESET = 1,
    INVULNERABLE_NONE = 0,
};

inline bool IsFront(unsigned int i) {
    return i < 2;
}

inline bool IsRear(unsigned int i) {
    return i > 1;
}

#endif
