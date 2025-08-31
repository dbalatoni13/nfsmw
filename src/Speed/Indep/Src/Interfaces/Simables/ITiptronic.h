#ifndef INTERFACES_SIMABLES_ITIPTRONIC_H
#define INTERFACES_SIMABLES_ITIPTRONIC_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"

// Credit: Brawltendo
class ITiptronic : public UTL::COM::IUnknown {
  public:
    ITiptronic(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    virtual ~ITiptronic() {}

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  public:
    virtual bool SportShift(GearID gear) {}
};

#endif
