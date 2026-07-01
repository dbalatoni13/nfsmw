#ifndef RBVEHICLE_H
#define RBVEHICLE_H

#include "Speed/Indep/Src/Interfaces/Simables/IRBVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Physics/Behaviors/RigidBody.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

// total size: 0x1A4
class RBVehicle : public RigidBody, public IRBVehicle {
  public:
    ~RBVehicle() override;
    RBVehicle(const BehaviorParams &bp, const RBComplexParams &params);
    static Behavior *Construct(const BehaviorParams &params);

  public:
    // IRigidBody
    unsigned int GetTriggerFlags() const override;
    void PlaceObject(const UMath::Matrix4 &orientMat, const UMath::Vector3 &initPos) override;

    // ICollisionBody
    bool IsInGroundContact() const override;
    unsigned int GetNumContactPoints() const override;

    // IRBVehicle
    void SetCollisionMass(float mass) override {
        this->mCollisionMass = mass;
    }

    void SetCollisionCOG(const UMath::Vector3 &cog) override {
        this->mCollisionCOG = cog;
    }

    void SetPlayerReactions(const Attrib::Gen::collisionreactions &reactions) override {
        this->mPlayerReactions = reactions;
    }

    const Attrib::Gen::collisionreactions &GetPlayerReactions() const override {
        return this->mPlayerReactions;
    }

    void EnableObjectCollisions(bool enable) override {
        this->mObjectCollisionsEnabled = enable;
    }

  protected:
    // RigidBody
    void ModifyCollision(const RigidBody &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment) override;
    void ModifyCollision(const SimSurface &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment) override;
    bool ShouldSleep() const override;
    bool CanCollideWithWorld() const override;
    bool CanCollideWith(const RigidBody &other) const override;
    bool CanCollideWithGround() const override;
    void OnBeginFrame(const float dT) override;

    // IRBVehicle
    void SetInvulnerability(eInvulnerablitiy state, float time) override {
        this->mInvulnerableState = state;
        this->mInvulnerableTimer = time;
    }

    eInvulnerablitiy GetInvulnerability() const override {
        return this->mInvulnerableState;
    }

    // RigidBody
    bool DoPenetration(const RigidBody &other) override {
        if (this->mInvulnerableState != INVULNERABLE_NONE) {
            this->mLastPenetration = Sim::GetTime();
            return false;
        }
        return true;
    }

    // Behavior
    void OnTaskSimulate(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    IVehicle *GetVehicle() const {
        return this->mVehicle;
    }

  private:
    const CollisionReactionRecord &ChooseReaction(const Dynamics::Collision::Plane &plane) const;

    IVehicle *mVehicle;                                   // offset 0x148, size 0x4
    ISuspension *mSuspension;                             // offset 0x14C, size 0x4
    BehaviorSpecsPtr<Attrib::Gen::rigidbodyspecs> mSpecs; // offset 0x150, size 0x14
    float mDeadOnWheels;                                  // offset 0x164, size 0x4
    CollisionGeometry::Collection *mGeoms;                // offset 0x168, size 0x4
    unsigned int mFrame;                                  // offset 0x16C, size 0x4
    ALIGNVEC UMath::Vector3 mCollisionCOG;                // offset 0x170, size 0xC
    float mCollisionMass;                                 // offset 0x17C, size 0x4
    bool mObjectCollisionsEnabled;                        // offset 0x180, size 0x1
    eInvulnerablitiy mInvulnerableState;                  // offset 0x184, size 0x4
    Seconds mInvulnerableTimer;                           // offset 0x188, size 0x4
    Seconds mLastPenetration;                             // offset 0x18C, size 0x4
    Attrib::Gen::collisionreactions mPlayerReactions;     // offset 0x190, size 0x14
};

#endif
