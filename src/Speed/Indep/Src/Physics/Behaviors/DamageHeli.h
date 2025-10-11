#ifndef PHYSICS_BEHAVIORS_DAMAGEHELI_H
#define PHYSICS_BEHAVIORS_DAMAGEHELI_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "DamageVehicle.h"

// total size: 0xC0
class DamageHeli : public DamageVehicle {
  public:
    static Behavior *Construct(const BehaviorParams &params);

    DamageHeli(const BehaviorParams &bp, const DamageParams &dp);
    void DoAutoDestruct();

    // Overrides
    // IUnknown
    override virtual ~DamageHeli();

    // Behavior
    override virtual void Reset();
    override virtual void OnTaskSimulate(float dT);

  private:
    int mAutoDestruct; // offset 0xB8, size 0x4
    int mDestroying;   // offset 0xBC, size 0x4
};

#endif
