#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

bNode *bList::GetNode(int ordinal_number) {
    int n = 0;
    bNode *node = this->GetHead();
    while (node != this->EndOfList() && (n != ordinal_number)) {
        n++;
        node = node->GetNext();
    }
    return node;
}

int bList::TraversebList(bNode *match_node) {
    bNode *node = this->GetHead();
    int n = 0;
    while (node != this->EndOfList()) {
        if (node == match_node) {
            return n + 1;
        }
        n++;
        node = node->GetNext();
    }
    if (match_node) {
        n = 0;
    }
    return n;
}

// UNSOLVED
void bList::Sort(SortFunc check_flip) {
    bNode *node = this->GetHead();
    bNode *next_node = node->GetNext();
    int did_swap = 0;

    while (node != this->EndOfList() && next_node != this->EndOfList()) {
        if (check_flip(node, next_node) == 0) {
            did_swap++;
            next_node->Remove();
            next_node->AddBefore(node);
            next_node = node->GetNext();
            continue;
        }
        node = next_node;
        next_node = next_node->GetNext();
    }
    if (did_swap != 0) {
        this->MergeSort(check_flip);
    }
}

void bList::MergeSort(SortFunc cmp) {
    bNode *list = this->HeadNode.Next;
    if (list == &this->HeadNode) {
        return;
    }

    int insize = 1;
    list->Prev = this->HeadNode.Prev;
    this->HeadNode.Prev->Next = list;

    while (true) {
        bNode *oldhead = nullptr;
        bNode *tail = nullptr;
        int nmerges = 0;
        bNode *p = list;

        while (p != nullptr) {
            nmerges++;

            int psize = 0;
            bNode *q = p;
            for (int i = 0; i < insize; i++) {
                psize++;
                q = q->Next;
                if (q == list) {
                    q = nullptr;
                    break;
                }
            }
            int qsize = insize;
            while (psize > 0 || (qsize > 0 && q != nullptr)) {
                bNode *e;
                if (psize > 0 && (qsize == 0 || q == nullptr || cmp(p, q) > 0)) {
                    psize--;
                    e = p;
                    p = p->Next;
                    if (p == list) {
                        p = nullptr;
                    }
                } else {
                    qsize--;
                    e = q;
                    q = q->Next;
                    if (q == list) {
                        q = nullptr;
                    }
                }

                if (tail != nullptr) {
                    tail->Next = e;
                    e->Prev = tail;
                } else {
                    oldhead = e;
                    e->Prev = nullptr;
                }

                tail = e;
            }

            p = q;
        }

        tail->Next = oldhead;
        oldhead->Prev = tail;
        if (nmerges < 2) {
            oldhead->Prev = &this->HeadNode;
            this->HeadNode.Next = oldhead;
            this->HeadNode.Prev = tail;
            tail->Next = &this->HeadNode;
            return;
        }

        insize *= 2;
        list = oldhead;
    }
}

SlotPool *bPNodeSlotPool = nullptr;
BOOL bPListWantToClose = false;

void bPListInit(int num_expected_bpnodes) {
    if (!bPNodeSlotPool) {
        bPNodeSlotPool = bNewSlotPool(12, num_expected_bpnodes, "bPNode SlotPool", GetVirtualMemoryAllocParams());
        bPListWantToClose = false;
    }
}

void bPListClose() {
    if (bPNodeSlotPool) {
        if (bPNodeSlotPool->IsEmpty()) {
            bDeleteSlotPool(bPNodeSlotPool);
            bPNodeSlotPool = nullptr;
        } else {
            bPListWantToClose = true;
        }
    }
}

void *bPNode::Malloc() {
    if (!bPNodeSlotPool) {
        bPListInit(256);
    }
    return bOMalloc(bPNodeSlotPool);
}

void bPNode::Free(void *ptr) {
    bFree(bPNodeSlotPool, ptr);
    if (bPListWantToClose) {
        bPListClose();
    }
}
