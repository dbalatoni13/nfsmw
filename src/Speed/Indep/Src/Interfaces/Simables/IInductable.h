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
    IInductable(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    virtual ~IInductable() {}

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  public:
    virtual Physics::Info::eInductionType InductionType();
    virtual float InductionSpool();
    virtual float GetInductionPSI();
    virtual float GetMaxInductionPSI();
};

#endif
