#include "Speed/Indep/Src/Gameplay/GManager.h"

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/gameplay.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/milestonetypes.h"
#include "Speed/Indep/Src/Generated/Events/EAutoSave.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOff.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOn.hpp"
#include "Speed/Indep/Src/Generated/Messages/MEnteringGameplay.h"
#include "Speed/Indep/Src/Gameplay/GMarker.h"
#include "Speed/Indep/Src/Gameplay/GVault.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ICopMgr.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ITrafficMgr.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IArticulatedVehicle.h"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/LZCompress.hpp"
#include "Speed/Indep/Src/Misc/MD5.hpp"
#include "Speed/Indep/Src/Misc/Platform.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/TrackPath.hpp"
#include "Speed/Indep/Src/World/TrackInfo.hpp"
#include "Speed/Indep/Src/World/TrackPositionMarker.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/Src/World/WCollisionAssets.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribLoadAndGo.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

#include <algorithm>
#include <new>

extern int SkipFE;
extern int TWEAK_ShowAllGameplayIcons;
extern int gVerboseTesterOutput;

char *bStrIStr(const char *s1, const char *s2);
void bCloseMemoryPool(int pool_num);
bool bSetMemoryPoolDebugTracing(int pool_num, bool on_off);
void ApplyTimeOfDayTickOver();
void SetOverRideRainIntensity(float intensity);
void ForEachTrackPositionMarker(bool (*callback)(TrackPositionMarker *marker, unsigned int tag), unsigned int tag);
void LZByteSwapHeader(LZHeader *header);
void World_RestoreProps();
void GPS_Disengage();

class IPursuit;

namespace Speech {
class Manager {
  public:
    static int m_speechDisable;
    static bool IsCopSpeechPlaying(int event_id);
};
}; // namespace Speech

class PhotoFinishScreen {
  public:
    static int mActive;
};

#ifndef DECLARE_GAMEPLAY_MINIMAP_CLASS
#define DECLARE_GAMEPLAY_MINIMAP_CLASS
class Minimap {
  public:
    static void ConvertPos(bVector2 &worldPos, bVector2 &minimapPos, TrackInfo *track);
};
#endif

struct GManagerRaceStatusCompat {
    unsigned char _padRaceBin[0x1AB0];
    GRaceBin *mRaceBin;
    unsigned char _padRefresh[0x40];
    bool mRefreshBinAfterRace;
};

class GCopMgrCompat : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    static GCopMgrCompat *Get() {
        return reinterpret_cast<GCopMgrCompat *>(ICopMgr::Get());
    }

    virtual float GetLockoutTimeRemaining() const;
    virtual bool VehicleSpawningEnabled(bool isdespawn);
    virtual void SpawnCop(UMath::Vector3 &InitialPos, UMath::Vector3 &InitialVec, const char *VehicleName, bool InPursuit, bool RoadBlock);
    virtual bool IsCopSpawnPending() const;
    virtual void SetAllBustedTimersToZero();
    virtual void PursuitIsEvaded(IPursuit *ipursuit);
    virtual bool IsCopRequestPending();
    virtual bool CanPursueRacers();
    virtual bool IsPlayerPursuitActive();
    virtual bool PlayerPursuitHasCop() const;
    virtual void PursueAtHeatLevel(int minHeatLevel);
    virtual void ResetCopsForRestart(bool release);
    virtual void LockoutCops(bool lockout);
    virtual void NoNewPursuitsOrCops();

  protected:
    GCopMgrCompat(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}
    virtual ~GCopMgrCompat() {}
};

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

void GManager::PreBeginGameplay() {
    if (mRestartEventHash) {
        GRaceCustom *customRace = GRaceDatabase::Get().AllocCustomRace(GRaceDatabase::Get().GetRaceFromHash(mRestartEventHash));

        GRaceDatabase::Get().SetStartupRace(customRace, GRace::kRaceContext_Career);
        GRaceDatabase::Get().FreeCustomRace(customRace);
        mRestartEventHash = 0;
    }
}

void GManager::BeginGameplay() {
    for (unsigned int i = 0; i < mVaultCount; ++i) {
        if (mVaults[i].IsLoaded()) {
            mVaults[i].CreateGameplayObjects();
        }
    }

    GRaceCustom *startupRace = GRaceDatabase::Get().GetStartupRace();
    if (startupRace) {
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

    if (!startupRace) {
        GRaceStatus::Get().EnableBinBarriers();
    }

    MEnteringGameplay().Post(UCrc32(0x20D60DBF));
    mStartFreeRoamFromSafeHouse = false;
    mInGameplay = true;
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

void GManager::ClearAllSessionData() {
    mSessionStateBlocks.clear();
    DefragObjectStateStorage();
}

void GManager::ServicePendingCharacters() {
    for (GCharacterList::iterator it = mActiveCharacters.begin(); it != mActiveCharacters.end(); ++it) {
        GCharacter *character = *it;

        if (character->SpawnPending() && character->AttemptSpawn()) {
            return;
        }
    }
}

void GManager::UnspawnUselessCharacters() {
    for (GCharacterList::iterator it = mActiveCharacters.begin(); it != mActiveCharacters.end(); ++it) {
        GCharacter *character = *it;

        if (character->IsNoLongerUseful()) {
            character->Unspawn();
            return;
        }
    }
}

void GManager::EndGameplay() {
    UnspawnAllCharacters();
    ClearStockCars();

    for (unsigned int i = 0; i < mVaultCount; ++i) {
        if (mVaults[i].IsLoaded()) {
            mVaults[i].DestroyGameplayObjects();
        }
    }

    UnspawnAllIcons();
    ClearAllSessionData();
    WCollisionAssets::Get().RemovePackLoadCallback(NotifyCollisionPackLoaded);
    GRaceDatabase::Get().SetStartupRace(nullptr, GRace::kRaceContext_Career);
    mInGameplay = false;
    mWorstHashCollision = 0;
    mOverrideFreeRoamStartMarker = 0;
    ApplyTimeOfDayTickOver();
    SetOverRideRainIntensity(0.0f);
}

void GManager::StartWorldActivities(bool startFreeRoamOnly) {
    for (unsigned int i = 0; i < mInstanceHashTableSize; ++i) {
        GRuntimeInstance *instance = mKeyToInstanceMap[i].mInstance;

        if (instance && instance->GetType() == kGameplayObjType_Activity) {
            GActivity *activity = static_cast<GActivity *>(instance);
            bool autoStart = activity->AutoStart(0);

            if (activity->FreeRoamOnly(0) && !startFreeRoamOnly) {
                autoStart = false;
            }

            if (autoStart) {
                activity->Run();
            }
        }
    }
}

void GManager::StartActivities() {
    bool startWorld = SkipFE == 0;
    GRaceCustom *startupRace = GRaceDatabase::Get().GetStartupRace();

    if (startupRace) {
        startupRace->GetRaceActivity()->Run();
        GRaceStatus::Get().SetRaceContext(GRaceDatabase::Get().GetStartupRaceContext());
        startWorld = GRaceDatabase::Get().GetStartupRaceContext() == GRace::kRaceContext_Career && startWorld;
    }

    if (startWorld) {
        StartWorldActivities(startupRace == nullptr);
    }
}

void GManager::StartRaceFromInGame(unsigned int raceHash) {
    if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming) {
        GRaceParameters *race = GRaceDatabase::Get().GetRaceFromHash(raceHash);

        if (race) {
            GManagerRaceStatusCompat &raceStatus = reinterpret_cast<GManagerRaceStatusCompat &>(GRaceStatus::Get());

            if (raceStatus.mRaceBin) {
                if (race->GetParentVault() != raceStatus.mRaceBin->GetChildVault()) {
                    SuspendAllBinActivities();
                    raceStatus.mRefreshBinAfterRace = true;
                }
            }

            race->BlockUntilLoaded();
            race->GetActivity()->Reset();
            race->GetActivity()->Run();
        }
    }
}

void GManager::UpdateTimers(float dT) {
    for (unsigned int i = 0; i < 8; ++i) {
        mTimers[i].Update(dT);
    }
}

bool GManager::SetTimer(const char *name, float interval) {
    unsigned int hash = bStringHash(name);
    int index;

    for (index = 0; index < 8; ++index) {
        GEventTimer &timer = mTimers[index];

        if (hash == timer.GetNameHash()) {
            timer.Stop();
            timer.SetInterval(interval);
            timer.Start();
            return true;
        }
    }

    for (index = 0; index < 8; ++index) {
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
    unsigned int hash = bStringHash(name);

    for (unsigned int i = 0; i < 8; ++i) {
        if (hash == mTimers[i].GetNameHash()) {
            mTimers[i].Stop();
            return;
        }
    }
}

unsigned int GManager::SaveTimerInfo(SavedTimerInfo *saveInfo) {
    for (unsigned int i = 0; i < 8; ++i) {
        mTimers[i].Serialize(&saveInfo[i]);
    }

    return 8;
}

void GManager::LoadTimerInfo(SavedTimerInfo *saveInfo, unsigned int count) {
    for (unsigned int i = 0; i < count; ++i) {
        mTimers[i].Deserialize(&saveInfo[i]);
    }
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

GMilestone *GManager::GetFirstMilestone(bool availOnly, unsigned int binNumber) {
    return GetNextMilestone(mMilestones - 1, availOnly, binNumber);
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

float GManager::GetBestValue(unsigned int valueKey) {
    return mMilestoneTypeInfo.find(valueKey)->second.mBestValue;
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
    unsigned int collectionKey;

    mMilestoneTypeInfo.clear();
    collectionKey = mMilestoneClass->GetFirstCollection();
    while (collectionKey) {
        Attrib::Gen::milestonetypes milestoneType(
            Attrib::FindCollection(Attrib::Gen::milestonetypes::ClassKey(), collectionKey), 0, nullptr);
        MilestoneTypeInfo info;

        info.mTypeKey = collectionKey;
        info.mLastKnownValue = -1.0f;
        info.mBestValue = -1.0f;
        info.mFlags = milestoneType.MilestoneType() != 2 ? GMilestone::kFlag_BiggerIsBetter : 0;
        if (milestoneType.ResetWhenPursuitStarts()) {
            info.mFlags |= GMilestone::kFlag_CompletionFaked;
        }

        mMilestoneTypeInfo[collectionKey] = info;
        collectionKey = mMilestoneClass->GetNextCollection(collectionKey);
    }
}

void GManager::LoadMilestoneInfo(MilestoneTypeInfo *savedInfo, unsigned int count) {
    unsigned int i;

    ResetMilestoneTrackingInfo();
    for (i = 0; i < count; ++i) {
        MilestoneTypeInfo &info = savedInfo[i];
        mMilestoneTypeInfo[info.mTypeKey] = info;
    }
}

ObjectStateBlockHeader *GManager::AllocObjectStateBlock(unsigned int key, unsigned int size, bool persistent) {
    ObjectStateMap &blocks = persistent ? mPersistentStateBlocks : mSessionStateBlocks;
    unsigned int shiftedKey = key >> mCollectionKeyShiftTo32;
    ObjectStateMap::iterator it = blocks.find(shiftedKey);
    unsigned int blockSize;
    unsigned int tryCount;
    ObjectStateBlockHeader *block;
    ObjectStateBlockHeader *userData;

    if (it != blocks.end()) {
        block = it->second;
        if (size <= block->mSize) {
            block->mSize = size;
            return block + 1;
        }

        ClearObjectStateBlock(key);
    }

    blockSize = (size + 0x17U) & ~0xFU;
    for (tryCount = 0; tryCount < 2; ++tryCount) {
        if (mObjectStateBufferSize >= static_cast<unsigned int>(mObjectStateBufferFree - mObjectStateBuffer) + blockSize) {
            break;
        }

        if (tryCount != 0) {
            return nullptr;
        }

        DefragObjectStateStorage();
    }

    block = reinterpret_cast<ObjectStateBlockHeader *>(mObjectStateBufferFree);
    mObjectStateBufferFree += blockSize;
    if (persistent) {
        bMemSet(block, 0, blockSize);
    }

    block->mKey = key;
    block->mSize = size;
    userData = block + 1;
    blocks[shiftedKey] = block;
    return userData;
}

void *GManager::GetObjectStateBlock(unsigned int key) {
    unsigned int shiftedKey = key >> mCollectionKeyShiftTo32;
    ObjectStateMap::iterator it = mSessionStateBlocks.find(shiftedKey);

    if (it == mSessionStateBlocks.end()) {
        it = mPersistentStateBlocks.find(shiftedKey);
        if (it == mPersistentStateBlocks.end()) {
            return nullptr;
        }
    }

    return it->second + 1;
}

void GManager::ClearObjectStateBlock(unsigned int key) {
    unsigned int shiftedKey = key >> mCollectionKeyShiftTo32;
    ObjectStateMap::iterator it = mSessionStateBlocks.find(shiftedKey);

    if (it != mSessionStateBlocks.end()) {
        mSessionStateBlocks.erase(it);
    }

    it = mPersistentStateBlocks.find(shiftedKey);
    if (it != mPersistentStateBlocks.end()) {
        mPersistentStateBlocks.erase(it);
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

void GManager::EnableBinMilestones(unsigned int binNumber) {
    for (GMilestone *milestone = GetFirstMilestone(false, binNumber); milestone;
         milestone = GetNextMilestone(milestone, false, binNumber)) {
        milestone->Unlock();
    }
}

void GManager::NotifyPursuitStarted() {
    for (MilestoneInfoMap::iterator it = mMilestoneTypeInfo.begin(); it != mMilestoneTypeInfo.end(); ++it) {
        if ((it->second.mFlags & GMilestone::kFlag_CompletionFaked) != 0) {
            it->second.mLastKnownValue = -1.0f;
            it->second.mBestValue = -1.0f;
        }
    }
}

void GManager::NotifyCollisionPackLoaded(int sectionID, bool loaded) {
    if (mObj) {
        if (loaded) {
            mObj->SpawnSectionIcons(sectionID);
        } else {
            mObj->UnspawnSectionIcons(sectionID);
        }
    }
}

void GManager::SpawnSectionIcons(int section) {
    for (unsigned int i = 0; i < mNumIcons; ++i) {
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
    for (unsigned int i = 0; i < mNumIcons; ++i) {
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
    for (unsigned int i = 0; i < mNumIcons; ++i) {
        mIcons[i]->Unspawn();
    }
}

bool GManager::GetIsIconVisible(GIcon *icon) {
    for (unsigned int i = 0; i < mNumVisibleIcons; ++i) {
        if (mIcons[i] == icon) {
            return true;
        }
    }

    return false;
}

void GManager::SpawnAllLoadedSectionIcons() {
    for (unsigned int i = 0; i < mNumIcons; ++i) {
        GIcon *icon = mIcons[i];

        icon->FindSection();
        if (icon->GetSectionID() >= 0) {
            if (TheTrackStreamer.IsSectionVisible(icon->GetSectionID()) ||
                TheTrackStreamer.IsSectionVisible(icon->GetCombinedSectionID())) {
                icon->Spawn();
            }
        }
    }
}

void GManager::RestorePursuitBreakerIcons(int sectionID) {
    for (unsigned int i = 0; i < mNumIcons; ++i) {
        GIcon *icon = mIcons[i];

        if (icon->GetType() == GIcon::kType_PursuitBreaker &&
            (sectionID == -1 || sectionID == icon->GetSectionID() || sectionID == icon->GetCombinedSectionID())) {
            icon->SetFlag(1);
        }
    }
}

void GManager::HidePursuitBreakerIcon(const UMath::Vector3 &pos, float radius) {
    UMath::Vector3 swizzledPos = UMath::Vector3Make(pos.z, -pos.x, pos.y);

    for (unsigned int i = 0; i < mNumIcons; ++i) {
        GIcon *icon = mIcons[i];

        if (icon->GetType() == GIcon::kType_PursuitBreaker &&
            UMath::DistanceSquare(swizzledPos, icon->GetPosition()) <= radius * radius) {
            icon->ClearFlag(1);
        }
    }
}

void GManager::FreeDisposableIcons(GIcon::Type iconType) {
    for (unsigned int i = 0; i < mNumIcons;) {
        GIcon *icon = mIcons[i];

        if (icon->GetIsDisposable() && icon->GetType() == iconType) {
            FreeIconAt(i);
        } else {
            ++i;
        }
    }
}

void GManager::FreeIcon(GIcon *icon) {
    for (unsigned int i = 0; i < mNumIcons; ++i) {
        if (mIcons[i] == icon) {
            FreeIconAt(i);
            return;
        }
    }
}

void GManager::FreeAllIcons() {
    while (mNumIcons != 0) {
        FreeIconAt(0);
    }
}

unsigned int GManager::GetBountySpawnMarker(unsigned int index) const {
    if (index >= mNumBountySpawnPoints) {
        return 0;
    }

    return mBountySpawnPoint[index];
}

int GManager::GetBountySpawnMarkerTag(unsigned int index) const {
    Attrib::Gen::gameplay gameplay(Attrib::FindCollection(Attrib::Gen::gameplay::ClassKey(), GetBountySpawnMarker(index)), 0, nullptr);

    return gameplay.LocalizationTag(0);
}

void GManager::RefreshZoneIcons() {
    FreeDisposableIcons(GIcon::kType_HidingSpot);

    for (TrackPathZone *zone = TheTrackPathManager.FindZone(nullptr, TRACK_PATH_ZONE_HIDDEN, nullptr); zone;
         zone = TheTrackPathManager.FindZone(nullptr, TRACK_PATH_ZONE_HIDDEN, zone)) {
        UMath::Vector3 pos;

        pos.x = zone->Position.x;
        pos.y = zone->Position.y;
        pos.z = zone->Elevation;

        GIcon *icon = AllocIcon(GIcon::kType_HidingSpot, pos, 0.0f, true);
        if (icon) {
            icon->Show();
            icon->ShowOnMap();
        }
    }

    if (GetInGameplay()) {
        SpawnAllLoadedSectionIcons();
    }
}

void GManager::RefreshWorldParticleEffects() {
    for (unsigned int i = 0; i < mInstanceHashTableSize; ++i) {
        GRuntimeInstance *instance = mKeyToInstanceMap[i].mInstance;

        if (instance && instance->GetType() == kGameplayObjType_Trigger) {
            static_cast<GTrigger *>(instance)->RefreshParticleEffects();
        }
    }

    for (unsigned int i = 0; i < mNumIcons; ++i) {
        mIcons[i]->RefreshEffects();
    }
}

void GManager::RefreshEngageTriggerIcons() {
    for (unsigned int i = 0; i < mInstanceHashTableSize; ++i) {
        GRuntimeInstance *instance = mKeyToInstanceMap[i].mInstance;

        if (instance && instance->GetType() == kGameplayObjType_Trigger) {
            GTrigger *trigger = static_cast<GTrigger *>(instance);
            GActivity *activity = trigger->GetTargetActivity();

            if (activity) {
                GRaceParameters *race = GRaceDatabase::Get().GetRaceFromActivity(activity);

                if (race) {
                    if (!race->GetIsAvailable(GRace::kRaceContext_Career)) {
                        trigger->HideIcon();
                    } else {
                        trigger->ShowIcon();
                    }
                }
            }
        }
    }
}

GIcon *GManager::AllocIcon(GIcon::Type iconType, const UMath::Vector3 &pos, float rotDeg, bool disposable) {
    GIcon *icon = nullptr;

    if (mNumIcons < 200) {
        icon = new GIcon(iconType, pos, rotDeg);
        if (icon) {
            mIcons[mNumIcons++] = icon;
            if (disposable) {
                icon->MarkDisposable();
            }
        }
    }

    return icon;
}

void GManager::FreeIconAt(unsigned int index) {
    if (mNumIcons == 0) {
        return;
    }

    GIcon *icon = mIcons[index];
    if (icon) {
        icon->ClearGPSing();
        delete icon;
        mIcons[index] = nullptr;
    }

    if (index + 1 < mNumIcons) {
        if (index + 1 < mNumVisibleIcons) {
            mIcons[index] = mIcons[mNumVisibleIcons - 1];
            if (mNumVisibleIcons < mNumIcons) {
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

struct IconSort {
    GIcon *icon;
    int distance;
};

static int CompareVisibleIcons(const void *lhs, const void *rhs) {
    return reinterpret_cast<const IconSort *>(lhs)->distance - reinterpret_cast<const IconSort *>(rhs)->distance;
}

int GManager::GatherVisibleIcons(GIcon **iconArray, IPlayer *player) {
    UMath::Vector3 playerPos = UMath::Vector3::kZero;
    IconSort iconSort[200];
    ISimable *simable = nullptr;

    if (player) {
        simable = player->GetSimable();
    }

    if (simable) {
        const UMath::Vector3 &pos = simable->GetPosition();

        playerPos.x = pos.z;
        playerPos.y = -pos.x;
        playerPos.z = pos.y;
    }

    int count = 0;
    for (unsigned int i = 0; i < mNumVisibleIcons; i++) {
        GIcon *icon = mIcons[i];
        bool show = false;

        if (icon->IsFlagSet(1)) {
            if (icon->IsFlagSet(2)) {
                show = true;
            }
        }

        if (show) {
            iconSort[count].icon = icon;
            if (!simable) {
                iconSort[count].distance = 0;
            } else {
                iconSort[count].distance = static_cast<int>(VU0_v3distancesquarexz(icon->GetPosition(), playerPos));
            }
            count++;
        }
    }

    if (simable) {
        qsort(iconSort, count, sizeof(IconSort), CompareVisibleIcons);
    }

    for (int i = 0; i < count; i++) {
        iconArray[i] = iconSort[i].icon;
    }

    return count;
}

bool GManager::AddIconForTrackMarker(TrackPositionMarker *marker, unsigned int tag) {
    if (marker->NameHash == tag) {
        UMath::Vector3 pos;

        pos.x = marker->Position.x;
        pos.y = marker->Position.y;
        pos.z = marker->Position.z;

        GIcon *icon = GManager::Get().AllocIcon(GIcon::kType_PursuitBreaker, pos, 0.0f, true);
        if (icon) {
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

void GManager::FindBountySpawnPoints() {
    Attrib::Gen::gameplay gameplay(Attrib::FindCollection(Attrib::Gen::gameplay::ClassKey(), 0x3D48E303), 0, nullptr);
    AttribKeyList keys;
    AttribKeyList::iterator it;

    GatherInstanceKeys(gameplay, keys, 0xA7BCCF63);
    mNumBountySpawnPoints = 0;
    for (it = keys.begin(); it != keys.end(); ++it) {
        mBountySpawnPoint[mNumBountySpawnPoints++] = *it;
        if (mNumBountySpawnPoints > 0x13) {
            break;
        }
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

void GManager::NotifyPursuitEnded(bool evaded) {
    for (GMilestone *availMile = GetFirstMilestone(true, 0); availMile;
         availMile = GetNextMilestone(availMile, true, 0)) {
        availMile->NotifyPursuitOver(evaded);
    }

    if (evaded && GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming) {
        new EAutoSave();
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

void GManager::RecursivePreloadCharacterCars(GRuntimeInstance *instance, bool forcePreload) {
    if (instance->GetType() == kGameplayObjType_Character) {
        const char *const *carNamePtr = reinterpret_cast<const char *const *>(instance->GetAttributePointer(0xF833C06F, 0));
        const char *const *stockCarNamePtr;
        const int *preloadPtr;
        const char *carName;
        const char *stockCarName;

        if (!carNamePtr) {
            carNamePtr = reinterpret_cast<const char *const *>(Attrib::DefaultDataArea(sizeof(const char *)));
        }

        carName = *carNamePtr;
        stockCarNamePtr = reinterpret_cast<const char *const *>(instance->GetAttributePointer(0xFD3CF790, 0));
        if (!stockCarNamePtr) {
            stockCarNamePtr = reinterpret_cast<const char *const *>(Attrib::DefaultDataArea(sizeof(const char *)));
        }

        stockCarName = *stockCarNamePtr;
        preloadPtr = reinterpret_cast<const int *>(instance->GetAttributePointer(0x9652AF0F, 0));
        if (!preloadPtr) {
            preloadPtr = reinterpret_cast<const int *>(Attrib::DefaultDataArea(sizeof(int)));
        }

        if (stockCarName && *stockCarName) {
            carName = stockCarName;
        }

        if (*preloadPtr != 0 || forcePreload) {
            ReserveStockCar(carName);
        }
    }

    unsigned int i = 0;

    while (true) {
        Attrib::Attribute children = instance->Get(0x916E0E78);
        unsigned int numChildren = children.GetLength();

        children.~Attribute();
        if (numChildren <= i) {
            break;
        }

        const unsigned int *childKey = reinterpret_cast<const unsigned int *>(instance->GetAttributePointer(0x916E0E78, i));

        if (!childKey) {
            childKey = reinterpret_cast<const unsigned int *>(Attrib::DefaultDataArea(sizeof(unsigned int)));
        }

        GRuntimeInstance *child = FindInstance(*childKey);
        if (child) {
            RecursivePreloadCharacterCars(child, forcePreload);
        }

        i += 1;
    }
}

void GManager::PreloadStockCarsForActivity(GActivity *activity) {
    GRaceParameters *raceParms;
    bool forcePreload;

    ClearStockCars();
    if (activity) {
        forcePreload = false;
        raceParms = GRaceDatabase::Get().GetRaceFromActivity(activity);

        if (raceParms) {
            forcePreload = raceParms->GetRaceType() == GRace::kRaceType_Drag;
        }

        if (gVerboseTesterOutput && raceParms) {
            raceParms->GetEventID();
        }

        RecursivePreloadCharacterCars(activity, forcePreload);
    }
}

void GManager::ClearStockCars() {
    mStockCars.clear();
}

void GManager::ReserveStockCar(const char *carName) {
    unsigned int carHash;
    StockCarMap::iterator iterExisting;

    if (!carName || !*carName) {
        return;
    }

    carHash = Attrib::StringHash32(carName);
    iterExisting = mStockCars.find(carHash);
    if (iterExisting == mStockCars.end()) {
        UMath::Vector3 pos = UMath::Vector3::kZero;
        UMath::Vector3 dir = UMath::Vector3::kZero;
        IVehicleCache *cache = static_cast<IVehicleCache *>(this);
        VehicleParams params(cache, DRIVER_TRAFFIC, Attrib::StringToKey(carName), dir, pos, 0, nullptr, nullptr);
        ISimable *simable = UTL::COM::Factory<Sim::Param, ISimable, UCrc32>::CreateInstance("PVehicle", params);

        if (simable) {
            IVehicle *vehicle;

            if (simable->QueryInterface(&vehicle)) {
                vehicle->Deactivate();
            }

            mStockCars[carHash] = simable;
        }
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
    typedef UTL::Std::vector<StockCarMap::iterator, _type_vector> CandidateCars;

    CandidateCars candidates;
    GRaceParameters *raceParms = GRaceStatus::Exists() ? GRaceStatus::Get().GetRaceParameters() : nullptr;
    ISimable *stockCar = nullptr;

    if (raceParms && raceParms->GetRaceType() != 2) {
        return nullptr;
    }

    candidates.reserve(mStockCars.size());
    for (StockCarMap::iterator it = mStockCars.begin(); it != mStockCars.end(); ++it) {
        IArticulatedVehicle *iarticulation;

        if (!it->second || !it->second->QueryInterface(&iarticulation)) {
            candidates.push_back(it);
        }
    }

    if (!candidates.empty()) {
        StockCarMap::iterator it = candidates[bRandom(static_cast<int>(candidates.size()))];
        stockCar = it->second;
        mStockCars.erase(it);
    }

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
    IPlayer *player;

    if (Speech::Manager::m_speechDisable != 0 || TheGameFlowManager.GetState() != GAMEFLOW_STATE_RACING ||
        UTL::Collections::Singleton<INIS>::Get() != nullptr || PhotoFinishScreen::mActive != 0 ||
        !GRaceStatus::Exists() || GRaceStatus::Get().GetPlayMode() != GRaceStatus::kPlayMode_Roaming) {
        return false;
    }

    if (!FEManager::IsOkayToRequestPauseSimulation(0, true, true)) {
        return false;
    }

    if (Speech::Manager::IsCopSpeechPlaying(0xCC)) {
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
    if (!ihud) {
        return false;
    }

    if (!ihud->AreResourcesLoaded()) {
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

    return !ivehicleai->GetPursuit();
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
    unsigned char *srcStart;
    SavedGameplayDataHeader *gameplayHeader;
    unsigned char *startChecksum;
    unsigned int bytesToChecksum;
    MD5 md5;
    unsigned int spotBytes;
    unsigned int binBytesRead;
    unsigned int respawnMarker;
    unsigned int onBlock;

    if (maxSize < 0x80) {
        return false;
    }

    srcStart = src;
    gameplayHeader = reinterpret_cast<SavedGameplayDataHeader *>(srcStart);
    startChecksum = srcStart + 0x10;
    bytesToChecksum = maxSize - 0x10;
    md5.Reset();
    md5.Update(startChecksum, bytesToChecksum);
    md5.GetRaw();
    if (bMemCmp(srcStart, md5.GetRaw(), md5.GetRawLength()) != 0 || gameplayHeader->mMagic != 0x656D6147 ||
        gameplayHeader->mVersion < 8) {
        return false;
    }

    ResetAllGameplayData();

    src += 0x80;
    for (onBlock = 0; onBlock < gameplayHeader->mNumPersistent; ++onBlock) {
        ObjectStateBlockHeader *header = reinterpret_cast<ObjectStateBlockHeader *>(src);
        unsigned int allocSize = header->mSize;
        unsigned char *newBlock = reinterpret_cast<unsigned char *>(AllocObjectStateBlock(header->mKey, allocSize, true));

        if (newBlock) {
            bMemCpy(newBlock, header + 1, header->mSize);
        }

        src += (allocSize + 0x17U) & ~0xFU;
    }

    src = reinterpret_cast<unsigned char *>((reinterpret_cast<unsigned int>(src) + 0xFU) & ~0xFU);
    LoadTimerInfo(reinterpret_cast<SavedTimerInfo *>(src), gameplayHeader->mNumSavedTimers);
    src += gameplayHeader->mNumSavedTimers * 0x20;

    LoadMilestoneInfo(reinterpret_cast<MilestoneTypeInfo *>(src), gameplayHeader->mNumMilestoneTypes);
    src += gameplayHeader->mNumMilestoneTypes * 0x10;

    LoadMilestones(reinterpret_cast<GMilestone *>(src), gameplayHeader->mNumMilestoneRecords);
    src += gameplayHeader->mNumMilestoneRecords * 0x14;
    src = reinterpret_cast<unsigned char *>((reinterpret_cast<unsigned int>(src) + 0xFU) & ~0xFU);

    LoadSpeedTraps(reinterpret_cast<GSpeedTrap *>(src), gameplayHeader->mNumSpeedTrapRecords);
    src += gameplayHeader->mNumSpeedTrapRecords * 0x14;
    src = reinterpret_cast<unsigned char *>((reinterpret_cast<unsigned int>(src) + 0xFU) & ~0xFU);

    spotBytes = (gameplayHeader->mNumHidingSpotFlags + 7U) >> 3;
    bMemCpy(mHidingSpotFound, src, spotBytes);
    src += spotBytes;
    src = reinterpret_cast<unsigned char *>((reinterpret_cast<unsigned int>(src) + 0xFU) & ~0xFU);

    binBytesRead = GRaceDatabase::Get().DeserializeBins(src);
    src += binBytesRead;
    src = reinterpret_cast<unsigned char *>((reinterpret_cast<unsigned int>(src) + 0xFU) & ~0xFU);

    LoadSMSInfo(reinterpret_cast<int *>(src), gameplayHeader->mNumPendingSMS);
    src += gameplayHeader->mNumPendingSMS * sizeof(int);
    src = reinterpret_cast<unsigned char *>((reinterpret_cast<unsigned int>(src) + 0xFU) & ~0xFU);

    bMemCpy(&respawnMarker, src, sizeof(respawnMarker));
    {
        Attrib::Gen::gameplay testInstance(respawnMarker, 0, nullptr);

        if (testInstance.IsValid()) {
            Get().SetFreeRoamStartMarker(respawnMarker);
        }
    }

    bMemCpy(&respawnMarker, src + 0x10, sizeof(respawnMarker));
    {
        Attrib::Gen::gameplay testInstance2(respawnMarker, 0, nullptr);

        if (testInstance2.IsValid()) {
            Get().SetFreeRoamFromSafeHouseStartMarker(respawnMarker);
        }
    }

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

void GManager::GetPlayerPursuitInterfaces(IPursuit *&pursuit, IPerpetrator *&perpetrator) {
    IPlayer *player = IPlayer::First(PLAYER_LOCAL);
    ISimable *simable = player ? player->GetSimable() : nullptr;
    IVehicleAI *vehicleAI = nullptr;

    if (simable) {
        simable->QueryInterface(&vehicleAI);
    }

    if (!vehicleAI) {
        pursuit = nullptr;
    } else {
        pursuit = vehicleAI->GetPursuit();
    }

    perpetrator = nullptr;
    if (simable) {
        simable->QueryInterface(&perpetrator);
    }
}

void GManager::OnRemovedVehicleCache(IVehicle *ivehicle) {
    for (StockCarMap::iterator it = mStockCars.begin(); it != mStockCars.end(); ++it) {
        if (UTL::COM::ComparePtr(it->second, ivehicle)) {
            mStockCars.erase(it);
            return;
        }
    }
}

eVehicleCacheResult GManager::OnQueryVehicleCache(const IVehicle *removethis, const IVehicleCache *whosasking) const {
    GManager *self = const_cast<GManager *>(this);

    for (StockCarMap::iterator it = self->mStockCars.begin(); it != self->mStockCars.end(); ++it) {
        IVehicle *stockCar = nullptr;

        if (it->second && it->second->QueryInterface(&stockCar) && UTL::COM::ComparePtr(stockCar, removethis)) {
            if (!UTL::COM::ComparePtr(whosasking, INIS::Get())) {
                return VCR_WANT;
            }

            self->mStockCars.erase(it);
            return VCR_DONTCARE;
        }
    }

    for (GCharacterList::const_iterator it = mActiveCharacters.begin(); it != mActiveCharacters.end(); ++it) {
        GCharacter *character = *it;
        IVehicle *vehicle = character->GetSpawnedVehicle();

        if (!UTL::COM::ComparePtr(vehicle, removethis)) {
            continue;
        }

        if (vehicle && character->IsFlagSet(GCharacter::kCharFlag_UsingStockCar)) {
            return VCR_DONTCARE;
        }

        if (UTL::COM::ComparePtr(whosasking, ITrafficMgr::Get()) || vehicle->GetOffscreenTime() == 0.0f) {
            return VCR_DONTCARE;
        }

        return character == mActiveCharacters.front() ? VCR_DONTCARE : VCR_WANT;
    }

    return VCR_DONTCARE;
}

void GManager::SuspendAllBinActivities() {
    for (unsigned int i = 0; i < GRaceDatabase::Get().GetBinCount(); ++i) {
        GRaceBin *bin = GRaceDatabase::Get().GetBin(i);

        if (bin) {
            GActivity *activity = static_cast<GActivity *>(FindInstance(bin->GetCollectionKey()));

            if (activity && activity->GetIsRunning()) {
                activity->SerializeVars(false);
                activity->Suspend();
            }
        }
    }
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

void GManager::UpdateTriggerAvailability() {
    IPursuit *pursuit;
    IPerpetrator *perpetrator;
    bool roaming;
    bool cooldown;
    bool inPursuit;

    pursuit = nullptr;
    perpetrator = nullptr;
    roaming = GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming;
    GetPlayerPursuitInterfaces(pursuit, perpetrator);

    cooldown = false;
    inPursuit = pursuit != nullptr;
    if (inPursuit) {
        cooldown = pursuit->GetPursuitStatus() == 2;
    }

    mAllowMenuGates = roaming && !inPursuit;
    mAllowEngageEvents = roaming && !inPursuit;
    mAllowEngageSafehouse = roaming && (!inPursuit || cooldown);
}

void GManager::UpdateIconVisibility() {
    bool visible[GIcon::kType_Count];

    mEventIconsShown = mAllowEngageEvents;
    mMenuGateIconsShown = mAllowMenuGates;
    mSpeedTrapIconsShown = mAllowEngageEvents;
    mSpeedTrapRaceIconsShown =
        GRaceStatus::Exists() && GRaceStatus::Get().GetRaceParameters() &&
        GRaceStatus::Get().GetRaceParameters()->GetRaceType() == GRace::kRaceType_SpeedTrap;

    for (unsigned int i = 0; i < GIcon::kType_Count; ++i) {
        visible[i] = TWEAK_ShowAllGameplayIcons != 0;
    }

    if (!TWEAK_ShowAllGameplayIcons && !INIS::Exists()) {
        visible[GIcon::kType_RaceSprint] = mEventIconsShown;
        visible[GIcon::kType_RaceCircuit] = mEventIconsShown;
        visible[GIcon::kType_RaceDrag] = mEventIconsShown;
        visible[GIcon::kType_RaceKnockout] = mEventIconsShown;
        visible[GIcon::kType_RaceTollbooth] = mEventIconsShown;
        visible[GIcon::kType_RaceSpeedtrap] = mEventIconsShown;
        visible[GIcon::kType_RaceRival] = mEventIconsShown;
        visible[GIcon::kType_GateSafehouse] = mAllowEngageSafehouse;
        visible[GIcon::kType_GateCarLot] = mMenuGateIconsShown;
        visible[GIcon::kType_GateCustomShop] = mMenuGateIconsShown;
        visible[GIcon::kType_HidingSpot] = mHidingSpotIconsShown;
        visible[GIcon::kType_PursuitBreaker] = mPursuitBreakerIconsShown;
        visible[GIcon::kType_SpeedTrap] = mSpeedTrapIconsShown;
        visible[GIcon::kType_SpeedTrapInRace] = mSpeedTrapRaceIconsShown;
        visible[GIcon::kType_AreaUnlock] = true;
    }

    mNumVisibleIcons = 0;
    for (unsigned int i = 0; i < mNumIcons; ++i) {
        GIcon *icon = mIcons[i];
        bool shouldShow = icon->IsFlagSet(1) && visible[icon->GetType()];

        if (shouldShow) {
            if (icon->IsFlagClear(8)) {
                icon->Enable();
            }

            GIcon *swap = mIcons[mNumVisibleIcons];
            mIcons[mNumVisibleIcons] = mIcons[i];
            mIcons[i] = swap;
            mNumVisibleIcons++;
        } else if (icon->IsFlagSet(8)) {
            icon->Disable();
        }
    }
}

bool GManager::CalcMapCoordsForMarker(unsigned int markerKey, bVector2 &outPos, float &outRotDeg) {
    Attrib::Gen::gameplay marker(markerKey, 0, nullptr);

    if (!marker.IsValid()) {
        return false;
    }

    const UMath::Vector3 &pos = marker.Position(0);
    TrackInfo *trackInfo = TrackInfo::GetTrackInfo(2000);
    bVector2 worldPos(pos.x, pos.y);
    UMath::Matrix4 rotMat;
    UMath::Vector3 forwardVec = {0.0f, 0.0f, 1.0f};

    Minimap::ConvertPos(worldPos, outPos, trackInfo);
    UMath::Init(rotMat, 1.0f, 1.0f, 1.0f);
    UMath::MultYRot(rotMat, -marker.Rotation(0) * 0.0027777778f, rotMat);
    UMath::Rotate(forwardVec, rotMat, forwardVec);
    outRotDeg = bAngToDeg(bATan(-forwardVec.x, forwardVec.z));
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

void GManager::NotifyWorldService() {
    if (!mWarping) {
        return;
    }

    if (TheTrackStreamer.IsLoadingInProgress() == 1) {
        return;
    }

    if (mWarpTargetMarker != 0) {
        GMarker *marker = static_cast<GMarker *>(FindInstance(mWarpTargetMarker));
        const UMath::Vector3 &position = marker->GetPosition();
        const UMath::Vector3 &direction = marker->GetDirection();
        IVehicle *vehicle = nullptr;

        if (IPlayer::First(PLAYER_LOCAL)->GetSimable()->QueryInterface(&vehicle)) {
            vehicle->SetVehicleOnGround(position, direction);
            vehicle->Activate();
        }

        mWarpTargetMarker = 0;
        if (mWarpStartPursuit) {
            World_RestoreProps();
            GPS_Disengage();
            ITrafficMgr::Get()->FlushAllTraffic(true);
            GCopMgrCompat::Get()->ResetCopsForRestart(true);
            GCopMgrCompat::Get()->LockoutCops(false);
            GCopMgrCompat::Get()->PursueAtHeatLevel(1);
        }
    }

    if (mWarpStartPursuit) {
        if (GCopMgrCompat::Get()->IsCopSpawnPending()) {
            mWarpStartPursuit = false;
        } else {
            GCopMgrCompat::Get()->PursueAtHeatLevel(1);
            goto done;
        }
    }

    mWarping = false;

done:
    if (!mWarping) {
        new EFadeScreenOff(0x161A918);
    }
}

bool GManager::WarpToMarker(unsigned int markerKey, bool startPursuit) {
    if (GRaceStatus::Exists() && GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing) {
        return false;
    }

    if (mWarping) {
        return false;
    }

    GMarker *marker = static_cast<GMarker *>(FindInstance(markerKey));
    if (!marker) {
        return false;
    }

    new EFadeScreenOn(false);
    Sim::SetStream(const_cast<UMath::Vector3 &>(marker->GetPosition()), false);

    IPlayer *player = IPlayer::First(PLAYER_LOCAL);
    ISimable *simable = player ? player->GetSimable() : nullptr;
    IVehicle *vehicle = nullptr;

    if (simable) {
        simable->QueryInterface(&vehicle);
    }

    if (vehicle) {
        vehicle->SetVehicleOnGround(marker->GetPosition(), marker->GetDirection());
    }

    mWarpStartPursuit = startPursuit;
    mWarping = true;
    mWarpTargetMarker = markerKey;
    return true;
}

unsigned int GManager::GetRespawnMarker() {
    unsigned int markerKey = mOverrideFreeRoamStartMarker;
    const Attrib::Collection *collection = Attrib::FindCollection(Attrib::Gen::gameplay::ClassKey(), markerKey);

    if (!collection) {
        markerKey = mStartFreeRoamFromSafeHouse ? mFreeRoamFromSafeHouseStartMarker : mFreeRoamStartMarker;
        collection = Attrib::FindCollection(Attrib::Gen::gameplay::ClassKey(), markerKey);

        if (!collection) {
            char markerName[32];

            bSPrintf(markerName, "career_start_%s", LoadedTrackInfo->GetLoadedTrackInfo());
            markerKey = Attrib::StringToLowerCaseKey(markerName);
            mFreeRoamStartMarker = markerKey;
        }
    }

    return markerKey;
}

void GManager::GetRespawnLocation(UMath::Vector3 &startLoc, UMath::Vector3 &initialVec) {
    Attrib::Gen::gameplay gameplayObj(GetRespawnMarker(), 0, nullptr);
    UMath::Vector3 position = gameplayObj.Position(0);
    UMath::Matrix4 rotMat;
    UMath::Vector3 forward = {0.0f, 0.0f, 1.0f};

    startLoc.x = -position.y;
    startLoc.y = position.z;
    startLoc.z = position.x;

    UMath::MultYRot(UMath::Matrix4::kIdentity, -gameplayObj.Rotation(0) * 0.00069444446f, rotMat);
    VU0_MATRIX3x4_vect3mult(forward, rotMat, initialVec);
}
