#ifndef INTTERFACES_SIMABLES_IVEHICLE_H
#define INTTERFACES_SIMABLES_IVEHICLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
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

enum CarType {
    CARTYPE_NONE = -1,
    CARTYPE_911TURBO = 0,
    CARTYPE_CARRERAGT = 1,
    CARTYPE_VIPER = 2,
    CARTYPE_COPMIDSIZE = 3,
    CARTYPE_COPHELI = 4,
    CARTYPE_RX8 = 5,
    CARTYPE_IMPREZAWRX = 6,
    CARTYPE_LANCEREVO8 = 7,
    CARTYPE_MUSTANGGT = 8,
    CARTYPE_CAMARO = 9,
    CARTYPE_SEMI = 10,
    CARTYPE_GTO = 11,
    CARTYPE_SLR = 12,
    CARTYPE_TRAILERA = 13,
    CARTYPE_TRAILERB = 14,
    CARTYPE_CORVETTE = 15,
    CARTYPE_TRAFHA = 16,
    CARTYPE_TRAFSUVA = 17,
    CARTYPE_TRAF4DSEDB = 18,
    CARTYPE_TRAFFICCOUP = 19,
    CARTYPE_TT = 20,
    CARTYPE_A3 = 21,
    CARTYPE_BMWM3GTR = 22,
    CARTYPE_SL500 = 23,
    CARTYPE_997S = 24,
    CARTYPE_TRAF4DSEDA = 25,
    CARTYPE_TRAF4DSEDC = 26,
    CARTYPE_TRAFCAMPER = 27,
    CARTYPE_TRAFCEMTR = 28,
    CARTYPE_TRAFDMPTR = 29,
    CARTYPE_TRAFFIRE = 30,
    CARTYPE_TRAFGARB = 31,
    CARTYPE_TRAFMINIVAN = 32,
    CARTYPE_TRAFNEWS = 33,
    CARTYPE_TRAFPICKUPA = 34,
    CARTYPE_TRAFSTWAG = 35,
    CARTYPE_TRAFVANB = 36,
    CARTYPE_TRAFTAXI = 37,
    CARTYPE_TRAFCOURT = 38,
    CARTYPE_TRAFPIZZA = 39,
    CARTYPE_911GT2 = 40,
    CARTYPE_BMWM3GTRE46 = 41,
    CARTYPE_TRAFAMB = 42,
    CARTYPE_TRAILERLOG = 43,
    CARTYPE_TRAILERCON = 44,
    CARTYPE_TRAILERCRATE = 45,
    CARTYPE_TRAILERCMT = 46,
    CARTYPE_IS300 = 47,
    CARTYPE_MONARO = 48,
    CARTYPE_SL65 = 49,
    CARTYPE_ELISE = 50,
    CARTYPE_COPMIDSIZEINT = 51,
    CARTYPE_FORDGT = 52,
    CARTYPE_ECLIPSEGT = 53,
    CARTYPE_SUPRA = 54,
    CARTYPE_CORVETTEC6R = 55,
    CARTYPE_RX7 = 56,
    CARTYPE_COPSUV = 57,
    CARTYPE_RX8SPEEDT = 58,
    CARTYPE_MURCIELAGO = 59,
    CARTYPE_A4 = 60,
    CARTYPE_COPSPORT = 61,
    CARTYPE_GTI = 62,
    CARTYPE_CAYMANS = 63,
    CARTYPE_CLK500 = 64,
    CARTYPE_CTS = 65,
    CARTYPE_DB9 = 66,
    CARTYPE_GALLARDO = 67,
    CARTYPE_COBALTSS = 68,
    CARTYPE_BMWM3 = 69,
    CARTYPE_CLIO = 70,
    CARTYPE_COPGHOST = 71,
    CARTYPE_COPSUVL = 72,
    CARTYPE_COPGTO = 73,
    CARTYPE_PUNTO = 74,
    CARTYPE_COPGTOGHOST = 75,
    CARTYPE_COPSPORTHENCH = 76,
    CARTYPE_COPSPORTGHOST = 77,
    CARTYPE_CEMTR = 78,
    CARTYPE_GARB = 79,
    CARTYPE_PIZZA = 80,
    CARTYPE_TAXI = 81,
    CARTYPE_MINIVAN = 82,
    CARTYPE_PICKUPA = 83,
    NUM_CARTYPES = 84,
};

enum DriverClass {
    DRIVER_HUMAN = 0,
    DRIVER_TRAFFIC = 1,
    DRIVER_COP = 2,
    DRIVER_RACER = 3,
    DRIVER_NONE = 4,
    DRIVER_NIS = 5,
    DRIVER_REMOTE = 6,
};

// TODO move?
enum ID {
    LIGHT_NONE = 0,
    LIGHT_LHEAD = 1,
    LIGHT_RHEAD = 2,
    LIGHT_CHEAD = 4,
    LIGHT_LBRAKE = 8,
    LIGHT_RBRAKE = 16,
    LIGHT_CBRAKE = 32,
    LIGHT_LREVERSE = 64,
    LIGHT_RREVERSE = 128,
    LIGHT_LRSIGNAL = 256,
    LIGHT_RRSIGNAL = 512,
    LIGHT_LFSIGNAL = 1024,
    LIGHT_RFSIGNAL = 2048,
    LIGHT_COPRED = 4096,
    LIGHT_COPBLUE = 8192,
    LIGHT_COPWHITE = 16384,
    LIGHT_COPS = 28672,
    LIGHT_LSIGNAL = 1280,
    LIGHT_RSIGNAL = 2560,
    LIGHT_HEADLIGHTS = 7,
    LIGHT_REVERSE = 192,
    LIGHT_BRAKELIGHTS = 56,
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
    virtual bool IsGlareOn(ID glare) const = 0;
    virtual void GlareOn(ID glare) = 0;
    virtual void GlareOff(ID glare) = 0;
    virtual bool IsCollidingWithSoftBarrier() = 0;
    virtual class IVehicleAI *GetAIVehiclePtr() const = 0;
    virtual float GetSlipAngle() const = 0;
    virtual UMath::Vector3 &GetLocalVelocity() const = 0;
    virtual void ComputeHeading(UMath::Vector3 *v) = 0;
    virtual bool IsAnimating() const = 0;
    virtual void SetAnimating(bool animate) = 0;
    virtual bool IsOffWorld() const = 0;
    virtual const struct FECustomizationRecord *GetCustomizations() const = 0;
    virtual const Physics::Tunings *GetTunings() const = 0;
    virtual void SetTunings(const Physics::Tunings &tunings) = 0;
    virtual bool GetPerformance(Physics::Info::Performance &performance) const = 0;
    virtual const char *GetCacheName() const = 0;
};

#endif
