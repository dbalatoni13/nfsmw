#ifndef INTERFACES_SIMABLES_IENGINE_H
#define INTERFACES_SIMABLES_IENGINE_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

// Credits: Brawltendo
class IEngine : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IEngine);

    virtual float GetRPM() const;
    virtual float GetRedline() const;
    virtual float GetMaxRPM() const;
    virtual float GetMinRPM() const;
    virtual float GetPeakTorqueRPM() const;
    virtual void MatchSpeed(float speed);
    virtual float GetNOSCapacity() const;
    virtual bool IsNOSEngaged() const;
    virtual float GetNOSFlowRate() const;
    virtual float GetNOSBoost() const;
    virtual bool HasNOS() const;
    virtual void ChargeNOS(float charge);
    virtual float GetMaxHorsePower() const;
    virtual Hp GetMinHorsePower() const;
    virtual float GetHorsePower() const;
};

class IDragEngine : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IDragEngine);

    virtual float GetOverRev() const;
    virtual float GetHeat() const;
};

class IRaceEngine : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IRaceEngine);

    virtual float GetPerfectLaunchRange(float &range);
};

#endif
