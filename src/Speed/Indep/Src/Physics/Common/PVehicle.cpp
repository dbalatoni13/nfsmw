#include "Speed/Indep/Src/Physics/PVehicle.h"

#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/FE/FECustomizationRecord.h"
#include "Speed/Indep/Src/Generated/Events/EPerfectLaunch.hpp"
#include "Speed/Indep/Src/Generated/Events/EPlayerAirborne.hpp"
#include "Speed/Indep/Src/Generated/Messages/MJumpCut.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAudible.h"
#include "Speed/Indep/Src/Interfaces/Simables/IArticulatedVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEffects.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/IExplosion.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRenderable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Bounds.h"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Sim/SimSurface.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/Sim/Util.h"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WWorldPos.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

class IOnlinePlayer : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() { return (HINTERFACE)_IHandle; }
    IOnlinePlayer(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}
    virtual ~IOnlinePlayer() {}
    virtual void SendSetVehicleOnGround();
};

class CarPartDatabase { public: CarType GetCarType(unsigned int hash); };
extern CarPartDatabase CarPartDB;
bool CarInfo_IsSkinned(CarType type);
unsigned int CarInfo_GetResourceCost(CarType type, bool is_player, bool split_screen);
bool IsSplitScreen();
PresetCar *FindFEPresetCar(unsigned int hash);
int GetMikeMannBuild();
bool CanSpawnRigidBody(const UMath::Vector3 &position, bool highPriority);

namespace Physics { namespace Upgrades {
void RemoveJunkman(Attrib::Gen::pvehicle &vehicle, Type type);
void RemovePart(Attrib::Gen::pvehicle &vehicle, Type type);
int GetLevel(const Attrib::Gen::pvehicle &vehicle, Type type);
int GetMaxLevel(const Attrib::Gen::pvehicle &vehicle, Type type);
bool SetLevel(Attrib::Gen::pvehicle &vehicle, Type type, int level);
bool MatchPerformance(Attrib::Gen::pvehicle &vehicle, const Physics::Info::Performance &matched_performance);
}; };

extern Attrib::StringKey BEHAVIOR_MECHANIC_AI;
extern Attrib::StringKey BEHAVIOR_MECHANIC_AUDIO;
extern Attrib::StringKey BEHAVIOR_MECHANIC_DAMAGE;
extern Attrib::StringKey BEHAVIOR_MECHANIC_DRAW;
extern Attrib::StringKey BEHAVIOR_MECHANIC_EFFECTS;
extern Attrib::StringKey BEHAVIOR_MECHANIC_ENGINE;
extern Attrib::StringKey BEHAVIOR_MECHANIC_INPUT;
extern Attrib::StringKey BEHAVIOR_MECHANIC_RESET;
extern Attrib::StringKey BEHAVIOR_MECHANIC_RIGIDBODY;
extern Attrib::StringKey BEHAVIOR_MECHANIC_SUSPENSION;

extern bool Tweak_UseTweakerTunings;
extern float Tweak_TuningAero;

namespace CameraAI {
void AddAvoidable(IBody *body);
void RemoveAvoidable(IBody *body);
} // namespace CameraAI

bool CanInstancesShareResourceCost(CarType type);

struct AIBehaviors {
    DriverClass dclass;
    UCrc32 vclass;
    UCrc32 signature;
};
extern AIBehaviors ai_behaviors[];

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
    if (mPerfectLaunch.Time > 0.0f) {
        return;
    }
    if (mDriverClass == DRIVER_HUMAN) {
        if (mPerfectLaunch.Amount > 0.0f) {
            mPerfectLaunch.Time = 10.0f;
            new EPerfectLaunch(ISimable::GetInstanceHandle(), mPerfectLaunch.Amount);
        }
    } else {
        mPerfectLaunch.Clear();
    }
}

float PVehicle::GetPerfectLaunch() const {
    if (!IsStaging() && 0.5f < mPerfectLaunch.Time) {
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
        mSlipAngle = 0.0f;
        mSpeed = 0.0f;
        mAbsSpeed = 0.0f;
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

void PVehicle::CheckOffWorld() {
    UCrc32 susp(BEHAVIOR_MECHANIC_SUSPENSION);
    if (IsBehaviorActive(susp)) {
        if (static_cast<ISimable *>(this)->GetWPos().GetSurface() !=
            SimSurface::kNull.GetConstCollection()) {
            goto set_false;
        }
        if (mSuspension == nullptr) {
            goto set_true;
        }
        {
            unsigned int invalid_tires = 0;
            for (unsigned int i = 0; i < mSuspension->GetNumWheels(); i++) {
                if (mSuspension->GetWheelRoadSurface(i).GetConstCollection() ==
                    SimSurface::kNull.GetConstCollection()) {
                    invalid_tires++;
                }
            }
            mOffWorld = !(invalid_tires < 2);
        }
        goto done;
    set_true:
        mOffWorld = true;
        goto done;
    set_false:
        mOffWorld = false;
    } else {
        float worldHeight = 0.0f;
        WCollisionMgr mgr(0, 3);
        const UMath::Vector3 &pos = static_cast<ISimable *>(this)->GetPosition();
        mOffWorld = !mgr.GetWorldHeightAtPointRigorous(pos, worldHeight, nullptr);
    }
done:;
}

void PVehicle::OnTaskSimulate(float dT) {
    UCrc32 mechDraw(BEHAVIOR_MECHANIC_DRAW);
    bool visible = false;
    if (IsBehaviorActive(mechDraw) && mRenderable != nullptr && mRenderable->IsRenderable()) {
        visible = true;
    }
    if (visible) {
        if (!mRenderable->InView()) {
            mOffScreenTime = mOffScreenTime + dT;
            mOnScreenTime = 0.0f;
        } else {
            mOffScreenTime = 0.0f;
            mOnScreenTime = mOnScreenTime + dT;
        }
    } else {
        mOffScreenTime = 0.0f;
        mOnScreenTime = 0.0f;
    }
    CommitBehaviorOverrides();
    DoDebug(dT);
    if (mCollisionBody != nullptr) {
        if (mCollisionBody->IsModeling() != mIsModeling) {
            mIsModeling = mCollisionBody->IsModeling();
            if (mIsModeling) {
                OnEnableModeling();
            } else {
                OnDisableModeling();
            }
        } else {
            mIsModeling = mCollisionBody->IsModeling();
        }
    }
    if (mPhysicsMode != PHYSICS_MODE_INACTIVE) {
        UpdateLocalVelocities();
        CheckOffWorld();
    }
    if (mPhysicsMode == PHYSICS_MODE_SIMULATED) {
        UCrc32 mechSusp(BEHAVIOR_MECHANIC_SUSPENSION);
        bool sleeping = false;
        if (mCollisionBody->IsSleeping() && IsStaging()) {
            sleeping = true;
        }
        PauseBehavior(mechSusp, sleeping);
        if (mTranny != nullptr) {
            if (!mTranny->IsGearChanging()) {
                mSpeedometer = mTranny->GetSpeedometer();
            }
        } else {
            mSpeedometer = mAbsSpeed;
        }
        unsigned int num_onground;
        if (mSuspension != nullptr) {
            num_onground = mSuspension->GetNumWheelsOnGround();
            if (num_onground == 0 && mWheelsOnGround != 0 && mDriverClass == DRIVER_HUMAN) {
                new EPlayerAirborne(ISimable::GetInstanceHandle());
            }
            mWheelsOnGround = num_onground;
            if (mSuspension == nullptr || num_onground == 0) {
                mTimeInAir = mTimeInAir + dT;
            } else {
                mTimeInAir = 0.0f;
            }
        } else {
            mTimeInAir = mTimeInAir + dT;
        }
        if (IsStaging()) {
            DoStaging(dT);
        } else {
            if (mPerfectLaunch.IsSet()) {
                if (mTranny != nullptr) {
                    if (!mTranny->IsGearChanging()) {
                        mPerfectLaunch.Tick(dT);
                    } else {
                        if (mDriverClass == DRIVER_HUMAN) {
                            mPerfectLaunch.Clear();
                        }
                    }
                    if (GetSpeed() > MPH2MPS(60.0f)) {
                        mPerfectLaunch.Clear();
                    }
                }
            }
        }
    } else if (mPhysicsMode == PHYSICS_MODE_EMULATED) {
        mTimeInAir = 0.0f;
        if (mSuspension != nullptr) {
            mWheelsOnGround = mSuspension->GetNumWheels();
        } else {
            mWheelsOnGround = 0;
        }
        mSpeedometer = mAbsSpeed;
    } else {
        mWheelsOnGround = 0;
        mTimeInAir = 0.0f;
        mSpeedometer = 0.0f;
    }
}

bool CanInstancesShareResourceCost(CarType type) {
    CarUsageType usage_type = GetCarTypeInfo(type)->GetCarUsageType();
    if (usage_type == CAR_USAGE_TYPE_COP) {
        return true;
    }
    return usage_type == CAR_USAGE_TYPE_TRAFFIC;
}

void PVehicle::CleanResources() {
    for (PVehicle *dirty = mInstances.GetHead(); dirty != mInstances.EndOfList();) {
        PVehicle *next = dirty->GetNext();
        if (dirty->IsDirty()) {
            static_cast<ISimable *>(dirty)->Kill();
        }
        dirty = next;
    }
}

const Physics::Tunings *PVehicle::GetTunings() const {
    if (IsLoading()) {
        return nullptr;
    }
    if (Tweak_UseTweakerTunings) {
        static Physics::Tunings tunings;
        static bool __tmp = false;
        if (!__tmp) {
            tunings.Default();
            __tmp = true;
        }
        tunings.Value[Physics::Tunings::STEERING] = 0.0f;
        tunings.Value[Physics::Tunings::HANDLING] = 0.0f;
        tunings.Value[Physics::Tunings::BRAKES] = 0.0f;
        tunings.Value[Physics::Tunings::RIDEHEIGHT] = 0.0f;
        tunings.Value[Physics::Tunings::AERODYNAMICS] = Tweak_TuningAero;
        tunings.Value[Physics::Tunings::NOS] = 0.0f;
        tunings.Value[Physics::Tunings::INDUCTION] = 0.0f;
        return &tunings;
    }
    if (mCustomization != nullptr) {
        return mCustomization->GetTunings();
    }
    return nullptr;
}

unsigned int PVehicle::CountResources() {
    unsigned int count = 0;
    UTL::Std::list<Resource, _type_list> resource_list;
    for (PVehicle *pv = mInstances.GetHead(); pv != mInstances.EndOfList(); pv = pv->GetNext()) {
        bool found = false;
        for (UTL::Std::list<Resource, _type_list>::const_iterator iter = resource_list.begin();
             iter != resource_list.end(); ++iter) {
            if ((*iter).Type == pv->mResources.Type) {
                found = true;
                break;
            }
        }
        if (found && CanInstancesShareResourceCost(pv->mResources.Type)) {
        } else {
            resource_list.push_back(pv->mResources);
        }
        count += pv->mResources.Cost;
    }
    return count;
}

void PVehicle::OnTaskFX(float dT) {
    static_cast<ISimable *>(this)->GetRigidBody();
    if (INIS::Get() == nullptr) {
        GlareOn(static_cast<VehicleFX::ID>(7));
    }
    bool do_brake = false;
    if (mInput != nullptr) {
        do_brake = mInput->GetControls().fBrake > 0.0f;
    }
    if (!static_cast<ISimable *>(this)->IsPlayer()) {
        if (do_brake) {
            mBrakeTime = mBrakeTime + dT;
        } else {
            mBrakeTime = 0.0f;
        }
        do_brake = mBrakeTime > 0.5f;
    }
    if (do_brake) {
        GlareOn(static_cast<VehicleFX::ID>(0x38));
    } else {
        GlareOff(static_cast<VehicleFX::ID>(0x38));
    }
    if (mTranny != nullptr && mTranny->IsReversing()) {
        GlareOn(static_cast<VehicleFX::ID>(0xc0));
    } else {
        GlareOff(static_cast<VehicleFX::ID>(0xc0));
    }
}

void PVehicle::OnBeginMode(PhysicsMode mode) {
    if (mode == PHYSICS_MODE_INACTIVE) {
        IVehicle::AddToList(VEHICLE_INACTIVE);
        if (mCollisionBody != nullptr) {
            mCollisionBody->DisableTriggering();
        }
        static_cast<ISimable *>(this)->DetachEntity();
    } else if (mode == PHYSICS_MODE_SIMULATED) {
        if (mCollisionBody != nullptr) {
            mCollisionBody->EnableModeling();
        }
        CameraAI::AddAvoidable(static_cast<IBody *>(this));
        {
            UCrc32 mechanic(BEHAVIOR_MECHANIC_DRAW);
            PauseBehavior(mechanic, false);
        }
        {
            UCrc32 mechanic(BEHAVIOR_MECHANIC_SUSPENSION);
            PauseBehavior(mechanic, false);
        }
        {
            UCrc32 mechanic(BEHAVIOR_MECHANIC_ENGINE);
            PauseBehavior(mechanic, false);
        }
        {
            UCrc32 mechanic(BEHAVIOR_MECHANIC_RIGIDBODY);
            PauseBehavior(mechanic, false);
        }
        {
            UCrc32 mechanic(BEHAVIOR_MECHANIC_EFFECTS);
            PauseBehavior(mechanic, false);
        }
        {
            UCrc32 mechanic(BEHAVIOR_MECHANIC_RESET);
            PauseBehavior(mechanic, false);
        }
        {
            UCrc32 mechanic(BEHAVIOR_MECHANIC_AUDIO);
            PauseBehavior(mechanic, false);
        }
        {
            UCrc32 mechanic(BEHAVIOR_MECHANIC_DAMAGE);
            PauseBehavior(mechanic, false);
        }
    }
}

void PVehicle::OnEndMode(PhysicsMode mode) {
    if (mode == PHYSICS_MODE_INACTIVE) {
        if (mCollisionBody != nullptr) {
            mCollisionBody->EnableTriggering();
        }
        static_cast<ISimable *>(this)->DetachEntity();
        IVehicle::UnList(VEHICLE_INACTIVE);
    } else if (mode == PHYSICS_MODE_EMULATED) {
    } else if (mode == PHYSICS_MODE_SIMULATED) {
        if (mCollisionBody != nullptr) {
            mCollisionBody->DisableModeling();
        }
        CameraAI::RemoveAvoidable(static_cast<IBody *>(this));
        {
            UCrc32 mechanic(BEHAVIOR_MECHANIC_DRAW);
            PauseBehavior(mechanic, true);
        }
        {
            UCrc32 mechanic(BEHAVIOR_MECHANIC_SUSPENSION);
            PauseBehavior(mechanic, true);
        }
        {
            UCrc32 mechanic(BEHAVIOR_MECHANIC_ENGINE);
            PauseBehavior(mechanic, true);
        }
        {
            UCrc32 mechanic(BEHAVIOR_MECHANIC_RIGIDBODY);
            PauseBehavior(mechanic, true);
        }
        {
            UCrc32 mechanic(BEHAVIOR_MECHANIC_EFFECTS);
            PauseBehavior(mechanic, true);
        }
        {
            UCrc32 mechanic(BEHAVIOR_MECHANIC_RESET);
            PauseBehavior(mechanic, true);
        }
        {
            UCrc32 mechanic(BEHAVIOR_MECHANIC_AUDIO);
            PauseBehavior(mechanic, true);
        }
        {
            UCrc32 mechanic(BEHAVIOR_MECHANIC_DAMAGE);
            PauseBehavior(mechanic, true);
        }
    }
}


bool PVehicle::SetDynamicData(const EventSequencer::System *system, EventDynamicData *data) {
    if (IsDirty()) {
        return false;
    }
    data->fhSimable = reinterpret_cast<uintptr_t>(ISimable::GetInstanceHandle());
    data->fWorldID = static_cast<ISimable *>(this)->GetWorldID();
    IRigidBody *body = static_cast<ISimable *>(this)->GetRigidBody();
    if (body != nullptr) {
        const UMath::Vector3 &pos = body->GetPosition();
        data->fPosition = UMath::Vector4Make(pos, 0.0f);
        UMath::Vector3 dir;
        body->GetForwardVector(dir);
        data->fVector = UMath::Vector4Make(dir, 0.0f);
        const UMath::Vector3 &vel = body->GetLinearVelocity();
        data->fVelocity = UMath::Vector4Make(vel, 0.0f);
        const UMath::Vector3 &angvel = body->GetAngularVelocity();
        data->fAngularVelocity = UMath::Vector4Make(angvel, 0.0f);
    }
    if (mRenderable != nullptr) {
        data->fhModel = reinterpret_cast<uintptr_t>(mRenderable->GetModelHandle());
    }
    return true;
}

bool PVehicle::OnExplosion(const UMath::Vector3 &normal, const UMath::Vector3 &position, float dT, IExplosion *explosion) {
    unsigned int targets = explosion->GetTargets();
    if ((targets & 2) == 0) {
        return false;
    }
    if (static_cast<ISimable *>(this)->IsPlayer()) {
        if ((targets & 4) == 0) {
            return false;
        }
    }
    if (static_cast<ISimable *>(this)->IsOwnedByPlayer()) {
        return false;
    }
    HCAUSE causality = explosion->GetCausality();
    if (causality != nullptr) {
        ICause *cause = ICause::FindInstance(causality);
        if (cause != nullptr) {
            cause->OnCausedExplosion(explosion, static_cast<ISimable *>(this));
        }
    }
    IRigidBody *irb = static_cast<ISimable *>(this)->GetRigidBody();
    float factor = explosion->GetExpansionSpeed() * (1.0f / irb->GetMass());
    UMath::Vector3 point_velocity;
    irb->GetPointVelocity(position, point_velocity);
    float speed = UMath::Dot(point_velocity, normal);
    if (speed < factor) {
        float deltaspeed = factor - speed;
        UMath::Vector3 impactvel;
        UMath::Scale(normal, deltaspeed, impactvel);
        UMath::Vector3 force;
        UMath::Scale(impactvel, irb->GetMass() / dT, force);
        irb->ResolveForce(force, position);
    }
    if (mSequencer != nullptr) {
        mSequencer->ProcessStimulus(0xab556d39, Sim::GetTime(), nullptr, EventSequencer::QUEUE_ALLOW);
        if (explosion->HasDamage()) {
            mSequencer->ProcessStimulus(0xffcd8a63, Sim::GetTime(), nullptr, EventSequencer::QUEUE_ALLOW);
        }
    }
    return true;
}

void PVehicle::UpdateListing() {
    for (unsigned int i = 1; i < VEHICLE_MAX; i++) {
        IVehicle::UnList(static_cast<eVehicleList>(i));
    }
    switch (mDriverClass) {
    case DRIVER_HUMAN:
        IVehicle::AddToList(VEHICLE_PLAYERS);
        IVehicle::AddToList(VEHICLE_RACERS);
        break;
    case DRIVER_TRAFFIC:
        IVehicle::AddToList(VEHICLE_AITRAFFIC);
        IVehicle::AddToList(VEHICLE_AI);
        break;
    case DRIVER_COP:
        IVehicle::AddToList(VEHICLE_AICOPS);
        IVehicle::AddToList(VEHICLE_AI);
        break;
    case DRIVER_RACER:
        IVehicle::AddToList(VEHICLE_AIRACERS);
        IVehicle::AddToList(VEHICLE_AI);
        IVehicle::AddToList(VEHICLE_RACERS);
        break;
    case DRIVER_REMOTE:
        IVehicle::AddToList(VEHICLE_REMOTE);
        IVehicle::AddToList(VEHICLE_RACERS);
        break;
    default:
        break;
    }
}

PVehicle::Resource::Resource(const Attrib::Gen::pvehicle &attribs, bool spool, bool is_player) {
    Flags = 0;
    CarPartDatabase &db = CarPartDB;
    const char *text = attribs.MODEL().GetString();
    if (text == nullptr) {
        text = "";
    }
    CarType type = db.GetCarType(bStringHash(text));
    Type = type;
    if (type != CARTYPE_NONE && type < NUM_CARTYPES) {
        if (CarInfo_IsSkinned(type)) {
            Flags |= 4;
        }
        bool split_screen = Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN;
        Cost = CarInfo_GetResourceCost(Type, is_player, split_screen);
        if (spool) {
            Flags |= 2;
        }
        if (Cost != 0) {
            Flags |= 1;
        }
    }
}

PVehicle::PVehicle(DriverClass dc, const Attrib::Gen::pvehicle &attribs, const UMath::Vector3 &initialVec,
                   const UMath::Vector3 &initialPos, const CollisionGeometry::Bounds *bounds,
                   const FECustomizationRecord *customization, const Resource &resource,
                   const Physics::Info::Performance *performance, const char *cache_name)
    : PhysicsObject(attribs.GetBase(), SIMABLE_VEHICLE, 0, 0x18) //
    , bTNode<PVehicle>() //
    , IVehicle(this) //
    , Debugable() //
    , EventSequencer::IContext(this) //
    , IExplodeable(this) //
    , IAttributeable() //
    , mAttributes(attribs) //
    , mClass() //
    , mPerfectLaunch() //
    , mPerformance() //
{
    mCustomization = nullptr;
    mInput = nullptr;
    mCollisionBody = nullptr;
    mSuspension = nullptr;
    mEngine = nullptr;
    mDamage = nullptr;
    mTranny = nullptr;
    mAI = nullptr;
    mArticulation = nullptr;
    mRenderable = nullptr;
    mAudible = nullptr;
    mSequencer = nullptr;
    mTaskFX = nullptr;
    mSpeed = 0.0f;
    mAbsSpeed = 0.0f;
    mSpeedometer = 0.0f;
    mTimeInAir = 0.0f;
    mSlipAngle = 0.0f;
    mWheelsOnGround = 0;
    mLocalVel = UMath::Vector3::kZero;
    mStartingNOS = 0.0f;
    mDriverClass = dc;
    mDriverStyle = STYLE_RACING;
    mGlareState = 0;
    mBrakeTime = 0.0f;
    mForceStop = 0;
    mPhysicsMode = PHYSICS_MODE_SIMULATED;
    mAnimating = false;
    mStaging = false;
    mBounds = bounds;
    mOnScreenTime = 0.0f;
    mOverrideDirty = false;
    mIsModeling = true;
    mOffScreenTime = 0.0f;
    mOffWorld = false;
    mHasDyno = false;
    mResources = resource;
    mPerformance.Default();
    mPerformanceValid = false;
    mCacheName = cache_name;
    if (performance != nullptr) {
        mPerformance.TopSpeed = performance->TopSpeed;
        mPerformance.Acceleration = performance->Acceleration;
        mPerformance.Handling = performance->Handling;
        mPerformanceValid = true;
    }
    mInstances.AddTail(this);
    AITarget::Register(static_cast<ISimable *>(this));
    if (customization != nullptr) {
        FECustomizationRecord *pFVar = static_cast<FECustomizationRecord *>(operator new(0x198));
        bMemCpy(pFVar, customization, 0x198);
        mCustomization = pFVar;
    }
    UCrc32 classKey(mAttributes.CLASS());
    mClass = classKey;
    IVehicle::AddToList(VEHICLE_ALL);
    UpdateListing();
    float rate;
    if (mDriverClass == DRIVER_HUMAN) {
        rate = 0.0f;
    } else if (mDriverClass == DRIVER_TRAFFIC) {
        rate = 0.05f;
    } else {
        rate = 0.02f;
    }
    UCrc32 fxName("FX");
    mTaskFX = AddTask(fxName, rate, 0.0f, Sim::TASK_FRAME_FIXED);
    Debugable::MakeDebugable(DBG_PHYSICS_RACERS);
    PhysicsObject::Reset();
    if (mDamage != nullptr) {
        mDamage->ResetDamage();
    }
    mGlareState = 0;
    UMath::Matrix4 initMat = Util_GenerateMatrix(initialVec, nullptr);
    LoadBehaviors(initialPos, initMat);
    const Attrib::StringKey &seq = mAttributes.EventSequencer();
    if (seq.IsNotEmpty()) {
        mSequencer = EventSequencer::Create(this, static_cast<EventSequencer::IContext *>(this),
                                            UCrc32(seq.GetString()), Sim::GetTime(), 0.0f);
    }
}

PVehicle::~PVehicle() {
    PhysicsObject::DetachAll();
    AITarget::UnRegister(static_cast<ISimable *>(this));
    if (mCustomization != nullptr) {
        gFastMem.Free(mCustomization, sizeof(FECustomizationRecord), nullptr);
        mCustomization = nullptr;
    }
    ReleaseBehaviors();
    if (mTaskFX != nullptr) {
        RemoveTask(mTaskFX);
        mTaskFX = nullptr;
    }
    if (mSequencer != nullptr) {
        mSequencer->Release();
        mSequencer = nullptr;
    }
    mInstances.Remove(this);
    CameraAI::RemoveAvoidable(static_cast<IBody *>(this));
    mBehaviorOverrides.clear();
    IVehicle::UnList();
}

UCrc32 PVehicle::LookupBehaviorSignature(const Attrib::StringKey &mechanic) const {
    if (mechanic == BEHAVIOR_MECHANIC_AUDIO && !IsSoundEnabled) {
        return UCrc32::kNull;
    }
    UTL::Std::map<UCrc32, UCrc32, _type_ID_PVehicleChangeReq>::const_iterator it =
        mBehaviorOverrides.find(UCrc32(mechanic));
    if (it != mBehaviorOverrides.end()) {
        return (*it).second;
    }
    if (mAnimating) {
        if (mClass != VehicleClass::CHOPPER) {
            if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
                return UCrc32("SuspensionSpline");
            }
            if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
                return UCrc32("EngineSpline");
            }
        }
        if (mechanic == BEHAVIOR_MECHANIC_RESET) {
            return UCrc32::kNull;
        }
        if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
            return UCrc32("InputNIS");
        }
    }
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY && mDriverClass == DRIVER_REMOTE) {
        return UCrc32("RBRemote");
    }
    if (mechanic == BEHAVIOR_MECHANIC_INPUT && mDriverClass == DRIVER_HUMAN) {
        if (mDriverStyle == STYLE_DRAG) {
            return UCrc32("InputPlayerDrag");
        }
        return UCrc32("InputPlayer");
    }
    if (mechanic == BEHAVIOR_MECHANIC_DAMAGE && mDriverStyle == STYLE_DRAG) {
        return UCrc32("DamageDragster");
    }
    if (mechanic == BEHAVIOR_MECHANIC_ENGINE && mClass == VehicleClass::CAR &&
        mDriverStyle == STYLE_DRAG) {
        return UCrc32("EngineDragster");
    }
    if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION && mClass == VehicleClass::CAR) {
        int dc = static_cast<int>(mDriverClass);
        if (dc >= 3 && (dc < 5 || dc == 6)) {
            return UCrc32("SuspensionSimple");
        }
    }
    if (mechanic == BEHAVIOR_MECHANIC_AI) {
        AIBehaviors *ab = ai_behaviors;
        UCrc32 sig = ab->signature;
        while (sig != UCrc32::kNull) {
            if (mDriverClass == ab->dclass &&
                (mClass == ab->vclass || ab->vclass == UCrc32::kNull)) {
                return ab->signature;
            }
            ab++;
            sig = ab->signature;
        }
        return sig;
    }
    if (mechanic == BEHAVIOR_MECHANIC_EFFECTS) {
        if (mDriverClass == DRIVER_HUMAN ||
            static_cast<const ISimable *>(this)->IsPlayer()) {
            return UCrc32("EffectsPlayer");
        }
    }
    Attrib::StringKey behaviourKey;
    const void *data = mAttributes.GetAttributePointer(mechanic.GetHash32(), 0);
    if (data != nullptr) {
        return UCrc32(static_cast<const char *>(data));
    }
    return UCrc32::kNull;
}

void PVehicle::LoadBehaviors(const UMath::Vector3 &initialPos, const UMath::Matrix4 &initMat) {
    if (IsDirty()) {
        return;
    }
    if (mEngine != nullptr) {
        mStartingNOS = mEngine->GetNOSCapacity();
    }
    UMath::Vector3 linearVel;
    bMemSet(&linearVel, 0, sizeof(UMath::Vector3));
    UMath::Vector3 Dimension;
    mBounds->GetHalfDimensions(Dimension);
    unsigned int collision_mask = 0;
    DriverClass dc = mDriverClass;
    if (dc == DRIVER_COP) {
        collision_mask = 0x80;
    } else if (dc != DRIVER_HUMAN && dc != DRIVER_RACER && dc != DRIVER_REMOTE) {
        collision_mask = 0x40;
    }
    float mass = mAttributes.MASS();
    const UMath::Vector3 &tensorScale = UMath::Vector4To3(mAttributes.TENSOR_SCALE());
    UMath::Vector3 initMoment = Util_GenerateCarTensor(mass, Dimension.x, Dimension.y, Dimension.z, tensorScale);
    UCrc32 rbclass = LookupBehaviorSignature(BEHAVIOR_MECHANIC_RIGIDBODY);
    Sim::Param rbparam;
    LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY), rbclass, rbparam);

    UCrc32 iclass = LookupBehaviorSignature(BEHAVIOR_MECHANIC_INPUT);
    Sim::Param inputparam;
    LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_INPUT), iclass, inputparam);

    UCrc32 engine = LookupBehaviorSignature(BEHAVIOR_MECHANIC_ENGINE);
    Sim::Param engineparam;
    LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_ENGINE), engine, engineparam);

    UCrc32 suspclass = LookupBehaviorSignature(BEHAVIOR_MECHANIC_SUSPENSION);
    Sim::Param suspparam;
    LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_SUSPENSION), suspclass, suspparam);

    UCrc32 damageclass = LookupBehaviorSignature(BEHAVIOR_MECHANIC_DAMAGE);
    Sim::Param damageparam;
    LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_DAMAGE), damageclass, damageparam);

    UCrc32 drawclass = LookupBehaviorSignature(BEHAVIOR_MECHANIC_DRAW);
    Sim::Param drawparam;
    LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_DRAW), drawclass, drawparam);

    UCrc32 audioclass = LookupBehaviorSignature(BEHAVIOR_MECHANIC_AUDIO);
    Sim::Param audioparam;
    LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_AUDIO), audioclass, audioparam);

    UCrc32 aiclass = LookupBehaviorSignature(BEHAVIOR_MECHANIC_AI);
    Sim::Param aiparam;
    LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_AI), aiclass, aiparam);

    UCrc32 effectsclass = LookupBehaviorSignature(BEHAVIOR_MECHANIC_EFFECTS);
    Sim::Param effectsparam;
    LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_EFFECTS), effectsclass, effectsparam);

    UCrc32 resetclass = LookupBehaviorSignature(BEHAVIOR_MECHANIC_RESET);
    Sim::Param resetparam;
    LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_RESET), resetclass, resetparam);

    ResetBehavior(UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY));
    ResetBehavior(UCrc32(BEHAVIOR_MECHANIC_INPUT));
    ResetBehavior(UCrc32(BEHAVIOR_MECHANIC_SUSPENSION));
    ResetBehavior(UCrc32(BEHAVIOR_MECHANIC_ENGINE));
    ResetBehavior(UCrc32(BEHAVIOR_MECHANIC_DAMAGE));
    ResetBehavior(UCrc32(BEHAVIOR_MECHANIC_EFFECTS));
}

bool PVehicle::SetVehicleOnGround(const UMath::Vector3 &resetPos, const UMath::Vector3 &initialVec) {
    IRigidBody *rigid_body = static_cast<ISimable *>(this)->GetRigidBody();
    if (rigid_body == nullptr || mCollisionBody == nullptr) {
        return false;
    }
    bool success = true;
    MJumpCut msg(static_cast<ISimable *>(this)->GetWorldID());
    UCrc32 port("CameraMessagePort");
    msg.Send(port);
    UMath::Vector3 dim;
    rigid_body->GetDimension(dim);
    UMath::Vector3 position;
    position.x = resetPos.x;
    position.y = resetPos.y + dim.y;
    position.z = resetPos.z;
    UMath::Matrix4 orientMat = Util_GenerateMatrix(initialVec, nullptr);
    rigid_body->SetLinearVelocity(UMath::Vector3::kZero);
    rigid_body->SetAngularVelocity(UMath::Vector3::kZero);
    float load = mCollisionBody->GetGravity() * rigid_body->GetMass();
    float worldHeight = 0.0f;
    WCollisionMgr mgr(0, 3);
    bool found = mgr.GetWorldHeightAtPointRigorous(position, worldHeight, nullptr);
    if (found) {
        if (mSuspension == nullptr) {
            position.x = resetPos.x;
            position.y = worldHeight + dim.y;
            position.z = resetPos.z;
        } else {
            WWorldPos wpos(dim.y);
            wpos.SetTolerance(1.0f);
            float wheelHeights[4];
            position.y = worldHeight;
            for (unsigned int i = 0; i < 4; i++) {
                const UMath::Vector3 &wheelPos = mSuspension->GetWheelLocalPos(i);
                UMath::Vector3 localPos;
                localPos.x = wheelPos.x;
                localPos.y = 0.0f;
                localPos.z = wheelPos.z;
                UMath::Vector3 worldPos;
                UMath::Rotate(localPos, orientMat, worldPos);
                worldPos.x += position.x;
                worldPos.y += position.y;
                worldPos.z += position.z;
                wpos.FindClosestFace(worldPos, true);
                if (wpos.OnValidFace()) {
                    float compression = mSuspension->GuessCompression(i, load);
                    float ride = mSuspension->GetRideHeight(i);
                    float faceHeight = wpos.HeightAtPoint(worldPos);
                    wheelHeights[i] = dim.y + faceHeight + (ride - compression);
                } else {
                    wheelHeights[i] = position.y;
                    success = false;
                }
            }
            if (!success) {
                position = resetPos;
            } else {
                float avg_y = (wheelHeights[0] + wheelHeights[1] + wheelHeights[2] + wheelHeights[3]) * 0.25f;
                position.x = resetPos.x;
                position.y = avg_y;
                position.z = resetPos.z;
            }
        }
    } else {
        position = resetPos;
        success = false;
    }
    rigid_body->PlaceObject(orientMat, position);
    IPlayer *player = nullptr;
    IOnlinePlayer *online = nullptr;
    if (static_cast<ISimable *>(this)->QueryInterface(&online)) {
        online->SendSetVehicleOnGround();
    }
    if (mArticulation != nullptr && !mArticulation->Pose()) {
        success = false;
    }
    ResetBehavior(UCrc32(BEHAVIOR_MECHANIC_SUSPENSION));
    ResetBehavior(UCrc32(BEHAVIOR_MECHANIC_ENGINE));
    ResetBehavior(UCrc32(BEHAVIOR_MECHANIC_AI));
    ResetBehavior(UCrc32(BEHAVIOR_MECHANIC_RESET));
    mOffWorld = !success;
    AITarget::Track(static_cast<const ISimable *>(this));
    return success;
}

ISimable *PVehicle::Construct(Sim::Param params) {
    const VehicleParams vp = params.Fetch< VehicleParams >(UCrc32(0xa6b47fac));
    Attrib::Gen::pvehicle attributes(
        Attrib::FindCollection(Attrib::Gen::pvehicle::ClassKey(), vp.carType), 0, nullptr);
    if (!attributes.IsValid()) {
        return nullptr;
    }
    const FECustomizationRecord *customizations = vp.customization;
    if (customizations == nullptr) {
        const char *vehicle_name = attributes.DefaultPresetRide();
        if (vehicle_name != nullptr) {
            unsigned int hash = bStringHashUpper(vehicle_name);
            PresetCar *preset = FindFEPresetCar(hash);
            if (preset != nullptr) {
                static FECustomizationRecord temp_record;
                temp_record.Default();
                temp_record.BecomePreset(preset);
                customizations = &temp_record;
            }
        }
        if (customizations == nullptr) {
            return nullptr;
        }
    }
    if (!customizations->WriteRecordIntoPhysics(attributes)) {
        return nullptr;
    }
    const Physics::Info::Performance *performance = vp.matched;
    if (performance != nullptr && !Physics::Upgrades::MatchPerformance(attributes, *performance)) {
        return nullptr;
    }
    if ((vp.Flags & 4) != 0) {
        Physics::Upgrades::RemoveJunkman(attributes, Physics::Upgrades::PUT_NOS);
        Physics::Upgrades::RemovePart(attributes, Physics::Upgrades::PUT_NOS);
    }
    if (GetMikeMannBuild() != 0) {
        int maxLevel = Physics::Upgrades::GetMaxLevel(attributes, Physics::Upgrades::PUT_NOS);
        Physics::Upgrades::SetLevel(attributes, Physics::Upgrades::PUT_NOS, maxLevel);
    }
    if ((vp.Flags & 0x10) != 0) {
        if (Physics::Upgrades::GetLevel(attributes, Physics::Upgrades::PUT_NOS) == 0) {
            int maxLevel = Physics::Upgrades::GetMaxLevel(attributes, Physics::Upgrades::PUT_NOS);
            if (maxLevel > 0) {
                Physics::Upgrades::SetLevel(attributes, Physics::Upgrades::PUT_NOS, 1);
            }
        }
    }
        const CollisionGeometry::Collection *geoms =
        CollisionGeometry::Lookup(UCrc32(attributes.MODEL()));
    if (geoms == nullptr) {
        return nullptr;
    }
    const CollisionGeometry::Bounds *bounds = geoms->GetRoot();
    if (bounds == nullptr) {
        return nullptr;
    }
    bool spooling_resources = (vp.Flags & 1) != 0;
    Resource resource(attributes, spooling_resources, vp.carClass == DRIVER_HUMAN);
    if (!resource.IsValid()) {
        return nullptr;
    }
    UTL::Std::list< Resource, _type_list > resources;
    resources.push_back(resource);
    Attrib::RefSpec trailer_ref;
    const Attrib::RefSpec *src = reinterpret_cast< const Attrib::RefSpec * >(
        attributes.GetAttributePointer(0x9a5537fe, 0));
    if (src != nullptr) {
        trailer_ref = *src;
    }
    if (trailer_ref.GetCollectionKey() != 0) {
        Attrib::Gen::pvehicle trailerAttribs(trailer_ref.GetCollectionKey(), 0, nullptr);
        Resource trailerResource(trailerAttribs, spooling_resources, false);
        resources.push_back(trailerResource);
    }
    if (!MakeRoom(vp.VehicleCache, resources)) {
        return nullptr;
    }
    Physics::Info::Performance perf;
    if (performance == nullptr && (vp.Flags & 8) != 0) {
        if (Physics::Info::ComputePerformance(attributes, perf)) {
            performance = &perf;
        }
    }
    if (!CanSpawnRigidBody(vp.initialPos, true)) {
        return nullptr;
    }
    const char *cache_name = nullptr;
    if (vp.VehicleCache != nullptr) {
        cache_name = vp.VehicleCache->GetCacheName();
    }
    PVehicle *pv = new PVehicle(vp.carClass, attributes, vp.initialVec, vp.initialPos, bounds,
                                customizations, resource, performance, cache_name);
    if ((vp.Flags & 2) != 0) {
        pv->SetVehicleOnGround(vp.initialPos, vp.initialVec);
    }
    ISimable *result = nullptr;
    if (pv != nullptr) {
        result = static_cast< ISimable * >(pv);
    }
    return result;
}

bool PVehicle::MakeRoom(IVehicleCache *whosasking, const UTL::Std::list<Resource, _type_list> &resources) {
    unsigned int newresources = 0;
    bool needs_compositing = false;
    for (UTL::Std::list<Resource, _type_list>::const_iterator res_iter = resources.begin();
         res_iter != resources.end(); ++res_iter) {
        const Resource &resource = *res_iter;
        unsigned int cost = resource.Cost;
        if (resource.NeedsCompositing()) {
            needs_compositing = true;
        }
        for (PVehicle *v = mInstances.GetHead(); v != mInstances.EndOfList();
             v = v->GetNext()) {
            if (v->mResources.Type == resource.Type && CanInstancesShareResourceCost(resource.Type)) {
                cost = 0;
                break;
            }
        }
        newresources += cost;
    }
    return true;
}
