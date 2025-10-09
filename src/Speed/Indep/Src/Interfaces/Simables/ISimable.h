#ifndef INTERFACES_SIMABLES_ISIMABLE_H
#define INTERFACES_SIMABLES_ISIMABLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IEntity.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICause.h"
#include "Speed/Indep/Src/Main/EventSequencer.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/Src/World/World.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

struct HSIMABLE__ {
    // total size: 0x4
    int unused; // offset 0x0, size 0x4
};

typedef HSIMABLE__ *HSIMABLE;

// TODO move?
enum SimableType {
    SIMABLE_FRAGMENT = 8,
    SIMABLE_SENTRY = 7,
    SIMABLE_NEWTON = 6,
    SIMABLE_WEAPON = 5,
    SIMABLE_HUMAN = 4,
    SIMABLE_EXPLOSION = 3,
    SIMABLE_SMACKABLE = 2,
    SIMABLE_VEHICLE = 1,
    SIMABLE_INVALID = 0,
};

class ISimable : public UTL::COM::IUnknown,
                 public UTL::Collections::Instanceable<HSIMABLE, ISimable, 160>,
                 public UTL::COM::Factory<Sim::Param, ISimable, UCrc32> {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    ISimable(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~ISimable() {}

    virtual SimableType GetSimableType() const = 0;
    virtual void Kill() = 0;
    virtual bool Attach(UTL::COM::IUnknown *object) = 0;
    virtual bool Detach(UTL::COM::IUnknown *object) = 0;
    virtual const UTL::Std::list<IAttachable *, _type_IAttachableList> *GetAttachments() const = 0;
    virtual void AttachEntity(Sim::IEntity *e) = 0;
    virtual void DetachEntity() = 0;
    virtual struct IPlayer *GetPlayer() const = 0;
    virtual bool IsPlayer() const = 0;
    virtual bool IsOwnedByPlayer() const = 0;
    virtual Sim::IEntity *GetEntity() const = 0;
    virtual void DebugObject() = 0;
    virtual HSIMABLE GetOwnerHandle() const = 0;
    virtual ISimable *GetOwner() const = 0;
    virtual bool IsOwnedBy(ISimable *queriedOwner) const = 0;
    virtual void SetOwnerObject(ISimable *pOwner) = 0;
    virtual const Attrib::Instance &GetAttributes() const = 0;
    virtual WWorldPos &GetWPos() = 0;
    virtual const WWorldPos &GetWPos() const = 0;
    virtual class IRigidBody *GetRigidBody() = 0;
    virtual const class IRigidBody *GetRigidBody() const = 0;
    virtual bool IsRigidBodySimple() const = 0;
    virtual bool IsRigidBodyComplex() const = 0;
    virtual const UMath::Vector3 &GetPosition() const = 0;
    virtual void GetTransform(UMath::Matrix4 &matrix) const = 0;
    virtual void GetLinearVelocity(UMath::Vector3 &velocity) const = 0;
    virtual void GetAngularVelocity(UMath::Vector3 &velocity) const = 0;
    virtual unsigned int GetWorldID() const = 0;
    virtual EventSequencer::IEngine *GetEventSequencer() = 0;
    virtual void ProcessStimulus(unsigned int stimulus) = 0;
    virtual const IModel *GetModel() const = 0;
    virtual IModel *GetModel() = 0;
    virtual void SetCausality(HCAUSE from, float time) = 0;
    virtual HCAUSE GetCausality() const = 0;
    virtual float GetCausalityTime() const = 0;
};

#endif
