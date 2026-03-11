#include "Speed/Indep/Src/World/WGridManagedDynamicElem.h"

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

void WGridManagedDynamicElem::UpdateElems() {
    std::list<WGridManagedDynamicElem, UTL::Std::Allocator<WGridManagedDynamicElem, _type_list> >::iterator eIter;
    for (eIter = fgManagedDynamicElemList.begin(); eIter != fgManagedDynamicElemList.end(); ++eIter) {
        (*eIter).Update();
    }
}
