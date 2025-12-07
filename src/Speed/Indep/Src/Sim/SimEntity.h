#ifndef SIM_SIMENTITY_H
#define SIM_SIMENTITY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IEntity.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Sim/SimObject.h"

namespace Sim {

// total size: 0x50
class Entity : public Object, public UTL::Collections::GarbageNode<Sim::Entity, 8>, public IEntity, public IAttachable {
  public:
    Entity();

    // Virtual methods
    // IUnknown
    ~Entity();

    // IEntity
    virtual void AttachPhysics(ISimable *object);
    virtual void DetachPhysics();

    // Own
    virtual const UMath::Vector3 &GetPosition() const;
    virtual bool SetPosition(const UMath::Vector3 &position) const;

    // IEntity
    virtual ISimable *GetSimable() const {
        return mSimable;
    }
    // Own
    virtual void Kill();

    // IAttachable
    override virtual bool Attach(IUnknown *object);
    override virtual bool Detach(IUnknown *object);

    override virtual bool IsAttached(const IUnknown *pOther) const {}

    override virtual void OnAttached(IAttachable *pOther) {}

    override virtual void OnDetached(IAttachable *pOther);

    override virtual const UTL::Std::list<IAttachable *, _type_IAttachableList> *GetAttachments() const {}

  private:
    ISimable *mSimable;        // offset 0x48, size 0x4
    Attachments *mAttachments; // offset 0x4C, size 0x4
};

}; // namespace Sim

#endif
