#include "RBCop.h"

Behavior *RBCop::Construct(const struct BehaviorParams &params) {
    const RBComplexParams rp(params.fparams.Fetch<RBComplexParams>(UCrc32(0xa6b47fac)));
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
