#ifndef INTERFACES_SIMABLES_IENGINE_H
#define INTERFACES_SIMABLES_IENGINE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

// Credits: Brawltendo
class IEngine : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IEngine(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IEngine();

  public:
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

struct EngineParams : public Sim::Param {
    EngineParams(const EngineParams &_ctor_arg) : Sim::Param(_ctor_arg) {}

    EngineParams() : Sim::Param(TypeName(), static_cast<EngineParams *>(nullptr)) {}

    static UCrc32 TypeName() {
        static UCrc32 value = "EngineParams";
        return value;
    }
};

class IDragEngine : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IDragEngine(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IDragEngine();

  public:
    virtual float GetOverRev() const;
    virtual float GetHeat() const;
};

class IRaceEngine : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle();

    IRaceEngine(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IRaceEngine();

  public:
    virtual float GetPerfectLaunchRange(float &range);
};

#endif
