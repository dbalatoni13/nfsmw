#include "GVault.h"

#include "GManager.h"
#include "GObjectBlock.h"
#include "GRaceDatabase.h"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/Misc/AttribAlloc.h"
#include "Speed/Indep/Src/Misc/AttribAsset.h"
#include "Speed/Indep/Src/Misc/AttribVaultPack.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

// total size: 0x18
class LoggingAttribAllocator : public IAttribAllocator {
  public:
    USE_FASTALLOC(LoggingAttribAllocator);

    virtual ~LoggingAttribAllocator() {
    }

    unsigned int GetChecksum() const {
        return mChecksum;
    }
    unsigned int GetByteCount() const {
        return mAllocBytes;
    }

  protected:
    LoggingAttribAllocator() {
        mChecksum = 0xEA0FF1CE;
        mAllocCount = 0;
        mAllocBytes = 0;
        mFreeCount = 0;
        mFreeBytes = 0;
    }

    void LogAlloc(unsigned int bytes, const char *name) {
        mAllocCount++;
        mAllocBytes += bytes;
    }

    void LogFree(unsigned int bytes, const char *name) {
        mFreeCount++;
        mFreeBytes += bytes;
    }

    unsigned int mChecksum;   // offset 0x4, size 0x4
    unsigned int mAllocCount; // offset 0x8, size 0x4
    unsigned int mAllocBytes; // offset 0xC, size 0x4
    unsigned int mFreeCount;  // offset 0x10, size 0x4
    unsigned int mFreeBytes;  // offset 0x14, size 0x4
};

// total size: 0x1C
class PreloadingAttribAllocator : public LoggingAttribAllocator {
  public:
    PreloadingAttribAllocator(int pool_num) {
        mPoolNum = pool_num;
    }

    void *Allocate(std::size_t bytes, const char *name) override {
        bytes = (bytes + 15) & ~15;
        LogAlloc(bytes, name);
        return bMalloc(bytes, name, 0, BMEMORY_POOL(mPoolNum));
    }

    void Free(void *ptr, std::size_t bytes, const char *name) override {
        bytes = (bytes + 15) & ~15;
        LogFree(bytes, name);
        bFree(ptr);
    }

    int mPoolNum; // offset 0x18, size 0x4
};

// total size: 0x24
class BlockLoadingAttribAllocator : public LoggingAttribAllocator {
  public:
    BlockLoadingAttribAllocator(unsigned char *buffer, unsigned int heapSize, unsigned int targetChecksum) {
        mAllocPtr = buffer;
        mAvailBytes = heapSize;
        mTargetChecksum = targetChecksum;
    }

    ~BlockLoadingAttribAllocator() {
    }

    void *Allocate(std::size_t bytes, const char *name) override {
        bytes = (bytes + 15) & ~15;
        LogAlloc(bytes, name);
        void *ptr = mAllocPtr;
        mAllocPtr += bytes;
        mAvailBytes -= bytes;
        return ptr;
    }

    void Free(void *ptr, std::size_t bytes, const char *name) override {
        bytes = (bytes + 15) & ~15;
        LogFree(bytes, name);
        mAvailBytes += bytes;
    }

    void VerifyAllocations() {
    }

    unsigned char *mAllocPtr;     // offset 0x18, size 0x4
    unsigned int mAvailBytes;     // offset 0x1C, size 0x4
    unsigned int mTargetChecksum; // offset 0x20, size 0x4
};

GVault::GVault(AttribVaultPackEntry *packEntry, const char *vaultName) {
    mVaultName = vaultName;
    mVault = 0;
    mFlags = 0;
    mBinResidentData = 0;
    mBinOffset = packEntry->mBinOffset;
    mBinSize = (packEntry->mBinSize + 15) & ~15;
    mVltOffset = packEntry->mVltOffset;
    mVltSize = (packEntry->mVltSize + 15) & ~15;
    mAttribAllocator = 0;
    mAttribObjSize = 0;
    mAttribAllocChecksum = 0;
    mAttribTransientData = 0;
    mGameObjSize = 0;
    mGameObjCount = 0;
    mGameObjBlock = 0;
    mGameObjData = 0;
}

GVault::~GVault() {
    if (IsLoaded()) {
        Unload();
    }
}

void GVault::LoadResident(AttribVaultPackImage *packImage) {
    unsigned char *binBlock = packImage->GetData(mBinOffset);
    unsigned char *vltBlock = packImage->GetData(mVltOffset);

    char binFileName[128];
    bSPrintf(binFileName, "%s.bin", GetName());

    char vltFileName[128];
    bSPrintf(vltFileName, "%s.vlt", GetName());

    char allocDesc[128];
    bSPrintf(allocDesc, "Gameplay resident data: %s", binFileName);

    mBinResidentData = (unsigned char *) bMalloc(mBinSize, allocDesc, 0, GetVirtualMemoryAllocParams());

    bMemCpy(mBinResidentData, binBlock, mBinSize);

    AddDepFile(binFileName, mBinResidentData, mBinSize);

    mVault = AddVault(vltFileName, vltBlock, mVltSize);

    unsigned int blockTrackingSize = GObjectBlock::CalcSpaceRequired(this, &mGameObjCount);

    mGameObjSize = (blockTrackingSize + sizeof(GObjectBlock) + 15) & ~15;
    mGameObjData = (unsigned char *) bMalloc(mGameObjSize, allocDesc, 0, GetVirtualMemoryAllocParams() | BMEMORY_ALIGNMENT(16));

    mFlags |= kFlagLoaded;
}

void GVault::PreloadTransient(AttribVaultPackImage *packImage, int pool_num) {
    unsigned char *binBlock = packImage->GetData(mBinOffset);
    unsigned char *vltBlock = packImage->GetData(mVltOffset);

    char binFileName[128];
    bSPrintf(binFileName, "%s.bin", GetName());

    char vltFileName[128];
    bSPrintf(vltFileName, "%s.vlt", GetName());

    PreloadingAttribAllocator *preloadingAllocator = new PreloadingAttribAllocator(pool_num);

    IAttribAllocator *oldAllocator = AttribAlloc::OverrideAllocator(preloadingAllocator);

    AddDepFile(binFileName, binBlock, mBinSize);

    mVault = AddVault(vltFileName, vltBlock, mVltSize);

    mAttribAllocator = preloadingAllocator;
    mAttribAllocChecksum = preloadingAllocator->GetChecksum();
    mAttribObjSize = preloadingAllocator->GetByteCount();

    AttribAlloc::OverrideAllocator(oldAllocator);

    unsigned int blockTrackingSize = GObjectBlock::CalcSpaceRequired(this, &mGameObjCount);

    mGameObjSize = (blockTrackingSize + sizeof(GObjectBlock) + 15) & ~15;
}

unsigned int GVault::InitTransient(unsigned char *binBlock, unsigned char *vltBlock) {
    char binFileName[128];
    bSPrintf(binFileName, "%s.bin", GetName());

    char vltFileName[128];
    bSPrintf(vltFileName, "%s.vlt", GetName());

    mAttribTransientData = binBlock + mBinSize;
    mGameObjData = mAttribTransientData + mAttribObjSize;

    BlockLoadingAttribAllocator *blockLoadingAllocator =
        new BlockLoadingAttribAllocator(mAttribTransientData, mAttribObjSize, mAttribAllocChecksum);

    IAttribAllocator *oldAllocator = AttribAlloc::OverrideAllocator(blockLoadingAllocator);

    AddDepFile(binFileName, binBlock, mBinSize);

    mVault = AddVault(vltFileName, vltBlock, mVltSize);

    blockLoadingAllocator->VerifyAllocations();

    mAttribAllocator = blockLoadingAllocator;

    AttribAlloc::OverrideAllocator(oldAllocator);

    if (GManager::Get().GetInGameplay()) {
        CreateGameplayObjects();
    }

    GRaceDatabase::Get().NotifyVaultLoaded(this);

    return GetFootprint();
}

void GVault::CreateGameplayObjects() {
    new (mGameObjData) GObjectBlock(this, mGameObjData + sizeof(GObjectBlock));
    mGameObjBlock = (GObjectBlock *) mGameObjData;

    unsigned int blockTrackingSize = mGameObjSize;
    mGameObjBlock->Initialize(blockTrackingSize);
}

void GVault::DestroyGameplayObjects() {
    mGameObjBlock->~GObjectBlock();
    mGameObjBlock = 0;
}

void GVault::LoadSyncTransient() {
    GManager::Get().LoadVaultSync(this);
}

void GVault::LoadAsyncTransient() {
}

void GVault::Unload() {
    GRaceDatabase::Get().NotifyVaultUnloading(this);

    IAttribAllocator *oldAllocator = 0;

    if (IsTransient()) {
        oldAllocator = AttribAlloc::OverrideAllocator(mAttribAllocator);
    }

    if (mGameObjBlock != 0) {
        DestroyGameplayObjects();
    }

    if (IsResident()) {
        bFree(mGameObjData);
    }
    mGameObjData = 0;

    mVault->Deinitialize();
    mVault->Release();

    mVault = 0;

    char binFileName[128];
    bSPrintf(binFileName, "%s.bin", GetName());
    RemoveDepFile(binFileName);

    char vltFileName[128];
    bSPrintf(vltFileName, "%s.vlt", GetName());
    RemoveVault(vltFileName);

    if (oldAllocator != 0) {
        AttribAlloc::OverrideAllocator(oldAllocator);
    }

    if (IsTransient()) {
        delete mAttribAllocator;
        mAttribAllocator = 0;
    }

    if (IsResident()) {
        delete[] mBinResidentData;
        mBinResidentData = 0;

        mFlags &= ~kFlagLoaded;
    }
}

const char *GVault::GetName() const {
    return mVaultName;
}

Attrib::Vault *GVault::GetAttribVault() const {
    return mVault;
}

GObjectBlock *GVault::GetObjectBlock() const {
    return mGameObjBlock;
}

unsigned int GVault::GetObjectCount() const {
    return mGameObjCount;
}

unsigned int GVault::GetFootprint() const {
    return mBinSize + mAttribObjSize + mGameObjSize;
}

unsigned int GVault::GetDataOffset() const {
    return mBinOffset;
}

unsigned int GVault::GetDataSize() const {
    return mBinSize;
}

unsigned int GVault::GetLoadDataOffset() const {
    return mVltOffset;
}

unsigned int GVault::GetLoadDataSize() const {
    return mVltSize;
}

bool GVault::IsLoaded() const {
    return mVault != 0;
}

bool GVault::IsResident() const {
    return (mFlags & kFlagLoaded) != 0;
}

bool GVault::IsTransient() const {
    return (mFlags & kFlagLoaded) == 0;
}

bool GVault::IsRaceBin() const {
    return (mFlags & kFlagRaceBin) != 0;
}

void GVault::SetRaceBin() {
    mFlags |= kFlagRaceBin;
}
