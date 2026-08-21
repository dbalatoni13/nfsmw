#ifndef SLINKLIST_H
#define SLINKLIST_H

// total size: 0x8
// Decl: 24
#include <cstddef>
typedef struct CListDNode {
private:
    CListDNode *pnext; // offset 0x0, size 0x4
    CListDNode *pprev; // offset 0x4, size 0x4

public:
    inline void SetNext(CListDNode *pnode) {
        this->pnext = pnode;
    }

    inline void SetPrev(CListDNode *pnode) {
        this->pprev = pnode;
    }

    inline CListDNode *GetNext() {
        return this->pnext;
    }

    inline CListDNode *GetPrev() {
        return this->pprev;
    }
} CListDNode;

// total size: 0x4
// Decl: 96
class CListDStack {
private:
    CListDNode *phead; // offset 0x0, size 0x4, Decl: 101

public:
    CListDStack() : phead(NULL) {} // Decl: 104

    inline void Reset() {
        this->phead = NULL;
    } // Decl: 106

    inline bool IsEmpty() {
        return this->phead == NULL;
    } // Decl: 108

    inline CListDNode *GetHead() { return phead; } // Decl: 110

    inline void Push(CListDNode *pnode) {
        pnode->SetNext(this->phead);
        pnode->SetPrev(NULL);
        if (this->phead != NULL) {
            this->phead->SetPrev(pnode);
        }
        this->phead = pnode;
    } // Decl: 112

    inline CListDNode *Pop() {
        CListDNode *pnode = this->phead;
        if (pnode != NULL) {
            this->phead = pnode->GetNext();
            if (this->phead != NULL) {
                this->phead->SetPrev(NULL);
            }
        }
        return pnode;
    } // Decl: 129

    inline void Remove(CListDNode *pnode) {
        if (pnode == this->phead) {
            this->phead = pnode->GetNext();
        }

        if (pnode->GetPrev() != NULL) {
            CListDNode *ptempnode = pnode->GetPrev();
            ptempnode->SetNext(pnode->GetNext());
        }

        if (pnode->GetNext() != NULL) {
            CListDNode *ptempnode = pnode->GetNext();
            ptempnode->SetPrev(pnode->GetPrev());
        }
    } // Decl: 148
};

#endif
