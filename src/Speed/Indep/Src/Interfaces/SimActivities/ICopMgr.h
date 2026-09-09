//
//
//
#ifndef ICOPMGR_H
#define ICOPMGR_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"

// total size: 0xC
class ICopMgr : public UTL::COM::IUnknown, public UTL::Collections::Singleton<ICopMgr> {
  public:
    DECL_INTERFACE(ICopMgr);
    static int mDisableCops;

#ifndef EA_BUILD_A124
    virtual bool VehicleSpawningEnabled(bool isdespawn) = 0;
#endif
    virtual void ResetCopsForRestart(bool release) = 0;
#ifndef EA_BUILD_A124
    virtual void SetAllBustedTimersToZero() = 0;
#endif
    virtual void PursuitIsEvaded(IPursuit *ipursuit) = 0;
    virtual bool IsCopRequestPending() = 0;
    virtual bool IsCopSpawnPending() const = 0;
    virtual void SpawnCop(UMath::Vector3 &InitialPos, UMath::Vector3 &InitialVec, const char *VehicleName, bool InPursuit, bool RoadBlock) = 0;
    virtual bool PlayerPursuitHasCop() const = 0;
    virtual bool CanPursueRacers() = 0;
    virtual bool IsPlayerPursuitActive() = 0;
    virtual void LockoutCops(bool lockout) = 0;
    virtual void NoNewPursuitsOrCops() = 0;
    virtual void PursueAtHeatLevel(int minHeatLevel) = 0;
#ifndef EA_BUILD_A124
    virtual float GetLockoutTimeRemaining() const = 0;
#endif

    static void EnableCops() {}

    static void DisableCops() {}

    static bool AreCopsEnabled() {}
};

#endif
