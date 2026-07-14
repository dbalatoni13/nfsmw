#ifndef IRESETABLE_H
#define IRESETABLE_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

class IResetable : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IResetable);

    virtual bool HasResetPosition() = 0;
    virtual bool ResetVehicle(bool manual) = 0;
    virtual void SetResetPosition(const UMath::Vector3 &position, const UMath::Vector3 &direction) = 0;
    virtual void ClearResetPosition() = 0;
};

#endif
