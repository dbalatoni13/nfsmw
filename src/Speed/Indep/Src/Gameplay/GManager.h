#ifndef GAMEPLAY_GMANAGER_H
#define GAMEPLAY_GMANAGER_H

#include "Speed/Indep/Src/Gameplay/GIcon.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Misc/AttribVaultPack.h"
#include "Speed/Indep/Src/World/TrackPositionMarker.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "GActivity.h"
#include "GCharacter.h"
#include "GMilestone.h"
#include "GRaceDatabase.h"
#include "GSpeedTrap.h"
#include "GTimer.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"
#include "Speed/Indep/Src/Misc/bFile.hpp"

DECLARE_CONTAINER_TYPE(ID_StockCarMap);
DECLARE_CONTAINER_TYPE(ID_MilestoneInfoMap);
DECLARE_CONTAINER_TYPE(ID_PendingSMSList);

// total size: 0x8
struct ObjectStateBlockHeader {
    // Members
    Attrib::Key mKey; // offset 0x0, size 0x4
    uint32 mSize;     // offset 0x4, size 0x4
};

DECLARE_CONTAINER_TYPE(ID_ObjectStateMap);
DECLARE_CONTAINER_TYPE(ID_AttribKeyList);

typedef UTL::Std::vector<GCharacter *, _type_ID_GCharacterList> GCharacterList;
typedef UTL::Std::map<uint32, MilestoneTypeInfo, _type_ID_MilestoneInfoMap> MilestoneInfoMap;
typedef UTL::Std::map<uint32, ISimable *, _type_ID_StockCarMap> StockCarMap;
typedef UTL::Std::map<uint32, ObjectStateBlockHeader *, _type_ID_ObjectStateMap> ObjectStateMap;
typedef UTL::Std::list<int, _type_ID_PendingSMSList> PendingSMSList;
typedef UTL::Std::list<uint32, _type_ID_AttribKeyList> AttribKeyList;

// total size: 0x308
class GManager : public UTL::COM::Object, public IVehicleCache {
  public:
    // total size: 0x8
    struct HashEntry {
        uint32 mKey32;               // offset 0x0, size 0x4
        GRuntimeInstance *mInstance; // offset 0x4, size 0x4
    };

    // total size: 0x3C
    struct SavedGameplayDataHeader {
        uint8 mChecksum[16];         // offset 0x0, size 0x10
        uint32 mMagic;               // offset 0x10, size 0x4
        uint32 mVersion;             // offset 0x14, size 0x4
        uint32 mNumPersistent;       // offset 0x18, size 0x4
        uint32 mNumSavedTimers;      // offset 0x1C, size 0x4
        uint32 mNumMilestoneTypes;   // offset 0x20, size 0x4
        uint32 mNumMilestoneRecords; // offset 0x24, size 0x4
        uint32 mNumSpeedTrapRecords; // offset 0x28, size 0x4
        uint32 mNumUnlockedGates;    // offset 0x2C, size 0x4
        uint32 mNumHidingSpotFlags;  // offset 0x30, size 0x4
        uint32 mNumBytesBinStats;    // offset 0x34, size 0x4
        uint32 mNumPendingSMS;       // offset 0x38, size 0x4
    };

    static void Init(const char *vaultPackName);
    static void Shutdown();
    static GManager &Get() {
        return *mObj;
    }
    static bool Exists() {
        return mObj != nullptr;
    }
    void InitializeRaceStreaming();
#ifndef EA_BUILD_A124
    void PreBeginGameplay();
#endif
    void BeginGameplay();
    void EndGameplay();
    bool GetInGameplay() const;
    void Update(float dT);
    void NotifyWorldService();
    void SetFreeRoamStartMarker(Attrib::Key markerKey) {
        mFreeRoamStartMarker = markerKey;
    }
    Attrib::Key GetFreeRoamStartMarker() const {
        return mFreeRoamStartMarker;
    }
    void SetFreeRoamFromSafeHouseStartMarker(Attrib::Key markerKey) {
        mFreeRoamFromSafeHouseStartMarker = markerKey;
    }
    Attrib::Key GetFreeRoamFromSafeHouseStartMarker() const {
        return mFreeRoamFromSafeHouseStartMarker;
    }
    void SetStartingFreeRoamFromSafeHouse() {
        mStartFreeRoamFromSafeHouse = true;
    }
    void OverrideFreeRoamStartMarker(Attrib::Key markerKey) {
        mOverrideFreeRoamStartMarker = markerKey;
    }
    Attrib::Key GetRespawnMarker();
    void GetRespawnLocation(UMath::Vector3 &startLoc, UMath::Vector3 &initialVec);
    void QueueFreeRoamPursuit(float minHeat) {
        mStartFreeRoamPursuit = true;
        mQueuedPursuitMinHeat = minHeat;
    }
    void ClearQueuedFreeRoamPursuit() {
        mStartFreeRoamPursuit = false;
    }
    bool GetStartFreeRoamPursuit() {
        return mStartFreeRoamPursuit;
    }
    inline float GetQueuedPursuitHeat() {
        return mQueuedPursuitMinHeat;
    }
    bool GetIsWarping() const {
        return mWarping;
    }
    bool GetAllowEngageEvents() const {
        return mAllowEngageEvents;
    }
    bool GetAllowMenuGates() const {
        return mAllowMenuGates;
    }
    bool GetAllowEngageSafehouse() const {
        return mAllowEngageSafehouse;
    }
    uint32 Get32BitCollectionKey(Attrib::Key collectionKey) const {
        return collectionKey >> mCollectionKeyShiftTo32;
    }
    uint32 Get24BitAttributeKey(Attrib::Key attribKey) const {
        return attribKey >> mAttributeKeyShiftTo24;
    }
    bool CalcMapCoordsForMarker(Attrib::Key markerKey, bVector2 &outPos, float &outRotDeg);
    void *AllocObjectStateBlock(Attrib::Key collectionKey, unsigned int size, bool persistent);
    void *GetObjectStateBlock(Attrib::Key collectionKey);
    void ClearObjectStateBlock(Attrib::Key collectionKey);
    bool SaveGameplayData(uint8 *dest, unsigned int maxSize);
    bool LoadGameplayData(uint8 *src, unsigned int maxSize);
    void ResetAllGameplayData();
#ifndef EA_BUILD_A124
    void StartWorldActivities(bool startFreeRoamOnly);
    void StartBinActivity(GRaceBin *raceBin);
    void SuspendAllBinActivities();
#endif
    void SuspendAllActivities();
    void StartRaceFromInGame(uint32 raceHash);
    bool WarpToMarker(Attrib::Key markerKey, bool startPursuit);
    void RefreshWorldParticleEffects();
    bool GetHidingSpotFound(unsigned int index);
    void SetHidingSpotFound(unsigned int index, bool found);
    void TrackValue(const char *valueName, float value);
    void TrackValue(const char *valueName, int value) {
        TrackValue(valueName, static_cast<float>(value));
    }
    void IncValue(const char *valueName);
    float GetValue(const char *valueName);
    float GetValue(Attrib::Key valueKey);
    float GetBestValue(const char *valueName);
    float GetBestValue(Attrib::Key valueKey);
    bool GetIsBiggerValueBetter(Attrib::Key valueKey);
    unsigned int GetNumMilestones();
    GMilestone *GetMilestone(unsigned int index);
    GMilestone *GetFirstMilestone(bool availOnly, unsigned int binNumber);
    GMilestone *GetNextMilestone(GMilestone *current, bool availOnly, unsigned int binNumber);
    void EnableBinMilestones(unsigned int binNumber);
    void NotifyPursuitStarted();
    void NotifyPursuitEnded(bool evaded);
    unsigned int GetNumSpeedTraps();
    GSpeedTrap *GetSpeedTrap(unsigned int index);
    GSpeedTrap *GetFirstSpeedTrap(bool activeOnly, unsigned int binNumber);
    GSpeedTrap *GetNextSpeedTrap(GSpeedTrap *current, bool activeOnly, unsigned int binNumber);
    void EnableBinSpeedTraps(unsigned int binNumber);
    void RefreshSpeedTrapIcons();
    unsigned int GetNumBountySpawnMarkers() const;
    Attrib::Key GetBountySpawnMarker(unsigned int index) const;
    int GetBountySpawnMarkerTag(unsigned int index) const;
    void AttachCharacter(GCharacter *character);
    void DetachCharacter(GCharacter *character);
    void UnspawnAllCharacters();
    void PreloadStockCarsForActivity(GActivity *activity);
    void ReserveStockCar(const char *carName);
    bool StockCarsLoaded();
    void ClearStockCars();
    ISimable *GetStockCar(const char *carName);
    ISimable *GetRandomEmergencyStockCar();
    void ReleaseStockCar(ISimable *stockCar);
    void RegisterInstance(GRuntimeInstance *instance);
    void UnregisterInstance(GRuntimeInstance *instance);
    GVault *FindVault(const char *vaultName);
    GVault *FindVaultContaining(Attrib::Key collectionKey);
    void LoadVaultSync(GVault *vault);
    eVehicleCacheResult OnQueryVehicleCache(const IVehicle *removethis, const IVehicleCache *whosasking) const override;
    void OnRemovedVehicleCache(IVehicle *ivehicle) override;
    GRuntimeInstance *FindInstance(Attrib::Key key) const;
    bool SetTimer(const char *name, float interval);
    void KillTimer(const char *name);
    void AddSMS(int smsID);
    int PushSMSToInbox();
    bool GetHasPendingSMS() const;
    void RefreshZoneIcons();
    void RefreshTrackMarkerIcons();
    void RefreshEngageTriggerIcons();
    void HidePursuitBreakerIcon(const UMath::Vector3 &pos, float radius);
    void RestorePursuitBreakerIcons(int sectionID);
    GIcon *AllocIcon(GIcon::Type iconType, const UMath::Vector3 &pos, float rotDeg, bool disposable);
    void FreeIcon(GIcon *icon);
    int GatherVisibleIcons(GIcon **iconArray, IPlayer *player);
    bool GetIsIconVisible(GIcon *icon);

#ifndef EA_BUILD_A124
    const char *GetCacheName() const override {
        return "GManager";
    }
    void SetRestartEvent(uint32 eventHash) {
        mRestartEventHash = eventHash;
    }
    bool GetHasPendingRestartEvent() const {
        return mRestartEventHash != 0;
    }
#endif

  private:
    GManager(const char *vaultPackName);
    void InitializeVaults();
    void BuildVaultTable(AttribVaultPackImage *packImage);
    void LoadCoreVault(AttribVaultPackImage *packImage);
    void PreloadTransientVaults(AttribVaultPackImage *packImage);
    void FindKeyReductionShifts();
    void AllocateStreamingBuffers();
    void AllocateInstanceMap();
    void AllocateObjectStateStorage();
    void AllocateMilestones();
    void AllocateSpeedTraps();
    void AllocateIcons();
    void ReleaseIcons();
    void ReleaseSpeedTraps();
    void ReleaseMilestones();
    void ReleaseObjectStateStorage();
    void ReleaseInstanceMap();
    void ReleaseStreamingBuffers();
    void UnloadTransientVaults();
    void UnloadCoreVault();
    void DestroyVaults();
    Attrib::Key GetStrippedNameKey(const char *name);
    unsigned int FindUniqueKeyShift(Attrib::Key *keys, unsigned int numKeys, unsigned int uniqueBits);
    unsigned int GetAvailableBinSlot();
    unsigned int GetAvailableRaceSlot();
    void DefragObjectStateStorage();
    void ClearAllSessionData();
    void ConnectRuntimeInstances();
    void ConnectInstanceReferences(GRuntimeInstance *runtimeInstance, const Attrib::Gen::gameplay &collection);
    void ConnectChildren(GRuntimeInstance *runtimeInstance);
    void StartActivities();
    void UpdatePursuit();
    void ServicePendingCharacters();
    void UnspawnUselessCharacters();
    void GatherInstanceKeys(Attrib::Gen::gameplay &collection, AttribKeyList &list, Attrib::Key templateKey);
    void FindBountySpawnPoints();
    void ResetMilestoneTrackingInfo();
    void LoadMilestoneInfo(MilestoneTypeInfo *savedInfo, unsigned int count);
    unsigned int SaveMilestoneInfo(MilestoneTypeInfo *dest);
    void ResetMilestones();
    unsigned int SaveMilestones(GMilestone *dest);
    void LoadMilestones(GMilestone *src, unsigned int count);
    void ResetSpeedTraps();
    unsigned int SaveSpeedTraps(GSpeedTrap *dest);
    void LoadSpeedTraps(GSpeedTrap *src, unsigned int count);
    void UpdateTimers(float dT);
    void ResetTimers();
    unsigned int SaveTimerInfo(SavedTimerInfo *saveInfo);
    void LoadTimerInfo(SavedTimerInfo *saveInfo, unsigned int count);
    unsigned int SaveSMSInfo(int *saveInfo);
    void LoadSMSInfo(int *loadInfo, unsigned int count);
    void DispatchSMSMessage(int smsID);
    void UpdatePendingSMS();
    bool CanPlaySMS() const;
    void GetPlayerPursuitInterfaces(IPursuit *&pursuit, IPerpetrator *&perp);
    void UpdateTriggerAvailability();
    void FreeDisposableIcons(GIcon::Type iconType);
    void FreeAllIcons();
    void FreeIconAt(unsigned int index);
    void UpdateIconVisibility();
    void SpawnAllLoadedSectionIcons();
    void SpawnSectionIcons(int section);
    void UnspawnSectionIcons(int section);
    void UnspawnAllIcons();
    static bool AddIconForTrackMarker(TrackPositionMarker *marker, uint32 tag);
    static void NotifyCollisionPackLoaded(int sectionID, bool loaded);
#ifndef EA_BUILD_A124
    void RecursivePreloadCharacterCars(GRuntimeInstance *instance, bool forcePreload);
#endif

    static GManager *mObj;
    const char *mVaultPackFileName;                // offset 0x1C, size 0x4
    bFile *mVaultPackFile;                         // offset 0x20, size 0x4
    unsigned int mVaultCount;                      // offset 0x24, size 0x4
    struct GVault *mVaults;                        // offset 0x28, size 0x4
    const char *mVaultNameStrings;                 // offset 0x2C, size 0x4
    AttribVaultPackImage *mLoadingPackImage;       // offset 0x30, size 0x4
    unsigned int mBinSlotSize;                     // offset 0x34, size 0x4
    uint8 *mStreamedBinSlots;                      // offset 0x38, size 0x4
    GVault *mBinVaultInSlot[1];                    // offset 0x3C, size 0x4
    uint32 mRaceSlotSize;                          // offset 0x40, size 0x4
    uint8 *mStreamedRaceSlots;                     // offset 0x44, size 0x4
    GVault *mRaceVaultInSlot[1];                   // offset 0x48, size 0x4
    uint8 *mTempLoadData;                          // offset 0x4C, size 0x4
    int mTransientPoolNumber;                      // offset 0x50, size 0x4
    void *mTransientPoolMemory;                    // offset 0x54, size 0x4
    Attrib::Class *mGameplayClass;                 // offset 0x58, size 0x4
    Attrib::Class *mMilestoneClass;                // offset 0x5C, size 0x4
    unsigned int mAttributeKeyShiftTo24;           // offset 0x60, size 0x4
    unsigned int mCollectionKeyShiftTo32;          // offset 0x64, size 0x4
    unsigned int mMaxObjects;                      // offset 0x68, size 0x4
    void *mClassTempBuffer;                        // offset 0x6C, size 0x4
    unsigned int mInstanceHashTableSize;           // offset 0x70, size 0x4
    unsigned int mInstanceHashTableMask;           // offset 0x74, size 0x4
    unsigned int mWorstHashCollision;              // offset 0x78, size 0x4
    HashEntry *mKeyToInstanceMap;                  // offset 0x7C, size 0x4
    GCharacterList mActiveCharacters;              // offset 0x80, size 0x10
    StockCarMap mStockCars;                        // offset 0x90, size 0x10
    MilestoneInfoMap mMilestoneTypeInfo;           // offset 0xA0, size 0x10
    unsigned int mNumMilestones;                   // offset 0xB0, size 0x4
    GMilestone *mMilestones;                       // offset 0xB4, size 0x4
    unsigned int mNumSpeedTraps;                   // offset 0xB8, size 0x4
    GSpeedTrap *mSpeedTraps;                       // offset 0xBC, size 0x4
    unsigned int mNumBountySpawnPoints;            // offset 0xC0, size 0x4
    unsigned int mBountySpawnPoint[20];            // offset 0xC4, size 0x50
    Attrib::Key mFreeRoamStartMarker;              // offset 0x114, size 0x4
    Attrib::Key mFreeRoamFromSafeHouseStartMarker; // offset 0x118, size 0x4
    bool mStartFreeRoamFromSafeHouse;              // offset 0x11C, size 0x1
    bool mStartFreeRoamPursuit;                    // offset 0x120, size 0x1
    float mQueuedPursuitMinHeat;                   // offset 0x124, size 0x4
    bool mInGameplay;                              // offset 0x128, size 0x1
    unsigned int mOverrideFreeRoamStartMarker;     // offset 0x12C, size 0x4
    uint8 *mObjectStateBuffer;                     // offset 0x130, size 0x4
    uint8 *mObjectStateBufferFree;                 // offset 0x134, size 0x4
    unsigned int mObjectStateBufferSize;           // offset 0x138, size 0x4
    ObjectStateMap mPersistentStateBlocks;         // offset 0x13C, size 0x10
    ObjectStateMap mSessionStateBlocks;            // offset 0x14C, size 0x10
    GEventTimer mTimers[8];                        // offset 0x15C, size 0x120
    unsigned int mHidingSpotFound[16];             // offset 0x27C, size 0x40
    PendingSMSList mPendingSMS;                    // offset 0x2BC, size 0x8
    float mLastCouldSendTime;                      // offset 0x2C4, size 0x4
    bool mWarping;                                 // offset 0x2C8, size 0x1
    bool mWarpStartPursuit;                        // offset 0x2CC, size 0x1
    Attrib::Key mWarpTargetMarker;                 // offset 0x2D0, size 0x4
    unsigned int mNumIcons;                        // offset 0x2D4, size 0x4
    unsigned int mNumVisibleIcons;                 // offset 0x2D8, size 0x4
    GIcon **mIcons;                                // offset 0x2DC, size 0x4
    bool mPursuitBreakerIconsShown;                // offset 0x2E0, size 0x1
    bool mHidingSpotIconsShown;                    // offset 0x2E4, size 0x1
    bool mEventIconsShown;                         // offset 0x2E8, size 0x1
    bool mMenuGateIconsShown;                      // offset 0x2EC, size 0x1
    bool mSpeedTrapIconsShown;                     // offset 0x2F0, size 0x1
    bool mSpeedTrapRaceIconsShown;                 // offset 0x2F4, size 0x1
    bool mAllowEngageEvents;                       // offset 0x2F8, size 0x1
    bool mAllowEngageSafehouse;                    // offset 0x2FC, size 0x1
    bool mAllowMenuGates;                          // offset 0x300, size 0x1
    unsigned int mRestartEventHash;                // offset 0x304, size 0x4
};

void NotifyTrackMarkersChanged();
void NotifyGameZonesChanged();

#endif
