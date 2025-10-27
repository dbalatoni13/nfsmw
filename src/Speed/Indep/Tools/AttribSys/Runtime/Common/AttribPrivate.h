#ifndef ATTRIBSYS_ATTRIB_PRIVATE_H
#define ATTRIBSYS_ATTRIB_PRIVATE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "../VecHashMap64.h"
#include "AttribHashMap.h"

// Credit: Brawltendo
namespace Attrib {

// total size: 0x2C
class Collection {
  public:
    Collection(Class *aclass, const Collection *parent, Key k, unsigned int reserveSize);
    Collection(const class CollectionLoadData &loadData, Vault *v);
    // Unused
    Collection(const Collection &src, Key k);
    ~Collection();
    Node *GetNode(Key attributeKey, const Collection *&container) const;
    Attribute Get(const Instance &instance, Key attributeKey) const;
    void *GetData(Key attributeKey, std::size_t index) const;
    std::size_t Count() const;
    Key FirstKey(bool &inLayout) const;
    void SetParent(Key parent);
    void Delete() const;
    Key NextKey(Key prev, bool &inLayout) const;
    bool Contains(Key k) const;
    bool AddAttribute(Key attributeKey, unsigned int count);
    bool RemoveAttribute(Key attributeKey);

    void *operator new(std::size_t bytes) {
        AllocationAccounting(bytes, true);
        return Alloc(bytes, nullptr);
    }

    // TODO this must be in AttribPrivate.h, why?
    void AddRef() const {
        mRefCount++;
    }

    void Release() const {
        if (mRefCount >= 2) {
            mRefCount--;
        } else {
            mRefCount = 0;
            Database::Get().Delete(this);
        }
    }

    const Collection *Parent() const {
        return mParent;
    }

    Key GetParent() const {
        if (mParent) {
            return mParent->GetKey();
        }
        return 0;
    }

    bool IsReferenced() const {
        return mRefCount != 0;
    }

    bool IsDynamic() const {
        return !mSource;
    }

    Class *ContainingClass() const {
        return mClass;
    }

    Key GetClass() const {
        if (mClass) {
            return mClass->GetKey();
        }
        return 0;
    }

    void *GetLayout() const {
        return mLayout;
    }

    Key GetKey() const {
        return mKey;
    }

  private:
    HashMap mTable;
    const Collection *mParent;
    class Class *mClass;
    void *mLayout;
    mutable std::size_t mRefCount;
    Key mKey;
    Vault *mSource;
    const char *mNamePtr;
};

class Private {
  public:
    unsigned int GetLength() const;

    const Array *ToArray() const {
        return reinterpret_cast<const Array *>(mData);
    }

  private:
    unsigned char mData[8];
};

// total size: 0x3C
class ClassPrivate : public Class {
  public:
    // total size: 0x10
    class CollectionHashMap : public VecHashMap<unsigned int, Attrib::Collection, Attrib::Class::TablePolicy, true, 40> {};

    // TODO this is inline
    ClassPrivate(const struct ClassLoadData &loadData, Vault *v);

    HashMap mLayoutTable;           // offset 0xC, size 0x10
    CollectionHashMap mCollections; // offset 0x1C, size 0x10
    unsigned short mLayoutSize;     // offset 0x2C, size 0x2
    unsigned short mNumDefinitions; // offset 0x2E, size 0x2
    Definition *mDefinitions;       // offset 0x30, size 0x4
    struct Vault *mSource;          // offset 0x34, size 0x4
    const char *mNamePtr;           // offset 0x38, size 0x4
};

// total size: 0x4C
class DatabasePrivate : public Database {
  public:
    static void QueueForDelete(const Collection *obj, std::list<const Collection *> &bag) {
        obj->IsReferenced();
        if (std::find(bag.begin(), bag.end(), obj) == bag.end()) {
            bag.push_back(obj);
        }
    }

    static void QueueForDelete(const Class *obj, std::list<const Class *> &bag) {
        obj->IsReferenced();
        if (std::find(bag.begin(), bag.end(), obj) == bag.end()) {
            bag.push_back(obj);
        }
    }

    static void CollectGarbageBag(std::list<const Collection *> &bag) {
        std::list<const Collection *>::iterator iter = bag.begin();

        while (iter != bag.end()) {
            const Collection *obj = *iter;
            if (!obj->IsReferenced()) {
                obj->Delete();
            }
            bag.pop_front();
            iter = bag.begin();
        }
    }

    static void CollectGarbageBag(std::list<const Class *> &bag) {
        std::list<const Class *>::iterator iter = bag.begin();

        while (iter != bag.end()) {
            const Class *obj = *iter;
            if (!obj->IsReferenced()) {
                obj->Delete();
            }
            bag.pop_front();
            iter = bag.begin();
        }
    }

    // TODO inline
    DatabasePrivate(const struct DatabaseLoadData &loadData);

    ClassTable mClasses;                // offset 0x8, size 0x10
    unsigned int mNumCompiledTypes;     // offset 0x18, size 0x4
    TypeDescPtrVec mCompiledTypes;      // offset 0x1C, size 0x10
    TypeTable mTypes;                   // offset 0x2C, size 0x10
    CollectionList mGarbageCollections; // offset 0x3C, size 0x8
    ClassList mGarbageClasses;          // offset 0x44, size 0x8
};

// TODO move?
Key ScanForValidKey(const ClassPrivate::CollectionHashMap &v, std::size_t index);

} // namespace Attrib

#endif
