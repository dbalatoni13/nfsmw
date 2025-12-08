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

    // Virtual methods
    // IUnknown
    virtual ~Activity();

    // IActivity
    override virtual void Release();

    // IAttachable
    override virtual void OnAttached(IAttachable *pOther) {}

    override virtual void OnDetached(IAttachable *pOther) {}

  private:
    Attachments *mAttachments; // offset 0x48, size 0x4
};

}; // namespace Sim

#endif
