#ifndef INTERFACES_SIMABLES_IHELICOPTER_H
#define INTERFACES_SIMABLES_IHELICOPTER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

class ISimpleChopper : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    ISimpleChopper(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  protected:
    ~ISimpleChopper() override {}

  public:
};

#endif
