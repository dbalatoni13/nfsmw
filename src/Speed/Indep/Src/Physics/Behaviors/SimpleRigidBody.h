#ifndef PHYSICS_BEHAVIORS_SIMPLERIGIDBODY_H
#define PHYSICS_BEHAVIORS_SIMPLERIGIDBODY_H

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Debug/Debugable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimpleBody.h"
#include "Speed/Indep/Src/Main/ScratchPtr.h"
#include "Speed/Indep/Src/Physics/Behavior.h"

#define SIMPLE_RIGID_BODY_MAX (96)

// total size: 0x28
struct RBSimpleParams : public Sim::Param {
    RBSimpleParams(const RBSimpleParams &_ctor_arg)
        : Sim::Param(_ctor_arg), finitPos(_ctor_arg.finitPos), finitVel(_ctor_arg.finitVel), finitAngVel(_ctor_arg.finitAngVel),
          finitMat(_ctor_arg.finitMat), finitRadius(_ctor_arg.finitRadius), finitMass(_ctor_arg.finitMass) {}

    RBSimpleParams(const UMath::Vector3 &initPos, const UMath::Vector3 &initVel, const UMath::Vector3 &initAngVel, const UMath::Matrix4 &initMat,
                   float initRadius, float initMass)
        : Sim::Param(TypeName(), static_cast<RBSimpleParams *>(nullptr)), finitPos(initPos), finitVel(initVel), finitAngVel(initAngVel),
          finitMat(initMat), finitRadius(initRadius), finitMass(initMass) {}

    static UCrc32 TypeName() {
        static UCrc32 value = UCrc32("RBSimpleParams");
        return value;
    }

    const UMath::Vector3 &finitPos;    // offset 0x10, size 0x4
    const UMath::Vector3 &finitVel;    // offset 0x14, size 0x4
    const UMath::Vector3 &finitAngVel; // offset 0x18, size 0x4
    const UMath::Matrix4 &finitMat;    // offset 0x1C, size 0x4
    float finitRadius;                 // offset 0x20, size 0x4
    float finitMass;                   // offset 0x24, size 0x4
};

// total size: 0x74
class SimpleRigidBody : public Behavior, public IRigidBody, public ISimpleBody, public bTNode<SimpleRigidBody>, public Debugable {
  public:
    // total size: 0x40
    struct Volatile {
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

    // Static functions
    static Behavior *Construct(const struct BehaviorParams &params);
    static void Update(const float dT, void *workspace);
    static IRigidBody *Get(unsigned int index);
    static unsigned int AssignSlot();

    // Methods
    SimpleRigidBody(const BehaviorParams &bp, const RBSimpleParams &params);
    float GetScalarVelocity() const;
    void ApplyFriction();
    void DoIntegration(const float dT);
    void RecalcOrientMat(UMath::Matrix4 &resultMat4) const;

    // Virtual methods
    virtual void OnDebugDraw();

    // Overrides
    // IRigidBody
    ~SimpleRigidBody() override;
    ISimable *GetOwner() const override;
    void GetForwardVector(UMath::Vector3 &vec) const override;
    void GetRightVector(UMath::Vector3 &vec) const override;
    void GetUpVector(UMath::Vector3 &vec) const override;
    void SetOrientation(const UMath::Matrix4 &orientMat) override;
    void SetOrientation(const UMath::Vector4 &orientation) override;
    void Accelerate(const UMath::Vector3 &a, float dT) override;
    void GetPointVelocity(const UMath::Vector3 &worldPt, UMath::Vector3 &pv) const override;
    void PlaceObject(const UMath::Matrix4 &orientMat, const UMath::Vector3 &initPos) override;
    void ConvertLocalToWorld(UMath::Vector3 &val, bool translate) const override;
    void ConvertWorldToLocal(UMath::Vector3 &val, bool translate) const override;
    void Resolve(const UMath::Vector3 &force, const UMath::Vector3 &torque) override;
    void ResolveForce(const UMath::Vector3 &force) override;
    void ResolveForce(const UMath::Vector3 &, const UMath::Vector3 &) override;
    void ResolveTorque(const UMath::Vector3 &torque) override;
    unsigned int GetTriggerFlags() const override;

    // Inline virtuals
    // Behavior
    void OnTaskSimulate(float dT) override {}

    void Reset() override {}

    // ISimpleBody
    void ModifyFlags(unsigned int uRemove, unsigned int uAdd) override {
        mData->flags &= ~uRemove;
        mData->flags |= uAdd;
    }

    bool CanCollideWithSRB() const override {
        return !(mData->flags & 1);
    }

    bool CanCollideWithRB() const override {
        return !(mData->flags & 2);
    }

    bool CanHitTrigger() const override {
        return mData->flags & 0x100;
    }

    const SimCollisionMap *GetCollisionMap() const override {
        return &mCollisionMap[mData->index];
    }

    SimCollisionMap *GetCollisionMap() override {
        return &mCollisionMap[mData->index];
    }

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

    void ModifyXPos(float offset) override {
        mData->position.x += offset;
    }

    void ModifyZPos(float offset) override {
        mData->position.z += offset;
    }

    void ModifyYPos(float offset) override {
        mData->position.y += offset;
    }

    void ResolveTorque(const UMath::Vector3 &f, const UMath::Vector3 &p) override {}

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

    bool HasHadCollision() const override {
        return false;
    }

    void Debug() override {}

    // Inlines
    static unsigned int GetCount() {
        return mCount;
    }

    bool CheckAnyFlags(unsigned int flagsToCheck) const {
        return mData->flags & flagsToCheck;
    }

  private:
    static SimCollisionMap mCollisionMap[SIMPLE_RIGID_BODY_MAX];
    static SimpleRigidBody *mMaps[SIMPLE_RIGID_BODY_MAX];
    static unsigned int mCount;

    ScratchPtr<Volatile> mData; // offset 0x70, size 0x4
};

typedef struct bTList<SimpleRigidBody> SimpleBodyList;
extern SimpleBodyList TheSimpleBodies;

#endif
