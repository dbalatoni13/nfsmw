#ifndef INTTERFACES_SIMABLES_IVEHICLE_H
#define INTTERFACES_SIMABLES_IVEHICLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"

enum eVehicleList {
    VEHICLE_MAX = 10,
    VEHICLE_TRAILERS = 9,
    VEHICLE_INACTIVE = 8,
    VEHICLE_REMOTE = 7,
    VEHICLE_RACERS = 6,
    VEHICLE_AITRAFFIC = 5,
    VEHICLE_AICOPS = 4,
    VEHICLE_AIRACERS = 3,
    VEHICLE_AI = 2,
    VEHICLE_PLAYERS = 1,
    VEHICLE_ALL = 0,
};

class IVehicle : public UTL::COM::IUnknown, public UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10> {
  public:
    IVehicle(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    // virtual ~IVehicle() {}
    // virtual void *GetSimable() const;
    // virtual void *GetSimable();
    // virtual UMath::Vector3 &GetPosition();
    // virtual void SetBehaviorOverride(uint32_t, uint32_t);
    // virtual void RemoveBehaviorOverride(uint32_t);
    // virtual void CommitBehaviorOverrides();
    // virtual void SetStaging(bool staging);
    // virtual bool IsStaging();
    // virtual void Launch();
    // virtual float GetPerfectLaunch();
    // virtual void SetDriverStyle(DriverStyle style);
    // virtual DriverStyle GetDriverStyle();
    // virtual void SetPhysicsMode(enum PhysicsMode mode);
    // virtual enum PhysicsMode GetPhysicsMode();
    // virtual int GetModelType();
    // virtual bool IsSpooled();
    // virtual int GetVehicleClass();
    // virtual int GetVehicleAttributes();
    // virtual char *GetVehicleName();
    // virtual uint32_t GetVehicleKey();
    // virtual void SetDriverClass(DriverClass dc);
    // virtual DriverClass GetDriverClass();
    // virtual bool IsLoading();
    // virtual float GetOffscreenTime();
    // virtual float GetOnScreenTime();
    // virtual void SetVehicleOnGround(const UMath::Vector3 &, const UMath::Vector3 &);
    // virtual void ForceStopOn(bool);
    // virtual void ForceStopOff(bool);
    // virtual bool GetForceStop();
    // virtual bool InShock();
    // virtual bool IsDestroyed();
    // virtual void Activate();
    // virtual void Deactivate();
    // virtual bool IsActive();
    // virtual float GetSpeedometer();
    // virtual float GetSpeed();
    // virtual void SetSpeed(float speed);
    // virtual float GetAbsoluteSpeed();
    // virtual bool IsGlareOn(int id);
    // virtual void GlareOn(int id);
    // virtual void GlareOff(int id);
    // virtual bool IsCollidingWithSoftBarrier();
    // virtual void *GetAIVehiclePtr();
    // virtual float GetSlipAngle();
    // virtual UMath::Vector3 &GetLocalVelocity();
    // virtual void ComputeHeading(UMath::Vector3 *);
    // virtual bool IsAnimating();
    // virtual void SetAnimating(bool animating);
    // virtual bool IsOffWorld();
    // virtual void *GetCustomizations();
    // virtual Physics::Tunings *GetTunings();
    // virtual void SetTunings(const Physics::Tunings &);
    // virtual void *GetPerformance(void *);
    // virtual char *GetCacheName();
};

#endif
