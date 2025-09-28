#ifndef PHYSICS_WHEEL_H
#define PHYSICS_WHEEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"
#include "Speed/Indep/Src/World/World.hpp"

class Wheel {
  public:
    Wheel() {
        // TODO
    }

    ~Wheel();

    bool UpdatePosition(const UMath::Vector3 &body_av, const UMath::Vector3 &body_lv, const UMath::Matrix4 &body_matrix, const UMath::Vector3 &cog,
                        float dT, float wheel_radius, bool usecache, const void *collider, float vehicle_height);

    const UMath::Vector4 &GetNormal() const {
        return mNormal;
    }

    const UMath::Vector3 &GetPosition() const {
        return mPosition;
    }

    void SetPosition(UMath::Vector3 &p) {
        mPosition = p;
    }

    void SetY(float y) {
        mPosition.y = y;
    }

    const UMath::Vector3 &GetForce() const {
        return mForce;
    }

    void SetForce(const UMath::Vector3 &f) {
        mForce = f;
    }

    void IncAirTime(float dT) {
        mAirTime += dT;
    }

    const UMath::Vector3 &GetLocalArm() const {
        return mLocalArm;
    }

    float GetCompression() const {
        return mCompression;
    }

    void SetCompression(float c) {
        mCompression = c;
    }

    const UMath::Vector3 &GetVelocity() const {
        return mVelocity;
    }

  protected:
    // total size: 0xC4
    WWorldPos mWorldPos;                                               // offset 0x0, size 0x48
    UMath::Vector4 mNormal;                                            // offset 0x48, size 0x10
    UMath::Vector3 mPosition;                                          // offset 0x58, size 0xC
    enum eWheelFlags { WF_SMOOTHING = 1, WF_CHECKDYNAMIC = 2 } mFlags; // offset 0x64, size 0x4
    UMath::Vector3 mForce;                                             // offset 0x68, size 0xC
    float mAirTime;                                                    // offset 0x74, size 0x4
    UMath::Vector3 mLocalArm;                                          // offset 0x78, size 0xC
    float mCompression;                                                // offset 0x84, size 0x4
    UMath::Vector3 mWorldArm;                                          // offset 0x88, size 0xC
    int pad;                                                           // offset 0x94, size 0x4
    UMath::Vector3 mVelocity;                                          // offset 0x98, size 0xC
    int pad2;                                                          // offset 0xA4, size 0x4
    SimSurface mSurface;                                               // offset 0xA8, size 0x14
    float mSurfaceStick;                                               // offset 0xBC, size 0x4
    void *mIntegral;                                                   // offset 0xC0, size 0x4
};

#endif
