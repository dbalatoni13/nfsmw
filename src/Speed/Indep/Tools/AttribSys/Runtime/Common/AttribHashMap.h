#ifndef ATTRIBSYS_ATTRIB_HASH_MAP_H
#define ATTRIBSYS_ATTRIB_HASH_MAP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

// Credit: Brawltendo
namespace Attrib {

class HashMap {
  public:
    class HashMapTablePolicy {
      public:
        static unsigned int KeyIndex(Key k, unsigned int tableSize, unsigned int keyShift) {
            return RotateNTo32(k, keyShift) % tableSize;
        }

        static unsigned int WrapIndex(Key k, unsigned int tableSize, unsigned int keyShift) {
            return k % tableSize;
        }

        static void Free(void *ptr, std::size_t bytes) {
            Attrib::Free(ptr, bytes, nullptr);
        }

        static std::size_t GrowRequest(unsigned int currententries, bool collisionoverflow) {
            // TODO handle collisionoverflow
            return (((currententries * 0x14) >> 4) + 3) & 0x1FFFFFFC;
        }
    };

    void operator delete(void *ptr, std::size_t bytes) {
        Free(ptr, bytes, nullptr);
    }

    HashMap(std::size_t reservationSize, unsigned int keyShift, bool exactFit) {
        mTable = nullptr;
        mTableSize = 0;
        mNumEntries = 0;
        mWorstCollision = 0;
        mKeyShift = 0;
        if (reservationSize != 0) {
            RebuildTable(exactFit ? reservationSize : HashMapTablePolicy::GrowRequest(reservationSize - 1, false));
        }
    }

    ~HashMap() {
        if (mTable) {
            HashMapTablePolicy::Free(mTable, sizeof(*mTable) * mTableSize);
        }
    }

    void RebuildTable(std::size_t requestedCount) {
        // TODO it exists out of line
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

    unsigned int FindIndex(Key key) const {
        if (mNumEntries == 0 || key == 0) {
            return mTableSize;
        }
        Node *table = mTable;
        unsigned int actualIndex = HashMapTablePolicy::KeyIndex(key, mTableSize, mKeyShift);
        unsigned int searchLen = 0;
        unsigned int maxSearchLen = table[actualIndex].MaxSearch();
        while (searchLen < maxSearchLen && table[actualIndex].GetKey() != key) {
            // TODO why is there a Node::IsValid call somewhere here? it's there sometimes and sometimes not
            table[actualIndex].IsValid();
            actualIndex = HashMapTablePolicy::WrapIndex(actualIndex + 1, mTableSize, 0);
            searchLen++;
        }
        return table[actualIndex].GetKey() != key ? mTableSize : actualIndex;
    }

    Node *Find(Key key) const {
        if (!key)
            return 0;

        unsigned int index = FindIndex(key);
        return ValidIndex(index) ? &mTable[index] : nullptr;
    }

    unsigned int GetNextValidIndex(unsigned int startPoint) const {
        unsigned int index = startPoint + 1;
        for (; index < mTableSize && !mTable[index].IsValid(); index++) {
        }
        return index;
    }

    unsigned int GetKeyAtIndex(unsigned int index) const {
        if (ValidIndex(index)) {
            (void)ValidIndex(index);
            return mTable[index].GetKey();
        }
        return 0;
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
