
#include "FEGroup.h"

FEGroup::FEGroup(const FEGroup &Object, bool bCloneChildren, bool bReference) : FEObject(Object, bReference) {
    if (bCloneChildren) {
        FEObject *pObject = static_cast<FEObject *>(Object.Children.GetHead());
        while (pObject != nullptr) {
            AddObject(pObject->Clone(bReference));

            pObject = pObject->GetNext();
        }
    }
}

FEObject *FEGroup::FindChildRecursive(u32 NameHash) const {
    FEObject *pChild = GetFirstChild();
    while (pChild != nullptr) {
        if (pChild->NameHash == NameHash) {
            return pChild;
        }
        FEObject *groupChild = nullptr;
        if (pChild->Type == FE_Group) {
            groupChild = static_cast<FEGroup *>(pChild)->FindChildRecursive(NameHash);
        }
        if (groupChild != nullptr) {
            return groupChild;
        }
        pChild = pChild->GetNext();
    }
    return nullptr;
}
