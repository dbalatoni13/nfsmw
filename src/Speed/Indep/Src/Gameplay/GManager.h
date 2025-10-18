#ifndef GAMEPLAY_GMANAGER_H
#define GAMEPLAY_GMANAGER_H

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"
#include "Speed/Indep/Src/Misc/bFile.hpp"

class GManager : public UTL::COM::Object, public IVehicleCache {
    // total size: 0x308
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
    struct Class *mGameplayClass;                   // offset 0x58, size 0x4
    struct Class *mMilestoneClass;                  // offset 0x5C, size 0x4
    unsigned int mAttributeKeyShiftTo24;            // offset 0x60, size 0x4
    unsigned int mCollectionKeyShiftTo32;           // offset 0x64, size 0x4
    unsigned int mMaxObjects;                       // offset 0x68, size 0x4
    void *mClassTempBuffer;                         // offset 0x6C, size 0x4
    unsigned int mInstanceHashTableSize;            // offset 0x70, size 0x4
    unsigned int mInstanceHashTableMask;            // offset 0x74, size 0x4
    unsigned int mWorstHashCollision;               // offset 0x78, size 0x4
    struct HashEntry *mKeyToInstanceMap;            // offset 0x7C, size 0x4
    // struct vector<GCharacter *, _type_ID_GCharacterList> mActiveCharacters;                    // offset 0x80, size 0x10
    // struct map<unsigned int, ISimable *, _type_ID_StockCarMap> mStockCars;                     // offset 0x90, size 0x10
    // struct map<unsigned int, MilestoneTypeInfo, _type_ID_MilestoneInfoMap> mMilestoneTypeInfo; // offset 0xA0, size 0x10
    unsigned int mNumMilestones;                    // offset 0xB0, size 0x4
    struct GMilestone *mMilestones;                 // offset 0xB4, size 0x4
    unsigned int mNumSpeedTraps;                    // offset 0xB8, size 0x4
    struct GSpeedTrap *mSpeedTraps;                 // offset 0xBC, size 0x4
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
    // struct map<unsigned int, ObjectStateBlockHeader *, _type_ID_ObjectStateMap> mPersistentStateBlocks; // offset 0x13C, size 0x10
    // struct map<unsigned int, ObjectStateBlockHeader *, _type_ID_ObjectStateMap> mSessionStateBlocks;    // offset 0x14C, size 0x10
    // struct GEventTimer mTimers[8];                         // offset 0x15C, size 0x120
    unsigned int mHidingSpotFound[16]; // offset 0x27C, size 0x40
    // struct list<int, _type_ID_PendingSMSList> mPendingSMS; // offset 0x2BC, size 0x8
    float mLastCouldSendTime;       // offset 0x2C4, size 0x4
    bool mWarping;                  // offset 0x2C8, size 0x1
    bool mWarpStartPursuit;         // offset 0x2CC, size 0x1
    unsigned int mWarpTargetMarker; // offset 0x2D0, size 0x4
    unsigned int mNumIcons;         // offset 0x2D4, size 0x4
    unsigned int mNumVisibleIcons;  // offset 0x2D8, size 0x4
    struct GIcon **mIcons;          // offset 0x2DC, size 0x4
    bool mPursuitBreakerIconsShown; // offset 0x2E0, size 0x1
    bool mHidingSpotIconsShown;     // offset 0x2E4, size 0x1
    bool mEventIconsShown;          // offset 0x2E8, size 0x1
    bool mMenuGateIconsShown;       // offset 0x2EC, size 0x1
    bool mSpeedTrapIconsShown;      // offset 0x2F0, size 0x1
    bool mSpeedTrapRaceIconsShown;  // offset 0x2F4, size 0x1
    bool mAllowEngageEvents;        // offset 0x2F8, size 0x1
    bool mAllowEngageSafehouse;     // offset 0x2FC, size 0x1
    bool mAllowMenuGates;           // offset 0x300, size 0x1
    unsigned int mRestartEventHash; // offset 0x304, size 0x4

  public:
    static GManager *mObj;

    GManager(const char *vaultPackName);
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
};

#endif
