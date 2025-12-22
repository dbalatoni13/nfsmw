#ifndef WORLD_WCOLLIDER_H
#define WORLD_WCOLLIDER_H

#include "types.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/World/WCollisionTri.h"

class WCollider : public UTL::Collections::Listable<WCollider, 100> {
  public:
    // total size: 0x9C
    enum eColliderShape {
        kColliderShape_FORCE_INT = 2147483647,
        kColliderShape_Cylinder = 1,
        kColliderShape_Sphere = 0,
    };

    struct InstValidationRec {
        // total size: 0x8
        const struct WCollisionArticle *fArt;            // offset 0x0, size 0x4
        const struct WCollisionArticle *const *fInstArt; // offset 0x4, size 0x4
    };

    struct InstValidationRecList : public UTL::Std::vector<WCollider::InstValidationRec, _type_vector> {
        // total size: 0x10
    };

    static void Destroy(WCollider *col);

    void Clear();
    bool IsEmpty() const;
    void Refresh(const UMath::Vector3 &pt, float radius, bool predictiveSizing);

    WCollisionInstanceCacheList &GetInstanceList() {
        return fInstanceCacheList;
    }

    ALIGN_PS2(16) UMath::Vector3 fRequestedPosition;     // offset 0x4, size 0xC
    float fRequestedRadius;                              // offset 0x10, size 0x4
    ALIGN_PS2(16) UMath::Vector3 fLastRequestedPosition; // offset 0x14, size 0xC
    float fLastRequestedRadius;                          // offset 0x20, size 0x4
    ALIGN_PS2(16) UMath::Vector3 fPosition;              // offset 0x24, size 0xC
    float fRadius;                                       // offset 0x30, size 0x4
    ALIGN_PS2(16) UMath::Vector3 fLastRefreshedPosition; // offset 0x34, size 0xC
    WCollisionInstanceCacheList fInstanceCacheList;      // offset 0x40, size 0x10
    WCollisionBarrierList fBarrierList;                  // offset 0x50, size 0x10
    WCollisionTriList fTriList;                          // offset 0x60, size 0x14
    WCollisionObjectList fObbList;                       // offset 0x74, size 0x10
    bool fRegionInitialized;                             // offset 0x84, size 0x1
    eColliderShape fColliderShape;                       // offset 0x88, size 0x4
    unsigned int fTypeMask;                              // offset 0x8C, size 0x4
    unsigned int fRefCount;                              // offset 0x90, size 0x4
    unsigned int fWorldID;                               // offset 0x94, size 0x4
    unsigned int fExclusionFlags;                        // offset 0x98, size 0x4
};

#endif
