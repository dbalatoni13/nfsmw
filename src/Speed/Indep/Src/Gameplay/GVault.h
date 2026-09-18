#ifndef GAMEPLAY_GVAULT_H
#define GAMEPLAY_GVAULT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace Attrib {
class Vault;
}

struct AttribVaultPackEntry;
struct AttribVaultPackImage;
class LoggingAttribAllocator;
struct GObjectBlock;

// total size: 0x40
struct GVault {
    enum Flags {
        kFlagLoaded = 1 << 0,
        kFlagRaceBin = 1 << 1,
    };

    GVault(AttribVaultPackEntry *packEntry, const char *vaultName);

    ~GVault();

    void LoadResident(AttribVaultPackImage *packImage);

    void PreloadTransient(AttribVaultPackImage *packImage, int pool_num);

    unsigned int InitTransient(unsigned char *binBlock, unsigned char *vltBlock);

    void CreateGameplayObjects();

    void DestroyGameplayObjects();

    void LoadSyncTransient();

    void LoadAsyncTransient();

    void Unload();

    const char *GetName() const;

    Attrib::Vault *GetAttribVault() const;

    GObjectBlock *GetObjectBlock() const;

    unsigned int GetObjectCount() const;

    unsigned int GetFootprint() const;

    unsigned int GetDataOffset() const;

    unsigned int GetDataSize() const;

    unsigned int GetLoadDataOffset() const;

    unsigned int GetLoadDataSize() const;

    bool IsLoaded() const;

    bool IsResident() const;

    bool IsTransient() const;

    bool IsRaceBin() const;

    void SetRaceBin();

    Attrib::Vault *mVault;                     // offset 0x0, size 0x4
    const char *mVaultName;                    // offset 0x4, size 0x4
    unsigned int mFlags;                       // offset 0x8, size 0x4
    unsigned char *mBinResidentData;           // offset 0xC, size 0x4
    unsigned int mBinOffset;                   // offset 0x10, size 0x4
    unsigned int mBinSize;                     // offset 0x14, size 0x4
    unsigned int mVltOffset;                   // offset 0x18, size 0x4
    unsigned int mVltSize;                     // offset 0x1C, size 0x4
    LoggingAttribAllocator *mAttribAllocator;  // offset 0x20, size 0x4
    unsigned int mAttribObjSize;               // offset 0x24, size 0x4
    unsigned int mAttribAllocChecksum;         // offset 0x28, size 0x4
    unsigned char *mAttribTransientData;       // offset 0x2C, size 0x4
    unsigned int mGameObjSize;                 // offset 0x30, size 0x4
    unsigned int mGameObjCount;                // offset 0x34, size 0x4
    GObjectBlock *mGameObjBlock;               // offset 0x38, size 0x4
    unsigned char *mGameObjData;               // offset 0x3C, size 0x4
};

#endif
