#ifndef ICHEATER_H
#define ICHEATER_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

// Credits: Brawltendo
class ICheater : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(ICheater);

    virtual float GetCatchupCheat() const = 0;
};

#endif
