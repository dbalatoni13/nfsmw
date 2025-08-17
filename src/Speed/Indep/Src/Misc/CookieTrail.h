#ifndef MISC_COOKIE_TRAIL_H
#define MISC_COOKIE_TRAIL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UMath.h"

template <typename T, std::size_t U> class CookieTrail {
    // total size: 0x810
    int mCount;          // offset 0x0, size 0x4
    int mLast;           // offset 0x4, size 0x4
    const int mCapacity; // offset 0x8, size 0x4
    int pad;             // offset 0xC, size 0x4
    T mData[U];          // offset 0x10, size 0x800

  public:
    CookieTrail() {}
};

struct NavCookie {
    // total size: 0x40
    UMath::Vector2 Left;               // offset 0x0, size 0x8
    UMath::Vector2 Right;              // offset 0x8, size 0x8
    UMath::Vector2 Forward;            // offset 0x10, size 0x8
    float Length;                      // offset 0x18, size 0x4
    float Curvature;                   // offset 0x1C, size 0x4
    float LeftOffset;                  // offset 0x20, size 0x4
    float RightOffset;                 // offset 0x24, size 0x4
    unsigned int Flags;                // offset 0x28, size 0x4
    float Padding;                     // offset 0x2C, size 0x4
    UMath::Vector3 Centre;             // offset 0x30, size 0xC
    short SegmentParameter;            // offset 0x3C, size 0x2
    unsigned short SegmentNumber : 15; // offset 0x3E, size 0x2
    unsigned short SegmentNodeInd : 1; // offset 0x3E, size 0x2
};

struct ResetCookie {
    // total size: 0x20
    UMath::Vector3 position;  // offset 0x0, size 0xC
    unsigned int flags;       // offset 0xC, size 0x4
    UMath::Vector3 direction; // offset 0x10, size 0xC
    float time;               // offset 0x1C, size 0x4
};

#endif
