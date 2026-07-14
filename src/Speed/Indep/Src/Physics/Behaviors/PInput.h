#ifndef PHYSICS_BEHAVIORS_PINPUT_H
#define PHYSICS_BEHAVIORS_PINPUT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Physics/Behavior.h"

// total size: 0x7C
class PInput : public Behavior, public IInput {
  public:
    static Behavior *Construct(const BehaviorParams &params);

  protected:
    // Overrides: Behavior
    void OnTaskSimulate(float dT) override;
    void Reset() override;

    // Overrides: IInput
    void ClearInput() override;
    void SetControlSteering(float steer) override {
        this->mControls.fSteering = steer;
    }
    void SetControlGas(float gas) override {
        this->mControls.fGas = gas;
    }
    void SetControlBrake(float brake) override {
        this->mControls.fBrake = brake;
    }
    InputControls &GetControls() const override {
        return const_cast<InputControls &>(this->mControls);
    }
    void SetControlHandBrake(float hbrake) override {
        this->mControls.fHandBrake = hbrake;
    }
    bool GetControlHandBrake() const override {
        return this->mControls.fHandBrake != 0.0f;
    }
    void SetControlActionButton(bool hAction) override {
        this->mControls.fActionButton = hAction;
    }
    bool GetControlActionButton() const override {
        if (!this->mControls.fActionButton) {
            return false;
        }
        return true;
    }
    void SetControlSteeringVertical(float steer) override {
        this->mControls.fSteeringVertical = steer;
    }
    void SetControlBanking(float bank) override {
        this->mControls.fBanking = bank;
    }
    float GetControlBanking() override {
        return this->mControls.fBanking;
    }
    void SetControlNOS(bool nos_on) override {
        this->mControls.fNOS = nos_on;
    }
    bool IsLookBackButtonPressed() const override {
        return false;
    }
    bool IsPullBackButtonPressed() const override {
        return false;
    }
    bool IsAutomaticShift() const override {
        return true;
    }

    PInput(const BehaviorParams &bp);
    ~PInput() override;

    virtual void SetControlStrafeVertical(float steer) {
        this->mControls.fStrafeVertical = steer;
    }

    virtual void SetControlStrafeHorizontal(float steer) {
        this->mControls.fStrafeHorizontal = steer;
    }

  private:
    InputControls mControls; // offset 0x54, size 0x28
};

// total size: 0x344
class InputPlayer : public PInput, public IInputPlayer {
  public:
    static Behavior *Construct(const BehaviorParams &params);

    // Overrides: IInputPlayer
    void BlockInput(bool block) override;
    bool IsBlocked() const override {
        return this->mBlocked;
    }
    void FlushInput() override;
    void FetchInput() override;

    // Overrides: IInput
    void ClearInput() override;
    bool IsLookBackButtonPressed() const override {
        return mLookBackButton > 0.0f;
    }
    bool IsPullBackButtonPressed() const override {
        return mPullBackButton > 0.0f;
    }
    bool IsAutomaticShift() const override;

    // Overrides: Behavior
    void OnTaskSimulate(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;
    void Reset() override;

    InputPlayer(const BehaviorParams &bp);
    ~InputPlayer() override;

    virtual bool OnAction(const ActionRef &a) {
        return true;
    }

    IVehicle *GetVehicle() const {
        return mVehicle;
    }

  private:
    void DoAutoBraking(const float gas_input, const float brake_input);
    bool DoShifting(int shift_dir);
    void DoAutoReverse(const float gas_input, const float brake_input);

    IVehicle *mVehicle;    // offset 0x88, size 0x4
    IHumanAI *mHumanAI;    // offset 0x8C, size 0x4
    ActionQueue mActionQ;  // offset 0x90, size 0x294
    float mBanking;        // offset 0x324, size 0x4
    float mSteerRight;     // offset 0x328, size 0x4
    float mSteerLeft;      // offset 0x32C, size 0x4
    float mGasButton;      // offset 0x330, size 0x4
    float mBrakeButton;    // offset 0x334, size 0x4
    float mLookBackButton; // offset 0x338, size 0x4
    float mPullBackButton; // offset 0x33C, size 0x4
    bool mBlocked;         // offset 0x340, size 0x1
};

// total size: 0x344
class InputPlayerDrag : public InputPlayer {
  public:
    static Behavior *Construct(const BehaviorParams &params) {
        return new InputPlayerDrag(params);
    }

    InputPlayerDrag(const BehaviorParams &bp);

    // Overrides
    // IInputPlayer
    void FetchInput() override;

    // InputPlayer
    bool OnAction(const ActionRef &a) override;
};

// total size: 0x7C
class InputNIS : public PInput {
  public:
    InputNIS(const BehaviorParams &bp);

    static Behavior *Construct(const BehaviorParams &params) {
        return new InputNIS(params);
    }
};

#endif
