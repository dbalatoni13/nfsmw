#ifndef INTERFACES_SIMABLES_IRESETABLE_H
#define INTERFACES_SIMABLES_IRESETABLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

class IResetable : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IResetable(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IResetable() {}

    virtual bool HasResetPosition();
    virtual bool ResetVehicle(bool manual);
    virtual void SetResetPosition(const UMath::Vector3 &position, const UMath::Vector3 &direction);
    virtual void ClearResetPosition();
};

#endif
