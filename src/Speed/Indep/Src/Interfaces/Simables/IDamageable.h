#ifndef IDAMAGEABLE_H
#define IDAMAGEABLE_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/World/Damagezones.h"
#include "Speed/Indep/Src/World/VehicleFX.h"

class IDamageable : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IDamageable);

    virtual void SetInShock(float scale) = 0;
    virtual void SetShockForce(float f) = 0;
    virtual float InShock() const = 0;
    virtual void ResetDamage() = 0;
    virtual float GetHealth() const = 0;
    virtual bool IsDestroyed() const = 0;
    virtual void Destroy() = 0;
    virtual struct DamageZone::Info GetZoneDamage() const = 0;
};

class IDamageableVehicle : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IDamageableVehicle);

    virtual bool IsLightDamaged(VehicleFX::ID idx) const = 0;
    virtual void DamageLight(VehicleFX::ID idx, bool b) = 0;
};

#endif
