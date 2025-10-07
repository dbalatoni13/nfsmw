#ifndef PHYSICS_BEHAVIORS_RBCOP_H
#define PHYSICS_BEHAVIORS_RBCOP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "RBVehicle.h"

// total size: 0x1A4
class RBCop : public RBVehicle {
  public:
    static Behavior *Construct(const BehaviorParams &params);

    RBCop(const BehaviorParams &bp, const RBComplexParams &params);

    // Overrides
    // IUnknown
    override virtual ~RBCop();

    // RigidBody
    override virtual void ModifyCollision(const struct RigidBody &other, const Dynamics::Collision::Plane &plane,
                                          Dynamics::Collision::Moment &myMoment);
    override virtual void ModifyCollision(const struct SimSurface &other, const Dynamics::Collision::Plane &plane,
                                          Dynamics::Collision::Moment &myMoment);
};

#endif
