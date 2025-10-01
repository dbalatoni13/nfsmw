#ifndef WORLD_WCOLLISION_MGR_H
#define WORLD_WCOLLISION_MGR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/World/WCollision.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

class WCollisionMgr {
  public:
    struct WorldCollisionInfo {
        UMath::Vector4 fCollidePt;
        UMath::Vector4 fNormal;
        WCollisionBarrierListEntry fBle;
        const WCollisionObject *fObj;
        float fDist;
        unsigned char fAnimated;
        unsigned char fType;
        uint16_t fPad;
        const WCollisionInstance *fCInst;

        WorldCollisionInfo() {}
        bool HitSomething() const {}
    };

    class ICollisionHandler {
      public:
        ICollisionHandler() {}

        virtual bool OnWCollide(const struct WorldCollisionInfo &cInfo, const bVector3 &cPoint, void *userdata);
    };

    unsigned int fSurfaceExclusionMask; // offset 0x0, size 0x4
    unsigned int fPrimitiveMask;        // offset 0x4, size 0x4

    bool GetWorldHeightAtPointRigorous(const UMath::Vector3 &pt, float &height, UMath::Vector3 *normal);

    WCollisionMgr(unsigned int surfaceExclMask, unsigned int primitiveExclMask) {
        this->fSurfaceExclusionMask = surfaceExclMask;
        this->fPrimitiveMask = primitiveExclMask;
    }

    ~WCollisionMgr() {}
};

#endif
