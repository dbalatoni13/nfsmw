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

    virtual ~ITransmission() {}

  public:
    virtual GearID GetGear() const;
    virtual GearID GetTopGear() const;
    virtual bool Shift(GearID to_gear);
    virtual bool IsGearChanging() const;
    virtual bool IsReversing() const;
    virtual float GetSpeedometer() const;
    virtual float GetMaxSpeedometer() const;
    virtual float GetDriveTorque() const;
    virtual float GetOptimalShiftRange(GearID to_gear) const;
    virtual float GetShiftPoint(GearID from_gear, GearID to_gear) const;
    virtual ShiftStatus GetShiftStatus() const;
};

class IDragTransmission : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IDragTransmission(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IDragTransmission() {}

  public:
  	virtual float GetShiftBoost() const;
};

#endif
