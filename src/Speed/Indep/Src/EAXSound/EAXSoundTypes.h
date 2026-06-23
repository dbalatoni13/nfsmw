#ifndef EAXSOUND_EAXSOUNDTYPES_H
#define EAXSOUND_EAXSOUNDTYPES_H

#include "Speed/Indep/Src/Physics/CarBasics.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"

extern void *NullPointer;

namespace Sound {

enum Context {
    CONTEXT_PLAYER = 0,
    CONTEXT_AIRACER = 1,
    CONTEXT_COP = 2,
    CONTEXT_TRAFFIC = 3,
    CONTEXT_ONLINE = 4,
    CONTEXT_TRACTOR = 5,
    CONTEXT_TRAILER = 6,
    CONTEXT_MAX = 7,
};

enum SirenState {
    SIREN_OFF = -1,
    SIREN_WAIL = 0,
    SIREN_YELP = 1,
    SIREN_SCREAM = 2,
    SIREN_DIE = 3,
};

enum WheelConfig {
    EAX4WD_FL = 0,
    EAX4WD_FR = 1,
    EAX4WD_RR = 2,
    EAX4WD_RL = 3,
};

enum ControlSource {
    CONTROL_NONE = 0,
    CONTROL_HUMAN = 1,
    CONTROL_AI = 2,
    CONTROL_NIS = 3,
    CONTROL_ONLINE = 4,
};

// total size: 0x44
struct Wheel {
    Wheel() {}

    ~Wheel();

    void Reset() {}

    bVector2 mWheelSlip;         // offset 0x0, size 0x8
    float mWheelForceZ;          // offset 0x8, size 0x4
    float mPercentFsFkTransfer;  // offset 0xC, size 0x4
    int mWheelOnGround;          // offset 0x10, size 0x4
    SimSurface mTerrainType;     // offset 0x14, size 0x14
    SimSurface mPrevTerrainType; // offset 0x28, size 0x14
    float mLoad;                 // offset 0x3C, size 0x4
    uint8 mBlownState;           // offset 0x40, size 0x1
    uint8 mPrevBlownState;       // offset 0x41, size 0x1
};

// total size: 0x1C
struct Engine {
    Engine() {}

    void Reset() {}

    int mBoostFlag;  // offset 0x0, size 0x4
    int mNOSFlag;    // offset 0x4, size 0x4
    float mNOS;      // offset 0x8, size 0x4
    float mRPMPct;   // offset 0xC, size 0x4
    float mThrottle; // offset 0x10, size 0x4
    float mBoost;    // offset 0x14, size 0x4
    int mBlownFlag;  // offset 0x18, size 0x4
};

// total size: 0x8
struct Driveline {
    Driveline() {}

    int mGearShiftFlag; // offset 0x0, size 0x4
    Gear mGear;         // offset 0x4, size 0x4
};

}; // namespace Sound

#endif
