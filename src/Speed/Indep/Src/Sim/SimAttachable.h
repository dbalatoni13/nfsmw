#ifndef SIM_SIMATTACHABLE_H
#define SIM_SIMATTACHABLE_H

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

    // Virtual functions
    virtual ~Attachments();

    // Inlines
    void *operator new(std::size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    Attachments(IAttachable *pOwner) {
        mOwner = pOwner;
    }

    const UTL::Std::list<IAttachable *, _type_IAttachableList> &GetList() const {
        return mList;
    }

  private:
    UTL::Std::list<IAttachable *, _type_IAttachableList> mList; // offset 0x0, size 0x8
    IAttachable *mOwner;                                        // offset 0x8, size 0x4
};

}; // namespace Sim

#endif
