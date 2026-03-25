#include "RBTractor.h"
#include "Speed/Indep/Src/Generated/Events/EJointDetached.hpp"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"
#include "Speed/Indep/Src/Physics/Bounds.h"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Src/Physics/Dynamics.h"
#include "rbvehicle.h"

Behavior *RBTractor::Construct(const struct BehaviorParams &params) {
    const RBComplexParams rp(params.fparams.Fetch<RBComplexParams>(UCrc32(0xa6b47fac)));
    return new RBTractor(params, rp);
}

RBTractor::~RBTractor() {
    if (mHitched) {
        Dynamics::Articulation::Release(this);
        mHitched = false;
    }
    if (mTrailerTask) {
        RemoveTask(mTrailerTask);
        mTrailerTask = nullptr;
    }
}

void RBTractor::SetInvulnerability(eInvulnerablitiy state, float time) {
    if (mTrailer && IsHitched()) {
        IRBVehicle *irbv = nullptr;
        mTrailer->QueryInterface(&irbv);
        if (irbv) {
            irbv->SetInvulnerability(state, time);
        }
    }
    RBVehicle::SetInvulnerability(state, time);
}

void RBTractor::SetHitch(bool hitched) {
    if (hitched) {
        float YAW_LIMIT;
        float PITCHROLL_LIMIT;
        CollisionGeometry::IBoundable *ibounds_trailer;
        if (mTrailer && !mHitched && mTrailer->QueryInterface(&ibounds_trailer)) {
            UCrc32 name5thwheel("5THWHEEL");
            // TODO clanker
            // const Attrib::Collection *tractor_col = CollisionGeometry::Lookup(UCrc32(GetVehicle()->GetVehicleAttributes().MODEL()));
            // const Attrib::Collection *trailer_col = CollisionGeometry::Lookup(UCrc32(mTrailer->GetVehicleAttributes().MODEL()));

            mHitched = CollisionGeometry::CreateJoint(this, name5thwheel, ibounds_trailer, name5thwheel, &m5thWheel, &mTrailer5thWheel, 0);
            if (mHitched) {
                Pose();
            }
        }
        if (!mTrailerTask) {
            mTrailerTask = AddTask("Physics", 0.1f, 0.0f, Sim::TASK_FRAME_FIXED);
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
    return ComparePtr(removethis, mTrailer) ? VCR_WANT : VCR_DONTCARE;
}

void RBTractor::OnRemovedVehicleCache(IVehicle *ivehicle) {
    if (ComparePtr(ivehicle, mTrailer)) {
        mTrailer = nullptr;
    }
}

void RBTractor::OnOwnerDetached(IAttachable *pOther) {
    if (mTrailer && ComparePtr(pOther, mTrailer)) {
        mTrailer->GetSimable()->Kill();
        mTrailer = nullptr;
    }
    Behavior::OnOwnerDetached(pOther);
}

bool RBTractor::CanCollideWith(const RigidBody &other) const {
    if (mTrailer && mHitched) {
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
    if (htask == mTrailerTask) {
        if (mTrailer && mTrailer->IsActive()) {
            UpdateTrailer(dT);
        }
        return true;
    } else {
        Sim::Object::OnTask(htask, dT);
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

RBTractor::RBTractor(const BehaviorParams &bp, const RBComplexParams &params)
    : RBVehicle(bp, params), //
      IArticulatedVehicle(bp.fowner), //
      IVehicleCache(this), //
      mTrailer(nullptr), //
      mIInput(nullptr), //
      mTrailerTask(nullptr), //
      mHitched(false), //
      m5thWheel(UMath::Vector3::kZero), //
      mTrailer5thWheel(UMath::Vector3::kZero), //
      mDetachTimer(0.0f) {
    GetOwner()->QueryInterface(&mIInput);

    const Attrib::RefSpec &trailerRef = GetVehicle()->GetVehicleAttributes().Trailer();
    if (trailerRef.GetCollectionKey() != 0) {
        UMath::Vector3 initialVec;
        UMath::Vector3 initialPos;
        GetForwardVector(initialVec);
        initialPos = GetPosition();

        ISimable *isimable = UTL::COM::Factory<Sim::Param, ISimable, UCrc32>::CreateInstance(
            UCrc32("PVehicle"), VehicleParams(this, DRIVER_NONE, trailerRef.GetCollectionKey(), initialVec, initialPos, 4, nullptr, nullptr));
        if (isimable) {
            if (isimable->QueryInterface(&mTrailer)) {
                GetOwner()->Attach(isimable);
                SetHitch(true);
            } else {
                delete isimable;
            }
        }
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
