#include "Speed/Indep/Src/Gameplay/GVault.h"

#include "Speed/Indep/Src/Gameplay/GManager.h"

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

void GVault::LoadSyncTransient() {
    GManager::Get().LoadVaultSync(this);
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
