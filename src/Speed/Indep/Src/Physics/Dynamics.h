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

struct HJOINT__;

enum eJointFlags {
    JF_IMMOBILE_FEMALE = 2,
    JF_IMMOBILE_MALE = 1,
    JF_NONE = 0,
};

enum eConstraint {
    CONICAL = 2,
    HYPERBOLIC = 1,
    PRISMATIC = 0,
};

HJOINT__ *Create(IEntity *female, const UMath::Vector3 &female_lever, IEntity *male, const UMath::Vector3 &male_lever, eJointFlags flags);
void Constrain(HJOINT__ *hjoint, IEntity *entity, const UMath::Matrix4 &mat, float theta1, float theta2, const UMath::Vector3 &post_vec, eConstraint type);
void Release(IEntity *rb0);
bool IsJoined(const IEntity *rb);
bool IsJoined(const IEntity *rb0, const IEntity *rb1);

}; // namespace Articulation

}; // namespace Dynamics

class IDynamicsEntity : public UTL::COM::IUnknown, public Dynamics::IEntity {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IDynamicsEntity(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IDynamicsEntity();
};

#endif
