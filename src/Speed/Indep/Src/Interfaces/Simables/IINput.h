#ifndef INTERFACES_SIMABLES_IINPUT_H
#define INTERFACES_SIMABLES_IINPUT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"

// total size: 0x28
struct InputControls {
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

    InputControls() {
        fBanking = 0.0f;
        fSteering = 0.0f;
        fSteeringVertical = 0.0f;
        fStrafeVertical = 0.0f;
        fStrafeHorizontal = 0.0f;
        fGas = 0.0f;
        fBrake = 0.0f;
        fHandBrake = 0.0f;
        fActionButton = false;
        fNOS = false;
    }
};

// Credits: Brawltendo
class IInput : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IInput(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IInput() {}

    virtual void ClearInput() = 0;
    virtual InputControls &GetControls() const = 0;
    virtual float GetControlHandBrake() const = 0;
    virtual bool GetControlActionButton() const = 0;
    virtual void SetControlSteering(float steer) = 0;
    virtual void SetControlGas(float gas) = 0;
    virtual void SetControlBrake(float brake) = 0;
    virtual void SetControlNOS(bool nos_on) = 0;
    virtual void SetControlHandBrake(float hbrake) = 0;
    virtual void SetControlActionButton(bool hAction) = 0;
    virtual void SetControlSteeringVertical(float steer) = 0;
    virtual void SetControlBanking(float bank) = 0;
    virtual float GetControlBanking() = 0;
    virtual bool IsLookBackButtonPressed() const = 0;
    virtual bool IsPullBackButtonPressed() const = 0;
    virtual bool IsAutomaticShift() const = 0;
};

class IInputPlayer : public UTL::COM::IUnknown, public UTL::Collections::Listable<IInputPlayer, 8> {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IInputPlayer(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IInputPlayer() {}

    virtual void BlockInput(bool block) = 0;
    virtual bool IsBlocked() const = 0;
    virtual void FlushInput() = 0;
    virtual void FetchInput() = 0;
    virtual void ClearInput() = 0;
};

#endif
