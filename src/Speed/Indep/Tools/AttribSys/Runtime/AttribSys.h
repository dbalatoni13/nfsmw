#ifndef ATTRIBSYS_ATTRIBSYS_H
#define ATTRIBSYS_ATTRIBSYS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AttribArray.h"
#include "Common/AttribHashMap.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Misc/AttribAlloc.h"

// Credit: Brawltendo
namespace Attrib {

typedef unsigned int Key;

// const int kTypeHandlerCount = 7;
// unsigned int kTypeHandlerIds[kTypeHandlerCount] = {0x2B936EB7u, 0xAA229CD7u, 0x341F03A0u, 0x600994C4u, 0x681D219Cu, 0x5FDE6463u, 0x57D382C9u};

const void *DefaultDataArea(std::size_t bytes);
void AllocationAccounting(std::size_t bytes, bool add);

inline void Free(void *ptr, std::size_t bytes, const char *name) {
    AttribAlloc::Free(ptr, bytes, name);
}

class Instance;
class Attribute;

class Collection {
  public:
    class Node *GetNode(Key attributeKey, const Collection *&container) const;
    Attribute Get(const Instance &instance, Key attributeKey) const;
    void *GetData(Key attributeKey, std::size_t index) const;
    std::size_t Count() const;
    Key FirstKey(bool &inLayout) const;

    // TODO this must be in AttribPrivate.h, why?
    void AddRef() const {
        // TODO what the hell?
        const_cast<Collection *>(this)->mRefCount++;
    }

  private:
    HashMap mTable;
    const Collection *mParent;
    class Class *mClass;
    void *mLayout;
    std::size_t mRefCount;
    Key mKey;
    class Vault *mSource;
    const char *mNamePtr;
};

class RefSpec {
    // total size: 0xC
    Key mClassKey;                    // offset 0x0, size 0x4
    Key mCollectionKey;               // offset 0x4, size 0x4
    const Collection *mCollectionPtr; // offset 0x8, size 0x4

  public:
    void SetCollection(const Collection *collectionPtr);
    const Class *GetClass() const;
    const Collection *GetCollection() const;
    const Collection *GetCollectionWithDefault() const;
    RefSpec &operator=(const RefSpec &rhs);
    void Clean() const;

    void operator delete(void *ptr, std::size_t bytes) {
        Free(ptr, bytes, "RefSpec");
    }

    RefSpec() {
        mClassKey = 0;
        mCollectionKey = 0;
        mCollectionPtr = nullptr;
    }

    ~RefSpec() {
        if (mCollectionPtr) {
            Clean();
        }
    }

    Key GetClassKey() const {
        return mClassKey;
    }

    Key GetCollectionKey() const {
        return mCollectionKey;
    }
};

class Attribute {
  protected:
    void *GetElementPointer(unsigned int index) const {
        return &reinterpret_cast<int *>(mDataPointer)[index];
    }

  public:
    Attribute(const Attribute &src);
    Attribute();
    Attribute(const Instance &instance, const Collection *collection, Node *node);
    ~Attribute();
    const Attribute &operator=(const Attribute &rhs);
    bool operator==(const Attribute &rhs) const;
    bool operator!=(const Attribute &rhs) const;
    bool IsValid() const;
    bool IsInherited() const;
    bool IsMutable() const;
    bool IsLocatable();
    Key GetKey() const;
    unsigned int GetType() const;
    const Instance *GetInstance() const;
    const Collection *GetCollection() const;
    unsigned int GetSize() const;
    unsigned int GetLength() const;
    bool SetLength(unsigned int);
    void SendChangeMsg() const;
    template <typename T> const T &Get(unsigned int index, T &result) const;

    Key GetCollectionKey() const {
        // TODO
    }

    const void *GetDataAddress() const {
        return this->mDataPointer;
    }

    Attrib::Node *GetInternal() const {
        return this->mInternal;
    }

    bool Get(unsigned int index, RefSpec &result) {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(mDataPointer ? GetElementPointer(index) : GetInternalPointer(index));
        if (resultptr) {
            result = *resultptr;
            return true;
        } else {
            return false;
        }
    }

  private:
    void *GetInternalPointer(unsigned int index) const;

    // total size: 0x10
    const Instance *mInstance;     // offset 0x0, size 0x4
    const Collection *mCollection; // offset 0x4, size 0x4
    struct Node *mInternal;        // offset 0x8, size 0x4
    void *mDataPointer;            // offset 0xC, size 0x4
};

namespace Gen {
class GenericAccessor;
};

class Instance {
  public:
    enum Flags { kDynamic = 1 };

    Instance(const Instance &src);
    Instance(const struct Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner);
    Instance(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner);
    ~Instance();
    Key GetClass() const;
    Key GetCollection() const;
    Key GetParent() const;
    void SetParent(unsigned int parent);
    const void *GetAttributePointer(Key attribkey, unsigned int index) const;
    const Instance &operator=(const Instance &rhs);
    Attribute Get(Key attributeKey) const;
    void Change(const Collection *collection);

    void *GetLayoutPointer() const {
        return mLayoutPtr;
    }

    void *GetLayoutPointer() {
        return mLayoutPtr;
    }

    const Collection *GetConstCollection() const {
        return mCollection;
    }

    void SetDefaultLayout(unsigned int bytes) {
        if (this->mLayoutPtr == nullptr) {
            this->mLayoutPtr = const_cast<void *>(DefaultDataArea(bytes));
        }
    }

    bool IsValid() const {
        return this->mCollection != nullptr;
    }

    const Gen::GenericAccessor *operator->() const {
        return reinterpret_cast<const Gen::GenericAccessor *>(this);
    }

  private:
    UTL::COM::IUnknown *mOwner;
    const Collection *mCollection;
    void *mLayoutPtr;
    unsigned int mMsgPort;
    unsigned short mFlags;
    unsigned short mLocks;
};

class DatabasePrivate;
class ClassPrivate;

// total size: 0x10
class Definition {
  public:
    enum Flags {
        kIsNotSearchable = 8,
        kIsBound = 4,
        kInLayout = 2,
        kArray = 1,
    };

    Definition(unsigned int k) {
        mKey = k;
    }

    Key GetKey() const {
        return mKey;
    }

    bool operator<(const Definition &rhs) const {
        return mKey < rhs.mKey;
    }

  private:
    Key mKey;                 // offset 0x0, size 0x4
    unsigned int mType;       // offset 0x4, size 0x4
    unsigned short mOffset;   // offset 0x8, size 0x2
    unsigned short mSize;     // offset 0xA, size 0x2
    unsigned short mMaxCount; // offset 0xC, size 0x2
    unsigned char mFlags;     // offset 0xE, size 0x1
    unsigned char mAlignment; // offset 0xF, size 0x1
};

// total size: 0xC
class Class {
  public:
    class TablePolicy {};

    Class(unsigned int k, ClassPrivate &privates);
    ~Class();
    const Definition *GetDefinition(Key key) const;
    std::size_t GetNumDefinitions() const;
    Key GetFirstDefinition() const;
    Key GetNextDefinition(Key prev) const;
    std::size_t GetNumCollections() const;
    Key GetFirstCollection() const;
    std::size_t GetTableNodeSize() const;
    void CopyLayout(void *srcLayout, void *dstLayout) const;
    void FreeLayout(void *layout) const;

  private:
    void operator delete(void *ptr, std::size_t bytes) {
        AllocationAccounting(bytes, false);
        Free(ptr, bytes, nullptr);
    }

    Key mKey;              // offset 0x0, size 0x4
    std::size_t mRefCount; // offset 0x4, size 0x4
  public:
    // TODO how does Collection::Count access this without an inline?
    ClassPrivate &mPrivates; // offset 0x8, size 0x4
};

// total size: 0x8
class Database {
  public:
    bool AddClass(Class *c);
    void RemoveClass(Class *c);

    static Database &Get() {
        return *sThis;
    }

    bool IsInitialized() {}

  private:
    Database(DatabasePrivate &privates);

    static Attrib::Database *sThis;

    DatabasePrivate &mPrivates; // offset 0x0, size 0x4
};

template <typename t> class TAttrib : public Attribute {
  public:
    void operator delete(void *ptr, std::size_t bytes) {
        Free(ptr, bytes, nullptr);
    }

    TAttrib(const Attribute &src) : Attribute(src) {}
    ~TAttrib() {}

    bool &Get(unsigned int index) const;
};

} // namespace Attrib

template <typename T> class AttributeStructPtr : public T {
  public:
    static Attrib::Key GetClassKey() {
        return mAttributeClass;
    }

    AttributeStructPtr(Attrib::Key namekey) : T(namekey, 0, nullptr) {
        if (!T::IsValid()) {
            T::Change(0xeec2271a);
        }
        T::IsValid();
    }

    ~AttributeStructPtr() {}

    const T &operator*() const {
        return *this;
    }

    const T *operator->() const {
        return this;
    }

    operator const T *() const {
        return this;
    }

  private:
    static unsigned int mAttributeClass;
};

#endif
