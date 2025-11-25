#ifndef ATTRIBSYS_ATTRIBSYS_H
#define ATTRIBSYS_ATTRIBSYS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Misc/AttribAlloc.h"

#include <algorithm>

// Credit: Brawltendo
namespace Attrib {

typedef unsigned int Key;

// const int kTypeHandlerCount = 7;
// unsigned int kTypeHandlerIds[kTypeHandlerCount] = {0x2B936EB7u, 0xAA229CD7u, 0x341F03A0u, 0x600994C4u, 0x681D219Cu, 0x5FDE6463u, 0x57D382C9u};

const void *DefaultDataArea(std::size_t bytes);
void AllocationAccounting(std::size_t bytes, bool add);
Key RegisterString(const char *str);
const char *KeyToString(Key k);
Key StringToKey(const char *str);
std::size_t AdjustHashTableSize(std::size_t requiredSize);
void PrepareToAddStrings(unsigned int numstrings);

// Rotates (v) by (amount) bits
// TODO probably not in this namespace
inline unsigned int RotateNTo32(unsigned int v, unsigned int amount) {
    return (v << amount) | (v >> (32 - amount));
}

inline void *Alloc(std::size_t bytes, const char *name) {
    AllocationAccounting(bytes, true);
    if (bytes != 0) {
        return AttribAlloc::Allocate(bytes, name);
    } else {
        return nullptr;
    }
}

inline void Free(void *ptr, std::size_t bytes, const char *name) {
    AllocationAccounting(bytes, false);
    if (ptr && bytes != 0) {
        AttribAlloc::Free(ptr, bytes, name);
    }
}

inline void *TableAllocFunc(std::size_t bytes) {
    return AttribAlloc::Allocate(bytes, "Attrib::CollectionHashMap");
}

inline void TableFreeFunc(void *ptr, std::size_t bytes) {
    AttribAlloc::Free(ptr, bytes, "Attrib::CollectionHashMap");
}

class Instance;
class Attribute;
class Class;
class Definition;
class DatabasePrivate;
class ClassPrivate;
class Collection;
class ExportManager;
class Vault;

class ITypeHandler {
  public:
    virtual void *Retain(void *obj) = 0;
    virtual void *Clone(void *obj) = 0;
    virtual void Clean(void *obj) = 0;
    virtual void Release(void *obj) = 0;
};

class IExportPolicy {
  public:
    virtual void Initialize(Vault &v, const unsigned int &type, const unsigned int &id, const char *name, void *data, std::size_t bytes);
    virtual bool IsReferenced(const Vault &v, const unsigned int &type, const unsigned int &id);
    virtual void Clean(Vault &v, const unsigned int &type, const unsigned int &id);
    virtual void Deinitialize(Vault &v, const unsigned int &type, const unsigned int &id);
};

// total size: 0x14
class TypeDesc {
  public:
    static ITypeHandler *Lookup(unsigned int t);
    static unsigned int NameToType(const char *name);

    TypeDesc() : mType(0), mName(""), mSize(0), mIndex(0), mHandler(nullptr) {}

    TypeDesc(unsigned int t) : mType(t), mName(nullptr), mSize(0), mIndex(0), mHandler(Lookup(t)) {}

    TypeDesc(const char *name, std::size_t size, std::size_t index)
        : mType(NameToType(name)), mName(name), mSize(size), mIndex(index), mHandler(Lookup(mType)) {}

    unsigned int GetType() const {
        return mType;
    }

    unsigned int GetSize() const {
        return mSize;
    }

    unsigned short GetIndex() const {
        return mIndex;
    }

    ITypeHandler *GetHandler() const {
        return mHandler;
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
    static ExportManager &GetExportPolicies();

    Class *GetClass(Key k) const;
    bool AddClass(Class *c);
    void RemoveClass(const Class *c);
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
        Free(ptr, bytes, "Attrib::Database");
    }

    bool IsInitialized() {
        return sThis != nullptr;
    }

    friend class DatabasePrivate;
    friend class DatabaseExportPolicy;

  private:
    Database(DatabasePrivate &privates);
    virtual ~Database();

    static Database *sThis;

    DatabasePrivate &mPrivates; // offset 0x0, size 0x4
};

class Array {
#define Flag_AlignedAt16 (1 << 15)
  private: // Returns the base location of this array's data
    unsigned char *BasePointer() const {
        return const_cast<unsigned char *>(data);
    }

    void *Data(std::size_t byteindex) const {
        unsigned char *base = BasePointer(); // unused
        return (void *)(data + GetPad() + byteindex);
    }

  public:
    void SetTypeIndex(unsigned short typeIndex) {
        mEncodedTypePad = typeIndex | (mEncodedTypePad & 0x8000);
    }

    const Array &operator=(const Array &rhs) {
        for (unsigned int i = 0; i < mCount; i++) {
            SetData(i, nullptr);
        }

        mAlloc = rhs.mAlloc;
        mCount = rhs.mCount;
        mSize = rhs.mSize;
        mEncodedTypePad = rhs.mEncodedTypePad;

        for (unsigned int i = 0; i < mCount; i++) {
            SetData(i, rhs.GetData(i));
        }
    }

    bool IsReferences() const {
        return mSize == 0;
    }

    unsigned short GetTypeIndex() const {
        return mEncodedTypePad & 0x7fff;
    }

    std::size_t GetTypeSize() const {
        // TODO
    }

    std::size_t GetElementSize() const {
        if (IsReferences()) {
            return sizeof(void *);
        } else {
            return mSize;
        }
    }

    std::size_t GetAlloc() const {
        return GetPad() + sizeof(*this) + mAlloc * GetElementSize();
    }

    std::size_t GetCount() const {
        return mCount;
    }

    std::size_t GetPad() const {
        if (!(mEncodedTypePad & Flag_AlignedAt16)) {
            return 0;
        }
        return sizeof(*this);
    }

    const TypeDesc &GetTypeDesc() const {
        return Database::Get().GetIndexedTypeDesc(GetTypeIndex());
    }

    bool SetCount(std::size_t newCount) {
        if (newCount > mAlloc) {
            return false;
        } else {
            if (IsReferences()) {
                for (std::size_t i = mCount; i < newCount; i++) {
                    SetData(i, nullptr);
                }
            }
            mCount = newCount;
            return true;
        }
    }

    void *GetData(std::size_t index) const {
        if (index < mCount) {
            if (IsReferences()) {
                return reinterpret_cast<void **>(Data(0))[index];
            } else {
                return Data(index * mSize);
            }
        } else {
            return nullptr;
        }
    }

    void SetData(std::size_t index, void *value) {
        if (IsReferences()) {
            ITypeHandler *typeHandler = GetTypeDesc().GetHandler();
            typeHandler->Release(GetData(index));
            reinterpret_cast<void **>(Data(0))[index] = typeHandler->Retain(value);
        } else if (value) {
            std::memcpy(GetData(index), value, mSize);
        } else {
            std::memset(GetData(index), 0, mSize);
        }
    }

    static Array *CreateInPlace(void *ptr, unsigned int t, std::size_t count, std::size_t allocSize) {
        const TypeDesc &desc = Database::Get().GetTypeDesc(t);
        unsigned short typeIndex = desc.GetIndex();
        unsigned int typesize = desc.GetSize();
        bool align16 = typesize > 15;

        return new (ptr) Array(typesize, count, allocSize, typeIndex, align16);
    }

    static Array *Create(unsigned int t, std::size_t count) {
        const TypeDesc &desc = Database::Get().GetTypeDesc(t);
        unsigned short typeIndex = desc.GetIndex();
        unsigned int typesize = desc.GetSize();
        bool align16 = typesize > 15;
        std::size_t actualtypesize = typesize;
        if (typesize == 0) {
            actualtypesize = 4;
        }
        unsigned int overhead = align16 ? 16 : 8;
        std::size_t allocSize = (((overhead + (actualtypesize * count) + 63) & ~63) - overhead) / actualtypesize;
        std::size_t allocBytes = overhead + allocSize * actualtypesize;

        return new (Alloc(allocBytes, "Attrib::Array")) Array(typesize, count, allocSize, typeIndex, align16);
    }

    static void Destroy(Array *array) {
        std::size_t allocSize = array->GetAlloc();
        array->~Array();
        Free(array, allocSize, "Attrib::Array");
    }

  private:
    Array(std::size_t typesize, std::size_t count, std::size_t allocSize, std::size_t typeIndex, bool align16) {
        mAlloc = allocSize;
        mCount = count;
        mSize = typesize;
        mEncodedTypePad = typeIndex;
        if (align16 && (typesize != 0)) {
            mEncodedTypePad = typeIndex | Flag_AlignedAt16;
        }
        for (std::size_t i = 0; i < count; i++) {
            SetData(i, nullptr);
        }
        if (IsReferences()) {
            GetTypeDesc();
        }
    }

    // TODO is this really overriden?
    void operator delete(void *ptr) {}

    ~Array() {
        if (IsReferences()) {
            ITypeHandler *typeHandler = GetTypeDesc().GetHandler();
            void **ptrs = reinterpret_cast<void **>(Data(0));
            for (std::size_t i = 0; i < mCount; i++) {
                typeHandler->Release(ptrs[i]);
            }
        }
    }

    void *operator new(std::size_t, void *ptr) {
        return ptr;
    }

    unsigned short mAlloc;
    unsigned short mCount;
    unsigned short mSize;
    unsigned short mEncodedTypePad;
    unsigned char data[];
};

// Credit: Brawltendo
// total size: 0xC
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

    void *operator new(std::size_t, void *ptr) {
        return ptr;
    }

    Node() : mKey(0), mTypeIndex(0), mMax(0), mFlags(0), mPtr(this) {}

    Node(Key key, unsigned int type, void *ptr, bool ptrIsRaw, unsigned char flags, void *layoutptr)
        : mKey(key), mPtr(ptr), mTypeIndex(Database::Get().GetTypeDesc(type).GetIndex()), mFlags(flags) {
        if (ptrIsRaw && IsLaidOut()) {
            mPtr = (void *)((uintptr_t)ptr - (uintptr_t)layoutptr);
        }
    }

    void Move(Node &src) {
        mKey = src.mKey;
        mTypeIndex = src.mTypeIndex;
        mPtr = src.mPtr;
        mFlags = src.mFlags;

        src.mPtr = &src;
        src.mFlags = 0;
        src.mKey = 0;
    }

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

    std::size_t MaxSearch() const {
        return mMax;
    }

    void SetSearchLength(std::size_t searchLen) {
        mMax = std::max(mMax, (unsigned char)searchLen);
    }

    void ResetSearchLength(std::size_t searchLen) {
        mMax = searchLen;
    }

    const TypeDesc &GetTypeDesc() const {
        return Database::Get().GetIndexedTypeDesc(mTypeIndex);
    }

    void Invalidate() {
        mPtr = this;
        mKey = 0;
    }

  private:
    Key mKey;
    union {
        void *mPtr;
        Array *mArray;
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
      public:
        static std::size_t KeyIndex(std::size_t k, std::size_t tableSize, unsigned int keyShift) {
            return RotateNTo32(k, keyShift) % tableSize;
        }

        static std::size_t WrapIndex(std::size_t index, std::size_t tableSize, unsigned int keyShift) {
            return index % tableSize;
        }

        static std::size_t TableSize(std::size_t entries) {
            return AdjustHashTableSize(entries);
        }

        static std::size_t GrowRequest(std::size_t currententries, bool collisionoverrun) {
            if (collisionoverrun) {
                return (currententries * 20 / 16 + 3) & 0xFFFFFFFC;
            } else {
                return (currententries * 20 / 16 + 3) & 0x1FFFFFFC;
            }
        }

        static void *Alloc(std::size_t bytes) {
            return TableAllocFunc(bytes);
        }

        static void Free(void *ptr, std::size_t bytes) {
            TableFreeFunc(ptr, bytes);
        }
    };

    Class(Key k, ClassPrivate &privates);
    ~Class();
    void Delete() const;
    bool AddCollection(Collection *c);
    bool RemoveCollection(Collection *c);
    void *AllocLayout() const;
    void *CloneLayout(void *srcLayout) const;
    const Definition *GetDefinition(Key key) const;
    std::size_t GetNumDefinitions() const;
    Key GetFirstDefinition() const;
    Key GetNextDefinition(Key prev) const;
    std::size_t GetNumCollections() const;
    Key GetFirstCollection() const;
    Key GetNextCollection(Key prev) const;
    void SetTableBuffer(void *fixedAlloc, std::size_t bytes);
    std::size_t GetTableNodeSize() const;
    void CopyLayout(void *srcLayout, void *dstLayout) const;
    void FreeLayout(void *layout) const;
    const Collection *GetCollection(Key key) const;
    const Collection *GetCollectionWithDefault(Key key) const;

    bool IsReferenced() const {
        return mRefCount > 0;
    }

    void Release() const {
        if (mRefCount >= 2) {
            mRefCount--;
        } else {
            mRefCount = 0;
            Database::Get().Delete(this);
        }
    }

    Key GetKey() const {
        return mKey;
    }

    void AddRef() const {
        mRefCount++;
    }

    // TODO private? how?
    void operator delete(void *ptr, std::size_t bytes) {
        Free(ptr, bytes, "Attrib::Class");
    }

    friend class Collection;
    friend class ClassPrivate;

  private:
    Key mKey;                      // offset 0x0, size 0x4
    mutable std::size_t mRefCount; // offset 0x4, size 0x4
    ClassPrivate &mPrivates;       // offset 0x8, size 0x4
};

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

    RefSpec() : mClassKey(0), mCollectionKey(0), mCollectionPtr(nullptr) {}

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
        if (mDataPointer) {
            return index == 0 ? mDataPointer : nullptr;
        }
        return GetInternalPointer(index);
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
        Free(ptr, bytes, "Attrib::Attribute");
    }

    Key GetCollectionKey() const {
        // TODO
    }

    const void *GetDataAddress() const {
        return mDataPointer;
    }

    Node *GetInternal() const {
        return mInternal;
    }

    bool Get(unsigned int index, RefSpec &result) {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetElementPointer(index));

        if (resultptr) {
            result = *resultptr;
            return true;
        }

        return false;
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

    bool Valid() {
        return mCurrentKey != 0;
    }

    Key GetKey() const {
        return mCurrentKey;
    }

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
        Free(ptr, bytes, "Attrib::Instance");
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
        if (mLayoutPtr == nullptr) {
            mLayoutPtr = const_cast<void *>(DefaultDataArea(bytes));
        }
    }

    bool IsValid() const {
        return mCollection != nullptr;
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
        kArray = 1 << 0,
        kInLayout = 1 << 1,
        kIsBound = 1 << 2,
        kIsNotSearchable = 1 << 3,
    };

    Definition(unsigned int k) {
        mKey = k;
    }

    bool IsArray() const {
        return GetFlag(kArray);
    }

    bool InLayout() const {
        return GetFlag(kInLayout);
    }

    bool IsBound() const {
        return GetFlag(kIsBound);
    }

    bool IsNotSearchable() const {
        return GetFlag(kIsNotSearchable);
    }

    bool GetFlag(unsigned int f) const {
        return mFlags & f;
    }

    Key GetKey() const {
        return mKey;
    }

    unsigned int GetType() const {
        return mType;
    }

    unsigned int GetOffset() const {
        return mOffset;
    }

    unsigned int GetSize() const {
        return mSize;
    }

    unsigned int GetMaxCount() const {
        return mMaxCount;
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
        Free(ptr, bytes, "Attrib::TAttrib");
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
