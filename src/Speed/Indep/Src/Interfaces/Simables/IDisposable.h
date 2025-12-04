#ifndef INTERFACES_SIMABLES_IDISPOSABLE_H
#define INTERFACES_SIMABLES_IDISPOSABLE_H

#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

class IDisposable : public UTL::COM::IUnknown, public UTL::Collections::Listable<IDisposable, 160> {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IDisposable(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IDisposable() {}

  public:
    virtual bool IsRequired() const;
};

#endif
