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

    virtual void SetDesiredVelocity(const UMath::Vector3 &vel);
    virtual void GetDesiredVelocity(UMath::Vector3 &vel);
    virtual void MaxDeceleration(bool t);
    virtual void SetDesiredFacingVector(const UMath::Vector3 &facingDir);
    virtual void GetDesiredFacingVector(UMath::Vector3 &facingDir);
};

class IAIHelicopter : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IAIHelicopter);

    virtual float GetDesiredHeightOverDest() const;
    virtual void SetDesiredHeightOverDest(const float height);
    virtual void SetLookAtPosition(UMath::Vector3 la);
    virtual UMath::Vector3 GetLookAtPosition() const;
    virtual void SetDestinationVelocity(const UMath::Vector3 &v);
    virtual void SteerToNav(WRoadNav *road_nav, float height, float speed, bool bStopAtDest);
    virtual bool StartPathToPoint(UMath::Vector3 &point);
    virtual bool StrafeToDestIsSet() const;
    virtual void SetStrafeToDest(bool strafe);
    virtual bool FilterHeliAltitude(UMath::Vector3 &point);
    virtual void RestrictPointToRoadNet(UMath::Vector3 &seekPosition);
    virtual void SetFuelFull();
    virtual float GetFuelTimeRemaining();
    virtual void SetShadowScale(float s);
    virtual float GetShadowScale();
    virtual void SetDustStormIntensity(float d);
    virtual float GetDustStormIntensity();
};

#endif
