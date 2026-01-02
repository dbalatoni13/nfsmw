#ifndef PHYSICS_BEHAVIORS_RBVEHICLE_H
#define PHYSICS_BEHAVIORS_RBVEHICLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/IRBVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Physics/Behaviors/RigidBody.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

// total size: 0x1A4
class RBVehicle : public RigidBody, public IRBVehicle {
  public:
    static Behavior *Construct(const BehaviorParams &params);

    RBVehicle(const BehaviorParams &bp, const RBComplexParams &params);

  private:
    const CollisionReactionRecord &ChooseReaction(const Dynamics::Collision::Plane &plane) const;

  public:
    // Overrides
    ~RBVehicle() override;

    // ICollisionBody
    unsigned int GetNumContactPoints() const override;
    bool IsInGroundContact() const override;

    // RigidBody
    bool CanCollideWith(const RigidBody &other) const override;
    void OnBeginFrame(float dT) override;
    bool ShouldSleep() const override;
    void ModifyCollision(const RigidBody &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment) override;
    void ModifyCollision(const SimSurface &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment) override;
    bool CanCollideWithGround() const override;
    bool CanCollideWithWorld() const override;

    // Behavior
    void OnTaskSimulate(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    // IRigidBody
    void PlaceObject(const UMath::Matrix4 &orientMat, const UMath::Vector3 &initPos) override;
    unsigned int GetTriggerFlags() const override;

    // Inline virtuals
    // IRBVehicle
    void SetCollisionMass(float mass) override {
        mCollisionMass = mass;
    }

    void SetCollisionCOG(const UMath::Vector3 &cog) override {
        mCollisionCOG = cog;
    }

    void SetPlayerReactions(const Attrib::Gen::collisionreactions &reactions) override {
        mPlayerReactions = reactions;
    }

    const Attrib::Gen::collisionreactions &GetPlayerReactions() const override {
        return mPlayerReactions;
    }

    void EnableObjectCollisions(bool enable) override {
        mObjectCollisionsEnabled = enable;
    }

    void SetInvulnerability(eInvulnerablitiy state, float time) override {
        mInvulnerableTimer = time;
        mInvulnerableState = state;
    }

    eInvulnerablitiy GetInvulnerability() const override {
        return mInvulnerableState;
    }

    // RigidBody
    bool DoPenetration(const RigidBody &other) override {
        if (mInvulnerableState != INVULNERABLE_NONE) {
            mLastPenetration = Sim::GetTime();
            return false;
        }
        return true;
    }

  protected:
    IVehicle *GetVehicle() const {
        return mVehicle;
    }

  private:
    IVehicle *mVehicle;                                         // offset 0x148, size 0x4
    ISuspension *mSuspension;                                   // offset 0x14C, size 0x4
    BehaviorSpecsPtr<Attrib::Gen::rigidbodyspecs> mSpecs;       // offset 0x150, size 0x14
    float mDeadOnWheels;                                        // offset 0x164, size 0x4
    CollisionGeometry::Collection *mGeoms;                      // offset 0x168, size 0x4
    unsigned int mFrame;                                        // offset 0x16C, size 0x4
    ALIGN_PS2(16) ALIGN_XENON(16) UMath::Vector3 mCollisionCOG; // offset 0x170, size 0xC
    float mCollisionMass;                                       // offset 0x17C, size 0x4
    bool mObjectCollisionsEnabled;                              // offset 0x180, size 0x1
    eInvulnerablitiy mInvulnerableState;                        // offset 0x184, size 0x4
    float mInvulnerableTimer;                                   // offset 0x188, size 0x4
    float mLastPenetration;                                     // offset 0x18C, size 0x4
    Attrib::Gen::collisionreactions mPlayerReactions;           // offset 0x190, size 0x14
};

#endif
