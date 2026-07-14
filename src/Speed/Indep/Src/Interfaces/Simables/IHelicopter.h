#ifndef IHELICOPTER_H
#define IHELICOPTER_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"

class IHelicopter : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IHelicopter);
};

class ISimpleChopper : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(ISimpleChopper);

    virtual void SetDesiredVelocity(const UMath::Vector3 &vel) = 0;
    virtual void GetDesiredVelocity(UMath::Vector3 &vel) = 0;
    virtual void MaxDeceleration(bool t) = 0;
    virtual void SetDesiredFacingVector(const UMath::Vector3 &facingDir) = 0;
    virtual void GetDesiredFacingVector(UMath::Vector3 &facingDir) = 0;
};

class IAIHelicopter : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IAIHelicopter);

    virtual float GetDesiredHeightOverDest() const = 0;
    virtual void SetDesiredHeightOverDest(const float height) = 0;
    virtual void SetLookAtPosition(UMath::Vector3 la) = 0;
    virtual UMath::Vector3 GetLookAtPosition() const = 0;
    virtual void SetDestinationVelocity(const UMath::Vector3 &v) = 0;
    virtual void SteerToNav(WRoadNav *road_nav, float height, float speed, bool bStopAtDest) = 0;
    virtual bool StartPathToPoint(UMath::Vector3 &point) = 0;
    virtual bool StrafeToDestIsSet() const = 0;
    virtual void SetStrafeToDest(bool strafe) = 0;
    virtual bool FilterHeliAltitude(UMath::Vector3 &point) = 0;
    virtual void RestrictPointToRoadNet(UMath::Vector3 &seekPosition) = 0;
    virtual void SetFuelFull() = 0;
    virtual float GetFuelTimeRemaining() = 0;
    virtual void SetShadowScale(float s) = 0;
    virtual float GetShadowScale() = 0;
    virtual void SetDustStormIntensity(float d) = 0;
    virtual float GetDustStormIntensity() = 0;
};

#endif
