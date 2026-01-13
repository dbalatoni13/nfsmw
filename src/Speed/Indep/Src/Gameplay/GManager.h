#ifndef GAMEPLAY_GMANAGER_H
#define GAMEPLAY_GMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "GActivity.h"
#include "GCharacter.h"
#include "GMilestone.h"
#include "GSpeedTrap.h"
#include "GTimer.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"
#include "Speed/Indep/Src/Misc/bFile.hpp"

DECLARE_VECTOR_TYPE(ID_StockCarMap);
DECLARE_VECTOR_TYPE(ID_MilestoneInfoMap);
DECLARE_VECTOR_TYPE(ID_PendingSMSList);

// total size: 0x8
struct ObjectStateBlockHeader {
    // Members
    unsigned int mKey;  // offset 0x0, size 0x4
    unsigned int mSize; // offset 0x4, size 0x4
};

DECLARE_VECTOR_TYPE(ID_ObjectStateMap);
DECLARE_VECTOR_TYPE(ID_AttribKeyList);

typedef UTL::Std::vector<GCharacter *, _type_ID_GCharacterList> GCharacterList;
typedef UTL::Std::map<unsigned int, MilestoneTypeInfo, _type_ID_MilestoneInfoMap> MilestoneInfoMap;
typedef UTL::Std::map<unsigned int, ISimable *, _type_ID_StockCarMap> StockCarMap;
typedef UTL::Std::map<unsigned int, ObjectStateBlockHeader *, _type_ID_ObjectStateMap> ObjectStateMap;
typedef UTL::Std::list<int, _type_ID_PendingSMSList> PendingSMSList;
typedef UTL::Std::list<unsigned int, _type_ID_AttribKeyList> AttribKeyList;

// total size: 0x308
class GManager : public UTL::COM::Object, public IVehicleCache {
  public:
    // total size: 0x8
    struct HashEntry {
        unsigned int mKey32;         // offset 0x0, size 0x4
        GRuntimeInstance *mInstance; // offset 0x4, size 0x4
    };

    // total size: 0x3C
    struct SavedGameplayDataHeader {
        unsigned char mChecksum[16];       // offset 0x0, size 0x10
        unsigned int mMagic;               // offset 0x10, size 0x4
        unsigned int mVersion;             // offset 0x14, size 0x4
        unsigned int mNumPersistent;       // offset 0x18, size 0x4
        unsigned int mNumSavedTimers;      // offset 0x1C, size 0x4
        unsigned int mNumMilestoneTypes;   // offset 0x20, size 0x4
        unsigned int mNumMilestoneRecords; // offset 0x24, size 0x4
        unsigned int mNumSpeedTrapRecords; // offset 0x28, size 0x4
        unsigned int mNumUnlockedGates;    // offset 0x2C, size 0x4
        unsigned int mNumHidingSpotFlags;  // offset 0x30, size 0x4
        unsigned int mNumBytesBinStats;    // offset 0x34, size 0x4
        unsigned int mNumPendingSMS;       // offset 0x38, size 0x4
    };

    static void Init(const char *vaultPackName);

    void InitializeRaceStreaming();
    void PreBeginGameplay();
    void BeginGameplay();
    void EndGameplay();
    void Update(float dT);
    void RefreshWorldParticleEffects();
    void GetRespawnLocation(UMath::Vector3 &startLoc, UMath::Vector3 &initialVec);
    void RestorePursuitBreakerIcons(int sectionID);
    void NotifyWorldService();

    void TrackValue(const char *valueName, float value);
    void IncValue(const char *valueName);
    float GetValue(const char *valueName);

    void RegisterInstance(GRuntimeInstance *instance);
    void UnregisterInstance(GRuntimeInstance *instance);
    GRuntimeInstance *FindInstance(Attrib::Key key) const;
    void ConnectRuntimeInstances();

    void ConnectInstanceReferences(GRuntimeInstance *runtimeInstance, const Attrib::Gen::gameplay &collection);

    void ConnectChildren(GRuntimeInstance *runtimeInstance);
    unsigned int GetStrippedNameKey(const char *name);

    void StartActivities();
    void StartWorldActivities(bool startFreeRoamOnly);
    void StartBinActivity(struct GRaceBin *raceBin);
    void SuspendAllBinActivities();
    void SuspendAllActivities();

    void StartRaceFromInGame(unsigned int raceHash);
    bool CalcMapCoordsForMarker(unsigned int markerKey, bVector2 &outPos, float &outRotDeg);
    bool WarpToMarker(unsigned int markerKey, bool startPursuit);
    bool GetHidingSpotFound(unsigned int index);
    void SetHidingSpotFound(unsigned int index, bool found);
    void NotifyPursuitStarted();
    void NotifyPursuitEnded(bool evaded);
    void EnableBinSpeedTraps(unsigned int binNumber);
    void RefreshSpeedTrapIcons();
    void GatherInstanceKeys(Attrib::Gen::gameplay &collection, AttribKeyList &list, unsigned int templateKey);
    void FindBountySpawnPoints();
    unsigned int GetNumBountySpawnMarkers() const;
    unsigned int GetBountySpawnMarker(unsigned int index) const;
    void ServicePendingCharacters();
    void UnspawnUselessCharacters();
    unsigned int GetRespawnMarker();
    int GetBountySpawnMarkerTag(unsigned int index) const;
    void RefreshZoneIcons();
    static bool AddIconForTrackMarker(struct TrackPositionMarker *marker, unsigned int tag);
    void RefreshTrackMarkerIcons();
    void RefreshEngageTriggerIcons();
    void HidePursuitBreakerIcon(const UMath::Vector3 &pos, float radius);

    // struct GIcon *AllocIcon(enum Type iconType, const UMath::Vector3 &pos, float rotDeg, bool disposable);
    // void FreeDisposableIcons(enum Type iconType);
    void FreeIcon(struct GIcon *icon);
    void FreeIconAt(unsigned int index);
    int GatherVisibleIcons(struct GIcon **iconArray, IPlayer *player);
    bool GetIsIconVisible(struct GIcon *icon);
    void SpawnAllLoadedSectionIcons();
    void SpawnSectionIcons(int section);
    void UnspawnSectionIcons(int section);
    void UnspawnAllIcons();
    void FreeAllIcons();

    unsigned int GetNumMilestones();
    GMilestone *GetMilestone(unsigned int index);
    GMilestone *GetFirstMilestone(bool availOnly, unsigned int binNumber);
    GMilestone *GetNextMilestone(GMilestone *current, bool availOnly, unsigned int binNumber);
    void AllocateMilestones();
    void ReleaseMilestones();
    void ResetMilestoneTrackingInfo();
    void LoadMilestoneInfo(MilestoneTypeInfo *savedInfo, unsigned int count);
    unsigned int SaveMilestoneInfo(MilestoneTypeInfo *dest);
    void EnableBinMilestones(unsigned int binNumber);
    void ResetMilestones();
    unsigned int SaveMilestones(GMilestone *dest);
    void LoadMilestones(GMilestone *src, unsigned int count);

    unsigned int GetNumSpeedTraps();
    GSpeedTrap *GetSpeedTrap(unsigned int index);
    GSpeedTrap *GetFirstSpeedTrap(bool activeOnly, unsigned int binNumber);
    GSpeedTrap *GetNextSpeedTrap(GSpeedTrap *current, bool activeOnly, unsigned int binNumber);
    void AllocateSpeedTraps();
    void ReleaseSpeedTraps();
    void ResetSpeedTraps();
    unsigned int SaveSpeedTraps(GSpeedTrap *dest);
    void LoadSpeedTraps(GSpeedTrap *src, unsigned int count);

    void RecursivePreloadCharacterCars(GRuntimeInstance *instance, bool forcePreload);
    void PreloadStockCarsForActivity(GActivity *activity);
    void ReserveStockCar(const char *carName);
    bool StockCarsLoaded();
    void ClearStockCars();
    struct ISimable *GetStockCar(const char *carName);
    struct ISimable *GetRandomEmergencyStockCar();
    void ReleaseStockCar(struct ISimable *stockCar);

    bool SetTimer(const char *name, float interval);
    void KillTimer(const char *name);
    void ResetTimers();
    void UpdateTimers(float dT);
    unsigned int SaveTimerInfo(struct SavedTimerInfo *saveInfo);
    void LoadTimerInfo(struct SavedTimerInfo *saveInfo, unsigned int count);

    unsigned int SaveSMSInfo(int *saveInfo);
    void LoadSMSInfo(int *loadInfo, unsigned int count);
    bool GetHasPendingSMS() const;
    bool CanPlaySMS() const;
    void AddSMS(int smsID);
    void DispatchSMSMessage(int smsID);
    void UpdatePendingSMS();
    int PushSMSToInbox();

    static GManager &Get() {
        return *mObj;
    }

    static bool Exists() {
        return mObj != nullptr;
    }

    bool GetIsWarping() const {
        return mWarping;
    }

    bool GetStartFreeRoamPursuit() {
        return mStartFreeRoamPursuit;
    }

    void TrackValue(const char *valueName, int value) {
        TrackValue(valueName, static_cast<float>(value));
    }

  private:
    GManager(const char *vaultPackName);

    static GManager *mObj;

    const char *mVaultPackFileName;                 // offset 0x1C, size 0x4
    bFile *mVaultPackFile;                          // offset 0x20, size 0x4
    unsigned int mVaultCount;                       // offset 0x24, size 0x4
    struct GVault *mVaults;                         // offset 0x28, size 0x4
    const char *mVaultNameStrings;                  // offset 0x2C, size 0x4
    struct AttribVaultPackImage *mLoadingPackImage; // offset 0x30, size 0x4
    unsigned int mBinSlotSize;                      // offset 0x34, size 0x4
    unsigned char *mStreamedBinSlots;               // offset 0x38, size 0x4
    struct GVault *mBinVaultInSlot[1];              // offset 0x3C, size 0x4
    unsigned int mRaceSlotSize;                     // offset 0x40, size 0x4
    unsigned char *mStreamedRaceSlots;              // offset 0x44, size 0x4
    struct GVault *mRaceVaultInSlot[1];             // offset 0x48, size 0x4
    unsigned char *mTempLoadData;                   // offset 0x4C, size 0x4
    int mTransientPoolNumber;                       // offset 0x50, size 0x4
    void *mTransientPoolMemory;                     // offset 0x54, size 0x4
    Attrib::Class *mGameplayClass;                  // offset 0x58, size 0x4
    Attrib::Class *mMilestoneClass;                 // offset 0x5C, size 0x4
    unsigned int mAttributeKeyShiftTo24;            // offset 0x60, size 0x4
    unsigned int mCollectionKeyShiftTo32;           // offset 0x64, size 0x4
    unsigned int mMaxObjects;                       // offset 0x68, size 0x4
    void *mClassTempBuffer;                         // offset 0x6C, size 0x4
    unsigned int mInstanceHashTableSize;            // offset 0x70, size 0x4
    unsigned int mInstanceHashTableMask;            // offset 0x74, size 0x4
    unsigned int mWorstHashCollision;               // offset 0x78, size 0x4
    struct HashEntry *mKeyToInstanceMap;            // offset 0x7C, size 0x4
    GCharacterList mActiveCharacters;               // offset 0x80, size 0x10
    StockCarMap mStockCars;                         // offset 0x90, size 0x10
    MilestoneInfoMap mMilestoneTypeInfo;            // offset 0xA0, size 0x10
    unsigned int mNumMilestones;                    // offset 0xB0, size 0x4
    GMilestone *mMilestones;                        // offset 0xB4, size 0x4
    unsigned int mNumSpeedTraps;                    // offset 0xB8, size 0x4
    GSpeedTrap *mSpeedTraps;                        // offset 0xBC, size 0x4
    unsigned int mNumBountySpawnPoints;             // offset 0xC0, size 0x4
    unsigned int mBountySpawnPoint[20];             // offset 0xC4, size 0x50
    unsigned int mFreeRoamStartMarker;              // offset 0x114, size 0x4
    unsigned int mFreeRoamFromSafeHouseStartMarker; // offset 0x118, size 0x4
    bool mStartFreeRoamFromSafeHouse;               // offset 0x11C, size 0x1
    bool mStartFreeRoamPursuit;                     // offset 0x120, size 0x1
    float mQueuedPursuitMinHeat;                    // offset 0x124, size 0x4
    bool mInGameplay;                               // offset 0x128, size 0x1
    unsigned int mOverrideFreeRoamStartMarker;      // offset 0x12C, size 0x4
    unsigned char *mObjectStateBuffer;              // offset 0x130, size 0x4
    unsigned char *mObjectStateBufferFree;          // offset 0x134, size 0x4
    unsigned int mObjectStateBufferSize;            // offset 0x138, size 0x4
    ObjectStateMap mPersistentStateBlocks;          // offset 0x13C, size 0x10
    ObjectStateMap mSessionStateBlocks;             // offset 0x14C, size 0x10
    GEventTimer mTimers[8];                         // offset 0x15C, size 0x120
    unsigned int mHidingSpotFound[16];              // offset 0x27C, size 0x40
    PendingSMSList mPendingSMS;                     // offset 0x2BC, size 0x8
    float mLastCouldSendTime;                       // offset 0x2C4, size 0x4
    bool mWarping;                                  // offset 0x2C8, size 0x1
    bool mWarpStartPursuit;                         // offset 0x2CC, size 0x1
    unsigned int mWarpTargetMarker;                 // offset 0x2D0, size 0x4
    unsigned int mNumIcons;                         // offset 0x2D4, size 0x4
    unsigned int mNumVisibleIcons;                  // offset 0x2D8, size 0x4
    struct GIcon **mIcons;                          // offset 0x2DC, size 0x4
    bool mPursuitBreakerIconsShown;                 // offset 0x2E0, size 0x1
    bool mHidingSpotIconsShown;                     // offset 0x2E4, size 0x1
    bool mEventIconsShown;                          // offset 0x2E8, size 0x1
    bool mMenuGateIconsShown;                       // offset 0x2EC, size 0x1
    bool mSpeedTrapIconsShown;                      // offset 0x2F0, size 0x1
    bool mSpeedTrapRaceIconsShown;                  // offset 0x2F4, size 0x1
    bool mAllowEngageEvents;                        // offset 0x2F8, size 0x1
    bool mAllowEngageSafehouse;                     // offset 0x2FC, size 0x1
    bool mAllowMenuGates;                           // offset 0x300, size 0x1
    unsigned int mRestartEventHash;                 // offset 0x304, size 0x4
};

#endif
