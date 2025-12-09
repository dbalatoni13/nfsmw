#ifndef PHYSICS_BEHAVIORS_DAMAGECOPCAR_H
#define PHYSICS_BEHAVIORS_DAMAGECOPCAR_H

#include "Speed/Indep/Src/Sim/SimSurface.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "DamageVehicle.h"

// total size: 0xC0
class DamageCopCar : public DamageVehicle {
  public:
    static Behavior *Construct(const BehaviorParams &params);

    DamageCopCar(const BehaviorParams &bp, const DamageParams &dp);
    void CheckUpright(float dT);

    // Overrides
    // IUnknown
    ~DamageCopCar() override;

    // IDamageable
    void ResetDamage() override;

    // ITaskable
    bool OnTask(HSIMTASK htask, float dT) override;

    // DamageVehicle
    void OnImpact(const UMath::Vector3 &arm, const UMath::Vector3 &normal, float force, float speed, const SimSurface &mysurface,
                  ISimable *iother) override;

  private:
    float mFlippedOver;  // offset 0xB8, size 0x4
    HSIMTASK mThinkTask; // offset 0xBC, size 0x4
};

#endif
