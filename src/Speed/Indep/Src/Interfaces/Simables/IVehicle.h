#ifndef IVEHICLE_H
#define IVEHICLE_H

#include "Speed/Indep/Src/World/VehicleFX.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Generated/CarTypes.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Physics/PhysicsTunings.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"

enum eVehicleList {
    VEHICLE_ALL,
    VEHICLE_PLAYERS,
    VEHICLE_AI,
    VEHICLE_AIRACERS,
    VEHICLE_AICOPS,
    VEHICLE_AITRAFFIC,
    VEHICLE_RACERS,
    VEHICLE_REMOTE,
    VEHICLE_INACTIVE,
    VEHICLE_TRAILERS,
    VEHICLE_MAX,
};

class IVehicle : public UTL::COM::IUnknown, public UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, VEHICLE_MAX> {
  public:
    DECL_INTERFACE(IVehicle);

    virtual const ISimable *GetSimable() const = 0;
    virtual ISimable *GetSimable() = 0;
    virtual UMath::Vector3 &GetPosition() const = 0;
    virtual void SetBehaviorOverride(UCrc32 mechanic, UCrc32 behavior) = 0;
    virtual void RemoveBehaviorOverride(UCrc32 mechanic) = 0;
    virtual void CommitBehaviorOverrides() = 0;
    virtual void SetStaging(bool staging) = 0;
    virtual bool IsStaging() const = 0;
    virtual void Launch() = 0;
    virtual float GetPerfectLaunch() const = 0;
    virtual void SetDriverStyle(DriverStyle style) = 0;
    virtual DriverStyle GetDriverStyle() const = 0;
    virtual void SetPhysicsMode(PhysicsMode mode) = 0;
    virtual PhysicsMode GetPhysicsMode() const = 0;
    virtual CarType GetModelType() const = 0;
    virtual bool IsSpooled() const = 0;
    virtual const UCrc32 &GetVehicleClass() const = 0;
    virtual Attrib::Gen::pvehicle &GetVehicleAttributes() const = 0;
    virtual const char *GetVehicleName() const = 0;
    virtual unsigned int GetVehicleKey() const = 0;
    virtual void SetDriverClass(DriverClass dc) = 0;
    virtual DriverClass GetDriverClass() const = 0;
    virtual bool IsLoading() const = 0;
    virtual float GetOffscreenTime() const = 0;
    virtual float GetOnScreenTime() const = 0;
    virtual bool SetVehicleOnGround(const UMath::Vector3 &resetPos, const UMath::Vector3 &initialVec) = 0;
    virtual void ForceStopOn(char forceStopBits) = 0;
    virtual void ForceStopOff(char forceStopBits) = 0;
    virtual char GetForceStop() = 0;
    virtual bool InShock() const = 0;
    virtual bool IsDestroyed() const = 0;
    virtual void Activate() = 0;
    virtual void Deactivate() = 0;
    virtual bool IsActive() const = 0;
    virtual float GetSpeedometer() const = 0;
    virtual float GetSpeed() const = 0;
    virtual void SetSpeed(float speed) = 0;
    virtual float GetAbsoluteSpeed() const = 0;
    virtual bool IsGlareOn(VehicleFX::ID glare) const = 0;
    virtual void GlareOn(VehicleFX::ID glare) = 0;
    virtual void GlareOff(VehicleFX::ID glare) = 0;
    virtual bool IsCollidingWithSoftBarrier() = 0;
    virtual class IVehicleAI *GetAIVehiclePtr() const = 0;
    virtual float GetSlipAngle() const = 0;
    virtual UMath::Vector3 &GetLocalVelocity() const = 0;
    virtual void ComputeHeading(UMath::Vector3 *v) = 0;
    virtual bool IsAnimating() const = 0;
    virtual void SetAnimating(bool animate) = 0;
    virtual bool IsOffWorld() const = 0;
    virtual const struct FECustomizationRecord *GetCustomizations() const = 0; // TODO remove "struct"?
    virtual const Physics::Tunings *GetTunings() const = 0;
    virtual void SetTunings(const Physics::Tunings &tunings) = 0;
    virtual bool GetPerformance(Physics::Info::Performance &performance) const = 0;
    virtual const char *GetCacheName() const = 0;
};

#endif
