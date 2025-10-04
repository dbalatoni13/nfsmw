#ifndef PHYSICS_DYANMICS_COLLISION_H
#define PHYSICS_DYANMICS_COLLISION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"
#include "Speed/Indep/Src/Physics/Dynamics.h"
#include "Speed/Indep/Src/Physics/Dynamics/Inertia.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribCollection.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

struct CollisionSurface {
    unsigned char fSurface;
    unsigned char fFlags;
};

struct CollisionObject {
    // total size: 0x70
    bVector4 fPosRadius;               // offset 0x0, size 0x10
    bVector4 fDimensions;              // offset 0x10, size 0x10
    unsigned char fType;               // offset 0x20, size 0x1
    unsigned char fShape;              // offset 0x21, size 0x1
    unsigned short fFlags;             // offset 0x22, size 0x2
    unsigned short fRenderInstanceInd; // offset 0x24, size 0x2
    CollisionSurface fSurface;         // offset 0x26, size 0x2
    float fPAD[2];                     // offset 0x28, size 0x8
    bMatrix4 fMat;                     // offset 0x30, size 0x40
};

struct CollisionInstance {
    // total size: 0x40
    bVector4 fInvMatRow0Width;                         // offset 0x0, size 0x10
    unsigned short fIterStamp;                         // offset 0x10, size 0x2
    unsigned short fFlags;                             // offset 0x12, size 0x2
    float fHeight;                                     // offset 0x14, size 0x4
    unsigned short fGroupNumber;                       // offset 0x18, size 0x2
    unsigned short fRenderInstanceInd;                 // offset 0x1A, size 0x2
    const struct WCollisionArticle *fCollisionArticle; // offset 0x1C, size 0x4
    bVector4 fInvMatRow2Length;                        // offset 0x20, size 0x10
    bVector4 fInvPosRadius;                            // offset 0x30, size 0x10
};

struct CollisionPacket {
    // total size: 0x30
    UMath::Vector3 lever;                  // offset 0x0, size 0xC
    const Attrib::Collection *bodysurface; // offset 0xC, size 0x4
    UMath::Vector3 normal;                 // offset 0x10, size 0xC
    float penetration;                     // offset 0x1C, size 0x4
    UMath::Vector3 arm;                    // offset 0x20, size 0xC
    const Attrib::Collection *surface;     // offset 0x2C, size 0x4
};

namespace Dynamics {
namespace Collision {

class Geometry {
    // total size: 0xB0
  public:
    enum Shape {
        UNKNOWN = 0,
        BOX = 1,
        SPHERE = 2,
        MAXSHAPES = 3,
    };

    static bool FindIntersection(const Geometry *A, const Geometry *B, Geometry *result);

    Geometry();
    void Set(const UMath::Matrix4 &orient, const UMath::Vector3 &position, const UMath::Vector3 &dimension, Shape shape, const UMath::Vector3 &delta);

    const UMath::Vector3 &GetPosition() const {
        return *reinterpret_cast<const UMath::Vector3 *>(&mPosition);
    }

    const UMath::Matrix4 &GetOrientation() const {
        return *reinterpret_cast<const UMath::Matrix4 *>(&mNormal);
    }

    const UMath::Vector3 &GetCollisionPoint() const {
        return *reinterpret_cast<const UMath::Vector3 *>(&mCollision_point);
    }

    const UMath::Vector3 &GetCollisionNormal() const {
        return *reinterpret_cast<const UMath::Vector3 *>(&mCollision_normal);
    }

    const UMath::Vector3 &GetDimension() const {
        return *reinterpret_cast<const UMath::Vector3 *>(mDimension);
    }

    bool PenetratesOther() const {
        return mPenetratesOther != 0;
    }

    float GetOverlap() const {
        return mOverlap;
    }

  private:
    UMath::Vector4 mPosition;         // offset 0x0, size 0x10
    UMath::Vector4 mNormal[3];        // offset 0x10, size 0x30
    UMath::Vector4 mExtent[3];        // offset 0x40, size 0x30
    UMath::Vector4 mCollision_point;  // offset 0x70, size 0x10
    UMath::Vector4 mCollision_normal; // offset 0x80, size 0x10
    float mDimension[3];              // offset 0x90, size 0xC
    unsigned short mShape;            // offset 0x9C, size 0x2
    unsigned short mPenetratesOther;  // offset 0x9E, size 0x2
    UMath::Vector3 mDelta;            // offset 0xA0, size 0xC
    float mOverlap;                   // offset 0xAC, size 0x4
};

// total size: 0x8
struct Friction {
    enum State {
        Dynamic = 2,
        Static = 1,
        None = 0,
    };

    Friction() {
        mUk = 0.0f;
        mUs = 0.0f;
    }

    Friction(float uk, float us) {
        mUk = uk;
        mUs = us;
    }

    void operator*=(float scale) {
        mUk *= scale;
        mUs *= scale;
    }

    float mUk; // offset 0x0, size 0x4
    float mUs; // offset 0x4, size 0x4
};

// total size: 0x20
struct Plane {
    Plane() : friction(0.0f, 0.0f) {}

    UVector3 point;    // offset 0x0, size 0xC
    UVector3 normal;   // offset 0xC, size 0xC
    Friction friction; // offset 0x18, size 0x8
};

// total size: 0x114
class Moment {
  public:
    Moment(const UMath::Matrix4 &orientation, float mass, const UMath::Vector3 &inertia, const UMath::Vector3 &cg, const UMath::Vector3 &linearVel,
           const UMath::Vector3 &angularVel, const UMath::Vector3 &position);
    Moment(const IEntity *rb);
    void SetInertia(const UMath::Vector3 &inertiaP);
    void SetMass(float mass);
    void SetCG(const UMath::Vector3 &cg);
    bool React(const Plane &plane, int nSteps);
    bool React(Moment &other, const Plane &plane, int nSteps);
    bool React(Moment &other, const struct Joint &joint, int nSteps);
    bool React(const struct Joint &joint, int nSteps);

    const UVector3 &GetLinearVelocity() const {
        return mLinearVelocity;
    }

    void SetLinearVelocity(const UVector3 &vel) {
        mLinearVelocity = vel;
    }

    const UVector3 &GetAngularVelocity() const {
        return mAngularVelocity;
    }

    void SetPosition(const UMath::Vector3 &pos) {
        mPosition = pos;
    }

    void SetInertiaScale(const UMath::Vector3 &scale) {
        mInertialScale = scale;
    }

    bool IsImmobile() const {
        return mImmobile;
    }

    float GetBreakingForce() {
        return mBrakingForce;
    }

    void MakeImmobile(bool b, float force) {
        mImmobile = b;
        mBrakingForce = force;
    }

    float GetMass() const {
        return mMass;
    }

    float GetOOMass() const {
        return mMass > 0.0f ? 1.0f / mMass : 0.0f;
    }

    const UVector3 &GetClosingVelocity() const {
        return mClosingVelocity;
    }

    const UVector3 &GetSlidingVelocity() const {
        return mSlidingVelocity;
    }

    const UVector3 &GetForce() const {
        return mForce;
    }

    Friction::State GetFrictionState() const {
        return mFrictionState;
    }

    void SetElasticity(float e) {
        mElasticity = e;
    }

    void SetFixedCG(bool b) {
        mFixedCG = b;
    }

  private:
    UMath::Matrix4 mOrientation;      // offset 0x0, size 0x40
    UMath::Matrix4 mOrientationInv;   // offset 0x40, size 0x40
    struct Inertia::Tensor mInertiaP; // offset 0x80, size 0xC
    UVector3 mCG;                     // offset 0x8C, size 0xC
    UVector3 mLinearVelocity;         // offset 0x98, size 0xC
    UVector3 mAngularVelocity;        // offset 0xA4, size 0xC
    UVector3 mPosition;               // offset 0xB0, size 0xC
    UVector3 mInertialScale;          // offset 0xBC, size 0xC
    bool mImmobile;                   // offset 0xC8, size 0x1
    float mBrakingForce;              // offset 0xCC, size 0x4
    float mMass;                      // offset 0xD0, size 0x4
    float mMassInv;                   // offset 0xD4, size 0x4
    UVector3 mClosingVelocity;        // offset 0xD8, size 0xC
    UVector3 mSlidingVelocity;        // offset 0xE4, size 0xC
    UVector3 mForce;                  // offset 0xF0, size 0xC
    UVector3 mFriction;               // offset 0xFC, size 0xC
    Friction::State mFrictionState;   // offset 0x108, size 0x4
    float mElasticity;                // offset 0x10C, size 0x4
    bool mFixedCG;                    // offset 0x110, size 0x1
};

}; // namespace Collision
}; // namespace Dynamics

#endif
