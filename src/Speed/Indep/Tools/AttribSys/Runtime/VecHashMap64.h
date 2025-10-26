#ifndef ATTRIBSYS_VEC_HASH_MAP_64_H
#define ATTRIBSYS_VEC_HASH_MAP_64_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

// total size: 0x10
template <typename Unk, typename T, typename U, bool Unk2, std::size_t Unk3> class VecHashMap {
  public:
    // total size: 0x10
    class Node {
        // TODO I think this depends on "Unk"
        unsigned long long mKey; // offset 0x0, size 0x8
        T *mPtr;                 // offset 0x8, size 0x4
        std::size_t mMax;        // offset 0xC, size 0x4
    };

    std::size_t Size() const {
        return mNumEntries;
    }

  private:
    // TODO fix on xbox 360
    VecHashMap::Node *mTable;          // offset 0x0, size 0x4
    std::size_t mTableSize;            // offset 0x4, size 0x4
    std::size_t mNumEntries;           // offset 0x8, size 0x4
    unsigned int mFixedAlloc : 1;      // offset 0xC, size 0x4
    unsigned int mWorstCollision : 31; // offset 0xC, size 0x4
};

#endif
