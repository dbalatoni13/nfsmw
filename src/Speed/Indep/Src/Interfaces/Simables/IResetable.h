#ifndef INTERFACES_SIMABLES_IRESETABLE_H
#define INTERFACES_SIMABLES_IRESETABLE_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

class IResetable : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IResetable);

    virtual bool HasResetPosition();
    virtual bool ResetVehicle(bool manual);
    virtual void SetResetPosition(const UMath::Vector3 &position, const UMath::Vector3 &direction);
    virtual void ClearResetPosition();
};

#endif
