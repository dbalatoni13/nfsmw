#ifndef SIM_SIMATTACHABLE_H
#define SIM_SIMATTACHABLE_H

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/IAttachable.h"

namespace Sim {

// total size: 0x10
class Attachments {
  public:
    bool Attach(UTL::COM::IUnknown *object);
    bool Detach(UTL::COM::IUnknown *object);
    bool IsAttached(const UTL::COM::IUnknown *object) const;
    void DetachAll();

    virtual ~Attachments();

    USE_FASTALLOC(Sim::Attachments);

    Attachments(IAttachable *pOwner) {
        mOwner = pOwner;
    }

    const IAttachable::List &GetList() const {
        return mList;
    }

  private:
    IAttachable::List mList; // offset 0x0, size 0x8
    IAttachable *mOwner;     // offset 0x8, size 0x4
};

}; // namespace Sim

#endif
