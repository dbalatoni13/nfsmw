#ifndef AI_AIVEHICLE_H
#define AI_AIVEHICLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AIAvoidable.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/AI/AIAvoidable.h"
#include "Speed/Indep/Src/AI/AIMath.h"
#include "Speed/Indep/Src/Debug/Debugable.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"
#include "Speed/Indep/Src/World/WRoadNetwork.hpp"

enum TaskMode {
    TASK_FRAME_VARIABLE = 1,
    TASK_FRAME_FIXED = 0,
};

class AIVehicle : public VehicleBehavior, public IVehicleAI, public AIAvoidable, public Debugable {
    // total size: 0x754
    WRoadNav *mDriveToNav;                   // offset 0x70, size 0x4
    UMath::Vector3 mDirToNav;                // offset 0x74, size 0xC
    bool mDrivableToNav;                     // offset 0x80, size 0x1
    float mDriveSpeed;                       // offset 0x84, size 0x4
    UMath::Vector3 mDest;                    // offset 0x88, size 0xC
    float mReverseOverrideTimer;             // offset 0x94, size 0x4
    float mReverseOverrideSteer;             // offset 0x98, size 0x4
    bool mReverseOverrideDirection;          // offset 0x9C, size 0x1
    struct AITarget *mTarget;                // offset 0xA0, size 0x4
    short mDestSegment;                      // offset 0xA4, size 0x2
    bool mDrivableToTargetPos;               // offset 0xA8, size 0x1
    bool mReversingSpeed;                    // offset 0xAC, size 0x1
    bool mSteeringBehind;                    // offset 0xB0, size 0x1
    HSIMTASK mThinkTask;                     // offset 0xB4, size 0x4
    float mLastSpawnTime;                    // offset 0xB8, size 0x4
    bool mCanRespawn;                        // offset 0xBC, size 0x1
    struct AIGoal *mCurrentGoal;             // offset 0xC0, size 0x4
    struct IPursuit *mPursuit;               // offset 0xC4, size 0x4
    struct IRoadBlock *mRoadBlock;           // offset 0xC8, size 0x4
    UCrc32 mGoalName;                        // offset 0xCC, size 0x4
    float mAvoidableRadius;                  // offset 0xD0, size 0x4
    unsigned int mDriveFlags;                // offset 0xD4, size 0x4
    struct ICollisionBody *mCollisionBody;   // offset 0xD8, size 0x4
    struct ITransmission *mITransmission;    // offset 0xDC, size 0x4
    struct ISuspension *mISuspension;        // offset 0xE0, size 0x4
    struct IEngine *mIEngine;                // offset 0xE4, size 0x4
    struct IInput *mIInput;                  // offset 0xE8, size 0x4
    WRoadNav *mCollNav;                      // offset 0xEC, size 0x4
    struct aivehicle *mAttributes;           // offset 0xF0, size 0x4
    AI::Math::FloatSpring mDampedAngularVel; // offset 0xF4, size 0x10
    AI::Math::FloatSpring mDampedAngle;      // offset 0x104, size 0x10
    float mRoadUpdateTimer;                  // offset 0x114, size 0x4
    float mRoadIncrementTimer;               // offset 0x118, size 0x4
    float mSeekAheadTimer;                   // offset 0x11C, size 0x4
    UMath::Vector3 mSeekAheadPosition;       // offset 0x120, size 0xC
    UMath::Vector3 mFarFutureDirection;      // offset 0x12C, size 0xC
    UMath::Vector3 mFarFuturePosition;       // offset 0x138, size 0xC
    short mLastFutureSegment;                // offset 0x144, size 0x2
    short mLastFutureNodeInd;                // offset 0x146, size 0x2
    WRoadNav mCurrentRoad;                   // offset 0x148, size 0x2F0
    WRoadNav mFutureRoad;                    // offset 0x438, size 0x2F0
    float mAccelData[10];                    // offset 0x728, size 0x28
    float mTopSpeed;                         // offset 0x750, size 0x4

  public:
    virtual void OnTaskSimulate(float);

    virtual ISimable *GetSimable() const;

    virtual IVehicle *GetVehicle() const;

    virtual void Update(float dT);

    virtual const UTL::Std::list<AIAvoidable *, _type_AIAvoidableNeighbors> &GetAvoidableList();

    virtual bool OnUpdateAvoidable(UMath::Vector3 &pos, float &sweep);

    virtual void SetAvoidableRadius(float radius);

    virtual const AISplinePath *GetSplinePath();

    virtual void ResetDriveToNav(eLaneSelection lane_selection);

    virtual bool ResetVehicleToRoadNav(short segInd, char laneInd, float timeStep);

    virtual bool ResetVehicleToRoadNav(WRoadNav *other_nav);

    virtual bool ResetVehicleToRoadPos(const UMath::Vector3 &position, const UMath::Vector3 &forwardVector);

    virtual WRoadNav *GetDriveToNav();

    virtual bool GetDrivableToDriveToNav();

    virtual float GetDriveSpeed() const;

    virtual void SetDriveSpeed(float driveSpeed);

    virtual const UMath::Vector3 &GetDriveTarget() const;

    virtual void SetDriveTarget(const UMath::Vector3 &dest);

    virtual float GetLookAhead();

    float GetOverSteerCorrection(float steer);

    const UMath::Vector3 &GetAngularVelocity() const;

    const UMath::Vector3 &GetLinearVelocity() const;

    const UMath::Vector3 &GetPosition() const;

    const UMath::Matrix4 &GetOrientation() const;

    const UMath::Vector3 &GetForwardVector() const;

    const UMath::Vector3 &GetUpVector() const;

    const UMath::Vector3 &GetRightVector() const;

    virtual unsigned int GetDriveFlags() const;
    virtual void ClearDriveFlags();
    virtual void DoReverse();
    virtual void DoSteering();
    virtual void DoGasBrake();
    virtual void DoDriving(unsigned int flags);
    virtual float GetPathDistanceRemaining();
    virtual void SetReverseOverride(float time);
    virtual bool GetReverseOverride();
    void ClearReverseOverride();
    virtual AITarget *GetTarget() const;
    short GetDestSegment() const;
    virtual bool GetDrivableToTargetPos() const;
    virtual void SetSpawned();
    virtual void UnSpawn();
    virtual bool CanRespawn(bool respawnAvailable);
    virtual float GetLastSpawnTime();
    virtual const Attrib::Gen::aivehicle &GetAttributes() const;
    virtual float GetTopSpeed() const;
    // TODO
    // virtual void OnCollision(const Sim::Collision::Info &cinfo);
    virtual void OnCollision();
    virtual void OnDebugDraw();
    virtual bool IsCurrentGoal(const UCrc32 &name);
    virtual UCrc32 &GetGoalName();
    virtual void SetGoal(const UCrc32 &name);
    virtual bool IsCurrentAction(const UCrc32 &name);
    virtual UCrc32 GetActionName();
    virtual float GetSkill() const;
    virtual float GetShortcutSkill() const;
    virtual float GetPercentRaceComplete() const;
    IEngine *GetEngine() const;
    ITransmission *GetTransmission() const;
    // IChassis *GetSuspension() const;

  protected:
    virtual bool OnClearCausality(float start_time);
    virtual void OnDriving(float dT);
    virtual void OnOwnerAttached(IAttachable *pOther);
    virtual void OnOwnerDetached(IAttachable *pOther);
    virtual bool OnTask(HSIMTASK hTask, float dT);

    virtual const int GetPriority() const {}

    AIVehicle(const BehaviorParams &bp, float update_rate, float stagger, TaskMode taskmode);
    ~AIVehicle();

    virtual void Reset();
    virtual void ResetInternals();
    void ClearGoal();
    void UpdateSpawnTimer(float dT);
    void UpdateRoadNavInfo();
    void UpdateReverseOverride(float dT);
    void UpdateTargeting();
    bool WorldCollision(const UMath::Vector3 &pos, const UMath::Vector3 &dest);
    bool BarriersInPath(bool reverse);
    virtual bool IsTetheredToTarget(UTL::COM::IUnknown *object);
    void UpdateSimplePhysics(float dT);
    virtual bool IsSimplePhysicsActive();
    virtual void EnableSimplePhysics();
    virtual void DisableSimplePhysics();
    virtual void OnBehaviorChange(const UCrc32 &mechanic);
};

#endif
