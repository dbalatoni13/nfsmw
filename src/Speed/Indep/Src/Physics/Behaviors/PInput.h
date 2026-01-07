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

    PInput(const BehaviorParams &bp);

    // Overrides
    // IUnknown
    ~PInput() override;

    // Behavior
    void OnTaskSimulate(float dT) override;
    void Reset() override;

    // IInput
    void ClearInput() override;

    // Virtual methods
    virtual void SetControlStrafeVertical(float steer) {
        mControls.fStrafeVertical = steer;
    }

    virtual void SetControlStrafeHorizontal(float steer) {
        mControls.fStrafeHorizontal = steer;
    }

    void SetControlBanking(float bank) override {
        mControls.fBanking = bank;
    }

    float GetControlBanking() override {
        return mControls.fBanking;
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

  private:
    InputControls mControls; // offset 0x54, size 0x28
};

// total size: 0x344
class InputPlayer : public PInput, public IInputPlayer {
  public:
    static Behavior *Construct(const BehaviorParams &params);

    InputPlayer(const BehaviorParams &bp);
    bool DoShifting(int shift_dir);
    void DoAutoReverse(const float gas_input, const float brake_input);
    void DoAutoBraking(const float gas_input, const float brake_input);

    // Overrides
    // IUnknown
    ~InputPlayer() override;

    // Behavior
    void OnBehaviorChange(const UCrc32 &mechanic) override;
    void Reset() override;
    void OnTaskSimulate(float dT) override;

    // IInput
    void ClearInput() override;
    bool IsAutomaticShift() const override;

    // IInputPlayer
    void FlushInput() override;
    void BlockInput(bool block) override;
    void FetchInput() override;

    // Virtual methods
    virtual bool OnAction(const ActionRef &a) {
        return true;
    }

    // Inlines
    IVehicle *GetVehicle() const {
        return mVehicle;
    }

  private:
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
    static Behavior *Construct(const BehaviorParams &params);

    InputPlayerDrag(const BehaviorParams &bp);

    // Overrides
    // IInputPlayer
    void FetchInput() override;

    // InputPlayer
    bool OnAction(const ActionRef &a) override;
};

// total size: 0x7C
class InputNIS : public PInput {
    static Behavior *Construct(const BehaviorParams &params);

    InputNIS(const BehaviorParams &bp);
};

#endif
