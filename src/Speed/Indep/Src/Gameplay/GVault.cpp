#include "Speed/Indep/Src/Gameplay/GVault.h"

#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GObjectBlock.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Misc/AttribAlloc.h"
#include "Speed/Indep/Src/Misc/AttribAsset.h"

void *LoggingAttribAllocator::operator new(std::size_t size) {
    return gFastMem.Alloc(size, nullptr);
}

void LoggingAttribAllocator::operator delete(void *mem, std::size_t size) {
    if (mem) {
        gFastMem.Free(mem, size, nullptr);
    }
}

LoggingAttribAllocator::LoggingAttribAllocator()
    : mChecksum(0xEA0FF1CE), //
      mAllocCount(0), //
      mAllocBytes(0), //
      mFreeCount(0), //
      mFreeBytes(0) {}

LoggingAttribAllocator::~LoggingAttribAllocator() {}

void *LoggingAttribAllocator::Allocate(std::size_t bytes, const char *name) {
    void *memory = gFastMem.Alloc(bytes, name);

    if (memory) {
        LogAlloc(static_cast<unsigned int>(bytes), name);
    }

    return memory;
}

void LoggingAttribAllocator::Free(void *ptr, std::size_t bytes, const char *name) {
    if (ptr) {
        LogFree(static_cast<unsigned int>(bytes), name);
        gFastMem.Free(ptr, bytes, name);
    }
}

unsigned int LoggingAttribAllocator::GetChecksum() const {
    return mChecksum;
}

unsigned int LoggingAttribAllocator::GetByteCount() const {
    return mAllocBytes;
}

void LoggingAttribAllocator::LogAlloc(unsigned int bytes, const char *name) {
    mAllocCount++;
    mAllocBytes += bytes;
}

void LoggingAttribAllocator::LogFree(unsigned int bytes, const char *name) {
    mFreeCount++;
    mFreeBytes += bytes;
}

PreloadingAttribAllocator::PreloadingAttribAllocator(int pool_num) : LoggingAttribAllocator(), mPoolNum(pool_num) {}

void *PreloadingAttribAllocator::Allocate(std::size_t bytes, const char *name) {
    return LoggingAttribAllocator::Allocate(bytes, name);
}

void PreloadingAttribAllocator::Free(void *ptr, std::size_t bytes, const char *name) {
    LoggingAttribAllocator::Free(ptr, bytes, name);
}

BlockLoadingAttribAllocator::BlockLoadingAttribAllocator(unsigned char *buffer, unsigned int heapSize, unsigned int targetChecksum)
    : LoggingAttribAllocator(), //
      mAllocPtr(buffer), //
      mAvailBytes(heapSize), //
      mTargetChecksum(targetChecksum) {}

void *BlockLoadingAttribAllocator::Allocate(std::size_t bytes, const char *name) {
    const unsigned int alignedBytes = static_cast<unsigned int>((bytes + 15U) & ~15U);

    if (alignedBytes > mAvailBytes) {
        return nullptr;
    }

    void *memory = mAllocPtr;
    mAllocPtr += alignedBytes;
    mAvailBytes -= alignedBytes;
    LogAlloc(alignedBytes, name);
    return memory;
}

void BlockLoadingAttribAllocator::Free(void *ptr, std::size_t bytes, const char *name) {
    if (ptr) {
        LogFree(static_cast<unsigned int>(bytes), name);
    }
}

void BlockLoadingAttribAllocator::VerifyAllocations() {}

GVault::GVault(AttribVaultPackEntry *packEntry, const char *vaultName)
    : mVault(nullptr), //
      mVaultName(vaultName), //
      mFlags(0), //
      mBinResidentData(nullptr), //
      mBinOffset(packEntry->mBinOffset), //
      mBinSize((packEntry->mBinSize + 15U) & ~15U), //
      mVltOffset(packEntry->mVltOffset), //
      mVltSize((packEntry->mVltSize + 15U) & ~15U), //
      mAttribAllocator(nullptr), //
      mAttribObjSize(0), //
      mAttribAllocChecksum(0), //
      mAttribTransientData(nullptr), //
      mGameObjSize(0), //
      mGameObjCount(0), //
      mGameObjBlock(nullptr), //
      mGameObjData(nullptr) {}

GVault::~GVault() {
    if (IsLoaded()) {
        Unload();
    }
}

void GVault::LoadResident(AttribVaultPackImage *packImage) {
    char binName[128];
    char vltName[128];
    char allocName[128];

    bSPrintf(binName, "%s.bin", GetName());
    bSPrintf(vltName, "%s.vlt", GetName());
    bSPrintf(allocName, "Gameplay resident data: %s", binName);

    mBinResidentData = static_cast<unsigned char *>(bMalloc(mBinSize, allocName, 0, GetVirtualMemoryAllocParams()));
    bMemCpy(mBinResidentData, reinterpret_cast<unsigned char *>(packImage) + mBinOffset, mBinSize);
    AddDepFile(binName, mBinResidentData, mBinSize);
    mVault = AddVault(vltName, reinterpret_cast<unsigned char *>(packImage) + mVltOffset, mVltSize);

    mGameObjSize = (GObjectBlock::CalcSpaceRequired(this, &mGameObjCount) + 0x5FU) & ~15U;
    mGameObjData = static_cast<unsigned char *>(bMalloc(mGameObjSize, "Gameplay object data", 0, 0x400));
    mFlags |= 1;
}

void GVault::PreloadTransient(AttribVaultPackImage *packImage, int pool_num) {
    char binName[128];
    char vltName[128];
    PreloadingAttribAllocator *allocator;
    IAttribAllocator *prevAllocator;

    bSPrintf(binName, "%s.bin", GetName());
    bSPrintf(vltName, "%s.vlt", GetName());

    allocator = new PreloadingAttribAllocator(pool_num);
    prevAllocator = AttribAlloc::OverrideAllocator(allocator);

    AddDepFile(binName, reinterpret_cast<unsigned char *>(packImage) + mBinOffset, mBinSize);
    mVault = AddVault(vltName, reinterpret_cast<unsigned char *>(packImage) + mVltOffset, mVltSize);
    mAttribAllocator = allocator;
    mAttribAllocChecksum = allocator->GetChecksum();
    mAttribObjSize = allocator->GetByteCount();

    AttribAlloc::OverrideAllocator(prevAllocator);
    mGameObjSize = (GObjectBlock::CalcSpaceRequired(this, &mGameObjCount) + 0x5FU) & ~15U;
}

unsigned int GVault::InitTransient(unsigned char *binBlock, unsigned char *vltBlock) {
    char binName[128];
    char vltName[128];
    BlockLoadingAttribAllocator *allocator;
    IAttribAllocator *prevAllocator;

    bSPrintf(binName, "%s.bin", GetName());
    bSPrintf(vltName, "%s.vlt", GetName());

    mAttribTransientData = binBlock + mBinSize;
    mGameObjData = binBlock + mBinSize + mAttribObjSize;

    allocator = new BlockLoadingAttribAllocator(mAttribTransientData, mAttribObjSize, mAttribAllocChecksum);
    prevAllocator = AttribAlloc::OverrideAllocator(allocator);

    AddDepFile(binName, binBlock, mBinSize);
    mVault = AddVault(vltName, vltBlock, mVltSize);
    mAttribAllocator = allocator;

    AttribAlloc::OverrideAllocator(prevAllocator);

    if (GManager::Get().GetInGameplay()) {
        CreateGameplayObjects();
    }

    GRaceDatabase::Get().NotifyVaultLoaded(this);
    return GetFootprint();
}

void GVault::CreateGameplayObjects() {
    if (mGameObjData) {
        new (mGameObjData) GObjectBlock(this, mGameObjData + sizeof(GObjectBlock));
    }

    mGameObjBlock = reinterpret_cast<GObjectBlock *>(mGameObjData);
    mGameObjBlock->Initialize(mGameObjSize);
}

void GVault::DestroyGameplayObjects() {
    delete mGameObjBlock;
    mGameObjBlock = nullptr;
}

void GVault::LoadSyncTransient() {
    GManager::Get().LoadVaultSync(this);
}

void GVault::Unload() {
    IAttribAllocator *prevAllocator;
    char binName[128];
    char vltName[128];

    prevAllocator = nullptr;
    GRaceDatabase::Get().NotifyVaultUnloading(this);

    if (IsTransient()) {
        prevAllocator = AttribAlloc::OverrideAllocator(mAttribAllocator);
    }

    if (mGameObjBlock) {
        DestroyGameplayObjects();
    }

    if (IsResident()) {
        bFree(mGameObjData);
    }
    mGameObjData = nullptr;

    mVault->Deinitialize();
    mVault->Release();
    mVault = nullptr;

    bSPrintf(binName, "%s.bin", GetName());
    RemoveDepFile(binName);

    bSPrintf(vltName, "%s.vlt", GetName());
    RemoveVault(vltName);

    if (prevAllocator) {
        AttribAlloc::OverrideAllocator(prevAllocator);
    }

    if (IsTransient()) {
        delete mAttribAllocator;
        mAttribAllocator = nullptr;
    }

    if (IsResident()) {
        if (mBinResidentData) {
            bFree(mBinResidentData);
        }

        mBinResidentData = nullptr;
        mFlags &= ~1U;
    }
}

const char *GVault::GetName() const {
    return mVaultName;
}

Attrib::Vault *GVault::GetAttribVault() const {
    return mVault;
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
    return mVault != nullptr;
}

bool GVault::IsResident() const {
    return (mFlags & 1) != 0;
}

bool GVault::IsTransient() const {
    return (mFlags ^ 1) & 1;
}

bool GVault::IsRaceBin() const {
    return (mFlags & 2) != 0;
}

void GVault::SetRaceBin() {
    mFlags |= 2;
}
