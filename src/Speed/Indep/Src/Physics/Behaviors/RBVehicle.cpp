#include "RBVehicle.h"
#include "RigidBody.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/collisionreactions.h"
#include "Speed/Indep/Src/Generated/AttribSys/GenericAccessor.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRBVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

#include <types.h>

Behavior *RBVehicle::Construct(const BehaviorParams &params) {
    const RBComplexParams rp(params.fparams.Fetch<RBComplexParams>(UCrc32(0xa6b47fac)));
    return new RBVehicle(params, rp);
}

RBVehicle::RBVehicle(const BehaviorParams &bp, const RBComplexParams &params)
    : RigidBody(bp, params), IRBVehicle(bp.fowner), mVehicle(nullptr), mSuspension(nullptr), mSpecs(this, 0), mDeadOnWheels(0.0f), mFrame(0),
      mCollisionCOG(UMath::Vector3::kZero), mCollisionMass(0.0f), mObjectCollisionsEnabled(true), mInvulnerableState(INVULNERABLE_NONE),
      mInvulnerableTimer(0.0f), mLastPenetration(0.0f), mPlayerReactions((Attrib::Collection *)nullptr, 0, (UTL::COM::IUnknown *)nullptr) {
    GetOwner()->QueryInterface(&mVehicle);
    GetOwner()->QueryInterface(&mSuspension);

    mGeoms = params.fgeoms ? params.fgeoms->fCollection : nullptr;
}

unsigned int RBVehicle::GetNumContactPoints() const {
    unsigned int numpoints = RigidBody::GetNumContactPoints();
    if (mSuspension) {
        numpoints += mSuspension->GetNumWheelsOnGround();
    }
    return numpoints;
}

bool RBVehicle::IsInGroundContact() const {
    if (mSuspension && mSuspension->GetNumWheelsOnGround() != 0) {
        return true;
    } else {
        return RigidBody::IsInGroundContact();
    }
}

bool RBVehicle::CanCollideWith(const RigidBody &other) const {
    return mObjectCollisionsEnabled ? RigidBody::CanCollideWith(other) : false;
}

void RBVehicle::OnBeginFrame(const float dT) {
    if (mVehicle->IsStaging() && IsModeling() && !mVehicle->IsAnimating()) {
        UMath::Vector3 position = GetPosition();
        RigidBody::OnBeginFrame(dT);
        UMath::Vector3 angular_vel = GetAngularVelocity();
        UMath::Vector3 linear_vel = GetLinearVelocity();

        ConvertWorldToLocal(linear_vel, false);
        ConvertWorldToLocal(angular_vel, false);
        linear_vel.x = 0.0f;
        linear_vel.z = 0.0f;
        angular_vel.y = 0.0f;
        position.y = GetPosition().y;
        ConvertLocalToWorld(linear_vel, false);
        ConvertLocalToWorld(angular_vel, false);

        SetPosition(position);
        SetAngularVelocity(angular_vel);
        SetLinearVelocity(linear_vel);
    } else {
        RigidBody::OnBeginFrame(dT);
    }
}

void RBVehicle::OnTaskSimulate(float dT) {
    Behavior::OnTaskSimulate(dT);
    mFrame++;
    if ((mInvulnerableState != INVULNERABLE_NONE) && (Sim::GetTime() > mLastPenetration + 1.0f) && (mInvulnerableTimer -= dT) <= 0.0f) {
        mInvulnerableTimer = 0.0f;
        mInvulnerableState = INVULNERABLE_NONE;
    }
    if (mSuspension && GetVehicle()->IsDestroyed() && !GetOwner()->IsPlayer() &&
        (mSuspension->GetNumWheelsOnGround() > mSuspension->GetNumWheels() / 2)) {
        mDeadOnWheels += dT;
    } else {
        mDeadOnWheels = 0.0f;
    }
}

void RBVehicle::PlaceObject(const UMath::Matrix4 &orientMat, const UMath::Vector3 &initPos) {
    RigidBody::PlaceObject(orientMat, initPos);
    mDeadOnWheels = 0.0f;
}

bool RBVehicle::ShouldSleep() const {
    if (GetVehicle()->IsAnimating() || HasHadObjectCollision()) {
        return false;
    }
    if (mSuspension && mSuspension->GetNumWheelsOnGround() != 0 && mDeadOnWheels < 2.0f) {
        return false;
    }
    return RigidBody::ShouldSleep();
}

void RBVehicle::ModifyCollision(const SimSurface &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment) {
    RigidBody::ModifyCollision(other, plane, myMoment);
    if (GetOwner()->IsPlayer()) {
        float nose_dot = -UMath::Dot(plane.normal, GetForwardVector());
        if (nose_dot >= -1.0f) {
            float closing_speed = UMath::Ramp(-UMath::Dot(plane.normal, GetLinearVelocity()), 0.0f, 10.0f);
            float damp = UMath::Ramp(nose_dot, -1.0f, 1.0f);
            UMath::Vector3 inertia = myMoment.GetInertia();
            myMoment.SetElasticity(0.0f);
            inertia.y *= closing_speed * damp * 8.0f + 1.0f;
            myMoment.SetInertia(inertia);
        }
        UMath::Vector3 cog = myMoment.GetCG();
        cog.y = 0.0f;
        myMoment.SetCG(cog);
    } else {
        UMath::Vector3 cog = myMoment.GetCG();
        cog.y = 0.0f;
        myMoment.SetCG(cog);
    }
}

const CollisionReactionRecord &RBVehicle::ChooseReaction(const Dynamics::Collision::Plane &plane) const {
    UMath::Vector3 rp;

    UMath::Sub(plane.point, GetPosition(), rp);
    float dot_front = UMath::Dot(rp, GetForwardVector());
    float dot_right = UMath::Dot(rp, GetRightVector());
    float dot_normal = UMath::Abs(UMath::Dot(plane.normal, GetForwardVector()));
    if (dot_front > 0.0f) {
        if (dot_normal < 0.707f) {
            return mPlayerReactions.FRONTSIDE_REACTION();
        } else {
            return mPlayerReactions.FRONT_REACTION();
        }
    } else {
        if (dot_normal < 0.707f) {
            return mPlayerReactions.REARSIDE_REACTION();
        } else {
            return mPlayerReactions.REAR_REACTION();
        }
    }
}

void RBVehicle::ModifyCollision(const RigidBody &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment) {
    RigidBody::ModifyCollision(other, plane, myMoment);
    SimableType type = other.GetSimableType();
    if (GetVehicle()->IsAnimating()) {
        myMoment.MakeImmobile(true, 0.0f);
    } else {
        if (type == SIMABLE_VEHICLE) {
            IVehicle *other_vehicle;
            ISuspension *other_susp;
            if (GetOwner()->IsPlayer()) {
                if (mSuspension && mSuspension->GetNumWheelsOnGround() > 2 && other.GetOwner()->QueryInterface(&other_susp) &&
                    other_susp->GetNumWheelsOnGround() > 2) {
                    UVector3 cg = myMoment.GetCG();
                    cg.y = plane.point.y - GetPosition().y + -0.125f;
                    myMoment.SetCG(cg);
                }
            } else if (mPlayerReactions.IsValid() && other.GetOwner()->QueryInterface(&other_vehicle)) {
                DriverClass driver = other_vehicle->GetDriverClass();
                if (driver == DRIVER_RACER || driver == DRIVER_HUMAN) {
                    const CollisionReactionRecord &reaction = ChooseReaction(plane);
                    myMoment.SetElasticity(UMath::Clamp(myMoment.GetElasticity() + reaction.Elasticity, 0.0f, 1.0f));
                    if (reaction.MassScale > 0.0f) {
                        myMoment.SetInertia(myMoment.GetInertia() * reaction.MassScale);
                        myMoment.SetMass(myMoment.GetMass() * reaction.MassScale);
                    }
                    UVector3 cg = myMoment.GetCG();
                    cg.y += reaction.RollHeight;
                    cg.z += reaction.WeightBias;
                    myMoment.SetCG(cg);
                }
            }
        } else if ((type == SIMABLE_SMACKABLE) && !other.IsImmobile()) {
            bool no_car_effect = false;
            // TODO UNSOLVED, it saves sp28 to r31 instead of always recalculating it
            if (other.GetOwner()->GetAttributes()->NO_CAR_EFFECT(no_car_effect, 0) && no_car_effect) {
                myMoment.MakeImmobile(true, 0.0f);
            }
        }
    }
    if ((mCollisionMass > FLOAT_EPSILON) && !myMoment.IsImmobile()) {
        float tensorscale = mCollisionMass * myMoment.GetOOMass();
        myMoment.SetInertia(myMoment.GetInertia() * tensorscale);
        myMoment.SetMass(mCollisionMass);
    }
    UMath::Vector3 cog;
    UMath::Add(myMoment.GetCG(), mCollisionCOG, cog);
    myMoment.SetCG(cog);
}

void RBVehicle::OnBehaviorChange(const UCrc32 &mechanic) {
    Behavior::OnBehaviorChange(mechanic);
    if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
        GetOwner()->QueryInterface(&mSuspension);
    }
}

bool RBVehicle::CanCollideWithGround() const {
    if (!GetOwner()->IsPlayer() && mSuspension) {
        if (mSuspension->GetNumWheels() == mSuspension->GetNumWheelsOnGround() && mSuspension->GetNumWheels() > 2 && !HasHadObjectCollision()) {
            return false;
        }
    }
    return RigidBody::CanCollideWithGround();
}

unsigned int RBVehicle::GetTriggerFlags() const {
    unsigned int flag = RigidBody::GetTriggerFlags();

    if (flag == 0) {
        return flag;
    }
    flag |= 0x20;

    if (GetOwner()->IsPlayer()) {
        flag |= 4;
    } else {
        flag |= 8;
    }
    if (GetVehicle()->GetDriverClass() == 2) {
        flag |= 0x20000;
    }

    return flag;
}

bool RBVehicle::CanCollideWithWorld() const {
    if ((GetVehicle()->GetDriverClass() == DRIVER_TRAFFIC) && !HasHadCollision()) {
        float velSquare = UMath::LengthSquare(GetLinearVelocity());
        if (velSquare < 4.0f) {
            if (mFrame & 3) {
                return false;
            }
        } else if (velSquare < 225.0f && (mFrame & 1)) {
            return false;
        }
    }
    if (GetOwner()->IsPlayer()) {
        return true;
    } else {
        return RigidBody::CanCollideWithWorld();
    }
}
