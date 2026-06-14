#ifndef __WGridManagedDynamicElem_H
#define __WGridManagedDynamicElem_H

#include "Speed/Indep/Libs/Support/Miscellaneous/CARP.h"
#include "Speed/Indep/Src/Physics/Dynamics/Collision.h"
#include "Speed/Indep/Src/World/Common/WGridNode.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

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
