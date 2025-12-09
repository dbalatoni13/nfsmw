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
    ~RBCop() override;

    // RigidBody
    void ModifyCollision(const struct RigidBody &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment) override;
    void ModifyCollision(const struct SimSurface &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment) override;
};

#endif
