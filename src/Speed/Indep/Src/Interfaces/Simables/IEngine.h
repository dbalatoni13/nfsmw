#ifndef INTERFACES_SIMABLES_IENGINE_H
#define INTERFACES_SIMABLES_IENGINE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

// Credits: Brawltendo
class IEngine : public UTL::COM::IUnknown {
  public:
    IEngine(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    virtual ~IEngine() {}

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  public:
    virtual float GetRPM();
    virtual float GetRedline();
    virtual float GetMaxRPM();
    virtual float GetMinRPM();
    virtual float GetPeakTorqueRPM();
    virtual void MatchSpeed(float speed);
    virtual float GetNOSCapacity();
    virtual bool IsNOSEngaged() const;
    virtual float GetNOSFlowRate() const;
    virtual float GetNOSBoost();
    virtual bool HasNOS() const;
    virtual void ChargeNOS(float charge);
    virtual float GetMaxHorsePower();
    virtual float GetMinHorsePower();
    virtual float GetHorsePower();
};

class IRaceEngine : public UTL::COM::IUnknown {
  public:
    IRaceEngine(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    virtual ~IRaceEngine() {}

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  public:
    virtual float GetPerfectLaunchRange(float &range);
};

#endif
