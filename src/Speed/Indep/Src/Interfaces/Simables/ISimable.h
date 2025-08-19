#ifndef INTERFACES_SIMABLES_ISIMABLE_H
#define INTERFACES_SIMABLES_ISIMABLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

struct HSIMABLE__ {
    // total size: 0x4
    int unused; // offset 0x0, size 0x4
};

typedef HSIMABLE__ *HSIMABLE;

namespace Sim {

struct ISimable : public UTL::COM::IUnknown {
  public:
    ISimable(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    virtual ~ISimable() {}

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }
};

}; // namespace Sim

#endif
