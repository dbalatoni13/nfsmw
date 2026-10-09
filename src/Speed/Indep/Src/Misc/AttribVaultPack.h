//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
#ifndef ATTRIBVAULTPACK_H__
#define ATTRIBVAULTPACK_H__ // Decl: 18

#define ATTRIBVAULTPACK_HEADER_MAGIC ("VPAK") // Decl: 20

#include "Speed/Indep/bWare/Inc/bWare.hpp"

// total size: 0x10
// Decl: 28
struct AttribVaultPackHeader {
    char mMagic[4];            // offset 0x0, size 0x4
    uint32 mNumEntries;        // offset 0x4, size 0x4
    uint32 mStringBlockOffset; // offset 0x8, size 0x4
    uint32 mStringBlockSize;   // offset 0xC, size 0x4
};

// total size: 0x14
// Decl: 39
struct AttribVaultPackEntry {
    uint32 mVaultNameOffset; // offset 0x0, size 0x4
    uint32 mBinSize;         // offset 0x4, size 0x4
    uint32 mVltSize;         // offset 0x8, size 0x4
    uint32 mBinOffset;       // offset 0xC, size 0x4
    uint32 mVltOffset;       // offset 0x10, size 0x4
};

// total size: 0x24
// Decl: 48
struct AttribVaultPackImage {
    const char *GetVaultName(int index) {
        return reinterpret_cast<const char *>(reinterpret_cast<unsigned char *>(&this->mHeader) + this->mHeader.mStringBlockOffset +
                                              this->mEntry[index].mVaultNameOffset);
    }

    int GetVaultIndex(const char *name) {
        for (int onEntry = 0; onEntry < static_cast<int>(this->mHeader.mNumEntries); onEntry++) {
            const char *entryName = this->GetVaultName(onEntry);
            if (bStrCmp(name, entryName) == 0) {
                return onEntry;
            }
        }
        return -1;
    }

    AttribVaultPackEntry &GetEntry(int index) {
        return this->mEntry[index];
    }

    unsigned char *GetData(unsigned int offset) {
        return reinterpret_cast<unsigned char *>(&this->mHeader) + offset;
    }

    void EndianSwap() {
        bPlatEndianSwap(&this->mHeader.mNumEntries);
        bPlatEndianSwap(&this->mHeader.mStringBlockOffset);
        bPlatEndianSwap(&this->mHeader.mStringBlockSize);
        for (int onEntry = 0; onEntry < static_cast<int>(this->mHeader.mNumEntries); onEntry++) {
            AttribVaultPackEntry &entry = GetEntry(onEntry);
            bPlatEndianSwap(&entry.mVaultNameOffset);
            bPlatEndianSwap(&entry.mBinSize);
            bPlatEndianSwap(&entry.mVltSize);
            bPlatEndianSwap(&entry.mBinOffset);
            bPlatEndianSwap(&entry.mVltOffset);
        }
    }

    AttribVaultPackHeader mHeader;  // offset 0x0, size 0x10
    AttribVaultPackEntry mEntry[1]; // offset 0x10, size 0x14
};

#endif
