#include "Speed/Indep/Src/World/WGridManagedDynamicElem.h"

#include "Speed/Indep/Libs/Support/Utility/UEALibs.hpp"
#include "Speed/Indep/Libs/Support/Utility/UTLVector.h"

#include "Speed/Indep/Src/Physics/Dynamics/Collision.h"
#include "Speed/Indep/Src/World/Common/WGrid.h"
#include "Speed/Indep/Src/World/WCollider.h"
#include "Speed/Indep/Src/World/WTrigger.h"

void OrthoInverse(UMath::Matrix4 &m);

std::list<WGridManagedDynamicElem, UTL::Std::Allocator<WGridManagedDynamicElem, _type_list> > WGridManagedDynamicElem::fgManagedDynamicElemList;

WGridManagedDynamicElem::WGridManagedDynamicElem(UMath::Vector4 *dstPosRad, const UMath::Vector4 *srcPosRad, const WGridNodeElem &elem)
    : fType(1), //
      fPosRad(srcPosRad), //
      fLastPosRad(UMath::Vector4::kIdentity), //
      fElem(elem), //
      fSrcPosRad(srcPosRad), //
      fDstPosRad(dstPosRad), //
      fDstCInst(nullptr), //
      fDstTrigger(nullptr) {}

void WGridManagedDynamicElem::AddElem(const UMath::Vector4 *oldPosRad, const UMath::Vector4 *newPosRad,
                                      WGridNode_ElemType type, unsigned int dataInd) {
    if (oldPosRad != nullptr) {
        UTL::FastVector<unsigned int, 16> nodeInds;
        WGrid::Get().FindNodes(UMath::Vector4To3(*oldPosRad), oldPosRad->w, nodeInds);
        for (int i = 0; i < static_cast<int>(nodeInds.size()); i++) {
            WGridNode *n = WGrid::Get().GetNode(nodeInds[i]);
            if (n != nullptr) {
                n->RemoveDynamic(dataInd, type);
            }
        }
    }

    if (newPosRad != nullptr) {
        UTL::FastVector<unsigned int, 16> nodeInds;
        WGrid::Get().FindNodes(UMath::Vector4To3(*newPosRad), newPosRad->w, nodeInds);
        for (int i = 0; i < static_cast<int>(nodeInds.size()); i++) {
            WGridNode *n = WGrid::Get().GetNode(nodeInds[i]);
            if (n != nullptr) {
                int count = n->GetElemTypeCount(type);
                bool inList = false;
                const unsigned int *typeInds = n->GetElemTypePtr(type);
                for (int cnt = 0; cnt < count; cnt++) {
                    if (typeInds[cnt] == dataInd) {
                        inList = true;
                        break;
                    }
                }
                if (!inList) {
                    n->AddDynamic(dataInd, type);
                }
            }
        }
    }
}


void WGridManagedDynamicElem::UpdateElems() {
    for (std::list<WGridManagedDynamicElem, UTL::Std::Allocator<WGridManagedDynamicElem, _type_list> >::iterator eIter = fgManagedDynamicElemList.begin(); eIter != fgManagedDynamicElemList.end(); ++eIter) {
        (*eIter).Update();
    }
}
