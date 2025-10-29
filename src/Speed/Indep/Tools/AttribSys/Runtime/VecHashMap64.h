#ifndef ATTRIBSYS_VEC_HASH_MAP_64_H
#define ATTRIBSYS_VEC_HASH_MAP_64_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

// total size: 0x10
template <typename KeyType, typename T, typename U, bool Unk2, std::size_t Unk3> class VecHashMap {
  public:
    // total size: 0x10
    class Node {
      public:
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

      private:
        KeyType mKey;     // offset 0x0, size 0x8
        T *mPtr;          // offset 0x8, size 0x4
        std::size_t mMax; // offset 0xC, size 0x4
    };

    unsigned int FindIndex(unsigned int key) const {
        if (mNumEntries == 0) {
            return mTableSize;
        }
        Node *table = mTable;
        unsigned int actualIndex = Attrib::Class::TablePolicy::KeyIndex(key, mTableSize, 0);
        unsigned int searchLen = 0;
        unsigned int maxSearchLen = table[actualIndex].MaxSearch();
        while (searchLen < maxSearchLen && table[actualIndex].Key() != key) {
            // TODO why is there a Node::IsValid call somewhere here?
            actualIndex = Attrib::Class::TablePolicy::WrapIndex(actualIndex + 1, mTableSize, 0);
            searchLen++;
        }
        return table[actualIndex].Key() != key ? mTableSize : actualIndex;
    }

    bool ValidIndex(unsigned int index) const {
        return index < mTableSize && mTable[index].IsValid();
    }

    T *Find(unsigned int key) const {
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
    Node *mTable;                      // offset 0x0, size 0x4
    std::size_t mTableSize;            // offset 0x4, size 0x4
    std::size_t mNumEntries;           // offset 0x8, size 0x4
    unsigned int mFixedAlloc : 1;      // offset 0xC, size 0x4
    unsigned int mWorstCollision : 31; // offset 0xC, size 0x4
};

#endif
