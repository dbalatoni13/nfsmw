#include "RBTractor.h"
#include "Speed/Indep/Src/Generated/Events/EJointDetached.hpp"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"
#include "Speed/Indep/Src/Physics/Bounds.h"
#include "Speed/Indep/Src/Physics/Dynamics.h"
#include "rbvehicle.h"

Behavior *RBTractor::Construct(const struct BehaviorParams &params) {
    const RBComplexParams rp(params.fparams.Fetch<RBComplexParams>(UCrc32(0xa6b47fac)));
    return new RBTractor(params, rp);
}

void RBTractor::SetHitch(bool hitched) {
    if (hitched) {
        float YAW_LIMIT;
        float PITCHROLL_LIMIT;
        CollisionGeometry::IBoundable *ibounds_trailer;
        if (mTrailer && !mHitched && mTrailer->QueryInterface(&ibounds_trailer)) {
            UCrc32 name5thwheel("5THWHEEL");
            const Attrib::Collection *tractor_col = CollisionGeometry::Lookup(UCrc32(GetVehicle()->GetVehicleAttributes().MODEL()));
            const Attrib::Collection *trailer_col = CollisionGeometry::Lookup(UCrc32(mTrailer->GetVehicleAttributes().MODEL()));

            mHitched =
                CollisionGeometry::CreateJoint(this, UCrc32(name5thwheel), ibounds_trailer, UCrc32(name5thwheel), &m5thWheel, &mTrailer5thWheel, 0);
            if (mHitched) {
                Pose();
            }
        }
        if (!mTrailerTask) {
            mTrailerTask = AddTask(UCrc32("Physics"), 0.1f, 0.0f, Sim::TASK_FRAME_FIXED);
        }
    } else {
        if (mHitched && mTrailer) {
            IInput *iinput;
            if (mTrailer->QueryInterface(&iinput)) {
                iinput->SetControlBrake(0.0f);
                iinput->SetControlHandBrake(0.0f);
            }
            Dynamics::Articulation::Release(this);
            mHitched = false;
            new EJointDetached(GetOwner()->GetInstanceHandle());
            new EJointDetached(mTrailer->GetSimable()->GetInstanceHandle());
        }
        if (mTrailerTask) {
            RemoveTask(mTrailerTask);
            mTrailerTask = nullptr;
        }
    }
}

void RBTractor::ModifyCollision(const RigidBody &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment) {
    RigidBody::ModifyCollision(other, plane, myMoment);

    if (other.IsImmobile()) {
        myMoment.MakeImmobile(false, 0.0f);
    }
}

void RBTractor::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        GetOwner()->QueryInterface(&mIInput);
    }
    RBVehicle::OnBehaviorChange(mechanic);
}

eVehicleCacheResult RBTractor::OnQueryVehicleCache(const IVehicle *removethis, const IVehicleCache *whosasking) const {
    // TODO
}

void RBTractor::OnRemovedVehicleCache(IVehicle *ivehicle) {
    // TODO huh?
    if (!ivehicle || !mTrailer) {
        if (!ivehicle) {
            if (!mTrailer) {
                mTrailer = nullptr;
            }
        }
    } else {
        if (!ComparePtr(ivehicle, mTrailer)) {
            mTrailer = nullptr;
        }
    }
}

// UNSOLVED
void RBTractor::OnOwnerDetached(IAttachable *pOther) {
    if (mTrailer && pOther && ComparePtr(pOther, mTrailer)) {
        mTrailer->GetSimable()->Kill();
        mTrailer = nullptr;
    }
    Behavior::OnOwnerDetached(pOther);
}

// UNSOLVED branching
bool RBTractor::CanCollideWith(const RigidBody &other) const {
    if (mTrailer && mHitched) {
        // TODO huh
        if (&other == (RigidBody *)0xffffffb4) {
            return false;
        }
        if (ComparePtr(mTrailer, static_cast<const IRigidBody *>(&other))) {
            return false;
        }
    }
    return RigidBody::CanCollideWith(other);
}

void RBTractor::UpdateTrailer(float dT) {
    IInput *iinput;

    if (mIInput && mTrailer->QueryInterface(&iinput)) {
        if (mHitched) {
            float brake = mIInput->GetControls().fBrake;
            float ebrake = mIInput->GetControls().fHandBrake;
            iinput->SetControlBrake(brake);
            iinput->SetControlHandBrake(ebrake);
        } else {
            iinput->SetControlBrake(0.0f);
            iinput->SetControlHandBrake(0.0f);
        }
    }
    if (mHitched) {
        if (GetUpVector().y < 0.75f) {
            SetHitch(0.0f);
        }
        ICollisionBody *trailer_body;
        if (mTrailer->QueryInterface(&trailer_body)) {
            if (trailer_body->GetUpVector().y < 0.75f) {
                SetHitch(false);
            }
            if (UMath::Dot(trailer_body->GetUpVector(), GetUpVector()) < 0.8f) {
                SetHitch(false);
            }
        }
        ISuspension *tractor_suspension;
        ISuspension *trailer_suspension;
        if (mTrailer->QueryInterface(&tractor_suspension) && GetOwner()->QueryInterface(&trailer_suspension)) {
            if (tractor_suspension->GetNumWheelsOnGround() < 2 || trailer_suspension->GetNumWheelsOnGround() < 2) {
                mDetachTimer += dT;
            } else {
                mDetachTimer = 0.0f;
            }
            if (mDetachTimer > 2.0f) {
                SetHitch(false);
            }
        }
    }
}

bool RBTractor::OnTask(HSIMTASK htask, float dT) {
    Sim::ITaskable::OnTask(htask, dT);

    if (htask == mTrailerTask) {
        if (mTrailer && mTrailer->IsActive()) {
            UpdateTrailer(dT);
        }
        return true;
    } else {
        return false;
    }
}

void RBTractor::PlaceObject(const UMath::Matrix4 &orientMat, const UMath::Vector3 &initPos) {
    RBVehicle::PlaceObject(orientMat, initPos);
    if (mTrailer) {
        SetHitch(true);
        Pose();
    }
}

bool RBTractor::Pose() {
    if (!mTrailer || !mHitched) {
        return false;
    }
    mDetachTimer = 0.0f;

    UMath::Vector3 initialVec;
    GetForwardVector(initialVec);

    UMath::Matrix4 mat;
    GetMatrix4(mat);

    UVector3 resetPos(m5thWheel);
    resetPos -= mTrailer5thWheel;
    resetPos *= mat;
    resetPos += GetPosition();

    return mTrailer->SetVehicleOnGround(resetPos, initialVec);
}

Behavior *RBTrailer::Construct(const struct BehaviorParams &params) {
    const RBComplexParams rp(params.fparams.Fetch<RBComplexParams>(UCrc32(0xa6b47fac)));
    return new RBTrailer(params, rp);
}

RBTrailer::RBTrailer(const BehaviorParams &bp, const RBComplexParams &params) : RBVehicle(bp, params) {}

void RBTrailer::ModifyCollision(const RigidBody &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment) {
    RigidBody::ModifyCollision(other, plane, myMoment);
    if (other.IsImmobile()) {
        myMoment.MakeImmobile(false, 0.0f);
    }
}
