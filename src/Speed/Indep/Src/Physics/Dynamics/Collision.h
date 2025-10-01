#ifndef PHYSICS_DYANMICS_COLLISION_H
#define PHYSICS_DYANMICS_COLLISION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
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

    void Set(const UMath::Matrix4 &orient, const UMath::Vector3 &position, const UMath::Vector3 &dimension, Shape shape, const UMath::Vector3 &delta);

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

}; // namespace Collision
}; // namespace Dynamics

#endif
