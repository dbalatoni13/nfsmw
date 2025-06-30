#ifndef WORLD_WCOLLISION_H
#define WORLD_WCOLLISION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/Src/Physics/Dynamics/Collision.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

struct WSurface : CollisionSurface {};

struct WCollisionArticle {
    // total size: 0x10
    unsigned short fNumStrips;         // offset 0x0, size 0x2
    unsigned short fStripsSize;        // offset 0x2, size 0x2
    unsigned short fNumEdges;          // offset 0x4, size 0x2
    unsigned short fEdgesSize;         // offset 0x6, size 0x2
    unsigned char fResolvedFlag;       // offset 0x8, size 0x1
    unsigned char fNumSurfaces;        // offset 0x9, size 0x1
    unsigned short fSurfacesSize;      // offset 0xA, size 0x2
    unsigned short fIntermediatObjInd; // offset 0xC, size 0x2
    short fFlags;                      // offset 0xE, size 0x2
};

struct WCollisionBarrier {
    // total size: 0x20
    bVector4 fPts[2]; // offset 0x0, size 0x20
};

struct WCollisionBarrierListEntry {
    // total size: 0x28
    WCollisionBarrier fB;                 // offset 0x0, size 0x20
    const struct Collection *fSurfaceRef; // offset 0x20, size 0x4
    float fDistanceToSq;                  // offset 0x24, size 0x4
};

struct WCollisionObject : public CollisionObject {
    // total size: 0x70
};

struct WCollisionInstance : public CollisionInstance {
    // total size: 0x40
};

struct WorldCollisionInfo {
    bVector4 fCollidePt;
    bVector4 fNormal;
    WCollisionBarrierListEntry fBle;
    WCollisionObject *fObj;
    float fDist;
    unsigned char fAnimated;
    unsigned char fType;
    unsigned short fPad;
    WCollisionInstance *fCInst;
};

#endif
