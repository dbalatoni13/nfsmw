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
#ifndef EA_BUILD_A124
    virtual bool VehicleSpawningEnabled(bool isdespawn) = 0;
#endif
    virtual void ResetCopsForRestart(bool release) = 0;
#ifndef EA_BUILD_A124
    virtual float GetLockoutTimeRemaining() const = 0; // maybe 1 higher and maybe this is SetAllBusted's place
#endif
    virtual void PursuitIsEvaded(struct IPursuit *ipursuit) = 0;
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
    virtual void SetAllBustedTimersToZero() = 0; // TODO fix the position of this
#endif

    static void EnableCops() {}

    static void DisableCops() {}

    static bool AreCopsEnabled() {}
};

#endif
