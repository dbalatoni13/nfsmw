#ifndef INTERFACES_SIMABLES_ICHEATER_H
#define INTERFACES_SIMABLES_ICHEATER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

// Credits: Brawltendo
class ICheater : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    ICheater(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~ICheater() {}

  public:
    virtual float GetCatchupCheat();
};

#endif
