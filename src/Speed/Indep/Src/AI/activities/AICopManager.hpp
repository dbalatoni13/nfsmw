#ifndef AI_ACTIVITIES_AICOPMANAGER_H
#define AI_ACTIVITIES_AICOPMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/AI/Common/AISpawnManager.hpp"
#include "Speed/Indep/Src/Generated/Messages/MBreakerStopCops.h"
#include "Speed/Indep/Src/Generated/Messages/MForcePursuitStart.h"
#include "Speed/Indep/Src/Generated/Messages/MSetCopAutoSpawnMode.h"
#include "Speed/Indep/Src/Generated/Messages/MSetCopsEnabled.h"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ICopMgr.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/Sim/SimActivity.h"

struct _type_AICopManagerSpawnRequests {
    const char *name() {
        return "AICopManagerSpawnRequests";
    };
};

struct _type_AICopManagerPursuits {
    const char *name() {
        return "AICopManagerPursuits";
    };
};

struct _type_AICopManagerRoadBlocks {
    const char *name() {
        return "AICopManagerRoadBlocks";
    };
};

// total size: 0x130
class AICopManager : public Sim::Activity, public AISpawnManager, public ICopMgr, public IVehicleCache, public Debugable {
  public:
    // total size: 0x40
    struct SpawnCopRequest {
        SpawnCopRequest(const UMath::Vector3 &ip, const UMath::Vector3 &iv, const char *vn, bool inp, bool inr) {}

        UMath::Vector3 InitialPos; // offset 0x0, size 0xC
        UMath::Vector3 InitialVec; // offset 0xC, size 0xC
        char VehicleName[32];      // offset 0x18, size 0x20
        bool InPursuit;            // offset 0x38, size 0x1
        bool InRoadBlock;          // offset 0x3C, size 0x1
    };

    // total size: 0x14
    struct BreakerZone {
        BreakerZone(const UMath::Vector3 &p, float r, float e) {}

        UMath::Vector3 position; // offset 0x0, size 0xC
        float endtime;           // offset 0xC, size 0x4
        float radius;            // offset 0x10, size 0x4
    };

    AICopManager(Sim::Param params);
    static IActivity *Construct(Sim::Param params);
    bool IsPendingSupportVehicle(IVehicle *ivehicle) const;
    IVehicle *GetAvailableCopVehicleByClass(UCrc32 vehicleClass, bool bValidOnesOnly);
    IVehicle *GetAvailableCopVehicleByName(const char *name);
    IVehicle *GetActiveCopVehicleFromOutOfView(UCrc32 vehicleClass);
    IPursuit *GetPursuitActivity(ISimable *itargetSimable);
    bool TrySpawnCop(const SpawnCopRequest &request);
    void UpdateSpawnRequests();
    void MessageSetAutoSpawnMode(const MSetCopAutoSpawnMode &message);
    void MessageSetCopsEnabled(const MSetCopsEnabled &message);
    void MessageForcePursuitStart(const MForcePursuitStart &message);
    void MessageBreakerStopCops(const MBreakerStopCops &message);
    void ApplyBreakerZones();
    bool SpawnPatrolCar();
    bool GetSpawnPositionAheadOfTarget(IPursuit *ip, UMath::Vector3 &pos, UMath::Vector3 &forward, float distAhead);
    bool SpawnPursuitCar(IPursuit *ipursuit);
    bool SpawnPursuitIVehicle(IPursuit *ipursuit, IVehicle *availableCopCar);
    bool SpawnPursuitCarByName(IPursuit *ipursuit, const char *name);
    void SpawnVehicleBehindTarget(IPursuit *ipursuit, IVehicle *availableCopCar);
    bool SpawnCopCarNow(IPursuit *ipursuit);
    bool SpawnPursuitHelicopter(IPursuit *ipursuit);
    // TODO is IVehicle::List correct?
    bool CreateRoadBlock(IPursuit *ipursuit, int cop_count, IVehicle *ivehicle_chopper, IVehicle::List *suvList);
    void RemoveActiveCopVehicle(IVehicle *ivehicle);
    void UpdatePatrols();
    bool GetHeavySupportVehicles(GroundSupportRequest *gsr);
    bool StartHeavySupport(IPursuit *ipursuit, GroundSupportRequest *gsr);
    bool GetLeaderSupportVehicles(GroundSupportRequest *gsr);
    bool StartLeaderSupport(IPursuit *ipursuit, GroundSupportRequest *gsr);
    void UpdateSupportCops(IPursuit *ipursuit);
    void CommitPursuitDetails(IPursuit *ipursuit);
    void UpdatePursuits();
    void UpdateRoadBlocks();
    void UpdateDebug();
    void UpdateCopPriorities(int numActiveCopCars);

    // Virtual functions
    virtual void OnDebugDraw();

    // Virtual overrides
    // IUnknown
    ~AICopManager() override;

    // IVehicleCache
    // const char *GetCacheName() const override {}
    eVehicleCacheResult OnQueryVehicleCache(const IVehicle *removethis, const IVehicleCache *whosasking) const override;
    void OnRemovedVehicleCache(IVehicle *ivehicle) override;

    // ICopMgr
    // float GetLockoutTimeRemaining() const override {}
    bool VehicleSpawningEnabled(bool isdespawn) override;
    void SpawnCop(UMath::Vector3 &InitialPos, UMath::Vector3 &InitialVec, const char *VehicleName, bool InPursuit, bool RoadBlock) override;
    bool IsCopSpawnPending() const override;
    void SetAllBustedTimersToZero() override;
    void PursuitIsEvaded(IPursuit *ipursuit) override;
    bool IsCopRequestPending() override;
    bool CanPursueRacers() override;
    bool IsPlayerPursuitActive() override;
    bool PlayerPursuitHasCop() const override;
    void PursueAtHeatLevel(int minHeatLevel) override;
    void ResetCopsForRestart(bool release) override;
    void LockoutCops(bool lockout) override;
    void NoNewPursuitsOrCops() override;

    // IAttachable
    void OnAttached(IAttachable *pOther) override;
    void OnDetached(IAttachable *pOther) override;

    // ITaskable
    bool OnTask(HSIMTASK htask, float dT) override;

    typedef UTL::Std::list<AICopManager::SpawnCopRequest, _type_AICopManagerSpawnRequests> SpawnList;
    typedef UTL::Std::list<AICopManager::BreakerZone, _type_list> BreakerList;
    typedef UTL::Std::list<IPursuit *, _type_AICopManagerPursuits> Pursuits;
    typedef UTL::Std::list<IRoadBlock *, _type_AICopManagerRoadBlocks> RoadBlocks;

  private:
    static int mCopCarNow;         // size: 0x4, address: 0xFFFFFFFF
    static float mCopMinSpawnDist; // size: 0x4, address: 0x80415264
    static float mCopMaxSpawnDist; // size: 0x4, address: 0x80415268

    SpawnList mSpawnRequests;                   // offset 0x74, size 0x8
    BreakerList mBreakerZones;                  // offset 0x7C, size 0x8
    HSIMTASK mSimulateTask;                     // offset 0x84, size 0x4
    int mMaxCopCars;                            // offset 0x88, size 0x4
    int mMaxCopHelicopters;                     // offset 0x8C, size 0x4
    int mPlatformBudgetCopCars;                 // offset 0x90, size 0x4
    int mMaxPatrolCopCars;                      // offset 0x94, size 0x4
    int mNumActiveCopCars;                      // offset 0x98, size 0x4
    int mMaxActiveCopCars;                      // offset 0x9C, size 0x4
    int mNumActiveCopHelicopters;               // offset 0xA0, size 0x4
    int mMaxActiveCopHelicopters;               // offset 0xA4, size 0x4
    int mPursuitsInARow;                        // offset 0xA8, size 0x4
    int mTotalCopsDestroyed;                    // offset 0xAC, size 0x4
    float mLockoutTimer;                        // offset 0xB0, size 0x4
    float mHeavySupportDelayTimer;              // offset 0xB4, size 0x4
    bool mNoNewPursuitsOrCops;                  // offset 0xB8, size 0x1
    int mNumCopsForLatchedRoadblockReq;         // offset 0xBC, size 0x4
    IPursuit *mIPursuitWithLatchedRoadblockReq; // offset 0xC0, size 0x4
    IVehicle *mPursuitRequestVehicle;           // offset 0xC4, size 0x4
    IVehicle::List mIVehicleList;               // offset 0xC8, size 0x38
    Pursuits mIPursuitList;                     // offset 0x100, size 0x8
    RoadBlocks mRoadBlockList;                  // offset 0x108, size 0x8
    ActionQueue *mActionQ;                      // offset 0x110, size 0x4
    IActivity *mSpeech;                         // offset 0x114, size 0x4
    class AttributeSet *mAttributes;            // offset 0x118, size 0x4
    Hermes::HHANDLER mMessSpawnCop;             // offset 0x11C, size 0x4
    Hermes::HHANDLER mMessSetAutoSpawn;         // offset 0x120, size 0x4
    Hermes::HHANDLER mMessSetCopsEnabled;       // offset 0x124, size 0x4
    Hermes::HHANDLER mMessBreakerStopCops;      // offset 0x128, size 0x4
    Hermes::HHANDLER mMessForcePursuitStart;    // offset 0x12C, size 0x4
};

#endif
