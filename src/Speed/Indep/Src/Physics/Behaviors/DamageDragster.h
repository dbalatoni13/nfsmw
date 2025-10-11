#ifndef PHYSICS_BEHAVIORS_DAMAGEDRAGSTER_H
#define PHYSICS_BEHAVIORS_DAMAGEDRAGSTER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "DamageRacer.h"
#include "Speed/Indep/Src/Sim/Collision.h"

// total size: 0xDC
class DamageDragster : public DamageRacer {
  public:
    static Behavior *Construct(const BehaviorParams &params);

    DamageDragster(const BehaviorParams &bp, const DamageParams &dp);
    void CheckTotaling(const COLLISION_INFO &cinfo);

    // Overrides
    // IUnknown
    override virtual ~DamageDragster();

    // IListener
    override virtual void OnCollision(const COLLISION_INFO &cinfo);
};

#endif
