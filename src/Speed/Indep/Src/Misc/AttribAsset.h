#ifndef MISC_ATTRIB_ASSET_H
#define MISC_ATTRIB_ASSET_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Tools/AttribSys/Runtime/AttribLoadAndGo.h"

// TODO is this right to be here?

namespace Attrib {

typedef HashInt AssetID;
typedef HashInt ExportID;
typedef HashInt TypeID;

class IGarbageCollector {
  public:
    virtual void ReleaseData(unsigned int id, void *data, size_t bytes) = 0;
};

}; // namespace Attrib

class VaultMap : public std::map<unsigned int, Attrib::Vault *> {
  public:
    VaultMap();
};

class FileRecord {
  public:
    FileRecord(void *data, size_t bytes) {
        mRefCount = 0;
        mData = data;
        mBytes = bytes;
    }

    unsigned int mRefCount; // offset 0x0, size 0x4
    void *mData;            // offset 0x4, size 0x4
    size_t mBytes;          // offset 0x8, size 0x4
};

class FileMap : public std::map<Attrib::AssetID, FileRecord> {
  public:
    FileMap();
};

#endif
