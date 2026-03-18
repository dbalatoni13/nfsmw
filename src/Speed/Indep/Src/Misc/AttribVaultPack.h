#ifndef MISC_ATTRIBVAULTPACK_H
#define MISC_ATTRIBVAULTPACK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bWare.hpp"

// total size: 0x10
struct AttribVaultPackHeader {
    char mMagic[4];                    // offset 0x0, size 0x4
    unsigned int mNumEntries;          // offset 0x4, size 0x4
    unsigned int mStringBlockOffset;   // offset 0x8, size 0x4
    unsigned int mStringBlockSize;     // offset 0xC, size 0x4
};

// total size: 0x14
struct AttribVaultPackEntry {
    unsigned int mVaultNameOffset; // offset 0x0, size 0x4
    unsigned int mBinSize;         // offset 0x4, size 0x4
    unsigned int mVltSize;         // offset 0x8, size 0x4
    unsigned int mBinOffset;       // offset 0xC, size 0x4
    unsigned int mVltOffset;       // offset 0x10, size 0x4
};

// total size: 0x24
struct AttribVaultPackImage {
    const char *GetVaultName(int index) {
        return reinterpret_cast<const char *>(
            reinterpret_cast<unsigned char *>(&mHeader) + mHeader.mStringBlockOffset + mEntry[index].mVaultNameOffset);
    }

    int GetVaultIndex(const char *name) {
        for (int i = 0; i < static_cast<int>(mHeader.mNumEntries); i++) {
            if (bStrCmp(name, GetVaultName(i)) == 0) {
                return i;
            }
        }
        return -1;
    }

    AttribVaultPackEntry &GetEntry(int index) {
        return mEntry[index];
    }

    unsigned char *GetData(unsigned int offset) {
        return reinterpret_cast<unsigned char *>(&mHeader) + offset;
    }

    void EndianSwap() {
        bEndianSwap32(&mHeader.mNumEntries);
        bEndianSwap32(&mHeader.mStringBlockOffset);
        bEndianSwap32(&mHeader.mStringBlockSize);
        for (int i = 0; i < static_cast<int>(mHeader.mNumEntries); i++) {
            bEndianSwap32(&mEntry[i].mVaultNameOffset);
            bEndianSwap32(&mEntry[i].mBinSize);
            bEndianSwap32(&mEntry[i].mVltSize);
            bEndianSwap32(&mEntry[i].mBinOffset);
            bEndianSwap32(&mEntry[i].mVltOffset);
        }
    }

    AttribVaultPackHeader mHeader; // offset 0x0, size 0x10
    AttribVaultPackEntry mEntry[1]; // offset 0x10, size 0x14
};

#endif
