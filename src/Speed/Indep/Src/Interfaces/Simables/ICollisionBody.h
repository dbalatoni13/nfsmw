#ifndef INTERFACES_SIMABLES_ICOLLISIONBODY_H
#define INTERFACES_SIMABLES_ICOLLISIONBODY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

class ICollisionBody : public UTL::COM::IUnknown, public UTL::Collections::Listable<ICollisionBody, 160> {
  public:
    ICollisionBody(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    virtual ~ICollisionBody() {}

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  public:
    virtual const UMath::Vector3 &GetPosition() const = 0;
    virtual const UMath::Vector3 &GetLinearVelocity() const = 0;
    virtual const UMath::Vector3 &GetAngularVelocity() const = 0;
    virtual UMath::Vector3 GetDimension() const = 0;
    virtual void Damp(float amount) = 0;
    virtual void SetAnimating(bool animating) = 0;
    virtual bool IsModeling() const = 0;
    virtual void DisableModeling() = 0;
    virtual void EnableModeling() = 0;
    virtual bool IsTriggering() const = 0;
    virtual void DisableTriggering() = 0;
    virtual void EnableTriggering() = 0;
    virtual bool IsSleeping() const = 0;
    virtual void SetCenterOfGravity(const UMath::Vector3 &cg) = 0;
    virtual const UMath::Vector3 &GetCenterOfGravity() const = 0;
    virtual bool HasHadCollision() const = 0;
    virtual bool HasHadWorldCollision() const = 0;
    virtual bool HasHadObjectCollision() const = 0;
    virtual void EnableCollisionGeometries(UCrc32 name, bool enable) = 0;
    virtual bool DistributeMass() = 0;
    virtual const UMath::Vector3 &GetWorldMomentScale() const = 0;
    virtual const UMath::Vector3 &GetGroundMomentScale() const = 0;
    virtual bool IsAttachedToWorld() const = 0;
    virtual void AttachedToWorld(bool b, float detachforce) = 0;
    virtual bool IsAnchored() const = 0;
    virtual void SetAnchored(bool b) = 0;
    virtual void SetInertiaTensor(const UMath::Vector3 &moment) = 0;
    virtual const UMath::Vector3 &GetInertiaTensor() const = 0;
    virtual float GetOrientToGround() const = 0;
    virtual bool IsInGroundContact() const = 0;
    virtual unsigned int GetNumContactPoints() const = 0;
    virtual const UMath::Vector4 &GetGroundNormal() const = 0;
    virtual void SetForce(const UMath::Vector3 &v) = 0;
    virtual void SetTorque(const UMath::Vector3 &v) = 0;
    virtual const UMath::Vector3 &GetForce() const = 0;
    virtual const UMath::Vector3 &GetTorque() const = 0;
    virtual float GetGravity() const = 0;
    virtual const UMath::Vector3 &GetForwardVector() const = 0;
    virtual const UMath::Vector3 &GetRightVector() const = 0;
    virtual const UMath::Vector3 &GetUpVector() const = 0;
    virtual const UMath::Matrix4 &GetMatrix4() const = 0;
};

#endif
