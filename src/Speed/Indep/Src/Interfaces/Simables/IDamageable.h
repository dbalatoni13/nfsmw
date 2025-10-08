#ifndef INTERFACES_SIMABLES_IDAMAGEABLE_H
#define INTERFACES_SIMABLES_IDAMAGEABLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Src/World/Damagezones.h"

class IDamageable : public UTL::COM::IUnknown {
  public:
    IDamageable(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    virtual ~IDamageable() {}

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

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
    IDamageableVehicle(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    virtual ~IDamageableVehicle() {}

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    virtual bool IsLightDamaged(VehicleFX::ID idx) const;
    virtual void DamageLight(VehicleFX::ID idx, bool b);
};

#endif
