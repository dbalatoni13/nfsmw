#ifndef ATTRIBSYS_ATTRIB_PRIVATE_H
#define ATTRIBSYS_ATTRIB_PRIVATE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "../AttribSys.h"
#include "../VecHashMap64.h"

// Credit: Brawltendo
namespace Attrib {

class Private {
  public:
    int GetLength() const;

  private:
    unsigned char mData[8];
};

// total size: 0x3C
class ClassPrivate : public Class {
  public:
    // total size: 0x10
    class CollectionHashMap : public VecHashMap<unsigned int, Attrib::Collection, Attrib::Class::TablePolicy, true, 40> {};

    // TODO this is inline
    ClassPrivate(const struct ClassLoadData &loadData, Vault *v);

    HashMap mLayoutTable;           // offset 0xC, size 0x10
    CollectionHashMap mCollections; // offset 0x1C, size 0x10
    unsigned short mLayoutSize;     // offset 0x2C, size 0x2
    unsigned short mNumDefinitions; // offset 0x2E, size 0x2
    Definition *mDefinitions;       // offset 0x30, size 0x4
    struct Vault *mSource;          // offset 0x34, size 0x4
    const char *mNamePtr;           // offset 0x38, size 0x4
};

// TODO move?
Key ScanForValidKey(const ClassPrivate::CollectionHashMap &v, std::size_t index);

} // namespace Attrib

#endif
