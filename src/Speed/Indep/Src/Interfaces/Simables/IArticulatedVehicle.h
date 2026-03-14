#ifndef INTERFACES_SIMABLES_IARTICULATEDVEHICLE_H
#define INTERFACES_SIMABLES_IARTICULATEDVEHICLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"

class IArticulatedVehicle : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IArticulatedVehicle(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IArticulatedVehicle() {}

    virtual IVehicle *GetTrailer() const = 0;
    virtual void SetHitch(bool hitched) = 0;
    virtual bool Pose() = 0;
    virtual bool IsHitched() const = 0;
};

#endif
