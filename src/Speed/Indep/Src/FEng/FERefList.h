#ifndef FENG_FEREFLIST_H
#define FENG_FEREFLIST_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FEList.h"

// total size: 0x10
class FERefList {
  public:
    FERefList() {
        head = nullptr;
        tail = nullptr;
        bIsReference = false;
    }
    virtual ~FERefList() {
        if (!bIsReference) {
            Purge();
        }
    }

    inline bool IsReference() const { return bIsReference; }
    inline FERefList* GetRefSource() { return pRef; }
    inline FEMinNode* GetHead() const { return bIsReference ? pRef->GetHead() : head; }
    inline FEMinNode* GetTail() const { return bIsReference ? pRef->GetTail() : tail; }
    inline bool IsListEmpty() const { return GetHead() == nullptr; }

    void ReferenceList(FERefList* pList);
    inline void AddTail(FEMinNode* n) { AddNode(tail, n); }
    void AddNode(FEMinNode* insertpoint, FEMinNode* node);
    bool IsInList(FEMinNode* node) const;
    int ElementNumber(FEMinNode* node);
    FEMinNode* RemNode(FEMinNode* node);
    FEMinNode* RemHead();
    FEMinNode* RemTail();
    FEMinNode* FindNode(unsigned long ordinalnumber) const;
    inline void Purge() {
        FEMinNode* cmn = RemHead();
        while (cmn) {
            delete cmn;
            cmn = RemHead();
        }
    }
    unsigned long GetNumElements();

  private:
    bool bIsReference; // offset 0x0, size 0x1
  protected:
    union {
        FERefList *pRef; // offset 0x0, size 0x4
        FEMinNode *head; // offset 0x0, size 0x4
    }; // offset 0x4, size 0x4
    FEMinNode *tail; // offset 0x8, size 0x4
};

#endif
