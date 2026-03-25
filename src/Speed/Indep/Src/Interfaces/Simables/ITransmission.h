#ifndef INTERFACES_SIMABLES_ITRANSMISSION_H
#define INTERFACES_SIMABLES_ITRANSMISSION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"

// Credit: Brawltendo
class ITransmission : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    ITransmission(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~ITransmission();

  public:
    virtual GearID GetGear() const = 0;
    virtual GearID GetTopGear() const = 0;
    virtual bool Shift(GearID to_gear) = 0;
    virtual bool IsGearChanging() const = 0;
    virtual bool IsReversing() const = 0;
    virtual float GetSpeedometer() const = 0;
    virtual float GetMaxSpeedometer() const = 0;
    virtual float GetDriveTorque() const = 0;
    virtual float GetShiftPoint(GearID from_gear, GearID to_gear) const = 0;
    virtual ShiftStatus GetShiftStatus() const = 0;
    virtual ShiftPotential GetShiftPotential() const = 0;
};

class IDragTransmission : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle();

    IDragTransmission(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, (HINTERFACE)_IHandle) {}

    virtual ~IDragTransmission() {}

  public:
    virtual float GetShiftBoost() const = 0;
};

#endif
