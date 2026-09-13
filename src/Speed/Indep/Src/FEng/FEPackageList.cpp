#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/FEng/FEPackageList.h"

void FEPackageList::AddPackage(FEPackage *pPack) {
    FEPackage *pNode = GetLastPackage();
    while (pNode != nullptr && pNode->GetPriority() > pPack->GetPriority()) {
        pNode = pNode->GetPrev();
    }
    Packages.AddNode(pNode, pPack);
}

bool FEPackageList::RemovePackage(FEPackage *pPack) {
    FEPackage *pNode = GetFirstPackage();
    while (pNode != nullptr) {
        if (pNode == pPack) {
            Packages.RemNode(pPack);
            return true;
        }
        pNode = pNode->GetNext();
    }
    return false;
}

void FEPackageList::ReplaceParentLinks(const FEPackage *pParent, const FEPackage *pReplacement) {
    FEPackage *pNode = GetFirstPackage();
    while (pNode != nullptr) {
        if (pNode->GetParentPackage() == pParent) {
            pNode->SetParentPackage(const_cast<FEPackage *>(pReplacement));
        }
        pNode = pNode->GetNext();
    }
}
