#ifndef PHYSICS_DYANMICS_COLLISION_H
#define PHYSICS_DYANMICS_COLLISION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

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

#endif
