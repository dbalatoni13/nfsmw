#ifndef INTERFACES_SIMABLES_ITIPTRONIC_H
#define INTERFACES_SIMABLES_ITIPTRONIC_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"

// Credit: Brawltendo
class ITiptronic : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(ITiptronic);

    virtual bool SportShift(GearID gear);
};

#endif
