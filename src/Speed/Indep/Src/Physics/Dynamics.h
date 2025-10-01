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

    virtual const UMath::Vector3 &GetPosition() const;
    virtual void SetPosition(const UMath::Vector3 &position);
    virtual const UMath::Vector3 &GetAngularVelocity() const;
    virtual void SetAngularVelocity(const UMath::Vector3 &);
    virtual const UMath::Vector3 &GetLinearVelocity() const;
    virtual void SetLinearVelocity(const UMath::Vector3 &);
    virtual const UMath::Matrix4 &GetRotation() const;
    virtual void SetRotation(const UMath::Matrix4 &mat);
    virtual const UMath::Vector4 &GetOrientation() const;
    virtual void SetOrientation(const UMath::Vector4 &);
    virtual const UMath::Vector3 &GetPrincipalInertia() const;
    virtual float GetMass() const;
    virtual const UMath::Vector3 &GetCenterOfGravity() const;
    virtual bool IsImmobile() const;
};

namespace Articulation {

void Release(IEntity *rb0);

};

}; // namespace Dynamics

class IDynamicsEntity : public UTL::COM::IUnknown, public Dynamics::IEntity {
  public:
    IDynamicsEntity(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    virtual ~IDynamicsEntity() {}

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }
};

#endif
