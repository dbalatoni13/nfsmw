
#include "FEGroup.h"

FEGroup::FEGroup(const FEGroup &Object, bool bCloneChildren, bool bReference) : FEObject(Object, bReference) {
    if (bCloneChildren) {
        for (FEObject *pObject = static_cast<FEObject *>(Object.Children.GetHead()); pObject; pObject = pObject->GetNext()) {
            AddObject(pObject->Clone(bReference));
        }
    }
}

FEObject *FEGroup::FindChildRecursive(u32 NameHash) const {
    for (FEObject *pChild = static_cast<FEObject *>(Children.GetHead()); pChild; pChild = pChild->GetNext()) {
        if (pChild->NameHash == NameHash) {
            return pChild;
        }
        FEObject *groupChild = nullptr;
        if (pChild->Type == FE_Group) {
            groupChild = static_cast<FEGroup *>(pChild)->FindChildRecursive(NameHash);
        }
        if (groupChild) {
            return groupChild;
        }
    }
    return nullptr;
}
