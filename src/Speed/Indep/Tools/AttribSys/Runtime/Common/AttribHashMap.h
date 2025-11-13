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
            // TODO handle collisionoverflow
            return (((currententries * 0x14) >> 4) + 3) & 0x1FFFFFFC;
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
            if (mKeyShift > 16 && !exactFit) {
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
            // TODO why is there a Node::IsValid call somewhere here? it's there sometimes and sometimes not
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
