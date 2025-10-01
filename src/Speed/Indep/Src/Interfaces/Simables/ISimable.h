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
    ISimable(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    virtual ~ISimable() {}

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    virtual SimableType GetSimableType() const;

    virtual void Kill();

    virtual bool Attach(UTL::COM::IUnknown *object);

    virtual bool Detach(UTL::COM::IUnknown *object);

    virtual const UTL::Std::list<IAttachable *, _type_IAttachableList> *GetAttachments() const;

    virtual void AttachEntity(Sim::IEntity *e);

    virtual void DetachEntity();

    virtual struct IPlayer *GetPlayer() const;

    virtual bool IsPlayer() const;

    virtual bool IsOwnedByPlayer() const;

    virtual Sim::IEntity *GetEntity() const;

    virtual void DebugObject();

    virtual HSIMABLE GetOwnerHandle() const;

    virtual ISimable *GetOwner() const;

    virtual bool IsOwnedBy(ISimable *queriedOwner) const;

    virtual void SetOwnerObject(ISimable *pOwner);

    virtual const Attrib::Instance &GetAttributes() const;

    virtual const WWorldPos &GetWPos() const;

    virtual WWorldPos &GetWPos();

    virtual class IRigidBody *GetRigidBody();

    virtual const class IRigidBody *GetRigidBody() const;

    virtual bool IsRigidBodySimple() const;

    virtual bool IsRigidBodyComplex() const;

    virtual const UMath::Vector3 &GetPosition() const;

    virtual void GetTransform(UMath::Matrix4 &matrix) const;

    virtual void GetLinearVelocity(UMath::Vector3 &velocity) const;

    virtual void GetAngularVelocity(UMath::Vector3 &velocity) const;

    virtual unsigned int GetWorldID() const;

    virtual EventSequencer::IEngine *GetEventSequencer();

    virtual void ProcessStimulus(unsigned int stimulus);

    virtual const IModel *GetModel() const;

    virtual IModel *GetModel();

    virtual void SetCausality(HCAUSE from, float time);

    virtual HCAUSE GetCausality() const;

    virtual float GetCausalityTime() const;
};

#endif
