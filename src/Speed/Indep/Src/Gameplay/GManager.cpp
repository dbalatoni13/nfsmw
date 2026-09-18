#include "GManager.h"

#include "GIcon.h"
#include "GMarker.h"
#include "GRaceDatabase.h"
#include "GRaceStatus.h"
#include "GRuntimeInstance.h"
#include "GVault.h"
#include "GTrigger.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/AI/AIBasics.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/P2temp.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/gameplay.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/gameplay_hash.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/milestonetypes.h"
#include "Speed/Indep/Src/Generated/Events/EAutoSave.hpp"
#include "Speed/Indep/Src/Generated/Events/ECellCall.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOff.hpp"
#include "Speed/Indep/Src/Generated/Events/EShowSMS.hpp"
#include "Speed/Indep/Src/Generated/Messages/MEnteringGameplay.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyMilestoneProgress.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ICopMgr.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ITrafficMgr.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IArticulatedVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/AttribVaultPack.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/MD5.hpp"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/LZCompress.hpp"
#include "Speed/Indep/Src/World/Rain.hpp"
#include "Speed/Indep/Src/World/TimeOfDay.hpp"
#include "Speed/Indep/Src/World/World.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribLoadAndGo.h"

#include <algorithm>
#include <stdlib.h>
#include <string.h>

namespace Attrib {
Key GetCollectionKey(const Collection *c);
}
#include "Speed/Indep/Src/World/TrackInfo.hpp"
#include "Speed/Indep/Src/Interfaces/IMenuZoneTrigger.h"
#include "Speed/Indep/Src/World/TrackPath.hpp"
#include "Speed/Indep/Src/World/TrackPositionMarker.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

class WorldMap {
  public:
    static void ClearGPSing();

    static GIcon *mGPSingIcon;
};

class PhotoFinishScreen {
  public:
    static bool GetActive() {
        return mActive;
    }

    static bool mActive;
};

// El objetivo las define en .data en ESTE orden, justo antes de GManager::mObj
// (zGameplay.s .data:0xDC y 0xE0). Sin definicion quedaban UNDEF en el .o.
int TWEAK_ShowGameplayMilestoneValues = 0;
int TWEAK_ShowAllGameplayIcons = 0;

GManager *GManager::mObj = NULL;

void GManager::Init(const char *vaultPackName) {
    mObj = new ("GManager", 0) GManager(vaultPackName);
    mObj->InitializeVaults();
}

GManager::GManager(const char *vaultPackName)
    : UTL::COM::Object(1),                     //
      IVehicleCache(this),                     //
      mVaultPackFileName(vaultPackName),       //
      mVaultPackFile(NULL),                    //
      mVaultCount(0),                          //
      mVaults(NULL),                           //
      mVaultNameStrings(NULL),                 //
      mLoadingPackImage(NULL),                 //
      mBinSlotSize(0),                         //
      mStreamedBinSlots(NULL),                 //
      mRaceSlotSize(0),                        //
      mStreamedRaceSlots(NULL),                //
      mTempLoadData(NULL),                     //
      mTransientPoolNumber(0),                 //
      mTransientPoolMemory(NULL),              //
      mGameplayClass(NULL),                    //
      mMilestoneClass(NULL),                   //
      mAttributeKeyShiftTo24(0),               //
      mCollectionKeyShiftTo32(0),              //
      mMaxObjects(0),                          //
      mClassTempBuffer(NULL),                  //
      mInstanceHashTableSize(0),               //
      mInstanceHashTableMask(0),               //
      mWorstHashCollision(0),                  //
      mKeyToInstanceMap(NULL),                 //
      mNumMilestones(0),                       //
      mMilestones(NULL),                       //
      mNumSpeedTraps(0),                       //
      mSpeedTraps(NULL),                       //
      mNumBountySpawnPoints(0),                //
      mFreeRoamStartMarker(0),                 //
      mFreeRoamFromSafeHouseStartMarker(0),    //
      mStartFreeRoamFromSafeHouse(false),      //
      mStartFreeRoamPursuit(false),            //
      mQueuedPursuitMinHeat(0.0f),             //
      mInGameplay(false),                      //
      mOverrideFreeRoamStartMarker(0),         //
      mObjectStateBuffer(NULL),                //
      mObjectStateBufferFree(NULL),            //
      mObjectStateBufferSize(0),               //
      mWarping(false),                         //
      mWarpStartPursuit(false),                //
      mWarpTargetMarker(0),                    //
      mNumIcons(0),                            //
      mNumVisibleIcons(0),                     //
      mIcons(NULL),                            //
      mPursuitBreakerIconsShown(false),        //
      mHidingSpotIconsShown(false),            //
      mEventIconsShown(false),                 //
      mMenuGateIconsShown(false),              //
      mSpeedTrapIconsShown(false),             //
      mSpeedTrapRaceIconsShown(false),         //
      mAllowEngageEvents(false),               //
      mAllowEngageSafehouse(false),            //
      mAllowMenuGates(false),                  //
      mRestartEventHash(0) {
    mGameplayClass = Attrib::Database::Get().GetClass(Attrib::Gen::gameplay::ClassKey());
    mMilestoneClass = Attrib::Database::Get().GetClass(Attrib::Gen::milestonetypes::ClassKey());

    bMemSet(mBinVaultInSlot, 0, sizeof(mBinVaultInSlot));
    bMemSet(mRaceVaultInSlot, 0, sizeof(mRaceVaultInSlot));
    bMemSet(mHidingSpotFound, 0, sizeof(mHidingSpotFound));
    bMemSet(mBountySpawnPoint, 0, sizeof(mBountySpawnPoint));

    mActiveCharacters.reserve(16);

    AllocateObjectStateStorage();
    ResetAllGameplayData();
}

GManager::~GManager() {
    mActiveCharacters.clear();

    ReleaseSpeedTraps();
    ReleaseMilestones();
    FreeAllIcons();
    ClearStockCars();
    UnloadCoreVault();
    ReleaseIcons();
    ReleaseInstanceMap();
    ReleaseStreamingBuffers();
    ReleaseObjectStateStorage();
    DestroyVaults();

    if (mVaultPackFile) {
        bClose(mVaultPackFile);
        mVaultPackFile = NULL;
    }
}

void GManager::InitializeVaults() {
    char compressed_filename[128];

    bStrCpy(compressed_filename, mVaultPackFileName);
    bStrCpy(bStrIStr(compressed_filename, ".BIN"), ".LZC");

    LZHeader *compressed_buf = (LZHeader *)bGetFile(compressed_filename, NULL, 0);
    LZByteSwapHeader(compressed_buf);

    void *imageBuffer = bMalloc(compressed_buf->UncompressedSize, "Gameplay vault pack image (temporary)", 0, 0x1047);
    LZDecompress((uint8 *)compressed_buf, (uint8 *)imageBuffer);
    bFree(compressed_buf);

    mLoadingPackImage = (AttribVaultPackImage *)imageBuffer;
    mLoadingPackImage->EndianSwap();

    unsigned int tableBufferSize = mGameplayClass->GetTableNodeSize() << 14;
    mClassTempBuffer = bMalloc(tableBufferSize, "Gameplay collection table (temporary)", 0, 0x40);
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

    unsigned int numNodes = mMaxObjects * 3;

    mGameplayClass->SetTableBuffer(NULL, numNodes * mGameplayClass->GetTableNodeSize());

    bFree(mClassTempBuffer);
    mClassTempBuffer = NULL;

    bFree(mLoadingPackImage);
    mLoadingPackImage = NULL;
}

void GManager::BuildVaultTable(AttribVaultPackImage *packImage) {
    mVaultNameStrings = new ("Gameplay vault name strings", 0) char[packImage->mHeader.mStringBlockSize];

    bMemCpy((void *) mVaultNameStrings, packImage->GetData(packImage->mHeader.mStringBlockOffset), packImage->mHeader.mStringBlockSize);

    unsigned int numVaults = packImage->mHeader.mNumEntries;

    mVaultCount = numVaults;
    unsigned int allocFlags = GetVirtualMemoryAllocParams() | 0x1000;
    mVaults = (GVault *) bMalloc(numVaults * sizeof(GVault), "Gameplay vault tracking array", 0, allocFlags);

    for (unsigned int onVault = 0; onVault < mVaultCount; onVault++) {
        AttribVaultPackEntry &packEntry = packImage->GetEntry(onVault);
        const char *vaultName = mVaultNameStrings + packEntry.mVaultNameOffset;

        GVault *vaultPtr = &mVaults[onVault];

        new (vaultPtr) GVault(&packEntry, vaultName);
    }
}

GVault *GManager::FindVault(const char *name) {
    int lo = 0;
    int hi = mVaultCount - 1;

    while (lo <= hi) {
        int mid = (lo + hi) >> 1;
        int cmp = bStrCmp(name, mVaults[mid].GetName());

        if (cmp > 0) {
            lo = mid + 1;
        } else if (cmp < 0) {
            hi = mid - 1;
        } else {
            return &mVaults[mid];
        }
    }

    return NULL;
}

GVault *GManager::FindVaultContaining(unsigned int collectionKey) {
    for (unsigned int i = 0; i < mVaultCount; i++) {
        GVault *vault = &mVaults[i];

        if (!vault->IsLoaded()) {
            continue;
        }

        Attrib::Vault *attribVault = vault->GetAttribVault();
        unsigned int numExports = attribVault->CountExports();
        unsigned int collectionLoadDataType = Attrib::StringToTypeID("Attrib::CollectionLoadData");

        for (unsigned int e = 0; e < numExports; e++) {
            if (attribVault->GetExportType(e) == collectionLoadDataType) {
                void *exportData = attribVault->GetExportData(e);

                if (exportData != NULL && collectionKey == Attrib::GetCollectionKey((const Attrib::Collection *)exportData)) {
                    return vault;
                }
            }
        }
    }

    return NULL;
}

void GManager::LoadVaultSync(GVault *vault) {
    unsigned char *binBuffer;
    unsigned int bytesAvail;
    unsigned int bytesUsed;

    if (vault->IsRaceBin()) {
        unsigned int raceSlotIndex = GetAvailableRaceSlot();
        unsigned int binSlotIndex = GetAvailableBinSlot();

        binBuffer = mStreamedBinSlots + binSlotIndex * mBinSlotSize;

        mBinVaultInSlot[binSlotIndex] = vault;
    } else {
        unsigned int raceSlotIndex = GetAvailableRaceSlot();

        binBuffer = mStreamedRaceSlots + raceSlotIndex * mRaceSlotSize;

        mRaceVaultInSlot[raceSlotIndex] = vault;
    }

    mVaultPackFile = bOpen(mVaultPackFileName, 1, 1);

    bSeek(mVaultPackFile, vault->GetDataOffset(), 0);
    bRead(mVaultPackFile, binBuffer, vault->GetDataSize());

    bSeek(mVaultPackFile, vault->GetLoadDataOffset(), 0);
    bRead(mVaultPackFile, mTempLoadData, vault->GetLoadDataSize());

    bClose(mVaultPackFile);
    mVaultPackFile = NULL;

    vault->InitTransient(binBuffer, mTempLoadData);

    if (mInGameplay) {
        ConnectRuntimeInstances();
    }
}

int GManager::GetAvailableBinSlot() {
    if (mBinVaultInSlot[0]) {
        mBinVaultInSlot[0]->Unload();
        mBinVaultInSlot[0] = NULL;
    }

    return 0;
}

int GManager::GetAvailableRaceSlot() {
    if (mRaceVaultInSlot[0]) {
        mRaceVaultInSlot[0]->Unload();
        mRaceVaultInSlot[0] = NULL;
    }

    return 0;
}

void GManager::LoadCoreVault(AttribVaultPackImage *packImage) {
    GVault *vault = FindVault("gpcore");

    vault->LoadResident(packImage);
}

void GManager::PreloadTransientVaults(AttribVaultPackImage *packImage) {
    const int pool_size = 0x164000;

    mTransientPoolMemory = bMalloc(pool_size, "GManager Temp", 0, 0x47);
    mTransientPoolNumber = bGetFreeMemoryPoolNum();

    bInitMemoryPool(mTransientPoolNumber, mTransientPoolMemory, pool_size, "GManager Temp");

    bSetMemoryPoolDebugTracing(mTransientPoolNumber, false);

    for (unsigned int onVault = 0; onVault < mVaultCount; onVault++) {
        DVDErrorTask(NULL, 0);

        GVault *vault = mVaults + onVault;

        if (!vault->IsLoaded()) {
            vault->PreloadTransient(packImage, mTransientPoolNumber);
        }
    }
}

void GManager::FindKeyReductionShifts() {
    unsigned int numCollections = mGameplayClass->GetNumCollections();
    unsigned int numDefinitions = mGameplayClass->GetNumDefinitions();
    unsigned int numChildNames = numCollections;
    unsigned int maxCollKeys = numChildNames;
    unsigned int maxAttribKeys = numDefinitions + numCollections;
    unsigned int keyBufferSize = std::max(maxCollKeys, maxAttribKeys);
    Attrib::Key collectionKey = 0;
    unsigned int *keyBuffer = new ("Attrib::Key", 0) unsigned int[keyBufferSize];
    unsigned int *destKey = keyBuffer;

    mCollectionKeyShiftTo32 = collectionKey;

    Attrib::Key attribKey = mGameplayClass->GetFirstDefinition();

    while (attribKey) {
        *destKey = attribKey;
        attribKey = mGameplayClass->GetNextDefinition(attribKey);
        destKey++;
    }

    collectionKey = mGameplayClass->GetFirstCollection();

    while (collectionKey) {
        Attrib::Gen::gameplay instance(collectionKey, 0, NULL);

        *destKey = GetStrippedNameKey(instance.CollectionName());
        destKey++;
        collectionKey = mGameplayClass->GetNextCollection(collectionKey);
    }

    std::sort(keyBuffer, destKey);
    unsigned int *uniqueEnd = std::unique(keyBuffer, destKey);

    mAttributeKeyShiftTo24 = FindUniqueKeyShift(keyBuffer, uniqueEnd - keyBuffer, 0x18);

    if (keyBuffer) {
        delete[] keyBuffer;
    }
}

unsigned int GManager::FindUniqueKeyShift(unsigned int *attribKeys, unsigned int numKeys, unsigned int attribKeyBits) {
    if (numKeys > 1) {
        unsigned int maxShift = 32 - attribKeyBits;
        unsigned int uniqueBitMask = (1 << attribKeyBits) - 1;
        unsigned int keyMask = uniqueBitMask << maxShift;
        unsigned int clearHighMask = 0xFFFFFFFF;

        for (int onShift = maxShift; onShift >= 0; onShift--) {
            bool collision;

            for (unsigned int onClear = 0; onClear < numKeys; onClear++) {
                attribKeys[onClear] &= clearHighMask;
            }

            std::sort(attribKeys, attribKeys + numKeys);

            collision = false;

            for (unsigned int onCheck = 1; onCheck < numKeys; onCheck++) {
                unsigned int thisValue = attribKeys[onCheck] & keyMask;
                unsigned int prevValue = attribKeys[onCheck - 1] & keyMask;

                if (thisValue == prevValue) {
                    collision = true;
                    break;
                }
            }

            if (!collision) {
                return onShift;
            }

            clearHighMask >>= 1;
            keyMask >>= 1;
        }
    }

    return 0;
}

void GManager::AllocateIcons() {
    mNumIcons = 0;
    mNumVisibleIcons = 0;
    mIcons = new ("Gameplay icon pointers", 0) GIcon *[200];
}

void GManager::ReleaseIcons() {
    if (mIcons) {
        delete[] mIcons;
    }

    mNumIcons = 0;
    mNumVisibleIcons = 0;
    mIcons = NULL;
}

void GManager::AllocateObjectStateStorage() {
    mObjectStateBuffer = (unsigned char *)bMalloc(16384, "Gameplay persistent data pool", 0, GetVirtualMemoryAllocParams());
    mObjectStateBufferFree = mObjectStateBuffer;
    mObjectStateBufferSize = 16384;
}

void GManager::ReleaseObjectStateStorage() {
    mPersistentStateBlocks.clear();
    mSessionStateBlocks.clear();

    if (mObjectStateBuffer) {
        bFree(mObjectStateBuffer);
    }

    mObjectStateBufferFree = NULL;
    mObjectStateBufferSize = 0;
}

void GManager::DefragObjectStateStorage() {
    ObjectStateBlockHeader *stackList[256];

    unsigned int count = mPersistentStateBlocks.size() + mSessionStateBlocks.size();

    ObjectStateBlockHeader **list = stackList;

    if (count > 256) {
        list = new ("Temp buffer to defrag object state", 0) ObjectStateBlockHeader *[count];
    }

    unsigned int numBlocks = 0;

    for (ObjectStateMap::iterator it = mPersistentStateBlocks.begin(); it != mPersistentStateBlocks.end(); ++it) {
        list[numBlocks++] = it->second;
    }

    for (ObjectStateMap::iterator it = mSessionStateBlocks.begin(); it != mSessionStateBlocks.end(); ++it) {
        list[numBlocks++] = it->second;
    }

    std::sort(list, list + count);

    unsigned char *dest = mObjectStateBuffer;

    for (unsigned int i = 0; i < count; i++) {
        ObjectStateBlockHeader *header = list[i];

        unsigned int blockSize = (header->mSize + sizeof(ObjectStateBlockHeader) + 15) & ~15;

        if ((unsigned char *)header != dest) {
            bOverlappedMemCpy(dest, header, blockSize);

            unsigned int key32 = ReduceCollectionKey(((ObjectStateBlockHeader *)dest)->mKey);

            for (unsigned int j = 0; j < 2; j++) {
                ObjectStateMap &blocks = j ? mPersistentStateBlocks : mSessionStateBlocks;

                ObjectStateMap::iterator it = blocks.find(key32);

                if (it != blocks.end()) {
                    blocks[key32] = (ObjectStateBlockHeader *)dest;
                }
            }
        }

        dest += blockSize;
    }

    if (list != stackList) {
        delete[] list;
    }

    mObjectStateBufferFree = dest;
}

unsigned char *GManager::AllocObjectStateBlock(unsigned int key, unsigned int bytes, bool persistent) {
    ObjectStateMap &blocks = persistent ? mPersistentStateBlocks : mSessionStateBlocks;

    unsigned int key32 = ReduceCollectionKey(key);

    ObjectStateMap::iterator it = blocks.find(key32);

    if (it != blocks.end()) {
        ObjectStateBlockHeader *header = it->second;

        if (bytes <= header->mSize) {
            header->mSize = bytes;

            return (unsigned char *)(header + 1);
        }

        ClearObjectStateBlock(key);
    }

    unsigned int blockSize = (bytes + sizeof(ObjectStateBlockHeader) + 15) & ~15;

    for (unsigned int i = 0; i < 2; i++) {
        unsigned int used = mObjectStateBufferFree - mObjectStateBuffer;

        if (used + blockSize > mObjectStateBufferSize) {
            if (i == 0) {
                DefragObjectStateStorage();
            } else {
                return NULL;
            }
        }
    }

    ObjectStateBlockHeader *header = (ObjectStateBlockHeader *)mObjectStateBufferFree;

    mObjectStateBufferFree += blockSize;

    if (persistent) {
        bMemSet(header, 0, blockSize);
    }

    unsigned char *block = (unsigned char *)(header + 1);

    header->mKey = key;
    header->mSize = bytes;

    blocks[key32] = header;

    return block;
}

unsigned char *GManager::GetObjectStateBlock(unsigned int key) {
    unsigned int key32 = ReduceCollectionKey(key);

    for (unsigned int i = 0; i < 2; i++) {
        ObjectStateMap &blocks = i ? mPersistentStateBlocks : mSessionStateBlocks;

        ObjectStateMap::iterator it = blocks.find(key32);

        if (it != blocks.end()) {
            ObjectStateBlockHeader *header = it->second;

            return (unsigned char *)(header + 1);
        }
    }

    return NULL;
}

void GManager::ClearObjectStateBlock(unsigned int key) {
    unsigned int key32 = ReduceCollectionKey(key);

    for (unsigned int i = 0; i < 2; i++) {
        ObjectStateMap &blocks = i ? mPersistentStateBlocks : mSessionStateBlocks;

        ObjectStateMap::iterator it = blocks.find(key32);

        if (it != blocks.end()) {
            blocks.erase(it);
        }
    }
}

void GManager::ClearAllSessionData() {
    mSessionStateBlocks.clear();

    DefragObjectStateStorage();
}

static inline void AlignPointer(unsigned char *&ptr, unsigned int bound) {
    ptr = (unsigned char *)(((unsigned int)ptr + bound - 1) & ~(bound - 1));
}

bool GManager::SaveGameplayData(unsigned char *dest, unsigned int maxSize) {
    unsigned char *destStart = dest;

    bMemSet(dest, 0, maxSize);

    for (GObjectIterator<GActivity> activityIter(0xFFFFFFFF); activityIter.IsValid(); activityIter.Advance()) {
        activityIter.GetInstance()->SerializeVars(false);
    }

    if (maxSize <= 0x7f) {
        return false;
    }

    SavedGameplayDataHeader *gameplayHeader = (SavedGameplayDataHeader *)dest;
    gameplayHeader->mMagic = 0x656d6147;
    gameplayHeader->mVersion = 8;
    gameplayHeader->mNumPersistent = mPersistentStateBlocks.size();

    dest = (unsigned char *)gameplayHeader + 0x80;

    for (ObjectStateMap::iterator it = mPersistentStateBlocks.begin(); it != mPersistentStateBlocks.end(); ++it) {
        ObjectStateBlockHeader *block = it->second;
        unsigned int blockSize = (block->mSize + 0x17) & ~15;

        if (dest + blockSize > destStart + maxSize) {
            return false;
        }

        bMemCpy(dest, block, blockSize);
        dest += blockSize;
    }

    AlignPointer(dest, 16);

    gameplayHeader->mNumSavedTimers = SaveTimerInfo((SavedTimerInfo *)dest);
    dest += gameplayHeader->mNumSavedTimers << 5;

    AlignPointer(dest, 16);
    gameplayHeader->mNumMilestoneTypes = SaveMilestoneInfo((MilestoneTypeInfo *)dest);
    dest += gameplayHeader->mNumMilestoneTypes << 4;

    AlignPointer(dest, 16);
    gameplayHeader->mNumMilestoneRecords = SaveMilestones((GMilestone *)dest);
    dest += gameplayHeader->mNumMilestoneRecords * 0x14;

    AlignPointer(dest, 16);
    gameplayHeader->mNumSpeedTrapRecords = SaveSpeedTraps((GSpeedTrap *)dest);
    dest += gameplayHeader->mNumSpeedTrapRecords * 0x14;

    unsigned int spotBytes = sizeof(mHidingSpotFound);

    gameplayHeader->mNumHidingSpotFlags = spotBytes * 8;
    AlignPointer(dest, 16);
    bMemCpy(dest, mHidingSpotFound, spotBytes);
    dest += spotBytes;

    AlignPointer(dest, 16);
    gameplayHeader->mNumBytesBinStats = GRaceDatabase::Get().SerializeBins(dest);
    dest += gameplayHeader->mNumBytesBinStats;

    AlignPointer(dest, 16);
    gameplayHeader->mNumPendingSMS = SaveSMSInfo((int *)dest);
    dest += gameplayHeader->mNumPendingSMS << 2;

    AlignPointer(dest, 16);
    unsigned int respawnMarker = Get().mFreeRoamStartMarker;
    bMemCpy(dest, &respawnMarker, 4);

    dest += 4;
    AlignPointer(dest, 16);
    unsigned int respawnSafeHouseMarker = Get().mFreeRoamFromSafeHouseStartMarker;
    bMemCpy(dest, &respawnSafeHouseMarker, 4);

    dest += 4;
    AlignPointer(dest, 16);

    unsigned char *startChecksum = (unsigned char *)gameplayHeader + 0x10;
    unsigned int bytesToChecksum = maxSize - (startChecksum - destStart);

    MD5 md5;
    md5.Update(startChecksum, (int)bytesToChecksum);
    void *raw = md5.GetRaw();
    raw = md5.GetRaw();
    bMemCpy(gameplayHeader, raw, md5.GetRawLength());

    return true;
}

bool GManager::LoadGameplayData(unsigned char *src, unsigned int maxSize) {
    if (maxSize <= 0x7f) {
        return false;
    }

    unsigned char *srcStart = src;

    SavedGameplayDataHeader *gameplayHeader = (SavedGameplayDataHeader *)src;

    src += 0x80;

    unsigned char *startChecksum = (unsigned char *)gameplayHeader + 0x10;
    unsigned int bytesToChecksum = maxSize - 0x10;

    MD5 md5;
    md5.Update(startChecksum, (int)bytesToChecksum);
    md5.GetRaw();

    if (bMemCmp(gameplayHeader, md5.GetRaw(), md5.GetRawLength()) != 0) {
        return false;
    }

    if (gameplayHeader->mMagic != 0x656d6147) {
        return false;
    }

    if (gameplayHeader->mVersion <= 7) {
        return false;
    }

    ResetAllGameplayData();

    for (unsigned int onBlock = 0; onBlock < gameplayHeader->mNumPersistent; onBlock++) {
        ObjectStateBlockHeader *header = (ObjectStateBlockHeader *)src;
        unsigned int allocSize = (header->mSize + 0x17) & ~15;
        unsigned char *newBlock = AllocObjectStateBlock(header->mKey, header->mSize, true);

        if (newBlock) {
            unsigned char *data = (unsigned char *)header + 8;
            bMemCpy(newBlock, data, header->mSize);
        }

        src += allocSize;
    }

    AlignPointer(src, 16);
    LoadTimerInfo((SavedTimerInfo *)src, gameplayHeader->mNumSavedTimers);
    src += gameplayHeader->mNumSavedTimers << 5;

    AlignPointer(src, 16);
    LoadMilestoneInfo((MilestoneTypeInfo *)src, gameplayHeader->mNumMilestoneTypes);
    src += gameplayHeader->mNumMilestoneTypes << 4;

    AlignPointer(src, 16);
    LoadMilestones((GMilestone *)src, gameplayHeader->mNumMilestoneRecords);
    src += gameplayHeader->mNumMilestoneRecords * 0x14;

    AlignPointer(src, 16);
    LoadSpeedTraps((GSpeedTrap *)src, gameplayHeader->mNumSpeedTrapRecords);
    src += gameplayHeader->mNumSpeedTrapRecords * 0x14;

    unsigned int spotBytes = (gameplayHeader->mNumHidingSpotFlags + 7) >> 3;
    AlignPointer(src, 16);
    bMemCpy(mHidingSpotFound, src, spotBytes);
    src += spotBytes;

    AlignPointer(src, 16);
    unsigned int binBytesRead = GRaceDatabase::Get().DeserializeBins(src);
    src += binBytesRead;

    AlignPointer(src, 16);
    LoadSMSInfo((int *)src, gameplayHeader->mNumPendingSMS);
    src += gameplayHeader->mNumPendingSMS << 2;

    unsigned int respawnMarker;
    AlignPointer(src, 16);
    bMemCpy(&respawnMarker, src, 4);

    src += 4;
    AlignPointer(src, 16);

    Attrib::Gen::gameplay testInstance(respawnMarker, 0, NULL);
    if (testInstance.IsValid()) {
        Get().SetFreeRoamStartMarker(respawnMarker);
    }

    bMemCpy(&respawnMarker, src, 4);

    Attrib::Gen::gameplay testInstance2(respawnMarker, 0, NULL);
    if (testInstance2.IsValid()) {
        Get().SetFreeRoamFromSafeHouseStartMarker(respawnMarker);
    }

    src += 4;
    AlignPointer(src, 16);
    return true;
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

    mFreeRoamStartMarker = 0;
    mOverrideFreeRoamStartMarker = 0;

    mFreeRoamFromSafeHouseStartMarker = 0;
    mStartFreeRoamFromSafeHouse = false;

    mPendingSMS.clear();
}

void GManager::AllocateStreamingBuffers() {
    GRaceDatabase &db = GRaceDatabase::Get();
    GVault *largestBinVault = NULL;
    GVault *largestRaceVault = NULL;

    for (unsigned int onBin = 0; onBin < db.GetBinCount(); onBin++) {
        GVault *binVault = db.GetBin(onBin)->GetChildVault();

        if (binVault && !binVault->IsResident()) {
            if (!largestBinVault || binVault->GetFootprint() > largestBinVault->GetFootprint()) {
                largestBinVault = binVault;
            }
        }
    }

    for (unsigned int onRace = 0; onRace < db.GetRaceCount(); onRace++) {
        GVault *raceVault = db.GetRaceParameters(onRace)->GetChildVault();

        if (raceVault && !raceVault->IsResident()) {
            if (!largestRaceVault || raceVault->GetFootprint() > largestRaceVault->GetFootprint()) {
                largestRaceVault = raceVault;
            }
        }
    }

    if (largestBinVault) {
        mBinSlotSize = largestBinVault->GetFootprint();
    } else {
        mBinSlotSize = 0;
    }

    if (largestRaceVault) {
        mRaceSlotSize = largestRaceVault->GetFootprint();
    } else {
        mRaceSlotSize = 0;
    }

    mStreamedBinSlots = new ("Gameplay streamed bins", 0) unsigned char[mBinSlotSize];
    mStreamedRaceSlots = new ("Gameplay streamed races", 0) unsigned char[mRaceSlotSize];

    unsigned int loadDataSize = 0;

    for (unsigned int onVault = 0; onVault < mVaultCount; onVault++) {
        GVault &vault = mVaults[onVault];

        if (vault.IsTransient()) {
            loadDataSize = UMath::Max(vault.GetLoadDataSize(), loadDataSize);
        }
    }

    mTempLoadData = new ("Gameplay vault loading buffer", 0) unsigned char[loadDataSize];
}

void GManager::AllocateInstanceMap() {
    mMaxObjects = 0;

    unsigned int mostVaultObjects = 0;
    unsigned int mostTransientVaults = 2;

    for (unsigned int onVault = 0; onVault < mVaultCount; onVault++) {
        GVault &vault = mVaults[onVault];

        if (vault.IsResident()) {
            mMaxObjects += vault.GetObjectCount();
        } else {
            mostVaultObjects = std::max(mostVaultObjects, vault.GetObjectCount());
        }
    }

    mMaxObjects += mostVaultObjects * mostTransientVaults;

    unsigned int minHashEntries = mMaxObjects + mMaxObjects / 2;
    // EL MECANISMO (.greg): REG_ALLOC_ORDER (rs6000.h:932) da los preservados en
    // el orden 31, 30, 29..., asi que el PRIMER allocno que pide uno se lleva r31.
    // `vault` (pri 34285, refs pesadas por estar en el bucle) va antes que `this`
    // (pri 10322) y arrampla con r31. En el objetivo el primero es `this`, o sea
    // que alli `allocno_compare` los ordena al reves. Medido y NEGATIVO: declarar
    // minHashEntries/tableSize arriba como en el DWARF -> las mismas 24 filas.
    unsigned int tableSize = 256;

    while (tableSize < minHashEntries) {
        tableSize <<= 1;
    }

    mInstanceHashTableSize = tableSize;
    mInstanceHashTableMask = tableSize - 1;

    tableSize <<= 3;

    mKeyToInstanceMap = (HashEntry *)bMalloc(tableSize, "Gameplay instance hash table", 0, GetVirtualMemoryAllocParams());

    bMemSet(mKeyToInstanceMap, 0, tableSize);
}

void GManager::ReleaseInstanceMap() {
    if (mKeyToInstanceMap) {
        bFree(mKeyToInstanceMap);
        mKeyToInstanceMap = NULL;
    }

    mInstanceHashTableSize = 0;
    mInstanceHashTableMask = 0;
}

void GManager::UnloadCoreVault() {
    FindVault("gpcore")->Unload();
}

void GManager::UnloadTransientVaults() {
    for (unsigned int i = 0; i < mVaultCount; i++) {
        GVault *vault = &mVaults[i];

        if (vault->IsTransient()) {
            vault->Unload();
        }
    }

    bCloseMemoryPool(mTransientPoolNumber);
    bFree(mTransientPoolMemory);

    mTransientPoolMemory = NULL;
    mTransientPoolNumber = 0;
}

void GManager::ReleaseStreamingBuffers() {
    if (mStreamedBinSlots) {
        delete[] mStreamedBinSlots;
        mStreamedBinSlots = NULL;
    }

    if (mStreamedRaceSlots) {
        delete[] mStreamedRaceSlots;
        mStreamedRaceSlots = NULL;
    }

    if (mTempLoadData) {
        delete[] mTempLoadData;
        mTempLoadData = NULL;
    }
}

void GManager::DestroyVaults() {
    if (mVaults) {
        for (unsigned int i = 0; i < mVaultCount; i++) {
            GVault *vault = &mVaults[i];
            vault->~GVault();
        }

        mVaultCount = 0;

        bFree(mVaults);
        mVaults = NULL;
    }

    if (mVaultNameStrings) {
        delete[] mVaultNameStrings;
        mVaultNameStrings = NULL;
    }
}

void GManager::BeginGameplay() {
    for (unsigned int onVault = 0; onVault < mVaultCount; onVault++) {
        GVault &vault = mVaults[onVault];

        if (vault.IsLoaded()) {
            vault.CreateGameplayObjects();
        }
    }

    GRaceCustom *startupRace = GRaceDatabase::Get().GetStartupRace();

    if (startupRace != NULL) {
        startupRace->CreateRaceActivity();
    }

    ConnectRuntimeInstances();

    StartActivities();

    WCollisionAssets::Get().AddPackLoadCallback(NotifyCollisionPackLoaded);

    mPursuitBreakerIconsShown = false;
    mHidingSpotIconsShown = false;
    mEventIconsShown = false;
    mMenuGateIconsShown = false;
    mSpeedTrapIconsShown = false;
    mSpeedTrapRaceIconsShown = false;

    RefreshEngageTriggerIcons();
    RefreshSpeedTrapIcons();
    SpawnAllLoadedSectionIcons();

    if (startupRace == NULL) {
        GRaceStatus::Get().EnableBinBarriers();
    }

    MEnteringGameplay().Post(UCrc32(0x20D60DBF));

    mStartFreeRoamFromSafeHouse = false;
    mInGameplay = true;
}

void GManager::EndGameplay() {

    UnspawnAllCharacters();
    ClearStockCars();

    for (unsigned int onVault = 0; onVault < mVaultCount; onVault++) {

        GVault &vault = mVaults[onVault];

        if (vault.IsLoaded()) {
            vault.DestroyGameplayObjects();
        }
    }

    UnspawnAllIcons();

    ClearAllSessionData();

    WCollisionAssets::Get().RemovePackLoadCallback(NotifyCollisionPackLoaded);

    GRaceDatabase::Get().ClearStartupRace();

    mWorstHashCollision = 0;

    mOverrideFreeRoamStartMarker = 0;

    mInGameplay = false;

    ApplyTimeOfDayTickOver();

    SetOverRideRainIntensity(0.0f);
}

void GManager::PreBeginGameplay() {
    if (mRestartEventHash) {
        GRaceParameters *params = GRaceDatabase::Get().GetRaceFromHash(mRestartEventHash);
        GRaceCustom *custom = GRaceDatabase::Get().AllocCustomRace(params);
        GRaceDatabase::Get().SetStartupRace(custom, GRace::kRaceContext_Career);
        GRaceDatabase::Get().FreeCustomRace(custom);
        mRestartEventHash = 0;
    }
}

bool GManager::GetInGameplay() const {
    return mInGameplay;
}

void GManager::Update(float dT) {
    GRaceStatus::Get().Update(dT);

    UnspawnUselessCharacters();
    ServicePendingCharacters();
    UpdatePursuit();
    UpdateTimers(dT);
    UpdatePendingSMS();
    UpdateTriggerAvailability();
    UpdateIconVisibility();
}

void GManager::GetPlayerPursuitInterfaces(IPursuit *&pursuit, IPerpetrator *&perp) {
    IPlayer *player = IPlayer::First(PLAYER_LOCAL);
    ISimable *simable;

    if (player) {
        simable = player->GetSimable();
    } else {
        simable = NULL;
    }

    IVehicleAI *ai = NULL;
    if (simable) {
        simable->QueryInterface(&ai);
    }

    if (ai) {
        pursuit = ai->GetPursuit();
    } else {
        pursuit = NULL;
    }

    IPerpetrator *p = NULL;
    if (simable) {
        simable->QueryInterface(&p);
    }
    perp = p;
}

void GManager::UpdatePursuit() {
    bool roaming = GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming;

    IPursuit *pursuit = NULL;
    IPerpetrator *perp = NULL;

    GetPlayerPursuitInterfaces(pursuit, perp);

    bool inPursuit = false;
    bool inCooldown = false;

    if (pursuit) {
        int costToStateInPursuit = pursuit->CalcTotalCostToState();
        Get().TrackValue("cost_to_state_in_pursuit", costToStateInPursuit);

        inPursuit = true;
        inCooldown = pursuit->GetPursuitStatus() == PS_COOL_DOWN;
    }

#ifndef EA_BUILD_A124
    GRaceStatus::Get().SetPlayerPursuitInCooldown(inCooldown);
#endif

    if (perp) {
        int costToState = perp->GetCostToState();
        Get().TrackValue("cost_to_state", costToState);

        int bounty = perp->GetPendingRepPointsNormal() + perp->GetPendingRepPointsFromCopDestruction();
        Get().TrackValue("bounty_in_pursuit", bounty);
    }

    bool challengeSeries = false;
    if (GRaceStatus::Get().GetRaceParameters()) {
        if (GRaceStatus::Get().GetRaceParameters()->GetIsChallengeSeriesRace()) {
            challengeSeries = true;
        }
    }

    mHidingSpotIconsShown = (roaming || challengeSeries) && inPursuit && inCooldown;
    mPursuitBreakerIconsShown = (roaming || challengeSeries || GRaceStatus::IsFinalEpicPursuit()) && inPursuit && !inCooldown;

    if (TWEAK_ShowGameplayMilestoneValues) {
        static volatile int xLeft = -130;
        static volatile int yTop = -230;
        static volatile int xWidth = 220;
        static volatile int line = 16;
        static const char *milestoneNames[9] = {
            "cops_damaged",
            "cops_destroyed_in_pursuit",
            "cost_to_state_in_pursuit",
            "pursuit_evasion_time",
            "pursuit_length",
            "roadblocks_dodged",
            "tire_spikes_dodged",
            "total_infractions",
            "bounty_in_pursuit",
        };

        int x = xLeft;
        int y = yTop;
        for (int i = 0; i <= 8; i++) {
            GetValue(milestoneNames[i]);
            int l = line;
        }
    }
}

void GManager::UpdateTriggerAvailability() {
    bool roaming = GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming;
    bool racing = GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing;

    bool inPursuit = false;
    bool inCooldown = false;

    IPursuit *pursuit = NULL;
    IPerpetrator *perp = NULL;

    GetPlayerPursuitInterfaces(pursuit, perp);

    if (pursuit != NULL) {
        inPursuit = true;
        inCooldown = pursuit->GetPursuitStatus() == PS_COOL_DOWN;
    }

    mAllowMenuGates = roaming && !inPursuit;
    mAllowEngageEvents = roaming && !inPursuit;
    mAllowEngageSafehouse = roaming && (!inPursuit || inCooldown);
}

void GManager::UpdateIconVisibility() {
    bool hideAll;
    bool showAll;
    bool iconTypeVisible[17];

    mEventIconsShown = mAllowEngageEvents;
    mMenuGateIconsShown = mAllowMenuGates;
    mSpeedTrapIconsShown = mAllowEngageEvents;

    mSpeedTrapRaceIconsShown = GRaceStatus::IsSpeedTrapRace();

    hideAll = false;
    showAll = false;

    if (INIS::Get() != NULL) {
        hideAll = true;
    }
    if (TWEAK_ShowAllGameplayIcons != 0) {
        showAll = true;
    }

    for (unsigned int onFlag = 0; onFlag <= 16; onFlag++) {
        iconTypeVisible[onFlag] = showAll;
    }

    if (!showAll && !hideAll) {
        iconTypeVisible[GIcon::kType_RaceSprint] = mEventIconsShown;
        iconTypeVisible[GIcon::kType_RaceCircuit] = mEventIconsShown;
        iconTypeVisible[GIcon::kType_RaceDrag] = mEventIconsShown;
        iconTypeVisible[GIcon::kType_RaceKnockout] = mEventIconsShown;
        iconTypeVisible[GIcon::kType_RaceTollbooth] = mEventIconsShown;
        iconTypeVisible[GIcon::kType_RaceSpeedtrap] = mEventIconsShown;
        iconTypeVisible[GIcon::kType_RaceRival] = mEventIconsShown;
        iconTypeVisible[GIcon::kType_GateSafehouse] = mAllowEngageSafehouse;
        iconTypeVisible[GIcon::kType_GateCarLot] = mMenuGateIconsShown;
        iconTypeVisible[GIcon::kType_GateCustomShop] = mMenuGateIconsShown;
        iconTypeVisible[GIcon::kType_HidingSpot] = mHidingSpotIconsShown;
        iconTypeVisible[GIcon::kType_PursuitBreaker] = mPursuitBreakerIconsShown;
        iconTypeVisible[GIcon::kType_SpeedTrap] = mSpeedTrapIconsShown;
        iconTypeVisible[GIcon::kType_SpeedTrapInRace] = mSpeedTrapRaceIconsShown;
        iconTypeVisible[GIcon::kType_AreaUnlock] = true;
    }

    mNumVisibleIcons = 0;

    for (unsigned int onIcon = 0; onIcon < mNumIcons; onIcon++) {
        GIcon *icon = mIcons[onIcon];
        bool enabled = icon->GetIsEnabled();
        bool iconVisible = icon->GetVisibleInWorld();
        bool typeVisible = iconTypeVisible[icon->GetType()];
        bool shouldBeVisible = iconVisible && typeVisible;

        if (!shouldBeVisible) {
            if (enabled) {
                icon->Disable();
            }
        } else {
            if (!enabled) {
                icon->Enable();
            }

            std::swap(mIcons[onIcon], mIcons[mNumVisibleIcons]);
            mNumVisibleIcons++;
        }
    }
}

void GManager::NotifyWorldService() {
    if (mWarping) {
        if (TheTrackStreamer.IsLoadingInProgress() == true) {
            return;
        }

        {
            if (mWarpTargetMarker) {
                GMarker *marker = static_cast<GMarker *>(FindInstance(mWarpTargetMarker));
                const UMath::Vector3 &markerPos = marker->GetPosition();
                const UMath::Vector3 &markerDir = marker->GetDirection();
                ISimable *player = IPlayer::First(PLAYER_LOCAL)->GetSimable();
                IVehicle *vehicle;

                if (player->QueryInterface(&vehicle)) {
                    vehicle->SetVehicleOnGround(markerPos, markerDir);
                    vehicle->Activate();
                }

                mWarpTargetMarker = 0;

                if (mWarpStartPursuit) {
                    World_RestoreProps();
                    GPS_Disengage();
                    ITrafficMgr::Get()->FlushAllTraffic(true);
                    ICopMgr::Get()->ResetCopsForRestart(true);
                    ICopMgr::Get()->LockoutCops(false);
                    ICopMgr::Get()->PursueAtHeatLevel(1);
                }
            }

            if (mWarpStartPursuit) {
                if (ICopMgr::Get()->PlayerPursuitHasCop()) {
                    mWarping = false;
                    mWarpStartPursuit = false;
                } else {
                    ICopMgr::Get()->PursueAtHeatLevel(1);
                }
            } else {
                mWarping = false;
            }

            if (!mWarping) {
                new EFadeScreenOff(0x161a918);
            }
        }
    }
}

void GManager::NotifyCollisionPackLoaded(int sectionNumber, bool loaded) {
    if (Exists()) {
        if (loaded) {
            Get().SpawnSectionIcons(sectionNumber);
        } else {
            Get().UnspawnSectionIcons(sectionNumber);
        }
    }
}

void GManager::AttachCharacter(GCharacter *character) {
    GCharacterList::iterator it = std::find(mActiveCharacters.begin(), mActiveCharacters.end(), character);

    if (it == mActiveCharacters.end()) {
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
    while (mActiveCharacters.size()) {
        mActiveCharacters[0]->Unspawn();
    }

    mActiveCharacters.clear();
}

void GManager::TrackValue(const char *valueName, float value) {
    MilestoneInfoMap::iterator it = mMilestoneTypeInfo.find(Attrib::StringToKey(valueName));

    MilestoneTypeInfo &info = it->second;

    info.mLastKnownValue = value;

    bool newRecord = false;

    if (info.mBestValue == -1.0f) {
        newRecord = true;
    } else if (info.mFlags & GMilestone::kFlag_BiggerIsBetter) {
        if (value > info.mBestValue) {
            newRecord = true;
        }
    } else {
        if (value < info.mBestValue) {
            newRecord = true;
        }
    }

    if (newRecord) {
        info.mBestValue = value;
        MNotifyMilestoneProgress(valueName, value).Post(UCrc32(0x20d60dbf));

        if (GRaceStatus::Exists() && GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming) {
            GMilestone *milestone = GetFirstMilestone(true, 0);

            while (milestone) {
                if (milestone->GetTypeKey() == info.mTypeKey) {
                    milestone->NotifyProgress(value);
                }

                milestone = GetNextMilestone(milestone, true, 0);
            }
        }
    }
}

void GManager::IncValue(const char *valueName) {
    MilestoneInfoMap::iterator it = mMilestoneTypeInfo.find(Attrib::StringToKey(valueName));

    if (it->second.mLastKnownValue == -1.0f) {
        TrackValue(valueName, 1.0f);
    } else {
        TrackValue(valueName, it->second.mLastKnownValue + 1.0f);
    }
}

float GManager::GetValue(const char *valueName) {
    MilestoneInfoMap::iterator it = mMilestoneTypeInfo.find(Attrib::StringToKey(valueName));

    return it->second.mLastKnownValue;
}

float GManager::GetValue(unsigned int valueKey) {
    MilestoneInfoMap::iterator it = mMilestoneTypeInfo.find(valueKey);

    return it->second.mLastKnownValue;
}

float GManager::GetBestValue(Attrib::Key valueKey) {
    MilestoneInfoMap::iterator it = mMilestoneTypeInfo.find(valueKey);

    return it->second.mBestValue;
}

bool GManager::GetIsBiggerValueBetter(unsigned int valueKey) {
    MilestoneInfoMap::iterator it = mMilestoneTypeInfo.find(valueKey);

    return it->second.mFlags & GMilestone::kFlag_BiggerIsBetter;
}

eVehicleCacheResult GManager::OnQueryVehicleCache(const IVehicle *removethis, const IVehicleCache *whosasking) const {
    for (StockCarMap::iterator it = const_cast<StockCarMap &>(mStockCars).begin(); it != const_cast<StockCarMap &>(mStockCars).end(); ++it) {
        ISimable *simable = it->second;
        IVehicle *vehicle = NULL;

        if (simable->QueryInterface(&vehicle) && vehicle == removethis) {
            if (UTL::COM::ComparePtr(whosasking, INIS::Get())) {
                const_cast<StockCarMap &>(mStockCars).erase(it);
                return VCR_DONTCARE;
            }
            return VCR_WANT;
        }
    }

    for (GCharacterList::const_iterator it = mActiveCharacters.begin(); it != mActiveCharacters.end(); ++it) {
        GCharacter *character = *it;
        IVehicle *spawned = character->GetSpawnedVehicle();

        if (spawned != removethis) {
            continue;
        }

        {
            if (character->HasStockCar()) {
                return VCR_WANT;
            }

            if (UTL::COM::ComparePtr(whosasking, ITrafficMgr::Get())) {
                return VCR_WANT;
            }

            if (spawned->GetOffscreenTime() == 0.0f) {
                return VCR_WANT;
            }

            GCharacter *lowestPrio = NULL;

            if (mActiveCharacters.size() != 0) {
                lowestPrio = *mActiveCharacters.begin();
            }

            return character == lowestPrio ? VCR_DONTCARE : VCR_WANT;
        }
    }

    return VCR_DONTCARE;
}

void GManager::OnRemovedVehicleCache(IVehicle *ivehicle) {
    for (StockCarMap::iterator it = mStockCars.begin(); it != mStockCars.end(); ++it) {
        ISimable *stockCar = it->second;

        if (UTL::COM::ComparePtr(stockCar, ivehicle)) {
            mStockCars.erase(it);
            break;
        }
    }
}

void GManager::RegisterInstance(GRuntimeInstance *instance) {
    unsigned int key32 = ReduceCollectionKey(instance->GetCollection());

    unsigned int index = key32 & mInstanceHashTableMask;

    for (unsigned int probe = 0; probe < mInstanceHashTableSize; probe++) {
        HashEntry *entry = &mKeyToInstanceMap[index];

        if (entry->mInstance == NULL) {
            mWorstHashCollision = std::max(mWorstHashCollision, probe);

            entry->mKey32 = key32;
            entry->mInstance = instance;
            return;
        }

        index = (index + 1) & mInstanceHashTableMask;
    }
}

void GManager::UnregisterInstance(GRuntimeInstance *instance) {
    unsigned int key32 = ReduceCollectionKey(instance->GetCollection());

    unsigned int index = key32 & mInstanceHashTableMask;

    for (unsigned int probe = 0; probe <= mWorstHashCollision; probe++) {
        HashEntry *entry = &mKeyToInstanceMap[index];

        if (entry->mKey32 == key32) {
            entry->mKey32 = 0;
            entry->mInstance = NULL;
            return;
        }

        index = (index + 1) & mInstanceHashTableMask;
    }
}

GRuntimeInstance *GManager::FindInstance(Attrib::Key key) const {
    unsigned int key32 = ReduceCollectionKey(key);
    unsigned int index = key32 & mInstanceHashTableMask;

    for (unsigned int probe = 0; probe <= mWorstHashCollision; probe++) {
        HashEntry *entry = &mKeyToInstanceMap[index];

        if (entry->mKey32 == key32) {
            return entry->mInstance;
        }

        index = (index + 1) & mInstanceHashTableMask;
    }

    return NULL;
}

void GManager::ConnectRuntimeInstances() {
    for (unsigned int i = 0; i < mInstanceHashTableSize; i++) {
        GRuntimeInstance *instance = mKeyToInstanceMap[i].mInstance;

        if (instance) {
            instance->ResetConnections();

            ConnectInstanceReferences(instance, *instance);

            Attrib::Key parentKey = instance->GetParent();

            while (parentKey) {
                Attrib::Gen::gameplay parent(parentKey, 0, NULL);

                ConnectInstanceReferences(instance, parent);
                parentKey = parent.GetParent();
            }

            ConnectChildren(instance);

            instance->LockConnections();
        }
    }
}

void GManager::ConnectInstanceReferences(GRuntimeInstance *runtimeInstance, const Attrib::Gen::gameplay &collection) {
    for (Attrib::AttributeIterator iter = collection.Iterator(); iter.Valid(); iter.Advance()) {
        unsigned int attribKey = iter.GetKey();

        Attrib::Attribute attribute = collection.Get(attribKey);

        if (attribKey == Attrib::Hash::gameplay::Children) {
            continue;
        }

        if (attribute.IsValid()) {
            const Attrib::TypeDesc &typeDesc = Attrib::Database::Get().GetTypeDesc(attribute.GetType());

            if (bStrCmp(typeDesc.GetName(), "GCollectionKey") == 0) {
                const Attrib::Definition *definition = mGameplayClass->GetDefinition(attribute.GetKey());
                int count;

                if (definition->IsArray()) {
                    count = attribute.GetLength();
                } else {
                    count = 1;
                }

                for (int onIndex = 0; onIndex < count; onIndex++) {
                    const GCollectionKey &refSpec = attribute.Get<GCollectionKey>(onIndex);
                    GRuntimeInstance *targetInstance = FindInstance(refSpec.GetCollectionKey());

                    if (targetInstance) {
                        runtimeInstance->ConnectToInstance(attribKey, onIndex, targetInstance);
                    }
                }
            }
        }
    }
}

void GManager::ConnectChildren(GRuntimeInstance *runtimeInstance) {

    for (unsigned int onChild = 0; onChild < runtimeInstance->Num_Children(); onChild++) {

        const GCollectionKey &childSpec = runtimeInstance->Children(onChild);
        GRuntimeInstance *childInstance = FindInstance(childSpec.GetCollectionKey());

        if (childInstance != NULL) {

            const char *childName = childInstance->CollectionName();

            unsigned int strippedNameKey = GetStrippedNameKey(childName);

            runtimeInstance->ConnectToInstance(strippedNameKey, 0, childInstance);
        }
    }
}

unsigned int GManager::GetStrippedNameKey(const char *name) {
    const char *lastSlash = (const char *)(bStrLen(name) + (unsigned int)name);

    while (--lastSlash >= name) {
        if (*lastSlash == '/' || *lastSlash == '\\') {
            name = lastSlash + 1;
            break;
        }
    }

    return Attrib::StringToKey(name);
}

void GManager::ResetMilestoneTrackingInfo() {
    mMilestoneTypeInfo.clear();

    Attrib::Key key = mMilestoneClass->GetFirstCollection();

    while (key != 0) {
        Attrib::Gen::milestonetypes type(key, 0, NULL);
        MilestoneTypeInfo info;

        info.mTypeKey = key;
        info.mBestValue = -1.0f;
        info.mLastKnownValue = -1.0f;
        info.mFlags = 0;

        if (type.MilestoneType() != 2) {
            info.mFlags = GMilestone::kFlag_BiggerIsBetter;
        }

        if (type.ResetWhenPursuitStarts()) {
            info.mFlags |= GMilestone::kFlag_CompletionFaked;
        }

        mMilestoneTypeInfo[key] = info;
        key = mMilestoneClass->GetNextCollection(key);
    }
}

void GManager::LoadMilestoneInfo(MilestoneTypeInfo *savedInfo, unsigned int count) {
    ResetMilestoneTrackingInfo();

    for (unsigned int i = 0; i < count; i++) {
        MilestoneTypeInfo *saved = &savedInfo[i];

        MilestoneInfoMap::iterator it = mMilestoneTypeInfo.find(saved->mTypeKey);

        if (it != mMilestoneTypeInfo.end()) {
            MilestoneTypeInfo &info = it->second;

            info.mBestValue = saved->mBestValue;
            info.mLastKnownValue = saved->mLastKnownValue;
        }
    }
}

unsigned int GManager::SaveMilestoneInfo(MilestoneTypeInfo *dest) {
    for (MilestoneInfoMap::iterator it = mMilestoneTypeInfo.begin(); it != mMilestoneTypeInfo.end(); ++it) {
        MilestoneTypeInfo &info = it->second;

        *dest++ = info;
    }

    return mMilestoneTypeInfo.size();
}

void GManager::StartActivities() {
    bool startCareerActivities = SkipFE == 0;

    bool previousBinRace = false;

    GRaceCustom *startupRace = GRaceDatabase::Get().GetStartupRace();
    GRaceBin *currentBin = GRaceStatus::Get().GetRaceBin();

    if (startupRace) {
        GActivity *raceActivity = startupRace->GetRaceActivity();

        raceActivity->Run();

        Context raceContext = GRaceDatabase::Get().GetStartupRaceContext();

        GRaceStatus::Get().SetRaceContext((GRace::Context)raceContext);

        if (raceContext != kRaceContext_Career) {
            startCareerActivities = false;
        }

        if (currentBin) {
            if (startupRace->GetParentVault() != currentBin->GetChildVault()) {
                previousBinRace = true;
            }
        }

#ifndef EA_BUILD_A124
        if (previousBinRace && raceContext == kRaceContext_Career) {
            GRaceStatus::Get().SetRefreshBinAfterRace(true);
        }
#endif
    }

    if (startCareerActivities) {
        bool freeRoam = startupRace == NULL;

        StartWorldActivities(freeRoam);

        if (!previousBinRace) {
            StartBinActivity(currentBin);
        }
    }
}

void GManager::StartWorldActivities(bool startFreeRoamOnly) {
    GObjectIterator<GActivity> activityIter(-1);

    while (activityIter.IsValid()) {
        GActivity *activity = activityIter.GetInstance();
        bool autoStart = activity->AutoStart();

        if (activity->FreeRoamOnly() && !startFreeRoamOnly) {
            autoStart = false;
        }
        if (autoStart) {
            activity->Run();
        }

        activityIter.Advance();
    }
}

void GManager::StartBinActivity(GRaceBin *raceBin) {
    if (raceBin) {
        GActivity *activity = (GActivity *)Get().FindInstance(raceBin->GetCollectionKey());

        if (!activity->GetIsRunning()) {
            activity->Run();
        }
    }
}

void GManager::SuspendAllBinActivities() {
    for (unsigned int i = 0; i < GRaceDatabase::Get().GetBinCount(); i++) {
        GRaceBin *bin = GRaceDatabase::Get().GetBin(i);

        if (bin) {
            GActivity *activity = (GActivity *)Get().FindInstance(bin->GetCollectionKey());

            if (activity && activity->GetIsRunning()) {
                activity->SerializeVars(false);
                activity->Suspend();
            }
        }
    }
}

void GManager::StartRaceFromInGame(unsigned int raceHash) {
    if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming) {
        GRaceParameters *params = GRaceDatabase::Get().GetRaceFromHash(raceHash);

        if (params) {
            GRaceBin *bin = GRaceStatus::Get().GetRaceBin();

            if (bin) {
                if (params->GetParentVault() != bin->GetChildVault()) {
                    SuspendAllBinActivities();

#ifndef EA_BUILD_A124
                    GRaceStatus::Get().SetRefreshBinAfterRace(true);
#endif
                }
            }

            params->BlockUntilLoaded();

            GActivity *activity = params->GetActivity();

            activity->Reset();
            activity->Run();
        }
    }
}

bool GManager::CalcMapCoordsForMarker(unsigned int markerKey, bVector2 &outPos, float &outRotDeg) {
    Attrib::Gen::gameplay marker(markerKey, 0, NULL);

    if (!marker.IsValid()) {
        return false;
    }

    const UMath::Vector3 &pos = marker.Position();
    TrackInfo *trackInfo = TrackInfo::GetTrackInfo(2000);
    bVector2 worldPos;
    float worldX = pos.x;
    float worldY = pos.y;
    worldPos.x = worldX;
    worldPos.y = worldY;
    Minimap::ConvertPos(worldPos, outPos, trackInfo);

    UMath::Matrix4 rotMat = UMath::Matrix4::kIdentity;
    UMath::Vector3 dir = {0.0f, 0.0f, 1.0f};

    UMath::Init(rotMat, 1.0f, 1.0f, 1.0f);
    MATRIX4_multyrot(&rotMat, -marker.Rotation() / 360.0f, &rotMat);
    UMath::Rotate(dir, rotMat, dir);

    outRotDeg = static_cast<unsigned int>(bATan(-dir.x, dir.z)) * 0.0054931640625f;
    return true;
}

bool GManager::WarpToMarker(unsigned int markerKey, bool startPursuit) {
    if (!GRaceStatus::Exists()) {
        return false;
    }

    if (GRaceStatus::Get().GetPlayMode() != GRaceStatus::kPlayMode_Roaming) {
        return false;
    }

    if (mWarping) {
        return false;
    }

    GMarker *marker = (GMarker *) FindInstance(markerKey);

    if (marker == NULL) {
        return false;
    }

    const UMath::Vector3 &markerPos = marker->GetPosition();
    const UMath::Vector3 &markerDir = marker->GetDirection();

    new EFadeScreenOn(false);

    TheTrackStreamer.EnableZoneSwitching();

    Sim::SetStream(markerPos, false);

    IPlayer *player = UTL::Collections::ListableSet<IPlayer, 8, ePlayerList, 3>::First(PLAYER_LOCAL);
    IVehicle *vehicle;

    if (player->GetSimable()->QueryInterface(&vehicle)) {
        vehicle->SetVehicleOnGround(markerPos, markerDir);
        vehicle->Deactivate();
    }

    mWarping = true;
    mWarpTargetMarker = markerKey;
    mWarpStartPursuit = startPursuit;

    return true;
}

void GManager::RefreshWorldParticleEffects() {

    GObjectIterator<GTrigger> triggerIter(-1);

    while (triggerIter.IsValid()) {

        triggerIter.GetInstance()->RefreshParticleEffects();
        triggerIter.Advance();
    }

    for (unsigned int onIcon = 0; onIcon < mNumIcons; onIcon++) {
        mIcons[onIcon]->RefreshEffects();
    }
}

unsigned int GManager::GetNumMilestones() {
    return mNumMilestones;
}

GMilestone *GManager::GetMilestone(unsigned int index) {
    return &mMilestones[index];
}

GMilestone *GManager::GetFirstMilestone(bool availOnly, unsigned int binNumber) {
    return GetNextMilestone(mMilestones - 1, availOnly, binNumber);
}

GMilestone *GManager::GetNextMilestone(GMilestone *current, bool availOnly, unsigned int binNumber) {
    for (GMilestone *milestone = current + 1; milestone < mMilestones + mNumMilestones; milestone++) {
        if (availOnly && !milestone->GetIsAvailable()) {
            continue;
        }

        if (binNumber && milestone->GetBinNumber() != binNumber) {
            continue;
        }

        return milestone;
    }

    return NULL;
}

void GManager::EnableBinMilestones(unsigned int binNumber) {
    GMilestone *milestone = GetFirstMilestone(false, binNumber);

    while (milestone) {
        milestone->Unlock();
        milestone = GetNextMilestone(milestone, false, binNumber);
    }
}

void GManager::NotifyPursuitStarted() {
    for (MilestoneInfoMap::iterator it = mMilestoneTypeInfo.begin(); it != mMilestoneTypeInfo.end(); ++it) {
        MilestoneTypeInfo &info = it->second;

        if (info.mFlags & GMilestone::kFlag_CompletionFaked) {
            info.mBestValue = -1.0f;
            info.mLastKnownValue = -1.0f;
        }
    }
}

void GManager::NotifyPursuitEnded(bool evaded) {
    GMilestone *milestone = GetFirstMilestone(true, 0);

    while (milestone) {
        milestone->NotifyPursuitOver(evaded);
        milestone = GetNextMilestone(milestone, true, 0);
    }

    if (evaded && GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming) {
        new EAutoSave();
    }
}

unsigned int GManager::GetNumSpeedTraps() {
    return mNumSpeedTraps;
}

GSpeedTrap *GManager::GetSpeedTrap(unsigned int index) {
    return &mSpeedTraps[index];
}

GSpeedTrap *GManager::GetFirstSpeedTrap(bool activeOnly, unsigned int binNumber) {
    return GetNextSpeedTrap(mSpeedTraps - 1, activeOnly, binNumber);
}

GSpeedTrap *GManager::GetNextSpeedTrap(GSpeedTrap *current, bool activeOnly, unsigned int binNumber) {
    for (GSpeedTrap *trap = current + 1; trap < mSpeedTraps + mNumSpeedTraps; trap++) {
        if (activeOnly && !trap->GetIsActive()) {
            continue;
        }

        if (binNumber != 0 && trap->GetBinNumber() != binNumber) {
            continue;
        }

        return trap;
    }

    return NULL;
}

void GManager::EnableBinSpeedTraps(unsigned int binNumber) {
    GSpeedTrap *speedTrap = GetFirstSpeedTrap(false, binNumber);

    while (speedTrap) {
        speedTrap->Unlock();
        speedTrap->Activate();

        speedTrap = GetNextSpeedTrap(speedTrap, false, binNumber);
    }
}

void GManager::RefreshSpeedTrapIcons() {
    GSpeedTrap *speedTrap = GetFirstSpeedTrap(false, 0);

    while (speedTrap) {
        GTrigger *trigger = speedTrap->GetTrapTrigger();

        if (trigger) {
            if (speedTrap->GetIsActive() && !speedTrap->GetIsCompleted()) {
                trigger->ShowIcon();
            } else {
                trigger->HideIcon();
            }
        }

        speedTrap = GetNextSpeedTrap(speedTrap, false, 0);
    }
}

void GManager::GatherInstanceKeys(Attrib::Gen::gameplay &collection, AttribKeyList &list, unsigned int templateKey) {
    unsigned int parentKey = collection.GetParent();

    if (parentKey != 0) {
        do {
            if (parentKey == templateKey) {
                list.push_back(collection.GetCollection());
                break;
            }

            Attrib::Gen::gameplay parentObj(parentKey, 0, NULL);
            parentKey = parentObj.GetParent();
        } while (parentKey != 0);
    }

    for (unsigned int onChild = 0; onChild < collection.Num_Children(); onChild++) {
        const GCollectionKey &childSpec = collection.Children(onChild);
        Attrib::Gen::gameplay child(childSpec.GetCollectionKey(), 0, NULL);

        GatherInstanceKeys(child, list, templateKey);
    }
}

void GManager::FindBountySpawnPoints() {
    Attrib::Gen::gameplay jumpRoot(0x3D48E303, 0, NULL);
    AttribKeyList keys;

    GatherInstanceKeys(jumpRoot, keys, Attrib::Hash::gameplay::key_marker);

    mNumBountySpawnPoints = 0;

    for (AttribKeyList::iterator iter = keys.begin(); iter != keys.end(); ++iter) {
        mBountySpawnPoint[mNumBountySpawnPoints++] = *iter;

        if (mNumBountySpawnPoints >= 20) {
            break;
        }
    }
}

unsigned int GManager::GetNumBountySpawnMarkers() const {
    return mNumBountySpawnPoints;
}

unsigned int GManager::GetBountySpawnMarker(unsigned int index) const {
    if (index < mNumBountySpawnPoints) {
        return mBountySpawnPoint[index];
    }

    return 0;
}

int GManager::GetBountySpawnMarkerTag(unsigned int index) const {
    Attrib::Gen::gameplay marker(GetBountySpawnMarker(index), 0, NULL);

    int tag = marker.LocalizationTag();

    return tag;
}

void NotifyGameZonesChanged() {
    if (GManager::Exists()) {
        GManager::Get().RefreshZoneIcons();
    }
}

void NotifyTrackMarkersChanged() {
    if (GManager::Exists()) {
        GManager::Get().RefreshTrackMarkerIcons();
    }
}

void GManager::RefreshZoneIcons() {
    FreeDisposableIcons(GIcon::kType_HidingSpot);

    TrackPathZone *hidingZone = TheTrackPathManager.FindZone(NULL, TRACK_PATH_ZONE_HIDDEN, NULL);

    while (hidingZone != NULL) {
        UMath::Vector3 pos = UMath::Vector3Make(hidingZone->Position.x, hidingZone->Position.y, hidingZone->Elevation);

        GIcon *icon = AllocIcon(GIcon::kType_HidingSpot, pos, 0.0f, true);

        if (icon != NULL) {
            icon->Show();
            icon->ShowOnMap();
        }

        hidingZone = TheTrackPathManager.FindZone(NULL, TRACK_PATH_ZONE_HIDDEN, hidingZone);
    }

    if (GetInGameplay()) {
        SpawnAllLoadedSectionIcons();
    }
}

bool GManager::AddIconForTrackMarker(TrackPositionMarker *marker, unsigned int momentHash) {
    if (marker->NameHash == momentHash) {
        UMath::Vector3 pos = UMath::Vector3Make(marker->Position.x, marker->Position.y, marker->Position.z);

        GIcon *icon = GManager::Get().AllocIcon(GIcon::kType_PursuitBreaker, pos, 0.0f, true);

        if (icon != NULL) {
            icon->Show();
            icon->ShowOnMap();
        }
    }

    return true;
}

void GManager::RefreshTrackMarkerIcons() {
    FreeDisposableIcons(GIcon::kType_PursuitBreaker);

    ForEachTrackPositionMarker(AddIconForTrackMarker, bStringHash("IconMarker"));

    if (GetInGameplay()) {
        SpawnAllLoadedSectionIcons();
    }
}

void GManager::RefreshEngageTriggerIcons() {
    GObjectIterator<GTrigger> triggerIter(0x200);

    while (triggerIter.IsValid()) {
        GTrigger *trigger = triggerIter.GetInstance();
        GActivity *activity = trigger->GetTargetActivity();

        if (activity != nullptr) {
            GRaceParameters *race = GRaceDatabase::Get().GetRaceFromActivity(activity);
            bool completed = GRaceDatabase::Get().CheckRaceScoreFlags(race->GetEventHash(), GRaceDatabase::kCompleted_ContextCareer);
            bool unlocked = GRaceDatabase::Get().CheckRaceScoreFlags(race->GetEventHash(), GRaceDatabase::kUnlocked_Career);

            if (unlocked && !completed) {
                trigger->ShowIcon();
            } else {
                trigger->HideIcon();
            }
        }

        triggerIter.Advance();
    }
}

void GManager::HidePursuitBreakerIcon(const UMath::Vector3 &pos, float radius) {
    UMath::Vector3 posHide;

    bConvertFromBond(posHide, pos);

    for (unsigned int onIcon = 0; onIcon < mNumIcons; onIcon++) {
        GIcon *icon = mIcons[onIcon];

        if (icon->GetType() == GIcon::kType_PursuitBreaker) {
            float d2 = UMath::DistanceSquare(posHide, icon->GetPosition());

            if (d2 <= radius * radius) {
                icon->Hide();
            }
        }
    }
}

void GManager::RestorePursuitBreakerIcons(int sectionID) {
    for (unsigned int i = 0; i < mNumIcons; i++) {
        GIcon *icon = mIcons[i];

        if (icon->GetType() == GIcon::kType_PursuitBreaker) {
            if (sectionID == -1 || sectionID == icon->GetSectionID() || sectionID == icon->GetCombinedSectionID()) {
                icon->Show();
            }
        }
    }
}

GIcon *GManager::AllocIcon(GIcon::Type iconType, const UMath::Vector3 &pos, float rotDeg, bool disposable) {
    if (mNumIcons >= 200) {
        return NULL;
    }

    GIcon *icon = new GIcon(iconType, pos, rotDeg);

    if (icon) {
        mIcons[mNumIcons++] = icon;

        if (disposable) {
            icon->MarkDisposable();
        }
    }

    return icon;
}

void GManager::FreeDisposableIcons(GIcon::Type iconType) {
    unsigned int i = 0;

    while (i < mNumIcons) {
        GIcon *icon = mIcons[i];

        if (icon->GetIsDisposable() && icon->GetType() == iconType) {
            FreeIconAt(i);
        } else {
            i++;
        }
    }
}

void GManager::FreeIcon(GIcon *icon) {
    for (unsigned int i = 0; i < mNumIcons; i++) {
        if (mIcons[i] == icon) {
            FreeIconAt(i);
            return;
        }
    }
}

void GManager::FreeIconAt(unsigned int index) {
    if (mNumIcons) {
        if (mIcons[index]) {
            if (mIcons[index] == WorldMap::mGPSingIcon) {
                WorldMap::ClearGPSing();
            }

            delete mIcons[index];

            mIcons[index] = NULL;
        }

        if (index + 1 < mNumIcons) {
            if (index + 1 < mNumVisibleIcons) {
                mIcons[index] = mIcons[mNumVisibleIcons - 1];

                if (mNumIcons > mNumVisibleIcons) {
                    mIcons[mNumVisibleIcons - 1] = mIcons[mNumIcons - 1];
                }
            } else {
                mIcons[index] = mIcons[mNumIcons - 1];
            }
        }

        if (index < mNumVisibleIcons) {
            mNumVisibleIcons--;
        }

        mNumIcons--;
    }
}

int GManager::GatherVisibleIcons(GIcon **iconArray, IPlayer *player) {
    struct IconSort {
        static int Compare(const void *a, const void *b) {
            return ((const IconSort *)a)->mDist - ((const IconSort *)b)->mDist;
        }

        GIcon *mIcon;
        int mDist;
    };

    UMath::Vector3 playerPos = UMath::Vector3::kZero;
    ISimable *simable = NULL;
    IconSort iconSort[200];

    if (player) {
        simable = player->GetSimable();
    }

    if (simable) {
        bConvertFromBond(playerPos, simable->GetPosition());
    }

    int numOnMap = 0;

    for (unsigned int onIcon = 0; onIcon < mNumVisibleIcons; onIcon++) {
        GIcon *icon = mIcons[onIcon];

        if (icon->GetVisibleOnMap()) {
            iconSort[numOnMap].mIcon = icon;
            iconSort[numOnMap].mDist = simable ? (int)UMath::DistanceSquarexz(icon->GetPosition(), playerPos) : 0;

            numOnMap++;
        }
    }

    if (simable) {
        qsort(iconSort, numOnMap, sizeof(IconSort), IconSort::Compare);
    }

    for (int onCopy = 0; onCopy < numOnMap; onCopy++) {
        iconArray[onCopy] = iconSort[onCopy].mIcon;
    }

    return numOnMap;
}

bool GManager::GetIsIconVisible(GIcon *icon) {
    for (unsigned int i = 0; i < mNumVisibleIcons; i++) {
        if (mIcons[i] == icon) {
            return true;
        }
    }

    return false;
}

void GManager::SpawnAllLoadedSectionIcons() {
    for (unsigned int i = 0; i < mNumIcons; i++) {
        GIcon *icon = mIcons[i];

        icon->FindSection();

        if (icon->GetSectionID() >= 0) {
            TrackStreamingSection *section = TheTrackStreamer.FindSection(icon->GetSectionID());

            if (section && section->Status == TrackStreamingSection::ACTIVATED) {
                icon->Spawn();
            } else {
                section = TheTrackStreamer.FindSection(icon->GetCombinedSectionID());

                if (section && section->Status == TrackStreamingSection::ACTIVATED) {
                    icon->Spawn();
                }
            }
        }
    }
}

void GManager::SpawnSectionIcons(int section) {
    for (unsigned int i = 0; i < mNumIcons; i++) {
        GIcon *icon = mIcons[i];

        if (icon->GetSectionID() < 0) {
            icon->FindSection();
        }

        if (icon->GetSectionID() == section || icon->GetCombinedSectionID() == section) {
            icon->Spawn();
        }
    }
}

void GManager::UnspawnSectionIcons(int section) {
    for (unsigned int i = 0; i < mNumIcons; i++) {
        GIcon *icon = mIcons[i];

        if (icon->GetSectionID() < 0) {
            icon->FindSection();
        }

        if (icon->GetSectionID() == section || icon->GetCombinedSectionID() == section) {
            icon->Unspawn();
        }
    }
}

void GManager::UnspawnAllIcons() {
    for (unsigned int i = 0; i < mNumIcons; i++) {
        mIcons[i]->Unspawn();
    }
}

void GManager::FreeAllIcons() {
    while (mNumIcons) {
        FreeIconAt(0);
    }
}

void GManager::AllocateMilestones() {
    Attrib::Gen::gameplay collection(Attrib::Hash::gameplay::key_milestones, 0, NULL);
    AttribKeyList keyList;

    GatherInstanceKeys(collection, keyList, Attrib::Hash::gameplay::key_milestone);

    mNumMilestones = keyList.size();
    mMilestones = new ("Gameplay milestone challenges", 0) GMilestone[mNumMilestones];

    GMilestone *milestone = mMilestones;

    for (AttribKeyList::iterator it = keyList.begin(); it != keyList.end(); ++it) {
        milestone->Init(*it);
        milestone++;
    }
}

void GManager::ReleaseMilestones() {
    if (mMilestones) {
        delete[] mMilestones;
    }

    mMilestones = NULL;
    mNumMilestones = 0;
}

void GManager::ResetMilestones() {
    if (mNumMilestones) {
        Attrib::Gen::gameplay collection(Attrib::Hash::gameplay::key_milestones, 0, NULL);
        AttribKeyList keyList;

        GatherInstanceKeys(collection, keyList, Attrib::Hash::gameplay::key_milestone);

        GMilestone *milestone = mMilestones;

        for (AttribKeyList::iterator it = keyList.begin(); it != keyList.end(); ++it) {
            milestone->Init(*it);
            milestone++;
        }
    }
}

unsigned int GManager::SaveMilestones(GMilestone *dest) {
    bMemCpy(dest, mMilestones, mNumMilestones * sizeof(GMilestone));
    return mNumMilestones;
}

void GManager::LoadMilestones(GMilestone *src, unsigned int count) {
    for (unsigned int i = 0; i < count; i++) {
        GMilestone *saved = &src[i];

        for (unsigned int j = 0; j < mNumMilestones; j++) {
            GMilestone *milestone = &mMilestones[j];

            if (milestone->GetChallengeKey() == saved->GetChallengeKey()) {
                *milestone = *saved;
                break;
            }
        }
    }
}

void GManager::AllocateSpeedTraps() {
    Attrib::Gen::gameplay collection(Attrib::Hash::gameplay::key_speedtraps, 0, NULL);
    AttribKeyList keyList;

    GatherInstanceKeys(collection, keyList, Attrib::Hash::gameplay::key_speedtrap);

    mNumSpeedTraps = keyList.size();
    mSpeedTraps = new ("Gameplay speed traps", 0) GSpeedTrap[mNumSpeedTraps];

    GSpeedTrap *speedTrap = mSpeedTraps;

    for (AttribKeyList::iterator it = keyList.begin(); it != keyList.end(); ++it) {
        speedTrap->Init(*it);
        speedTrap++;
    }
}

void GManager::ReleaseSpeedTraps() {
    if (mSpeedTraps) {
        delete[] mSpeedTraps;
    }

    mSpeedTraps = NULL;
    mNumSpeedTraps = 0;
}

void GManager::ResetSpeedTraps() {
    if (mNumSpeedTraps) {
        Attrib::Gen::gameplay collection(Attrib::Hash::gameplay::key_speedtraps, 0, NULL);
        AttribKeyList keyList;

        GatherInstanceKeys(collection, keyList, Attrib::Hash::gameplay::key_speedtrap);

        GSpeedTrap *speedTrap = mSpeedTraps;

        for (AttribKeyList::iterator it = keyList.begin(); it != keyList.end(); ++it) {
            speedTrap->Init(*it);
            speedTrap++;
        }
    }
}

unsigned int GManager::SaveSpeedTraps(GSpeedTrap *dest) {
    bMemCpy(dest, mSpeedTraps, mNumSpeedTraps * sizeof(GSpeedTrap));
    return mNumSpeedTraps;
}

void GManager::LoadSpeedTraps(GSpeedTrap *src, unsigned int count) {
    for (unsigned int i = 0; i < count; i++) {
        GSpeedTrap *saved = &src[i];

        for (unsigned int j = 0; j < mNumSpeedTraps; j++) {
            GSpeedTrap *speedTrap = &mSpeedTraps[j];

            if (speedTrap->GetSpeedTrapKey() == saved->GetSpeedTrapKey()) {
                *speedTrap = *saved;
                break;
            }
        }
    }
}

void GManager::ServicePendingCharacters() {
    for (GCharacterList::iterator it = mActiveCharacters.begin(); it != mActiveCharacters.end(); it++) {
        GCharacter *character = *it;

        if (character->SpawnPending()) {
            if (character->AttemptSpawn()) {
                break;
            }
        }
    }
}

void GManager::UnspawnUselessCharacters() {
    for (GCharacterList::iterator it = mActiveCharacters.begin(); it != mActiveCharacters.end(); it++) {
        GCharacter *character = *it;

        if (character->IsNoLongerUseful()) {
            character->Unspawn();
            break;
        }
    }
}

void GManager::RecursivePreloadCharacterCars(GRuntimeInstance *instance, bool forcePreload) {
    if (instance->GetType() == kGameplayObjType_Character) {
        const char *carType = instance->CarType();
        const char *carTypeLowMem = instance->CarTypeLowMem();

        if (carTypeLowMem != NULL && carTypeLowMem[0] != 0) {
            carType = carTypeLowMem;
        }

        if (instance->ForcePreload() || forcePreload) {
            ReserveStockCar(carType);
        }
    }

    for (unsigned int onChild = 0; onChild < instance->Num_Children(); onChild++) {
        const GCollectionKey &childSpec = instance->Children(onChild);

        GRuntimeInstance *childInst = FindInstance(childSpec.GetCollectionKey());

        if (childInst != NULL) {
            RecursivePreloadCharacterCars(childInst, forcePreload);
        }
    }
}

void GManager::PreloadStockCarsForActivity(GActivity *activity) {
    ClearStockCars();

    if (activity != NULL) {
        bool forcePreload = false;

        GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromActivity(activity);

        if (parms != NULL) {
            forcePreload = parms->GetRaceType() == GRace::kRaceType_Drag;
        }

        if (gVerboseTesterOutput) {
            if (parms != NULL) {
                const char *raceName = parms->GetEventID();
            }
        }

        RecursivePreloadCharacterCars(activity, forcePreload);
    }
}

void GManager::ReserveStockCar(const char *carName) {
    if (carName == NULL || carName[0] == 0) {
        return;
    }

    unsigned int carHash = Attrib::StringHash32(carName);
    StockCarMap::iterator iterExisting = mStockCars.find(carHash);

    if (iterExisting == mStockCars.end()) {
        UMath::Vector3 pos = {0.0f, 0.0f, 0.0f};
        UMath::Vector3 dir = {0.0f, 0.0f, 1.0f};
        IVehicleCache *cache = this;

        VehicleParams params(cache, DRIVER_NONE, Attrib::StringToKey(carName), dir, pos, 0, NULL, NULL);
        ISimable *simable = ISimable::CreateInstance("PVehicle", params);
        if (simable == NULL) {
            return;
        }

        {
            IVehicle *vehicle = NULL;
            if (simable->QueryInterface(&vehicle)) {
                vehicle->Deactivate();
            }
        }

        mStockCars[carHash] = simable;
    }
}

bool GManager::StockCarsLoaded() {
    for (StockCarMap::iterator it = mStockCars.begin(); it != mStockCars.end(); ++it) {
        ISimable *car = it->second;
        IVehicle *vehicle;

        if (car->QueryInterface(&vehicle)) {
            if (vehicle->IsLoading()) {
                return false;
            }
        }
    }

    return true;
}

void GManager::ClearStockCars() {
    for (StockCarMap::iterator it = mStockCars.begin(); it != mStockCars.end(); ++it) {
        ISimable *car = it->second;
        car->Kill();
    }

    mStockCars.clear();
}

ISimable *GManager::GetStockCar(const char *carName) {
    StockCarMap::iterator it = mStockCars.find(Attrib::StringHash32(carName));

    if (it != mStockCars.end()) {
        ISimable *stockCar = it->second;

        mStockCars.erase(it);

        return stockCar;
    }

    return NULL;
}

ISimable *GManager::GetRandomEmergencyStockCar() {
    if (GRaceStatus::Get().GetRaceParameters() && GRaceStatus::Get().GetRaceParameters()->GetRaceType() != GRace::kRaceType_Drag) {
        return NULL;
    }

    UTL::Std::vector<StockCarMap::iterator, _type_vector> candidates;
    candidates.reserve(mStockCars.size());

    for (StockCarMap::iterator it = mStockCars.begin(); it != mStockCars.end(); ++it) {
        ISimable *car = it->second;
        IArticulatedVehicle *articulated = NULL;

        if (car) {
            car->QueryInterface(&articulated);
        }

        if (articulated) {
            continue;
        }

        candidates.push_back(it);
    }

    if ((int)candidates.size() > 0) {
        int index = bRandom((int)candidates.size());
        ISimable *car = candidates[index]->second;
        mStockCars.erase(candidates[index]);
        return car;
    }

    return NULL;
}

void GManager::ReleaseStockCar(ISimable *stockCar) {
    const Attrib::Instance &attributes = stockCar->GetAttributes();
    unsigned int carKey = attributes.GetCollection();

    StockCarMap::iterator it = mStockCars.find(carKey);

    mStockCars[carKey] = stockCar;
}

bool GManager::SetTimer(const char *name, float interval) {
    unsigned int hash = bStringHash(name);

    for (int index = 0; index < 8; index++) {
        GEventTimer &timer = mTimers[index];

        if (hash == timer.GetNameHash()) {
            timer.Stop();
            timer.SetInterval(interval);
            timer.Start();

            return true;
        }
    }

    for (int index = 0; index < 8; index++) {
        GEventTimer &timer = mTimers[index];

        if (!timer.IsRunning()) {
            timer.SetName(name);
            timer.SetInterval(interval);
            timer.Start();

            return true;
        }
    }

    return false;
}

void GManager::KillTimer(const char *name) {
    unsigned int nameHash = bStringHash(name);

    for (unsigned int i = 0; i < 8; i++) {
        GEventTimer *timer = &mTimers[i];

        if (nameHash == timer->GetNameHash()) {
            timer->Stop();
            break;
        }
    }
}

void GManager::ResetTimers() {
    for (unsigned int i = 0; i < 8; i++) {
        GEventTimer *timer = &mTimers[i];
        timer->Reset();
    }
}

void GManager::UpdateTimers(float dT) {
    for (unsigned int i = 0; i < 8; i++) {
        GEventTimer *timer = &mTimers[i];
        timer->Update(dT);
    }
}

unsigned int GManager::SaveTimerInfo(SavedTimerInfo *saveInfo) {
    for (unsigned int i = 0; i < 8; i++) {
        GEventTimer *timer = &mTimers[i];
        timer->Serialize(&saveInfo[i]);
    }

    return 8;
}

void GManager::LoadTimerInfo(SavedTimerInfo *saveInfo, unsigned int count) {
    for (unsigned int i = 0; i < count; i++) {
        GEventTimer *timer = &mTimers[i];
        timer->Deserialize(&saveInfo[i]);
    }
}

unsigned int GManager::SaveSMSInfo(int *saveInfo) {
    int numMessages = mPendingSMS.size();
    PendingSMSList::iterator it = mPendingSMS.begin();

    for (int index = 0; index < numMessages; index++) {
        int smsID = *it;
        saveInfo[index++] = smsID;
        it++;
    }

    return numMessages;
}

void GManager::LoadSMSInfo(int *loadInfo, unsigned int count) {
    mPendingSMS.clear();

    for (unsigned int i = 0; i < count; i++) {
        mPendingSMS.push_back(loadInfo[i]);
    }
}

bool GManager::GetHasPendingSMS() const {
    if (mPendingSMS.size() == 0) {
        return false;
    }

    return CanPlaySMS();
}

bool GManager::CanPlaySMS() const {
    IPlayer *player;

    if (Speech::Manager::m_speechDisable) {
        return false;
    }

    if (TheGameFlowManager.GetState() != GAMEFLOW_STATE_RACING) {
        return false;
    }

    if (INIS::Get()) {
        return false;
    }

    if (PhotoFinishScreen::GetActive()) {
        return false;
    }

    if (!GRaceStatus::Exists() || GRaceStatus::Get().GetPlayMode() != GRaceStatus::kPlayMode_Roaming) {
        return false;
    }

    if (!FEManager::IsOkayToRequestPauseSimulation(0, true, true)) {
        return false;
    }

    if (Speech::Manager::IsCopSpeechPlaying(kSPCH1_EventID_CellCall)) {
        return false;
    }

    player = IPlayer::First(PLAYER_LOCAL);

    if (!player) {
        return false;
    }

    ISimable *simable = player->GetSimable();

    if (!simable) {
        return false;
    }

    IHud *ihud = player->GetHud();

    if (!ihud || !ihud->IsHudVisible()) {
        return false;
    }

    IVehicle *ivehicle;

    if (!simable->QueryInterface(&ivehicle) || !ivehicle) {
        return false;
    }

    if (ivehicle->IsAnimating() || ivehicle->IsStaging() || ivehicle->IsLoading()) {
        return false;
    }

    IVehicleAI *ivehicleai = ivehicle->GetAIVehiclePtr();

    if (!ivehicleai) {
        return false;
    }

    return ivehicleai->GetPursuit() == NULL;
}

void GManager::AddSMS(int smsID) {
    if ((smsID >= 10 && smsID <= 14) || smsID == 95 || smsID == 96) {
        DispatchSMSMessage(smsID);
    } else {
        SMSMessage *message = FEDatabase->GetCareerSettings()->GetSMSMessage(smsID);

        if (message && message->GetFlags() != 0) {
            return;
        }

        for (PendingSMSList::iterator it = mPendingSMS.begin(); it != mPendingSMS.end(); it++) {
            if (*it == smsID) {
                return;
            }
        }

        mPendingSMS.push_back(smsID);
    }
}

void GManager::DispatchSMSMessage(int smsID) {
    SMSMessage *message = FEDatabase->GetCareerSettings()->GetSMSMessage(smsID);

    if (message && message->GetFlags() == 0) {
        message->Flags = SMS_FLAG_READ;

        message->SetSortOrder(FEDatabase->GetCareerSettings()->GetSMSSortOrder());

        new ECellCall(smsID);
    }
}

void GManager::UpdatePendingSMS() {
    if (mPendingSMS.size() != 0 && CanPlaySMS()) {
        int smsToShow = PushSMSToInbox();

        if (smsToShow != -1) {
            new EShowSMS(smsToShow);
        }

        IPlayer *player = IPlayer::First(PLAYER_LOCAL);

        if (player != NULL) {
            IHud *ihud = player->GetHud();

            if (ihud != NULL) {
                IMenuZoneTrigger *izone;

                if (ihud->QueryInterface(&izone)) {
                    if (izone != NULL) {
                        izone->RequestCingularLogo();
                    }
                }
            }
        }
    }
}

int GManager::PushSMSToInbox() {
    int smsToPlay = -1;

    for (PendingSMSList::iterator it = mPendingSMS.begin(); it != mPendingSMS.end(); it++) {
        int smsID = *it;

        SMSMessage *msg = FEDatabase->GetCareerSettings()->GetSMSMessage(smsID);

        if (msg != NULL) {
            if (msg->IsVoice() && smsToPlay == -1) {
                smsToPlay = smsID;
            }

            msg->ClearFlags();
            msg->SetFlag(2);
            msg->SetSortOrder(FEDatabase->GetCareerSettings()->GetSMSSortOrder());
        }
    }

    if (smsToPlay == -1 && mPendingSMS.size() != 0) {
        smsToPlay = *mPendingSMS.begin();
    }

    mPendingSMS.clear();

    return smsToPlay;
}

unsigned int GManager::GetRespawnMarker() {
    Attrib::Gen::gameplay overrideMarker(mOverrideFreeRoamStartMarker, 0, NULL);

    if (overrideMarker.IsValid()) {
        return mOverrideFreeRoamStartMarker;
    }

    unsigned int markerKey;

    if (mStartFreeRoamFromSafeHouse) {
        markerKey = mFreeRoamFromSafeHouseStartMarker;
    } else {
        markerKey = mFreeRoamStartMarker;
    }

    Attrib::Gen::gameplay marker(markerKey, 0, NULL);

    if (marker.IsValid()) {
        return markerKey;
    }

    char buffer[32];

    bSPrintf(buffer, "career_start_%s", TrackInfo::GetLoadedTrackInfo()->RegionName);

    markerKey = Attrib::StringToLowerCaseKey(buffer);
    mFreeRoamStartMarker = markerKey;

    return markerKey;
}

void GManager::GetRespawnLocation(UMath::Vector3 &startLoc, UMath::Vector3 &initialVec) {
    unsigned int markerKey = GetRespawnMarker();
    Attrib::Gen::gameplay marker(markerKey, 0, NULL);

    const UMath::Vector3 &pos = marker.Position();
    startLoc = UMath::Vector3Make(-pos.y, pos.z, pos.x);

    UMath::Matrix4 rotMat = UMath::Matrix4::kIdentity;
    initialVec = UMath::Vector3Make(0.0f, 0.0f, 1.0f);

    UMath::Init(rotMat, 1.0f, 1.0f, 1.0f);
    MATRIX4_multyrot(&rotMat, -marker.Rotation() / 360.0f, &rotMat);
    UMath::Rotate(initialVec, rotMat, initialVec);
}
