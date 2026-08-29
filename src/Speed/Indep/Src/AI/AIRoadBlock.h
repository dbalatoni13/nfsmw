//
//
//
//
//
//
//
//
//
//
#ifndef __AIROADBLOCK_H
#define __AIROADBLOCK_H 1 // Decl: 12

// Decl: 109
enum RBElementType {
    kNone = 0,
    kCar = 1,
    kBarrier = 2,
    kSpikeStrip = 3,
};

// Decl: 119
struct RoadblockElement {
    RoadblockElement(RBElementType e, float offx, float offz, float a)
        : mElementType(e), //
          mOffsetX(offx),  //
          mOffsetZ(offz),  //
          mAngle(a) {}

    RBElementType mElementType; // offset 0x0, size 0x4
    float mOffsetX;             // offset 0x4, size 0x4
    float mOffsetZ;             // offset 0x8, size 0x4
    float mAngle;               // offset 0xC, size 0x4
};

static const int MAX_RB_ELEMENTS = 6; // Decl: 128

#define RE(a, b, c, d) RoadblockElement(a, b, c, d) // Decl: 130
#define NoRE RE(kNone, 0.f, 0.f, 0)                 // Decl: 131

// total size: 0x68
// Decl: 133
struct RoadblockSetup {
    float mMinimumWidthRequired;                 // offset 0x0, size 0x4
    int mRequiredVehicles;                       // offset 0x4, size 0x4
    RoadblockElement mContents[MAX_RB_ELEMENTS]; // offset 0x8, size 0x60
};

// TODO remove?
extern RoadblockSetup RoadblockCandidateList[16];
extern RoadblockSetup SPIKES_RoadblockCandidateList[10];

#endif
