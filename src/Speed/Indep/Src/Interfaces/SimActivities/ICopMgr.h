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
    virtual bool VehicleSpawningEnabled(bool isdespawn);
#endif
    virtual void ResetCopsForRestart(bool release);
#ifndef EA_BUILD_A124
    virtual float GetLockoutTimeRemaining() const; // maybe 1 higher and maybe this is SetAllBusted's place
#endif
    virtual void PursuitIsEvaded(struct IPursuit *ipursuit);
    virtual bool IsCopRequestPending();
    virtual bool IsCopSpawnPending() const;
    virtual void SpawnCop(UMath::Vector3 &InitialPos, UMath::Vector3 &InitialVec, const char *VehicleName, bool InPursuit, bool RoadBlock);
    virtual bool PlayerPursuitHasCop() const;
    virtual bool CanPursueRacers();
    virtual bool IsPlayerPursuitActive();
    virtual void LockoutCops(bool lockout);
    virtual void NoNewPursuitsOrCops();
    virtual void PursueAtHeatLevel(int minHeatLevel);
#ifndef EA_BUILD_A124
    virtual void SetAllBustedTimersToZero(); // TODO fix the position of this
#endif

    static void EnableCops() {}

    static void DisableCops() {}

    static bool AreCopsEnabled() {}
};

#endif
