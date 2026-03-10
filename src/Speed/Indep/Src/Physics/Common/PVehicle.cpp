#include "Speed/Indep/Src/Physics/PVehicle.h"

#include "Speed/Indep/Src/FE/FECustomizationRecord.h"
#include "Speed/Indep/Src/Generated/Events/EPerfectLaunch.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAudible.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IArticulatedVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEffects.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRenderable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/Physics/Behavior.h"

extern Attrib::StringKey BEHAVIOR_MECHANIC_AI;
extern Attrib::StringKey BEHAVIOR_MECHANIC_AUDIO;
extern Attrib::StringKey BEHAVIOR_MECHANIC_DAMAGE;
extern Attrib::StringKey BEHAVIOR_MECHANIC_DRAW;
extern Attrib::StringKey BEHAVIOR_MECHANIC_ENGINE;
extern Attrib::StringKey BEHAVIOR_MECHANIC_INPUT;
extern Attrib::StringKey BEHAVIOR_MECHANIC_RIGIDBODY;
extern Attrib::StringKey BEHAVIOR_MECHANIC_SUSPENSION;

const ISimable *PVehicle::GetSimable() const { return static_cast<const ISimable *>(this); }

ISimable *PVehicle::GetSimable() { return static_cast<ISimable *>(this); }

Attrib::Gen::pvehicle &PVehicle::GetVehicleAttributes() const {
    return const_cast<Attrib::Gen::pvehicle &>(mAttributes);
}

const UCrc32 &PVehicle::GetVehicleClass() const { return mClass; }

const char *PVehicle::GetVehicleName() const { return mAttributes.CollectionName(); }

DriverClass PVehicle::GetDriverClass() const { return mDriverClass; }

DriverStyle PVehicle::GetDriverStyle() const { return mDriverStyle; }

float PVehicle::GetAbsoluteSpeed() const { return mAbsSpeed; }

float PVehicle::GetSpeedometer() const { return mSpeedometer; }

float PVehicle::GetSpeed() const { return mSpeed; }

bool PVehicle::IsOffWorld() const { return mOffWorld; }

bool PVehicle::IsStaging() const { return mStaging; }

float PVehicle::GetOffscreenTime() const { return mOffScreenTime; }

float PVehicle::GetOnScreenTime() const { return mOnScreenTime; }

bool PVehicle::IsCollidingWithSoftBarrier() { return false; }

bool PVehicle::IsAnimating() const { return mAnimating; }

PhysicsMode PVehicle::GetPhysicsMode() const { return mPhysicsMode; }

const char *PVehicle::GetCacheName() const { return mCacheName; }

IVehicleAI *PVehicle::GetAIVehiclePtr() const { return mAI; }

float PVehicle::GetSlipAngle() const { return mSlipAngle; }

const UMath::Vector3 &PVehicle::GetLocalVelocity() const { return mLocalVel; }

const FECustomizationRecord *PVehicle::GetCustomizations() const { return mCustomization; }

EventSequencer::IEngine *PVehicle::GetEventSequencer() { return mSequencer; }

char PVehicle::GetForceStop() { return mForceStop; }

bool PVehicle::IsGlareOn(VehicleFX::ID glare) { return (mGlareState & glare) != 0; }

bool PVehicle::IsActive() const { return mPhysicsMode != PHYSICS_MODE_INACTIVE; }

void PVehicle::GlareOn(VehicleFX::ID glare) { mGlareState |= glare; }

void PVehicle::GlareOff(VehicleFX::ID glare) { mGlareState &= ~glare; }

void PVehicle::DebugObject() { PhysicsObject::DebugObject(); }

void PVehicle::OnAttributeChange(const Attrib::Collection *collection, unsigned int attribkey) {}

void PVehicle::OnEnableModeling() {}

void PVehicle::DoDebug(float dT) {}

void PVehicle::OnDebugDraw() {}

void PVehicle::CommitBehaviorOverrides() {
    if (mOverrideDirty) {
        mOverrideDirty = false;
        ReloadBehaviors();
    }
}

void PVehicle::Reset() {
    mBehaviors.Reset();
    mTimeInAir = 0.0f;
    mWheelsOnGround = 0;
    mBrakeTime = 0.0f;
}

void PVehicle::SetDriverClass(DriverClass dc) {
    if (mDriverClass != dc) {
        mDriverClass = dc;
        UpdateListing();
        ReloadBehaviors();
    }
}

void PVehicle::Activate() {
    if (mPhysicsMode == PHYSICS_MODE_INACTIVE) {
        SetPhysicsMode(PHYSICS_MODE_SIMULATED);
    }
}

void PVehicle::Deactivate() { SetPhysicsMode(PHYSICS_MODE_INACTIVE); }

void PVehicle::Kill() {
    PhysicsObject::Kill();
    {
        UCrc32 mechanic(BEHAVIOR_MECHANIC_DRAW);
        ReleaseBehavior(mechanic);
    }
    {
        UCrc32 mechanic(BEHAVIOR_MECHANIC_AUDIO);
        ReleaseBehavior(mechanic);
    }
    mResources.Invalidate();
}

void PVehicle::ForceStopOn(char forceStopBits) {
    mForceStop |= forceStopBits;
    if (mInput != nullptr) {
        mInput->ClearInput();
    }
}

void PVehicle::ForceStopOff(char forceStopBits) {
    mForceStop &= ~forceStopBits;
    if (mInput != nullptr) {
        mInput->ClearInput();
    }
}

void PVehicle::OnDisableModeling() {
    IEffects *ieff;
    if (static_cast< ISimable * >(this)->QueryInterface(&ieff)) {
        ieff->Purge();
    }
}

bool PVehicle::GetPerformance(Physics::Info::Performance &performance) const {
    performance = mPerformance;
    return mPerformanceValid;
}

unsigned int PVehicle::GetVehicleKey() const { return mAttributes.GetCollection(); }

CarType PVehicle::GetModelType() const { return mResources.Type; }

bool PVehicle::IsSpooled() const { return mResources.IsSpooled(); }

const UMath::Vector3 &PVehicle::GetPosition() const {
    if (mRigidBody == nullptr) {
        return UMath::Vector3::kZero;
    }
    return mRigidBody->GetPosition();
}

IModel *PVehicle::GetModel() {
    if (mRenderable != nullptr) {
        return mRenderable->GetModel();
    } else {
        return nullptr;
    }
}

const IModel *PVehicle::GetModel() const {
    if (mRenderable == nullptr) {
        return nullptr;
    }
    return mRenderable->GetModel();
}

bool PVehicle::InShock() const {
    if (mDamage == nullptr) {
        return false;
    }
    return mDamage->InShock() > 0.0f;
}

bool PVehicle::IsDestroyed() const {
    if (mDamage != nullptr) {
        return mDamage->IsDestroyed();
    } else {
        return false;
    }
}

void PVehicle::SetPhysicsMode(PhysicsMode mode) {
    if (mode != mPhysicsMode) {
        OnEndMode(mPhysicsMode);
        mPhysicsMode = mode;
        OnBeginMode(mode);
    }
    if (mArticulation != nullptr) {
        IVehicle *itrailer = mArticulation->GetTrailer();
        if (itrailer != nullptr) {
            itrailer->SetPhysicsMode(mode);
        }
    }
}

void PVehicle::SetTunings(const Physics::Tunings &tunings) {
    if (mCustomization == nullptr) {
        return;
    }
    for (unsigned int i = 0; i < Physics::Tunings::MAX_TUNINGS; i++) {
        mCustomization->SetTuning(static_cast<Physics::Tunings::Path>(i), tunings.Value[i]);
    }
}

bool PVehicle::OnTask(HSIMTASK htask, float dT) {
    if (mTaskFX == htask) {
        OnTaskFX(dT);
        return true;
    }
    return PhysicsObject::OnTask(htask, dT);
}

void PVehicle::SetStaging(bool staging) {
    if (static_cast< unsigned int >(staging) != static_cast< unsigned int >(mStaging)) {
        mStaging = staging;
        if (staging) {
            SetSpeed(0.0f);
        }
    }
}

void PVehicle::SetDriverStyle(DriverStyle style) {
    if (mDriverStyle != style) {
        mDriverStyle = style;
        ReloadBehaviors();
        if (mDriverStyle == STYLE_RACING && mEngine != nullptr) {
            mEngine->MatchSpeed(mLocalVel.z);
        }
    }
}

void PVehicle::Launch() {
    if (mEngine == nullptr) {
        return;
    }
    if (mPerfectLaunch.Time <= 0.0f) {
        return;
    }
    if (mDriverClass == DRIVER_HUMAN) {
        if (mPerfectLaunch.Amount > 0.0f) {
            mPerfectLaunch.Set(10.0f);
            new EPerfectLaunch(ISimable::GetInstanceHandle(), mPerfectLaunch.Amount);
        }
    } else {
        mPerfectLaunch.Clear();
    }
}

float PVehicle::GetPerfectLaunch() const {
    if (!IsStaging() && mPerfectLaunch.IsSet()) {
        return mPerfectLaunch.Amount;
    }
    return 0.5f;
}

bool PVehicle::IsLoading() const {
    if (mRenderable != nullptr && !mRenderable->IsRenderable()) {
        return true;
    }
    if (mAudible != nullptr && !mAudible->IsAudible()) {
        return true;
    }
    if (mArticulation != nullptr) {
        IVehicle *trailer = mArticulation->GetTrailer();
        if (trailer != nullptr) {
            return trailer->IsLoading();
        }
    }
    return false;
}

void PVehicle::ReloadBehaviors() {
    UMath::Vector3 pos;
    UMath::Matrix4 mat;
    pos = static_cast<ISimable *>(this)->GetRigidBody()->GetPosition();
    static_cast<ISimable *>(this)->GetRigidBody()->GetMatrix4(mat);
    LoadBehaviors(pos, mat);
}

void PVehicle::SetAnimating(bool animate) {
    if (static_cast< unsigned int >(animate) != static_cast< unsigned int >(mAnimating)) {
        mBehaviorOverrides.clear();
        mAnimating = animate;
        ReloadBehaviors();
        UCrc32 mechanic(BEHAVIOR_MECHANIC_AI);
        PauseBehavior(mechanic, animate);
        if (mCollisionBody != nullptr) {
            mCollisionBody->SetAnimating(mAnimating);
        }
    }
}

void PVehicle::SetBehaviorOverride(UCrc32 mechanic, UCrc32 behavior) {
    Behavior *beh = FindBehavior(mechanic);
    if (beh == nullptr || beh->GetSignature() != behavior) {
        mBehaviorOverrides[mechanic] = behavior;
        mOverrideDirty = true;
    }
}

void PVehicle::RemoveBehaviorOverride(UCrc32 mechanic) {
    UTL::Std::map<UCrc32, UCrc32, _type_ID_PVehicleChangeReq>::iterator it = mBehaviorOverrides.find(mechanic);
    if (it != mBehaviorOverrides.end()) {
        mBehaviorOverrides.erase(it);
        mOverrideDirty = true;
    }
}

void PVehicle::OnBehaviorChange(const UCrc32 &mechanic) {
    PhysicsObject::OnBehaviorChange(mechanic);
    unsigned int crc = mechanic.GetValue();
    if (crc == UCrc32(BEHAVIOR_MECHANIC_AI).GetValue()) {
        static_cast<ISimable *>(this)->QueryInterface(&mAI);
    } else if (crc == UCrc32(BEHAVIOR_MECHANIC_INPUT).GetValue()) {
        static_cast<ISimable *>(this)->QueryInterface(&mInput);
    } else if (crc == UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY).GetValue()) {
        if (static_cast<ISimable *>(this)->QueryInterface(&mCollisionBody)) {
            mCollisionBody->SetAnimating(mAnimating);
        }
        static_cast<ISimable *>(this)->QueryInterface(&mArticulation);
    } else if (crc == UCrc32(BEHAVIOR_MECHANIC_DRAW).GetValue()) {
        static_cast<ISimable *>(this)->QueryInterface(&mRenderable);
    } else if (crc == UCrc32(BEHAVIOR_MECHANIC_AUDIO).GetValue()) {
        static_cast<ISimable *>(this)->QueryInterface(&mAudible);
    } else if (crc == UCrc32(BEHAVIOR_MECHANIC_SUSPENSION).GetValue()) {
        if (!static_cast<ISimable *>(this)->QueryInterface(&mSuspension)) {
            return;
        }
        if (mCollisionBody == nullptr) {
            return;
        }
        const UMath::Vector3 &fwd = mCollisionBody->GetForwardVector();
        const UMath::Vector3 &vel = static_cast<ISimable *>(this)->GetRigidBody()->GetLinearVelocity();
        float speed = UMath::Dot(fwd, vel);
        mSuspension->MatchSpeed(speed);
    } else if (crc == UCrc32(BEHAVIOR_MECHANIC_ENGINE).GetValue()) {
        static_cast<ISimable *>(this)->QueryInterface(&mTranny);
        if (!static_cast<ISimable *>(this)->QueryInterface(&mEngine)) {
            return;
        }
        mEngine->ChargeNOS(mStartingNOS - mEngine->GetNOSCapacity());
        if (mCollisionBody == nullptr) {
            return;
        }
        const UMath::Vector3 &fwd = mCollisionBody->GetForwardVector();
        const UMath::Vector3 &vel = static_cast<ISimable *>(this)->GetRigidBody()->GetLinearVelocity();
        float speed = UMath::Dot(fwd, vel);
        mEngine->MatchSpeed(speed);
    } else if (crc == UCrc32(BEHAVIOR_MECHANIC_DAMAGE).GetValue()) {
        static_cast<ISimable *>(this)->QueryInterface(&mDamage);
    }
}

void PVehicle::SetSpeed(float speed) {
    mSpeed = speed;
    mPerfectLaunch.Clear();
    if (mCollisionBody != nullptr) {
        UMath::Vector3 vel;
        const UMath::Vector3 &fwd = mCollisionBody->GetForwardVector();
        UMath::Scale(fwd, speed, vel);
        static_cast<ISimable *>(this)->GetRigidBody()->SetLinearVelocity(vel);
        if (mSuspension != nullptr) {
            mSuspension->MatchSpeed(speed);
        }
        if (mEngine != nullptr) {
            mEngine->MatchSpeed(speed);
        }
        UpdateLocalVelocities();
        if (mArticulation != nullptr && mArticulation->GetTrailer() != nullptr) {
            mArticulation->GetTrailer()->SetSpeed(speed);
        }
    }
}


void PVehicle::UpdateLocalVelocities() {
    IRigidBody *rigidbody = static_cast<ISimable *>(this)->GetRigidBody();
    if (rigidbody == nullptr || mCollisionBody == nullptr) {
        UMath::Clear(mLocalVel);
        mAbsSpeed = 0.0f;
        mSlipAngle = 0.0f;
        mSpeed = 0.0f;
    } else {
        mLocalVel = rigidbody->GetLinearVelocity();
        rigidbody->ConvertWorldToLocal(mLocalVel, false);
        mSlipAngle = UMath::Atan2a(mLocalVel.x, UMath::Abs(mLocalVel.z));
        mSpeed = rigidbody->GetSpeed();
        if (UMath::Dot(mCollisionBody->GetForwardVector(), rigidbody->GetLinearVelocity()) < 0.0f) {
            mSpeed = -mSpeed;
        }
        mAbsSpeed = UMath::Abs(mSpeed);
    }
}

void PVehicle::DoStaging(float dT) {
    if (!mPerfectLaunch.IsSet()) {
        mPerfectLaunch.Amount = 0.0f;
        if (mEngine != nullptr) {
            IRaceEngine *raceEngine;
            if (mEngine->QueryInterface(&raceEngine)) {
                float range = 0.0f;
                float peak_rpm = raceEngine->GetPerfectLaunchRange(range);
                if (range > 0.0f && peak_rpm > 0.0f) {
                    float rpm = mEngine->GetRPM();
                    float dist = rpm - peak_rpm;
                    if (dist < range && dist > 0.0f) {
                        mPerfectLaunch.Amount = (dist / range) * 0.5f + 0.5f;
                    }
                }
            }
        }
    }
}

void PVehicle::ComputeHeading(UMath::Vector3 *v) {
    UMath::Vector3 forward;
    UMath::Vector3 velocity;
    IRigidBody *rigid_body = static_cast<ISimable *>(this)->GetRigidBody();
    float speed = rigid_body->GetSpeed();
    rigid_body->GetForwardVector(forward);
    velocity = rigid_body->GetLinearVelocity();
    if (speed > 0.01f) {
        UMath::Unit(velocity, velocity);
    }
    float velocity_blend = UMath::Clamp(speed, 0.0f, 1.0f);
    float forward_blend = 1.0f - velocity_blend;
    UMath::Scale(forward, forward_blend, forward);
    UMath::ScaleAdd(velocity, velocity_blend, forward, forward);
    UMath::Unit(forward, *v);
}
