#ifndef ATTRIBSYS_VEC_HASH_MAP_64_H
#define ATTRIBSYS_VEC_HASH_MAP_64_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>
#include <types.h>

// total size: 0x10
template <typename KeyType, typename T, typename U, bool Unk2, std::size_t Unk3> class VecHashMap {
  public:
    // total size: 0x10
    class Node {
      public:
        bool IsValid() const {
            return mKey != mMax;
            // return mKey != mPtr; // TODO
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

        unsigned int MaxSearch() const {
            return mMax;
        }

      private:
        KeyType mKey;     // offset 0x0, size 0x8
        T *mPtr;          // offset 0x8, size 0x4
        std::size_t mMax; // offset 0xC, size 0x4
    };

    unsigned int FindIndex(unsigned int key) const {
        unsigned short uVar1;
        Node *pVVar2;
        unsigned int uVar3;
        Node *pVVar4;
        unsigned int actualIndex;
        Node *table;
        unsigned int uVar7;

        if (mNumEntries == 0) {
            return mTableSize;
        }
        table = mTable;
        // TODO TablePolicy::KeyIndex(key, mTableSize, 0)
        actualIndex = key % mTableSize;
        unsigned int searchLen = 0;
        unsigned int maxSearchLen = table[actualIndex].MaxSearch();
        for (searchLen = 0; searchLen < maxSearchLen; searchLen++) {
            pVVar4 = table + actualIndex;
            if (key == pVVar4->Key())
                break;
            actualIndex = (actualIndex + 1) % mTableSize;
        }
        table += actualIndex;
        if (key != table->Key()) {
            return (unsigned int)mTableSize;
        }
        return actualIndex;
    }

    bool ValidIndex(unsigned int index) const {
        return index < mTableSize && mTable[index].IsValid();
    }

    T *Find(unsigned int key) const {
        unsigned int index = FindIndex(key);
        if (ValidIndex(index)) {
            return mTable[index].Get();
        }
        return 0;
    }

    std::size_t Size() const {
        return mNumEntries;
    }

  private:
    Node *mTable;                      // offset 0x0, size 0x4
    std::size_t mTableSize;            // offset 0x4, size 0x4
    std::size_t mNumEntries;           // offset 0x8, size 0x4
    unsigned int mFixedAlloc : 1;      // offset 0xC, size 0x4
    unsigned int mWorstCollision : 31; // offset 0xC, size 0x4
};

#endif
