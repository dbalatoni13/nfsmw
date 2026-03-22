#include "Speed/Indep/Src/FEng/FEMsgTargetList.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

void FEMsgTargetList::Allocate(unsigned long NewAlloc) {
    if (NewAlloc == 0) {
        if (pTargets) {
            delete[] reinterpret_cast<char*>(pTargets);
        }
        pTargets = nullptr;
        Count = 0;
        Alloc = 0;
    } else if (NewAlloc != Alloc) {
        FEObject** pNewTargets = static_cast<FEObject**>(FEngMalloc(NewAlloc * sizeof(FEObject*), nullptr, 0));
        if (NewAlloc < Alloc) {
            FEngMemCpy(pNewTargets, pTargets, NewAlloc * sizeof(FEObject*));
        } else {
            FEngMemCpy(pNewTargets, pTargets, Alloc * sizeof(FEObject*));
        }
        if (pTargets) {
            delete[] reinterpret_cast<char*>(pTargets);
        }
        pTargets = pNewTargets;
    }
    Alloc = NewAlloc;
}

void FEMsgTargetList::AppendTarget(FEObject* pObject) {
    if (Count == Alloc) {
        Allocate(Count + 1);
    }
    pTargets[Count++] = pObject;
}