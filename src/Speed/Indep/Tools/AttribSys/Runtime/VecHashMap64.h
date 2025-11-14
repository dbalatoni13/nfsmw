#ifndef ATTRIBSYS_VEC_HASH_MAP_64_H
#define ATTRIBSYS_VEC_HASH_MAP_64_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

// total size: 0x10
template <typename KeyType, typename T, typename Policy, bool Unk2, std::size_t Unk3> class VecHashMap {
  public:
    // total size: 0xC or 0x10
    class Node {
      public:
        void *operator new(std::size_t, void *place) {
            return place;
        }

        Node() : mKey(0), mPtr((T *)this), mMax(0) {}

        Node(KeyType key, T *ptr) : mKey(key), mPtr(ptr) {}

        void Move(Node &src) {
            mKey = src.mKey;
            mPtr = src.mPtr;

            src.mKey = 0;
            src.mPtr = (T *)&src;
        }

        bool IsValid() const {
            return (void *)mPtr != (void *)this;
        }

        void Invalidate() {
            mKey = 0;
            mPtr = (T *)this;
        }

        T *Get() const {
            if (IsValid()) {
                return mPtr;
            }
            return nullptr;
        }

        KeyType Key() const {
            return IsValid() ? mKey : 0;
        }

        std::size_t MaxSearch() const {
            return mMax;
        }

        void SetSearchLength(std::size_t searchLen) {
            mMax = std::max(mMax, searchLen);
        }

        void ResetSearchLength(std::size_t searchLen) {
            mMax = searchLen;
        }

      private:
        KeyType mKey;     // offset 0x0, size 0x4 or 0x8
        T *mPtr;          // offset 0x4 or 0x8, size 0x4
        std::size_t mMax; // offset 0x8 or 0xC, size 0x4
    };

  public:
    VecHashMap(std::size_t reservationSize) : mTable(nullptr), mTableSize(0), mNumEntries(0), mFixedAlloc(0), mWorstCollision(0) {
        if (reservationSize != 0) {
            RebuildTable(reservationSize);
        }
    }

    ~VecHashMap() {
        Clear();
    }

    std::size_t Size() const {
        return mNumEntries;
    }

    std::size_t Capacity() const {
        return mTableSize;
    }

    void Clear() {
        for (std::size_t i = 0; i < mTableSize && mNumEntries != 0; i++) {
            if (mTable[i].IsValid()) {
                delete mTable[i].Get();
                mNumEntries--;
            }
        }
        if (mFixedAlloc == 0) {
            Policy::Free(mTable, mTableSize * sizeof(Node));
            mTable = nullptr;
            mTableSize = 0;
        }
        mNumEntries = 0;
        mWorstCollision = 0;
    }

    bool Add(KeyType key, T *ptr) {
        bool result = InternalAdd(key, ptr);
        if (mWorstCollision > Unk3) {
            RebuildTable(Policy::GrowRequest(mTableSize, true));
        }
        return result;
    }

    T *RemoveIndex(std::size_t actualIndex) {
        if (!ValidIndex(actualIndex)) {
            return nullptr;
        }
        T *result = mTable[actualIndex].Get();
        KeyType key = mTable[actualIndex].Key();
        mTable[actualIndex].Invalidate();
        mNumEntries--;

        std::size_t freedIndex = UpdateSearchLength(Policy::KeyIndex(key, mTableSize, 0), actualIndex);
        // TODO UNSOLVED making it a while loop changes the output, in AttribHashMap.h it's a while loop
        for (; freedIndex < mTableSize; freedIndex = UpdateSearchLength(freedIndex, freedIndex)) {
        }
        return result;
    }

    T *Remove(KeyType key) {
        std::size_t actualIndex = FindIndex(key);
        return RemoveIndex(actualIndex);
    }

    std::size_t FindIndex(KeyType key) const {
        if (mNumEntries == 0) {
            return mTableSize;
        }
        Node *table = mTable;
        std::size_t actualIndex = Policy::KeyIndex(key, mTableSize, 0);
        std::size_t searchLen = 0;
        std::size_t maxSearchLen = table[actualIndex].MaxSearch();
        while (searchLen < maxSearchLen && table[actualIndex].Key() != key) {
            // TODO why is there a Node::IsValid call somewhere here?
            if (table[actualIndex].IsValid()) {
            }
            actualIndex = Policy::WrapIndex(actualIndex + 1, mTableSize, 0);
            searchLen++;
        }
        return table[actualIndex].Key() != key ? mTableSize : actualIndex;
    }

    bool ValidIndex(std::size_t index) const {
        return index < mTableSize && mTable[index].IsValid();
    }

    T *Find(KeyType key) const {
        if (key == 0) {
            return nullptr;
        }
        unsigned int index = FindIndex(key);
        return ValidIndex(index) ? mTable[index].Get() : nullptr;
    }

    void SetTableBuffer(void *fixedAlloc, std::size_t bytes) {
        std::size_t capacity = bytes / sizeof(Node);
        Node *oldTable = mTable;
        std::size_t oldSize = mTableSize;
        bool needFree = !mFixedAlloc;

        mNumEntries = 0;
        mWorstCollision = 0;
        if (!fixedAlloc) {
            mFixedAlloc = false;
            mTableSize = capacity;
            mTable = reinterpret_cast<Node *>(Policy::Alloc(mTableSize * sizeof(Node)));
        } else {
            mFixedAlloc = true;
            mTableSize = capacity;
            mTable = reinterpret_cast<Node *>(fixedAlloc);
        }
        CopyFromOldTable(oldTable, oldSize, needFree);
    }

  private:
    bool InternalAdd(KeyType key, T *ptr) {
        if (mNumEntries == mTableSize) {
            RebuildTable(Policy::GrowRequest(mTableSize, false));
        }
        std::size_t targetIndex = Policy::KeyIndex(key, mTableSize, 0);
        std::size_t actualIndex = targetIndex;
        std::size_t searchLen = 0;
        std::size_t tableSize = mTableSize;
        while (mTable[actualIndex].IsValid()) {
            if (mTable[actualIndex].Key() == key) {
                return false;
            }
            searchLen++;
            actualIndex = Policy::WrapIndex(actualIndex + 1, tableSize, 0);
        }
        if (&mTable[actualIndex]) {
            new (&mTable[actualIndex]) Node(key, ptr);
        }
        mTable[targetIndex].SetSearchLength(searchLen);
        if (searchLen > mWorstCollision) {
            mWorstCollision = searchLen;
        }
        mNumEntries++;
        return true;
    }

    std::size_t UpdateSearchLength(std::size_t targetIndex, std::size_t freeIndex) {
        if (targetIndex == freeIndex && mTable[targetIndex].MaxSearch() == 0) {
            targetIndex = Policy::WrapIndex(targetIndex + mTableSize - mWorstCollision, mTableSize, 0);
            std::size_t distance = mWorstCollision;
            while (mTable[targetIndex].MaxSearch() < distance && distance > 0) {
                targetIndex = Policy::WrapIndex(targetIndex + 1, mTableSize, 0);
                distance--;
            }
            if (distance == 0) {
                return static_cast<std::size_t>(-1);
            }
        }

        std::size_t maxSearch = mTable[targetIndex].MaxSearch();
        std::size_t worstIndex = Policy::WrapIndex(targetIndex + maxSearch, mTableSize, 0);

        if (mTable[worstIndex].IsValid()) {
            Policy::KeyIndex(mTable[worstIndex].Key(), mTableSize, 0);
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
            std::size_t index = Policy::WrapIndex(targetIndex + searchLen, mTableSize, 0);
            if (Policy::KeyIndex(mTable[index].Key(), mTableSize, 0) == targetIndex) {
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

    void CopyFromOldTable(Node *oldTable, std::size_t oldSize, bool needFree) {
        for (std::size_t i = 0; i < mTableSize; i++) {
            // TODO UNSOLVED
            if (&mTable[i]) {
                new (&mTable[i]) Node();
            }
        }
        if (!oldTable) {
            return;
        }
        for (std::size_t i = 0; i < oldSize; i++) {
            if (oldTable[i].IsValid()) {
                InternalAdd(oldTable[i].Key(), oldTable[i].Get());
            }
        }
        if (needFree) {
            Policy::Free(oldTable, oldSize * sizeof(Node));
        }
    }

    void RebuildTable(std::size_t requestedCount) {
        if (requestedCount != 0) {
            requestedCount--;
            do {
                requestedCount++;
                std::size_t tableSize = Policy::TableSize(requestedCount);
                Node *oldTable = mTable;
                std::size_t oldSize = mTableSize;

                mTableSize = tableSize;
                mNumEntries = 0;
                mWorstCollision = 0;
                mTable = reinterpret_cast<Node *>(Policy::Alloc(mTableSize * sizeof(Node)));
                CopyFromOldTable(oldTable, oldSize, true);
            } while (mWorstCollision > Unk3);
        }
    }

    // TODO private
  protected:
    Node *mTable;                      // offset 0x0, size 0x4
    std::size_t mTableSize;            // offset 0x4, size 0x4
    std::size_t mNumEntries;           // offset 0x8, size 0x4
    unsigned int mFixedAlloc : 1;      // offset 0xC, size 0x4
    unsigned int mWorstCollision : 31; // offset 0xC, size 0x4
};

#endif
