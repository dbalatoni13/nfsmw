#include "Speed/Indep/Src/FEng/FERefList.h"

static FEMinNode* const kRemovedNode = reinterpret_cast<FEMinNode*>(0xABADCAFE);

FEMinNode* FERefList::GetHead() const {
    return bIsReference ? pRef->GetHead() : head;
}

void FERefList::ReferenceList(FERefList* pList) {
    FEMinNode* n;

    if (pList) {
        if (!bIsReference) {
            while ((n = RemHead()) != nullptr) {
                delete n;
            }
        }

        pRef = pList;
        bIsReference = true;
    } else {
        *reinterpret_cast<unsigned long*>(this) = reinterpret_cast<unsigned long>(pList);
        *reinterpret_cast<FERefList**>(reinterpret_cast<char*>(this) + 0x4) = pList;
        *reinterpret_cast<FEMinNode**>(reinterpret_cast<char*>(this) + 0x8) = nullptr;
    }
}

void FERefList::AddNode(FEMinNode* insertpoint, FEMinNode* node) {
    FEMinNode* n;

    if (!node) {
        return;
    }

    if (insertpoint) {
        n = insertpoint->next;
        node->next = n;
        if (n) {
            n->prev = node;
        }
        node->prev = insertpoint;
        insertpoint->next = node;
    } else {
        n = head;
        node->next = n;
        if (n) {
            n->prev = node;
        }
        node->prev = nullptr;
        head = node;
    }

    if (tail == insertpoint) {
        tail = node;
    }
}

FEMinNode* FERefList::RemNode(FEMinNode* node) {
    FERefList* pList = this;
    FEMinNode* ret = node;

    if (!ret) {
        return ret;
    }

    if (ret == pList->head) {
        pList->head = ret->next;
    }

    if (ret == pList->tail) {
        pList->tail = ret->prev;
    }

    if (ret->prev) {
        ret->prev->next = ret->next;
    }

    if (ret->next) {
        ret->next->prev = ret->prev;
    }

    ret->prev = kRemovedNode;
    ret->next = kRemovedNode;
    return ret;
}

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

void FERefList::Purge() {
    FEMinNode* n;

    while ((n = RemHead()) != nullptr) {
        delete n;
    }
}

FERefList::~FERefList() {
    if (!bIsReference) {
        Purge();
    }
}
