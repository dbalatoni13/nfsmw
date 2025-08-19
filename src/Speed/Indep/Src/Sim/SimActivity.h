#ifndef SIM_SIM_ACTIVITY_H
#define SIM_SIM_ACTIVITY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "SimTypes.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IActivity.h"

namespace Sim {

struct Activity : public Sim::Object, public UTL::Collections::GarbageNode<Sim::Activity, 40>, public IActivity, public IAttachable {
    // total size: 0x4C
    struct Attachments *mAttachments; // offset 0x48, size 0x4

    Activity(unsigned int num_interfaces);
    virtual ~Activity();
    void DetachAll();
    virtual void Release();

    virtual void OnAttached(IAttachable *pOther) {}

    virtual void OnDetached(IAttachable *pOther) {}
};

}; // namespace Sim

#endif
