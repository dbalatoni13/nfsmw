#ifndef INTERFACES_SIMABLES_IINDUCTABLE_H
#define INTERFACES_SIMABLES_IINDUCTABLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"

// Credits: Brawltendo
class IInductable : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IInductable(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IInductable() {}

  public:
    virtual Physics::Info::eInductionType InductionType();
    virtual float InductionSpool();
    virtual float GetInductionPSI();
    virtual float GetMaxInductionPSI();
};

#endif
