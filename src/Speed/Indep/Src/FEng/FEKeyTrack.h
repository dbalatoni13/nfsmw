#ifndef FENG_FEKEYTRACK_H
#define FENG_FEKEYTRACK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FEGenericVal.h"
#include "FERefList.h"

// total size: 0x20
class FEKeyNode : public FEMinNode {
    int tTime;        // offset 0xC, size 0x4
    FEGenericVal Val; // offset 0x10, size 0x10
};

// total size: 0x38
class FEKeyTrack {
    unsigned char ParamType;    // offset 0x0, size 0x1
    unsigned char ParamSize;    // offset 0x1, size 0x1
    unsigned char InterpType;   // offset 0x2, size 0x1
    unsigned char InterpAction; // offset 0x3, size 0x1
    int Length : 24;            // offset 0x4, size 0x4
    int LongOffset : 8;         // offset 0x4, size 0x4
    FEKeyNode BaseKey;          // offset 0x8, size 0x20
    FERefList DeltaKeys;        // offset 0x28, size 0x10
};

#endif
