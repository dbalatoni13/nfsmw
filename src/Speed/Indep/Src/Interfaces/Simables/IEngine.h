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

    virtual ~IEngine() {}

  public:
    virtual float GetRPM() const = 0;
    virtual float GetRedline() const = 0;
    virtual float GetMaxRPM() const = 0;
    virtual float GetMinRPM() const = 0;
    virtual float GetPeakTorqueRPM() const = 0;
    virtual void MatchSpeed(float speed) = 0;
    virtual float GetNOSCapacity() const = 0;
    virtual bool IsNOSEngaged() const = 0;
    virtual float GetNOSFlowRate() const = 0;
    virtual float GetNOSBoost() const = 0;
    virtual bool HasNOS() const = 0;
    virtual void ChargeNOS(float charge) = 0;
    virtual float GetMaxHorsePower() const = 0;
    virtual Hp GetMinHorsePower() const = 0;
    virtual float GetHorsePower() const = 0;
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

    virtual ~IDragEngine() {}

  public:
    virtual float GetOverRev() const = 0;
    virtual float GetHeat() const = 0;
};

class IRaceEngine : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle();

    IRaceEngine(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, (HINTERFACE)_IHandle) {}

    virtual ~IRaceEngine() {}

  public:
    virtual float GetPerfectLaunchRange(float &range) = 0;
};

#endif
