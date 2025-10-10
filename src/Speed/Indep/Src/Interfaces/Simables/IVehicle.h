#ifndef INTTERFACES_SIMABLES_IVEHICLE_H
#define INTTERFACES_SIMABLES_IVEHICLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/AI/AIVehicle.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Physics/PhysicsTunings.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"

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

enum CarType {
    NUM_CARTYPES = 84,
    CARTYPE_PICKUPA = 83,
    CARTYPE_MINIVAN = 82,
    CARTYPE_TAXI = 81,
    CARTYPE_PIZZA = 80,
    CARTYPE_GARB = 79,
    CARTYPE_CEMTR = 78,
    CARTYPE_COPSPORTGHOST = 77,
    CARTYPE_COPSPORTHENCH = 76,
    CARTYPE_COPGTOGHOST = 75,
    CARTYPE_PUNTO = 74,
    CARTYPE_COPGTO = 73,
    CARTYPE_COPSUVL = 72,
    CARTYPE_COPGHOST = 71,
    CARTYPE_CLIO = 70,
    CARTYPE_BMWM3 = 69,
    CARTYPE_COBALTSS = 68,
    CARTYPE_GALLARDO = 67,
    CARTYPE_DB9 = 66,
    CARTYPE_CTS = 65,
    CARTYPE_CLK500 = 64,
    CARTYPE_CAYMANS = 63,
    CARTYPE_GTI = 62,
    CARTYPE_COPSPORT = 61,
    CARTYPE_A4 = 60,
    CARTYPE_MURCIELAGO = 59,
    CARTYPE_RX8SPEEDT = 58,
    CARTYPE_COPSUV = 57,
    CARTYPE_RX7 = 56,
    CARTYPE_CORVETTEC6R = 55,
    CARTYPE_SUPRA = 54,
    CARTYPE_ECLIPSEGT = 53,
    CARTYPE_FORDGT = 52,
    CARTYPE_COPMIDSIZEINT = 51,
    CARTYPE_ELISE = 50,
    CARTYPE_SL65 = 49,
    CARTYPE_MONARO = 48,
    CARTYPE_IS300 = 47,
    CARTYPE_TRAILERCMT = 46,
    CARTYPE_TRAILERCRATE = 45,
    CARTYPE_TRAILERCON = 44,
    CARTYPE_TRAILERLOG = 43,
    CARTYPE_TRAFAMB = 42,
    CARTYPE_BMWM3GTRE46 = 41,
    CARTYPE_911GT2 = 40,
    CARTYPE_TRAFPIZZA = 39,
    CARTYPE_TRAFCOURT = 38,
    CARTYPE_TRAFTAXI = 37,
    CARTYPE_TRAFVANB = 36,
    CARTYPE_TRAFSTWAG = 35,
    CARTYPE_TRAFPICKUPA = 34,
    CARTYPE_TRAFNEWS = 33,
    CARTYPE_TRAFMINIVAN = 32,
    CARTYPE_TRAFGARB = 31,
    CARTYPE_TRAFFIRE = 30,
    CARTYPE_TRAFDMPTR = 29,
    CARTYPE_TRAFCEMTR = 28,
    CARTYPE_TRAFCAMPER = 27,
    CARTYPE_TRAF4DSEDC = 26,
    CARTYPE_TRAF4DSEDA = 25,
    CARTYPE_997S = 24,
    CARTYPE_SL500 = 23,
    CARTYPE_BMWM3GTR = 22,
    CARTYPE_A3 = 21,
    CARTYPE_TT = 20,
    CARTYPE_TRAFFICCOUP = 19,
    CARTYPE_TRAF4DSEDB = 18,
    CARTYPE_TRAFSUVA = 17,
    CARTYPE_TRAFHA = 16,
    CARTYPE_CORVETTE = 15,
    CARTYPE_TRAILERB = 14,
    CARTYPE_TRAILERA = 13,
    CARTYPE_SLR = 12,
    CARTYPE_GTO = 11,
    CARTYPE_SEMI = 10,
    CARTYPE_CAMARO = 9,
    CARTYPE_MUSTANGGT = 8,
    CARTYPE_LANCEREVO8 = 7,
    CARTYPE_IMPREZAWRX = 6,
    CARTYPE_RX8 = 5,
    CARTYPE_COPHELI = 4,
    CARTYPE_COPMIDSIZE = 3,
    CARTYPE_VIPER = 2,
    CARTYPE_CARRERAGT = 1,
    CARTYPE_911TURBO = 0,
    CARTYPE_NONE = -1,
};

enum DriverClass {
    DRIVER_REMOTE = 6,
    DRIVER_NIS = 5,
    DRIVER_NONE = 4,
    DRIVER_RACER = 3,
    DRIVER_COP = 2,
    DRIVER_TRAFFIC = 1,
    DRIVER_HUMAN = 0,
};

class IVehicle : public UTL::COM::IUnknown, public UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, VEHICLE_MAX> {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IVehicle(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IVehicle() {}
    virtual const ISimable *GetSimable() const = 0;
    virtual ISimable *GetSimable() = 0;
    virtual UMath::Vector3 &GetPosition() = 0;
    virtual void SetBehaviorOverride(UCrc32 mechanic, UCrc32 behavior) = 0;
    virtual void RemoveBehaviorOverride(UCrc32 mechanic) = 0;
    virtual void CommitBehaviorOverrides() = 0;
    virtual void SetStaging(bool staging) = 0;
    virtual bool IsStaging() = 0;
    virtual void Launch() = 0;
    virtual float GetPerfectLaunch() = 0;
    virtual void SetDriverStyle(DriverStyle style) = 0;
    virtual DriverStyle GetDriverStyle() = 0;
    virtual void SetPhysicsMode(PhysicsMode mode) = 0;
    virtual PhysicsMode GetPhysicsMode() const = 0;
    virtual CarType GetModelType() = 0;
    virtual bool IsSpooled() = 0;
    virtual const UCrc32 GetVehicleClass() = 0;
    virtual Attrib::Gen::pvehicle &GetVehicleAttributes() = 0;
    virtual const char *GetVehicleName() = 0;
    virtual unsigned int GetVehicleKey() = 0;
    virtual void SetDriverClass(DriverClass dc) = 0;
    virtual DriverClass GetDriverClass() = 0;
    virtual bool IsLoading() const = 0;
    virtual float GetOffscreenTime() = 0;
    virtual float GetOnScreenTime() = 0;
    virtual bool SetVehicleOnGround(const UMath::Vector3 &resetPos, const UMath::Vector3 &initialVec) = 0;
    virtual void ForceStopOn(char forceStopBits) = 0;
    virtual void ForceStopOff(char forceStopBits) = 0;
    virtual bool GetForceStop() = 0;
    virtual bool InShock() = 0;
    virtual bool IsDestroyed() = 0;
    virtual void Activate() = 0;
    virtual void Deactivate() = 0;
    virtual bool IsActive() const = 0;
    virtual float GetSpeedometer() = 0;
    virtual float GetSpeed() = 0;
    virtual void SetSpeed(float speed) = 0;
    virtual float GetAbsoluteSpeed() = 0;
    virtual bool IsGlareOn(int glare) = 0;
    virtual void GlareOn(int glare) = 0; // TODO "ID" enum
    virtual void GlareOff(int glare) = 0;
    virtual bool IsCollidingWithSoftBarrier() = 0;
    virtual AIVehicle *GetAIVehiclePtr() = 0;
    virtual float GetSlipAngle() = 0;
    virtual UMath::Vector3 &GetLocalVelocity() = 0;
    virtual void ComputeHeading(UMath::Vector3 *v) = 0;
    virtual bool IsAnimating() = 0;
    virtual void SetAnimating(bool animate) = 0;
    virtual bool IsOffWorld() = 0;
    virtual struct FECustomizationRecord *GetCustomizations() = 0;
    virtual Physics::Tunings *GetTunings() = 0;
    virtual void SetTunings(const Physics::Tunings &tunings) = 0;
    virtual bool GetPerformance(struct Performance &performance) = 0;
    virtual const char *GetCacheName() = 0;
};

#endif
