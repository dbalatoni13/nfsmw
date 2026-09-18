#ifndef ATTRIBSYS_ATTRIB_HASH_MAP_H
#define ATTRIBSYS_ATTRIB_HASH_MAP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

// Credit: Brawltendo
namespace Attrib {

// c36attx e1: movidas desde AttribSys.h -- es donde las tiene el original
// (attribhashmap.h:44-215 y :385-466 en debug_lines), y de eso depende que
// Definition::GetFlag salga DELANTE del bloque de Array en finish_file.

// TODO move to AttribHashMap.h
class Array {
#define Flag_AlignedAt16 (1 << 15)
  private:
    // Returns the base location of this array's data
    unsigned char *BasePointer() const {
        return (unsigned char *)(&this[1]);
    }

    void *Data(unsigned int byteindex) const {
        unsigned char *base = BasePointer(); // unused
        return (void *)((unsigned char *)(&this[1]) + GetPad() + byteindex);
    }

  public:
    void SetTypeIndex(uint16_t typeIndex) {
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

        return *this;
    }

    bool IsReferences() const {
        return mSize == 0;
    }

    unsigned short GetTypeIndex() const {
        return mEncodedTypePad & 0x7fff;
    }

    std::size_t GetTypeSize() const {
        return mSize;
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

    bool SetCount(unsigned int newCount) {
        if (newCount > mAlloc) {
            return false;
        } else {
            if (IsReferences()) {
                for (unsigned int i = mCount; i < newCount; i++) {
                    SetData(i, nullptr);
                }
            }
            mCount = newCount;
            return true;
        }
    }

    void *GetData(unsigned int index) const {
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

    void SetData(unsigned int index, void *value) {
        if (IsReferences()) {
            ITypeHandler *typeHandler = GetTypeDesc().GetHandler();
            typeHandler->Release(GetData(index));
            reinterpret_cast<void **>(Data(0))[index] = typeHandler->Retain(value);
        } else if (value) {
            memcpy(GetData(index), value, mSize);
        } else {
            memset(GetData(index), 0, mSize);
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

    uint16_t mAlloc;
    uint16_t mCount;
    uint16_t mSize;
    uint16_t mEncodedTypePad;
};

#undef Flag_AlignedAt16

// TOOD move to AttribHashMap.h
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
            return (void *)((uintptr_t)(layoutptr) + (uintptr_t)(mPtr));
        } else {
            return mPtr;
        }
    }

    Array *GetArray(void *layoutptr) const {
        if (IsLaidOut()) {
            return (Array *)((uintptr_t)(layoutptr) + (uintptr_t)(mArray));
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
    uint16_t mTypeIndex;
    uint8_t mMax;
    uint8_t mFlags;
};

class HashMap {
  public:
    class HashMapTablePolicy {
      public:
        static std::size_t KeyIndex(Key k, std::size_t tableSize, unsigned int keyShift) {
            return RotateNTo32(k, keyShift) % tableSize;
        }

        static std::size_t WrapIndex(Key k, std::size_t tableSize, unsigned int keyShift) {
            return k % tableSize;
        }

        static void *Alloc(std::size_t bytes) {
            return Attrib::Alloc(bytes, "Attrib::HashMapTable");
        }

        static void Free(void *ptr, std::size_t bytes) {
            Attrib::Free(ptr, bytes, "Attrib::HashMapTable");
        }

        static std::size_t TableSize(std::size_t entries) {
            return AdjustHashTableSize(entries);
        }

        static std::size_t GrowRequest(std::size_t currententries, bool collisionoverflow) {
            if (collisionoverflow) {
                return (currententries * 20 / 16 + 3) & 0xFFFFFFFC;
            } else {
                return (currententries * 20 / 16 + 3) & 0x1FFFFFFC;
            }
        }
    };

    void operator delete(void *ptr, std::size_t bytes) {
        Free(ptr, bytes, "Attrib::HashMap");
    }

    HashMap(std::size_t reservationSize, unsigned int keyShift, bool exactFit)
        : mTable(nullptr), mTableSize(0), mNumEntries(0), mWorstCollision(0), mKeyShift(keyShift) {
        if (reservationSize != 0) {
            RebuildTable(exactFit ? reservationSize : HashMapTablePolicy::GrowRequest(reservationSize - 1, false));
        }
    }

    ~HashMap() {
        if (mTable) {
            HashMapTablePolicy::Free(mTable, sizeof(*mTable) * mTableSize);
        }
    }

    bool Add(Key key, unsigned int type, void *ptr, bool ptrIsRaw, unsigned char flags, bool exactFit, void *layoutptr) {
        if (mNumEntries == mTableSize) {
            RebuildTable(HashMapTablePolicy::GrowRequest(mTableSize, false));
        }
        std::size_t searchLen = 0;
        std::size_t targetIndex = HashMapTablePolicy::KeyIndex(key, mTableSize, mKeyShift);
        std::size_t actualIndex = PreFlightAdd(key, targetIndex, searchLen);

        if (actualIndex < mTableSize) {
            new (&mTable[actualIndex]) Node(key, type, ptr, ptrIsRaw, flags, layoutptr);
            PostFlightAdd(targetIndex, searchLen);
            if (mWorstCollision > 16 && !exactFit) {
                RebuildTable(HashMapTablePolicy::GrowRequest(mTableSize, true));
            }
            return true;
        } else {
            return false;
        }
    }

    void RebuildTable(std::size_t requestedCount) {
        if (requestedCount == 0) {
            return;
        }
        std::size_t tableSize = HashMapTablePolicy::TableSize(requestedCount);
        Node *oldTable = mTable;
        std::size_t oldSize = mTableSize;
        mTableSize = tableSize;
        mNumEntries = 0;
        mWorstCollision = 0;
        mTable = new (HashMapTablePolicy::Alloc(mTableSize * sizeof(Node))) Node();
        for (int i = 1; i < mTableSize; i++) {
            new (&mTable[i]) Node();
        }
        if (oldTable) {
            for (int i = 0; i < oldSize; i++) {
                if (oldTable[i].IsValid()) {
                    oldTable[i].ResetSearchLength(0);
                    Transfer(oldTable[i]);
                }
            }
            HashMapTablePolicy::Free(oldTable, oldSize * sizeof(Node));
        }
    }

    void ClearForRelease() {
        mNumEntries = 0;
    }

    std::size_t Size() const {
        return mNumEntries;
    }

    bool ValidIndex(unsigned int index) const {
        return index < mTableSize && mTable[index].IsValid();
    }

    std::size_t FindIndex(Key key) const {
        if (mNumEntries == 0 || key == 0) {
            return mTableSize;
        }
        Node *table = mTable;
        unsigned int actualIndex = HashMapTablePolicy::KeyIndex(key, mTableSize, mKeyShift);
        unsigned int searchLen = 0;
        unsigned int maxSearchLen = table[actualIndex].MaxSearch();
        while (searchLen < maxSearchLen && table[actualIndex].GetKey() != key) {
            if (table[actualIndex].IsValid()) {
            }
            actualIndex = HashMapTablePolicy::WrapIndex(actualIndex + 1, mTableSize, 0);
            searchLen++;
        }
        return table[actualIndex].GetKey() != key ? mTableSize : actualIndex;
    }

    Node *Find(Key key) const {
        if (!key)
            return nullptr;

        unsigned int index = FindIndex(key);
        if (ValidIndex(index)) {
            return &mTable[index];
        } else {
            return nullptr;
        }
    }

    unsigned int GetNextValidIndex(unsigned int startPoint) const {
        unsigned int index = startPoint + 1;
        for (; index < mTableSize && !mTable[index].IsValid(); index++) {
        }
        return index;
    }

    Node *GetNodeAtIndex(unsigned int index) const {
        // TODO
        if (ValidIndex(index)) {
            return &mTable[index];
        }
        return nullptr;
    }

    unsigned int GetKeyAtIndex(unsigned int index) const {
        if (ValidIndex(index)) {
            (void)ValidIndex(index);
            return mTable[index].GetKey();
        }
        return 0;
    }

    void *Remove(Node *node, void *layoutptr, bool maintainTableInvariant) {
        if (node->IsValid()) {
            // useless but needed to match
        }
        Key key = node->GetKey();
        void *result = node->GetPointer(layoutptr);
        node->Invalidate();
        mNumEntries--;

        if (maintainTableInvariant) {
            std::size_t actualIndex = node - mTable; // or directly and actualIndex is used for something else?
            std::size_t freedIndex = UpdateSearchLength(HashMapTablePolicy::KeyIndex(key, mTableSize, mKeyShift), actualIndex);
            while (freedIndex < mTableSize) {
                freedIndex = UpdateSearchLength(freedIndex, freedIndex);
            }
        } else {
            node->ResetSearchLength(0);
        }
        return result;
    }

    unsigned int CountSearchCacheLines(Key key, unsigned int lineSize) const {
        unsigned int result = 0;

        if (this->mNumEntries == 0 || key == 0) {
            return result;
        }

        unsigned int prevline = 0;
        Node *table = this->mTable;
        unsigned int actualIndex = HashMapTablePolicy::KeyIndex(key, this->mTableSize, this->mKeyShift);
        unsigned int searchLen = 0;
        unsigned int maxSearchLen = table[actualIndex].MaxSearch();
        unsigned int currline = (uintptr_t)&table[actualIndex] >> (lineSize & 0x3f); // TODO huh?

        if (currline != 0) {
            result = 1;
            prevline = currline;
        }

        for (;;) {
            if (searchLen >= maxSearchLen) {
                break;
            }

            if (table[actualIndex].GetKey() == key) {
                break;
            }

            actualIndex = HashMapTablePolicy::WrapIndex(actualIndex + 1, this->mTableSize, this->mKeyShift);
            currline = (uintptr_t)&this->mTable[actualIndex] >> (lineSize & 0x3f);

            if (currline != prevline) {
                prevline = currline;
                result++;
            }

            searchLen++;
        }

        return result;
    }

  private:

    void Transfer(Node &src) {
        std::size_t searchLen = 0;
        std::size_t targetIndex = HashMapTablePolicy::KeyIndex(src.GetKey(), mTableSize, mKeyShift);
        std::size_t actualIndex = PreFlightAdd(src.GetKey(), targetIndex, searchLen);

        mTable[actualIndex].Move(src);
        PostFlightAdd(targetIndex, searchLen);
    }

    std::size_t UpdateSearchLength(std::size_t targetIndex, std::size_t freeIndex) {
        if (targetIndex == freeIndex && mTable[targetIndex].MaxSearch() == 0) {
            targetIndex = HashMapTablePolicy::WrapIndex(targetIndex + mTableSize - mWorstCollision, mTableSize, mKeyShift);
            std::size_t distance = mWorstCollision;
            while (mTable[targetIndex].MaxSearch() < distance && distance > 0) {
                targetIndex = HashMapTablePolicy::WrapIndex(targetIndex + 1, mTableSize, mKeyShift);
                distance--;
            }
            if (distance == 0) {
                return static_cast<std::size_t>(-1);
            }
        }

        std::size_t maxSearch = mTable[targetIndex].MaxSearch();
        std::size_t worstIndex = HashMapTablePolicy::WrapIndex(targetIndex + maxSearch, mTableSize, mKeyShift);

        if (mTable[worstIndex].IsValid()) {
            HashMapTablePolicy::KeyIndex(mTable[worstIndex].GetKey(), mTableSize, mKeyShift);
        }

        // useless but necessary to match, TODO probably some debug stuff going on
        if (mTable[freeIndex].IsValid()) {
        }
        if (freeIndex != worstIndex) {
            mTable[freeIndex].Move(mTable[worstIndex]);
        }
        if (mTable[worstIndex].IsValid()) {
        }

        std::size_t newMaxSearch = 0;
        for (std::size_t searchLen = 1; searchLen < maxSearch; searchLen++) {
            std::size_t index = HashMapTablePolicy::WrapIndex(targetIndex + searchLen, mTableSize, mKeyShift);
            if (HashMapTablePolicy::KeyIndex(mTable[index].GetKey(), mTableSize, mKeyShift) == targetIndex) {
                newMaxSearch = searchLen;
            }
        }

        mTable[targetIndex].ResetSearchLength(newMaxSearch);

        if (maxSearch == mWorstCollision && mTable[freeIndex].MaxSearch() < maxSearch && newMaxSearch < maxSearch) {
            mWorstCollision = 0;
            std::size_t prevWorst; // unused
            for (std::size_t i = 0; i < mTableSize && mWorstCollision < maxSearch; i++) {
                if (mTable[i].MaxSearch() > mWorstCollision) {
                    prevWorst = mWorstCollision = mTable[i].MaxSearch();
                }
            }
        }

        return worstIndex;
    }

    std::size_t PreFlightAdd(Key key, std::size_t targetIndex, std::size_t &searchLen) {
        searchLen = 0;
        while (mTable[targetIndex].IsValid()) {
            if (mTable[targetIndex].GetKey() == key) {
                return static_cast<std::size_t>(-1);
            }
            targetIndex = HashMapTablePolicy::WrapIndex(targetIndex + 1, mTableSize, 0);
            searchLen++;
        }
        return targetIndex;
    }

    void PostFlightAdd(std::size_t targetIndex, std::size_t searchLen) {
        mTable[targetIndex].SetSearchLength(searchLen);
        if (searchLen > mWorstCollision) {
            mWorstCollision = searchLen;
        }
        mNumEntries++;
    }

  private:
    Node *mTable;                   // offset 0x0, size 0x4
    std::size_t mTableSize;         // offset 0x4, size 0x4
    std::size_t mNumEntries;        // offset 0x8, size 0x4
    unsigned short mWorstCollision; // offset 0xC, size 0x2
    unsigned short mKeyShift;       // offset 0xE, size 0x2
};

} // namespace Attrib

#endif
