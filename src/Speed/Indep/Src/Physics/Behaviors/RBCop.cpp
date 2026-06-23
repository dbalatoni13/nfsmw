#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Physics/Behaviors/RBVehicle.h"

// total size: 0x1A4
class RBCop : public RBVehicle {
  public:
    typedef RBVehicle Base;

    static Behavior *Construct(const BehaviorParams &params);

    RBCop(const BehaviorParams &bp, const RBComplexParams &params);

  protected:
    // IUnknown
    ~RBCop() override;

    // RigidBody
    void ModifyCollision(const RigidBody &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment) override;
    void ModifyCollision(const SimSurface &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment) override;
};

BIND_BEHAVIOR_FACTORY(RBCop);

Behavior *RBCop::Construct(const BehaviorParams &params) {
    const RBComplexParams rp(params.fparams.Fetch<RBComplexParams>(UCrc32(UCRC32_BASE)));
    return new RBCop(params, rp);
}

RBCop::RBCop(const BehaviorParams &bp, const RBComplexParams &params) : RBVehicle(bp, params) {}

RBCop::~RBCop() {}

void RBCop::ModifyCollision(const RigidBody &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment) {
    RigidBody::ModifyCollision(other, plane, myMoment);
}

void RBCop::ModifyCollision(const SimSurface &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment) {
    RigidBody::ModifyCollision(other, plane, myMoment);
}
