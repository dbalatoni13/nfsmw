#ifndef _FELIST
#define _FELIST

#include <types.h>

// total size: 0xC
class FEMinNode {
    friend class FERefList;
    friend class FEMinList;
    friend class FEngine;
    friend class FESlotPool;
    friend class FEMultiPool;
    friend struct FEPackage;

  public:
    FEMinNode() {
        next = reinterpret_cast<FEMinNode *>(0xABADCAFE);
        prev = reinterpret_cast<FEMinNode *>(0xABADCAFE);
    }
    virtual ~FEMinNode() {}

    FEMinNode *GetNext() const {
        return next;
    }
    FEMinNode *GetPrev() const {
        return prev;
    }

  protected:
    FEMinNode *next; // offset 0x0, size 0x4
    FEMinNode *prev; // offset 0x4, size 0x4
};

// total size: 0x10
class FEMinList {
  public:
    FEMinList() {
        head = nullptr;
        tail = nullptr;
        numElements = 0;
    }
    virtual ~FEMinList() {
        Purge();
    }

  private:
    bool IsInList(FEMinNode *node) const;

  public:
    FEMinNode *RemNode(FEMinNode *node);
    FEMinNode *RemHead();
    FEMinNode *RemTail();
    i32 ElementNumber(FEMinNode *node);

    inline FEMinNode *GetHead() const {
        return head;
    }
    inline FEMinNode *GetTail() const {
        return tail;
    }
    inline void AddHead(FEMinNode *n) {
        AddNode(nullptr, n);
    }
    inline void AddTail(FEMinNode *n) {
        AddNode(tail, n);
    }
    void AddNode(FEMinNode *insertpoint, FEMinNode *node);
    FEMinNode *FindNode(u32 ordinalnumber) const;

    void Swap(FEMinNode *n, FEMinNode *m);

    inline void Purge() {
        FEMinNode *cmn = RemHead();
        while (cmn) {
            delete cmn;
            cmn = RemHead();
        }
    }
    inline bool IsListEmpty() const {
        return numElements == 0;
    }
    inline u32 GetNumElements() const {
        return numElements;
    }

    void Sort(bool (*CheckFlip)(FEMinNode *, FEMinNode *));

  private:
    unsigned int numElements; // offset 0x0, size 0x4

  protected:
    FEMinNode *head; // offset 0x4, size 0x4
    FEMinNode *tail; // offset 0x8, size 0x4
};

// total size: 0x14
class FENode : public FEMinNode {
  public:
    char *name;            // offset 0xC, size 0x4
    unsigned int nameHash; // offset 0x10, size 0x4

    FENode();
    ~FENode() override;
    bool SetName(const char *theName);

    inline const char *GetName() const {
        return name;
    }
    inline const unsigned int GetNameHash() const {
        return nameHash;
    }
    inline FENode *GetNext() const {
        return static_cast<FENode *>(FEMinNode::GetNext());
    }
    inline FENode *GetPrev() const {
        return static_cast<FENode *>(FEMinNode::GetPrev());
    }
};

// total size: 0x10
class FEList : public FEMinList {
  public:
    inline FENode *GetHead() const {
        return static_cast<FENode *>(FEMinList::GetHead());
    }
    inline FENode *RemNode(FEMinNode *n) {
        return static_cast<FENode *>(FEMinList::RemNode(n));
    }
    FENode *FindNode(const char *pName, FENode *node) const;
    FENode *FindNode(const char *pName) const;
};

u32 FEHashUpper(const char *String);

#endif
