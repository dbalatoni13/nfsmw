#include "Speed/Indep/Src/World/WGridManagedDynamicElem.h"

#include "Speed/Indep/Libs/Support/Utility/UTLVector.h"
#include "Speed/Indep/Src/World/Common/WGrid.h"
#include "Speed/Indep/Src/World/WCollider.h"

UTL::Std::list<WGridManagedDynamicElem, _type_list> WGridManagedDynamicElem::fgManagedDynamicElemList;

WGridManagedDynamicElem::WGridManagedDynamicElem(UMath::Vector4 *dstPosRad, const UMath::Vector4 *srcPosRad, const WGridNodeElem &elem)
    : fType(1),                               //
      fLastPosRad(UMath::Vector4::kIdentity), //
      fElem(elem),                            //
      fDstPosRad(dstPosRad),                  //
      fSrcPosRad(srcPosRad),                  //
      fDstCInst(nullptr),                     //
      fDstTrigger(nullptr) {
    fPosRad = srcPosRad;
}

void WGridManagedDynamicElem::Update() {
    switch (fType) {
        case 1: {
            *reinterpret_cast<UMath::Vector3 *>(fDstPosRad) = *reinterpret_cast<const UMath::Vector3 *>(fSrcPosRad);

            if ((UMath::Abs(fPosRad->x - fLastPosRad.x) > 0.01f) || (UMath::Abs(fPosRad->z - fLastPosRad.z) > 0.01f)) {
                UMath::Vector4 tempPosRad = UMath::Vector4Make(*UMath::Vector4To3(fSrcPosRad), fDstPosRad->w);

                AddElem(&fLastPosRad, &tempPosRad, fElem.fType, fElem.fInd);
                WCollider::InvalidateIntersectingColliders(tempPosRad);
                fLastPosRad = tempPosRad;
            }
            break;
        }
        case 2: {
            {
                // is this a bug?
                UMath::Matrix4 m;
#ifdef EA_BUILD_A124
                UMath::OrthoInverse(m);
#else
                OrthoInverse(m);
#endif

                // TODO scheduling issue
                reinterpret_cast<UMath::Vector3 &>(fDstCInst->fInvMatRow0Width) = reinterpret_cast<UMath::Vector3 &>(m[0]);
                reinterpret_cast<UMath::Vector3 &>(fDstCInst->fInvMatRow2Length) = reinterpret_cast<UMath::Vector3 &>(m[2]);
                reinterpret_cast<UMath::Vector3 &>(fDstCInst->fInvPosRadius) = reinterpret_cast<UMath::Vector3 &>(m[3]);

                if ((UMath::Abs(fPosRad->x - fLastPosRad.x) > 0.01f) || (UMath::Abs(fPosRad->z - fLastPosRad.z) > 0.01f)) {
                    UMath::Vector4 tempPosRad =
                        UMath::Vector4Make(*UMath::Vector4To3(fPosRad), reinterpret_cast<UMath::Vector4 &>(fDstCInst->fInvPosRadius).w);

                    AddElem(&fLastPosRad, &tempPosRad, fElem.fType, fElem.fInd);
                    WCollider::InvalidateIntersectingColliders(tempPosRad);
                    fLastPosRad = tempPosRad;
                }
            }
            break;
        }
        case 3: {
            {
                UMath::Matrix4 m;

                reinterpret_cast<UMath::Vector3 &>(fDstTrigger->fMatRow0Width) = reinterpret_cast<UMath::Vector3 &>(m[0]);
                reinterpret_cast<UMath::Vector3 &>(fDstTrigger->fMatRow2Length) = reinterpret_cast<UMath::Vector3 &>(m[2]);
                v3add(1, reinterpret_cast<const UMath::Vector3 *>(&m[3]), UMath::Vector4To3(&fOffsetVec),
                      UMath::Vector4To3(&fDstTrigger->fPosRadius));

                if ((UMath::Abs(fPosRad->x - fLastPosRad.x) > 0.01f) || (UMath::Abs(fPosRad->z - fLastPosRad.z) > 0.01f)) {
                    UMath::Vector4 tempPosRad = UMath::Vector4Make(*UMath::Vector4To3(fPosRad), fDstTrigger->fPosRadius.w);
                    UMath::Vector4 offsetPosRad = tempPosRad;

                    v3add(1, UMath::Vector4To3(&tempPosRad), UMath::Vector4To3(&fOffsetVec), UMath::Vector4To3(&offsetPosRad));

                    UMath::Vector4 offsetLastPosRad = fLastPosRad;
                    v3add(1, UMath::Vector4To3(&fLastPosRad), UMath::Vector4To3(&fOffsetVec), UMath::Vector4To3(&offsetLastPosRad));

                    AddElem(&offsetLastPosRad, &offsetPosRad, fElem.fType, fElem.fInd);
                    fLastPosRad = tempPosRad;
                }
            }
            break;
        }
    }
}

void WGridManagedDynamicElem::AddElem(const UMath::Vector4 *oldPosRad, const UMath::Vector4 *newPosRad, WGridNode_ElemType type, uintptr_t dataInd) {
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
            if (n == nullptr) {
                continue;
            }
            bool inList = false;
            {
                int count = n->GetElemTypeCount(type);
                const unsigned int *typeInds = n->GetElemTypePtr(type);
                for (int cnt = 0; cnt < count; cnt++) {
                    if (typeInds[cnt] == dataInd) {
                        inList = true;
                        break;
                    }
                }
            }
            if (!inList) {
                n->AddDynamic(dataInd, type);
            }
        }
    }
}

void WGridManagedDynamicElem::UpdateElems() {
    for (UTL::Std::list<WGridManagedDynamicElem, _type_list>::iterator eIter = fgManagedDynamicElemList.begin();
         eIter != fgManagedDynamicElemList.end(); ++eIter) {
        (*eIter).Update();
    }
}
