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
    unsigned int FindIndex(unsigned int key);

    std::size_t Size() const {
        return mNumEntries;
    }

    bool ValidIndex(unsigned int index) {
        return index < mTableSize && mTable[index].IsValid();
    }

    Node *Find(unsigned int key) {
        if (!key)
            return 0;

        unsigned int index = FindIndex(key);
        return ValidIndex(index) ? &mTable[index] : nullptr;
    }

    struct HashMapTablePolicy {
        static unsigned int KeyIndex(unsigned int k, unsigned int tableSize, unsigned int keyShift) {
            return RotateNTo32(k, keyShift) % tableSize;
        }
    };

  private:
    Node *mTable;
    std::size_t mTableSize;
    std::size_t mNumEntries;
    unsigned short mWorstCollision;
    unsigned short mKeyShift;
};

unsigned int ScanForValidKey(const HashMap &v, std::size_t index);

} // namespace Attrib

#endif
