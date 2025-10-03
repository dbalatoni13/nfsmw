#ifndef WORLD_WCOLLISION_MGR_H
#define WORLD_WCOLLISION_MGR_H

#include "dolphin/types.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "WCollision.h"
#include "WCollisionTri.h"

class WCollisionMgr {
  public:
    // total size: 0x58
    struct WorldCollisionInfo {
        UMath::Vector4 fCollidePt;        // offset 0x0, size 0x10
        UMath::Vector4 fNormal;           // offset 0x10, size 0x10
        WCollisionBarrierListEntry fBle;  // offset 0x20, size 0x28
        const WCollisionObject *fObj;     // offset 0x48, size 0x4
        float fDist;                      // offset 0x4C, size 0x4
        unsigned char fAnimated;          // offset 0x50, size 0x1
        unsigned char fType;              // offset 0x51, size 0x1
        unsigned short fPad;              // offset 0x52, size 0x2
        const WCollisionInstance *fCInst; // offset 0x54, size 0x4

        // UNSOLVED
        WorldCollisionInfo() {
            fCollidePt = UMath::Vector4::kIdentity;
            fNormal = UMath::Vector4::kIdentity;
            // TODO maybe this is in WCollisionBarrierListEntry's constructor?
            // std::memset(&fBle.fB, 0, sizeof(fBle.fB));
            fCInst = nullptr;
            fDist = 0.0f;
            fBle.fSurfaceRef = nullptr;
            fBle.fDistanceToSq = 0.0f;
            fObj = nullptr;
            fAnimated = false;
            fPad = 0;
        }

        bool HitSomething() const {}
    };

    class ICollisionHandler {
      public:
        ICollisionHandler() {}

        virtual bool OnWCollide(const WorldCollisionInfo &cInfo, const bVector3 &cPoint, void *userdata);
    };

    unsigned int fSurfaceExclusionMask; // offset 0x0, size 0x4
    unsigned int fPrimitiveMask;        // offset 0x4, size 0x4

    bool GetWorldHeightAtPointRigorous(const UMath::Vector3 &pt, float &height, UMath::Vector3 *normal);
    bool Collide(Dynamics::Collision::Geometry *geom, const WCollisionBarrierList *barrierList, ICollisionHandler *results, void *userdata,
                 bool force_single_sided);
    bool Collide(Dynamics::Collision::Geometry *geom, const WCollisionInstanceCacheList *instanceList, ICollisionHandler *results, void *userdata);

    WCollisionMgr(unsigned int surfaceExclMask, unsigned int primitiveExclMask) {
        this->fSurfaceExclusionMask = surfaceExclMask;
        this->fPrimitiveMask = primitiveExclMask;
    }

    ~WCollisionMgr() {}
};

#endif
