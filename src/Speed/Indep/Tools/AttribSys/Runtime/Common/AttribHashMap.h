#ifndef ATTRIBSYS_ATTRIB_HASH_MAP_H
#define ATTRIBSYS_ATTRIB_HASH_MAP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

// Credit: Brawltendo
namespace Attrib {

unsigned int AdjustHashTableSize(unsigned int requiredSize);

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

        static void *Alloc(std::size_t bytes) {
            return Attrib::Alloc(bytes, "Attrib::HashMapTable");
        }

        static void Free(void *ptr, std::size_t bytes) {
            Attrib::Free(ptr, bytes, "Attrib::HashMapTable");
        }

        static unsigned int TableSize(unsigned int entries) {
            return AdjustHashTableSize(entries);
        }

        static std::size_t GrowRequest(unsigned int currententries, bool collisionoverflow) {
            // TODO handle collisionoverflow
            return (((currententries * 0x14) >> 4) + 3) & 0x1FFFFFFC;
        }
    };

    void operator delete(void *ptr, std::size_t bytes) {
        Free(ptr, bytes, "Attrib::HashMap");
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
        if (requestedCount == 0) {
            return;
        }
        unsigned int tableSize = HashMapTablePolicy::TableSize(requestedCount);
        Node *oldTable = mTable;
        unsigned int oldSize = mTableSize;
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

    void Transfer(Node &src) {
        unsigned int searchLen = 0;
        unsigned int targetIndex = HashMapTablePolicy::KeyIndex(src.GetKey(), mTableSize, mKeyShift);
        unsigned int actualIndex = PreFlightAdd(src.GetKey(), targetIndex, searchLen);

        mTable[actualIndex].Move(src);
        PostFlightAdd(targetIndex, searchLen);
    }

    unsigned int PreFlightAdd(Key key, unsigned int targetIndex, unsigned int &searchLen) {
        searchLen = 0;
        while (mTable[targetIndex].IsValid()) {
            if (mTable[targetIndex].GetKey() == key) {
                return static_cast<unsigned int>(-1);
            }
            targetIndex = HashMapTablePolicy::WrapIndex(targetIndex + 1, mTableSize, 0);
            searchLen++;
        }
        return targetIndex;
    }

    void PostFlightAdd(unsigned int targetIndex, unsigned int searchLen) {
        mTable[targetIndex].SetSearchLength(searchLen);
        if (searchLen > mWorstCollision) {
            mWorstCollision = searchLen;
        }
        mNumEntries++;
    }

    void *Remove(Node *node, void *layoutptr, bool maintainTableInvariant) {
        node->IsValid(); // useless but needed to match
        Key key = node->GetKey();
        void *result = node->GetPointer(layoutptr);
        node->Invalidate();
        mNumEntries--;

        if (maintainTableInvariant) {
            std::size_t actualIndex = HashMapTablePolicy::KeyIndex(key, mTableSize, mKeyShift);
            std::size_t freedIndex = UpdateSearchLength(actualIndex, node - mTable);
            while (freedIndex < mTableSize) {
                freedIndex = UpdateSearchLength(freedIndex, freedIndex);
            }
        } else {
            node->ResetSearchLength(0);
        }
        return result;
    }

    std::size_t UpdateSearchLength(std::size_t targetIndex, std::size_t freeIndex) {
        // TODO it's out of line
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
