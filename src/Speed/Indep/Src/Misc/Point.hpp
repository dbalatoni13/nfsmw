#ifndef MISC_POINT_H
#define MISC_POINT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Spline.hpp"

// total size: 0x178
struct cPoint {
    tCubic3D mPos;                        // offset 0x0, size 0x84
    tCubic3D mRot;                        // offset 0x84, size 0x84
    tCubic1D mScale;                      // offset 0x108, size 0x2C
    tCubic1D mYScale;                     // offset 0x134, size 0x2C
    float mZBias;                         // offset 0x160, size 0x4
    void (*mCallBackFunc)(int, int);      // offset 0x164, size 0x4
    void (*mUpdateFunc)(cPoint *, float); // offset 0x168, size 0x4
    int mCBParam1;                        // offset 0x16C, size 0x4
    int mCBParam2;                        // offset 0x170, size 0x4

    static void SplineSeek(tCubic1D *cubic, float dt, float maxDeriv, float maxSecondDeriv);
    static void SplineSeek(tCubic2D *cubic, float dt);
};

#endif
