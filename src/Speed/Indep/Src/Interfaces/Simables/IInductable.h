#ifndef INTERFACES_SIMABLES_IINDUCTABLE_H
#define INTERFACES_SIMABLES_IINDUCTABLE_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"

// Credits: Brawltendo
class IInductable : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IInductable);

    virtual Physics::Info::eInductionType InductionType() const;
    virtual float InductionSpool() const;
    virtual float GetInductionPSI() const;
    virtual float GetMaxInductionPSI() const;
};

#endif
