#ifndef INTERFACES_SIMABLES_ISUSPENSION_H
#define INTERFACES_SIMABLES_ISUSPENSION_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"

// Credit: Brawltendo
// total size: 0x8
class ISuspension : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(ISuspension);

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

#endif
