#ifndef _FELIST
#define _FELIST

#include "types.h"

// total size: 0xC
struct FEMinNode {
    friend class FERefList;
    friend class FEMinList;
    friend class FEngine;
    friend class FESlotPool;
    friend class FEMultiPool;
    friend struct FEPackage;

protected:
    FEMinNode* next; // offset 0x0, size 0x4
    FEMinNode* prev; // offset 0x4, size 0x4

public:
    inline FEMinNode()
        : next(reinterpret_cast<FEMinNode*>(0xABADCAFE)), //
          prev(reinterpret_cast<FEMinNode*>(0xABADCAFE)) {
    }
    virtual ~FEMinNode() {}

    inline FEMinNode* GetNext() const { return next; }
    inline FEMinNode* GetPrev() const { return prev; }
};

// total size: 0x10
struct FEMinList {
    typedef bool (*CheckFlipFunc)(FEMinNode*, FEMinNode*);

private:
    unsigned int numElements; // offset 0x0, size 0x4

protected:
    FEMinNode* head; // offset 0x4, size 0x4
    FEMinNode* tail; // offset 0x8, size 0x4

public:
    inline FEMinList() {
        head = nullptr;
        tail = nullptr;
        numElements = 0;
    }
    virtual ~FEMinList() { Purge(); }

    inline FEMinNode* GetHead() const { return head; }
    inline FEMinNode* GetTail() const { return tail; }
    inline void AddHead(FEMinNode* n) { AddNode(nullptr, n); }
    inline void AddTail(FEMinNode* n) { AddNode(tail, n); }
    inline void Purge() {
        FEMinNode* cmn = RemHead();
        while (cmn) {
            delete cmn;
            cmn = RemHead();
        }
    }
    inline bool IsListEmpty() const { return numElements == 0; }
    inline unsigned long GetNumElements() const { return numElements; }

    void AddNode(FEMinNode* insertpoint, FEMinNode* node);
    bool IsInList(FEMinNode* node) const;
    int ElementNumber(FEMinNode* node);
    FEMinNode* RemNode(FEMinNode* node);
    FEMinNode* RemHead();
    FEMinNode* RemTail();
    FEMinNode* FindNode(unsigned long ordinalnumber) const;
    void Swap(FEMinNode* n, FEMinNode* m);
    void Sort(bool (*CheckFlip)(FEMinNode*, FEMinNode*));
};

unsigned long FEHashUpper(const char* String);

#endif
