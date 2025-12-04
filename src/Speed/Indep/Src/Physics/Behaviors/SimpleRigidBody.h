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
    override virtual ~SimpleRigidBody();
    override virtual ISimable *GetOwner() const;
    override virtual void GetForwardVector(UMath::Vector3 &vec) const;
    override virtual void GetRightVector(UMath::Vector3 &vec) const;
    override virtual void GetUpVector(UMath::Vector3 &vec) const;
    override virtual void SetOrientation(const UMath::Matrix4 &orientMat);
    override virtual void SetOrientation(const UMath::Vector4 &orientation);
    override virtual void Accelerate(const UMath::Vector3 &a, float dT);
    override virtual void GetPointVelocity(const UMath::Vector3 &worldPt, UMath::Vector3 &pv) const;
    override virtual void PlaceObject(const UMath::Matrix4 &orientMat, const UMath::Vector3 &initPos);
    override virtual void ConvertLocalToWorld(UMath::Vector3 &val, bool translate) const;
    override virtual void ConvertWorldToLocal(UMath::Vector3 &val, bool translate) const;
    override virtual void Resolve(const UMath::Vector3 &force, const UMath::Vector3 &torque);
    override virtual void ResolveForce(const UMath::Vector3 &force);
    override virtual void ResolveForce(const UMath::Vector3 &, const UMath::Vector3 &);
    override virtual void ResolveTorque(const UMath::Vector3 &torque);
    override virtual unsigned int GetTriggerFlags() const;

    // Inline virtuals
    // Behavior
    override virtual void OnTaskSimulate(float dT) {}

    override virtual void Reset() {}

    // ISimpleBody
    override virtual void ModifyFlags(unsigned int uRemove, unsigned int uAdd) {
        mData->flags &= ~uRemove;
        mData->flags |= uAdd;
    }

    override virtual bool CanCollideWithSRB() const {
        return !(mData->flags & 1);
    }

    override virtual bool CanCollideWithRB() const {
        return !(mData->flags & 2);
    }

    override virtual bool CanHitTrigger() const {
        return mData->flags & 0x100;
    }

    override virtual const SimCollisionMap *GetCollisionMap() const {
        return &mCollisionMap[mData->index];
    }

    override virtual SimCollisionMap *GetCollisionMap() {
        return &mCollisionMap[mData->index];
    }

    // IRigidBody
    override virtual bool IsSimple() const {
        return true;
    }

    override virtual SimableType GetSimableType() const {
        return GetOwner()->GetSimableType();
    }

    override virtual int GetIndex() const {
        return mData->index;
    }

    override virtual float GetRadius() const {
        return mData->radius;
    }

    override virtual float GetMass() const {
        return mData->mass;
    }

    override virtual float GetOOMass() const {
        return 1.0f / mData->mass;
    }

    override virtual const UMath::Vector3 &GetPosition() const {
        return mData->position;
    }

    override virtual const UMath::Vector3 &GetLinearVelocity() const {
        return mData->linearVel;
    }

    override virtual const UMath::Vector3 &GetAngularVelocity() const {
        return mData->angularVel;
    }

    override virtual float GetSpeed() const {
        return UMath::Length(GetLinearVelocity());
    }

    override virtual float GetSpeedXZ() const {
        return UMath::Lengthxz(GetLinearVelocity());
    }

    override virtual const WCollider *GetWCollider() const {
        return nullptr;
    }

    override virtual void SetPosition(const UMath::Vector3 &pos) {
        mData->position = pos;
    }

    override virtual void SetLinearVelocity(const UMath::Vector3 &vel) {
        mData->linearVel = vel;
    }

    override virtual void SetAngularVelocity(const UMath::Vector3 &vel) {
        mData->angularVel = vel;
    }

    override virtual void SetRadius(float radius) {
        mData->radius = radius;
    }

    override virtual void SetMass(float mass) {
        mData->mass = mass;
    }

    override virtual void ModifyXPos(float offset) {
        mData->position.x += offset;
    }

    override virtual void ModifyZPos(float offset) {
        mData->position.z += offset;
    }

    override virtual void ModifyYPos(float offset) {
        mData->position.y += offset;
    }

    override virtual void ResolveTorque(const UMath::Vector3 &f, const UMath::Vector3 &p) {}

    override virtual void GetMatrix4(UMath::Matrix4 &mat) const {
        RecalcOrientMat(mat);
    }

    override virtual const UMath::Vector4 &GetOrientation() const {
        return mData->orientation;
    }

    override virtual void GetDimension(UMath::Vector3 &dim) const {
        dim.x = dim.y = dim.z = mData->radius;
    }

    override virtual UMath::Vector3 GetDimension() const {
        UMath::Vector3 dim;
        dim.x = dim.y = dim.z = mData->radius;
        return dim;
    }

    override virtual bool HasHadCollision() const {
        return false;
    }

    override virtual void Debug() {}

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
