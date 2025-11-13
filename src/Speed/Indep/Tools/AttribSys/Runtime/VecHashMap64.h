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

        bool IsValid() const {
            return (void *)mPtr != (void *)this;
        }

        T *Get() const {
            if (IsValid()) {
                return mPtr;
            }
            return nullptr;
        }

        KeyType Key() const {
            if (IsValid()) {
                return mKey;
            }
            return 0;
        }

        std::size_t MaxSearch() const {
            return mMax;
        }

        void SetSearchLength(std::size_t searchLen) {
            mMax = std::max(mMax, searchLen);
        }

      private:
        KeyType mKey;     // offset 0x0, size 0x4 or 0x8
        T *mPtr;          // offset 0x4 or 0x8, size 0x4
        std::size_t mMax; // offset 0x8 or 0xC, size 0x4
    };

    VecHashMap(std::size_t reservationSize) : mTable(nullptr), mTableSize(0), mNumEntries(0), mFixedAlloc(0), mWorstCollision(0) {
        if (reservationSize != 0) {
            RebuildTable(reservationSize);
        }
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

    std::size_t Size() const {
        return mNumEntries;
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

    void CopyFromOldTable(Node *oldTable, std::size_t oldSize, bool needFree) {
        for (std::size_t i = 0; i < mTableSize; i++) {
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

  protected:
    Node *mTable;                      // offset 0x0, size 0x4
    std::size_t mTableSize;            // offset 0x4, size 0x4
    std::size_t mNumEntries;           // offset 0x8, size 0x4
    unsigned int mFixedAlloc : 1;      // offset 0xC, size 0x4
    unsigned int mWorstCollision : 31; // offset 0xC, size 0x4
};

#endif
