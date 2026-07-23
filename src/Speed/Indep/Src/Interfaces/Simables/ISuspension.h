#ifndef ISUSPENSION_H
#define ISUSPENSION_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"

// Credit: Brawltendo
// total size: 0x8
class ISuspension : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(ISuspension);

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

#endif
