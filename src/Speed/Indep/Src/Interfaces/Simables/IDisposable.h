#ifndef INTERFACES_SIMABLES_IDISPOSABLE_H
#define INTERFACES_SIMABLES_IDISPOSABLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

class IDisposable : public UTL::COM::IUnknown {
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
