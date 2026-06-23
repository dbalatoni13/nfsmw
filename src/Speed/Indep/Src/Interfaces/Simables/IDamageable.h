#ifndef INTERFACES_SIMABLES_IDAMAGEABLE_H
#define INTERFACES_SIMABLES_IDAMAGEABLE_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/World/Damagezones.h"
#include "Speed/Indep/Src/World/VehicleFX.h"

class IDamageable : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IDamageable);

    virtual void SetInShock(float scale);
    virtual void SetShockForce(float f);
    virtual float InShock() const;
    virtual void ResetDamage();
    virtual float GetHealth() const;
    virtual bool IsDestroyed() const;
    virtual void Destroy();
    virtual struct DamageZone::Info GetZoneDamage() const;
};

class IDamageableVehicle : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IDamageableVehicle);

    virtual bool IsLightDamaged(VehicleFX::ID idx) const;
    virtual void DamageLight(VehicleFX::ID idx, bool b);
};

#endif
