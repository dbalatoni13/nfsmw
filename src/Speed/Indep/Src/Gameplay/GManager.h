#ifndef GAMEPLAY_GMANAGER_H
#define GAMEPLAY_GMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "GCharacter.h"
#include "GMilestone.h"
#include "GSpeedTrap.h"
#include "GTimer.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"
#include "Speed/Indep/Src/Misc/bFile.hpp"

struct _type_ID_StockCarMap {
    const char *name() {
        return "ID_StockCarMap";
    }
};

struct _type_ID_MilestoneInfoMap {
    const char *name() {
        return "ID_MilestoneInfoMap";
    }
};

struct _type_ID_PendingSMSList {
    const char *name() {
        return "ID_PendingSMSList";
    }
};

// total size: 0x8
struct ObjectStateBlockHeader {
    // Members
    unsigned int mKey;  // offset 0x0, size 0x4
    unsigned int mSize; // offset 0x4, size 0x4
};

struct _type_ID_ObjectStateMap {
    const char *name() {
        return "ID_ObjectStateMap";
    }
};

typedef UTL::Std::vector<GCharacter *, _type_ID_GCharacterList> GCharacterList;
typedef UTL::Std::map<unsigned int, MilestoneTypeInfo, _type_ID_MilestoneInfoMap> MilestoneInfoMap;
typedef UTL::Std::map<unsigned int, ISimable *, _type_ID_StockCarMap> StockCarMap;
typedef UTL::Std::map<unsigned int, ObjectStateBlockHeader *, _type_ID_ObjectStateMap> ObjectStateMap;
typedef UTL::Std::list<int, _type_ID_PendingSMSList> PendingSMSList;

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

    GManager(const char *vaultPackName);
    void InitializeRaceStreaming();
    void PreBeginGameplay();
    void BeginGameplay();
    void EndGameplay();
    void Update(float dT);
    void RefreshWorldParticleEffects();
    void GetRespawnLocation(UMath::Vector3 &startLoc, UMath::Vector3 &initialVec);
    void RestorePursuitBreakerIcons(int sectionID);
    void NotifyWorldService();

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

  private:
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
