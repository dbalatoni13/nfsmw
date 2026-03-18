#ifndef MISC_SPLINE_H
#define MISC_SPLINE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bMath.hpp"

// total size: 0x2C
struct tCubic1D {
    float Val;        // offset 0x0, size 0x4
    float dVal;       // offset 0x4, size 0x4
    float ValDesired; // offset 0x8, size 0x4
    float dValDesired; // offset 0xC, size 0x4
    float Coeff[4];   // offset 0x10, size 0x10
    float time;       // offset 0x20, size 0x4
    float duration;   // offset 0x24, size 0x4
    short state;      // offset 0x28, size 0x2
    short flags;      // offset 0x2A, size 0x2

    void MakeCoeffs();
    float GetVal(float t);
    float GetdVal(float t);
    float GetddVal(float t);
    float GetDerivative(float t);
    float GetSecondDerivative(float t);
    void ClampDerivative(float maxDeriv);
    void ClampSecondDerivative(float maxSecondDeriv);
    void Update(float dt, float maxDeriv, float maxSecondDeriv);
};

// total size: 0x58
struct tCubic2D {
    tCubic1D x; // offset 0x0, size 0x2C
    tCubic1D y; // offset 0x2C, size 0x2C

    void SetValDesired(bVector2 *v);
    void GetVal(bVector2 *v);
};

// total size: 0x84
struct tCubic3D {
    tCubic1D x; // offset 0x0, size 0x2C
    tCubic1D y; // offset 0x2C, size 0x2C
    tCubic1D z; // offset 0x58, size 0x2C

    void SetVal(const bVector3 *v);
    void SetdVal(bVector3 *v);
    void SetValDesired(bVector3 *v);
    void GetVal(bVector3 *v);
    void GetValDesired(bVector3 *v);
    void Update(float dt, float maxDeriv, float maxSecondDeriv);
};

#endif
