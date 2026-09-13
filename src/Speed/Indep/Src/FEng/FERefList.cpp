#include "Speed/Indep/Src/FEng/FERefList.h"

void FERefList::ReferenceList(FERefList *pList) {
    if (pList != nullptr) {
        if (!bIsReference) {
            Purge();
        }

        pRef = pList;
        bIsReference = true;
    } else {
        this->pRef = pList;
        this->tail = nullptr;
        this->bIsReference = false;
    }
}

void FERefList::AddNode(FEMinNode *insertpoint, FEMinNode *node) {
    if (node == nullptr) {
        return;
    }

    if (insertpoint != nullptr) {
        node->next = insertpoint->next;
        if (node->next != nullptr) {
            node->next->prev = node;
        }
        node->prev = insertpoint;
        insertpoint->next = node;
    } else {
        node->next = head;
        if (node->next != nullptr) {
            node->next->prev = node;
        }
        node->prev = nullptr;
        head = node;
    }

    if (tail == insertpoint) {
        tail = node;
    }
}

FEMinNode *FERefList::RemNode(FEMinNode *node) {
    if (node != nullptr) {
        if (node == head) {
            head = node->next;
        }

        if (node == tail) {
            tail = node->prev;
        }

        if (node->prev != nullptr) {
            node->prev->next = node->next;
        }

        if (node->next != nullptr) {
            node->next->prev = node->prev;
        }

        node->next = reinterpret_cast<FEMinNode *>(LIST_MAGIC);
        node->prev = reinterpret_cast<FEMinNode *>(LIST_MAGIC);
    }

    return node;
}

FEMinNode *FERefList::RemHead() {
    FEMinNode *n = head;

    if (n != nullptr) {
        RemNode(n);
    }

    return n;
}

u32 FERefList::GetNumElements() {
    u32 Count = 0;
    FEMinNode *pNode = GetHead();

    while (pNode != nullptr) {
        pNode = pNode->GetNext();
        Count++;
    }

    return Count;
}
