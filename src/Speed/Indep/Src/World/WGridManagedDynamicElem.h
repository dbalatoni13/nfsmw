#ifndef WORLD_WGRIDMANAGEDDYNAMICELEM_H
#define WORLD_WGRIDMANAGEDDYNAMICELEM_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Miscellaneous/CARP.h"
#include "Speed/Indep/Src/Physics/Dynamics/Collision.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

typedef uintptr_t WGridNodeElemTag;

enum WGridNode_ElemType {
    WGrid_kInstance = 0,
    WGrid_kTrigger = 1,
    WGrid_kObject = 2,
    WGrid_kRoadSegment = 3,
    WGrid_kElemTypeCount = 4,
};

// total size: 0x8
struct WGridNodeElem {
    WGridNodeElem() {}
    WGridNodeElem(WGridNodeElemTag ind, WGridNode_ElemType type)
        : fInd(ind), //
          fType(type) {}

    WGridNodeElemTag fInd;    // offset 0x0, size 0x4
    WGridNode_ElemType fType; // offset 0x4, size 0x4
};

// total size: 0x40
class WGridManagedDynamicElem {
  public:
    static void UpdateElems();
    static void AddElem(const UMath::Vector4 *oldPosRad, const UMath::Vector4 *newPosRad, WGridNode_ElemType type, uintptr_t dataInd);

    static UTL::Std::list<WGridManagedDynamicElem, _type_list> &DynamicElemList() {
        return fgManagedDynamicElemList;
    }

    static void Init() {
        fgManagedDynamicElemList.clear();
    }

    static void Shutdown() {
        fgManagedDynamicElemList.clear();
    }

    WGridManagedDynamicElem();
    WGridManagedDynamicElem(UMath::Vector4 *dstPosRad, const UMath::Vector4 *srcPosRad, const WGridNodeElem &elem);
    WGridManagedDynamicElem(CollisionInstance *dst, const WGridNodeElem &elem);
    WGridManagedDynamicElem(CARP::Trigger *dst, const UMath::Vector4 &offsetVec, const WGridNodeElem &elem);

    void Update();

    unsigned int fType;               // offset 0x0, size 0x4
    const UMath::Vector4 *fPosRad;    // offset 0x4, size 0x4
    UMath::Vector4 fOffsetVec;        // offset 0x8, size 0x10
    UMath::Vector4 fLastPosRad;       // offset 0x18, size 0x10
    WGridNodeElem fElem;              // offset 0x28, size 0x8
    const UMath::Vector4 *fSrcPosRad; // offset 0x30, size 0x4
    UMath::Vector4 *fDstPosRad;       // offset 0x34, size 0x4
    CollisionInstance *fDstCInst;     // offset 0x38, size 0x4
    CARP::Trigger *fDstTrigger;       // offset 0x3C, size 0x4
    static UTL::Std::list<WGridManagedDynamicElem, _type_list> fgManagedDynamicElemList;
};

#endif
