#ifndef SLINKLIST_H
#define SLINKLIST_H

// total size: 0x8
// Decl: 24
#include <cstddef>
typedef struct CListDNode {
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

private:
    CListDNode *pnext; // offset 0x0, size 0x4
    CListDNode *pprev; // offset 0x4, size 0x4
} CListDNode;

// total size: 0x4
// Decl: 96
class CListDStack {
  private:
    CListDNode *phead; // offset 0x0, size 0x4, Decl: 101

  public:
    CListDStack() : phead(0) {} // Decl: 104

    void Reset() {} // Decl: 106

    bool IsEmpty() {} // Decl: 108

    CListDNode *GetHead() {} // Decl: 110

    void Push(CListDNode *pnode) {} // Decl: 112

    CListDNode *Pop() {} // Decl: 129

    void Remove(CListDNode *pnode) {
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
