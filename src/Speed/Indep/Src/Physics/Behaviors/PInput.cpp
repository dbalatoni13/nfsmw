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
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/World/RaceParameters.hpp"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"

BIND_BEHAVIOR_FACTORY(PInput);

Behavior *PInput::Construct(const BehaviorParams &params) {
    return new PInput(params);
}

PInput::PInput(const BehaviorParams &bp) : Behavior(bp, 0), IInput(bp.fowner) {}

PInput::~PInput() {}

void PInput::OnTaskSimulate(float dT) {}

void PInput::Reset() {
    this->ClearInput();
}

void PInput::ClearInput() {
    this->mControls = InputControls();
}

Behavior *InputPlayer::Construct(const BehaviorParams &params) {
    return new InputPlayer(params);
}

BIND_BEHAVIOR_FACTORY(InputPlayer);

static const bool Tweak_UseDigitalSteering = true; // TODO value and use

InputPlayer::InputPlayer(const BehaviorParams &bp) : PInput(bp), IInputPlayer(bp.fowner), mActionQ(false) {
    this->mBanking = 0.0f;
    this->mSteerRight = 0.0f;
    this->mSteerLeft = 0.0f;
    this->mGasButton = 0.0f;
    this->mBrakeButton = 0.0f;
    this->mLookBackButton = 0.0f;
    this->mPullBackButton = 0.0f;
    this->mBlocked = false;
    this->GetOwner()->QueryInterface(&this->mVehicle);
    this->GetOwner()->QueryInterface(&this->mHumanAI);
}

void InputPlayer::OnBehaviorChange(const UCrc32 &mechanic) {
    Behavior::OnBehaviorChange(mechanic);
    if (mechanic == BEHAVIOR_MECHANIC_AI) {
        this->GetOwner()->QueryInterface(&this->mHumanAI);
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
    this->mGasButton = 0.0f;
    this->mBrakeButton = 0.0f;
    this->mLookBackButton = 0.0f;
    this->mPullBackButton = 0.0f;
    this->mBanking = 0.0f;
    this->mSteerLeft = 0.0f;
    this->mSteerRight = 0.0f;
    this->mActionQ.Flush();
}

void InputPlayer::FlushInput() {
    this->mActionQ.Flush();
}

void InputPlayer::OnTaskSimulate(float dT) {
    if (this->mVehicle->GetForceStop()) {
        this->ClearInput();
        this->SetControlBrake(1.0f);
        if (this->mVehicle->GetForceStop() & 2) {
            this->GetOwner()->GetRigidBody()->SetAngularVelocity(UMath::Vector3::kZero);
            this->GetOwner()->GetRigidBody()->SetLinearVelocity(UMath::Vector3::kZero);
        }
    } else {
        PInput::OnTaskSimulate(dT);
    }
}

void InputPlayer::BlockInput(bool block) {
    if (this->mBlocked != block) {
        this->mBlocked = block;
        if (block) {
            this->mActionQ.Enable(false);
        } else {
            this->mActionQ.Enable(true);
        }
    }
}

void InputPlayer::FetchInput() {
    if (this->mBlocked) {
        FlushInput();
        return;
    }
    bool neutralRev = false;
    if ((this->mVehicle != nullptr) && this->mVehicle->GetForceStop()) {
        this->ClearInput();
        this->SetControlBrake(1.0f);
        return;
    }
    IPlayer *player = this->GetOwner()->GetPlayer();
    if ((player == nullptr) || (player->GetSettings() == nullptr)) {
        this->mActionQ.SetPort(-1);
        this->mActionQ.SetConfig(0, "InputPlayer");
        return;
    }

    bool wheel_connected = false;
    if ((player->GetSteeringDevice() != nullptr) && player->GetSteeringDevice()->IsConnected()) {
        wheel_connected = true;
    }

    PlayerSettings *settings = player->GetSettings();
    bool player_in_control = true;
    this->mActionQ.SetPort(player->GetControllerPort());
    this->mActionQ.SetConfig(settings->GetControllerAttribs(CA_DRIVING, wheel_connected), "InputPlayer");
    if (this->mHumanAI != nullptr) {
        player_in_control = this->mHumanAI->GetAiControl() == false;
    }

    bool drive_cam_in_control = true;
    eView *view = eGetView(1, false);
    if (view != nullptr) {
        CameraMover *m = view->GetCameraMover();
        if ((m != nullptr) && m->GetType() != CM_DRIVE_CUBIC) {
            drive_cam_in_control = false;
        }
        if (gCinematicMomementCamera) {
            drive_cam_in_control = false;
        }
    }
    int shift_dir = 0;
    bool nos = this->GetControls().fNOS;
    while (!this->mActionQ.IsEmpty()) {
        ActionRef a = this->mActionQ.GetAction();
        if (neutralRev) {
            if (a.ID() == 5) {
                this->mGasButton = a.Data();
            }
            this->mActionQ.PopAction();
        } else {
            if (this->OnAction(a)) {
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
                            this->SetControlActionButton(!this->GetControlActionButton());
                            player->ToggleGameBreaker();
                        }
                        break;
                    case GAMEACTION_STEERLEFT:
                        this->mSteerLeft = a.Data();
                        break;
                    case GAMEACTION_STEERRIGHT:
                        this->mSteerRight = a.Data();
                        break;
                    case CAMERAACTION_CHANGE:
                        if (settings != nullptr) {
                            settings->ScrollDriveCam(1);
                        }
                        break;
                    case CAMERAACTION_LOOKBACK:
                        this->mLookBackButton = a.Data();
                        break;
                    case CAMERAACTION_PULLBACK:
                        if (this->mPullBackButton > 0.0f) {
                            this->mPullBackButton = 0.0f;
                        } else {
                            this->mPullBackButton = a.Data();
                        }
                        break;
                    case GAMEACTION_RESET:
                        if (player_in_control && !this->mVehicle->IsStaging() && drive_cam_in_control) {
                            IResetable *ir;
                            if (!this->GetOwner()->QueryInterface(&ir)) {
                                break;
                            }
                            ir->ResetVehicle(true);
                            CameraAI::Reset();
                        }
                        break;
                    case GAMEACTION_GAS:
                        this->mGasButton = a.Data();
                        break;
                    case GAMEACTION_BRAKE:
                        this->mBrakeButton = a.Data();
                        break;
                    case GAMEACTION_HANDBRAKE:
                        if (player_in_control) {
                            if (a.Data() > 0.0f) {
                                this->SetControlHandBrake(a.Data());
                            } else {
                                this->SetControlHandBrake(0.0f);
                            }
                        }
                        break;
                }
            }
            this->mActionQ.PopAction();
        }
    }
    if (player_in_control) {
        if (!this->GetControls().fNOS && nos) {
            new EPlayerTriggeredNOS(GetOwner()->GetInstanceHandle());
        }
        this->SetControlNOS(nos);
        float steering = this->mSteerRight - this->mSteerLeft;
        if (this->mGasButton > 1.0f - VehicleSystem::PAD_DEAD_ZONE) {
            this->mGasButton = 1.0f;
        }
        if (this->mGasButton < VehicleSystem::PAD_DEAD_ZONE) {
            this->mGasButton = 0.0f;
        }
        if (this->mBrakeButton > 1.0f - VehicleSystem::PAD_DEAD_ZONE) {
            this->mBrakeButton = 1.0f;
        }
        if (this->mBrakeButton < VehicleSystem::PAD_DEAD_ZONE) {
            this->mBrakeButton = 0.0f;
        }
        float gas_input = this->mGasButton;
        float brake_input = this->mBrakeButton;
        this->SetControlGas(gas_input);
        this->SetControlBrake(brake_input);
        this->DoShifting(shift_dir);
        this->DoAutoReverse(gas_input, brake_input);
        this->DoAutoBraking(gas_input, brake_input);
        if ((this->mVehicle != nullptr) && this->GetControls().fHandBrake != 0.0f) {
            this->SetControlBrake(0.0f);
        }
        this->SetControlSteering(steering);
        this->SetControlBanking(this->mBanking);
    }
}

void InputPlayer::DoAutoBraking(const float gas_input, const float brake_input) {
    ITransmission *itranny;
    if (!this->mVehicle->QueryInterface(&itranny) || (gas_input > 0.0f) || (brake_input > 0.0f)) {
        return;
    }
    const int current_gear = itranny->GetGear();
    float speed = this->mVehicle->GetSpeed();
    if (current_gear > G_REVERSE) {
        if (speed < 8.0f) {
            float brake_ratio = UMath::Ramp(speed, 0.0f, 8.0f);
            this->SetControlBrake(1.0f - brake_ratio * 0.75f);
        }
    } else {
        float brake_ratio = UMath::Ramp(-speed, 0.0f, 10.0f);
        this->SetControlBrake(1.0f - brake_ratio * 0.75f);
    }
}

bool InputPlayer::DoShifting(int shift_dir) {
    ITransmission *itranny;
    if ((shift_dir == 0) || (this->mVehicle == nullptr) || !this->mVehicle->QueryInterface(&itranny)) {
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
    if (this->mVehicle->IsStaging() && (desired_gear > 2)) {
        return false;
    }
    if (!this->IsAutomaticShift()) {
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
    if (this->mVehicle == nullptr) {
        return;
    }
    ITransmission *itranny;
    if (!this->mVehicle->QueryInterface(&itranny)) {
        return;
    }
    float speed = this->mVehicle->GetSpeed();
    if (itranny->GetGear() != G_REVERSE) {
        if ((speed < 2.5f) && (brake_input > 0.0f) && gas_input == 0.0f) {
            itranny->Shift(G_REVERSE);
        }
    } else if ((speed > -5.0f) && ((brake_input == 0.0f) || (gas_input > 0.0f))) {
        itranny->Shift(G_FIRST);
    }
    if (itranny->GetGear() == G_REVERSE) {
        this->SetControlGas(brake_input);
        this->SetControlBrake(gas_input);
    }
}

bool InputPlayer::IsAutomaticShift() const {
    if ((this->mHumanAI != nullptr) && this->mHumanAI->GetAiControl()) {
        return true;
    }
    IPlayer *player = this->GetOwner()->GetPlayer();
    if ((player != nullptr) && (player->GetSettings() != nullptr) && player->GetSettings()->Transmission == TRANSMISSION_MANUAL) {
        return false;
    }
    if (this->GetVehicle()->GetDriverStyle() == STYLE_DRAG) {
        return false;
    }
    return true;
}

BIND_BEHAVIOR_FACTORY(InputPlayerDrag);

InputPlayerDrag::InputPlayerDrag(const BehaviorParams &bp) : InputPlayer(bp) {}

void InputPlayerDrag::FetchInput() {
    InputPlayer::FetchInput();
}

bool InputPlayerDrag::OnAction(const ActionRef &a) {
    switch (a.ID()) {
        case GAMEACTION_TURNLEFT: {
            IHumanAI *human;
            if (this->GetOwner()->QueryInterface(&human)) {
                human->ChangeDragLanes(true);
            }
            break;
        }
        case GAMEACTION_TURNRIGHT: {
            IHumanAI *human;
            if (this->GetOwner()->QueryInterface(&human)) {
                human->ChangeDragLanes(false);
            }
            break;
        }
    }
    InputPlayer::OnAction(a);
    return true;
}

BIND_BEHAVIOR_FACTORY(InputNIS);

InputNIS::InputNIS(const BehaviorParams &bp) : PInput(bp) {}
