#ifndef IARTICULATEDVEHICLE_H
#define IARTICULATEDVEHICLE_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"

class IArticulatedVehicle : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IArticulatedVehicle);

    virtual IVehicle *GetTrailer() const = 0;
    virtual void SetHitch(bool hitched) = 0;
    virtual bool Pose() = 0;
    virtual bool IsHitched() const = 0;
};

#endif
