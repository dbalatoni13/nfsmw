#ifndef WORLD_WGRIDMANAGEDDYNAMICELEM_H
#define WORLD_WGRIDMANAGEDDYNAMICELEM_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

enum WGridNode_ElemType {
    WGrid_kInstance = 0,
    WGrid_kTrigger = 1,
    WGrid_kObject = 2,
    WGrid_kRoadSegment = 3,
    WGrid_kElemTypeCount = 4,
};

// total size: 0x8
struct WGridNodeElem {
    unsigned int fInd;        // offset 0x0, size 0x4
    WGridNode_ElemType fType; // offset 0x4, size 0x4
};

// total size: 0x40
class WGridManagedDynamicElem {
  public:
    WGridManagedDynamicElem();
    WGridManagedDynamicElem(UMath::Vector4 *dstPosRad, const UMath::Vector4 *srcPosRad, const WGridNodeElem &elem);
    WGridManagedDynamicElem(struct CollisionInstance *dst, const WGridNodeElem &elem);
    WGridManagedDynamicElem(struct Trigger *dst, const UMath::Vector4 &offsetVec, const WGridNodeElem &elem);

    void Update();

    static void AddElem(const UMath::Vector4 *oldPosRad, const UMath::Vector4 *newPosRad, WGridNode_ElemType type, unsigned int dataInd);
    static void Shutdown();
    static void UpdateElems();

    static std::list<WGridManagedDynamicElem, UTL::Std::Allocator<WGridManagedDynamicElem, _type_list> > &DynamicElemList() {
        return fgManagedDynamicElemList;
    }

    static std::list<WGridManagedDynamicElem, UTL::Std::Allocator<WGridManagedDynamicElem, _type_list> > fgManagedDynamicElemList;

    unsigned int fType;                  // offset 0x0, size 0x4
    const UMath::Vector4 *fPosRad;       // offset 0x4, size 0x4
    UMath::Vector4 fOffsetVec;           // offset 0x8, size 0x10
    UMath::Vector4 fLastPosRad;          // offset 0x18, size 0x10
    WGridNodeElem fElem;                 // offset 0x28, size 0x8
    const UMath::Vector4 *fSrcPosRad;    // offset 0x30, size 0x4
    UMath::Vector4 *fDstPosRad;          // offset 0x34, size 0x4
    struct CollisionInstance *fDstCInst; // offset 0x38, size 0x4
    struct Trigger *fDstTrigger;         // offset 0x3C, size 0x4
};

#endif
