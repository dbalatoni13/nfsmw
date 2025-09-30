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
    virtual const UMath::Vector3 &GetPosition() const;
    virtual const UMath::Vector3 &GetLinearVelocity() const;
    virtual const UMath::Vector3 &GetAngularVelocity() const;
    virtual UMath::Vector3 GetDimension() const;
    virtual void Damp(float amount);
    virtual void SetAnimating(bool animating);
    virtual bool IsModeling() const;
    virtual void DisableModeling();
    virtual void EnableModeling();
    virtual bool IsTriggering() const;
    virtual void DisableTriggering();
    virtual void EnableTriggering();
    virtual bool IsSleeping() const;
    virtual void SetCenterOfGravity(const UMath::Vector3 &cg);
    virtual const UMath::Vector3 &GetCenterOfGravity() const;
    virtual bool HasHadCollision() const;
    virtual bool HasHadWorldCollision() const;
    virtual bool HasHadObjectCollision() const;
    virtual void EnableCollisionGeometries(UCrc32 name, bool enable);
    virtual bool DistributeMass();
    virtual const UMath::Vector3 &GetWorldMomentScale() const;
    virtual const UMath::Vector3 &GetGroundMomentScale() const;
    virtual bool IsAttachedToWorld() const;
    virtual void AttachedToWorld(bool b, float detachforce);
    virtual bool IsAnchored() const;
    virtual void SetAnchored(bool b);
    virtual void SetInertiaTensor(const UMath::Vector3 &moment);
    virtual const UMath::Vector3 &GetInertiaTensor() const;
    virtual float GetOrientToGround() const;
    virtual bool IsInGroundContact() const;
    virtual unsigned int GetNumContactPoints() const;
    virtual const UMath::Vector4 &GetGroundNormal() const;
    virtual void SetForce(const UMath::Vector3 &v);
    virtual void SetTorque(const UMath::Vector3 &v);
    virtual const UMath::Vector3 &GetForce() const;
    virtual const UMath::Vector3 &GetTorque() const;
    virtual float GetGravity() const;
    virtual const UMath::Vector3 &GetForwardVector() const;
    virtual const UMath::Vector3 &GetRightVector() const;
    virtual const UMath::Vector3 &GetUpVector() const;
    virtual const UMath::Matrix4 &GetMatrix4() const;
};

#endif
