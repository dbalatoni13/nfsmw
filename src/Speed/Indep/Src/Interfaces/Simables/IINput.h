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
    virtual void SetControlSteering(float steer);
    virtual void SetControlGas(float gas);
    virtual void SetControlBrake(float brake);
    virtual void SetControlNOS(bool nos_on);
    virtual void SetControlHandBrake(float hbrake);
    virtual void SetControlActionButton(bool hAction);
    virtual void SetControlSteeringVertical(float steer);
    virtual void SetControlBanking(float bank);
    virtual float GetControlBanking();
    virtual bool IsLookBackButtonPressed();
    virtual bool IsPullBackButtonPressed();
    virtual bool IsAutomaticShift();
};

struct InputControls {
    // total size: 0x28
    float fBanking;          // offset 0x0, size 0x4
    float fSteering;         // offset 0x4, size 0x4
    float fSteeringVertical; // offset 0x8, size 0x4
    float fStrafeVertical;   // offset 0xC, size 0x4
    float fStrafeHorizontal; // offset 0x10, size 0x4
    float fGas;              // offset 0x14, size 0x4
    float fBrake;            // offset 0x18, size 0x4
    float fHandBrake;        // offset 0x1C, size 0x4
    bool fActionButton;      // offset 0x20, size 0x1
    bool fNOS;               // offset 0x24, size 0x1

    InputControls() {}
};

#endif
