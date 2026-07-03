#ifndef FELIST_H
#define FELIST_H

#include <types.h>
#include "Speed/Indep/Src/FEng/FETypes.h"

#define LIST_MAGIC 0xABadCafe // :32

// File: speed/indep/src/feng/FEList.h
// total size: 0xC
// Decl: speed/indep/src/feng/FEList.h:53
class FEMinNode {
  protected:
    FEMinNode *next, *prev; // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FEList.h:55

  public:
    FEMinNode() { // Decl: speed/indep/src/feng/FEList.h:58
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
// Decl: speed/indep/src/feng/FEList.h:79
class FENode : public FEMinNode {
  protected:
    char *name;            // offset 0xC, size 0x4, Decl: speed/indep/src/feng/FEList.h:81
    unsigned int nameHash; // offset 0x10, size 0x4, Decl: speed/indep/src/feng/FEList.h:82

  public:
    FENode();
    ~FENode() override;

    bool SetName(const char *theName);

    char *const GetName() const {
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
// Decl: speed/indep/src/feng/FEList.h:111
class FEMinList {
  private:
    unsigned int numElements; // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FEList.h:113

  public:
    typedef bool (*CheckFlipFunc)(FEMinNode *, FEMinNode *);

  protected:
    FEMinNode *head, *tail; // offset 0x4, size 0x4, Decl: speed/indep/src/feng/FEList.h:118

  public:
    FEMinList() { // Decl: speed/indep/src/feng/FEList.h:121
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

    void AddHead(FEMinNode *n) { // Decl: speed/indep/src/feng/FEList.h:133
        AddNode(nullptr, n);
    }
    void AddTail(FEMinNode *n) { // Decl: speed/indep/src/feng/FEList.h:134
        AddNode(tail, n);
    }

    void AddNode(FEMinNode *insertpoint, FEMinNode *node);

    FEMinNode *FindNode(u32 ordinalnumber) const;

    void Swap(FEMinNode *n, FEMinNode *m);

    void Purge() { // Decl: speed/indep/src/feng/FEList.h:141
        FEMinNode *cmn = RemHead();
        while (cmn) {
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
// Decl: speed/indep/src/feng/FEList.h:165
class FEList : public FEMinList {
  public:
    FEList() {} // Decl: speed/indep/src/feng/FEList.h:167
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

    FENode *RemHead() {} // Decl: speed/indep/src/feng/FEList.h:178

    FENode *RemTail() {} // Decl: speed/indep/src/feng/FEList.h:179

    FENode *RemNode(FEMinNode *n) { // Decl: speed/indep/src/feng/FEList.h:180
        return static_cast<FENode *>(FEMinList::RemNode(n));
    }
};

class FEHashNode;

// total size: 0x10
// Decl: speed/indep/src/feng/FEList.h:191
class FEHashNodePtr : public FEMinNode {
  private:
    FEHashNode *NodePtr; // offset 0xC, size 0x4, Decl: speed/indep/src/feng/FEList.h:193
  public:
    ~FEHashNodePtr() override {}
    FEHashNodePtr(FEHashNode *PointTo) {} // Decl: speed/indep/src/feng/FEList.h:196
    FEHashNode *GetNodePtr() {}           // Decl: speed/indep/src/feng/FEList.h:197
};

// total size: 0x18
// Decl: speed/indep/src/feng/FEList.h:203
class FEHashNode : public FEMinNode {
  private:
    char *Name; // offset 0xC, size 0x4, Decl: speed/indep/src/feng/FEList.h:205

    u32 Hash;             // offset 0x10, size 0x4, Decl: speed/indep/src/feng/FEList.h:207
    FEHashNodePtr *myPtr; // offset 0x14, size 0x4, Decl: speed/indep/src/feng/FEList.h:208

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
// Decl: speed/indep/src/feng/FEList.h:229
class FEHashList {
  private:
    i32 TableSize;        // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FEList.h:231
    FEMinList *HashTable; // offset 0x4, size 0x4, Decl: speed/indep/src/feng/FEList.h:232
    FEMinList NodeList;   // offset 0x8, size 0x10, Decl: speed/indep/src/feng/FEList.h:233

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
