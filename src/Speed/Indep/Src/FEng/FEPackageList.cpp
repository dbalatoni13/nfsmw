#include "Speed/Indep/Src/FEng/fengine_full.h"

void FEPackageList::AddPackage(FEPackage* pPack) {
    FEPackage* pNode = GetLastPackage();
    for (;;) {
        if (!pNode) {
            break;
        }
        if (pNode->GetPriority() <= pPack->GetPriority()) {
            break;
        }
        pNode = pNode->GetPrev();
    }
    Packages.AddNode(static_cast<FEMinNode*>(static_cast<FENode*>(pNode)), static_cast<FEMinNode*>(static_cast<FENode*>(pPack)));
}

bool FEPackageList::RemovePackage(FEPackage* pPack) {
    FEPackage* pNode = GetFirstPackage();
    while (pNode) {
        if (pNode == pPack) {
            Packages.RemNode(static_cast<FEMinNode*>(static_cast<FENode*>(pPack)));
            return true;
        }
        pNode = pNode->GetNext();
    }
    return false;
}

void FEPackageList::ReplaceParentLinks(const FEPackage* pParent, const FEPackage* pReplacement) {
    FEPackage* pNode = GetFirstPackage();
    while (pNode) {
        if (pNode->GetParentPackage() == pParent) {
            pNode->SetParentPackage(const_cast<FEPackage*>(pReplacement));
        }
        pNode = pNode->GetNext();
    }
}
