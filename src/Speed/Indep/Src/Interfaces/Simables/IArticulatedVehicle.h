#ifndef INTERFACES_SIMABLES_IARTICULATEDVEHICLE_H
#define INTERFACES_SIMABLES_IARTICULATEDVEHICLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"

class IArticulatedVehicle : public UTL::COM::IUnknown {
  public:
    IArticulatedVehicle(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    virtual ~IArticulatedVehicle() {}

    virtual IVehicle *GetTrailer() const;
    virtual void SetHitch(bool hitched);
    virtual bool Pose();
    virtual bool IsHitched() const;
};

#endif
