#ifndef IINPUT_H
#define IINPUT_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"

class IInputPlayer : public UTL::COM::IUnknown, public UTL::Collections::Listable<IInputPlayer, 8> {
  public:
    DECL_INTERFACE(IInputPlayer);

    virtual void BlockInput(bool block) = 0;
    virtual bool IsBlocked() const = 0;
    virtual void FlushInput() = 0;
    virtual void FetchInput() = 0;
    virtual void ClearInput() = 0;
};

// Credits: Brawltendo
class IInput : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IInput);

    virtual void ClearInput() = 0;
    virtual InputControls &GetControls() const = 0;
    virtual bool GetControlHandBrake() const = 0;
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

#endif
