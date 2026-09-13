#ifndef FELIST_H
#define FELIST_H

#include <types.h>
#include "Speed/Indep/Src/FEng/FETypes.h"

#define LIST_MAGIC 0xABadCafe // :32

// total size: 0xC
// Decl: 53
class FEMinNode {
  protected:
    FEMinNode *next, *prev; // offset 0x0, size 0x4, Decl: 55

  public:
    FEMinNode() { // Decl: 58
        next = reinterpret_cast<FEMinNode *>(LIST_MAGIC);
        prev = reinterpret_cast<FEMinNode *>(LIST_MAGIC);
    }
    virtual ~FEMinNode() {}

    FEMinNode *GetNext() const {
        return next;
    }

    FEMinNode *GetPrev() const {
        return prev;
    }

    friend class FERefList;
    friend class FEMinList;
    friend class FEngine;
    friend class FESlotPool;
    friend class FEMultiPool;
    friend class FEPackage;
};

// total size: 0x14
// Decl: 79
class FENode : public FEMinNode {
    friend class FEList;

  protected:
    char *name;            // offset 0xC, size 0x4, Decl: 81
    unsigned int nameHash; // offset 0x10, size 0x4, Decl: 82

  public:
    FENode();
    ~FENode() override;

    bool SetName(const char *theName);

    const char *GetName() const {
        return name;
    }

    const unsigned int GetNameHash() const {
        return nameHash;
    }

    FENode *GetNext() const {
        return static_cast<FENode *>(FEMinNode::GetNext());
    }

    FENode *GetPrev() const {
        return static_cast<FENode *>(FEMinNode::GetPrev());
    }
};

// total size: 0x10
// Decl: 111
class FEMinList {
  private:
    unsigned int numElements; // offset 0x0, size 0x4, Decl: 113

  public:
    typedef bool (*CheckFlipFunc)(FEMinNode *, FEMinNode *);

  protected:
    FEMinNode *head, *tail; // offset 0x4, size 0x4, Decl: 118

  public:
    FEMinList() { // Decl: 121
        head = nullptr;
        tail = nullptr;
        numElements = 0;
    }
    virtual ~FEMinList() {
        Purge();
    }

    bool IsInList(FEMinNode *node) const;

    FEMinNode *RemHead();

    FEMinNode *RemTail();

    FEMinNode *RemNode(FEMinNode *node);

    i32 ElementNumber(FEMinNode *node);

    FEMinNode *GetHead() const {
        return head;
    }

    FEMinNode *GetTail() const {
        return tail;
    }

    void AddHead(FEMinNode *n) { // Decl: 133
        AddNode(nullptr, n);
    }
    void AddTail(FEMinNode *n) { // Decl: 134
        AddNode(tail, n);
    }

    void AddNode(FEMinNode *insertpoint, FEMinNode *node);

    FEMinNode *FindNode(u32 ordinalnumber) const;

    void Swap(FEMinNode *n, FEMinNode *m);

    void Purge() { // Decl: 141
        FEMinNode *cmn = RemHead();
        while (cmn != nullptr) {
            delete cmn;
            cmn = RemHead();
        }
    }

    bool IsListEmpty() const {
        return numElements == 0;
    }

    u32 GetNumElements() const {
        return numElements;
    }

    void Sort(bool (*CheckFlip)(FEMinNode *, FEMinNode *));
};

// total size: 0x10
// Decl: 165
class FEList : public FEMinList {
  public:
    FEList() {} // Decl: 167
    ~FEList() override {}

    FENode *FindNode(u32 ordinalnumber) const {}

    FENode *FindNode(const char *name, FENode *node) const;

    FENode *FindNode(const char *name) const;

    void SortAlpha();

    FENode *GetHead() const {
        return static_cast<FENode *>(FEMinList::GetHead());
    }

    FENode *GetTail() const {
        return static_cast<FENode *>(FEMinList::GetTail());
    }

    FENode *RemHead() { // Decl: 178
        return static_cast<FENode *>(FEMinList::RemHead());
    }

    FENode *RemTail() { // Decl: 179
        return static_cast<FENode *>(FEMinList::RemTail());
    }

    FENode *RemNode(FEMinNode *n) { // Decl: 180
        return static_cast<FENode *>(FEMinList::RemNode(n));
    }
};

class FEHashNode;

// total size: 0x10
// Decl: 191
class FEHashNodePtr : public FEMinNode {
  private:
    FEHashNode *NodePtr; // offset 0xC, size 0x4, Decl: 193
  public:
    ~FEHashNodePtr() override {}
    FEHashNodePtr(FEHashNode *PointTo) {} // Decl: 196
    FEHashNode *GetNodePtr() {}           // Decl: 197
};

// total size: 0x18
// Decl: 203
class FEHashNode : public FEMinNode {
  private:
    char *Name; // offset 0xC, size 0x4, Decl: 205

    u32 Hash;             // offset 0x10, size 0x4, Decl: 207
    FEHashNodePtr *myPtr; // offset 0x14, size 0x4, Decl: 208

  public:
    FEHashNode();

    FEHashNode(FEHashNode &hashnode);

    ~FEHashNode() override;

    bool SetName(const char *newName);

    char *const GetName() const {
        return Name;
    }

    FEHashNode *GetNext() const {}

    FEHashNode *GetPrev() const {}
};

// total size: 0x1C
// Decl: 229
class FEHashList {
  private:
    i32 TableSize;        // offset 0x0, size 0x4, Decl: 231
    FEMinList *HashTable; // offset 0x4, size 0x4, Decl: 232
    FEMinList NodeList;   // offset 0x8, size 0x10, Decl: 233

  public:
    FEHashList();

    FEHashList(u32 NewTableSize);

    void AddHashEntry(FEHashNode *n);

    void RemHashEntry(FEHashNode *n);

    virtual ~FEHashList();

    void SetTableSize(i32 NewTableSize);

    i32 GetTableSize() const {}

    void Purge();

    void AddHead(FEHashNode *n);

    void AddTail(FEHashNode *n);

    void AddNode(FEHashNode *insertPoint, FEHashNode *n);

    void RemNode(FEHashNode *n);

    FEHashNode *Find(const char *FindName);

    void Rename(FEHashNode *n, const char *NewName);

    u32 GetNumElements() const {}

    FEHashNode *GetHead() const {}

    FEHashNode *GetTail() const {}
};

u32 FEHash(const char *String);
u32 FEHashUpper(const char *String);

#endif
