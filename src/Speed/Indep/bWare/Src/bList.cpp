#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"

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

// TODO
void bList::AddTail(bList *list) {}

// TODO
void bList::AddHead(bList *list) {}

// STRIPPED
bNode *bList::AddSorted(SortFunc check_flip, bNode *node) {}

// UNSOLVED, it matches in ProStreet..
// r76b: sin andamio. El DWARF-1 del original da this=r29, check_flip=r27,
// node=r30, next_node=r31, did_swap=r28 y NO tiene la local `cmp` que
// teniamos (era invencion nuestra; fuera). Lo que falta es el reparto:
// global_alloc nos da check_flip=r28 / did_swap=r27, al reves, y sched1
// adelanta el `lwz node,0(this)` por delante del `li did_swap,0`.
// allocno_compare (global.c) = floor_log2(n_refs)*n_refs/live_length:
// check_flip 4 refs/32 insns = 2500, did_swap 6/54 = 2222, y en el original
// gana did_swap. Probadas y descartadas (todas dan lo mismo): did_swap
// declarado antes/despues de node y next_node; if/else en vez de continue;
// condicion invertida; for(;;) y do/while(1) con breaks; next_node calculado
// solo dentro del bucle; `if (did_swap == 0) return;`; `if (did_swap)`.
void bList::Sort(SortFunc check_flip) {
    int did_swap = 0;
    bNode *node = this->GetHead();
    bNode *next_node = node->GetNext();

    while (node != this->EndOfList() && next_node != this->EndOfList()) {
        if (check_flip(node, next_node) == 0) {
            next_node->Remove();
            next_node->AddBefore(node);
            did_swap++;
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
    if (IsEmpty()) {
        return;
    }

    bNode *list = GetHead();
    list->Prev = GetTail();
    GetTail()->Next = list;

    bNode *p;
    bNode *q;
    bNode *e;
    bNode *tail;
    bNode *oldhead;
    int insize = 1;
    int nmerges;
    int psize;
    int qsize;
    int i;

    while (true) {
        p = list;
        oldhead = p;
        list = nullptr;
        tail = nullptr;
        nmerges = 0;

        while (p) {
            nmerges++;

            q = p;
            psize = 0;
            for (i = 0; i < insize; i++) {
                psize++;
                q = q->Next == oldhead ? nullptr : q->Next;
                if (q == nullptr)
                    break;
            }
            qsize = insize;
            while (psize > 0 || (qsize > 0 && q)) {
                if (psize == 0) {
                    e = q;
                    q = q->Next;
                    qsize--;
                    if (q == oldhead) {
                        q = nullptr;
                    }
                } else if (qsize == 0 || !q) {
                    e = p;
                    p = p->Next;
                    psize--;
                    if (p == oldhead) {
                        p = nullptr;
                    }
                } else if (cmp(p, q) > 0) {
                    psize--;
                    e = p;
                    p = p->Next;
                    if (p == oldhead) {
                        p = nullptr;
                    }
                } else {
                    qsize--;
                    e = q;
                    q = q->Next;
                    if (q == oldhead) {
                        q = nullptr;
                    }
                }

                if (tail) {
                    tail->Next = e;
                } else {
                    list = e;
                }

                e->Prev = tail;
                tail = e;
            }
            p = q;
        }

        tail->Next = list;
        list->Prev = tail;
        if (nmerges <= 1) {
            list->Prev = &this->HeadNode;
            this->HeadNode.Next = list;
            this->HeadNode.Prev = tail;
            tail->Next = &this->HeadNode;
            return;
        }

        insize *= 2;
    }
}

SlotPool *bPNodeSlotPool = nullptr;
int bPListWantToClose = false;
int bPListAllocationNumber = 0;

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
