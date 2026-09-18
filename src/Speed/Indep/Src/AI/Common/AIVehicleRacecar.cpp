#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/AI/AIVehicle.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRBVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/Itransmission.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

int nThrottleIntegralTerms = 4;
int nThrottleDerivativeTerms = 4;

bVector2 vHeadingErrorModelData[] = {
    bVector2(0.0f, 0.0f), bVector2(3.0f, -1.0f), bVector2(6.0f, -2.049999952316284f),
    bVector2(9.0f, -3.5f), bVector2(12.0f, -4.940000057220459f), bVector2(15.0f, -6.139999866485596f),
    bVector2(18.0f, -7.349999904632568f), bVector2(21.0f, -8.550000190734863f), bVector2(24.0f, -9.399999618530273f),
    bVector2(27.0f, -10.0f),
};
Graph HeadingErrorModelGraph(vHeadingErrorModelData, 10);

bVector2 vVelocityErrorModelData[] = {
    bVector2(-20.0f, 100.0f), bVector2(-10.0f, 10.0f), bVector2(-5.0f, 7.0f),
    bVector2(-2.0f, 3.0f), bVector2(0.0f, 0.0f), bVector2(2.0f, 0.0f),
    bVector2(10.0f, -3.0f), bVector2(20.0f, -10.0f), bVector2(30.0f, -100.0f),
};
Graph VelocityErrorModelGraph(vVelocityErrorModelData, 9);

float PidProportionalData[10] = {0.328000009059906f, 0.2199999988079071f, 0.14800000190734863f, 0.11500000208616257f,
                                 0.09000000357627869f, 0.07400000095367432f, 0.05700000002980232f, 0.04899999871850014f,
                                 0.0430000014603138f, 0.03999999910593033f};
Table PidProportionalTable(PidProportionalData, 10, 0.0f, 160.0f);
float PidDerivativeData[10] = {0.0f, 0.07582899928092957f, 0.07582899928092957f, 0.07382600009441376f,
                               0.0604030005633831f, 0.04698000103235245f, 0.04698000103235245f, 0.04026800021529198f,
                               0.04026800021529198f, 0.04026800021529198f};
Table PidDerivativeTable(PidDerivativeData, 10, 0.0f, 160.0f);
float PidIntegralData[10] = {0.20999999344348907f, 0.24400000274181366f, 0.2669999897480011f, 0.28999999165534973f,
                             0.3050000071525574f, 0.32100000977516174f, 0.328000009059906f, 0.335999995470047f,
                             0.3409999907016754f, 0.3440000116825104f};
Table PidIntegralTable(PidIntegralData, 10, 0.0f, 160.0f);

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

void AIVehiclePid::OnSteering(float dT) {
    bool drag_racing = GetVehicle()->GetDriverStyle() == STYLE_DRAG;
    bool adaptive_pid = false;

    if (GetReverseOverride()) {
        AIVehicle::OnSteering(dT);
        return;
    }

    if ((GetDriveFlags() & 1) == 0) {
        return;
    }

    ISuspension *suspension = GetSuspension();
    if (!GetInput() || !suspension) {
        return;
    }

    GetInput()->SetControlSteering(0.0f);
    GetInput()->SetControlSteeringVertical(0.0f);

    ISimable *simable = GetSimable();
    IRigidBody *rigid_body = simable->GetRigidBody();

    float currentSpeed = rigid_body->GetSpeedXZ();
    if (mDriveSpeed == 0.0f && currentSpeed < 1.0f) {
        return;
    }

    UMath::Vector3 dirVector;
    UMath::Sub(mDest, simable->GetPosition(), dirVector);
    dirVector.y = 0.0f;
    UMath::Unit(dirVector, dirVector);

    UMath::Vector3 forwardVector;
    rigid_body->GetForwardVector(forwardVector);
    forwardVector.y = 0.0f;
    UMath::Unit(forwardVector, forwardVector);

    UMath::Vector3 velocity = rigid_body->GetLinearVelocity();
    velocity.y = 0.0f;
    if (currentSpeed > 0.01f) {
        UMath::Unit(velocity, velocity);
    }

    UMath::Vector3 heading;
    float velocity_blend = UMath::Clamp(currentSpeed, 0.0f, 1.0f);
    float forward_blend = 1.0f - velocity_blend;
    UMath::Scale(forwardVector, forward_blend, heading);
    UMath::ScaleAdd(velocity, velocity_blend, heading, heading);
    UMath::Unit(heading, heading);

    UMath::Vector3 steerProd;
    UMath::Vector3 headingProd;
    float body_blend = 1.0f;
    float heading_blend = 0.0f;

    UMath::Cross(forwardVector, dirVector, steerProd);
    pBodyError->Record(UMath::ASinr(UMath::Bound(steerProd.y, 1.0f)), dT, false, false);

    UMath::Cross(heading, dirVector, headingProd);
    pHeadingError->Record(UMath::ASinr(UMath::Bound(headingProd.y, 1.0f)), dT, false, false);

    if (adaptive_pid) {
        float body_error_squared = pBodyError->GetError() * pBodyError->GetError();
        float heading_error_squared = pHeadingError->GetError() * pHeadingError->GetError();
        float total_error_squared = body_error_squared + heading_error_squared;
        if (total_error_squared > 0.0f) {
            body_blend = body_error_squared / total_error_squared;
            heading_blend = heading_error_squared / total_error_squared;
        } else {
            body_blend = 0.5f;
            heading_blend = 0.5f;
        }
    }

    float angle_error = pBodyError->GetError();
    float angle_error_integral =
        bClamp(body_blend * pBodyError->GetErrorIntegral() + heading_blend * pHeadingError->GetErrorIntegral(), -0.5f, 0.5f);
    float angle_error_derivative =
        bClamp(body_blend * pBodyError->GetErrorDerivative() + heading_blend * pHeadingError->GetErrorDerivative(), -10.0f, 10.0f);

    bool error_growing = angle_error * angle_error_derivative >= 0.0f;
    float angle_error_abs_derivative = error_growing ? bAbs(angle_error_derivative) : -bAbs(angle_error_derivative);

    pSteeringController->SetTerm(eP_TERM, angle_error);
    pSteeringController->SetTerm(eI_TERM, angle_error_integral);
    pSteeringController->SetTerm(eD_TERM, angle_error_derivative);

    float model_behaviour_value = HeadingErrorModelGraph.GetValue(bAbs(bRadToDeg(angle_error)));
    float actual_behaviour_value = bRadToDeg(angle_error_abs_derivative);
    pSteeringController->Update(model_behaviour_value, actual_behaviour_value, dT, -99999.0f);

    if (currentSpeed < 10.0f) {
        float i_coefficient = PidIntegralTable.GetValue(currentSpeed);
        float p_coefficient = PidProportionalTable.GetValue(currentSpeed);
        float d_coefficient = PidDerivativeTable.GetValue(currentSpeed);
        pSteeringController->ForceCoefficient(eP_TERM, p_coefficient);
        pSteeringController->ForceCoefficient(eI_TERM, i_coefficient);
        pSteeringController->ForceCoefficient(eD_TERM, d_coefficient);
    }

    float steer = pSteeringController->GetOutput() / ANGLE2RAD(suspension->GetMaxSteering());
    float steerCorrection = GetOverSteerCorrection(steer);

    mSteeringBehind = false;
    if (GetTransmission() && GetTransmission()->IsReversing()) {
        if (GetVehicle()->GetSpeed() < 0.0f) {
            steer = (steer < 0.0f) ? 1.0f : -1.0f;
        } else {
            steer = 0.0f;
        }
    }

    steer = UMath::Clamp(steer, -1.0f, 1.0f);
    GetInput()->SetControlSteering(steer);
}

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

BIND_BEHAVIOR_FACTORY(AIVehicleRacecar)


// Functionally matching
bool AIVehicleRacecar::ShouldDoSimplePhysics() const {
    if (GetVehicle()->IsAnimating() || GetVehicle()->IsStaging() || GetOwner()->IsPlayer())
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

    AIPerpVehicle::Update(dT);
    UpdateSpawnTimer(dT);
    UpdateReverseOverride(dT);
    UpdateTargeting();

    if (GetGoal()) {
        GetGoal()->Update(dT);
    }
}
