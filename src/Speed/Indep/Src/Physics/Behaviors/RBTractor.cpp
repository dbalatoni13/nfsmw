#include "Speed/Indep/Src/Generated/Events/EJointDetached.hpp"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"
#include "Speed/Indep/Src/Interfaces/Simables/IArticulatedVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Physics/Behaviors/RBVehicle.h"
#include "Speed/Indep/Src/Physics/Bounds.h"
#include "Speed/Indep/Src/Physics/Dynamics.h"

static const float Tweak_OffGroundDetachTrailerTimer = 2.0f;
static const float Tweak_TrailerTractorDetachLimit = 0.8f;
static const float Tweak_TrailerUprightDetachLimit = 0.75f;

// total size: 0x1E4
class RBTractor : public RBVehicle, public IArticulatedVehicle, public IVehicleCache {
  public:
    typedef RBVehicle BASE;

    static Behavior *Construct(const BehaviorParams &params);

    RBTractor(const BehaviorParams &bp, const RBComplexParams &params);

  protected:
    // IUnknown
    ~RBTractor() override;

    void SetInvulnerability(eInvulnerablitiy state, float time) override {}

    // IArticulatedVehicle
    IVehicle *GetTrailer() const override {
        return mTrailer;
    }

    void SetHitch(bool hitched) override;
    bool Pose() override;

    bool IsHitched() const override {
        return mHitched;
    }

    // ITaskable
    bool OnTask(HSIMTASK htask, float dT) override;

    // Behavior
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    // IRigidBody
    void PlaceObject(const UMath::Matrix4 &orientMat, const UMath::Vector3 &initPos) override;

    // RigidBody
    bool CanCollideWith(const struct RigidBody &other) const override;
    void OnOwnerDetached(IAttachable *pOther) override;

    // IVehicleCache
    eVehicleCacheResult OnQueryVehicleCache(const IVehicle *removethis, const IVehicleCache *whosasking) const override;
    void OnRemovedVehicleCache(IVehicle *ivehicle) override;

#ifndef EA_BUILD_A124
    // IVehicleCache
    const char *GetCacheName() const override {
        return "RBTractor";
    }
#endif

    // RigidBody
    void ModifyCollision(const RigidBody &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment) override;

  private:
    void UpdateTrailer(float dT);

    IVehicle *mTrailer;                       // offset 0x1B8, size 0x4
    IInput *mIInput;                          // offset 0x1BC, size 0x4
    HSIMTASK mTrailerTask;                    // offset 0x1C0, size 0x4
    bool mHitched;                            // offset 0x1C4, size 0x1
    ALIGNVEC UMath::Vector3 m5thWheel;        // offset 0x1C8, size 0xC
    ALIGNVEC UMath::Vector3 mTrailer5thWheel; // offset 0x1D4, size 0xC
    Seconds mDetachTimer;                     // offset 0x1E0, size 0x4
};

BIND_BEHAVIOR_FACTORY(RBTractor);

Behavior *RBTractor::Construct(const BehaviorParams &params) {
    const RBComplexParams rp(params.fparams.Fetch<RBComplexParams>(UCrc32(UCRC32_BASE)));
    return new RBTractor(params, rp);
}

void RBTractor::SetHitch(bool hitched) {
    if (hitched) {
        float YAW_LIMIT;
        float PITCHROLL_LIMIT;
        CollisionGeometry::IBoundable *ibounds_trailer;
        if ((this->mTrailer != nullptr) && !this->mHitched && this->mTrailer->QueryInterface(&ibounds_trailer)) {
            UCrc32 name5thwheel("5THWHEEL");
            const CollisionGeometry::Collection *tractor_col = CollisionGeometry::Lookup(UCrc32(this->GetVehicle()->GetVehicleAttributes().MODEL()));
            const CollisionGeometry::Collection *trailer_col = CollisionGeometry::Lookup(UCrc32(this->mTrailer->GetVehicleAttributes().MODEL()));

            this->mHitched =
                CollisionGeometry::CreateJoint(this, name5thwheel, ibounds_trailer, name5thwheel, &this->m5thWheel, &this->mTrailer5thWheel, 0);
            if (this->mHitched) {
                this->Pose();
            }
        }
        if (this->mTrailerTask == nullptr) {
            this->mTrailerTask = this->AddTask("Physics", 0.1f, 0.0f, Sim::TASK_FRAME_FIXED);
        }
    } else {
        if (this->mHitched && (mTrailer != nullptr)) {
            IInput *iinput;
            if (this->mTrailer->QueryInterface(&iinput)) {
                iinput->SetControlBrake(0.0f);
                iinput->SetControlHandBrake(0.0f);
            }
            Dynamics::Articulation::Release(this);
            this->mHitched = false;
            new EJointDetached(this->GetOwner()->GetInstanceHandle());
            new EJointDetached(this->mTrailer->GetSimable()->GetInstanceHandle());
        }
        if (this->mTrailerTask != nullptr) {
            this->RemoveTask(this->mTrailerTask);
            this->mTrailerTask = nullptr;
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
        this->GetOwner()->QueryInterface(&this->mIInput);
    }
    RBVehicle::OnBehaviorChange(mechanic);
}

eVehicleCacheResult RBTractor::OnQueryVehicleCache(const IVehicle *removethis, const IVehicleCache *whosasking) const {
    return ComparePtr(removethis, this->mTrailer) ? VCR_WANT : VCR_DONTCARE;
}

void RBTractor::OnRemovedVehicleCache(IVehicle *ivehicle) {
    if (ComparePtr(ivehicle, this->mTrailer)) {
        this->mTrailer = nullptr;
    }
}

void RBTractor::OnOwnerDetached(IAttachable *pOther) {
    if ((this->mTrailer != nullptr) && ComparePtr(pOther, this->mTrailer)) {
        this->mTrailer->GetSimable()->Kill();
        this->mTrailer = nullptr;
    }
    Behavior::OnOwnerDetached(pOther);
}

bool RBTractor::CanCollideWith(const RigidBody &other) const {
    if ((this->mTrailer != nullptr) && this->mHitched) {
        if (ComparePtr(this->mTrailer, static_cast<const IRigidBody *>(&other))) {
            return false;
        }
    }
    return RigidBody::CanCollideWith(other);
}

void RBTractor::UpdateTrailer(float dT) {
    IInput *iinput;

    if ((this->mIInput != nullptr) && this->mTrailer->QueryInterface(&iinput)) {
        if (this->mHitched) {
            float brake = this->mIInput->GetControls().fBrake;
            float ebrake = this->mIInput->GetControls().fHandBrake;
            iinput->SetControlBrake(brake);
            iinput->SetControlHandBrake(ebrake);
        } else {
            iinput->SetControlBrake(0.0f);
            iinput->SetControlHandBrake(0.0f);
        }
    }
    if (this->mHitched) {
        if (this->GetUpVector().y < Tweak_TrailerUprightDetachLimit) {
            this->SetHitch(false);
        }
        ICollisionBody *trailer_body;
        if (this->mTrailer->QueryInterface(&trailer_body)) {
            if (trailer_body->GetUpVector().y < Tweak_TrailerUprightDetachLimit) {
                this->SetHitch(false);
            }
            if (UMath::Dot(trailer_body->GetUpVector(), this->GetUpVector()) < Tweak_TrailerTractorDetachLimit) {
                this->SetHitch(false);
            }
        }
        ISuspension *tractor_suspension;
        ISuspension *trailer_suspension;
        if (this->mTrailer->QueryInterface(&tractor_suspension) && this->GetOwner()->QueryInterface(&trailer_suspension)) {
            if (tractor_suspension->GetNumWheelsOnGround() < 2 || trailer_suspension->GetNumWheelsOnGround() < 2) {
                this->mDetachTimer += dT;
            } else {
                this->mDetachTimer = 0.0f;
            }
            if (this->mDetachTimer > Tweak_OffGroundDetachTrailerTimer) {
                this->SetHitch(false);
            }
        }
    }
}

bool RBTractor::OnTask(HSIMTASK htask, float dT) {
    if (htask == mTrailerTask) {
        if ((this->mTrailer != nullptr) && this->mTrailer->IsActive()) {
            this->UpdateTrailer(dT);
        }
        return true;
    } else {
        Sim::Object::OnTask(htask, dT);
        return false;
    }
}

void RBTractor::PlaceObject(const UMath::Matrix4 &orientMat, const UMath::Vector3 &initPos) {
    RBVehicle::PlaceObject(orientMat, initPos);
    if (this->mTrailer != nullptr) {
        this->SetHitch(true);
        this->Pose();
    }
}

// RBTractor::RBTractor(const BehaviorParams &bp, const RBComplexParams &params) {}

bool RBTractor::Pose() {
    if ((this->mTrailer == nullptr) || !this->mHitched) {
        return false;
    }
    this->mDetachTimer = 0.0f;

    UMath::Vector3 initialVec;
    this->GetForwardVector(initialVec);

    UMath::Matrix4 mat;
    this->GetMatrix4(mat);

    UVector3 resetPos(m5thWheel);
    resetPos -= mTrailer5thWheel;
    resetPos *= mat;
    resetPos += this->GetPosition();

    return this->mTrailer->SetVehicleOnGround(resetPos, initialVec);
}

// total size: 0x1A4
class RBTrailer : public RBVehicle {
  public:
    static Behavior *Construct(const BehaviorParams &params);

  protected:
    RBTrailer(const BehaviorParams &bp, const RBComplexParams &params);
    ~RBTrailer() override;

    // RigidBody
    void ModifyCollision(const RigidBody &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment) override;
};

BIND_BEHAVIOR_FACTORY(RBTrailer);

RBTrailer::RBTrailer(const BehaviorParams &bp, const RBComplexParams &params) : RBVehicle(bp, params) {}

Behavior *RBTrailer::Construct(const struct BehaviorParams &params) {
    const RBComplexParams rp(params.fparams.Fetch<RBComplexParams>(UCrc32(UCRC32_BASE)));
    return new RBTrailer(params, rp);
}

void RBTrailer::ModifyCollision(const RigidBody &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment) {
    RigidBody::ModifyCollision(other, plane, myMoment);
    if (other.IsImmobile()) {
        myMoment.MakeImmobile(false, 0.0f);
    }
}
