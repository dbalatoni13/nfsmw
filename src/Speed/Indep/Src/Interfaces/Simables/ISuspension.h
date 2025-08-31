#ifndef INTERFACES_SIMABLES_ISUSPENSION_H
#define INTERFACES_SIMABLES_ISUSPENSION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

// Credit: Brawltendo
class ISuspension : public UTL::COM::IUnknown {
  public:
    ISuspension(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    virtual ~ISuspension() {}

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  public:
    virtual float GetWheelTraction(unsigned int wheelIndex);
    virtual unsigned int GetNumWheels();
    virtual UMath::Vector3 *GetWheelPos(unsigned int wheelIndex);
    virtual UMath::Vector3 *GetWheelLocalPos(unsigned int wheelIndex);
    virtual UMath::Vector3 *GetWheelCenterPos(unsigned int wheelIndex);
    virtual float GetWheelLoad(unsigned int wheelIndex);
    virtual void ApplyVehicleEntryForces(bool enteringVehicle, const UMath::Vector3 &pos, bool calledfromEvent);
    virtual float GetWheelRoadHeight(unsigned int wheelIndex);
    virtual bool IsWheelOnGround(unsigned int wheelIndex);
    virtual float GetCompression(unsigned int wheelIndex);
    virtual float GuessCompression(unsigned int wheelIndex, float downforce);
    virtual float GetWheelSlip(unsigned int wheelIndex);
    virtual float GetToleratedSlip(unsigned int wheelIndex);
    virtual float GetWheelSkid(unsigned int wheelIndex);
    virtual float GetWheelSlipAngle(unsigned int wheelIndex);
    virtual UMath::Vector4 *GetWheelRoadNormal(unsigned int wheelIndex);
    virtual void *GetWheelRoadSurface(unsigned int wheelIndex);
    virtual UMath::Vector3 *GetWheelVelocity(unsigned int wheelIndex);
    virtual unsigned int GetNumWheelsOnGround();
    virtual float GetWheelAngularVelocity(unsigned int wheelIndex);
    virtual void SetWheelAngularVelocity(unsigned int wheelIndex, float av);
    virtual float GetWheelSteer(unsigned int wheelIndex);
    virtual float GetRideHeight(unsigned int wheelIndex);
    virtual float GetWheelRadius(unsigned int wheelIndex);
    virtual float GetMaxSteering();
};

#endif
