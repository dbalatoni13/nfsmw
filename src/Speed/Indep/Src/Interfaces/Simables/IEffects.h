#ifndef IEFFECTS_H
#define IEFFECTS_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

// total size: 0x8
class IEffects : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IEffects);

    virtual void HitGround() = 0;
    virtual void HitWorld() = 0;
    virtual void HitObject() = 0;
    virtual void ScrapeObject() = 0;
    virtual void ScrapeGround() = 0;
    virtual void ScrapeWorld() = 0;
    virtual void Purge() = 0;
};

// total size: 0x8
class IEffectsVehicle : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IEffectsVehicle);

    virtual void TireSkid() = 0;
    virtual void TireSpray() = 0;
    virtual void BottomOut() = 0;
    virtual void RidingRims() = 0;
    virtual void Wash() = 0;
    virtual void Backfire() = 0;
    virtual void TurretScrape() = 0;
};

#endif
