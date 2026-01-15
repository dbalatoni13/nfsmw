#ifndef AI_AIVEHICLE_H
#define AI_AIVEHICLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AIAvoidable.h"
#include "AIGoal.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Src/AI/AIAvoidable.h"
#include "Speed/Indep/Src/AI/AIMath.h"
#include "Speed/Indep/Src/Debug/Debugable.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"
#include "Speed/Indep/Src/Sim/Collision.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"

// total size: 0x754
class AIVehicle : public VehicleBehavior, public IVehicleAI, public AIAvoidable, public Debugable {
  public:
    static Behavior *Construct(const BehaviorParams &bp);

    float GetOverSteerCorrection(float steer);
    const UMath::Vector3 &GetAngularVelocity() const;
    const UMath::Vector3 &GetLinearVelocity() const;
    const UMath::Vector3 &GetPosition() const;
    const UMath::Matrix4 &GetOrientation() const;
    const UMath::Vector3 &GetForwardVector() const;
    const UMath::Vector3 &GetUpVector() const;
    const UMath::Vector3 &GetRightVector() const;

    void *operator new(std::size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    void operator delete(void *mem, std::size_t size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr);
        }
    }

    // Behavior
    void OnTaskSimulate(float) override;

    // IVehicleAI
    ISimable *GetSimable() const override {
        return Behavior::GetOwner();
    }

    IVehicle *GetVehicle() const override {
        return VehicleBehavior::GetVehicle();
    }

    virtual void Update(float dT);

    // IVehicleAI
    const AvoidableList &GetAvoidableList() override;

    // AIAvoidable
    bool OnUpdateAvoidable(UMath::Vector3 &pos, float &sweep) override;

    // IVehicleAI
    void SetAvoidableRadius(float radius) override;
    const AISplinePath *GetSplinePath() override;
    void ResetDriveToNav(eLaneSelection lane_selection) override;
    bool ResetVehicleToRoadNav(short segInd, char laneInd, float timeStep) override;
    bool ResetVehicleToRoadNav(WRoadNav *other_nav) override;
    bool ResetVehicleToRoadPos(const UMath::Vector3 &position, const UMath::Vector3 &forwardVector) override;
    WRoadNav *GetDriveToNav() override;
    bool GetDrivableToDriveToNav() override;
    float GetDriveSpeed() const override;
    void SetDriveSpeed(float driveSpeed) override;

    const UMath::Vector3 &GetDriveTarget() const override {
        return mDest;
    }

    void SetDriveTarget(const UMath::Vector3 &dest) override {
        mDest = dest;
    }

    float GetLookAhead() override;

    unsigned int GetDriveFlags() const override {
        return mDriveFlags;
    }

    void ClearDriveFlags() override {}

    void DoReverse() override {}

    void DoSteering() override {}

    void DoGasBrake() override {}

    void DoDriving(unsigned int flags) override {}

    float GetPathDistanceRemaining() override;

    void SetReverseOverride(float time) override {}

    bool GetReverseOverride() override {
        return mReverseOverrideTimer > 0.0f;
    }

    void ClearReverseOverride();

    AITarget *GetTarget() const override {
        return mTarget;
    }

    // short GetDestSegment() const {}

    bool GetDrivableToTargetPos() const override {
        return mDrivableToTargetPos;
    }

    void SetSpawned() override;
    void UnSpawn() override;
    bool CanRespawn(bool respawnAvailable) override;

    float GetLastSpawnTime() override {
        return mLastSpawnTime;
    }

    const Attrib::Gen::aivehicle &GetAttributes() const override {
        return *mAttributes;
    }

    float GetTopSpeed() const override {
        return mTopSpeed;
    }

    virtual void OnCollision(const COLLISION_INFO &cinfo);
    virtual void OnDebugDraw();

    // IVehicleAI
    bool IsCurrentGoal(const UCrc32 &name) override {
        return name == mGoalName;
    }

    UCrc32 &GetGoalName() override {
        return mGoalName;
    }

    void SetGoal(const UCrc32 &name);

    // IVehicleAI
    // bool IsCurrentAction(const UCrc32 &name) override {}

    // UCrc32 GetActionName() override {}

    // float GetSkill() const override {}

    // float GetShortcutSkill() const override {}

    // float GetPercentRaceComplete() const override {}

    // struct IEngine *GetEngine() const {}

    // ITransmission *GetTransmission() const {}

    // IChassis *GetSuspension() const;

  protected:
    virtual bool OnClearCausality(float start_time);
    virtual void OnDriving(float dT);

    // Behavior
    virtual void OnOwnerAttached(IAttachable *pOther);
    virtual void OnOwnerDetached(IAttachable *pOther);
    virtual bool OnTask(HSIMTASK hTask, float dT);

    const int GetPriority() const override {}

    AIVehicle(const BehaviorParams &bp, float update_rate, float stagger, Sim::TaskMode taskmode);
    ~AIVehicle();

    // Behavior
    void Reset() override {}

    virtual void ResetInternals();
    void ClearGoal();

    void UpdateSpawnTimer(float dT) {}

    void UpdateRoadNavInfo();
    void UpdateReverseOverride(float dT);
    void UpdateTargeting();
    bool WorldCollision(const UMath::Vector3 &pos, const UMath::Vector3 &dest);
    bool BarriersInPath(bool reverse);

    // virtual bool IsTetheredToTarget(UTL::COM::IUnknown *object) {}

    void UpdateSimplePhysics(float dT);
    virtual bool IsSimplePhysicsActive();

    // IVehicleAI
    void EnableSimplePhysics() override;
    void DisableSimplePhysics() override;

    // Behavior
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    WRoadNav *mDriveToNav;          // offset 0x70, size 0x4
    UMath::Vector3 mDirToNav;       // offset 0x74, size 0xC
    bool mDrivableToNav;            // offset 0x80, size 0x1
    float mDriveSpeed;              // offset 0x84, size 0x4
    UMath::Vector3 mDest;           // offset 0x88, size 0xC
    float mReverseOverrideTimer;    // offset 0x94, size 0x4
    float mReverseOverrideSteer;    // offset 0x98, size 0x4
    bool mReverseOverrideDirection; // offset 0x9C, size 0x1
    AITarget *mTarget;              // offset 0xA0, size 0x4
    short mDestSegment;             // offset 0xA4, size 0x2
    bool mDrivableToTargetPos;      // offset 0xA8, size 0x1
    bool mReversingSpeed;           // offset 0xAC, size 0x1
    bool mSteeringBehind;           // offset 0xB0, size 0x1

  private:
    HSIMTASK mThinkTask;                     // offset 0xB4, size 0x4
    float mLastSpawnTime;                    // offset 0xB8, size 0x4
    bool mCanRespawn;                        // offset 0xBC, size 0x1
    AIGoal *mCurrentGoal;                    // offset 0xC0, size 0x4
    IPursuit *mPursuit;                      // offset 0xC4, size 0x4
    IRoadBlock *mRoadBlock;                  // offset 0xC8, size 0x4
    UCrc32 mGoalName;                        // offset 0xCC, size 0x4
    float mAvoidableRadius;                  // offset 0xD0, size 0x4
    unsigned int mDriveFlags;                // offset 0xD4, size 0x4
    ICollisionBody *mCollisionBody;          // offset 0xD8, size 0x4
    ITransmission *mITransmission;           // offset 0xDC, size 0x4
    ISuspension *mISuspension;               // offset 0xE0, size 0x4
    struct IEngine *mIEngine;                // offset 0xE4, size 0x4
    IInput *mIInput;                         // offset 0xE8, size 0x4
    WRoadNav *mCollNav;                      // offset 0xEC, size 0x4
    Attrib::Gen::aivehicle *mAttributes;     // offset 0xF0, size 0x4
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
    Mps mTopSpeed;                           // offset 0x750, size 0x4
};

#endif
