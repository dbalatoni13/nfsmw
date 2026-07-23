#ifndef ISIMABLE_H
#define ISIMABLE_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICause.h"
#include "Speed/Indep/Src/Main/EventSequencer.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/Src/World/WWorldPos.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

namespace Sim {
class IEntity;
};

class ISimable : public UTL::COM::IUnknown,
                 public UTL::Collections::Instanceable<HSIMABLE, ISimable, 160>,
                 public UTL::COM::Factory<Sim::Param, ISimable, UCrc32> {
  public:
    DECL_INTERFACE(ISimable);

    virtual SimableType GetSimableType() const = 0;
    virtual void Kill() = 0;
    virtual bool Attach(UTL::COM::IUnknown *object) = 0;
    virtual bool Detach(UTL::COM::IUnknown *object) = 0;
    virtual const IAttachable::List *GetAttachments() const = 0;
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
    virtual IModel *GetModel() = 0;
    virtual const IModel *GetModel() const = 0;
    virtual void SetCausality(HCAUSE from, float time) = 0;
    virtual HCAUSE GetCausality() const = 0;
    virtual float GetCausalityTime() const = 0;
};

#define BIND_PHYSICS_FACTORY(_TYPE_) ISimable::Prototype _##_TYPE_(UCrc32(#_TYPE_), _TYPE_::Construct);

#endif
