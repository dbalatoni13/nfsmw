#ifndef IINDUCTIBLE_H
#define IINDUCTIBLE_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"

// Credits: Brawltendo
class IInductable : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IInductable);

    virtual Physics::Info::eInductionType InductionType() const = 0;
    virtual float InductionSpool() const = 0;
    virtual float GetInductionPSI() const = 0;
    virtual float GetMaxInductionPSI() const = 0;
};

#endif
