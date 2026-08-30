#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/AI/AIVehicle.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRBVehicle.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

int nThrottleIntegralTerms = 4;
int nThrottleDerivativeTerms = 4;

AIVehiclePid::AIVehiclePid(const BehaviorParams &bp, float update_rate, float stagger, Sim::TaskMode taskmode)
    : AIVehicle(bp, update_rate, stagger, taskmode), //
      mThrottleBrake(0.0f),                          //
      mPrevDesiredSpeed(0.0f) {
    this->pBodyError = new PidError(5, 5, 30.0f);
    this->pHeadingError = new PidError(5, 5, 30.0f);
    this->pVelocityError = new PidError(nThrottleIntegralTerms, nThrottleDerivativeTerms, 30.0f);

    this->pSteeringController = new AdaptivePIDControllerComplicated(eMIT_RULE, 0.1f);
    this->pThrottleBrakeController = new AdaptivePIDControllerSimple(eMIT_RULE, 0.1f, nThrottleIntegralTerms, nThrottleDerivativeTerms);

    this->pThrottleBrakeController->SetTimeSlice(0.1f);

    this->pThrottleBrakeController->SetAdaptationGain(eP_TERM, 5e-06f);
    this->pThrottleBrakeController->SetAdaptationGain(eI_TERM, 5e-06f);
    this->pThrottleBrakeController->SetAdaptationGain(eD_TERM, 5e-06f);

    this->pThrottleBrakeController->SetTuningThreshold(eP_TERM, 0.01f);
    this->pThrottleBrakeController->SetTuningThreshold(eI_TERM, 0.01f);
    this->pThrottleBrakeController->SetTuningThreshold(eD_TERM, 0.01f);

    this->pThrottleBrakeController->SetCoefficientClamp(eP_TERM, -0.4f, 0.0f);
    this->pThrottleBrakeController->SetCoefficientClamp(eI_TERM, -0.6f, 0.0f);
    this->pThrottleBrakeController->SetCoefficientClamp(eD_TERM, -0.01f, 0.0f);

    this->pSteeringController->SetTimeSlice(0.1f);

    this->pSteeringController->SetAdaptationGain(eP_TERM, 1e-05f);
    this->pSteeringController->SetAdaptationGain(eI_TERM, 1e-05f);
    this->pSteeringController->SetAdaptationGain(eD_TERM, 1e-05f);

    this->pSteeringController->SetTuningThreshold(eP_TERM, 0.01f);
    this->pSteeringController->SetTuningThreshold(eI_TERM, 0.01f);
    this->pSteeringController->SetTuningThreshold(eD_TERM, 0.01f);

    IVehicle *vehicle = this->GetVehicle();
    bool drag_racing = false;
    if (vehicle != nullptr && vehicle->GetDriverStyle() == STYLE_DRAG) {
        drag_racing = vehicle->GetDriverClass() == DRIVER_HUMAN;
    }

    float min_p = drag_racing ? 0.6f : 0.4f;
    float max_p = drag_racing ? 1.2f : 1.0f;
    float min_d = drag_racing ? 0.02f : 0.1f;
    float max_d = drag_racing ? 0.6f : 0.4f;
    float min_i = drag_racing ? 0.05f : 0.01f;
    float max_i = drag_racing ? 0.5f : 0.1f;

    this->pSteeringController->SetCoefficientClamp(eP_TERM, min_p, max_p);
    this->pSteeringController->SetCoefficientClamp(eI_TERM, min_i, max_i);
    this->pSteeringController->SetCoefficientClamp(eD_TERM, min_d, max_d);
}

AIVehiclePid::~AIVehiclePid() {
    delete this->pBodyError;
    delete this->pHeadingError;
    delete this->pVelocityError;
    delete this->pSteeringController;
    delete this->pThrottleBrakeController;
}

void AIVehiclePid::Reset() {
    this->mThrottleBrake = 0.0f;
    this->AIVehicle::Reset();
}

void AIVehiclePid::OnGasBrake(float dT) {
    // TODO magic
    if (!(this->GetDriveFlags() & 2)) {
        return;
    }
    IInput *input = this->GetInput();
    if (input == nullptr) {
        return;
    }
    input->SetControlGas(0.0f);
    input->SetControlBrake(0.0f);
    input->SetControlHandBrake(0.0f);
    input->SetControlSteeringVertical(0.0f);

    if (this->GetVehicle()->IsStaging()) {
        this->mThrottleBrake = 0.8f;
    } else if (!this->mReversingSpeed && this->mSteeringBehind) {
        this->mThrottleBrake = 1.0f;
        input->SetControlHandBrake(1.0f);
    } else {
        bool reversing = false;
        if (this->GetTransmission() != nullptr && this->GetTransmission()->IsReversing()) {
            reversing = true;
        }
        float currentSpeed = this->GetVehicle()->GetSpeed();
        float desiredSpeed = this->mDriveSpeed;
        float speed_error = currentSpeed - this->mDriveSpeed;

        this->pVelocityError->Record(speed_error, dT, false, false);

        if (desiredSpeed < 0.5f) {
            this->mThrottleBrake = -1.0f;
        } else {
            if (reversing) {
                if (currentSpeed > 1.0f) {
                    this->mThrottleBrake = -1.0f;
                } else {
                    this->mThrottleBrake = 1.0f;
                }
            } else if (currentSpeed < -1.0f) {
                this->mThrottleBrake = -1.0f;
                // one of the branches has lots of code that didn't make it
            } else {
                float speed_error_integral = bClamp(this->pVelocityError->GetErrorIntegral(), -5.0f, 5.0f);
                float speed_error_derivative = bClamp(this->pVelocityError->GetErrorDerivative(), -10.0f, 10.0f);

                float p = speed_error * -0.4f;
                float i = speed_error_integral * -0.01f;
                float d = speed_error_derivative * -0.1f;

                this->mThrottleBrake += p + i + d;
            }
        }
    }
    this->mPrevDesiredSpeed = this->mDriveSpeed;
    this->mThrottleBrake = UMath::Clamp(this->mThrottleBrake, -1.0f, 1.0f);
    input->SetControlGas(bClamp(this->mThrottleBrake, 0.0f, 1.0f));
    input->SetControlBrake(bClamp(-this->mThrottleBrake, 0.0f, 1.0f));
}

// void AIVehiclePid::OnSteering(float dT) {}

AIVehicleRacecar::AIVehicleRacecar(const BehaviorParams &bp)
    : AIPerpVehicle(bp), //
      IRacer(bp.fowner) {
    this->SetGoal(UCrc32("AIGoalRacer"));
}

AIVehicleRacecar::~AIVehicleRacecar() {}

void AIVehicleRacecar::StartRace(DriverStyle style) {
    this->ClearGoal();
    this->GetVehicle()->SetDriverStyle(style);
    this->SetGoal(UCrc32("AIGoalRacer"));

    IInputPlayer *input;
    if (this->GetOwner()->QueryInterface(&input)) {
        input->BlockInput(false);
    }

    IRBVehicle *vehiclebody;
    if (this->GetOwner()->QueryInterface(&vehiclebody)) {
        vehiclebody->SetInvulnerability(INVULNERABLE_NONE, 0.0f);
        vehiclebody->EnableObjectCollisions(true);
    }

    WRoadNav *nav = this->GetDriveToNav();
    if (nav != nullptr) {
        if (style == STYLE_DRAG) {
            nav->SetLaneType(WRoadNav::kLaneDrag);
        } else {
            nav->SetLaneType(WRoadNav::kLaneRacing);
        }
        nav->SetRaceFilter(true);
        nav->CancelPathFinding();
        nav->SetNavType(WRoadNav::kTypeDirection);
        this->ResetDriveToNav(SELECT_VALID_LANE);

        if (nav->IsValid()) {
            AITarget *target = this->GetTarget();
            if (target->IsValid()) {
                nav->FindPath(&target->GetPosition(), &target->GetDirection(), nullptr);
            }

            if (style == STYLE_DRAG) {
                nav->IncNavPosition(30.0f, this->GetForwardVector(), 0.0f);
            } else {
                nav->IncNavPosition(60.0f, this->GetForwardVector(), 0.0f);
            }

            nav->UpdateOccludedPosition(false);
        }
    }
}

void AIVehicleRacecar::QuitRace() {
    this->GetVehicle()->SetDriverStyle(STYLE_RACING);

    IInputPlayer *input;
    if (this->GetOwner()->QueryInterface(&input)) {
        input->BlockInput(false);
    }

    AITarget *target = this->GetTarget();
    if (target != nullptr) {
        target->Clear();
    }

    IRBVehicle *vehiclebody;
    if (this->GetOwner()->QueryInterface(&vehiclebody)) {
        vehiclebody->SetInvulnerability(INVULNERABLE_NONE, 0.0f);
        vehiclebody->EnableObjectCollisions(true);
    }

    WRoadNav *nav = this->GetDriveToNav();
    if (nav != nullptr) {
        nav->SetNavType(WRoadNav::kTypeDirection);
        nav->SetLaneType(WRoadNav::kLaneRacing);
        nav->SetRaceFilter(false);
        nav->CancelPathFinding();
        this->ResetDriveToNav(SELECT_VALID_LANE);
    }
}

void AIVehicleRacecar::PrepareForRace(const RacePreparationInfo &rpi) {
    this->ClearGoal();
    this->ComputeSkill();

    IVehicle *vehicle = this->GetVehicle();
    vehicle->Activate();
    vehicle->SetVehicleOnGround(rpi.Position, rpi.Direction);

    this->ClearReverseOverride();
    vehicle->SetSpeed(rpi.Speed);
    vehicle->ForceStopOff(vehicle->GetForceStop());

    this->SetHeat(UMath::Max(rpi.HeatLevel, 1.0f));

    bool valid_start_position = (rpi.Flags & 1) != 0;
    if (valid_start_position) {
        IDamageable *damageable;
        if (this->GetOwner()->QueryInterface(&damageable)) {
            damageable->ResetDamage();
        }
    }

    IEngine *engine;
    if (this->GetOwner()->QueryInterface(&engine)) {
        engine->ChargeNOS(1.0f);
    }

    IPlayer *player = this->GetOwner()->GetPlayer();
    if (player != nullptr) {
        player->ResetGameBreaker(true);
    }
}

Behavior *AIVehicleRacecar::Construct(const BehaviorParams &bp) {
    return new AIVehicleRacecar(bp);
}

bool AIVehicleRacecar::ShouldDoSimplePhysics() const {
    if (this->GetVehicle()->IsAnimating() || this->GetVehicle()->IsStaging() || this->GetOwner()->IsPlayer()) {
        return false;
    }

    if (this->GetVehicle()->IsOffWorld()) {
        return true;
    }

    return false;
}

void AIVehicleRacecar::Update(float dT) {
    ISimable *simable = this->GetSimable();
    bool have_simple_physics = this->IsSimplePhysicsActive();
    bool want_simple_physics = this->ShouldDoSimplePhysics();

    if (want_simple_physics) {
        if (!have_simple_physics) {
            this->EnableSimplePhysics();
        }
    } else if (have_simple_physics) {
        this->DisableSimplePhysics();
    }

    this->AIPerpVehicle::Update(dT);
    this->UpdateSpawnTimer(dT);
    this->UpdateReverseOverride(dT);
    this->UpdateTargeting();

    if (this->GetGoal() != nullptr) {
        this->GetGoal()->Update(dT);
    }
}
