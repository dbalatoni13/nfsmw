#ifndef INTERFACES_SIMABLES_IAUDIBLE_H
#define INTERFACES_SIMABLES_IAUDIBLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

class IAudible : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IAudible(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IAudible() {}

  public:
    virtual bool IsAudible() const;
};

#endif
