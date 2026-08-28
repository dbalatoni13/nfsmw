#ifndef AI_AIVEHICLE_H
#define AI_AIVEHICLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AIAvoidable.h"
#include "AIGoal.h"
#include "AdaptivePIDController.h"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/AI/AIAvoidable.h"
#include "Speed/Indep/Src/AI/AIMath.h"
#include "Speed/Indep/Src/Debug/Debugable.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICheater.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"
#include "Speed/Indep/Src/Sim/Collision.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "types.h"

// total size: 0x754
class AIVehicle : public VehicleBehavior, public IVehicleAI, public AIAvoidable, public Debugable {
  public:
    USE_FASTALLOC(AIVehicle);

    static Behavior *Construct(const BehaviorParams &bp);

    float GetOverSteerCorrection(float steer);

    const UMath::Vector3 &GetAngularVelocity() const {
        return this->mCollisionBody->GetAngularVelocity();
    }

    const UMath::Vector3 &GetLinearVelocity() const {
        return this->mCollisionBody->GetLinearVelocity();
    }

    const UMath::Vector3 &GetPosition() const {
        return this->mCollisionBody->GetPosition();
    }

    const UMath::Matrix4 &GetOrientation() const {
        return this->mCollisionBody->GetMatrix4();
    }

    const UMath::Vector3 &GetForwardVector() const {
        return UMath::Vector4To3(this->GetOrientation().v2);
    }

    const UMath::Vector3 &GetUpVector() const;
    const UMath::Vector3 &GetRightVector() const;

    // Behavior
    void OnTaskSimulate(float dT) override;

    // IVehicleAI
    ISimable *GetSimable() const override {
        return this->Behavior::GetOwner();
    }

    IVehicle *GetVehicle() const override {
        return this->VehicleBehavior::GetVehicle();
    }

    ISuspension *GetSuspension() const {
        return this->mISuspension;
    }

    virtual void Update(float dT);

    // IVehicleAI
    const AvoidableList &GetAvoidableList() override {
        return this->GetAvoidableNeighbors();
    }

    // AIAvoidable
    bool OnUpdateAvoidable(UMath::Vector3 &pos, float &sweep) override;

    // IVehicleAI
    void SetAvoidableRadius(float radius) override {
        this->mAvoidableRadius = radius;
    }

    const AISplinePath *GetSplinePath() override {
        return nullptr;
    }

    void ResetDriveToNav(eLaneSelection lane_selection) override;
#ifdef EA_BUILD_A124
    void ResetDriveToNav(UMath::Vector3 &target) override;
#endif
    bool ResetVehicleToRoadNav(short segInd, char laneInd, float timeStep) override;
    bool ResetVehicleToRoadNav(WRoadNav *other_nav) override;
    bool ResetVehicleToRoadPos(const UMath::Vector3 &position, const UMath::Vector3 &forwardVector) override;

    WRoadNav *GetDriveToNav() override {
        return this->mDriveToNav;
    }

    bool GetDrivableToDriveToNav() const override {
        return this->mDrivableToNav;
    }

    float GetDriveSpeed() override {
        return this->mDriveSpeed;
    }

    void SetDriveSpeed(float driveSpeed) override {
        this->mDriveSpeed = driveSpeed;
    }

    WRoadNav *GetCollNav(const UMath::Vector3 &forwardVector, float predictTime) override;
    WRoadNav *GetCurrentRoad() override;
    WRoadNav *GetFutureRoad() override;
    const UMath::Vector3 &GetFarFuturePosition() override;
    const UMath::Vector3 &GetFarFutureDirection() override;
    const UMath::Vector3 &GetSeekAheadPosition() override;

    const UMath::Vector3 &GetDriveTarget() override {
        return this->mDest;
    }

    void SetDriveTarget(const UMath::Vector3 &dest) override {
        this->mDest = dest;
    }

    float GetLookAhead() override;

    unsigned int GetDriveFlags() const override {
        return this->mDriveFlags;
    }

    void ClearDriveFlags() override {
        this->mDriveFlags = 0;
    }

    void DoReverse() override {
        this->mDriveFlags |= 4;
    }

    void DoSteering() override {
        this->mDriveFlags |= 1;
    }

    void DoGasBrake() override {
        this->mDriveFlags |= 2;
    }

    void DoDriving(unsigned int flags) override {
        this->mDriveFlags = flags;
    }

    float GetPathDistanceRemaining() override;

    void SetReverseOverride(float time) override;

    bool GetReverseOverride() override {
        return this->mReverseOverrideTimer > 0.0f;
    }

    void ClearReverseOverride();

    AITarget *GetTarget() const override {
        return this->mTarget;
    }

    // short GetDestSegment() const {}

    bool GetDrivableToTargetPos() const override {
        return this->mDrivableToTargetPos;
    }

    void SetSpawned() override;
    void UnSpawn() override;
    bool CanRespawn(bool respawnAvailable) override;

    // Overrides: IVehicleAI
    float GetLastSpawnTime() override {
        return this->mLastSpawnTime;
    }

    // Overrides: IVehicleAI
    const Attrib::Gen::aivehicle &GetAttributes() const override {
        return *this->mAttributes;
    }

    // Overrides: IVehicleAI
    float GetTopSpeed() const override {
        return this->mTopSpeed;
    }

    // Overrides: IVehicleAI
    IPursuit *GetPursuit() override {
        return this->mPursuit;
    }

    // Overrides: IVehicleAI
    IRoadBlock *GetRoadBlock() override {
        return this->mRoadBlock;
    }

    float GetAcceleration(float at) const override;

    virtual void OnCollision(const COLLISION_INFO &cinfo);
    virtual void OnDebugDraw();

    // Overrides: IVehicleAI
    bool IsCurrentGoal(const UCrc32 &name) override {
        return name == this->mGoalName;
    }

    UCrc32 &GetGoalName() override {
        return this->mGoalName;
    }

    void SetGoal(const UCrc32 &name);

    AIGoal *GetGoal() const {
        return this->mCurrentGoal;
    }

    // IVehicleAI
    bool IsCurrentAction(const UCrc32 &name) override {
        if (this->mCurrentGoal) {
            return this->mCurrentGoal->IsCurrentAction(name);
        } else {
            return false;
        }
    }

    UCrc32 GetActionName() override {
        if (this->mCurrentGoal) {
            return this->mCurrentGoal->GetActionName();
        } else {
            return (const char *)nullptr;
        }
    }

    float GetSkill() const override {
        return 0.0f;
    }

    float GetShortcutSkill() const override {
        return 0.0f;
    }

    float GetPercentRaceComplete() const override {
        return 0.0f;
    }

    EventSequencer::IEngine *GetEngine() const {
        return this->mIEngine;
    }

    ITransmission *GetTransmission() const {
        return this->mITransmission;
    }

    // IChassis *GetSuspension() const;

  protected:
    virtual bool OnClearCausality(float start_time);
    virtual void OnReverse(float dT);
    virtual void OnSteering(float dT);
    virtual void OnGasBrake(float dT);
    virtual void OnDriving(float dT);

    // Behavior
    void OnOwnerAttached(IAttachable *pOther) override;
    void OnOwnerDetached(IAttachable *pOther) override;
    bool OnTask(HSIMTASK hTask, float dT) override;

    const int GetPriority() const override {
        return -10;
    }

    AIVehicle(const BehaviorParams &bp, float update_rate, float stagger, Sim::TaskMode taskmode);
    ~AIVehicle();

    // Behavior
    void Reset() override {
        this->ResetDriveToNav(SELECT_CURRENT_LANE);
    }

    virtual void ResetInternals();
    void ClearGoal();

    void UpdateSpawnTimer(float dT) {
        this->mLastSpawnTime += dT;
    }

    void UpdateRoadNavInfo();
    void UpdateReverseOverride(float dT);
    void UpdateTargeting();
    bool WorldCollision(const UMath::Vector3 &pos, const UMath::Vector3 &dest);
    bool BarriersInPath(bool reverse);

    // Overrides: IVehicleAI
    bool GetWorldAvoidanceInfo(float dT, UMath::Vector3 &leftCollNormal, UMath::Vector3 &rightCollNormal) const override;

    virtual bool IsTetheredToTarget(UTL::COM::IUnknown *object) {
        return false;
    }

    void UpdateSimplePhysics(float dT);
    bool IsSimplePhysicsActive();

    // IVehicleAI
    void EnableSimplePhysics() override;
    void DisableSimplePhysics() override;
    void DoNOS() override;

    // Behavior
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    IInput *GetInput() const {
        return this->mIInput;
    }

    WRoadNav *mDriveToNav;             // offset 0x70, size 0x4
    ALIGN_16 UMath::Vector3 mDirToNav; // offset 0x74, size 0xC
    bool mDrivableToNav;               // offset 0x80, size 0x1
    float mDriveSpeed;                 // offset 0x84, size 0x4
    ALIGN_16 UMath::Vector3 mDest;     // offset 0x88, size 0xC
    float mReverseOverrideTimer;       // offset 0x94, size 0x4
    float mReverseOverrideSteer;       // offset 0x98, size 0x4
    bool mReverseOverrideDirection;    // offset 0x9C, size 0x1
    AITarget *mTarget;                 // offset 0xA0, size 0x4
    short mDestSegment;                // offset 0xA4, size 0x2
    bool mDrivableToTargetPos;         // offset 0xA8, size 0x1
    bool mReversingSpeed;              // offset 0xAC, size 0x1
    bool mSteeringBehind;              // offset 0xB0, size 0x1

  private:
    void UpdateRoads();

    HSIMTASK mThinkTask;                         // offset 0xB4, size 0x4
    float mLastSpawnTime;                        // offset 0xB8, size 0x4
    bool mCanRespawn;                            // offset 0xBC, size 0x1
    AIGoal *mCurrentGoal;                        // offset 0xC0, size 0x4
    IPursuit *mPursuit;                          // offset 0xC4, size 0x4
    IRoadBlock *mRoadBlock;                      // offset 0xC8, size 0x4
    UCrc32 mGoalName;                            // offset 0xCC, size 0x4
    float mAvoidableRadius;                      // offset 0xD0, size 0x4
    unsigned int mDriveFlags;                    // offset 0xD4, size 0x4
    ICollisionBody *mCollisionBody;              // offset 0xD8, size 0x4
    ITransmission *mITransmission;               // offset 0xDC, size 0x4
    ISuspension *mISuspension;                   // offset 0xE0, size 0x4
    EventSequencer::IEngine *mIEngine;           // offset 0xE4, size 0x4
    IInput *mIInput;                             // offset 0xE8, size 0x4
    WRoadNav *mCollNav;                          // offset 0xEC, size 0x4
    Attrib::Gen::aivehicle *mAttributes;         // offset 0xF0, size 0x4
    AI::Math::FloatSpring mDampedAngularVel;     // offset 0xF4, size 0x10
    AI::Math::FloatSpring mDampedAngle;          // offset 0x104, size 0x10
    float mRoadUpdateTimer;                      // offset 0x114, size 0x4
    float mRoadIncrementTimer;                   // offset 0x118, size 0x4
    float mSeekAheadTimer;                       // offset 0x11C, size 0x4
    ALIGN_16 UMath::Vector3 mSeekAheadPosition;  // offset 0x120, size 0xC
    ALIGN_16 UMath::Vector3 mFarFutureDirection; // offset 0x12C, size 0xC
    ALIGN_16 UMath::Vector3 mFarFuturePosition;  // offset 0x138, size 0xC
    short mLastFutureSegment;                    // offset 0x144, size 0x2
    short mLastFutureNodeInd;                    // offset 0x146, size 0x2
    WRoadNav mCurrentRoad;                       // offset 0x148, size 0x2F0
    WRoadNav mFutureRoad;                        // offset 0x438, size 0x2F0
    float mAccelData[10];                        // offset 0x728, size 0x28
    Mps mTopSpeed;                               // offset 0x750, size 0x4
};

class AIVehicleEmpty : public AIVehicle {
  public:
    static Behavior *Construct(const BehaviorParams &bp);

    AIVehicleEmpty(const BehaviorParams &bp);

    // Overrides: AIVehicle
    void Update(float dT) override {
        this->AIVehicle::Update(dT);
    }

    // Overrides: AIVehicle
    void OnDebugDraw() override {}
};

class AIVehiclePid : public AIVehicle {
  public:
    typedef AIVehicle Base;

    AIVehiclePid(const BehaviorParams &bp, float update_rate, float stagger, Sim::TaskMode taskmode);

    // Virtual overrides
    // IUnknown
    ~AIVehiclePid() override;

    // Behavior
    void Reset() override;

    // AIVehicle
    void OnGasBrake(float dT) override;
    void OnSteering(float dT) override;

  private:
    PidError *pBodyError;                                  // offset 0x754, size 0x4
    PidError *pHeadingError;                               // offset 0x758, size 0x4
    PidError *pVelocityError;                              // offset 0x75C, size 0x4
    AdaptivePIDControllerComplicated *pSteeringController; // offset 0x760, size 0x4
    AdaptivePIDControllerSimple *pThrottleBrakeController; // offset 0x764, size 0x4
    float mThrottleBrake;                                  // offset 0x768, size 0x4
    float mPrevDesiredSpeed;                               // offset 0x76C, size 0x4
};

class AIPerpVehicle : public AIVehiclePid, public IPerpetrator, public ICause, public ICheater {
  public:
    typedef AIVehiclePid Base;

    AIPerpVehicle(const BehaviorParams &bp);
    void ComputeSkill();

    // Virtual overrides
    // IUnknown
    ~AIPerpVehicle() override;

    // Overrides: IPerpetrator
    void SetRacerInfo(GRacerInfo *info) override;

    // Overrides: AIVehicle
    void Update(float dT) override;

    // Overrides: IPerpetrator
    void Set911CallTime(float time) override;

    // Overrides: Behavior
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    // Overrides: IPerpetrator
    bool IsPartiallyHidden(float &HowHidden) const override;

    // Overrides: IPerpetrator
    void SetCostToState(int cts) override;

    // Overrides: IPerpetrator
    int GetCostToState() const override;

    // Overrides: IPerpetrator
    void SetHeat(float heat) override;

    // Overrides: IVehicleAI
    float GetSkill() const override;

    // Overrides: ICheater
    float GetCatchupCheat() const override;

    // Overrides: IPerpetrator
    float GetHeat() const override;

    // Overrides: IPerpetrator
    void AddCostToState(int cost) override;

    // Overrides: IPerpetrator
    void AddToPendingRepPointsNormal(int amount) override;

    // Overrides: IPerpetrator
    void AddToPendingRepPointsFromCopDestruction(int amount) override;

    // Overrides: IPerpetrator
    bool IsRacing() const override;

    // Overrides: IVehicleAI
    float GetPercentRaceComplete() const override;

    // Overrides: IPerpetrator
    bool IsBeingPursued() const override;

    // Overrides: ICause
    void OnCausedExplosion(IExplosion *explosion, ISimable *to) override;

    // Overrides: AIVehicle
    bool OnClearCausality(float start_time) override;

    // Overrides: ICause
    void OnCausedCollision(const COLLISION_INFO &cinfo, ISimable *from, ISimable *to) override;

    // Overrides: IPerpetrator
    float GetLastTrafficHitTime() const override;

    // Overrides: IPerpetrator
    bool IsHiddenFromCars() const override {
        return this->mHiddenFromCars;
    }

    // Overrides: IPerpetrator
    bool IsHiddenFromHelicopters() const override {
        return this->mHiddenFromHelicopters;
    }

    // Overrides: IPerpetrator
    int GetPendingRepPointsNormal() const override {
        return this->mPendingRepPointsNormal;
    }

    // Overrides: IPerpetrator
    int GetPendingRepPointsFromCopDestruction() const override {
        return this->mPendingRepPointsFromCopDestruction;
    }

    // Overrides: IPerpetrator
    void ClearPendingRepPoints() override {
        this->mPendingRepPointsNormal = 0;
        this->mPendingRepPointsFromCopDestruction = 0;
    }

    // Overrides: IPerpetrator
    Attrib::Gen::pursuitescalation *GetPursuitEscalationAttrib() override {
        return this->mPursuitEscalationAttrib;
    }

    // Overrides: IPerpetrator
    Attrib::Gen::pursuitlevels *GetPursuitLevelAttrib() override {
        return this->mPursuitLevelAttrib;
    }

    // Overrides: IPerpetrator
    Attrib::Gen::pursuitsupport *GetPursuitSupportAttrib() override {
        return this->mPursuitSupportAttrib;
    }

    // Overrides: IPerpetrator
    GRacerInfo *GetRacerInfo() const override {
        return this->pRacerInfo;
    }

    // Overrides: IPerpetrator
    float GetShortcutSkill() const override {
        return bClamp(this->fBaseSkill + this->fGlueOutput, 0.0f, 1.0f);
    }

    // Overrides: IPerpetrator
    float Get911CallTime() const override {
        return this->m911CallTimer;
    }

    bool IsOnLegalRoad() {
        return this->GetCurrentRoad()->IsOnLegalRoad();
    }

  private:
    static float mStagger; // size: 0x4, address: 0x8041536C

    float mHeat;                                              // offset 0x78C, size 0x4
    int mCostToState;                                         // offset 0x790, size 0x4
    int mPendingRepPointsNormal;                              // offset 0x794, size 0x4
    int mPendingRepPointsFromCopDestruction;                  // offset 0x798, size 0x4
    bool mHiddenFromCars;                                     // offset 0x79C, size 0x1
    bool mHiddenFromHelicopters;                              // offset 0x7A0, size 0x1
    bool mWasInRaceEventLastHeatUpdate;                       // offset 0x7A4, size 0x1
    float mHiddenZoneTimer;                                   // offset 0x7A8, size 0x4
    float mHiddenZoneLatchTime;                               // offset 0x7AC, size 0x4
    bool mWasInZoneLastUpdate;                                // offset 0x7B0, size 0x1
    int mPursuitZoneCheck;                                    // offset 0x7B4, size 0x4
    Attrib::Gen::pursuitlevels *mPursuitLevelAttrib;          // offset 0x7B8, size 0x4
    Attrib::Gen::pursuitsupport *mPursuitSupportAttrib;       // offset 0x7BC, size 0x4
    Attrib::Gen::pursuitescalation *mPursuitEscalationAttrib; // offset 0x7C0, size 0x4
    GRacerInfo *pRacerInfo;                                   // offset 0x7C4, size 0x4
    float fBaseSkill;                                         // offset 0x7C8, size 0x4
    float fGlueSkill;                                         // offset 0x7CC, size 0x4
    PidError *pGlueError;                                     // offset 0x7D0, size 0x4
    float fGlueTimer;                                         // offset 0x7D4, size 0x4
    float fGlueOutput;                                        // offset 0x7D8, size 0x4
    float LastTrafficHitTime;                                 // offset 0x7DC, size 0x4
    float m911CallTimer;                                      // offset 0x7E0, size 0x4
};

// total size: 0x7EC
class AIVehicleRacecar : public AIPerpVehicle, public IRacer {
  public:
    AIVehicleRacecar(const BehaviorParams &bp);

    // Overrides: IUnknown
    ~AIVehicleRacecar() override;

    // Overrides: IRacer
    void StartRace(DriverStyle style) override;

    // Overrides: IRacer
    void QuitRace() override;

    // Overrides: IRacer
    void PrepareForRace(const RacePreparationInfo &rpi) override;

    static Behavior *Construct(const BehaviorParams &bp);

    bool ShouldDoSimplePhysics() const;

    // Overrides: AIVehicle
    void Update(float dT) override;
};

// total size: 0x80C
class AIVehicleHuman : public AIVehicleRacecar, public IHumanAI {
  public:
    AIVehicleHuman(const BehaviorParams &bp);

    static Behavior *Construct(const BehaviorParams &bp);

    // Overrides: IUnknown
    ~AIVehicleHuman() override;

    void UpdateWrongWay();

    // Overrides: IHumanAI
    void SetAiControl(bool ai_control) override;

    bool IsDragRacing();

    bool IsDragSteering();

    // Overrides: IHumanAI
    void ChangeDragLanes(bool left) override;

    // Overrides: AIVehicle
    void OnDebugDraw() override;

    // Overrides: AIVehicle
    void Update(float dT) override;

    // Overrides: IHumanAI
    bool IsPlayerSteering() override {
        if (this->bAiControl) {
            return false;
        } else {
            return this->IsDragSteering() == false;
        }
    }

    // Overrides: IHumanAI
    bool GetAiControl() override {
        return this->bAiControl;
    }

    // Overrides: IHumanAI
    void SetWorldMoment(const UMath::Vector3 &position, float radius) override {
        this->vMomentPosition = position;
        this->fMomentRadius = radius;
    }

    // Overrides: IHumanAI
    const UMath::Vector3 &GetWorldMomentPosition() override {
        return this->vMomentPosition;
    }

    // Overrides: IHumanAI
    float GetWorldMomentRadius() override {
        return this->fMomentRadius;
    }

    // Overrides: IHumanAI
    void ClearWorldMoment() override {
        this->fMomentRadius = 0.0f;
    }

    // Overrides: IVehicleAI
    float GetSkill() const override {
        return 1.0f;
    }

    // Overrides: IHumanAI
    bool IsFacingWrongWay() const override {
        return this->mWrongWay;
    }

    // Overrides: ICheater
    float GetCatchupCheat() const override {
        return 0.0f;
    }

  private:
    bool bAiControl;                // offset 0x7F4, size 0x1
    UMath::Vector3 vMomentPosition; // offset 0x7F8, size 0xC
    float fMomentRadius;            // offset 0x804, size 0x4
    bool mWrongWay;                 // offset 0x808, size 0x1
};

#endif
