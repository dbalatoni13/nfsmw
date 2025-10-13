#ifndef PHYSICS_DYNAMICS_H
#define PHYSICS_DYNAMICS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

namespace Dynamics {

class IEntity {
  public:
    IEntity() {}

    virtual const UMath::Vector3 &GetPosition() const = 0;
    virtual void SetPosition(const UMath::Vector3 &position) = 0;
    virtual const UMath::Vector3 &GetAngularVelocity() const = 0;
    virtual void SetAngularVelocity(const UMath::Vector3 &) = 0;
    virtual const UMath::Vector3 &GetLinearVelocity() const = 0;
    virtual void SetLinearVelocity(const UMath::Vector3 &) = 0;
    virtual const UMath::Matrix4 &GetRotation() const = 0;
    virtual void SetRotation(const UMath::Matrix4 &mat) = 0;
    virtual const UMath::Vector4 &GetOrientation() const = 0;
    virtual void SetOrientation(const UMath::Vector4 &) = 0;
    virtual const UMath::Vector3 &GetPrincipalInertia() const = 0;
    virtual float GetMass() const = 0;
    virtual const UMath::Vector3 &GetCenterOfGravity() const = 0;
    virtual bool IsImmobile() const = 0;
};

namespace Articulation {

void Release(IEntity *rb0);
bool IsJoined(const IEntity *rb);

}; // namespace Articulation

}; // namespace Dynamics

class IDynamicsEntity : public UTL::COM::IUnknown, public Dynamics::IEntity {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IDynamicsEntity(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IDynamicsEntity() {}
};

#endif
