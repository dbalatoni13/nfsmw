#include "Speed/Indep/Src/Gameplay/GManager.h"

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/Gameplay/GVault.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

GManager *GManager::mObj = nullptr;

GManager::GManager(const char *vaultPackName)
    : UTL::COM::Object(1), //
      IVehicleCache((UTL::COM::Object *)this), //
      mVaultPackFileName(vaultPackName), //
      mVaultPackFile(nullptr), //
      mVaultCount(0), //
      mVaults(nullptr), //
      mVaultNameStrings(nullptr), //
      mLoadingPackImage(nullptr), //
      mBinSlotSize(0), //
      mStreamedBinSlots(nullptr), //
      mRaceSlotSize(0), //
      mStreamedRaceSlots(nullptr), //
      mTempLoadData(nullptr), //
      mTransientPoolNumber(0), //
      mTransientPoolMemory(nullptr), //
      mGameplayClass(Attrib::Database::Get().GetClass(0x5CEA9D46)), //
      mMilestoneClass(Attrib::Database::Get().GetClass(0xE4C3D904)), //
      mAttributeKeyShiftTo24(0), //
      mCollectionKeyShiftTo32(0), //
      mMaxObjects(0), //
      mClassTempBuffer(nullptr), //
      mInstanceHashTableSize(0), //
      mInstanceHashTableMask(0), //
      mWorstHashCollision(0), //
      mKeyToInstanceMap(nullptr), //
      mActiveCharacters(), //
      mStockCars(), //
      mMilestoneTypeInfo(), //
      mNumMilestones(0), //
      mMilestones(nullptr), //
      mNumSpeedTraps(0), //
      mSpeedTraps(nullptr), //
      mNumBountySpawnPoints(0), //
      mFreeRoamStartMarker(0), //
      mFreeRoamFromSafeHouseStartMarker(0), //
      mStartFreeRoamFromSafeHouse(false), //
      mStartFreeRoamPursuit(false), //
      mQueuedPursuitMinHeat(0.0f), //
      mInGameplay(false), //
      mOverrideFreeRoamStartMarker(0), //
      mObjectStateBuffer(nullptr), //
      mObjectStateBufferFree(nullptr), //
      mObjectStateBufferSize(0), //
      mPersistentStateBlocks(), //
      mSessionStateBlocks(), //
      mPendingSMS(), //
      mLastCouldSendTime(0.0f), //
      mWarping(false), //
      mWarpStartPursuit(false), //
      mWarpTargetMarker(0), //
      mNumIcons(0), //
      mNumVisibleIcons(0), //
      mIcons(nullptr), //
      mPursuitBreakerIconsShown(false), //
      mHidingSpotIconsShown(false), //
      mEventIconsShown(false), //
      mMenuGateIconsShown(false), //
      mSpeedTrapIconsShown(false), //
      mSpeedTrapRaceIconsShown(false), //
      mAllowEngageEvents(false), //
      mAllowEngageSafehouse(false), //
      mAllowMenuGates(false), //
      mRestartEventHash(0) {
    mObj = this;

    bMemSet(mBinVaultInSlot, 0, sizeof(mBinVaultInSlot));
    bMemSet(mRaceVaultInSlot, 0, sizeof(mRaceVaultInSlot));
    bMemSet(mBountySpawnPoint, 0, sizeof(mBountySpawnPoint));
    bMemSet(mHidingSpotFound, 0, sizeof(mHidingSpotFound));

    mActiveCharacters.reserve(0x10);
    AllocateObjectStateStorage();
    ResetAllGameplayData();
}

GManager::~GManager() {
    mActiveCharacters.clear();
    ReleaseSpeedTraps();
    ReleaseMilestones();
    ClearStockCars();
    ReleaseStreamingBuffers();
    ReleaseInstanceMap();
    ReleaseObjectStateStorage();
    DestroyVaults();

    if (mVaultPackFile) {
        bClose(mVaultPackFile);
        mVaultPackFile = nullptr;
    }

    mObj = nullptr;
}

void GManager::StartBinActivity(GRaceBin *raceBin) {
    GActivity *activity;

    if (!raceBin) {
        return;
    }

    activity = static_cast<GActivity *>(FindInstance(raceBin->GetCollectionKey()));
    if (activity && !activity->GetIsRunning()) {
        activity->Run();
    }
}

void GManager::LoadVaultSync(GVault *vault) {
    int slot;
    int offset;
    unsigned char *streamBuffer;

    if (!vault->IsRaceBin()) {
        slot = GetAvailableRaceSlot();
        streamBuffer = mStreamedRaceSlots;
        offset = slot * mRaceSlotSize;
        mRaceVaultInSlot[slot] = vault;
    } else {
        GetAvailableRaceSlot();
        slot = GetAvailableBinSlot();
        streamBuffer = mStreamedBinSlots;
        offset = slot * mBinSlotSize;
        mBinVaultInSlot[slot] = vault;
    }

    mVaultPackFile = bOpen(mVaultPackFileName, 1, 1);
    bSeek(mVaultPackFile, vault->GetDataOffset(), 0);
    bRead(mVaultPackFile, streamBuffer + offset, vault->GetDataSize());
    bSeek(mVaultPackFile, vault->GetLoadDataOffset(), 0);
    bRead(mVaultPackFile, mTempLoadData, vault->GetLoadDataSize());
    bClose(mVaultPackFile);
    mVaultPackFile = nullptr;

    vault->InitTransient(streamBuffer + offset, mTempLoadData);
    if (mInGameplay) {
        ConnectRuntimeInstances();
    }
}

int GManager::GetAvailableBinSlot() {
    if (mBinVaultInSlot[0]) {
        mBinVaultInSlot[0]->Unload();
        mBinVaultInSlot[0] = nullptr;
    }

    return 0;
}

int GManager::GetAvailableRaceSlot() {
    if (mRaceVaultInSlot[0]) {
        mRaceVaultInSlot[0]->Unload();
        mRaceVaultInSlot[0] = nullptr;
    }

    return 0;
}

GMilestone *GManager::GetNextMilestone(GMilestone *current, bool availOnly, unsigned int binNumber) {
    GMilestone *next = current + 1;
    GMilestone *end = mMilestones + mNumMilestones;

    while (next < end) {
        if ((!availOnly || next->GetIsAvailable() || next->GetIsDonePendingEscape()) &&
            (binNumber == 0 || next->GetBinNumber() == binNumber)) {
            return next;
        }

        next++;
    }

    return nullptr;
}

unsigned int GManager::SaveMilestones(GMilestone *dest) {
    bMemCpy(dest, mMilestones, mNumMilestones * sizeof(GMilestone));
    return mNumMilestones;
}

void GManager::LoadMilestones(GMilestone *src, unsigned int count) {
    unsigned int i;
    unsigned int j;

    for (i = 0; i < count; ++i) {
        for (j = 0; j < mNumMilestones; ++j) {
            if (mMilestones[j].GetChallengeKey() == src[i].GetChallengeKey()) {
                mMilestones[j] = src[i];
                break;
            }
        }
    }
}

GSpeedTrap *GManager::GetNextSpeedTrap(GSpeedTrap *current, bool activeOnly, unsigned int binNumber) {
    GSpeedTrap *next = current + 1;
    GSpeedTrap *end = mSpeedTraps + mNumSpeedTraps;

    while (next < end) {
        if ((!activeOnly || next->IsFlagSet(GSpeedTrap::kFlag_Active)) &&
            (binNumber == 0 || next->GetBinNumber() == binNumber)) {
            return next;
        }

        next++;
    }

    return nullptr;
}

unsigned int GManager::SaveSpeedTraps(GSpeedTrap *dest) {
    bMemCpy(dest, mSpeedTraps, mNumSpeedTraps * sizeof(GSpeedTrap));
    return mNumSpeedTraps;
}

void GManager::LoadSpeedTraps(GSpeedTrap *src, unsigned int count) {
    unsigned int i;
    unsigned int j;

    for (i = 0; i < count; ++i) {
        for (j = 0; j < mNumSpeedTraps; ++j) {
            if (mSpeedTraps[j].GetSpeedTrapKey() == src[i].GetSpeedTrapKey()) {
                mSpeedTraps[j] = src[i];
                break;
            }
        }
    }
}

void GManager::AllocateObjectStateStorage() {
    mObjectStateBuffer = static_cast<unsigned char *>(bMalloc(0x4000, GetVirtualMemoryAllocParams()));
    mObjectStateBufferFree = mObjectStateBuffer;
    mObjectStateBufferSize = 0x4000;
}

void GManager::ReleaseObjectStateStorage() {
    mPersistentStateBlocks.clear();
    mSessionStateBlocks.clear();

    if (mObjectStateBuffer) {
        bFree(mObjectStateBuffer);
    }

    mObjectStateBuffer = nullptr;
    mObjectStateBufferFree = nullptr;
    mObjectStateBufferSize = 0;
}

void GManager::ReleaseInstanceMap() {
    if (mKeyToInstanceMap) {
        bFree(mKeyToInstanceMap);
        mKeyToInstanceMap = nullptr;
    }

    mInstanceHashTableMask = 0;
    mInstanceHashTableSize = 0;
}

void GManager::ReleaseStreamingBuffers() {
    if (mStreamedBinSlots) {
        delete[] mStreamedBinSlots;
        mStreamedBinSlots = nullptr;
    }

    if (mStreamedRaceSlots) {
        delete[] mStreamedRaceSlots;
        mStreamedRaceSlots = nullptr;
    }

    if (mTempLoadData) {
        delete[] mTempLoadData;
        mTempLoadData = nullptr;
    }
}

void GManager::DestroyVaults() {
    if (mVaults) {
        for (unsigned int i = 0; i < mVaultCount; ++i) {
            mVaults[i].~GVault();
        }

        mVaultCount = 0;
        bFree(mVaults);
        mVaults = nullptr;
    }

    if (mVaultNameStrings) {
        delete[] const_cast<char *>(mVaultNameStrings);
        mVaultNameStrings = nullptr;
    }
}

void GManager::ReleaseMilestones() {
    if (mMilestones) {
        delete[] mMilestones;
    }

    mNumMilestones = 0;
    mMilestones = nullptr;
}

void GManager::ResetMilestoneTrackingInfo() {
    mMilestoneTypeInfo.clear();
}

void GManager::ResetMilestones() {
    if (!mMilestones) {
        return;
    }

    for (unsigned int i = 0; i < mNumMilestones; ++i) {
        mMilestones[i].Reset();
    }
}

void GManager::ReleaseSpeedTraps() {
    if (mSpeedTraps) {
        delete[] mSpeedTraps;
    }

    mNumSpeedTraps = 0;
    mSpeedTraps = nullptr;
}

void GManager::ResetSpeedTraps() {
    if (!mSpeedTraps) {
        return;
    }

    for (unsigned int i = 0; i < mNumSpeedTraps; ++i) {
        mSpeedTraps[i].Reset();
    }
}

void GManager::ClearStockCars() {
    mStockCars.clear();
}

void GManager::ResetTimers() {
    bMemSet(mTimers, 0, sizeof(mTimers));
}

void GManager::ResetAllGameplayData() {
    mPersistentStateBlocks.clear();
    mSessionStateBlocks.clear();
    mObjectStateBufferFree = mObjectStateBuffer;
    ResetMilestoneTrackingInfo();
    ResetMilestones();
    ResetSpeedTraps();
    bMemSet(mHidingSpotFound, 0, sizeof(mHidingSpotFound));
    ResetTimers();
    mStartFreeRoamFromSafeHouse = false;
    mFreeRoamStartMarker = 0;
    mOverrideFreeRoamStartMarker = 0;
    mFreeRoamFromSafeHouseStartMarker = 0;
    mPendingSMS.clear();
}
