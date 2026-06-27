#include "RBVehicle.h"
#include "RigidBody.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/collisionreactions.h"
#include "Speed/Indep/Src/Generated/AttribSys/GenericAccessor.h"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IRBVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

static const float Tweak_PlayerCogModifire = -0.125f;

BIND_BEHAVIOR_FACTORY(RBVehicle);

Behavior *RBVehicle::Construct(const BehaviorParams &params) {
    const RBComplexParams rp(params.fparams.Fetch<RBComplexParams>(UCrc32(UCRC32_BASE)));
    return new RBVehicle(params, rp);
}

RBVehicle::RBVehicle(const BehaviorParams &bp, const RBComplexParams &params)
    : RigidBody(bp, params),                 //
      IRBVehicle(bp.fowner),                 //
      mVehicle(nullptr),                     //
      mSuspension(nullptr),                  //
      mSpecs(this, 0),                       //
      mDeadOnWheels(0.0f), mFrame(0),        //
      mCollisionCOG(UMath::Vector3::kZero),  //
      mCollisionMass(0.0f),                  //
      mObjectCollisionsEnabled(true),        //
      mInvulnerableState(INVULNERABLE_NONE), //
      mInvulnerableTimer(0.0f),              //
      mLastPenetration(0.0f),                //
      mPlayerReactions((Attrib::Collection *)nullptr, 0, (UTL::COM::IUnknown *)nullptr) {
    this->GetOwner()->QueryInterface(&this->mVehicle);
    this->GetOwner()->QueryInterface(&this->mSuspension);

    this->mGeoms = (params.fgeoms != nullptr) ? params.fgeoms->fCollection : nullptr;
}

unsigned int RBVehicle::GetNumContactPoints() const {
    unsigned int numpoints = RigidBody::GetNumContactPoints();
    if (this->mSuspension != nullptr) {
        numpoints += this->mSuspension->GetNumWheelsOnGround();
    }
    return numpoints;
}

bool RBVehicle::IsInGroundContact() const {
    if ((this->mSuspension != nullptr) && this->mSuspension->GetNumWheelsOnGround() != 0) {
        return true;
    } else {
        return RigidBody::IsInGroundContact();
    }
}

bool RBVehicle_DisableVehicleVsVehicleCollisions = false;

bool RBVehicle::CanCollideWith(const RigidBody &other) const {
    return this->mObjectCollisionsEnabled ? RigidBody::CanCollideWith(other) : false;
}

void RBVehicle::OnBeginFrame(const float dT) {
    if (this->mVehicle->IsStaging() && IsModeling() && !this->mVehicle->IsAnimating()) {
        UMath::Vector3 position = this->GetPosition();
        RigidBody::OnBeginFrame(dT);
        UMath::Vector3 angular_vel = this->GetAngularVelocity();
        UMath::Vector3 linear_vel = this->GetLinearVelocity();

        this->ConvertWorldToLocal(linear_vel, false);
        this->ConvertWorldToLocal(angular_vel, false);
        linear_vel.x = 0.0f;
        linear_vel.z = 0.0f;
        angular_vel.y = 0.0f;
        position.y = this->GetPosition().y;
        this->ConvertLocalToWorld(linear_vel, false);
        this->ConvertLocalToWorld(angular_vel, false);

        this->SetPosition(position);
        this->SetAngularVelocity(angular_vel);
        this->SetLinearVelocity(linear_vel);
    } else {
        RigidBody::OnBeginFrame(dT);
    }
}

void RBVehicle::OnTaskSimulate(float dT) {
    RigidBody::OnTaskSimulate(dT);
    this->mFrame++;
    if ((this->mInvulnerableState != INVULNERABLE_NONE) && (Sim::GetTime() > this->mLastPenetration + 1.0f) &&
        (this->mInvulnerableTimer -= dT) <= 0.0f) {
        this->mInvulnerableTimer = 0.0f;
        this->mInvulnerableState = INVULNERABLE_NONE;
    }
    if ((this->mSuspension != nullptr) && GetVehicle()->IsDestroyed() && !this->GetOwner()->IsPlayer() &&
        (this->mSuspension->GetNumWheelsOnGround() > this->mSuspension->GetNumWheels() / 2)) {
        this->mDeadOnWheels += dT;
    } else {
        this->mDeadOnWheels = 0.0f;
    }
}

void RBVehicle::PlaceObject(const UMath::Matrix4 &orientMat, const UMath::Vector3 &initPos) {
    RigidBody::PlaceObject(orientMat, initPos);
    this->mDeadOnWheels = 0.0f;
}

bool RBVehicle::ShouldSleep() const {
    if (this->GetVehicle()->IsAnimating() || HasHadObjectCollision()) {
        return false;
    }
    if ((this->mSuspension != nullptr) && this->mSuspension->GetNumWheelsOnGround() != 0 && this->mDeadOnWheels < 2.0f) {
        return false;
    }
    return RigidBody::ShouldSleep();
}

static const float Tweak_VehicleWallCGMod = 0.8f;
static const float Tweak_HeadOnDampStart = 0.0f;
static const float Tweak_HeadOnDampEnd = 5.0f;
static const float Tweak_HeadOnDamp = 8.0f;
static const float Tweak_HeadOnDot = -1.0f;
static const float Tweak_HeadOnElasticity = 0.0f;

void RBVehicle::ModifyCollision(const SimSurface &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment) {
    RigidBody::ModifyCollision(other, plane, myMoment);
    if (this->GetOwner()->IsPlayer()) {
        float nose_dot = -UMath::Dot(plane.normal, this->GetForwardVector());
        if (nose_dot >= Tweak_HeadOnDot) {
            float closing_speed = UMath::Ramp(-UMath::Dot(plane.normal, this->GetLinearVelocity()), Tweak_HeadOnDampStart, Tweak_HeadOnDampEnd);
            float damp = UMath::Ramp(nose_dot, -1.0f, 1.0f);
            UMath::Vector3 inertia = myMoment.GetInertia();
            myMoment.SetElasticity(0.0f);
            inertia.y *= closing_speed * damp * Tweak_HeadOnDamp + 1.0f;
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

    UMath::Sub(plane.point, this->GetPosition(), rp);
    float dot_front = UMath::Dot(rp, this->GetForwardVector());
    float dot_right = UMath::Dot(rp, this->GetRightVector());
    float dot_normal = UMath::Abs(UMath::Dot(plane.normal, this->GetForwardVector()));
    if (dot_front > 0.0f) {
        if (dot_normal < 0.707f) {
            return this->mPlayerReactions.FRONTSIDE_REACTION();
        } else {
            return this->mPlayerReactions.FRONT_REACTION();
        }
    } else {
        if (dot_normal < 0.707f) {
            return this->mPlayerReactions.REARSIDE_REACTION();
        } else {
            return this->mPlayerReactions.REAR_REACTION();
        }
    }
}

void RBVehicle::ModifyCollision(const RigidBody &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment) {
    RigidBody::ModifyCollision(other, plane, myMoment);
    SimableType type = other.GetSimableType();
    if (this->GetVehicle()->IsAnimating()) {
        myMoment.MakeImmobile(true, 0.0f);
    } else {
        if (type == SIMABLE_VEHICLE) {
            IVehicle *other_vehicle;
            ISuspension *other_susp;
            if (this->GetOwner()->IsPlayer()) {
                if ((this->mSuspension != nullptr) && this->mSuspension->GetNumWheelsOnGround() > 2 &&
                    other.GetOwner()->QueryInterface(&other_susp) && other_susp->GetNumWheelsOnGround() > 2) {
                    UVector3 cg = myMoment.GetCG();
                    cg.y = plane.point.y - this->GetPosition().y + Tweak_PlayerCogModifire;
                    myMoment.SetCG(cg);
                }
            } else if (this->mPlayerReactions.IsValid() && other.GetOwner()->QueryInterface(&other_vehicle)) {
                DriverClass driver = other_vehicle->GetDriverClass();
                if (driver == DRIVER_RACER || driver == DRIVER_HUMAN) {
                    const CollisionReactionRecord &reaction = this->ChooseReaction(plane);
                    myMoment.SetElasticity(UMath::Clamp(myMoment.GetElasticity() + reaction.Elasticity + Tweak_HeadOnElasticity, 0.0f, 1.0f));
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
    if ((this->mCollisionMass > UMath::Epsilon) && !myMoment.IsImmobile()) {
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
        this->GetOwner()->QueryInterface(&this->mSuspension);
    }
}

bool RBVehicle::CanCollideWithGround() const {
    if (!this->GetOwner()->IsPlayer() && (this->mSuspension != nullptr)) {
        if (this->mSuspension->GetNumWheels() == this->mSuspension->GetNumWheelsOnGround() && this->mSuspension->GetNumWheels() > 2 &&
            !HasHadObjectCollision()) {
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

    if (this->GetOwner()->IsPlayer()) {
        flag |= 4;
    } else {
        flag |= 8;
    }
    if (this->GetVehicle()->GetDriverClass() == DRIVER_COP) {
        flag |= 0x20000;
    }

    return flag;
}

bool RBVehicle::CanCollideWithWorld() const {
    if ((this->GetVehicle()->GetDriverClass() == DRIVER_TRAFFIC) && !this->HasHadCollision()) {
        float velSquare = UMath::LengthSquare(this->GetLinearVelocity());
        if (velSquare < 4.0f) {
            if (this->mFrame & 3) {
                return false;
            }
        } else if (velSquare < 225.0f && (this->mFrame & 1)) {
            return false;
        }
    }
    if (this->GetOwner()->IsPlayer()) {
        return true;
    } else {
        return RigidBody::CanCollideWithWorld();
    }
}
