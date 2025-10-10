#ifndef INTERFACES_SIMABLES_IRIGIDBODY_H
#define INTERFACES_SIMABLES_IRIGIDBODY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/World/WCollider.h"

// total size: 0xC
class IRigidBody : public UTL::COM::IUnknown, public UTL::Collections::Listable<IRigidBody, 160> {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IRigidBody(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IRigidBody() {}

    virtual ISimable *GetOwner() const = 0;
    virtual bool IsSimple() const = 0;
    virtual int GetIndex() const = 0;
    virtual SimableType GetSimableType() const = 0;
    virtual float GetRadius() const = 0;
    virtual float GetMass() const = 0;
    virtual float GetOOMass() const = 0;
    virtual const UMath::Vector3 &GetPosition() const = 0;
    virtual const UMath::Vector3 &GetLinearVelocity() const = 0;
    virtual const UMath::Vector3 &GetAngularVelocity() const = 0;
    virtual float GetSpeed() const = 0;
    virtual float GetSpeedXZ() const = 0;
    virtual void GetForwardVector(UMath::Vector3 &vec) const = 0;
    virtual void GetRightVector(UMath::Vector3 &vec) const = 0;
    virtual void GetUpVector(UMath::Vector3 &vec) const = 0;
    virtual void GetMatrix4(UMath::Matrix4 &mat) const = 0;
    virtual const UMath::Vector4 &GetOrientation() const = 0;
    virtual void GetDimension(UMath::Vector3 &dim) const = 0;
    virtual UMath::Vector3 GetDimension() const = 0;
    virtual unsigned int GetTriggerFlags() const = 0;
    virtual const WCollider *GetWCollider() const = 0;
    virtual void GetPointVelocity(const UMath::Vector3 &position, UMath::Vector3 &velocity) const = 0;
    virtual void SetPosition(const UMath::Vector3 &pos) = 0;
    virtual void SetLinearVelocity(const UMath::Vector3 &vel) = 0;
    virtual void SetAngularVelocity(const UMath::Vector3 &vel) = 0;
    virtual void SetRadius(float radius) = 0;
    virtual void SetMass(float mass) = 0;
    virtual void SetOrientation(const UMath::Vector4 &newOrientation) = 0;
    virtual void SetOrientation(const UMath::Matrix4 &orientMat) = 0;
    virtual void ModifyXPos(float offset) = 0;
    virtual void ModifyYPos(float offset) = 0;
    virtual void ModifyZPos(float offset) = 0;
    virtual void Resolve(const UMath::Vector3 &force, const UMath::Vector3 &torque) = 0;
    virtual void ResolveForce(const UMath::Vector3 &force) = 0;
    virtual void ResolveTorque(const UMath::Vector3 &torque) = 0;
    virtual void ResolveForce(const UMath::Vector3 &force, const UMath::Vector3 &p) = 0;
    virtual void ResolveTorque(const UMath::Vector3 &f, const UMath::Vector3 &p) = 0;
    virtual void PlaceObject(const UMath::Matrix4 &orientMat, const UMath::Vector3 &initPos) = 0;
    virtual void Accelerate(const UMath::Vector3 &a, float dT) = 0;
    virtual void ConvertLocalToWorld(UMath::Vector3 &val, bool translate) const = 0;
    virtual void ConvertWorldToLocal(UMath::Vector3 &val, bool translate) const = 0;
    virtual void Debug() = 0;
};

#endif
