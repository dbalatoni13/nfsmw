#ifndef PHYSICS_BEHAVIORS_RESETCAR_H
#define PHYSICS_BEHAVIORS_RESETCAR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRBVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IResetable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Misc/CookieTrail.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"
#include "Speed/Indep/Src/World/TrackPath.hpp"

// total size: 0xFC
class ResetCar : protected VehicleBehavior, protected IResetable {
  public:
    typedef VehicleBehavior Base;

    static Behavior *Construct(const BehaviorParams &params);

    ResetCar(const BehaviorParams &params);
    ~ResetCar() override;

    void OnBehaviorChange(const UCrc32 &mechanic) override;

    bool ValidTerrain(unsigned int checktires) const;
    bool CanRecord() const;
    bool ShouldReset() const;
    void TrackState(float dT);
    bool CheckZone(eTrackPathZoneType type);
    void Check(float dT);
    bool OnTask(HSIMTASK htask, float dT) override;
    void Reset() override;
    bool ResetTo(const UMath::Vector3 &position, const UMath::Vector3 &direction, bool manual);
    bool FindNearestRoad(ResetCookie *cookie) const;
    bool ResetVehicle(bool manual) override;
    bool HasResetPosition() override;
    void SetResetPosition(const UMath::Vector3 &position, const UMath::Vector3 &direction) override;
    void ClearResetPosition() override;

  protected:
    float mFlippedOver;                 // offset 0x58, size 0x4
    CookieTrail<ResetCookie, 4> mCookies; // offset 0x5C, size 0x90
    ICollisionBody *mCollisionBody;     // offset 0xEC, size 0x4
    ISuspension *mSuspension;           // offset 0xF0, size 0x4
    IRBVehicle *mVehicleBody;           // offset 0xF4, size 0x4
    HSIMTASK mCheckTask;                // offset 0xF8, size 0x4
};

#endif
