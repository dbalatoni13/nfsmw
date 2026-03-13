#ifndef SIM_SIM_ACTIVITY_H
#define SIM_SIM_ACTIVITY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "SimAttachable.h"
#include "SimObject.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IActivity.h"

namespace Sim {

// total size: 0x4C
class Activity : public Sim::Object, public UTL::Collections::GarbageNode<Sim::Activity, 40>, public IActivity, public IAttachable {
  public:
    Activity(unsigned int num_interfaces);
    void DetachAll();

    // Virtual overrides
    // IUnknown
    ~Activity() override;

    // IActivity
    void Release() override;
    bool Attach(IUnknown *object) override {
        if (mAttachments) {
            return mAttachments->Attach(object);
        }
        return false;
    }

    bool Detach(IUnknown *object) override {
        if (mAttachments) {
            return mAttachments->Detach(object);
        }
        return false;
    }

    // IAttachable
    void OnAttached(IAttachable *pOther) override {}

    void OnDetached(IAttachable *pOther) override {}

    bool IsAttached(const IUnknown *pOther) const override {
        if (mAttachments) {
            return mAttachments->IsAttached(pOther);
        }
        return false;
    }

    const IAttachable::List *GetAttachments() const override {
        if (mAttachments) {
            return &mAttachments->GetList();
        }
        return nullptr;
    }

  private:
    Attachments *mAttachments; // offset 0x48, size 0x4
};

}; // namespace Sim

#endif
