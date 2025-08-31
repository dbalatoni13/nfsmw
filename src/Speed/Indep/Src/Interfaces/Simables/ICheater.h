#ifndef INTERFACES_SIMABLES_ICHEATER_H
#define INTERFACES_SIMABLES_ICHEATER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

// Credits: Brawltendo
class ICheater : public UTL::COM::IUnknown {
  public:
    ICheater(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    virtual ~ICheater() {}

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  public:
    virtual float GetCatchupCheat();
};

#endif
