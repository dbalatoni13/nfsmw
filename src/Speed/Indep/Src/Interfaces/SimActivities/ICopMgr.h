#ifndef INTERFACES_SIMACTIVITIES_ICOPMGR_H
#define INTERFACES_SIMACTIVITIES_ICOPMGR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"

// total size: 0xC
class ICopMgr : public UTL::COM::IUnknown, public UTL::Collections::Singleton<ICopMgr> {
  public:
    static int mDisableCops;

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    ICopMgr(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  protected:
    ~ICopMgr() override {}

  public:
    virtual bool VehicleSpawningEnabled(bool isdespawn);
    virtual void ResetCopsForRestart(bool release);
    virtual void PursuitIsEvaded(struct IPursuit *ipursuit);
    virtual bool IsCopRequestPending();
    virtual bool IsCopSpawnPending() const;
    virtual void SpawnCop(UMath::Vector3 &InitialPos, UMath::Vector3 &InitialVec, const char *VehicleName, bool InPursuit, bool RoadBlock);
#ifndef EA_BUILD_A124
    virtual void SetAllBustedTimersToZero();
#endif
    virtual bool PlayerPursuitHasCop() const;
    virtual bool CanPursueRacers();
    virtual bool IsPlayerPursuitActive();
    virtual void LockoutCops(bool lockout);
    virtual void NoNewPursuitsOrCops();
    virtual void PursueAtHeatLevel(int minHeatLevel);
#ifndef EA_BUILD_A124
    virtual float GetLockoutTimeRemaining() const;
#endif

    static void EnableCops() {
        mDisableCops = 0;
    }

    static void DisableCops() {
        mDisableCops = 1;
    }

    static bool AreCopsEnabled() {
        return mDisableCops == 0;
    }
};

#endif
