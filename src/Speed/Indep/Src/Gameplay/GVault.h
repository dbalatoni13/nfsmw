#ifndef GAMEPLAY_GVAULT_H
#define GAMEPLAY_GVAULT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/AttribAlloc.h"

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

// total size: 0x18
class LoggingAttribAllocator : public IAttribAllocator {
  public:
    static void *operator new(std::size_t size);
    static void operator delete(void *mem, std::size_t size);

    LoggingAttribAllocator();
    virtual ~LoggingAttribAllocator();

    virtual void *Allocate(std::size_t bytes, const char *name);
    virtual void Free(void *ptr, std::size_t bytes, const char *name);

    unsigned int GetChecksum() const;
    unsigned int GetByteCount() const;

  protected:
    void LogAlloc(unsigned int bytes, const char *name);
    void LogFree(unsigned int bytes, const char *name);

    unsigned int mChecksum;  // offset 0x4, size 0x4
    unsigned int mAllocCount; // offset 0x8, size 0x4
    unsigned int mAllocBytes; // offset 0xC, size 0x4
    unsigned int mFreeCount;  // offset 0x10, size 0x4
    unsigned int mFreeBytes;  // offset 0x14, size 0x4
};

// total size: 0x1C
class PreloadingAttribAllocator : public LoggingAttribAllocator {
  public:
    PreloadingAttribAllocator(int pool_num);

    void *Allocate(std::size_t bytes, const char *name) override;
    void Free(void *ptr, std::size_t bytes, const char *name) override;

  private:
    int mPoolNum; // offset 0x18, size 0x4
};

// total size: 0x24
class BlockLoadingAttribAllocator : public LoggingAttribAllocator {
  public:
    BlockLoadingAttribAllocator(unsigned char *buffer, unsigned int heapSize, unsigned int targetChecksum);

    void *Allocate(std::size_t bytes, const char *name) override;
    void Free(void *ptr, std::size_t bytes, const char *name) override;
    void VerifyAllocations();

  private:
    unsigned char *mAllocPtr;       // offset 0x18, size 0x4
    unsigned int mAvailBytes;       // offset 0x1C, size 0x4
    unsigned int mTargetChecksum;   // offset 0x20, size 0x4
};

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
