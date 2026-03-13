#ifndef FENG_FEREFLIST_H
#define FENG_FEREFLIST_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FEList.h"

// total size: 0x10
class FERefList {
  public:
    FERefList() : bIsReference(false), head(nullptr), tail(nullptr) {}
    virtual ~FERefList() {}

    inline bool IsReference() const { return bIsReference; }
    inline FERefList* GetRefSource() { return pRef; }
    FEMinNode* GetHead() const;
    inline FEMinNode* GetTail() const { return bIsReference ? pRef->GetTail() : tail; }
    inline bool IsListEmpty() const { return GetHead() == nullptr; }

    void ReferenceList(FERefList* pList);
    void AddNode(FEMinNode* insertpoint, FEMinNode* node);
    bool IsInList(FEMinNode* node) const;
    int ElementNumber(FEMinNode* node);
    FEMinNode* RemNode(FEMinNode* node);
    FEMinNode* RemHead();
    FEMinNode* RemTail();
    FEMinNode* FindNode(unsigned long ordinalnumber) const;
    void Purge();
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
