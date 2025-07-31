#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

// UNSOLVED, matches on MWCC
bNode *bList::GetNode(int ordinal_number) {
    int n = 0;
    bNode *node = this->GetHead();
    while (node != this->EndOfList() && (n != ordinal_number)) {
        node = node->GetNext();
        n++;
    }
    return node;
}

// UNSOLVED, matches on MWCC
int bList::TraversebList(bNode *match_node) {
    bNode *node = this->GetHead();
    int n = 0;
    while (node != this->EndOfList()) {
        if (node == match_node) {
            return n + 1;
        }
        node = node->GetNext();
        n++;
    }
    if (match_node) {
        n = 0;
    }
    return n;
}

void bList::Sort(SortFunc check_flip) {
    bNode *node = this->GetHead();
    bNode *next_node = node->GetNext();
    int did_swap = 0;
    while (node != this->EndOfList() && next_node != this->EndOfList()) {
        if (check_flip(node, next_node) == 0) {
            did_swap++;
            next_node->Remove();
            next_node->AddBefore(node);
        }
        node = next_node;
        next_node = next_node->GetNext();
    }
    if (did_swap != 0) {
        this->MergeSort(check_flip);
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
