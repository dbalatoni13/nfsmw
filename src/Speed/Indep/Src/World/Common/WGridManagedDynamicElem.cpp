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

void WGridManagedDynamicElem::Update() {
    if (fType == 1) {
        UMath::Vector4To3(*fDstPosRad) = UMath::Vector4To3(*fSrcPosRad);

        if (UMath::Abs(fPosRad->x - fLastPosRad.x) <= 0.01f) {
            if (UMath::Abs(fPosRad->z - fLastPosRad.z) <= 0.01f) {
                return;
            }
        }
        {
            UMath::Vector4 tempPosRad = UMath::Vector4Make(UMath::Vector4To3(*fSrcPosRad), fDstPosRad->w);

            AddElem(&fLastPosRad, &tempPosRad, static_cast<WGridNode_ElemType>(fElem.fType), fElem.fInd);
            WCollider::InvalidateIntersectingColliders(tempPosRad);
            fLastPosRad = tempPosRad;
        }
    } else if (fType == 2) {
        {
            UMath::Matrix4 m;
            m[0] = fPosRad[0];
            m[2] = fPosRad[2];
            m[3] = fPosRad[3];
            OrthoInverse(m);

            UMath::Vector4To3(reinterpret_cast<UMath::Vector4 &>(fDstCInst->fInvMatRow0Width)) = UMath::Vector4To3(m[0]);
            UMath::Vector4To3(reinterpret_cast<UMath::Vector4 &>(fDstCInst->fInvMatRow2Length)) = UMath::Vector4To3(m[1]);
            UMath::Vector4To3(reinterpret_cast<UMath::Vector4 &>(fDstCInst->fInvPosRadius)) = UMath::Vector4To3(m[2]);

            if (UMath::Abs(fPosRad->x - fLastPosRad.x) <= 0.01f) {
                if (UMath::Abs(fPosRad->z - fLastPosRad.z) <= 0.01f) {
                    return;
                }
            }
            {
                UMath::Vector4 tempPosRad = UMath::Vector4Make(UMath::Vector4To3(*fPosRad), reinterpret_cast<UMath::Vector4 &>(fDstCInst->fInvPosRadius).w);

                AddElem(&fLastPosRad, &tempPosRad, static_cast<WGridNode_ElemType>(fElem.fType), fElem.fInd);
                WCollider::InvalidateIntersectingColliders(tempPosRad);
                fLastPosRad = tempPosRad;
            }
        }
    } else if (fType == 3) {
        {
            UMath::Matrix4 m;
            m[0] = fPosRad[0];
            m[1] = fPosRad[1];
            m[2] = fPosRad[2];
            m[3] = fPosRad[3];

            UMath::Vector4To3(fDstTrigger->fMatRow0Width) = UMath::Vector4To3(m[0]);
            UMath::Vector4To3(fDstTrigger->fMatRow2Length) = UMath::Vector4To3(m[2]);
            v3add(1, reinterpret_cast<const UMath::Vector3 *>(&m[3]),
                  reinterpret_cast<const UMath::Vector3 *>(&fOffsetVec),
                  reinterpret_cast<UMath::Vector3 *>(&fDstTrigger->fPosRadius));

            if (UMath::Abs(fPosRad->x - fLastPosRad.x) <= 0.01f) {
                if (UMath::Abs(fPosRad->z - fLastPosRad.z) <= 0.01f) {
                    return;
                }
            }
            {
                UMath::Vector4 tempPosRad = UMath::Vector4Make(UMath::Vector4To3(*fPosRad), fDstTrigger->fPosRadius.w);
                UMath::Vector4 offsetPosRad = tempPosRad;
                UMath::Vector4 offsetLastPosRad = fLastPosRad;

                v3add(1, reinterpret_cast<const UMath::Vector3 *>(&tempPosRad),
                      reinterpret_cast<const UMath::Vector3 *>(&fOffsetVec),
                      reinterpret_cast<UMath::Vector3 *>(&offsetPosRad));
                v3add(1, reinterpret_cast<const UMath::Vector3 *>(&fLastPosRad),
                      reinterpret_cast<const UMath::Vector3 *>(&fOffsetVec),
                      reinterpret_cast<UMath::Vector3 *>(&offsetLastPosRad));

                AddElem(&offsetLastPosRad, &offsetPosRad, static_cast<WGridNode_ElemType>(fElem.fType), fElem.fInd);
                fLastPosRad = tempPosRad;
            }
        }
    }
}

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
