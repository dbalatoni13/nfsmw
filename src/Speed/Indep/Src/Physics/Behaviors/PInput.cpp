#include "PInput.h"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Generated/Events/EPlayerTriggeredNOS.hpp"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Input/ActionRef.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IResetable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITiptronic.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"

Behavior *PInput::Construct(const BehaviorParams &params) {
    return new PInput(params);
}

PInput::PInput(const BehaviorParams &bp) : Behavior(bp, 0), IInput(bp.fowner) {}

PInput::~PInput() {}

void PInput::OnTaskSimulate(float dT) {}

void PInput::Reset() {
    ClearInput();
}

void PInput::ClearInput() {
    mControls = InputControls();
}

Behavior *InputPlayer::Construct(const BehaviorParams &params) {
    return new InputPlayer(params);
}

InputPlayer::InputPlayer(const BehaviorParams &bp) : PInput(bp), IInputPlayer(bp.fowner), mActionQ(false) {
    mBanking = 0.0f;
    mSteerRight = 0.0f;
    mSteerLeft = 0.0f;
    mGasButton = 0.0f;
    mBrakeButton = 0.0f;
    mLookBackButton = 0.0f;
    mPullBackButton = 0.0f;
    mBlocked = false;
    GetOwner()->QueryInterface(&mVehicle);
    GetOwner()->QueryInterface(&mHumanAI);
}

void InputPlayer::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_AI) {
        GetOwner()->QueryInterface(&mHumanAI);
    }
}

InputPlayer::~InputPlayer() {
    // TODO Unlist
}

void InputPlayer::Reset() {
    FlushInput();
    PInput::Reset();
}

void InputPlayer::ClearInput() {
    PInput::ClearInput();
    mGasButton = 0.0f;
    mBrakeButton = 0.0f;
    mLookBackButton = 0.0f;
    mPullBackButton = 0.0f;
    mBanking = 0.0f;
    mSteerLeft = 0.0f;
    mSteerRight = 0.0f;
    mActionQ.Flush();
}

void InputPlayer::FlushInput() {
    mActionQ.Flush();
}

void InputPlayer::OnTaskSimulate(float dT) {
    if (mVehicle->GetForceStop()) {
        ClearInput();
        SetControlBrake(1.0f);
        if (mVehicle->GetForceStop() & 2) {
            GetOwner()->GetRigidBody()->SetAngularVelocity(UMath::Vector3::kZero);
            GetOwner()->GetRigidBody()->SetLinearVelocity(UMath::Vector3::kZero);
        }
    } else {
        PInput::OnTaskSimulate(dT);
    }
}

void InputPlayer::BlockInput(bool block) {
    if (mBlocked != block) {
        mBlocked = block;
        if (block) {
            mActionQ.Enable(false);
        } else {
            mActionQ.Enable(true);
        }
    }
}

void InputPlayer::FetchInput() {
    if (mBlocked) {
        FlushInput();
        return;
    }
    bool neutralRev = false;
    if (mVehicle && mVehicle->GetForceStop()) {
        ClearInput();
        SetControlBrake(1.0f);
        return;
    }
    IPlayer *player = GetOwner()->GetPlayer();
    if (!player || !player->GetSettings()) {
        mActionQ.SetPort(-1);
        mActionQ.SetConfig(0, "InputPlayer");
        return;
    }

    bool wheel_connected = false;
    if (player->GetSteeringDevice() && player->GetSteeringDevice()->IsConnected()) {
        wheel_connected = true;
    }

    PlayerSettings *settings = player->GetSettings();
    bool player_in_control = true;
    mActionQ.SetPort(player->GetControllerPort());
    mActionQ.SetConfig(settings->GetControllerAttribs(CA_DRIVING, wheel_connected), "InputPlayer");
    if (mHumanAI) {
        player_in_control = mHumanAI->GetAiControl() == false;
    }

    bool drive_cam_in_control = true;
    eView *view = eGetView(1, false);
    if (view) {
        CameraMover *m = view->GetCameraMover();
        if (m && m->GetType() != CM_DRIVE_CUBIC) {
            drive_cam_in_control = false;
        }
        if (gCinematicMomementCamera) {
            drive_cam_in_control = false;
        }
    }
    int shift_dir = 0;
    bool nos = GetControls().fNOS;
    while (!mActionQ.IsEmpty()) {
        ActionRef a = mActionQ.GetAction();
        if (neutralRev) {
            if (a.ID() == 5) {
                mGasButton = a.Data();
            }
            mActionQ.PopAction();
        } else {
            if (OnAction(a)) {
                switch (a.ID()) {
                    case GAMEACTION_SHIFTUP:
                        shift_dir = 1;
                        break;
                    case GAMEACTION_SHIFTDOWN:
                        shift_dir = -1;
                        break;
                    case GAMEACTION_NOS:
                        nos = a.Data() > 0.0f;
                        break;
                    case GAMEACTION_GAMEBREAKER:
                        if (player_in_control && drive_cam_in_control) {
                            SetControlActionButton(!GetControlActionButton());
                            player->ToggleGameBreaker();
                        }
                        break;
                    case GAMEACTION_STEERLEFT:
                        mSteerLeft = a.Data();
                        break;
                    case GAMEACTION_STEERRIGHT:
                        mSteerRight = a.Data();
                        break;
                    case CAMERAACTION_CHANGE:
                        if (settings) {
                            settings->ScrollDriveCam(1);
                        }
                        break;
                    case CAMERAACTION_LOOKBACK:
                        mLookBackButton = a.Data();
                        break;
                    case CAMERAACTION_PULLBACK:
                        if (mPullBackButton > 0.0f) {
                            mPullBackButton = 0.0f;
                        } else {
                            mPullBackButton = a.Data();
                        }
                        break;
                    case GAMEACTION_RESET:
                        if (player_in_control && !mVehicle->IsStaging() && drive_cam_in_control) {
                            IResetable *ir;
                            if (!GetOwner()->QueryInterface(&ir)) {
                                break;
                            }
                            ir->ResetVehicle(true);
                            CameraAI::Reset();
                        }
                        break;
                    case GAMEACTION_GAS:
                        mGasButton = a.Data();
                        break;
                    case GAMEACTION_BRAKE:
                        mBrakeButton = a.Data();
                        break;
                    case GAMEACTION_HANDBRAKE:
                        if (player_in_control) {
                            if (a.Data() > 0.0f) {
                                SetControlHandBrake(a.Data());
                            } else {
                                SetControlHandBrake(0.0f);
                            }
                        }
                        break;
                }
            }
            mActionQ.PopAction();
        }
    }
    if (player_in_control) {
        if (!GetControls().fNOS && nos) {
            new EPlayerTriggeredNOS(GetOwner()->GetInstanceHandle());
        }
        SetControlNOS(nos);
        float steering = mSteerRight - mSteerLeft;
        if (mGasButton > 1.0f - VehicleSystem::PAD_DEAD_ZONE) {
            mGasButton = 1.0f;
        }
        if (mGasButton < VehicleSystem::PAD_DEAD_ZONE) {
            mGasButton = 0.0f;
        }
        if (mBrakeButton > 1.0f - VehicleSystem::PAD_DEAD_ZONE) {
            mBrakeButton = 1.0f;
        }
        if (mBrakeButton < VehicleSystem::PAD_DEAD_ZONE) {
            mBrakeButton = 0.0f;
        }
        float gas_input = mGasButton;
        float brake_input = mBrakeButton;
        SetControlGas(gas_input);
        SetControlBrake(brake_input);
        DoShifting(shift_dir);
        DoAutoReverse(gas_input, brake_input);
        DoAutoBraking(gas_input, brake_input);
        if (mVehicle && GetControls().fHandBrake != 0.0f) {
            SetControlBrake(0.0f);
        }
        SetControlSteering(steering);
        SetControlBanking(mBanking);
    }
}

void InputPlayer::DoAutoBraking(const float gas_input, const float brake_input) {
    ITransmission *itranny;
    if (!mVehicle->QueryInterface(&itranny) || (gas_input > 0.0f) || (brake_input > 0.0f)) {
        return;
    }
    const int current_gear = itranny->GetGear();
    float speed = mVehicle->GetSpeed();
    if (current_gear > G_REVERSE) {
        if (speed < 8.0f) {
            float brake_ratio = UMath::Ramp(speed, 0.0f, 8.0f);
            SetControlBrake(1.0f - brake_ratio * 0.75f);
        }
    } else {
        float brake_ratio = UMath::Ramp(-speed, 0.0f, 10.0f);
        SetControlBrake(1.0f - brake_ratio * 0.75f);
    }
}

bool InputPlayer::DoShifting(int shift_dir) {
    ITransmission *itranny;
    if ((shift_dir == 0) || !mVehicle || !mVehicle->QueryInterface(&itranny)) {
        return false;
    }
    const int bottom_gear = G_NEUTRAL;
    const int top_gear = itranny->GetTopGear();
    const int current_gear = itranny->GetGear();
    const int desired_gear = UMath::Clamp(current_gear + shift_dir, 1, top_gear);

    if (current_gear == 0) {
        return false;
    }
    if (desired_gear == current_gear) {
        return false;
    }
    if (mVehicle->IsStaging() && (desired_gear > 2)) {
        return false;
    }
    if (!IsAutomaticShift()) {
        if (itranny->Shift((GearID)desired_gear)) {
            return true;
        }
    } else {
        ITiptronic *itiptronic;
        if (itranny->QueryInterface(&itiptronic) && itiptronic->SportShift((GearID)desired_gear)) {
            return true;
        }
    }
    return false;
}

void InputPlayer::DoAutoReverse(const float gas_input, const float brake_input) {
    if (!mVehicle) {
        return;
    }
    ITransmission *itranny;
    if (!mVehicle->QueryInterface(&itranny)) {
        return;
    }
    float speed = mVehicle->GetSpeed();
    if (itranny->GetGear() != G_REVERSE) {
        if ((speed < 2.5f) && (brake_input > 0.0f) && gas_input == 0.0f) {
            itranny->Shift(G_REVERSE);
        }
    } else if ((speed > -5.0f) && ((brake_input == 0.0f) || (gas_input > 0.0f))) {
        itranny->Shift(G_FIRST);
    }
    if (itranny->GetGear() == G_REVERSE) {
        SetControlGas(brake_input);
        SetControlBrake(gas_input);
    }
}

bool InputPlayer::IsAutomaticShift() const {
    if (mHumanAI && mHumanAI->GetAiControl()) {
        return true;
    }
    IPlayer *player = GetOwner()->GetPlayer();
    if (player && player->GetSettings() && player->GetSettings()->Transmission == TRANSMISSION_MANUAL) {
        return false;
    }
    if (GetVehicle()->GetDriverStyle() == STYLE_DRAG) {
        return false;
    }
    return true;
}

Behavior *InputPlayerDrag::Construct(const BehaviorParams &params) {
    return new InputPlayerDrag(params);
}

InputPlayerDrag::InputPlayerDrag(const BehaviorParams &bp) : InputPlayer(bp) {}

void InputPlayerDrag::FetchInput() {
    InputPlayer::FetchInput();
}

bool InputPlayerDrag::OnAction(const ActionRef &a) {
    switch (a.ID()) {
        case GAMEACTION_TURNLEFT: {
            IHumanAI *human;
            if (GetOwner()->QueryInterface(&human)) {
                human->ChangeDragLanes(true);
            }
            break;
        }
        case GAMEACTION_TURNRIGHT: {
            IHumanAI *human;
            if (GetOwner()->QueryInterface(&human)) {
                human->ChangeDragLanes(false);
            }
            break;
        }
    }
    InputPlayer::OnAction(a);
    return true;
}

InputNIS::InputNIS(const BehaviorParams &bp) : PInput(bp) {}
