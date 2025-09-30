#ifndef INTERFACES_SIMABLES_IRIGIDBODY_H
#define INTERFACES_SIMABLES_IRIGIDBODY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"

class IRigidBody : public UTL::COM::IUnknown, public UTL::Collections::Listable<IRigidBody, 160> {
  public:
    IRigidBody(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    virtual ~IRigidBody() {}

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  public:
    virtual ISimable *GetOwner() const;
    virtual bool IsSimple() const;
    virtual int GetIndex() const;
    virtual SimableType GetSimableType() const;
    virtual float GetRadius() const;
    virtual float GetMass() const;
    virtual float GetOOMass() const;
    virtual const UMath::Vector3 &GetPosition() const;
    virtual const UMath::Vector3 &GetLinearVelocity() const;
    virtual const UMath::Vector3 &GetAngularVelocity() const;
    virtual float GetSpeed() const;
    virtual float GetSpeedXZ() const;
    virtual void GetForwardVector(UMath::Vector3 &vec) const;
    virtual void GetRightVector(UMath::Vector3 &vec) const;
    virtual void GetUpVector(UMath::Vector3 &vec) const;
    virtual void GetMatrix4(UMath::Matrix4 &mat) const;
    virtual const UMath::Vector4 &GetOrientation() const;
    virtual void GetDimension(UMath::Vector3 &dim) const;
    virtual UMath::Vector3 GetDimension() const;
    virtual unsigned int GetTriggerFlags() const;
    virtual const struct WCollider *GetWCollider() const;
    virtual void GetPointVelocity(const UMath::Vector3 &position, UMath::Vector3 &velocity) const;
    virtual void SetPosition(const UMath::Vector3 &pos);
    virtual void SetLinearVelocity(const UMath::Vector3 &vel);
    virtual void SetAngularVelocity(const UMath::Vector3 &vel);
    virtual void SetRadius(float radius);
    virtual void SetMass(float mass);
    virtual void SetOrientation(const UMath::Matrix4 &orientMat);
    virtual void SetOrientation(const UMath::Vector4 &newOrientation);
    virtual void ModifyXPos(float offset);
    virtual void ModifyYPos(float offset);
    virtual void ModifyZPos(float offset);
    virtual void Resolve(const UMath::Vector3 &force, const UMath::Vector3 &torque);
    virtual void ResolveForce(const UMath::Vector3 &force, const UMath::Vector3 &p);
    virtual void ResolveTorque(const UMath::Vector3 &f, const UMath::Vector3 &p);
    virtual void ResolveTorque(const UMath::Vector3 &torque);
    virtual void ResolveForce(const UMath::Vector3 &force);
    virtual void PlaceObject(const UMath::Matrix4 &orientMat, const UMath::Vector3 &initPos);
    virtual void Accelerate(const UMath::Vector3 &a, float dT);
    virtual void ConvertLocalToWorld(UMath::Vector3 &val, bool translate) const;
    virtual void ConvertWorldToLocal(UMath::Vector3 &val, bool translate) const;
    virtual void Debug();
};

#endif
