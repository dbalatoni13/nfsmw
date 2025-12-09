#ifndef PHYSICS_BEHAVIORS_DAMAGEraCER_H
#define PHYSICS_BEHAVIORS_DAMAGEraCER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISpikeable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Physics/Behaviors/DamageVehicle.h"

// total size: 0xDC
class DamageRacer : public DamageVehicle, public ISpikeable {
  public:
    static Behavior *Construct(const BehaviorParams &params);

    DamageRacer(const BehaviorParams &bp, const DamageParams &dp);

    // Overrides
    // IUnknown
    ~DamageRacer() override;

    // ISpikeable
    eTireDamage GetTireDamage(unsigned int wheelId) const override;
    unsigned int GetNumBlowouts() const override;
    void Puncture(unsigned int wheelId) override;

    // IDamageableVehicle
    bool IsLightDamaged(VehicleFX::ID idx) const override;

    // DamageVehicle
    bool CanDamageVisuals() const override;

    // Behavior
    void OnTaskSimulate(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    // IDamageable
    DamageZone::Info GetZoneDamage() const override;
    void ResetDamage() override;

  private:
    ISuspension *mSuspension; // offset 0xC4, size 0x4
    float mBlowOutTimes[4];   // offset 0xC8, size 0x10
    unsigned char mDamage[4]; // offset 0xD8, size 0x4
};

#endif
