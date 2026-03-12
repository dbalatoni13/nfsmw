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
    virtual float GetWheelTraction(unsigned int index) const;
    virtual unsigned int GetNumWheels() const;
    virtual const UMath::Vector3 &GetWheelPos(unsigned int wheelIndex) const;
    virtual const UMath::Vector3 &GetWheelLocalPos(unsigned int wheelIndex) const;
    virtual UMath::Vector3 GetWheelCenterPos(unsigned int wheelIndex) const;
    virtual float GetWheelLoad(unsigned int i) const;
    virtual void ApplyVehicleEntryForces(bool enteringVehicle, const UMath::Vector3 &pos, bool calledfromEvent);
    virtual const float GetWheelRoadHeight(unsigned int i) const;
    virtual bool IsWheelOnGround(unsigned int i) const;
    virtual float GetCompression(unsigned int i) const;
    virtual Meters GuessCompression(unsigned int wheelIndex, float downforce) const;
    virtual float GetWheelSlip(unsigned int idx) const;
    virtual float GetToleratedSlip(unsigned int idx) const;
    virtual float GetWheelSkid(unsigned int idx) const;
    virtual float GetWheelSlipAngle(unsigned int idx) const;
    virtual const UMath::Vector4 &GetWheelRoadNormal(unsigned int i) const;
    virtual const class SimSurface &GetWheelRoadSurface(unsigned int i) const;
    virtual const UMath::Vector3 &GetWheelVelocity(unsigned int i) const;
    virtual int GetNumWheelsOnGround() const;
    virtual Radians GetWheelAngularVelocity(int index) const;
    virtual void SetWheelAngularVelocity(int wheel, float w);
    virtual float GetWheelSteer(unsigned int wheel) const;
    virtual float CalculateUndersteerFactor() const;
    virtual float CalculateOversteerFactor() const;
    virtual Meters GetRideHeight(unsigned int wheelIndex) const;
    virtual float GetWheelRadius(unsigned int index) const;
    virtual float GetMaxSteering() const;
    virtual void MatchSpeed(float speed);
    virtual float GetRenderMotion() const;
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
