#ifndef INTERFACES_SIMABLES_IINPUT_H
#define INTERFACES_SIMABLES_IINPUT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

// Credits: Brawltendo
class IInput : public UTL::COM::IUnknown {
  public:
    IInput(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    virtual ~IInput() {}

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  public:
    virtual void ClearInput();
    virtual class InputControls &GetControls();
    virtual float GetControlHandBrake();
    virtual bool GetControlActionButton();
    virtual void SetControlGas(float gas);
    virtual void SetControlBrake(float brake);
    virtual void SetControlNOS(bool nos_on);
    virtual void SetControlHandBrake(float hbrake);
    virtual void SetControlSteering(float steer);
    virtual void SetControlActionButton(bool hAction);
    virtual void SetControlSteeringVertical(float steer);
    virtual void SetControlBanking(float bank);
    virtual float GetControlBanking();
    virtual bool IsLookBackButtonPressed();
    virtual bool IsPullBackButtonPressed();
    virtual bool IsAutomaticShift();
};

#endif
