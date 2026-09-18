#ifndef CAMERA_ICE_ICEANCHOR_H
#define CAMERA_ICE_ICEANCHOR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Camera/ICE/ICEMath.hpp"

// total size: 0x94
struct ICEAnchor {
    ICE::Vector3 *GetVelocity() { return &mVelocity; }

    float GetVelocityMagnitude() { return mVelMag; }

    ICE::Vector3 *GetForwardVector() { return reinterpret_cast<ICE::Vector3 *>(&mGeomRot.v0); }

    ICE::Vector3 *GetUpVector() { return reinterpret_cast<ICE::Vector3 *>(&mGeomRot.v2); }

    ICE::Vector3 *GetGeometryPosition() { return &mGeomPos; }

    ICE::Vector3 *GetAcceleration() { return &mAccel; }

    ICE::Matrix4 *GetGeometryOrientation() { return &mGeomRot; }

    float GetTopSpeed() const { return mTopSpeed; }

    void SetTopSpeed(float s) { mTopSpeed = s; }

    float GetRPM() const { return mRPM; }

    void SetRPM(float s) { mRPM = s; }

    int GetNumWheels() const { return mNumWheels; }

    void SetNumWheels(int w) { mNumWheels = w; }

    int IsTouchingGround() const { return mIsTouchingGround; }

    void SetTouchingGround(bool touching) { mIsTouchingGround = touching; }

    float GetForwardSlip() const { return mForwardSlip; }

    void SetForwardSlip(float slip) { mForwardSlip = slip; }

    float GetSlipAngle() const { return mSlipAngle; }

    void SetSlipAngle(float a) { mSlipAngle = a; }

    int IsNosEngaged() const { return mIsNosEngaged; }

    void SetNosEngaged(int engaged) { mIsNosEngaged = engaged; }

    float GetNosPercentageLeft() const { return mNosPercentageLeft; }

    void SetNosPercentageLeft(float percentage) { mNosPercentageLeft = percentage; }

    void Update(float dT, const ICE::Matrix4 &orientpos, const ICE::Vector3 &velocity, const ICE::Vector3 &acceleration);

    ICEAnchor();

    ICE::Vector3 mGeomPos;    // offset 0x0, size 0x10
    ICE::Matrix4 mGeomRot;    // offset 0x10, size 0x40
    ICE::Vector3 mVelocity;   // offset 0x50, size 0x10
    ICE::Vector3 mAccel;      // offset 0x60, size 0x10
    float mVelMag;            // offset 0x70, size 0x4
    float mTopSpeed;          // offset 0x74, size 0x4
    float mRPM;               // offset 0x78, size 0x4
    int mNumWheels;           // offset 0x7C, size 0x4
    float mForwardSlip;       // offset 0x80, size 0x4
    float mSlipAngle;         // offset 0x84, size 0x4
    bool mIsTouchingGround;   // offset 0x88, size 0x1
    int mIsNosEngaged;        // offset 0x8C, size 0x4
    float mNosPercentageLeft; // offset 0x90, size 0x4
};

#endif
