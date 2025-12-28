#ifndef PHYSICS_WHEEL_H
#define PHYSICS_WHEEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"
#include "Speed/Indep/Src/World/World.hpp"

class Wheel {
  public:
    void *operator new(std::size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    void operator delete(void *mem, std::size_t size) {
        gFastMem.Free(mem, size, nullptr);
    }

    Wheel() : mWorldPos(0.0f) {
        // TODO
    }

    Wheel(unsigned int flags);
    ~Wheel();
    bool UpdatePosition(const UMath::Vector3 &body_av, const UMath::Vector3 &body_lv, const UMath::Matrix4 &body_matrix, const UMath::Vector3 &cog,
                        float dT, float wheel_radius, bool usecache, const WCollider *collider, float vehicle_height);
    void UpdateSurface(const SimSurface &surface);
    bool InitPosition(const IRigidBody &rb, float maxcompression);
    void Reset();
    void UpdateTime(float dT);

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

    void SetVelocity(const UMath::Vector3 &v) {
        mVelocity = v;
    }

    void IncAirTime(float dT) {
        mAirTime += dT;
    }

    void SetAirTime(float f) {
        mAirTime = f;
    }

    const UMath::Vector3 &GetLocalArm() const {
        return mLocalArm;
    }

    void SetLocalArm(UMath::Vector3 &arm) {
        mLocalArm = arm;
    }

    const UMath::Vector3 &GetWorldArm() const {
        return mWorldArm;
    }

    float GetCompression() const {
        return mCompression;
    }

    void SetCompression(float c) {
        mCompression = UMath::Max(c, 0.0f);
    }

    const SimSurface &GetSurface() const {
        return mSurface;
    }

    const UMath::Vector3 &GetVelocity() const {
        return mVelocity;
    }

    bool IsOnGround() const {
        return mCompression > 0.0f;
    }

  protected:
    // total size: 0xC4
    WWorldPos mWorldPos;      // offset 0x0, size 0x3C
    UMath::Vector4 mNormal;   // offset 0x3C, size 0x10
    UMath::Vector3 mPosition; // offset 0x4C, size 0xC
    unsigned int mFlags;      // offset 0x58, size 0x4
    UMath::Vector3 mForce;    // offset 0x5C, size 0xC
    float mAirTime;           // offset 0x68, size 0x4
    UMath::Vector3 mLocalArm; // offset 0x6C, size 0xC
    float mCompression;       // offset 0x78, size 0x4
    UMath::Vector3 mWorldArm; // offset 0x7C, size 0xC
    int pad;                  // offset 0x88, size 0x4
    UMath::Vector3 mVelocity; // offset 0x8C, size 0xC
    int pad2;                 // offset 0x98, size 0x4
    SimSurface mSurface;      // offset 0x9C, size 0x14
    float mSurfaceStick;      // offset 0xB0, size 0x4
    UMath::Vector4 mIntegral; // offset 0xB4, size 0x10
};

#endif
