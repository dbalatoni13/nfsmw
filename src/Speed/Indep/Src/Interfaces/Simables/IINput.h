#ifndef IINPUT_H
#define IINPUT_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"

class IInputPlayer : public UTL::COM::IUnknown, public UTL::Collections::Listable<IInputPlayer, 8> {
  public:
    DECL_INTERFACE(IInputPlayer);

    virtual void BlockInput(bool block);
    virtual bool IsBlocked() const;
    virtual void FlushInput();
    virtual void FetchInput();
    virtual void ClearInput();
};

// Credits: Brawltendo
class IInput : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IInput);

    virtual void ClearInput();
    virtual InputControls &GetControls() const;
    virtual float GetControlHandBrake() const;
    virtual bool GetControlActionButton() const;
    virtual void SetControlSteering(float steer);
    virtual void SetControlGas(float gas);
    virtual void SetControlBrake(float brake);
    virtual void SetControlNOS(bool nos_on);
    virtual void SetControlHandBrake(float hbrake);
    virtual void SetControlActionButton(bool hAction);
    virtual void SetControlSteeringVertical(float steer);
    virtual void SetControlBanking(float bank);
    virtual float GetControlBanking();
    virtual bool IsLookBackButtonPressed() const;
    virtual bool IsPullBackButtonPressed() const;
    virtual bool IsAutomaticShift() const;
};

#endif
