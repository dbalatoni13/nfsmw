#ifndef MISC_SPLINE_H
#define MISC_SPLINE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bMath.hpp"

// total size: 0x2C
struct tCubic1D {
    void MakeCoeffs();
    float GetVal(float t);
    float GetdVal(float t);
    float GetddVal(float t);
    float GetDerivative(float t);
    float GetSecondDerivative(float t);
    void ClampDerivative(float maxDeriv);
    void ClampSecondDerivative(float maxSecondDeriv);
    void Update(float dt, float maxDeriv, float maxSecondDeriv);

    void Snap() {
        time = 1.0f;
        Val = ValDesired;
        dVal = dValDesired;
        state = 0;
    }

    void SetVal(const float v) {
        Val = v;
        if (v != ValDesired) {
            state = 2;
        }
    }

    void SetdVal(float v) {
        dVal = v;
        if (v != dValDesired) {
            state = 2;
        }
    }

    void SetValDesired(float v) {
        ValDesired = v;
        if (v != Val) {
            state = 2;
        }
    }

    void SetdValDesired(float v) {
        dValDesired = v;
    }

    float Val;         // offset 0x0, size 0x4
    float dVal;        // offset 0x4, size 0x4
    float ValDesired;  // offset 0x8, size 0x4
    float dValDesired; // offset 0xC, size 0x4
    float Coeff[4];    // offset 0x10, size 0x10
    float time;        // offset 0x20, size 0x4
    float duration;    // offset 0x24, size 0x4
    short state;       // offset 0x28, size 0x2
    short flags;       // offset 0x2A, size 0x2
};

// total size: 0x58
struct tCubic2D {
    void SetValDesired(bVector2 *v);
    void GetVal(bVector2 *v);

    void SetValDesired(float vx, float vy) {
        x.SetValDesired(vx);
        y.SetValDesired(vy);
    }

    tCubic1D x; // offset 0x0, size 0x2C
    tCubic1D y; // offset 0x2C, size 0x2C
};

// total size: 0x84
struct tCubic3D {
    void SetVal(const bVector3 *v);
    void SetdVal(bVector3 *v);
    void SetValDesired(bVector3 *v);
    void GetVal(bVector3 *v);
    void GetValDesired(bVector3 *v);
    void Update(float dt, float maxDeriv, float maxSecondDeriv);

    void SetVal(const float vx, const float vy, const float vz) {
        x.SetVal(vx);
        y.SetVal(vy);
        z.SetVal(vz);
    }

    void SetdVal(const float vx, const float vy, const float vz) {
        x.SetdVal(vx);
        y.SetdVal(vy);
        z.SetdVal(vz);
    }

    void SetValDesired(const float vx, const float vy, const float vz) {
        x.SetValDesired(vx);
        y.SetValDesired(vy);
        z.SetValDesired(vz);
    }

    tCubic1D x; // offset 0x0, size 0x2C
    tCubic1D y; // offset 0x2C, size 0x2C
    tCubic1D z; // offset 0x58, size 0x2C
};

#endif
