//
//
//
#ifndef STICH_STRUCT_H
#define STICH_STRUCT_H

#define MAX_NUM_SAMPLES_PER_STICH 18 // Decl: 7

#include "Speed/Indep/bWare/Inc/bMath.hpp"

enum STICH_TYPE {
    MAX_NUM_STICH_TYPE = 3,
    STICH_TYPE_STATIC = 2,
    STICH_TYPE_WOOSH = 1,
    STICH_TYPE_COLLISION = 0,
    STICH_TYPE_UNKNOWN = -1,
};

// total size: 0x10
// Decl: 29
struct SND_SampleRef {
    SND_SampleRef() {} // Decl: 30

    uint8 SampleIndex;  // offset 0x0, size 0x1, Decl: 46
    int8 eStichType;    // offset 0x1, size 0x1, Decl: 47
    uint16 Volume;      // offset 0x2, size 0x2, Decl: 48
    uint16 Pitch;       // offset 0x4, size 0x2, Decl: 49
    uint16 Offset;      // offset 0x6, size 0x2, Decl: 50
    bAngle Az;          // offset 0x8, size 0x2, Decl: 51
    uint16 RND_Vol;     // offset 0xA, size 0x2, Decl: 55
    int16 RND_Pitch;    // offset 0xC, size 0x2, Decl: 56
    uint8 Priority;     // offset 0xE, size 0x1, Decl: 57
    uint8 eRollOffType; // offset 0xF, size 0x1, Decl: 58
};

// total size: 0x14
// Decl: 64
struct SND_Stich {
    SND_Stich() {} // Decl: 65

    uint32 NameHash;      // offset 0x0, size 0x4, Decl: 76
    uint16 Volume;        // offset 0x4, size 0x2, Decl: 77
    uint16 StichIndex;    // offset 0x6, size 0x2, Decl: 78
    int8 eStichType;      // offset 0x8, size 0x1, Decl: 79
    uint8 Num_SampleRefs; // offset 0x9, size 0x1, Decl: 80
    int16 RND_Pitch;      // offset 0xA, size 0x2, Decl: 81
    uint16 RND_Vol;       // offset 0xC, size 0x2, Decl: 82

    SND_SampleRef *pSampleRefList; // offset 0x10, size 0x4, Decl: 86
};

#endif
