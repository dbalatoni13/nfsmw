#ifndef WORLD_WCOLISSION_TRI_H
#define WORLD_WCOLISSION_TRI_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "./WCollision.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

struct WCollisionTri {
    // total size: 0x30
    UMath::Vector3 fPt0;                  // offset 0x0, size 0xC
    const struct SimSurface *fSurfaceRef; // offset 0xC, size 0x4
    UMath::Vector3 fPt1;                  // offset 0x10, size 0xC
    unsigned int fFlags;                  // offset 0x1C, size 0x4
    UMath::Vector3 fPt2;                  // offset 0x20, size 0xC
    WSurface fSurface;                    // offset 0x2C, size 0x2
    unsigned short PAD;                   // offset 0x2E, size 0x2
};

DECLARE_VECTOR_TYPE(WCollisionWarnVector);
DECLARE_VECTOR_TYPE(WCollisionVector);

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
