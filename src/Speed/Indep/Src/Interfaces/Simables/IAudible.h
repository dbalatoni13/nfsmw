#ifndef IAUDIBLE_H
#define IAUDIBLE_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

class IAudible : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IAudible);

    virtual bool IsAudible() const = 0;
};

#endif
