#include "Speed/Indep/Src/Gameplay/GManager.h"

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/gameplay.h"
#include "Speed/Indep/Src/Gameplay/GMarker.h"
#include "Speed/Indep/Src/Gameplay/GVault.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Misc/LZCompress.hpp"
#include "Speed/Indep/Src/Misc/Platform.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribLoadAndGo.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#include <algorithm>
#include <new>

char *bStrIStr(const char *s1, const char *s2);
void bCloseMemoryPool(int pool_num);
bool bSetMemoryPoolDebugTracing(int pool_num, bool on_off);
void LZByteSwapHeader(LZHeader *header);

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

GVault *GManager::FindVault(const char *vaultName) {
    int lower = 0;
    int upper = mVaultCount - 1;

    while (lower <= upper) {
        int middle = (lower + upper) / 2;
        int cmp = bStrCmp(vaultName, mVaults[middle].GetName());

        if (cmp < 0) {
            upper = middle - 1;
        } else if (cmp > 0) {
            lower = middle + 1;
        } else {
            return &mVaults[middle];
        }
    }

    return nullptr;
}

GVault *GManager::FindVaultContaining(unsigned int collectionKey) {
    unsigned int collectionType = Attrib::StringToTypeID("Attrib::CollectionLoadData");

    for (int i = 0; i < mVaultCount; ++i) {
        GVault *vault = &mVaults[i];

        if (vault->IsLoaded()) {
            Attrib::Vault *attribVault = vault->GetAttribVault();
            unsigned int exportCount = attribVault->CountExports();

            for (unsigned int exportIndex = 0; exportIndex < exportCount; ++exportIndex) {
                if (attribVault->GetExportType(exportIndex) == collectionType) {
                    Attrib::Collection *collection = reinterpret_cast<Attrib::Collection *>(attribVault->GetExportData(exportIndex));

                    if (collection && collection->GetKey() == collectionKey) {
                        return vault;
                    }
                }
            }
        }
    }

    return nullptr;
}

void GManager::LoadCoreVault(AttribVaultPackImage *packImage) {
    FindVault("gpcore")->LoadResident(packImage);
}

void GManager::UnloadCoreVault() {
    FindVault("gpcore")->Unload();
}

void GManager::PreloadTransientVaults(AttribVaultPackImage *packImage) {
    mTransientPoolMemory = bMalloc(0x164000, 0x47);
    mTransientPoolNumber = bGetFreeMemoryPoolNum();
    bInitMemoryPool(mTransientPoolNumber, mTransientPoolMemory, 0x164000, "GManager Temp");
    bSetMemoryPoolDebugTracing(mTransientPoolNumber, false);

    for (unsigned int i = 0; i < mVaultCount; ++i) {
        DVDErrorTask(nullptr, 0);

        if (!mVaults[i].IsLoaded()) {
            mVaults[i].PreloadTransient(packImage, mTransientPoolNumber);
        }
    }
}

void GManager::Init(const char *vaultPackName) {
    mObj = new GManager(vaultPackName);
    mObj->InitializeVaults();
}

void GManager::InitializeVaults() {
    char compressedFilename[128];
    void *compressedBuf;
    LZHeader *header;
    AttribVaultPackImage *imageBuffer;
    unsigned int tableBufferSize;

    bStrCpy(compressedFilename, mVaultPackFileName);
    bStrCpy(bStrIStr(compressedFilename, ".BIN"), ".LZC");

    compressedBuf = bGetFile(compressedFilename, nullptr, 0);
    header = reinterpret_cast<LZHeader *>(compressedBuf);
    LZByteSwapHeader(header);
    imageBuffer = static_cast<AttribVaultPackImage *>(bMalloc(header->UncompressedSize, 0x1047));
    LZDecompress(reinterpret_cast<uint8 *>(compressedBuf), reinterpret_cast<uint8 *>(imageBuffer));
    bFree(compressedBuf);

    mLoadingPackImage = imageBuffer;
    mLoadingPackImage->EndianSwap();

    tableBufferSize = mGameplayClass->GetTableNodeSize() << 14;
    mClassTempBuffer = bMalloc(tableBufferSize, 0x40);
    mGameplayClass->SetTableBuffer(mClassTempBuffer, tableBufferSize);

    BuildVaultTable(mLoadingPackImage);
    LoadCoreVault(mLoadingPackImage);
    PreloadTransientVaults(mLoadingPackImage);
    FindKeyReductionShifts();
    AllocateIcons();
    AllocateMilestones();
    AllocateSpeedTraps();
    FindBountySpawnPoints();
    RefreshZoneIcons();
    RefreshTrackMarkerIcons();
}

void GManager::InitializeRaceStreaming() {
    AllocateStreamingBuffers();
    AllocateInstanceMap();
    UnloadTransientVaults();

    mGameplayClass->SetTableBuffer(nullptr, mMaxObjects * 3 * mGameplayClass->GetTableNodeSize());
    bFree(mClassTempBuffer);
    mClassTempBuffer = nullptr;
    bFree(mLoadingPackImage);
    mLoadingPackImage = nullptr;
}

void GManager::UnloadTransientVaults() {
    for (unsigned int i = 0; i < mVaultCount; ++i) {
        if (mVaults[i].IsTransient()) {
            mVaults[i].Unload();
        }
    }

    bCloseMemoryPool(mTransientPoolNumber);
    bFree(mTransientPoolMemory);
    mTransientPoolNumber = 0;
    mTransientPoolMemory = nullptr;
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

void GManager::RegisterInstance(GRuntimeInstance *instance) {
    unsigned int key;
    unsigned int collisions;
    unsigned int slot;

    key = instance->GetCollection() >> (mCollectionKeyShiftTo32 & 0x1F);
    collisions = 0;
    if (mInstanceHashTableSize != 0) {
        slot = key & mInstanceHashTableMask;
        do {
            if (!mKeyToInstanceMap[slot].mInstance) {
                if (mWorstHashCollision < collisions) {
                    mWorstHashCollision = collisions;
                }

                mKeyToInstanceMap[slot].mKey32 = key;
                mKeyToInstanceMap[slot].mInstance = instance;
                return;
            }

            collisions++;
            slot = (slot + 1) & mInstanceHashTableMask;
        } while (collisions < mInstanceHashTableSize);
    }
}

void GManager::UnregisterInstance(GRuntimeInstance *instance) {
    unsigned int key;
    unsigned int collisions;
    unsigned int slot;

    key = instance->GetCollection() >> (mCollectionKeyShiftTo32 & 0x1F);
    collisions = 0;
    slot = key & mInstanceHashTableMask;
    do {
        if (mKeyToInstanceMap[slot].mKey32 == key) {
            mKeyToInstanceMap[slot].mKey32 = 0;
            mKeyToInstanceMap[slot].mInstance = nullptr;
            return;
        }

        collisions++;
        slot = (slot + 1) & mInstanceHashTableMask;
    } while (collisions <= mWorstHashCollision);
}

GRuntimeInstance *GManager::FindInstance(Attrib::Key key) const {
    unsigned int key32;
    unsigned int collisions;
    unsigned int slot;

    collisions = 0;
    key32 = key >> (mCollectionKeyShiftTo32 & 0x1F);
    slot = key32 & mInstanceHashTableMask;
    do {
        if (mKeyToInstanceMap[slot].mKey32 == key32) {
            return mKeyToInstanceMap[slot].mInstance;
        }

        collisions++;
        slot = (slot + 1) & mInstanceHashTableMask;
    } while (collisions <= mWorstHashCollision);

    return nullptr;
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

void GManager::ConnectInstanceReferences(GRuntimeInstance *runtimeInstance, const Attrib::Gen::gameplay &collection) {}

void GManager::ConnectRuntimeInstances() {
    for (unsigned int i = 0; i < mInstanceHashTableSize; ++i) {
        GRuntimeInstance *instance = mKeyToInstanceMap[i].mInstance;

        if (instance) {
            instance->ResetConnections();
            ConnectChildren(instance);
            instance->LockConnections();
        }
    }
}

void GManager::ConnectChildren(GRuntimeInstance *runtimeInstance) {
    for (unsigned int i = 0; i < runtimeInstance->Num_Children(); ++i) {
        GRuntimeInstance *child = FindInstance(runtimeInstance->Children(i).GetCollectionKey());

        if (child) {
            runtimeInstance->ConnectToInstance(GetStrippedNameKey(child->CollectionName()), 0, child);
        }
    }
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

void GManager::AllocateIcons() {
    mNumIcons = 0;
    mNumVisibleIcons = 0;
    mIcons = new GIcon *[200];
}

void GManager::ReleaseIcons() {
    if (mIcons) {
        delete[] mIcons;
    }

    mIcons = nullptr;
    mNumIcons = 0;
    mNumVisibleIcons = 0;
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

GSpeedTrap *GManager::GetFirstSpeedTrap(bool activeOnly, unsigned int binNumber) {
    return GetNextSpeedTrap(mSpeedTraps - 1, activeOnly, binNumber);
}

void GManager::EnableBinSpeedTraps(unsigned int binNumber) {
    for (GSpeedTrap *speedTrap = GetFirstSpeedTrap(false, binNumber); speedTrap;
         speedTrap = GetNextSpeedTrap(speedTrap, false, binNumber)) {
        speedTrap->Unlock();
        speedTrap->Activate();
    }
}

void GManager::RefreshSpeedTrapIcons() {
    for (GSpeedTrap *speedTrap = GetFirstSpeedTrap(false, 0); speedTrap;
         speedTrap = GetNextSpeedTrap(speedTrap, false, 0)) {
        GTrigger *trigger = speedTrap->GetTrapTrigger();

        if (trigger) {
            if (!speedTrap->IsFlagSet(GSpeedTrap::kFlag_Active) || speedTrap->IsFlagSet(GSpeedTrap::kFlag_Completed)) {
                trigger->HideIcon();
            } else {
                trigger->ShowIcon();
            }
        }
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

void GManager::ReserveStockCar(const char *carName) {
    unsigned int carKey;

    if (!carName || !*carName) {
        return;
    }

    carKey = Attrib::StringHash32(carName);
    if (mStockCars.find(carKey) == mStockCars.end()) {
        mStockCars.insert(StockCarMap::value_type(carKey, static_cast<ISimable *>(nullptr)));
    }
}

bool GManager::StockCarsLoaded() {
    StockCarMap::iterator it;

    for (it = mStockCars.begin(); it != mStockCars.end(); ++it) {
        if (!it->second) {
            return false;
        }
    }

    return true;
}

ISimable *GManager::GetStockCar(const char *carName) {
    StockCarMap::iterator it;
    ISimable *stockCar;

    it = mStockCars.find(Attrib::StringHash32(carName));
    if (it == mStockCars.end()) {
        return nullptr;
    }

    stockCar = it->second;
    mStockCars.erase(it);
    return stockCar;
}

ISimable *GManager::GetRandomEmergencyStockCar() {
    StockCarMap::iterator it;
    int index;
    ISimable *stockCar;

    if (mStockCars.empty()) {
        return nullptr;
    }

    index = bRandom(static_cast<int>(mStockCars.size()));
    it = mStockCars.begin();
    while (index > 0 && it != mStockCars.end()) {
        ++it;
        index--;
    }

    if (it == mStockCars.end()) {
        return nullptr;
    }

    stockCar = it->second;
    mStockCars.erase(it);
    return stockCar;
}

void GManager::ReleaseStockCar(ISimable *stockCar) {
    if (!stockCar) {
        return;
    }

    stockCar->Kill();
    mStockCars[stockCar->GetAttributes().GetCollection()] = stockCar;
}

void GManager::AttachCharacter(GCharacter *character) {
    if (std::find(mActiveCharacters.begin(), mActiveCharacters.end(), character) == mActiveCharacters.end()) {
        mActiveCharacters.push_back(character);
    }
}

void GManager::DetachCharacter(GCharacter *character) {
    GCharacterList::iterator it = std::find(mActiveCharacters.begin(), mActiveCharacters.end(), character);

    if (it != mActiveCharacters.end()) {
        mActiveCharacters.erase(it);
    }
}

void GManager::UnspawnAllCharacters() {
    while (!mActiveCharacters.empty()) {
        mActiveCharacters.front()->Unspawn();
    }
}

bool GManager::GetHasPendingSMS() const {
    return !mPendingSMS.empty();
}

unsigned int GManager::SaveSMSInfo(int *saveInfo) {
    PendingSMSList::const_iterator it;
    unsigned int count;

    count = 0;
    for (it = mPendingSMS.begin(); it != mPendingSMS.end(); ++it) {
        *saveInfo++ = *it;
        count++;
    }

    return count;
}

void GManager::LoadSMSInfo(int *loadInfo, unsigned int count) {
    unsigned int i;

    mPendingSMS.clear();
    for (i = 0; i < count; ++i) {
        mPendingSMS.push_back(loadInfo[i]);
    }
}

bool GManager::CanPlaySMS() const {
    return !mPendingSMS.empty();
}

void GManager::DispatchSMSMessage(int smsID) {
    mPendingSMS.push_back(smsID);
}

void GManager::AddSMS(int smsID) {
    PendingSMSList::iterator it;

    if ((smsID - 10U < 5) || smsID == 0x5F || smsID == 0x60) {
        DispatchSMSMessage(smsID);
        return;
    }

    for (it = mPendingSMS.begin(); it != mPendingSMS.end(); ++it) {
        if (*it == smsID) {
            return;
        }
    }

    mPendingSMS.push_back(smsID);
}

int GManager::PushSMSToInbox() {
    int smsID;

    smsID = -1;
    if (!mPendingSMS.empty()) {
        smsID = mPendingSMS.front();
    }

    mPendingSMS.clear();
    return smsID;
}

void GManager::UpdatePendingSMS() {
    if (GetHasPendingSMS() && CanPlaySMS()) {
        PushSMSToInbox();
    }
}

bool GManager::SaveGameplayData(unsigned char *dest, unsigned int maxSize) {
    SavedGameplayDataHeader *header;
    unsigned char *cursor;
    ObjectStateMap::iterator it;

    if (maxSize < 0x80) {
        return false;
    }

    bMemSet(dest, 0, maxSize);
    header = reinterpret_cast<SavedGameplayDataHeader *>(dest);
    header->mMagic = 0x656D6147;
    header->mVersion = 8;
    header->mNumPersistent = mPersistentStateBlocks.size();

    cursor = dest + 0x80;
    for (it = mPersistentStateBlocks.begin(); it != mPersistentStateBlocks.end(); ++it) {
        ObjectStateBlockHeader *block = it->second;
        unsigned int blockSize = (block->mSize + 0x17U) & ~0xFU;

        if (static_cast<unsigned int>(cursor - dest) + blockSize > maxSize) {
            return false;
        }

        bMemCpy(cursor, block, blockSize);
        cursor += blockSize;
    }

    cursor = reinterpret_cast<unsigned char *>((reinterpret_cast<unsigned int>(cursor) + 0xFU) & ~0xFU);
    header->mNumSavedTimers = SaveTimerInfo(reinterpret_cast<SavedTimerInfo *>(cursor));
    cursor += header->mNumSavedTimers * 0x20;

    header->mNumMilestoneTypes = SaveMilestoneInfo(reinterpret_cast<MilestoneTypeInfo *>(cursor));
    cursor += header->mNumMilestoneTypes * 0x10;

    header->mNumMilestoneRecords = SaveMilestones(reinterpret_cast<GMilestone *>(cursor));
    cursor += header->mNumMilestoneRecords * 0x14;
    cursor = reinterpret_cast<unsigned char *>((reinterpret_cast<unsigned int>(cursor) + 0xFU) & ~0xFU);

    header->mNumSpeedTrapRecords = SaveSpeedTraps(reinterpret_cast<GSpeedTrap *>(cursor));
    cursor += header->mNumSpeedTrapRecords * 0x14;
    cursor = reinterpret_cast<unsigned char *>((reinterpret_cast<unsigned int>(cursor) + 0xFU) & ~0xFU);

    header->mNumHidingSpotFlags = 0x200;
    bMemCpy(cursor, mHidingSpotFound, sizeof(mHidingSpotFound));
    cursor += sizeof(mHidingSpotFound);

    header->mNumBytesBinStats = GRaceDatabase::Get().SerializeBins(cursor);
    cursor += header->mNumBytesBinStats;
    cursor = reinterpret_cast<unsigned char *>((reinterpret_cast<unsigned int>(cursor) + 0xFU) & ~0xFU);

    header->mNumPendingSMS = SaveSMSInfo(reinterpret_cast<int *>(cursor));
    cursor += header->mNumPendingSMS * sizeof(int);
    cursor = reinterpret_cast<unsigned char *>((reinterpret_cast<unsigned int>(cursor) + 0xFU) & ~0xFU);

    bMemCpy(cursor, &mFreeRoamStartMarker, sizeof(mFreeRoamStartMarker));
    bMemCpy(cursor + 0x10, &mFreeRoamFromSafeHouseStartMarker, sizeof(mFreeRoamFromSafeHouseStartMarker));
    return true;
}

bool GManager::LoadGameplayData(unsigned char *src, unsigned int maxSize) {
    SavedGameplayDataHeader *header;
    unsigned char *cursor;
    unsigned int i;

    if (maxSize < 0x80) {
        return false;
    }

    header = reinterpret_cast<SavedGameplayDataHeader *>(src);
    if (header->mMagic != 0x656D6147 || header->mVersion < 8) {
        return false;
    }

    ResetAllGameplayData();

    cursor = src + 0x80;
    for (i = 0; i < header->mNumPersistent; ++i) {
        ObjectStateBlockHeader *savedBlock = reinterpret_cast<ObjectStateBlockHeader *>(cursor);
        ObjectStateBlockHeader *block = AllocObjectStateBlock(savedBlock->mKey, savedBlock->mSize, true);

        if (block) {
            bMemCpy(block, savedBlock, savedBlock->mSize + sizeof(ObjectStateBlockHeader));
        }

        cursor += (savedBlock->mSize + 0x17U) & ~0xFU;
    }

    cursor = reinterpret_cast<unsigned char *>((reinterpret_cast<unsigned int>(cursor) + 0xFU) & ~0xFU);
    LoadTimerInfo(reinterpret_cast<SavedTimerInfo *>(cursor), header->mNumSavedTimers);
    cursor += header->mNumSavedTimers * 0x20;

    LoadMilestoneInfo(reinterpret_cast<MilestoneTypeInfo *>(cursor), header->mNumMilestoneTypes);
    cursor += header->mNumMilestoneTypes * 0x10;

    LoadMilestones(reinterpret_cast<GMilestone *>(cursor), header->mNumMilestoneRecords);
    cursor += header->mNumMilestoneRecords * 0x14;
    cursor = reinterpret_cast<unsigned char *>((reinterpret_cast<unsigned int>(cursor) + 0xFU) & ~0xFU);

    LoadSpeedTraps(reinterpret_cast<GSpeedTrap *>(cursor), header->mNumSpeedTrapRecords);
    cursor += header->mNumSpeedTrapRecords * 0x14;
    cursor = reinterpret_cast<unsigned char *>((reinterpret_cast<unsigned int>(cursor) + 0xFU) & ~0xFU);

    bMemCpy(mHidingSpotFound, cursor, sizeof(mHidingSpotFound));
    cursor += (header->mNumHidingSpotFlags + 7U) >> 3;
    cursor = reinterpret_cast<unsigned char *>((reinterpret_cast<unsigned int>(cursor) + 0xFU) & ~0xFU);

    cursor += GRaceDatabase::Get().DeserializeBins(cursor);
    cursor = reinterpret_cast<unsigned char *>((reinterpret_cast<unsigned int>(cursor) + 0xFU) & ~0xFU);

    LoadSMSInfo(reinterpret_cast<int *>(cursor), header->mNumPendingSMS);
    cursor += header->mNumPendingSMS * sizeof(int);
    cursor = reinterpret_cast<unsigned char *>((reinterpret_cast<unsigned int>(cursor) + 0xFU) & ~0xFU);

    bMemCpy(&mFreeRoamStartMarker, cursor, sizeof(mFreeRoamStartMarker));
    bMemCpy(&mFreeRoamFromSafeHouseStartMarker, cursor + 0x10, sizeof(mFreeRoamFromSafeHouseStartMarker));
    return true;
}

void GManager::DefragObjectStateStorage() {
    unsigned int count;
    unsigned int index;
    ObjectStateBlockHeader **blocks;
    ObjectStateMap::iterator it;
    unsigned char *freePtr;

    count = mPersistentStateBlocks.size() + mSessionStateBlocks.size();
    if (count == 0) {
        mObjectStateBufferFree = mObjectStateBuffer;
        return;
    }

    blocks = new ObjectStateBlockHeader *[count];
    index = 0;
    for (it = mPersistentStateBlocks.begin(); it != mPersistentStateBlocks.end(); ++it) {
        blocks[index++] = it->second;
    }
    for (it = mSessionStateBlocks.begin(); it != mSessionStateBlocks.end(); ++it) {
        blocks[index++] = it->second;
    }

    std::sort(blocks, blocks + count);
    freePtr = mObjectStateBuffer;
    for (index = 0; index < count; ++index) {
        ObjectStateBlockHeader *block = blocks[index];
        unsigned int blockSize = (block->mSize + 0x17U) & ~0xFU;

        if (reinterpret_cast<unsigned char *>(block) != freePtr) {
            bOverlappedMemCpy(freePtr, block, blockSize);

            it = mPersistentStateBlocks.find(block->mKey);
            if (it != mPersistentStateBlocks.end()) {
                it->second = reinterpret_cast<ObjectStateBlockHeader *>(freePtr);
            }

            it = mSessionStateBlocks.find(block->mKey);
            if (it != mSessionStateBlocks.end()) {
                it->second = reinterpret_cast<ObjectStateBlockHeader *>(freePtr);
            }
        }

        freePtr += blockSize;
    }

    delete[] blocks;
    mObjectStateBufferFree = freePtr;
}

void GManager::UpdatePursuit() {
    IPursuit *pursuit;
    IPerpetrator *perpetrator;
    bool roaming;
    bool challengeRace;
    bool cooldown;

    pursuit = nullptr;
    perpetrator = nullptr;
    roaming = GRaceStatus::Exists() && GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming;
    GetPlayerPursuitInterfaces(pursuit, perpetrator);

    cooldown = false;
    if (pursuit) {
        TrackValue("cost_to_state_in_pursuit", static_cast<float>(pursuit->CalcTotalCostToState()));
        cooldown = pursuit->GetPursuitStatus() == 2;
    }

    if (perpetrator) {
        TrackValue("cost_to_state", static_cast<float>(perpetrator->GetCostToState()));
        TrackValue("bounty_in_pursuit",
                   static_cast<float>(perpetrator->GetPendingRepPointsNormal() +
                                      perpetrator->GetPendingRepPointsFromCopDestruction()));
    }

    challengeRace = GRaceStatus::Exists() && GRaceStatus::Get().GetRaceParameters() &&
                    GRaceStatus::Get().GetRaceParameters()->GetIsChallengeSeriesRace();

    mHidingSpotIconsShown = (roaming || challengeRace) && pursuit && cooldown;
    mPursuitBreakerIconsShown = (roaming || challengeRace) && pursuit && !cooldown;
}

bool GManager::CalcMapCoordsForMarker(unsigned int markerKey, bVector2 &outPos, float &outRotDeg) {
    GMarker *marker;
    UMath::Vector3 pos;
    UMath::Vector3 dir;

    marker = static_cast<GMarker *>(FindInstance(markerKey));
    if (!marker) {
        return false;
    }

    pos = marker->GetPosition();
    dir = marker->GetDirection();
    outPos.x = pos.x;
    outPos.y = pos.z;
    outRotDeg = bAngToDeg(bATan(-dir.x, dir.z));
    return true;
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
