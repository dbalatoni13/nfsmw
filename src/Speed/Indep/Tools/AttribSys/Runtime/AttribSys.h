#ifndef ATTRIBSYS_ATTRIBSYS_H
#define ATTRIBSYS_ATTRIBSYS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AttribArray.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Misc/AttribAlloc.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/VecHashMap64.h"

// Credit: Brawltendo
namespace Attrib {

typedef unsigned int Key;

// const int kTypeHandlerCount = 7;
// unsigned int kTypeHandlerIds[kTypeHandlerCount] = {0x2B936EB7u, 0xAA229CD7u, 0x341F03A0u, 0x600994C4u, 0x681D219Cu, 0x5FDE6463u, 0x57D382C9u};

const void *DefaultDataArea(std::size_t bytes);
void AllocationAccounting(std::size_t bytes, bool add);
Key RegisterString(const char *str);
Key StringToKey(const char *str);

// Rotates (v) by (amount) bits
// TODO probably not in this namespace
inline unsigned int RotateNTo32(unsigned int v, unsigned int amount) {
    return (v << amount) | (v >> (32 - amount));
}

inline void *Alloc(std::size_t bytes, const char *name) {
    AllocationAccounting(bytes, true);
    return AttribAlloc::Allocate(bytes, name);
}

inline void Free(void *ptr, std::size_t bytes, const char *name) {
    AllocationAccounting(bytes, false);
    AttribAlloc::Free(ptr, bytes, name);
}

class Instance;
class Attribute;
class Class;
class Definition;
class DatabasePrivate;
class ClassPrivate;
class Collection;
class Vault;

class ITypeHandler {};

// total size: 0x14
class TypeDesc {
  public:
    static ITypeHandler *Lookup(unsigned int t);

    TypeDesc(unsigned int t) {
        mType = t;
        mName = nullptr;
        mSize = 0;
        mIndex = 0;
        mHandler = Lookup(t);
    }

    unsigned int GetType() const {
        return mType;
    }

    bool operator<(const TypeDesc &rhs) const {
        return mType < rhs.mType;
    }

  private:
    unsigned int mType;     // offset 0x0, size 0x4
    const char *mName;      // offset 0x4, size 0x4
    unsigned int mSize;     // offset 0x8, size 0x4
    unsigned int mIndex;    // offset 0xC, size 0x4
    ITypeHandler *mHandler; // offset 0x10, size 0x4
};

// total size: 0x10
class TypeDescPtrVec : public std::vector<const TypeDesc *> {};

// total size: 0x10
class TypeTable : public std::set<TypeDesc> {};

// total size: 0x8
class CollectionList : public std::list<const Collection *> {};

// total size: 0x8
class ClassList : public std::list<const Class *> {};

// total size: 0x8
class Database {
  public:
    bool AddClass(Class *c);
    void RemoveClass(Class *c);
    Class *GetClass(unsigned int k);
    void Delete(const Collection *c);
    void Delete(const Class *c);
    void CollectGarbage();
    unsigned int GetNumIndexedTypes() const;
    const TypeDesc &GetIndexedTypeDesc(unsigned short index) const;
    const TypeDesc &GetTypeDesc(unsigned int t) const;
    void DumpContents(unsigned int classFilter) const;

    static Database &Get() {
        return *sThis;
    }

    void operator delete(void *ptr, std::size_t bytes) {
        Free(ptr, bytes, nullptr);
    }

    bool IsInitialized() {
        return sThis != nullptr;
    }

  private:
    Database(DatabasePrivate &privates);
    virtual ~Database();

    static Database *sThis;

    DatabasePrivate &mPrivates; // offset 0x0, size 0x4
};

// Credit: Brawltendo
class Node {
  public:
    enum Flags {
        Flag_RequiresRelease = 1 << 0,
        Flag_IsArray = 1 << 1,
        Flag_IsInherited = 1 << 2,
        Flag_IsAccessor = 1 << 3,
        Flag_IsLaidOut = 1 << 4,
        Flag_IsByValue = 1 << 5,
        Flag_IsLocatable = 1 << 6,
    };

    bool GetFlag(unsigned int mask) const {
        return mFlags & mask;
    }

    bool RequiresRelease() const {
        return GetFlag(Flag_RequiresRelease);
    }

    bool IsArray() const {
        return GetFlag(Flag_IsArray);
    }

    bool IsInherited() const {
        return GetFlag(Flag_IsInherited);
    }

    bool IsAccessor() const {
        return GetFlag(Flag_IsAccessor);
    }

    bool IsLaidOut() const {
        return GetFlag(Flag_IsLaidOut);
    }

    bool IsByValue() const {
        return GetFlag(Flag_IsByValue);
    }

    bool IsLocatable() const {
        return GetFlag(Flag_IsLocatable);
    }

    bool IsValid() const {
        return IsLaidOut() || mPtr != this;
    }

    void *GetPointer(void *layoutptr) const {
        if (IsByValue()) {
            return &mValue;
        } else if (IsLaidOut()) {
            return (void *)(uintptr_t(layoutptr) + uintptr_t(mPtr));
        } else {
            return mPtr;
        }
    }

    Array *GetArray(void *layoutptr) const {
        if (IsLaidOut()) {
            return (Array *)(uintptr_t(layoutptr) + uintptr_t(mArray));
        } else {
            return mArray;
        }
    }

    std::size_t GetCount(void *layoutptr) const {
        if (IsValid()) {
            if (IsArray()) {
                return GetArray(layoutptr)->GetCount();
            }
            return 1;
        }
        return 0;
    }

    Key GetKey() const {
        return IsValid() ? mKey : 0;
    }

    const TypeDesc &GetTypeDesc() const {
        return Database::Get().GetTypeDesc(mTypeIndex);
    }

  private:
    Key mKey;
    union {
        void *mPtr;
        class Array *mArray;
        mutable unsigned int mValue;
        unsigned int mOffset;
    };
    unsigned short mTypeIndex;
    unsigned char mMax;
    unsigned char mFlags;
};

// total size: 0xC
class Class {
  public:
    class TablePolicy {
        static unsigned int KeyIndex(unsigned int k, unsigned int tableSize, unsigned int keyShift) {
            return RotateNTo32(k, keyShift) % tableSize;
        }

        static unsigned int WrapIndex(unsigned int index, unsigned int tableSize, unsigned int keyShift) {
            return RotateNTo32(index, keyShift) % tableSize;
        }
    };

    Class(Key k, ClassPrivate &privates);
    ~Class();
    void Delete() const;
    const Definition *GetDefinition(Key key) const;
    std::size_t GetNumDefinitions() const;
    Key GetFirstDefinition() const;
    Key GetNextDefinition(Key prev) const;
    std::size_t GetNumCollections() const;
    Key GetFirstCollection() const;
    Key GetNextCollection(Key prev) const;
    std::size_t GetTableNodeSize() const;
    void CopyLayout(void *srcLayout, void *dstLayout) const;
    void FreeLayout(void *layout) const;
    const Collection *GetCollection(Key key) const;
    const Collection *GetCollectionWithDefault(Key key) const;

    bool IsReferenced() const {
        return mRefCount > 0;
    }

    Key GetKey() const {
        return mKey;
    }

  private:
    void operator delete(void *ptr, std::size_t bytes) {
        Free(ptr, bytes, nullptr);
    }

    Key mKey;              // offset 0x0, size 0x4
    std::size_t mRefCount; // offset 0x4, size 0x4
  public:
    // TODO how does Collection::Count access this without an inline?
    ClassPrivate &mPrivates; // offset 0x8, size 0x4
};

// total size: 0x10
class ClassTable : public VecHashMap<unsigned int, Class, Class::TablePolicy, false, 16> {};

// total size: 0xC
class RefSpec {
  public:
    RefSpec(const RefSpec &src);
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

  private:
    Key mClassKey;                            // offset 0x0, size 0x4
    Key mCollectionKey;                       // offset 0x4, size 0x4
    mutable const Collection *mCollectionPtr; // offset 0x8, size 0x4
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

    void operator delete(void *ptr, std::size_t bytes) {
        Free(ptr, bytes, nullptr);
    }

    Key GetCollectionKey() const {
        // TODO
    }

    const void *GetDataAddress() const {
        return this->mDataPointer;
    }

    Node *GetInternal() const {
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
    Node *mInternal;               // offset 0x8, size 0x4
    void *mDataPointer;            // offset 0xC, size 0x4
};

// total size: 0xC
class AttributeIterator {
  public:
    AttributeIterator(const Collection *c);
    bool Advance();

  private:
    Key mCurrentKey;               // offset 0x0, size 0x4
    const Collection *mCollection; // offset 0x4, size 0x4
    bool mInLayout;                // offset 0x8, size 0x1
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
    const Instance &operator=(const Instance &rhs);
    Attribute Get(Key attributeKey) const;
    bool Lookup(Key attributeKey, Attribute &attrib) const;
    bool Contains(Key attributeKey) const;
    unsigned int LocalAttribCount() const;
    bool Add(Key attributeKey, unsigned int count);
    bool Remove(Key attributeKey);
    bool Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes);
    bool ModifyInternal(Key classKey, Key dynamicCollectionKey, unsigned int reserve);
    void Unmodify();
    Key GenerateUniqueKey(const char *name, bool registerName) const;
    void Change(const Collection *collection);
    void Change(const RefSpec &refspec);
    void ChangeWithDefault(const RefSpec &refspec);
    const void *GetAttributePointer(Key attribkey, unsigned int index) const;

    void operator delete(void *ptr, std::size_t bytes) {
        Free(ptr, bytes, nullptr);
    }

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

    bool IsDynamic() const {
        return mFlags & 1;
    }

    // TODO is this right in all cases?
    Collection *GetDynamicCollection() const {
        if (IsDynamic()) {
            return const_cast<Collection *>(mCollection);
        }
        return nullptr;
    }

    void Lock() const {
        mLocks++;
    }

    void Unlock() const {
        // TODO call LockReleased
        mLocks--;
    }

    AttributeIterator Iterator() const;

    const Gen::GenericAccessor *operator->() const {
        return reinterpret_cast<const Gen::GenericAccessor *>(this);
    }

  protected:
    Key GUKeyInternal(Key classKey, const char *name, bool registerName) const;

  private:
    void LockReleased() const {}

    UTL::COM::IUnknown *mOwner;    // offset 0x0, size 0x4
    const Collection *mCollection; // offset 0x4, size 0x4
    void *mLayoutPtr;              // offset 0x8, size 0x4
    unsigned int mMsgPort;         // offset 0xC, size 0x4
    unsigned short mFlags;         // offset 0x10, size 0x2
    mutable unsigned short mLocks; // offset 0x12, size 0x2
};

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
