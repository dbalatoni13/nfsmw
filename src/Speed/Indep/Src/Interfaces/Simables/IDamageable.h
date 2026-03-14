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
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IDamageable(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IDamageable() {}

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
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IDamageableVehicle(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IDamageableVehicle() {}

    virtual bool IsLightDamaged(VehicleFX::ID idx) const = 0;
    virtual void DamageLight(VehicleFX::ID idx, bool b) = 0;
};

#endif
