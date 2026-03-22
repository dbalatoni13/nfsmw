#ifndef CAMERA_ICE_ICEMOVER_H
#define CAMERA_ICE_ICEMOVER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "ICEMath.hpp"
#include "ICEData.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"

namespace ICE {

struct Cubic1D {
    Cubic1D(short type = 0, float dur = 0.0f)
        : Val(0.0f), //
          dVal(0.0f), //
          ValDesired(0.0f), //
          dValDesired(0.0f), //
          time(0.0f), //
          duration(dur), //
          state(0), //
          flags(type) {
        Coeff[0] = 0.0f;
        Coeff[1] = 0.0f;
        Coeff[2] = 0.0f;
        Coeff[3] = 0.0f;
    }

    void Snap() {
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
        if (v != dVal) {
            state = 2;
        }
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
        return GetddVal(time);
    }

    int HasArrived() const {
        return state == 0;
    }

    void MakeCoeffs();
    float GetVal(float t) const;
    float GetdVal(float t) const;
    float GetddVal(float t) const;
    float GetValDesired() const;
    float GetDerivative(float t) const;
    float GetSecondDerivative(float t) const;
    void ClampDerivative(float fMag);
    void ClampSecondDerivative(float fMag);
    void Update(float fSeconds, float fDClamp, float fDDClamp);

    float Val;
    float dVal;
    float ValDesired;
    float dValDesired;
    float Coeff[4];
    float time;
    float duration;
    short state;
    short flags;
};

struct Cubic3D {
    Cubic3D(short type = 0, float dur = 0.0f)
        : x(type, dur), //
          y(type, dur), //
          z(type, dur) {}

    Cubic3D(short type, const Vector3 *pDuration)
        : x(type, pDuration->x), //
          y(type, pDuration->y), //
          z(type, pDuration->z) {}

    int HasArrived() const {
        return x.HasArrived() && y.HasArrived() && z.HasArrived();
    }

    void Snap() {
        x.Snap();
        y.Snap();
        z.Snap();
    }

    void SetDuration(float t) {
        x.SetDuration(t);
        y.SetDuration(t);
        z.SetDuration(t);
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
        x.dValDesired = vx;
        y.dValDesired = vy;
        z.dValDesired = vz;
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

    Cubic1D x;
    Cubic1D y;
    Cubic1D z;
};

} // namespace ICE

struct ICEAnchor {
    ICE::Vector3 *GetVelocity() {
        return &mVelocity;
    }

    float GetVelocityMagnitude() {
        return mVelMag;
    }

    ICE::Vector3 *GetForwardVector() {
        return reinterpret_cast<ICE::Vector3 *>(&mGeomRot.v0);
    }

    ICE::Vector3 *GetUpVector() {
        return reinterpret_cast<ICE::Vector3 *>(&mGeomRot.v2);
    }

    ICE::Vector3 *GetGeometryPosition() {
        return &mGeomPos;
    }

    ICE::Vector3 *GetAcceleration() {
        return &mAccel;
    }

    ICE::Matrix4 *GetGeometryOrientation() {
        return &mGeomRot;
    }

    float GetTopSpeed() const {
        return mTopSpeed;
    }

    void SetTopSpeed(float s) {
        mTopSpeed = s;
    }

    float GetRPM() const {
        return mRPM;
    }

    void SetRPM(float s) {
        mRPM = s;
    }

    bool GetNumWheels() const {
        return mNumWheels;
    }

    void SetNumWheels(bool w) {
        mNumWheels = w;
    }

    int IsTouchingGround() const {
        return mIsTouchingGround;
    }

    void SetTouchingGround(bool touching) {
        mIsTouchingGround = touching;
    }

    float GetForwardSlip() const {
        return mForwardSlip;
    }

    void SetForwardSlip(float slip) {
        mForwardSlip = slip;
    }

    float GetSlipAngle() const {
        return mSlipAngle;
    }

    void SetSlipAngle(float a) {
        mSlipAngle = a;
    }

    bool IsNosEngaged() const {
        return mIsNosEngaged;
    }

    void SetNosEngaged(bool engaged) {
        mIsNosEngaged = engaged;
    }

    float GetNosPercentageLeft() const {
        return mNosPercentageLeft;
    }

    void SetNosPercentageLeft(float percentage) {
        mNosPercentageLeft = percentage;
    }

    void Update(float dT, const ICE::Matrix4 &orientpos, const ICE::Vector3 &velocity, const ICE::Vector3 &acceleration);
    ICEAnchor();

    ICE::Vector3 mGeomPos;
    ICE::Matrix4 mGeomRot;
    ICE::Vector3 mVelocity;
    ICE::Vector3 mAccel;
    float mVelMag;
    float mTopSpeed;
    float mRPM;
    bool mNumWheels;
    float mForwardSlip;
    float mSlipAngle;
    bool mIsTouchingGround;
    bool mIsNosEngaged;
    float mNosPercentageLeft;
};

class ICEMover : public CameraMover {
  public:
    ICEMover(int nView, ICEAnchor *pCar);
    ~ICEMover() override;

    void EyeCubicInit(ICE::Cubic3D *pEye, ICE::Matrix4 *pMatrix, ICE::Vector3 *pVelocity);
    void LookCubicInit(ICE::Cubic3D *pLook, ICE::Matrix4 *pMatrix, ICE::Vector3 *pVelocity);
    void DutchCubicInit(ICE::Cubic1D *pDutch);
    void FovCubicInit(ICE::Cubic1D *pFov);
    void SetDesired(bool b_snap, bool b_refresh);
    void GetEye(ICE::Vector3 *vEye, float f_param);
    void GetLook(ICE::Vector3 *vLook, float f_param);
    float GetDutch(float f_param);
    unsigned short GetFOV(float f_param);
    float GetNearClip(float f_param);
    float GetNoiseAmplitude(float f_param);
    float GetNoiseFrequency(float f_param);
    float GetFocalDistance(float f_param);
    float GetAperture(float f_param);
    float GetLetterbox(float f_param);
    float GetSimSpeed(float f_param);
    void Update(float dT) override;

    bool IsViolatingTopology() {
        return bViolatesTopology;
    }

    bool IsSmooth() {
        return pICEData != 0 && pICEData->bSmooth != 0;
    }

    ICEAnchor *GetICEAnchor() {
        return pCar;
    }

    ICEAnchor *pCar;
    ICE::Cubic3D *pEye;
    ICE::Cubic3D *pLook;
    ICE::Cubic1D *pDutch;
    ICE::Cubic1D *pFov;
    ICE::Cubic1D *pNearClip;
    ICE::Cubic1D *pNoiseAmplitude;
    ICE::Cubic1D *pNoiseFrequency;
    ICE::Cubic1D *pFocalDistance;
    ICE::Cubic1D *pAperture;
    ICE::Cubic1D *pLetterbox;
    ICE::Cubic1D *pSimSpeed;
    ICE::Cubic3D *pAccelOffset;
    float fParameter0;
    float fParameter1;
    int nSpaceEye;
    int nSpaceLook;
    ICEData *pICEData;
    bool bViolatesTopology;
    ICE::Matrix4 mHybridToWorld;
    ICE::Vector3 vSmoothCarPos;
    ICE::Vector3 vSmoothCarFwd;
};


#endif
