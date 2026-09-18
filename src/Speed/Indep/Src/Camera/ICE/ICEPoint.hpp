#ifndef CAMERA_ICE_ICEPOINT_H
#define CAMERA_ICE_ICEPOINT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Camera/ICE/ICEMath.hpp"

namespace ICE {

// total size: 0x2C
struct Cubic1D {
    Cubic1D(short type = 0, float dur = 1.0f) {
        Val = 0.0f; dVal = 0.0f; ValDesired = 0.0f; dValDesired = 0.0f; time = 0.0f; duration = dur; state = 0; flags = type;

        Coeff[0] = 0.0f;
        Coeff[1] = 0.0f;
        Coeff[2] = 0.0f;
        Coeff[3] = 0.0f;
    }

    void ZeroCoeffs() {
        Coeff[0] = 0.0f;
        Coeff[1] = 0.0f;
        Coeff[2] = 0.0f;
        Coeff[3] = 0.0f;
    }

    void Snap() {
        time = 1.0f;
        Val = ValDesired;
        dVal = dValDesired;
        state = 0;
    }

    void SetVal(float v) {
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

    void SetDuration(float t) {
        duration = t;
    }

    void SetState(short s) {
        state = s;
    }

    void SetFlags(short f) {
        flags = f;
    }

    float GetVal() const {
        return Val;
    }

    float GetdVal() const {
        return dVal;
    }

    float GetddVal() const {
        return Coeff[1] + Coeff[1];
    }

    int HasArrived() const {
        return state == 0;
    }

    void MakeCoeffs();
    float GetVal(float t) const;
    float GetdVal(float t) const;
    float GetddVal(float t) const;
    float GetValDesired() const;
    float GetdValDesired() const;
    float GetDerivative(float t) const;
    float GetSecondDerivative(float t) const;
    void ClampDerivative(float fMag);
    void ClampSecondDerivative(float fMag);
    void Update(float fSeconds, float fDClamp, float fDDClamp);

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

// total size: 0x84
struct Cubic3D {
    Cubic3D(short type = 0, float dur = 1.0f) : x(type, dur), y(type, dur), z(type, dur) {}

    int HasArrived() const {
        return x.HasArrived() && y.HasArrived() && z.HasArrived();
    }

    void Snap() {
        x.Snap();
        y.Snap();
        z.Snap();
    }

    void SetVal(float vx, float vy, float vz) {
        x.SetVal(vx);
        y.SetVal(vy);
        z.SetVal(vz);
    }

    void SetdVal(float vx, float vy, float vz) {
        x.SetdVal(vx);
        y.SetdVal(vy);
        z.SetdVal(vz);
    }

    void SetValDesired(float vx, float vy, float vz) {
        x.SetValDesired(vx);
        y.SetValDesired(vy);
        z.SetValDesired(vz);
    }

    void SetdValDesired(float vx, float vy, float vz) {
        x.SetdValDesired(vx);
        y.SetdValDesired(vy);
        z.SetdValDesired(vz);
    }

    void SetDuration(float t) {
        x.SetDuration(t);
        y.SetDuration(t);
        z.SetDuration(t);
    }

    void SetDuration(float tx, float ty, float tz) {
        x.SetDuration(tx);
        y.SetDuration(ty);
        z.SetDuration(tz);
    }

    void SetState(short s) {
        x.SetState(s);
        y.SetState(s);
        z.SetState(s);
    }

    void SetFlags(short s) {
        x.SetFlags(s);
        y.SetFlags(s);
        z.SetFlags(s);
    }

    void MakeCoeffs() {
        x.MakeCoeffs();
        y.MakeCoeffs();
        z.MakeCoeffs();
    }

    void SetVal(const Vector3 *pV);
    void SetdVal(const Vector3 *pV);
    void SetValDesired(const Vector3 *pV);
    void SetdValDesired(const Vector3 *pV);
    void GetVal(Vector3 *pV) const;
    void GetdVal(Vector3 *pV) const;
    void GetVal(Vector3 *pV, float t) const;
    void GetValDesired(Vector3 *pV) const;
    void Update(float fSeconds, float fDClamp, float fDDClamp);

    Cubic1D x; // offset 0x0, size 0x2C
    Cubic1D y; // offset 0x2C, size 0x2C
    Cubic1D z; // offset 0x58, size 0x2C
};

}; // namespace ICE

#endif
