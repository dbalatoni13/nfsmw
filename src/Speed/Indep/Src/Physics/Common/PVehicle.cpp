#include "Speed/Indep/Src/Physics/PVehicle.h"

#include <algorithm>

#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/FE/FECustomizationRecord.h"
#include "Speed/Indep/Src/Generated/Events/EPerfectLaunch.hpp"
#include "Speed/Indep/Src/Generated/Events/EPlayerAirborne.hpp"
#include "Speed/Indep/Src/Generated/Messages/MJumpCut.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ITrafficCenter.h"
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
#include "Speed/Indep/Src/Interfaces/Simables/IRecordablePlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRenderable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISpikeable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Behaviors/DamageVehicle.h"
#include "Speed/Indep/Src/Physics/Behaviors/RigidBody.h"
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

HINTERFACE IRaceEngine::_IHandle() {
    return (HINTERFACE)_IHandle;
}

class OnlineRacer;

class IOnlinePlayer : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() { return (HINTERFACE)_IHandle; }
    IOnlinePlayer(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}
    virtual ~IOnlinePlayer() {}
    virtual void SetOnlineRacer();
    virtual OnlineRacer *GetOnlineRacer();
    virtual void SendSetVehicleOnGround();
};

struct CarPartDatabase {
    CarType GetCarType(unsigned int hash);
};
extern CarPartDatabase CarPartDB;
bool CarInfo_IsSkinned(CarType type);
unsigned int CarInfo_GetResourceCost(CarType type, bool is_player, bool split_screen);
unsigned int CarInfo_GetResourcePool(bool needs_compositing);
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

inline PVehicle::LaunchState::LaunchState()
    : Time(0.0f), //
      Amount(0.0f) {}

inline void PVehicle::LaunchState::Clear() {
    Time = 0.0f;
    Amount = 0.0f;
}

inline bool PVehicle::LaunchState::IsSet() const {
    return Time > 0.0f;
}

inline void PVehicle::LaunchState::Set(float time) {
    Time = time;
}

inline void PVehicle::LaunchState::Tick(float dT) {
    Time -= dT;
}

const ISimable *PVehicle::GetSimable() const { return static_cast<const ISimable *>(this); }

ISimable *PVehicle::GetSimable() { return static_cast<ISimable *>(this); }

float PVehicle::GetSpeed() const { return mSpeed; }

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

void PVehicle::SetDriverClass(DriverClass cclass) {
    if (mDriverClass != cclass) {
        mDriverClass = cclass;
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
    ReleaseBehavior(UCrc32(BEHAVIOR_MECHANIC_DRAW));
    ReleaseBehavior(UCrc32(BEHAVIOR_MECHANIC_AUDIO));
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
    if (static_cast<ISimable *>(this)->QueryInterface(&ieff)) {
        ieff->Purge();
    }
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
        Physics::Tunings::Path path = static_cast<Physics::Tunings::Path>(i);
        mCustomization->SetTuning(path, tunings.Value[i]);
    }
}

bool PVehicle::OnTask(HSIMTASK htask, float dT) {
    ProfileNode profile_node("OnTask", 0);
    if (mTaskFX == htask) {
        OnTaskFX(dT);
        return true;
    }
    return PhysicsObject::OnTask(htask, dT);
}

void PVehicle::SetStaging(bool staging) {
    if (static_cast<unsigned int>(staging) != static_cast<unsigned int>(mStaging)) {
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
    if (mPerfectLaunch.IsSet()) {
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
    if (mPerfectLaunch.IsSet()) {}
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
    if (static_cast<unsigned int>(animate) != static_cast<unsigned int>(mAnimating)) {
        mBehaviorOverrides.clear();
        mAnimating = animate;
        ReloadBehaviors();
        PauseBehavior(UCrc32(BEHAVIOR_MECHANIC_AI), animate);
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
    UTL::Std::map<UCrc32, UCrc32, _type_ID_PVehicleChangeReq>::iterator iter = mBehaviorOverrides.find(mechanic);
    if (iter != mBehaviorOverrides.end()) {
        mBehaviorOverrides.erase(iter);
        mOverrideDirty = true;
    }
}

void PVehicle::OnBehaviorChange(const UCrc32 &mechanic) {
    PhysicsObject::OnBehaviorChange(mechanic);
    if (mechanic == UCrc32(BEHAVIOR_MECHANIC_AI)) {
        static_cast<ISimable *>(this)->QueryInterface(&mAI);
    } else if (mechanic == UCrc32(BEHAVIOR_MECHANIC_INPUT)) {
        static_cast<ISimable *>(this)->QueryInterface(&mInput);
    } else if (mechanic == UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY)) {
        if (static_cast<ISimable *>(this)->QueryInterface(&mCollisionBody)) {
            mCollisionBody->SetAnimating(mAnimating);
        }
        static_cast<ISimable *>(this)->QueryInterface(&mArticulation);
    } else if (mechanic == UCrc32(BEHAVIOR_MECHANIC_DRAW)) {
        static_cast<ISimable *>(this)->QueryInterface(&mRenderable);
    } else if (mechanic == UCrc32(BEHAVIOR_MECHANIC_AUDIO)) {
        static_cast<ISimable *>(this)->QueryInterface(&mAudible);
    } else if (mechanic == UCrc32(BEHAVIOR_MECHANIC_SUSPENSION)) {
        if (!static_cast<ISimable *>(this)->QueryInterface(&mSuspension)) {
            return;
        }
        if (mCollisionBody == nullptr) {
            return;
        }
        {
            float speed = UMath::Dot(mCollisionBody->GetForwardVector(),
                                     static_cast<ISimable *>(this)->GetRigidBody()->GetLinearVelocity());
            mSuspension->MatchSpeed(speed);
        }
    } else if (mechanic == UCrc32(BEHAVIOR_MECHANIC_ENGINE)) {
        static_cast<ISimable *>(this)->QueryInterface(&mTranny);
        if (!static_cast<ISimable *>(this)->QueryInterface(&mEngine)) {
            return;
        }
        mEngine->ChargeNOS(mStartingNOS - mEngine->GetNOSCapacity());
        if (mCollisionBody == nullptr) {
            return;
        }
        {
            float speed = UMath::Dot(mCollisionBody->GetForwardVector(),
                                     static_cast<ISimable *>(this)->GetRigidBody()->GetLinearVelocity());
            mEngine->MatchSpeed(speed);
        }
    } else if (mechanic == UCrc32(BEHAVIOR_MECHANIC_DAMAGE)) {
        static_cast<ISimable *>(this)->QueryInterface(&mDamage);
    }
}

void PVehicle::SetSpeed(float speed) {
    mSpeed = speed;
    mPerfectLaunch.Clear();
    if (mCollisionBody != nullptr) {
        UMath::Vector3 vel;
        UMath::Scale(mCollisionBody->GetForwardVector(), speed, vel);
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
            IRaceEngine *raceEngine = reinterpret_cast<IRaceEngine *>(
                (*reinterpret_cast<UTL::COM::Object **>(mEngine))->_mInterfaces.Find((HINTERFACE)IRaceEngine::_IHandle)
            );
            bool hasRaceEngine = raceEngine != nullptr;
            if (hasRaceEngine) {
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
    if (IsBehaviorActive(UCrc32(BEHAVIOR_MECHANIC_DRAW)) && mRenderable != nullptr && mRenderable->IsRenderable()) {
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
        PauseBehavior(UCrc32(BEHAVIOR_MECHANIC_SUSPENSION),
            mCollisionBody->IsSleeping() && IsDestroyed());
        if (mTranny != nullptr) {
            if (!mTranny->IsGearChanging()) {
                mSpeedometer = mTranny->GetSpeedometer();
            }
        } else {
            mSpeedometer = mAbsSpeed;
        }
        unsigned int num_onground = 0;
        if (mSuspension != nullptr) {
            num_onground = mSuspension->GetNumWheelsOnGround();
            if (num_onground == 0 && mWheelsOnGround != 0 && mDriverClass == DRIVER_HUMAN) {
                new EPlayerAirborne(ISimable::GetInstanceHandle());
            }
            mWheelsOnGround = num_onground;
        }
        if (mSuspension == nullptr || num_onground == 0) {
            mTimeInAir = mTimeInAir + dT;
        } else {
            mTimeInAir = 0.0f;
        }
        if (IsStaging()) {
            DoStaging(dT);
        } else {
            if (mPerfectLaunch.IsSet()) {
                if (mTranny != nullptr) {
                    if (!mTranny->IsGearChanging()) {
                        mPerfectLaunch.Time -= dT;
                        if (mPerfectLaunch.Time <= 0.0f) {
                            mPerfectLaunch.Amount = 0.0f;
                            mPerfectLaunch.Time = 0.0f;
                        }
                    } else {
                        if (mDriverStyle == STYLE_DRAG) {
                            mPerfectLaunch.Time = 0.0f;
                            mPerfectLaunch.Amount = 0.0f;
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
        if (dirty != nullptr && dirty->IsDirty()) {
            delete dirty;
        }
        dirty = next;
    }
}

const Physics::Tunings *PVehicle::GetTunings() const {
    if (GetDriverClass()) {
        return nullptr;
    }
    if (Tweak_UseTweakerTunings) {
        static Physics::Tunings tunings;
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
        return static_cast<const FECustomizationRecord *>(mCustomization)->GetTunings();
    }
    return nullptr;
}

unsigned int PVehicle::CountResources() {
    unsigned int total_resources = 0;
    UTL::Std::list<Resource, _type_list> resource_list;
    for (PVehicle *vehicle = mInstances.GetHead(); vehicle != mInstances.EndOfList();
         vehicle = vehicle->GetNext()) {
        bool found = false;
        for (UTL::Std::list<Resource, _type_list>::const_iterator iter = resource_list.begin();
             iter != resource_list.end(); iter++) {
            const Resource &resource = *iter;
            if (resource.Type == vehicle->mResources.Type) {
                found = true;
                break;
            }
        }
        int cost = 0;
        if (!(found && CanInstancesShareResourceCost(vehicle->mResources.Type))) {
            resource_list.push_back(vehicle->mResources);
            cost = vehicle->mResources.Cost;
        }
        total_resources += static_cast<unsigned int>(cost);
    }
    return total_resources;
}

void PVehicle::OnTaskFX(float dT) {
    IRigidBody &rigidBody = *static_cast<ISimable *>(this)->GetRigidBody();
    if (&rigidBody) {}
    {
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
    }
    if (mTranny != nullptr && mTranny->IsReversing()) {
        GlareOn(static_cast<VehicleFX::ID>(0xc0));
    } else {
        GlareOff(static_cast<VehicleFX::ID>(0xc0));
    }
}

void PVehicle::OnBeginMode(const PhysicsMode mode) {
    if (mode == PHYSICS_MODE_INACTIVE) {
        IVehicle::AddToList(VEHICLE_INACTIVE);
        if (mCollisionBody != nullptr) {
            mCollisionBody->DisableTriggering();
        }
        Reset();
    } else if (mode == PHYSICS_MODE_EMULATED) {
    } else if (mode == PHYSICS_MODE_SIMULATED) {
        if (mCollisionBody != nullptr) {
            mCollisionBody->EnableModeling();
        }
        CameraAI::AddAvoidable(static_cast<IBody *>(this));
        PauseBehavior(UCrc32(BEHAVIOR_MECHANIC_DRAW), false);
        PauseBehavior(UCrc32(BEHAVIOR_MECHANIC_SUSPENSION), false);
        PauseBehavior(UCrc32(BEHAVIOR_MECHANIC_ENGINE), false);
        PauseBehavior(UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY), false);
        PauseBehavior(UCrc32(BEHAVIOR_MECHANIC_EFFECTS), false);
        PauseBehavior(UCrc32(BEHAVIOR_MECHANIC_RESET), false);
        PauseBehavior(UCrc32(BEHAVIOR_MECHANIC_AUDIO), false);
        PauseBehavior(UCrc32(BEHAVIOR_MECHANIC_DAMAGE), false);
    }
}

void PVehicle::OnEndMode(const PhysicsMode mode) {
    if (mode == PHYSICS_MODE_INACTIVE) {
        if (mCollisionBody != nullptr) {
            mCollisionBody->EnableTriggering();
        }
        Reset();
        IVehicle::UnList(VEHICLE_INACTIVE);
    } else if (mode == PHYSICS_MODE_EMULATED) {
    } else if (mode == PHYSICS_MODE_SIMULATED) {
        if (mCollisionBody != nullptr) {
            mCollisionBody->DisableModeling();
        }
        CameraAI::RemoveAvoidable(static_cast<IBody *>(this));
        PauseBehavior(UCrc32(BEHAVIOR_MECHANIC_DRAW), true);
        PauseBehavior(UCrc32(BEHAVIOR_MECHANIC_SUSPENSION), true);
        PauseBehavior(UCrc32(BEHAVIOR_MECHANIC_ENGINE), true);
        PauseBehavior(UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY), true);
        PauseBehavior(UCrc32(BEHAVIOR_MECHANIC_EFFECTS), true);
        PauseBehavior(UCrc32(BEHAVIOR_MECHANIC_RESET), true);
        PauseBehavior(UCrc32(BEHAVIOR_MECHANIC_AUDIO), true);
        PauseBehavior(UCrc32(BEHAVIOR_MECHANIC_DAMAGE), true);
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
    if (static_cast<ISimable *>(this)->GetCausality() == nullptr && explosion->GetCausality() != nullptr) {
        ICause *cause = ICause::FindInstance(explosion->GetCausality());
        if (cause != nullptr) {
            cause->OnCausedExplosion(explosion, static_cast<ISimable *>(this));
        }
    }
    IRigidBody *irb = static_cast<ISimable *>(this)->GetRigidBody();
    float factor;
    float targetspeed = 1.0f / irb->GetMass();
    factor = explosion->GetExpansionSpeed() * targetspeed;
    targetspeed = factor;
    UMath::Vector3 point_velocity;
    irb->GetPointVelocity(position, point_velocity);
    float speed = UMath::Dot(point_velocity, normal);
    if (speed < targetspeed) {
        float deltaspeed = targetspeed - speed;
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
    case DRIVER_REMOTE:
        IVehicle::AddToList(VEHICLE_PLAYERS);
        IVehicle::AddToList(VEHICLE_RACERS);
        IVehicle::AddToList(VEHICLE_REMOTE);
        break;
    case DRIVER_COP:
        IVehicle::AddToList(VEHICLE_AI);
        IVehicle::AddToList(VEHICLE_AICOPS);
        break;
    case DRIVER_RACER:
        IVehicle::AddToList(VEHICLE_AI);
        IVehicle::AddToList(VEHICLE_AIRACERS);
        IVehicle::AddToList(VEHICLE_RACERS);
        break;
    case DRIVER_TRAFFIC:
        IVehicle::AddToList(VEHICLE_AI);
        IVehicle::AddToList(VEHICLE_AITRAFFIC);
        break;
    default:
        break;
    }
    if (!IsActive()) {
        IVehicle::AddToList(VEHICLE_INACTIVE);
    }
    if (mClass == VehicleClass::TRAILER) {
        IVehicle::AddToList(VEHICLE_TRAILERS);
    }
}

PVehicle::Resource::Resource(const Attrib::Gen::pvehicle &pvehicle, bool spool, bool is_player) {
    Flags = 0;
    CarPartDatabase &db = CarPartDB;
    const char *text = pvehicle.MODEL().GetString();
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
    , mCustomization(nullptr) //
    , mInput(nullptr) //
    , mCollisionBody(nullptr) //
    , mSuspension(nullptr) //
    , mEngine(nullptr) //
    , mDamage(nullptr) //
    , mTranny(nullptr) //
    , mAI(nullptr) //
    , mArticulation(nullptr) //
    , mRenderable(nullptr) //
    , mAudible(nullptr) //
    , mSequencer(nullptr) //
    , mTaskFX(nullptr) //
    , mClass() //
    , mSpeed(0.0f) //
    , mAbsSpeed(0.0f) //
    , mSpeedometer(0.0f) //
    , mTimeInAir(0.0f) //
    , mSlipAngle(0.0f) //
    , mWheelsOnGround(0) //
    , mLocalVel(UMath::Vector3::kZero) //
    , mDriverClass(dc) //
    , mDriverStyle(STYLE_RACING) //
    , mGlareState(0) //
    , mStartingNOS(0.0f) //
    , mBrakeTime(0.0f) //
    , mForceStop(0) //
    , mPhysicsMode(PHYSICS_MODE_SIMULATED) //
    , mAnimating(false) //
    , mStaging(false) //
    , mPerfectLaunch() //
    , mBehaviorOverrides() //
    , mResources() //
{
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
        mPerformance = *performance;
        mPerformanceValid = true;
    }
    mInstances.AddTail(this);
    AITarget::Register(static_cast<ISimable *>(this));
    if (customization != nullptr) {
        FECustomizationRecord *pFVar = static_cast<FECustomizationRecord *>(operator new(0x198));
        memcpy(pFVar, customization, 0x198);
        mCustomization = pFVar;
    }
    mClass = mAttributes.CLASS();
    IVehicle::AddToList(VEHICLE_ALL);
    UpdateListing();
    switch (mDriverClass) {
    case DRIVER_HUMAN:
        mTaskFX = AddTask("FX", 0.0f, 0.0f, Sim::TASK_FRAME_FIXED);
        break;
    case DRIVER_TRAFFIC:
        mTaskFX = AddTask("FX", 0.05f, 0.0f, Sim::TASK_FRAME_FIXED);
        break;
    default:
        mTaskFX = AddTask("FX", 0.02f, 0.0f, Sim::TASK_FRAME_FIXED);
        break;
    }
    Debugable::MakeDebugable(DBG_PHYSICS_RACERS);
    Reset();
    if (mDamage != nullptr) {
        mDamage->ResetDamage();
    }
    mGlareState = 0;
    UMath::Matrix4 initMat;
    initMat = Util_GenerateMatrix(initialVec, nullptr);
    LoadBehaviors(initialPos, initMat);
    SetOwnerObject(this);
    const Attrib::StringKey seq(mAttributes.EventSequencer());
    if (seq.IsNotEmpty()) {
        mSequencer = EventSequencer::Create(this, static_cast<EventSequencer::IContext *>(this),
                                            UCrc32(seq.GetString()), Sim::GetTime(), 0.0f);
    }
    OnBeginMode(PHYSICS_MODE_SIMULATED);
}

PVehicle::~PVehicle() {
    PhysicsObject::DetachAll();
    AITarget::UnRegister(static_cast<ISimable *>(this));
    IAttributeable::UnRegister(this);
    if (mCustomization != nullptr) {
        delete static_cast<FECustomizationRecord *>(mCustomization);
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
}

UCrc32 PVehicle::LookupBehaviorSignature(const Attrib::StringKey &mechanic) const {
    if (mechanic == BEHAVIOR_MECHANIC_AUDIO) {
        if (!IsSoundEnabled) {
            return UCrc32::kNull;
        }
    }
    UTL::Std::map<UCrc32, UCrc32, _type_ID_PVehicleChangeReq>::const_iterator iter =
        mBehaviorOverrides.find(UCrc32(mechanic));
    if (iter != mBehaviorOverrides.end()) {
        return (*iter).second;
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
        switch (mDriverClass) {
        case DRIVER_RACER:
        case DRIVER_NONE:
        case DRIVER_REMOTE:
            return UCrc32("SuspensionSimple");
        default:
            break;
        }
    }
    if (mechanic == BEHAVIOR_MECHANIC_AI) {
        const AIBehaviors *aibehavior = ai_behaviors;
        UCrc32 signature = aibehavior->signature;
        while (aibehavior->signature != UCrc32::kNull) {
            if (mDriverClass == aibehavior->dclass) {
                if (mClass == aibehavior->vclass || aibehavior->vclass == UCrc32::kNull) {
                    signature = aibehavior->signature;
                    break;
                }
            }
            aibehavior++;
        }
        return signature;
    }
    if (mechanic == BEHAVIOR_MECHANIC_EFFECTS) {
        if (mDriverClass == DRIVER_HUMAN ||
            static_cast<const ISimable *>(this)->IsPlayer()) {
            return UCrc32("EffectsPlayer");
        }
    }
    Attrib::Instance instance(nullptr, 0, nullptr);
    Attrib::StringKey behaviourKey;
    Attrib::Attribute atr = mAttributes.Get(static_cast<unsigned int>(mechanic));
    if (atr.Get(0, behaviourKey)) {
    } else {
        return UCrc32::kNull;
    }
    return UCrc32(behaviourKey);
}

void PVehicle::LoadBehaviors(const UMath::Vector3 &initialPos, const UMath::Matrix4 &initMat) {
    if (IsDirty()) {
        return;
    }
    ProfileNode profile_node("LoadBehaviors", 0);
    if (mEngine != nullptr) {
        mStartingNOS = mEngine->GetNOSCapacity();
    }
    UMath::Vector3 linearVel;
    memset(&linearVel, 0, sizeof(UMath::Vector3));
    UMath::Vector3 Dimension;
    mBounds->GetHalfDimensions(Dimension);
    unsigned int collision_mask = 0;
    switch (mDriverClass) {
    case DRIVER_HUMAN:
    case DRIVER_RACER:
    case DRIVER_REMOTE:
        break;
    case DRIVER_COP:
        collision_mask = 0x80;
        break;
    default:
        collision_mask |= 0x40;
        break;
    }
    float mass = mAttributes.MASS();
    const UMath::Vector3 &tensorScale = UMath::Vector4To3(mAttributes.TENSOR_SCALE());
    UMath::Vector3 initMoment = Util_GenerateCarTensor(mass, Dimension.x, Dimension.y, Dimension.z, tensorScale);

    UCrc32 rbclass = LookupBehaviorSignature(BEHAVIOR_MECHANIC_RIGIDBODY);
    LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY), rbclass,
                 RBComplexParams(initialPos, linearVel, UMath::Vector3::kZero, initMat, mass, initMoment, Dimension, mBounds, true, collision_mask));

    UCrc32 iclass = LookupBehaviorSignature(BEHAVIOR_MECHANIC_INPUT);
    LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_INPUT), iclass, Sim::Param());

    UCrc32 engine = LookupBehaviorSignature(BEHAVIOR_MECHANIC_ENGINE);
    LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_ENGINE), engine, EngineParams());

    UCrc32 suspclass = LookupBehaviorSignature(BEHAVIOR_MECHANIC_SUSPENSION);
    LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_SUSPENSION), suspclass, SuspensionParams());

    UCrc32 damageclass = LookupBehaviorSignature(BEHAVIOR_MECHANIC_DAMAGE);
    LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_DAMAGE), damageclass, DamageParams());

    UCrc32 drawclass = LookupBehaviorSignature(BEHAVIOR_MECHANIC_DRAW);
    LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_DRAW), drawclass, Sim::Param());

    UCrc32 audioclass = LookupBehaviorSignature(BEHAVIOR_MECHANIC_AUDIO);
    LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_AUDIO), audioclass, Sim::Param());

    UCrc32 aiclass = LookupBehaviorSignature(BEHAVIOR_MECHANIC_AI);
    LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_AI), aiclass, AIParams());

    UCrc32 effectsclass = LookupBehaviorSignature(BEHAVIOR_MECHANIC_EFFECTS);
    LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_EFFECTS), effectsclass, Sim::Param());

    UCrc32 resetclass = LookupBehaviorSignature(BEHAVIOR_MECHANIC_RESET);
    LoadBehavior(UCrc32(BEHAVIOR_MECHANIC_RESET), resetclass, Sim::Param());

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
    msg.Send(UCrc32("CameraMessagePort"));
    const UMath::Vector3 dim = rigid_body->GetDimension();
    UMath::Vector3 position = resetPos;
    position.y += dim.y;
    UMath::Matrix4 orientMat = Util_GenerateMatrix(initialVec, nullptr);
    rigid_body->SetLinearVelocity(UMath::Vector3::kZero);
    rigid_body->SetAngularVelocity(UMath::Vector3::kZero);
    float load = mCollisionBody->GetGravity() * rigid_body->GetMass();
    float worldHeight;
    if (!WCollisionMgr(0, 3).GetWorldHeightAtPointRigorous(position, worldHeight, nullptr)) {
        position = resetPos;
        success = false;
    } else if (mSuspension == nullptr) {
        position = resetPos;
        position.y = worldHeight + dim.y;
    } else {
        WWorldPos wpos(dim.y);
        wpos.SetTolerance(1.0f);
        UMath::Vector4 plane[4];
        UMath::Vector4 axle_center = UMath::Vector4::kZero;
        position.y = worldHeight;
        UMath::Vector4 p4 = UMath::Vector4Make(position, 1.0f);
        for (unsigned int i = 0; i < 4; i++) {
            UMath::Vector4 &this_corner = plane[i];
            const UMath::Vector3 &wheelPos = mSuspension->GetWheelLocalPos(i);
            this_corner = UMath::Vector4Make(wheelPos, 0.0f);
            this_corner.y = 0.0f;
            UMath::ScaleAdd(this_corner, 0.25f, axle_center, axle_center);
            UMath::Rotate(this_corner, orientMat, this_corner);
            UMath::Add(this_corner, p4, this_corner);
            wpos.FindClosestFace(UMath::Vector4To3(this_corner), true);
            if (wpos.OnValidFace()) {
                float compression = mSuspension->GuessCompression(i, load);
                float ride = mSuspension->GetRideHeight(i);
                this_corner.y = dim.y + wpos.HeightAtPoint(UMath::Vector4To3(this_corner)) + (ride - compression);
            } else {
                success = false;
            }
        }
        if (!success) {
            position = resetPos;
        } else {
            UMath::Vector4 front;
            UMath::Vector4 rear;
            UMath::Vector4 right;
            UMath::Vector4 left;
            UMath::Vector4 world_axle;
            UMath::Vector4 p0;
            UMath::Vector4 p1;
            UMath::AddScale(plane[0], plane[1], 0.5f, front);
            UMath::AddScale(plane[2], plane[3], 0.5f, rear);
            UMath::AddScale(plane[0], plane[2], 0.5f, left);
            UMath::AddScale(plane[1], plane[3], 0.5f, right);
            UMath::Subxyz(front, rear, orientMat.v2);
            UMath::Unitxyz(orientMat.v2);
            UMath::Subxyz(right, left, orientMat.v0);
            UMath::Unitxyz(orientMat.v0);
            UnitCrossxyz(orientMat.v2, orientMat.v0, orientMat.v1);
            UnitCrossxyz(orientMat.v1, orientMat.v2, orientMat.v0);
            UMath::Rotate(axle_center, orientMat, world_axle);
            AddScalexyz(front, rear, 0.5f, p0);
            AddScalexyz(right, left, 0.5f, p1);
            AddScalexyz(p0, p1, 0.5f, p4);
            UMath::Subxyz(p4, world_axle, p4);
            position = UMath::Vector4To3(p4);
        }
    }
    rigid_body->PlaceObject(orientMat, position);
    IPlayer *player = static_cast<ISimable *>(this)->GetPlayer();
    if (player != nullptr) {
        IOnlinePlayer *online = nullptr;
        if (player->QueryInterface(&online)) {
            online->SendSetVehicleOnGround();
        }
    }
    if (mArticulation != nullptr && !mArticulation->Pose()) {
        success = false;
    }
    ResetBehavior(UCrc32(BEHAVIOR_MECHANIC_SUSPENSION));
    ResetBehavior(UCrc32(BEHAVIOR_MECHANIC_ENGINE));
    ResetBehavior(UCrc32(BEHAVIOR_MECHANIC_AI));
    ResetBehavior(UCrc32(BEHAVIOR_MECHANIC_RESET));
    static_cast<IVehicle *>(this)->SetSpeed(0.0f);
    mOffWorld = !success;
    AITarget::Track(static_cast<const ISimable *>(this));
    return success;
}

ISimable *PVehicle::Construct(Sim::Param params) {
    const VehicleParams vp = params.Fetch< VehicleParams >(UCrc32(0xa6b47fac));
    Attrib::Gen::pvehicle attributes(vp.carType, 0, nullptr);
    if (!attributes.IsValid()) {
        return nullptr;
    }
    const char *vehicle_name;
    const FECustomizationRecord *customizations = vp.customization;
    if (customizations == nullptr) {
        vehicle_name = attributes.DefaultPresetRide();
        if (vehicle_name != nullptr) {
            PresetCar *preset = FindFEPresetCar(bStringHashUpper(vehicle_name));
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
    if (vp.matched != nullptr
        && !Physics::Upgrades::MatchPerformance(attributes, *vp.matched)) {
        return nullptr;
    }
    if ((vp.Flags & 4) != 0) {
        Physics::Upgrades::RemoveJunkman(attributes, Physics::Upgrades::PUT_NOS);
        Physics::Upgrades::RemovePart(attributes, Physics::Upgrades::PUT_NOS);
    }
    if (GetMikeMannBuild() != 0) {
        int new_nos = Physics::Upgrades::GetMaxLevel(attributes, Physics::Upgrades::PUT_NOS);
        Physics::Upgrades::SetLevel(attributes, Physics::Upgrades::PUT_NOS, new_nos);
    }
    if ((vp.Flags & 0x10) != 0) {
        if (Physics::Upgrades::GetLevel(attributes, Physics::Upgrades::PUT_NOS) == 0) {
            if (Physics::Upgrades::GetMaxLevel(attributes, Physics::Upgrades::PUT_NOS) > 0) {
                Physics::Upgrades::SetLevel(attributes, Physics::Upgrades::PUT_NOS, 1);
            }
        }
    }
        const CollisionGeometry::Collection *geoms =
        CollisionGeometry::Lookup(UCrc32(attributes.MODEL()));
    if (geoms == nullptr) {
        return nullptr;
    }
    if (geoms->GetRoot() == nullptr) {
        return nullptr;
    }
    bool spooling_resources = (vp.Flags & 1) != 0;
    bool is_player = vp.carClass == DRIVER_HUMAN;
    Resource resource(attributes, spooling_resources, is_player);
    if (!resource.IsValid()) {
        return nullptr;
    }
    UTL::Std::list< Resource, _type_list > resources;
    resources.push_back(resource);
    Attrib::RefSpec trailer_ref = attributes.Trailer();
    Physics::Info::Performance perf;
    if (trailer_ref.GetCollectionKey() != 0) {
        Attrib::Gen::pvehicle trailerAttribs(trailer_ref, 0, nullptr);
        resources.push_back(Resource(trailerAttribs, spooling_resources, false));
    }
    if (!MakeRoom(vp.VehicleCache, resources)) {
        return nullptr;
    }
    resources.clear();
    perf.Default();
    const Physics::Info::Performance *performance = nullptr;
    if (vp.matched != nullptr) {
        performance = vp.matched;
    } else if ((vp.Flags & 8) != 0) {
        if (Physics::Info::ComputePerformance(attributes, perf)) {
            performance = &perf;
        }
    }
    if (CanSpawnRigidBody(vp.initialPos, true)) {
        const char *cache_name;
        PVehicle *vehicle;
        if (vp.VehicleCache != nullptr) {
            cache_name = vp.VehicleCache->GetCacheName();
        } else {
            cache_name = nullptr;
        }
        vehicle = new PVehicle(vp.carClass, attributes, vp.initialVec, vp.initialPos,
                               geoms->GetRoot(),
                               customizations, resource, performance, cache_name);
        if ((vp.Flags & 2) != 0) {
            vehicle->SetVehicleOnGround(vp.initialPos, vp.initialVec);
        }
        if (vehicle != nullptr) {
            return static_cast<ISimable *>(vehicle);
        }
    }
    return nullptr;
}

bool PVehicle::MakeRoom(IVehicleCache *whosasking, const UTL::Std::list<Resource, _type_list> &resources) {
    CleanResources();

    unsigned int newinstances = resources.size();

    unsigned int newresources = 0;
    bool needs_compositing = false;
    for (UTL::Std::list<Resource, _type_list>::const_iterator res_iter = resources.begin();
         res_iter != resources.end(); res_iter++) {
        const Resource &resource = *res_iter;
        if (resource.NeedsCompositing()) {
            needs_compositing = true;
        }
        unsigned int cost = resource.Cost;
        for (PVehicle *vehicle = mInstances.GetHead(); vehicle != mInstances.EndOfList();
             vehicle = vehicle->GetNext()) {
            if (vehicle->mResources.Type == resource.Type) {
                if (CanInstancesShareResourceCost(resource.Type)) {
                    cost = 0;
                }
                break;
            }
        }
        newresources += cost;
    }

    unsigned int resource_limit = CarInfo_GetResourcePool(needs_compositing);
    unsigned int current_resources = CountResources();
    unsigned int current_instances = IVehicle::Count(VEHICLE_ALL);
    unsigned int needed_resources = 0;
    unsigned int needed_instances = 0;
    if (current_resources + newresources > resource_limit) {
        needed_resources = (current_resources + newresources) - resource_limit;
    }
    if (current_instances + newinstances > 10) {
        needed_instances = (current_instances + newinstances) - 10;
    }

    if (needed_resources == 0 && needed_instances == 0) {
        return true;
    }
    if (whosasking == nullptr) {
        return false;
    }

    ManagementList vehicle_list;
    vehicle_list.reserve(10);

    for (PVehicle *vehicle = mInstances.GetHead(); vehicle != mInstances.EndOfList();
         vehicle = vehicle->GetNext()) {
        ManageNode node;
        node.result = VCR_DONTCARE;
        node.resource.Flags = 0;
        node.instancecount = 0;
        node.vehicle = vehicle;
        node.resource = vehicle->mResources;
        vehicle_list.push_back(node);
    }

    for (ManageNode *node_iter = vehicle_list.begin();
         node_iter != vehicle_list.end(); ++node_iter) {
        ManageNode &node = *node_iter;
        if (node.result != VCR_WANT) {
            node.result = whosasking->OnQueryVehicleCache(node.vehicle, whosasking);
        }
    }

    for (ManageNode *node_iter = vehicle_list.begin();
         node_iter != vehicle_list.end(); ++node_iter) {
        ManageNode &node = *node_iter;
        if (node.result != VCR_WANT) {
            for (IVehicleCache *const *cache_iter =
                     UTL::Collections::Listable<IVehicleCache, 18>::GetList().begin();
                 cache_iter !=
                     UTL::Collections::Listable<IVehicleCache, 18>::GetList().end();
                 ++cache_iter) {
                IVehicleCache *cache = *cache_iter;
                if (!UTL::COM::ComparePtr(cache, whosasking)) {
                    if (cache->OnQueryVehicleCache(node.vehicle, whosasking) == VCR_WANT) {
                        node.result = VCR_WANT;
                        break;
                    }
                }
            }
        }
    }

    std::sort(vehicle_list.begin(), vehicle_list.end(), ManageNode::sort_by_keep);
    vehicle_list.print();

    for (UTL::Std::list<Resource, _type_list>::const_iterator res_iter = resources.begin();
         res_iter != resources.end(); res_iter++) {
        const Resource &resource = *res_iter;
        for (ManageNode *node_iter = vehicle_list.begin();
             node_iter != vehicle_list.end(); ++node_iter) {
            ManageNode &node = *node_iter;
            if (resource.Type == node.resource.Type) {
                node.result = VCR_WANT;
                break;
            }
        }
    }

    vehicle_list.print();

    if (needed_resources != 0) {
        CarType type = CARTYPE_NONE;
        eVehicleCacheResult pushresult = VCR_DONTCARE;
        for (ManageNode *node_iter = vehicle_list.begin();
             node_iter != vehicle_list.end(); ++node_iter) {
            ManageNode &node = *node_iter;
            CarType t = node.resource.Type;
            if (t != type) {
                pushresult = node.result;
                type = t;
            }
            if (pushresult == VCR_WANT) {
                node.result = VCR_WANT;
            }
        }
    }

    ManageNode *end_iter = std::remove_if(vehicle_list.begin(), vehicle_list.end(),
                                           ManageNode::is_kept);
    vehicle_list.erase(end_iter, vehicle_list.end());

    if (vehicle_list.size() == 0) {
        return false;
    }

    UTL::Std::map<CarType, unsigned int, _type_map> type_map;
    for (ManageNode *node_iter = vehicle_list.begin();
         node_iter != vehicle_list.end(); ++node_iter) {
        type_map[node_iter->resource.Type]++;
    }

    for (ManageNode *node_iter = vehicle_list.begin();
         node_iter != vehicle_list.end(); ++node_iter) {
        node_iter->instancecount = type_map[node_iter->resource.Type];
    }

    unsigned int found_instances = 0;
    ManageNode *node_iter = vehicle_list.begin();
    if (needed_resources != 0) {
        std::sort(vehicle_list.begin(), vehicle_list.end(),
                  ManageNode::sort_remove_resources);
        vehicle_list.print();

        unsigned int found_resources = 0;
        CarType type = CARTYPE_NONE;
        unsigned int type_cost = 0;
        for (node_iter = vehicle_list.begin(); node_iter != vehicle_list.end();
             ++node_iter) {
            CarType t = node_iter->resource.Type;
            if (t != type) {
                found_resources += type_cost;
                type = t;
            }
            if (found_resources >= needed_resources) {
                type_cost = 0;
                break;
            }
            type_cost = node_iter->resource.Cost;
            found_instances++;
        }

        if (found_resources + type_cost < needed_resources) {
            return false;
        }
    }

    if (found_instances < needed_instances) {
        std::sort(node_iter, vehicle_list.end(),
                  ManageNode::sort_remove_instances);
        vehicle_list.print();

        for (; node_iter != vehicle_list.end(); ++node_iter) {
            if (found_instances >= needed_instances) {
                break;
            }
            found_instances++;
        }

        if (found_instances < needed_instances) {
            return false;
        }
    }

    vehicle_list.erase(node_iter, vehicle_list.end());

    vehicle_list.print();

    for (ManageNode *node_iter = vehicle_list.begin();
         node_iter != vehicle_list.end(); ++node_iter) {
        ManageNode &node = *node_iter;
        PVehicle *killit = node.vehicle;
        for (IVehicleCache *const *citer =
                 UTL::Collections::Listable<IVehicleCache, 18>::GetList().begin();
             citer !=
                 UTL::Collections::Listable<IVehicleCache, 18>::GetList().end();
             ++citer) {
            IVehicleCache *cache = *citer;
            cache->OnRemovedVehicleCache(killit);
        }
        if (killit != nullptr) {
            delete killit;
        }
    }

    return true;
}

template void UTL::Vector<ICollisionBody *, 16>::push_back(ICollisionBody *const &);
template void UTL::Vector<IInputPlayer *, 16>::push_back(IInputPlayer *const &);
template void UTL::Vector<IRecordablePlayer *, 16>::push_back(IRecordablePlayer *const &);
template void UTL::Vector<ISpikeable *, 16>::push_back(ISpikeable *const &);
template UTL::Collections::Listable<ITrafficCenter, 8>::List::~List();
template UTL::Collections::Listable<ISpikeable, 10>::List::~List();
template Behavior *UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::CreateInstance(UCrc32, const BehaviorParams &);
