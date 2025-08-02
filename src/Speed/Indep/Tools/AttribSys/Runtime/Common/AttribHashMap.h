#ifndef ATTRIBSYS_ATTRIB_HASH_MAP_H
#define ATTRIBSYS_ATTRIB_HASH_MAP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "../AttribNode.h"

// Credit: Brawltendo
namespace Attrib {

// Rotates (v) by (amount) bits
unsigned int RotateNTo32(unsigned int v, unsigned int amount) {
    return (v << amount) | (v >> (32 - amount));
}

class HashMap {
  public:
    bool ValidIndex(unsigned int index) {
        return index < mTableSize && mTable[index].IsValid();
    }
    unsigned int FindIndex(unsigned int key);
    class Node *Find(unsigned int key) {
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
    class Node *mTable;
    unsigned int mTableSize;
    unsigned int mNumEntries;
    unsigned short mWorstCollision;
    unsigned short mKeyShift;
};

} // namespace Attrib

#endif
