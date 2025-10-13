#ifndef INTERFACES_SIMABLES_INISCARCONTROL_H
#define INTERFACES_SIMABLES_INISCARCONTROL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

class INISCarControl : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    INISCarControl(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~INISCarControl() {}

    virtual float GetBurnout() const;
    virtual void SetBurnout(float speed);
    virtual void SetBrakeLock(bool front, bool rear);
    virtual void SetConstraintAngle(float angle);
    virtual void SetSteering(float angle, float weight);
    virtual bool SetNISPosition(const UMath::Matrix4 &position, bool initial, float dT);
    virtual void RestoreState();
    virtual void SetAnimPitch(float dip, float time);
    virtual void SetAnimRoll(float dip, float time);
    virtual void SetAnimShake(float dip, float cycleRate, float cycleRamp, float time);
    virtual float GetAnimPitch() const;
    virtual float GetAnimRoll() const;
    virtual float GetAnimShake() const;
};

class INISCarEngine : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    INISCarEngine(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~INISCarEngine() {}

    virtual void SetNeutralRev(bool b, float throttle, float speed);
    virtual void RestoreState();
    virtual void SetNitro(bool b);
};

#endif
