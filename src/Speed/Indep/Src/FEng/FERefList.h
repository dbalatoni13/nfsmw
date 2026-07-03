#ifndef FEREFLIST_H_
#define FEREFLIST_H_

#include "FEList.h"

// File: speed/indep/src/feng/FERefList.h
// total size: 0x10
// Decl: speed/indep/src/feng/FERefList.h:15
class FERefList {
  private:
    bool bIsReference; // offset 0x0, size 0x1, Decl: speed/indep/src/feng/FERefList.h:18

  protected:
    union {
        FERefList *pRef; // offset 0x0, size 0x4
        FEMinNode *head; // offset 0x0, size 0x4
    };

    FEMinNode *tail; // offset 0x8, size 0x4, Decl: speed/indep/src/feng/FERefList.h:25

  private:
    bool IsInList(FEMinNode *node) const;

  public:
    FERefList() { // Decl: speed/indep/src/feng/FERefList.h:28
        head = nullptr;
        tail = nullptr;
        bIsReference = false;
    }
    virtual ~FERefList() { // Decl: speed/indep/src/feng/FERefList.h:29
        if (!bIsReference) {
            Purge();
        }
    }

    void ReferenceList(FERefList *pList);

    bool IsReference() const {
        return bIsReference;
    }

    FERefList *GetRefSource() { // Decl: speed/indep/src/feng/FERefList.h:33
        return pRef;
    }

    FEMinNode *RemHead();

    FEMinNode *RemTail();

    FEMinNode *RemNode(FEMinNode *node);

    i32 ElementNumber(FEMinNode *node);

    FEMinNode *GetHead() const {
        return bIsReference ? pRef->GetHead() : head;
    }
    FEMinNode *GetTail() const { // Decl: speed/indep/src/feng/FERefList.h:42
        return bIsReference ? pRef->GetTail() : tail;
    }
    void AddHead(FEMinNode *n) {} // Decl: speed/indep/src/feng/FERefList.h:43

    void AddTail(FEMinNode *n) { // Decl: speed/indep/src/feng/FERefList.h:45
        AddNode(tail, n);
    }
    void AddNode(FEMinNode *insertpoint, FEMinNode *node);
    FEMinNode *FindNode(u32 ordinalnumber) const;

    void Purge() { // Decl: speed/indep/src/feng/FERefList.h:49
        FEMinNode *cmn = RemHead();
        while (cmn) {
            delete cmn;
            cmn = RemHead();
        }
    }

    bool IsListEmpty() const { // Decl: speed/indep/src/feng/FERefList.h:62
        return GetHead() == nullptr;
    }

    u32 GetNumElements();
};

#endif
