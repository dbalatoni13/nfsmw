#ifndef SIM_SIM_ACTIVITY_H
#define SIM_SIM_ACTIVITY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "SimObject.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IActivity.h"

namespace Sim {

// total size: 0x4C
struct Activity : public Sim::Object, public UTL::Collections::GarbageNode<Sim::Activity, 40>, public IActivity, public IAttachable {
    struct Attachments *mAttachments; // offset 0x48, size 0x4

    Activity(unsigned int num_interfaces);
    virtual ~Activity();
    void DetachAll();

    // IActivity
    override virtual void Release();

    // IAttachable
    override virtual void OnAttached(IAttachable *pOther) {}

    override virtual void OnDetached(IAttachable *pOther) {}
};

}; // namespace Sim

#endif
