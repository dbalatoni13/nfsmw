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

    IVehicle *vehicle = GetVehicle();
    bool drag_racing = false;
    if (vehicle && vehicle->GetDriverStyle() == STYLE_DRAG) {
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
    delete pBodyError;
    delete pHeadingError;
    delete pVelocityError;
    delete pSteeringController;
    delete pThrottleBrakeController;
}

void AIVehiclePid::Reset() {
    mThrottleBrake = 0.0f;
    AIVehicle::Reset();
}

void AIVehiclePid::OnGasBrake(float dT) {
    // TODO magic
    if (!(GetDriveFlags() & 2)) {
        return;
    }
    IInput *input = GetInput();
    if (!input) {
        return;
    }
    input->SetControlGas(0.0f);
    input->SetControlBrake(0.0f);
    input->SetControlHandBrake(0.0f);
    input->SetControlSteeringVertical(0.0f);

    if (GetVehicle()->IsStaging()) {
        mThrottleBrake = 0.8f;
    } else if (!mReversingSpeed && mSteeringBehind) {
        mThrottleBrake = 1.0f;
        input->SetControlHandBrake(1.0f);
    } else {
        bool reversing = false;
        if (GetTransmission() && GetTransmission()->IsReversing()) {
            reversing = true;
        }
        float currentSpeed = GetVehicle()->GetSpeed();
        float desiredSpeed = mDriveSpeed;
        float speed_error = currentSpeed - mDriveSpeed;

        pVelocityError->Record(speed_error, dT, false, false);

        if (desiredSpeed < 0.5f) {
            mThrottleBrake = -1.0f;
        } else {
            if (reversing) {
                if (currentSpeed > 1.0f) {
                    mThrottleBrake = -1.0f;
                } else {
                    mThrottleBrake = 1.0f;
                }
            } else if (currentSpeed < -1.0f) {
                mThrottleBrake = -1.0f;
                // one of the branches has lots of code that didn't make it
            } else {
                float speed_error_integral = bClamp(pVelocityError->GetErrorIntegral(), -5.0f, 5.0f);
                float speed_error_derivative = bClamp(pVelocityError->GetErrorDerivative(), -10.0f, 10.0f);

                float p = speed_error * -0.4f;
                float i = speed_error_integral * -0.01f;
                float d = speed_error_derivative * -0.1f;

                mThrottleBrake += p + i + d;
            }
        }
    }
    mPrevDesiredSpeed = mDriveSpeed;
    mThrottleBrake = UMath::Clamp(mThrottleBrake, -1.0f, 1.0f);
    input->SetControlGas(bClamp(mThrottleBrake, 0.0f, 1.0f));
    input->SetControlBrake(bClamp(-mThrottleBrake, 0.0f, 1.0f));
}

// void AIVehiclePid::OnSteering(float dT) {}

AIVehicleRacecar::AIVehicleRacecar(const BehaviorParams &bp)
    : AIPerpVehicle(bp), //
      IRacer(bp.fowner) {
    SetGoal("AIGoalRacer");
}

AIVehicleRacecar::~AIVehicleRacecar() {}

void AIVehicleRacecar::StartRace(DriverStyle style) {
    ClearGoal();
    GetVehicle()->SetDriverStyle(style);
    SetGoal("AIGoalRacer");

    IInputPlayer *input;
    if (GetOwner()->QueryInterface(&input)) {
        input->BlockInput(false);
    }

    IRBVehicle *vehiclebody;
    if (GetOwner()->QueryInterface(&vehiclebody)) {
        vehiclebody->SetInvulnerability(INVULNERABLE_NONE, 0.0f);
        vehiclebody->EnableObjectCollisions(true);
    }

    WRoadNav *nav = GetDriveToNav();
    if (nav) {
        if (style == STYLE_DRAG) {
            nav->SetLaneType(WRoadNav::kLaneDrag);
        } else {
            nav->SetLaneType(WRoadNav::kLaneRacing);
        }
        nav->SetRaceFilter(true);
        nav->CancelPathFinding();
        nav->SetNavType(WRoadNav::kTypeDirection);
        ResetDriveToNav(SELECT_VALID_LANE);

        if (nav->IsValid()) {
            AITarget *target = GetTarget();
            if (target->IsValid()) {
                nav->FindPath(&target->GetPosition(), &target->GetDirection(), nullptr);
            }

            if (style == STYLE_DRAG) {
                nav->IncNavPosition(30.0f, GetForwardVector(), 0.0f);
            } else {
                nav->IncNavPosition(60.0f, GetForwardVector(), 0.0f);
            }

            nav->UpdateOccludedPosition(false);
        }
    }
}

void AIVehicleRacecar::QuitRace() {
    GetVehicle()->SetDriverStyle(STYLE_RACING);

    IInputPlayer *input;
    if (GetOwner()->QueryInterface(&input)) {
        input->BlockInput(false);
    }

    AITarget *target = GetTarget();
    if (target) {
        target->Clear();
    }

    IRBVehicle *vehiclebody;
    if (GetOwner()->QueryInterface(&vehiclebody)) {
        vehiclebody->SetInvulnerability(INVULNERABLE_NONE, 0.0f);
        vehiclebody->EnableObjectCollisions(1);
    }

    WRoadNav *nav = GetDriveToNav();
    if (nav) {
        nav->SetNavType(WRoadNav::kTypeDirection);
        nav->SetLaneType(WRoadNav::kLaneRacing);
        nav->SetRaceFilter(false);
        nav->CancelPathFinding();
        ResetDriveToNav(SELECT_VALID_LANE);
    }
}

void AIVehicleRacecar::PrepareForRace(const RacePreparationInfo &rpi) {
    ClearGoal();
    ComputeSkill();

    IVehicle *vehicle = GetVehicle();
    vehicle->Activate();
    vehicle->SetVehicleOnGround(rpi.Position, rpi.Direction);

    ClearReverseOverride();
    vehicle->SetSpeed(rpi.Speed);
    vehicle->ForceStopOff(vehicle->GetForceStop());

    SetHeat(UMath::Max(rpi.HeatLevel, 1.0f));

    bool valid_start_position = rpi.Flags & 1;
    if (valid_start_position) {
        IDamageable *damageable;
        if (GetOwner()->QueryInterface(&damageable)) {
            damageable->ResetDamage();
        }
    }

    IEngine *engine;
    if (GetOwner()->QueryInterface(&engine)) {
        engine->ChargeNOS(1.0f);
    }

    IPlayer *player = GetOwner()->GetPlayer();
    if (player) {
        player->ResetGameBreaker(true);
    }
}

Behavior *AIVehicleRacecar::Construct(const BehaviorParams &bp) {
    return new AIVehicleRacecar(bp);
}

// Functionally matching
bool AIVehicleRacecar::ShouldDoSimplePhysics() const {
    if (GetVehicle()->IsAnimating())
        return false;
    if (GetVehicle()->IsStaging())
        return false;
    if (GetOwner()->IsPlayer())
        return false;
    if (GetVehicle()->IsOffWorld())
        return true;

    return false;
}

void AIVehicleRacecar::Update(float dT) {
    ISimable *simable = GetSimable();
    bool have_simple_physics = IsSimplePhysicsActive();
    bool want_simple_physics = ShouldDoSimplePhysics();

    if (want_simple_physics) {
        if (!have_simple_physics) {
            EnableSimplePhysics();
        }
    } else if (have_simple_physics) {
        DisableSimplePhysics();
    }

    AIVehicle::Update(dT);
    UpdateSpawnTimer(dT);
    UpdateReverseOverride(dT);
    UpdateTargeting();

    if (GetGoal()) {
        GetGoal()->Update(dT);
    }
}
