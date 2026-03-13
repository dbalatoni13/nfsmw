#include "Speed/Indep/Src/FEng/FERefList.h"

FEMinNode* FERefList::RemHead() {
    FEMinNode* n = head;

    if (n) {
        RemNode(n);
    }

    return n;
}

unsigned long FERefList::GetNumElements() {
    unsigned long Count = 0;
    FEMinNode* pNode;

    if (bIsReference) {
        pNode = pRef->GetHead();
    } else {
        pNode = head;
    }

    while (pNode) {
        pNode = pNode->GetNext();
        Count++;
    }

    return Count;
}
