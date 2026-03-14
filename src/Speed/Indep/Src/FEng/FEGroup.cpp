#include <new>

#include "FEGroup.h"
#include "FEngStandard.h"

FEGroup::FEGroup(const FEGroup& Object, bool bCloneChildren, bool bReference)
    : FEObject(Object, bReference)
{
    if (bCloneChildren) {
        for (FEObject* pChild = static_cast<FEObject*>(Object.Children.GetHead()); pChild; pChild = pChild->GetNext()) {
            FEObject* pClone = pChild->Clone(bReference);
            Children.AddNode(Children.GetTail(), pClone);
        }
    }
}

FEGroup::~FEGroup() {}

FEObject* FEGroup::FindChildRecursive(unsigned long NameHash) const {
    FEObject* pChild = static_cast<FEObject*>(Children.GetHead());
    while (pChild) {
        if (pChild->NameHash == NameHash) {
            return pChild;
        }
        FEObject* pFound = nullptr;
        if (pChild->Type == FE_Group) {
            pFound = static_cast<FEGroup*>(pChild)->FindChildRecursive(NameHash);
        }
        if (pFound) {
            return pFound;
        }
        pChild = pChild->GetNext();
    }
    return nullptr;
}

FEObject* FEGroup::Clone(bool bReference) {
    FEGroup* pGroup = static_cast<FEGroup*>(FEngMalloc(sizeof(FEGroup), nullptr, 0));
    if (pGroup) {
        new (pGroup) FEGroup(*this, true, bReference);
    }
    return pGroup;
}
