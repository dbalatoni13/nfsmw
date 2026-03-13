#ifndef INTERFACES_SIMABLES_ISUSPENSION_H
#define INTERFACES_SIMABLES_ISUSPENSION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

// Credit: Brawltendo
// total size: 0x8
class ISuspension : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    ISuspension(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~ISuspension() {}

  public:
    virtual float GetWheelTraction(unsigned int index) const = 0;
    virtual unsigned int GetNumWheels() const = 0;
    virtual const UMath::Vector3 &GetWheelPos(unsigned int wheelIndex) const = 0;
    virtual const UMath::Vector3 &GetWheelLocalPos(unsigned int wheelIndex) const = 0;
    virtual UMath::Vector3 GetWheelCenterPos(unsigned int wheelIndex) const = 0;
    virtual float GetWheelLoad(unsigned int i) const = 0;
    virtual void ApplyVehicleEntryForces(bool enteringVehicle, const UMath::Vector3 &pos, bool calledfromEvent) = 0;
    virtual const float GetWheelRoadHeight(unsigned int i) const = 0;
    virtual bool IsWheelOnGround(unsigned int i) const = 0;
    virtual float GetCompression(unsigned int i) const = 0;
    virtual Meters GuessCompression(unsigned int wheelIndex, float downforce) const = 0;
    virtual float GetWheelSlip(unsigned int idx) const = 0;
    virtual float GetToleratedSlip(unsigned int idx) const = 0;
    virtual float GetWheelSkid(unsigned int idx) const = 0;
    virtual float GetWheelSlipAngle(unsigned int idx) const = 0;
    virtual const UMath::Vector4 &GetWheelRoadNormal(unsigned int i) const = 0;
    virtual const class SimSurface &GetWheelRoadSurface(unsigned int i) const = 0;
    virtual const UMath::Vector3 &GetWheelVelocity(unsigned int i) const = 0;
    virtual int GetNumWheelsOnGround() const = 0;
    virtual Radians GetWheelAngularVelocity(int index) const = 0;
    virtual void SetWheelAngularVelocity(int wheel, float w) = 0;
    virtual float GetWheelSteer(unsigned int wheel) const = 0;
    virtual float CalculateUndersteerFactor() const = 0;
    virtual float CalculateOversteerFactor() const = 0;
    virtual Meters GetRideHeight(unsigned int wheelIndex) const = 0;
    virtual float GetWheelRadius(unsigned int index) const = 0;
    virtual float GetMaxSteering() const = 0;
    virtual void MatchSpeed(float speed) = 0;
    virtual float GetRenderMotion() const = 0;
};

struct SuspensionParams : public Sim::Param {
    // total size: 0x10
    SuspensionParams(const SuspensionParams &_ctor_arg) : Sim::Param(_ctor_arg) {}

    // TODO
    SuspensionParams() : Sim::Param(TypeName(), static_cast<SuspensionParams *>(nullptr)) {}

    static UCrc32 TypeName() {
        static UCrc32 value = "SuspensionParams";
        return value;
    }
};

#endif
