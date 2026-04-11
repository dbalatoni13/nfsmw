#ifndef WORLD_WCOLISSION_TRI_H
#define WORLD_WCOLISSION_TRI_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "./WCollision.h"
#include "WSurfaceTypes.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

// TODO
void v3unit(const UMath::Vector3 *v, UMath::Vector3 *result);

// total size: 0x30
struct WCollisionTri {
    WCollisionTri() {}

    UMath::Vector3 fPt0;                  // offset 0x0, size 0xC
    const struct SimSurface *fSurfaceRef; // offset 0xC, size 0x4
    UMath::Vector3 fPt1;                  // offset 0x10, size 0xC
    unsigned int fFlags;                  // offset 0x1C, size 0x4
    UMath::Vector3 fPt2;                  // offset 0x20, size 0xC
    WSurface fSurface;                    // offset 0x2C, size 0x2
    unsigned short PAD;                   // offset 0x2E, size 0x2

    void GetNormal(UMath::Vector3 *norm) const {
        UMath::Vector3 vecX;
        UMath::Vector3 vecZ;
        UMath::Vector3 normal;

        vecZ.x = fPt1.x - fPt0.x;
        vecZ.y = fPt1.y - fPt0.y;
        vecZ.z = fPt1.z - fPt0.z;

        vecX.x = fPt0.x - fPt2.x;
        vecX.y = fPt0.y - fPt2.y;
        vecX.z = fPt0.z - fPt2.z;
        UMath::Cross(vecZ, vecX, normal);

        if (normal.x == 0.0f && normal.y == 0.0f && normal.z == 0.0f) {
            norm->x = 0.0f;
            norm->y = 1.0f;
            norm->z = 0.0f;
        } else {
            v3unit(&normal, norm);
        }
    }
};

DECLARE_CONTAINER_TYPE(WCollisionWarnVector);
DECLARE_CONTAINER_TYPE(WCollisionVector);

// TODO move these?
template <typename T, std::size_t N> class WCollisionWarnVector : public UTL::Std::vector<T, _type_WCollisionWarnVector> {};

struct WCollisionInstanceCacheList : public WCollisionWarnVector<const WCollisionInstance *, 80> {
    // total size: 0x10
};

template <typename T> class WCollisionVector : public UTL::Std::vector<T, _type_WCollisionVector> {};

struct WCollisionBarrierList : public WCollisionVector<WCollisionBarrierListEntry> {
    // total size: 0x10
};

struct WCollisionTriBlock : public WCollisionVector<WCollisionTri> {};

struct WCollisionTriList : public WCollisionVector<WCollisionTriBlock *> {};

struct WCollisionObjectList : public WCollisionVector<const WCollisionObject *> {};

#endif
