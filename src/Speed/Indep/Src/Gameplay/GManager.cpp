#include "Speed/Indep/Src/Gameplay/GManager.h"

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/gameplay.h"
#include "Speed/Indep/Src/Gameplay/GVault.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#include <algorithm>
#include <new>

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

void GManager::AllocateInstanceMap() {
    unsigned int largestTransientCount;

    largestTransientCount = 0;
    mMaxObjects = 0;

    for (unsigned int i = 0; i < mVaultCount; ++i) {
        GVault &vault = mVaults[i];

        if (vault.IsResident()) {
            mMaxObjects += vault.GetObjectCount();
        } else if (largestTransientCount < vault.GetObjectCount()) {
            largestTransientCount = vault.GetObjectCount();
        }
    }

    mMaxObjects += largestTransientCount * 2;
    mInstanceHashTableSize = 0x100;

    {
        unsigned int neededSize = mMaxObjects + (mMaxObjects >> 1);

        while (mInstanceHashTableSize < neededSize) {
            mInstanceHashTableSize <<= 1;
        }
    }

    mInstanceHashTableMask = mInstanceHashTableSize - 1;
    mKeyToInstanceMap = static_cast<HashEntry *>(bMalloc(mInstanceHashTableSize << 3, GetVirtualMemoryAllocParams()));
    bMemSet(mKeyToInstanceMap, 0, mInstanceHashTableSize << 3);
}

void GManager::AllocateStreamingBuffers() {
    GVault *largestBinVault;
    GVault *largestRaceVault;

    largestBinVault = nullptr;
    for (unsigned int i = 0; i < GRaceDatabase::Get().GetBinCount(); ++i) {
        GRaceBin *raceBin = GRaceDatabase::Get().GetBin(i);
        GVault *vault = raceBin->GetChildVault();

        if (vault && !vault->IsResident()) {
            if (!largestBinVault || largestBinVault->GetFootprint() < vault->GetFootprint()) {
                largestBinVault = vault;
            }
        }
    }

    largestRaceVault = nullptr;
    for (unsigned int i = 0; i < GRaceDatabase::Get().GetRaceCount(); ++i) {
        GRaceParameters *raceParameters = GRaceDatabase::Get().GetRaceParameters(i);
        GVault *vault = raceParameters->GetChildVault();

        if (vault && !vault->IsResident()) {
            if (!largestRaceVault || largestRaceVault->GetFootprint() < vault->GetFootprint()) {
                largestRaceVault = vault;
            }
        }
    }

    mBinSlotSize = largestBinVault ? largestBinVault->GetFootprint() : 0;
    mRaceSlotSize = largestRaceVault ? largestRaceVault->GetFootprint() : 0;

    mStreamedBinSlots = new unsigned char[mBinSlotSize];
    mStreamedRaceSlots = new unsigned char[mRaceSlotSize];

    {
        unsigned int maxLoadDataSize = 0;

        for (unsigned int i = 0; i < mVaultCount; ++i) {
            GVault &vault = mVaults[i];

            if (vault.IsTransient()) {
                if (maxLoadDataSize < vault.GetLoadDataSize()) {
                    maxLoadDataSize = vault.GetLoadDataSize();
                }
            }
        }

        mTempLoadData = new unsigned char[maxLoadDataSize];
    }
}

void GManager::BuildVaultTable(AttribVaultPackImage *packImage) {
    char *image;
    AttribVaultPackEntry *entries;

    image = reinterpret_cast<char *>(packImage);
    mVaultNameStrings = new char[*reinterpret_cast<unsigned int *>(image + 0xC)];
    bMemCpy(const_cast<char *>(mVaultNameStrings), image + *reinterpret_cast<int *>(image + 8), *reinterpret_cast<unsigned int *>(image + 0xC));

    mVaultCount = *reinterpret_cast<int *>(image + 4);
    mVaults = static_cast<GVault *>(bMalloc(mVaultCount << 6, GetVirtualMemoryAllocParams() | 0x1000));

    entries = reinterpret_cast<AttribVaultPackEntry *>(image + 0x10);
    for (unsigned int i = 0; i < mVaultCount; ++i) {
        new (&mVaults[i]) GVault(&entries[i], mVaultNameStrings + entries[i].mVaultNameOffset);
    }
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

unsigned int GManager::GetStrippedNameKey(const char *name) {
    int length;
    const char *start;
    const char *scan;

    length = bStrLen(name);
    scan = name + length;
    do {
        start = scan;
        scan = start - 1;
        if (scan < name || *scan == '/') {
            break;
        }
    } while (*scan != '\\');

    return Attrib::StringToKey(start);
}

unsigned int GManager::FindUniqueKeyShift(unsigned int *keys, unsigned int numKeys, unsigned int uniqueBits) {
    if (numKeys > 1) {
        unsigned int shift = 0x20 - uniqueBits;
        unsigned int clearMask = 0xFFFFFFFF;
        unsigned int compareMask = ((1 << (uniqueBits & 0x1F)) - 1) << (shift & 0x1F);

        while (static_cast<int>(shift) > -1) {
            for (unsigned int i = 0; i < numKeys; ++i) {
                keys[i] &= clearMask;
            }

            std::sort(keys, keys + numKeys);

            {
                bool foundDuplicate = false;

                for (unsigned int i = 1; i < numKeys; ++i) {
                    if ((keys[i] & compareMask) == (keys[i - 1] & compareMask)) {
                        foundDuplicate = true;
                        break;
                    }
                }

                if (!foundDuplicate) {
                    return shift;
                }
            }

            clearMask >>= 1;
            compareMask >>= 1;
            shift--;
        }
    }

    return 0;
}

void GManager::FindKeyReductionShifts() {
    unsigned int numCollections;
    unsigned int numDefinitions;
    unsigned int numKeys;
    unsigned int *keys;
    unsigned int *out;
    unsigned int collectionKey;
    Attrib::Key definitionKey;

    numCollections = mGameplayClass->GetNumCollections();
    numDefinitions = mGameplayClass->GetNumDefinitions();
    numKeys = numCollections;
    if (numKeys < numDefinitions + numCollections) {
        numKeys = numDefinitions + numCollections;
    }

    keys = new unsigned int[numKeys];
    out = keys;

    for (definitionKey = mGameplayClass->GetFirstDefinition(); definitionKey != 0; definitionKey = mGameplayClass->GetNextDefinition(definitionKey)) {
        *out++ = definitionKey;
    }

    for (collectionKey = mGameplayClass->GetFirstCollection(); collectionKey != 0; collectionKey = mGameplayClass->GetNextCollection(collectionKey)) {
        Attrib::Gen::gameplay gameplay(collectionKey, 0, nullptr);
        *out++ = GetStrippedNameKey(gameplay.CollectionName());
    }

    std::sort(keys, out);
    out = std::unique(keys, out);
    mAttributeKeyShiftTo24 = FindUniqueKeyShift(keys, out - keys, 0x18);
    delete[] keys;
}

void GManager::TrackValue(const char *valueName, float value) {
    unsigned int valueKey;
    MilestoneInfoMap::iterator it;
    bool updateBest;

    valueKey = Attrib::StringToKey(valueName);
    it = mMilestoneTypeInfo.find(valueKey);
    if (it == mMilestoneTypeInfo.end()) {
        MilestoneTypeInfo info;

        info.mTypeKey = valueKey;
        info.mLastKnownValue = value;
        info.mBestValue = value;
        info.mFlags = 0;
        mMilestoneTypeInfo.insert(MilestoneInfoMap::value_type(valueKey, info));
        return;
    }

    it->second.mLastKnownValue = value;
    updateBest = it->second.mBestValue == -1.0f;
    if (!updateBest) {
        if ((it->second.mFlags & GMilestone::kFlag_BiggerIsBetter) != 0) {
            updateBest = it->second.mBestValue < value;
        } else {
            updateBest = value < it->second.mBestValue;
        }
    }

    if (updateBest) {
        it->second.mBestValue = value;
    }
}

void GManager::IncValue(const char *valueName) {
    float value;

    value = GetValue(valueName);
    if (value == -1.0f) {
        TrackValue(valueName, 1.0f);
    } else {
        TrackValue(valueName, value + 1.0f);
    }
}

float GManager::GetValue(const char *valueName) {
    return GetValue(Attrib::StringToKey(valueName));
}

float GManager::GetValue(unsigned int valueKey) {
    MilestoneInfoMap::iterator it;

    it = mMilestoneTypeInfo.find(valueKey);
    if (it == mMilestoneTypeInfo.end()) {
        return -1.0f;
    }

    return it->second.mLastKnownValue;
}

bool GManager::GetIsBiggerValueBetter(unsigned int valueKey) {
    MilestoneInfoMap::iterator it;

    it = mMilestoneTypeInfo.find(valueKey);
    if (it == mMilestoneTypeInfo.end()) {
        return false;
    }

    return (it->second.mFlags & GMilestone::kFlag_BiggerIsBetter) != 0;
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

void GManager::AllocateMilestones() {
    Attrib::Gen::gameplay gameplay(Attrib::FindCollection(Attrib::Gen::gameplay::ClassKey(), 0x1D975142), 0, nullptr);
    AttribKeyList keys;
    AttribKeyList::iterator it;
    unsigned int i;

    GatherInstanceKeys(gameplay, keys, 0xA3D34781);

    mNumMilestones = 0;
    for (it = keys.begin(); it != keys.end(); ++it) {
        mNumMilestones++;
    }

    mMilestones = new GMilestone[mNumMilestones];

    i = 0;
    for (it = keys.begin(); it != keys.end(); ++it) {
        mMilestones[i].Init(*it);
        i++;
    }
}

void GManager::ResetMilestoneTrackingInfo() {
    mMilestoneTypeInfo.clear();
}

void GManager::LoadMilestoneInfo(MilestoneTypeInfo *savedInfo, unsigned int count) {
    unsigned int i;

    ResetMilestoneTrackingInfo();
    for (i = 0; i < count; ++i) {
        MilestoneTypeInfo &info = savedInfo[i];
        mMilestoneTypeInfo[info.mTypeKey] = info;
    }
}

unsigned int GManager::SaveMilestoneInfo(MilestoneTypeInfo *dest) {
    MilestoneInfoMap::iterator it;

    for (it = mMilestoneTypeInfo.begin(); it != mMilestoneTypeInfo.end(); ++it, ++dest) {
        *dest = it->second;
    }

    return mMilestoneTypeInfo.size();
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

void GManager::AllocateSpeedTraps() {
    Attrib::Gen::gameplay gameplay(Attrib::FindCollection(Attrib::Gen::gameplay::ClassKey(), 0x49511906), 0, nullptr);
    AttribKeyList keys;
    AttribKeyList::iterator it;
    unsigned int i;

    GatherInstanceKeys(gameplay, keys, 0xB05871D3);

    mNumSpeedTraps = 0;
    for (it = keys.begin(); it != keys.end(); ++it) {
        mNumSpeedTraps++;
    }

    mSpeedTraps = new GSpeedTrap[mNumSpeedTraps];

    i = 0;
    for (it = keys.begin(); it != keys.end(); ++it) {
        mSpeedTraps[i].Init(*it);
        i++;
    }
}

void GManager::ResetSpeedTraps() {
    if (!mSpeedTraps) {
        return;
    }

    for (unsigned int i = 0; i < mNumSpeedTraps; ++i) {
        mSpeedTraps[i].Reset();
    }
}

void GManager::GatherInstanceKeys(Attrib::Gen::gameplay &collection, AttribKeyList &list, unsigned int templateKey) {
    Attrib::Key parentKey;
    unsigned int i;

    parentKey = collection.GetParent();
    while (parentKey != 0) {
        if (parentKey == templateKey) {
            list.push_back(collection.GetCollection());
            break;
        }

        {
            Attrib::Gen::gameplay parent(Attrib::FindCollection(Attrib::Gen::gameplay::ClassKey(), parentKey), 0, nullptr);
            parentKey = parent.GetParent();
        }
    }

    for (i = 0; i < collection.Num_Children(); ++i) {
        Attrib::Gen::gameplay child(Attrib::FindCollection(Attrib::Gen::gameplay::ClassKey(), collection.Children(i).GetCollectionKey()), 0, nullptr);
        GatherInstanceKeys(child, list, templateKey);
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
