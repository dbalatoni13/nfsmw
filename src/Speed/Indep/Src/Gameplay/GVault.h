#ifndef GAMEPLAY_GVAULT_H
#define GAMEPLAY_GVAULT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0x14
struct AttribVaultPackEntry {
    unsigned int mVaultNameOffset; // offset 0x0, size 0x4
    unsigned int mBinSize;         // offset 0x4, size 0x4
    unsigned int mVltSize;         // offset 0x8, size 0x4
    unsigned int mBinOffset;       // offset 0xC, size 0x4
    unsigned int mVltOffset;       // offset 0x10, size 0x4
};

struct AttribVaultPackImage;
struct GObjectBlock;
struct LoggingAttribAllocator;

namespace Attrib {
class Vault;
}

// total size: 0x40
class GVault {
  public:
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
    GObjectBlock *GetObjectBlock() const { return mGameObjBlock; }
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

  private:
    Attrib::Vault *mVault;                    // offset 0x0, size 0x4
    const char *mVaultName;                   // offset 0x4, size 0x4
    unsigned int mFlags;                      // offset 0x8, size 0x4
    unsigned char *mBinResidentData;          // offset 0xC, size 0x4
    unsigned int mBinOffset;                  // offset 0x10, size 0x4
    unsigned int mBinSize;                    // offset 0x14, size 0x4
    unsigned int mVltOffset;                  // offset 0x18, size 0x4
    unsigned int mVltSize;                    // offset 0x1C, size 0x4
    LoggingAttribAllocator *mAttribAllocator; // offset 0x20, size 0x4
    unsigned int mAttribObjSize;              // offset 0x24, size 0x4
    unsigned int mAttribAllocChecksum;        // offset 0x28, size 0x4
    unsigned char *mAttribTransientData;      // offset 0x2C, size 0x4
    unsigned int mGameObjSize;                // offset 0x30, size 0x4
    unsigned int mGameObjCount;               // offset 0x34, size 0x4
    GObjectBlock *mGameObjBlock;              // offset 0x38, size 0x4
    unsigned char *mGameObjData;              // offset 0x3C, size 0x4
};

#endif
