#ifndef IARTICULATEDVEHICLE_H
#define IARTICULATEDVEHICLE_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"

class IArticulatedVehicle : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IArticulatedVehicle);

    virtual IVehicle *GetTrailer() const;
    virtual void SetHitch(bool hitched);
    virtual bool Pose();
    virtual bool IsHitched() const;
};

#endif
