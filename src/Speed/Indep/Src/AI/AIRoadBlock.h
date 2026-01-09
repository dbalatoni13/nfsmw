#ifndef AI_AIROADBLOCK_H
#define AI_AIROADBLOCK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

enum RBElementType {
    kNone = 0,
    kCar = 1,
    kBarrier = 2,
    kSpikeStrip = 3,
};

struct RoadblockElement {
    RoadblockElement(enum RBElementType e, float offx, float offz, float a) {}

    RBElementType mElementType; // offset 0x0, size 0x4
    float mOffsetX;             // offset 0x4, size 0x4
    float mOffsetZ;             // offset 0x8, size 0x4
    float mAngle;               // offset 0xC, size 0x4
};

// total size: 0x68
struct RoadblockSetup {
    float mMinimumWidthRequired;   // offset 0x0, size 0x4
    int mRequiredVehicles;         // offset 0x4, size 0x4
    RoadblockElement mContents[6]; // offset 0x8, size 0x60
};

extern RoadblockSetup RoadblockCandidateList[16];
extern RoadblockSetup SPIKES_RoadblockCandidateList[10];

#endif
