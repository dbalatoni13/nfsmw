#ifndef WHEEL_H
#define WHEEL_H

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"

// total size: 0xC4
class Wheel {
  public:
    enum eWheelFlags {
        WF_SMOOTHING = 1,
    };

    USE_FASTALLOC(Wheel);

    Wheel() : mWorldPos(0.0f) {
        // TODO
    }

    Wheel(unsigned int flags);
    ~Wheel();

    void UpdateTime(float dT);
    void Reset();
    bool InitPosition(const IRigidBody &rb, float maxcompression);
    bool UpdatePosition(const UMath::Vector3 &body_av, const UMath::Vector3 &body_lv, const UMath::Matrix4 &body_matrix, const UMath::Vector3 &cog,
                        float dT, float wheel_radius, bool usecache, const WCollider *collider, float vehicle_height);
    float GetRoughness() const;

    const UMath::Vector4 &GetNormal() const {
        return this->mNormal;
    }

    const UMath::Vector3 &GetPosition() const {
        return this->mPosition;
    }

    const UMath::Vector3 &GetVelocity() const {
        return this->mVelocity;
    }

    void SetVelocity(const UMath::Vector3 &v) {
        this->mVelocity = v;
    }

    void SetY(float y) {
        this->mPosition.y = y;
    }

    void SetPosition(UMath::Vector3 &p) {
        this->mPosition = p;
    }

    float GetCompression() const {
        return this->mCompression;
    }

    void SetCompression(float c) {
        this->mCompression = UMath::Max(c, 0.0f);
    }

    bool IsOnGround() const {
        return this->mCompression > 0.0f;
    }

    const UMath::Vector3 &GetForce() const {
        return this->mForce;
    }

    void SetForce(const UMath::Vector3 &f) {
        this->mForce = f;
    }

    const WWorldPos &GetWorld() const {
        return mWorldPos;
    }

    void ClearWorldPos() {
        // TODO
    }

    const SimSurface &GetSurface() const {
        return this->mSurface;
    }

    void SetSurface(const SimSurface &surface) {
        this->mSurface = surface;
    }

    void UpdateSurface(const SimSurface &surface);

    void SetAirTime(float f) {
        this->mAirTime = f;
    }

    void IncAirTime(float dT) {
        this->mAirTime += dT;
    }

    float GetAirTime() {
        return this->mAirTime;
    }

    void SetLocalArm(UMath::Vector3 &arm) {
        this->mLocalArm = arm;
    }

    const UMath::Vector3 &GetLocalArm() const {
        return this->mLocalArm;
    }

    const UMath::Vector3 &GetWorldArm() const {
        return this->mWorldArm;
    }

  protected:
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
