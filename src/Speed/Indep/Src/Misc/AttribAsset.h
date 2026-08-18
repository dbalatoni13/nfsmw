#ifndef MISC_ATTRIB_ASSET_H
#define MISC_ATTRIB_ASSET_H

#include "Speed/Indep/Tools/AttribSys/Runtime/AttribLoadAndGo.h"

class VaultMap : public std::map<unsigned int, Attrib::Vault *> {};

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

class FileMap : public std::map<Attrib::AssetID, FileRecord> {};

bool AddDepFile(const char *filename, void *data, size_t bytes);
bool RemoveDepFile(const char *filename);
Attrib::Vault *AddVault(const char *filename, void *data, size_t bytes);
void RemoveVault(const char *filename);

#endif
