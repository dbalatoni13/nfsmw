#ifndef SIM_SIMENTITY_H
#define SIM_SIMENTITY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "SimAttachable.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IEntity.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Sim/SimObject.h"

namespace Sim {

// total size: 0x50
class Entity : public Object, public UTL::Collections::GarbageNode<Sim::Entity, 8>, public IEntity, public IAttachable {
  public:
    Entity();

    void operator delete(void *mem, std::size_t size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr);
        }
    }

    // Virtual methods
    // IUnknown
    ~Entity() override;

    // IEntity
    void AttachPhysics(ISimable *object) override;
    void DetachPhysics() override;

    // Own
    virtual const UMath::Vector3 &GetPosition() const;
    virtual bool SetPosition(const UMath::Vector3 &position) const;

    // IEntity
    ISimable *GetSimable() const override {
        return mSimable;
    }
    // Own
    virtual void Kill();

    // IAttachable
    bool Attach(IUnknown *object) override;
    bool Detach(IUnknown *object) override;

    bool IsAttached(const IUnknown *pOther) const override {
        return mAttachments->IsAttached(pOther);
    }

    void OnAttached(IAttachable *pOther) override {}

    void OnDetached(IAttachable *pOther) override;

    const UTL::Std::list<IAttachable *, _type_IAttachableList> *GetAttachments() const override {
        return &mAttachments->GetList();
    }

  private:
    ISimable *mSimable;        // offset 0x48, size 0x4
    Attachments *mAttachments; // offset 0x4C, size 0x4
};

}; // namespace Sim

#endif
