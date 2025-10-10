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
    virtual float GetWheelTraction(unsigned int wheelIndex) const;
    virtual unsigned int GetNumWheels() const;
    virtual UMath::Vector3 &GetWheelPos(unsigned int wheelIndex) const;
    virtual UMath::Vector3 &GetWheelLocalPos(unsigned int wheelIndex) const;
    virtual UMath::Vector3 GetWheelCenterPos(unsigned int wheelIndex) const;
    virtual float GetWheelLoad(unsigned int wheelIndex) const;
    virtual void ApplyVehicleEntryForces(bool enteringVehicle, const UMath::Vector3 &pos, bool calledfromEvent);
    virtual float GetWheelRoadHeight(unsigned int wheelIndex) const;
    virtual bool IsWheelOnGround(unsigned int wheelIndex) const;
    virtual float GetCompression(unsigned int wheelIndex) const;
    virtual Meters GuessCompression(unsigned int wheelIndex, float downforce) const;
    virtual float GetWheelSlip(unsigned int wheelIndex) const;
    virtual float GetToleratedSlip(unsigned int wheelIndex) const;
    virtual float GetWheelSkid(unsigned int wheelIndex) const;
    virtual float GetWheelSlipAngle(unsigned int wheelIndex) const;
    virtual UMath::Vector4 *GetWheelRoadNormal(unsigned int wheelIndex) const;
    virtual void *GetWheelRoadSurface(unsigned int wheelIndex) const;
    virtual UMath::Vector3 *GetWheelVelocity(unsigned int wheelIndex) const;
    virtual int GetNumWheelsOnGround() const;
    virtual Radians GetWheelAngularVelocity(unsigned int wheelIndex) const;
    virtual void SetWheelAngularVelocity(unsigned int wheelIndex, float av);
    virtual float GetWheelSteer(unsigned int wheelIndex) const;
    virtual float CalculateUndersteerFactor() const;
    virtual float CalculateOversteerFactor() const;
    virtual Meters GetRideHeight(unsigned int wheelIndex) const;
    virtual float GetWheelRadius(unsigned int wheelIndex) const;
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
        static UCrc32 value = UCrc32("SuspensionParams");
        return value;
    }
};

#endif
