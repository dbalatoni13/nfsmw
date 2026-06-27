#include "Speed/Indep/Src/Sim/SimTypes.h"
#ifndef __SIMPLERIGIDBODY_H
#define __SIMPLERIGIDBODY_H 1

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"
#include "Speed/Indep/Src/Debug/Debugable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimpleBody.h"
#include "Speed/Indep/Src/Main/ScratchPtr.h"
#include "Speed/Indep/Src/Physics/Behavior.h"

// total size: 0x74
class SimpleRigidBody : public Behavior, public IRigidBody, public ISimpleBody, public bTNode<SimpleRigidBody>, public Debugable {
  public:
    // total size: 0x40
    struct Volatile {
        enum { MaxInstances = Sim::MaxSimpleBodies };
        enum {
            CAN_COLLIDE_WITH_SRB = 1,
            CAN_COLLIDE_WITH_RB = 1 << 1,
            CAN_HIT_TRIGGER = 1 << 8,
        };
        Volatile();

        UMath::Vector4 orientation; // offset 0x0, size 0x10
        UMath::Vector3 position;    // offset 0x10, size 0xC
        unsigned char unused;       // offset 0x1C, size 0x1
        unsigned char index;        // offset 0x1D, size 0x1
        unsigned short flags;       // offset 0x1E, size 0x2
        UMath::Vector3 linearVel;   // offset 0x20, size 0xC
        float radius;               // offset 0x2C, size 0x4
        UMath::Vector3 angularVel;  // offset 0x30, size 0xC
        float mass;                 // offset 0x3C, size 0x4
    };

    static Behavior *Construct(const struct BehaviorParams &params);
    static IRigidBody *Get(unsigned int index);

    static unsigned int GetCount() {
        return mCount;
    }

    static unsigned int AssignSlot();
    static void Update(const float dT, void *workspace);

    void SetCanHitTrigger(bool canHit);

    void SetFlags(unsigned int flagsToSet) {}

    void ClearFlags(unsigned int flagsToClear) {}

    bool CheckAllFlags(unsigned int flagsToCheck) const {}

    bool CheckAnyFlags(unsigned int flagsToCheck) const {
        return (mData->flags & flagsToCheck) != 0;
    }

    ISimable *GetOwner() const override;
    void RecalcOrientMat(UMath::Matrix4 &resultMat4) const;
    float GetScalarVelocity() const;

    // Behavior
    void OnTaskSimulate(float dT) override {}

    void Reset() override {}

    // ISimpleBody
    void ModifyFlags(unsigned int uRemove, unsigned int uAdd) override {
        mData->flags &= ~uRemove;
        mData->flags |= uAdd;
    }

    bool CanCollideWithSRB() const override {
        return (mData->flags & Volatile::CAN_COLLIDE_WITH_SRB) == 0;
    }

    bool CanCollideWithRB() const override {
        return (mData->flags & Volatile::CAN_COLLIDE_WITH_RB) == 0;
    }

    bool CanHitTrigger() const override {
        return (mData->flags & Volatile::CAN_HIT_TRIGGER) != 0;
    }

    const SimCollisionMap *GetCollisionMap() const override {
        return &mCollisionMap[mData->index];
    }

    SimCollisionMap *GetCollisionMap() override {
        return &mCollisionMap[mData->index];
    }

    virtual void OnDebugDraw();

    // IRigidBody
    bool IsSimple() const override {
        return true;
    }

    SimableType GetSimableType() const override {
        return GetOwner()->GetSimableType();
    }

    int GetIndex() const override {
        return mData->index;
    }

    float GetRadius() const override {
        return mData->radius;
    }

    float GetMass() const override {
        return mData->mass;
    }

    float GetOOMass() const override {
        return 1.0f / mData->mass;
    }

    const UMath::Vector3 &GetPosition() const override {
        return mData->position;
    }

    const UMath::Vector3 &GetLinearVelocity() const override {
        return mData->linearVel;
    }

    const UMath::Vector3 &GetAngularVelocity() const override {
        return mData->angularVel;
    }

    float GetSpeed() const override {
        return UMath::Length(GetLinearVelocity());
    }

    float GetSpeedXZ() const override {
        return UMath::Lengthxz(GetLinearVelocity());
    }

    const WCollider *GetWCollider() const override {
        return nullptr;
    }

    void SetPosition(const UMath::Vector3 &pos) override {
        mData->position = pos;
    }

    void SetLinearVelocity(const UMath::Vector3 &vel) override {
        mData->linearVel = vel;
    }

    void SetAngularVelocity(const UMath::Vector3 &vel) override {
        mData->angularVel = vel;
    }

    void SetRadius(float radius) override {
        mData->radius = radius;
    }

    void SetMass(float mass) override {
        mData->mass = mass;
    }

    void SetOrientation(const UMath::Matrix4 &orientMat) override;
    void SetOrientation(const UMath::Vector4 &orientation) override;

    void ModifyXPos(float offset) override {
        mData->position.x += offset;
    }

    void ModifyYPos(float offset) override {
        mData->position.y += offset;
    }

    void ModifyZPos(float offset) override {
        mData->position.z += offset;
    }

    void Resolve(const UMath::Vector3 &force, const UMath::Vector3 &torque) override;
    void ResolveForce(const UMath::Vector3 &force) override;
    void ResolveTorque(const UMath::Vector3 &torque) override;
    void ResolveTorque(const UMath::Vector3 &f, const UMath::Vector3 &p) override {}
    void ResolveForce(const UMath::Vector3 &, const UMath::Vector3 &) override;

    void GetMatrix4(UMath::Matrix4 &mat) const override {
        RecalcOrientMat(mat);
    }

    const UMath::Vector4 &GetOrientation() const override {
        return mData->orientation;
    }

    void GetDimension(UMath::Vector3 &dim) const override {
        dim.x = dim.y = dim.z = mData->radius;
    }

    UMath::Vector3 GetDimension() const override {
        UMath::Vector3 dim;
        dim.x = dim.y = dim.z = mData->radius;
        return dim;
    }

    virtual bool HasHadCollision() const {
        return false;
    }

    void PlaceObject(const UMath::Matrix4 &orientMat, const UMath::Vector3 &initPos) override;
    void Accelerate(const UMath::Vector3 &a, float dT) override;
    void GetPointVelocity(const UMath::Vector3 &worldPt, UMath::Vector3 &pv) const override;
    void ConvertLocalToWorld(UMath::Vector3 &val, bool translate) const override;
    void ConvertWorldToLocal(UMath::Vector3 &val, bool translate) const override;

    void Debug() override {}

    // TODO where?
    void GetForwardVector(UMath::Vector3 &vec) const override;
    void GetRightVector(UMath::Vector3 &vec) const override;
    void GetUpVector(UMath::Vector3 &vec) const override;
    unsigned int GetTriggerFlags() const override;

    ~SimpleRigidBody() override; // TODO protected?

  protected:
    void ApplyFriction();
    SimpleRigidBody(const BehaviorParams &bp, const RBSimpleParams &params);

  private:
    void DoIntegration(const float dT);
    void DoRBCollisions(const float dT);
    void DoSRBCollisions(SimpleRigidBody *other);

    static SimCollisionMap mCollisionMap[Sim::MaxSimpleBodies];
    static SimpleRigidBody *mMaps[Sim::MaxSimpleBodies];
    static unsigned int mCount;

    ScratchPtr<Volatile> mData; // offset 0x70, size 0x4
};

#endif
