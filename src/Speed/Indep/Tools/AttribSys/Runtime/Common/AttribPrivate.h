#ifndef ATTRIBSYS_ATTRIB_PRIVATE_H
#define ATTRIBSYS_ATTRIB_PRIVATE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "../VecHashMap64.h"
#include "AttribHashMap.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribLoadAndGo.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

// Credit: Brawltendo
namespace Attrib {

// total size: 0x20
class CollectionLoadData {
  public:
    // total size: 0xC
    struct AttribEntry {
        Key mKey;            // offset 0x0, size 0x4
        void *mData;         // offset 0x4, size 0x4
        uint16_t mType;      // offset 0x8, size 0x2
        uint8_t mNodeFlags;  // offset 0xA, size 0x1
        uint8_t mEntryFlags; // offset 0xB, size 0x1
    };

    const unsigned int *GetTypes() const {
        return (unsigned int *)&this[1];
    }

    const AttribEntry *GetEntries() const {
        return reinterpret_cast<const AttribEntry *>(&GetTypes()[mNumTypes]);
    }

    Key mKey;                // offset 0x0, size 0x4
    Key mClass;              // offset 0x4, size 0x4
    Key mParent;             // offset 0x8, size 0x4
    uint32_t mTableReserve;  // offset 0xC, size 0x4
    uint32_t mTableKeyShift; // offset 0x10, size 0x4
    uint32_t mNumEntries;    // offset 0x14, size 0x4
    uint32_t mNumTypes;      // offset 0x18, size 0x4
    void *mLayout;           // offset 0x1C, size 0x4
};

// total size: 0x2C
class Collection {
  public:
    Collection(Class *aclass, const Collection *parent, Key k, unsigned int reserveSize);
    Collection(const CollectionLoadData &loadData, Vault *v);
    // Unused
    Collection(const Collection &src, Key k);
    ~Collection();
    Node *GetNode(Key attributeKey, const Collection *&container) const;
    Attribute Get(const Instance &instance, Key attributeKey) const;
    void *GetData(Key attributeKey, unsigned int index) const;
    std::size_t Count() const;
    Key FirstKey(bool &inLayout) const;
    void SetParent(Key parent);
    void FreeNodeData(bool isArray, void *data, bool requiresRelease, const TypeDesc &typeDesc);
    void Delete() const;
    Key NextKey(Key prev, bool &inLayout) const;
    bool Contains(Key k) const;
    bool AddAttribute(Key attributeKey, unsigned int count);
    bool RemoveAttribute(Key attributeKey);
    void Clear();
    void Clean() const;

    void *operator new(std::size_t bytes) {
        return Alloc(bytes, "Attrib::Collection");
    }

    void operator delete(void *ptr, std::size_t bytes) {
        Free(ptr, bytes, "Attrib::Collection");
    }

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

    std::size_t GetRefs() const {
        return mRefCount;
    }

    Key GetKey() const {
        return mKey;
    }

    void SetName(const char *nameptr) {
        mNamePtr = nameptr;
    }

  private:
    HashMap mTable;                 // offset 0x0, size 0x10
    const Collection *mParent;      // offset 0x10, size 0x4
    Class *mClass;                  // offset 0x14, size 0x4
    void *mLayout;                  // offset 0x18, size 0x4
    mutable unsigned int mRefCount; // offset 0x1C, size 0x4
    Key mKey;                       // offset 0x20, size 0x4
    Vault *mSource;                 // offset 0x24, size 0x4
    const char *mNamePtr;           // offset 0x28, size 0x4
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

// total size: 0x1C
class ClassLoadData {
  public:
    Key mClass;                  // offset 0x0, size 0x4
    uint32_t mCollectionReserve; // offset 0x4, size 0x4
    uint32_t mNumDefinitions;    // offset 0x8, size 0x4
    Definition *mDefinitions;    // offset 0xC, size 0x4
    uint32_t mLayoutSize;        // offset 0x10, size 0x4
    uint32_t mLayoutKeyShift;    // offset 0x14, size 0x4
    uint32_t mLayoutCount;       // offset 0x18, size 0x4
};

// total size: 0x3C
class ClassPrivate : public Class {
  public:
    ClassPrivate(const ClassLoadData &loadData, Vault *v)
        : Class(loadData.mClass, *this), mLayoutTable(loadData.mLayoutCount, loadData.mLayoutKeyShift, true),
          mCollections(loadData.mCollectionReserve), mLayoutSize(loadData.mLayoutSize), mNumDefinitions(loadData.mNumDefinitions),
          mDefinitions(loadData.mDefinitions), mSource(v) {
        mSource->AddRef();

        for (std::size_t d = 0; d < mPrivates.mNumDefinitions; d++) {
            Definition &def = mPrivates.mDefinitions[d];
            if (def.InLayout() && !def.IsNotSearchable()) {
                unsigned char flags = Node::Flag_IsLaidOut;
                if (def.IsArray()) {
                    flags |= Node::Flag_IsArray;
                }
                mPrivates.mLayoutTable.Add(def.GetKey(), def.GetType(), (void *)def.GetOffset(), false, flags, true, nullptr);
            }
        }
        mNamePtr = nullptr;
    }

    // total size: 0x10
    class CollectionHashMap : public VecHashMap<unsigned int, Attrib::Collection, Attrib::Class::TablePolicy, true, 40> {
      public:
        CollectionHashMap(std::size_t reserve) : VecHashMap<unsigned int, Attrib::Collection, Attrib::Class::TablePolicy, true, 40>(reserve) {}

        unsigned int GetNextValidIndex(unsigned int startPoint) const {
            unsigned int index = startPoint + 1;
            for (; index < mTableSize && !mTable[index].IsValid(); index++) {
            }
            return index;
        }

        unsigned int GetKeyAtIndex(unsigned int index) const {
            if (ValidIndex(index)) {
                (void)ValidIndex(index);
                return mTable[index].Key();
            }
            return 0;
        }
    };

    void *operator new(std::size_t bytes) {
        return Alloc(bytes, "Attrib::ClassPrivate");
    }

    void operator delete(void *ptr, std::size_t bytes) {
        Free(ptr, bytes, "Attrib::ClassPrivate");
    }

    ~ClassPrivate() {
        mLayoutTable.ClearForRelease();
        mSource->Release();
    }

    std::size_t GetRefs() const {
        return mRefCount;
    }

    HashMap mLayoutTable;           // offset 0xC, size 0x10
    CollectionHashMap mCollections; // offset 0x1C, size 0x10
    uint16_t mLayoutSize;           // offset 0x2C, size 0x2
    uint16_t mNumDefinitions;       // offset 0x2E, size 0x2
    Definition *mDefinitions;       // offset 0x30, size 0x4
    Vault *mSource;                 // offset 0x34, size 0x4
    const char *mNamePtr;           // offset 0x38, size 0x4
};

// c36attx e2: `ClassTable`, `DatabaseLoadData` y `DatabasePrivate` VIVEN EN
// AttribDatabase.cpp en el original: debug_lines situa ~DatabasePrivate en
// attribdatabase.cpp:62 y los dtores de TypeTable/ClassTable en :36, y de
// attribprivate.h no usa ni una linea por encima de la 252 (ScanForValidKey
// es la :247). Aqui basta la declaracion adelantada.
class ClassTable;
class DatabasePrivate;
template <typename T> Key ScanForValidKey(const T &v, unsigned int index) {
    index = v.GetNextValidIndex(index);
    // (void)v.ValidIndex(index); // TODO how to get it to be here instead of in GetKeyAtIndex?
    return v.GetKeyAtIndex(index);
}

} // namespace Attrib

#endif
