#ifndef INTERFACES_SIMABLES_ICHEATER_H
#define INTERFACES_SIMABLES_ICHEATER_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

// Credits: Brawltendo
class ICheater : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(ICheater);

    virtual float GetCatchupCheat() const;
};

#endif
