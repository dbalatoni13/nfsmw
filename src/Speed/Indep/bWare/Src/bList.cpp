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
        bNode *old_tail = nullptr;
        bNode *tail = nullptr;
        int num_merges = 0;
        bNode *p = list;

        while (p != nullptr) {
            num_merges++;

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
            while (true) {
                bNode *e;
                bNode *next = q;
                if (psize < 1) {
                    if ((qsize < 1) || (next == nullptr)) {
                        break;
                    }

                    qsize--;
                    e = next;
                    q = next->Next;
                    if (q == list) {
                        q = nullptr;
                    }
                } else {
                    if ((qsize != 0) && (next != nullptr) && (cmp(p, next) > 0)) {
                        qsize--;
                        e = next;
                        q = next->Next;
                        if (q == list) {
                            q = nullptr;
                        }
                    } else {
                        psize--;
                        e = p;
                        p = p->Next;
                        if (p == list) {
                            p = nullptr;
                        }
                    }
                }

                if (tail != nullptr) {
                    tail->Next = e;
                    e->Prev = tail;
                } else {
                    old_tail = e;
                    e->Prev = nullptr;
                }

                tail = e;
            }

            p = q;
        }

        tail->Next = old_tail;
        old_tail->Prev = tail;
        if (num_merges < 2) {
            old_tail->Prev = &this->HeadNode;
            this->HeadNode.Next = old_tail;
            this->HeadNode.Prev = tail;
            tail->Next = &this->HeadNode;
            return;
        }

        insize *= 2;
        list = old_tail;
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
