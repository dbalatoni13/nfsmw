#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/AI/AIVehicle.h"
#include "Speed/Indep/Src/AI/AdaptivePIDController.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRBVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bTypes.hpp"

static const bool bRacerSimplePhysics = false;   // Decl: 21
static const bool bAdaptivePidController = true; // Decl: 22
static const bool bAdaptiveOversteer = true;     // Decl: 23 TODO use
static const bool bAdaptiveHeading = false;      // Decl: 24 TODO use

int nThrottleIntegralTerms = 4;   // Decl: 27
int nThrottleDerivativeTerms = 4; // Decl: 28

static const int nPsychoPidIntegralTerms = 5;   // Decl: 30
static const int nPsychoPidDerivativeTerms = 5; // Decl: 31

static const float fSteeringIntegralClamp = 0.5f;    // Decl: 33
static const float fSteeringDerivativeClamp = 10.0f; // Decl: 34

static const float fThrottlePidMinSpeed = 5.0f;      // Decl: 36
static const float fThrottleIntegralClamp = 5.0f;    // Decl: 37
static const float fThrottleDerivativeClamp = 10.0f; // Decl: 38

static const float fThrottleP = 0.4f;  // Decl: 40
static const float fThrottleI = 0.01f; // Decl: 41
static const float fThrottleD = 0.1f;  // Decl: 42

static const float fAiMaxBrake = 1.0f;        // Decl: 44
static const float fAiMaxThrottle = 1.0f;     // Decl: 45
static const float fAiStagingThrottle = 0.8f; // Decl: 46

static const float fAdaptiveMRACSlice = 0.1f; // Decl: 48

// Decl: 52
bVector2 vHeadingErrorModelData[10] = {
    bVector2(0.0f, 0.0f),    bVector2(3.0f, -1.0f),   bVector2(6.0f, -2.05f),  bVector2(9.0f, -3.5f),  bVector2(12.0f, -4.94f),
    bVector2(15.0f, -6.14f), bVector2(18.0f, -7.35f), bVector2(21.0f, -8.55f), bVector2(24.0f, -9.4f), bVector2(27.0f, -10.0f),
};

// Decl: 67
Graph HeadingErrorModelGraph(vHeadingErrorModelData, NUM_ELEMENTS(vHeadingErrorModelData));

static const float SteeringPidAdaptationGainP = 1e-5f; // Decl: 69
static const float SteeringPidAdaptationGainI = 1e-5f; // Decl: 70
static const float SteeringPidAdaptationGainD = 1e-5f; // Decl: 71

static const float DragPidMaxPCoefficient = 1.2f; // Decl: 73
static const float DragPidMinPCoefficient = 0.6f; // Decl: 74

static const float DragPidMaxICoefficient = 0.5f;  // Decl: 76
static const float DragPidMinICoefficient = 0.05f; // Decl: 77

static const float DragPidMaxDCoefficient = 0.6f;  // Decl: 79
static const float DragPidMinDCoefficient = 0.02f; // Decl: 80

static const float SteeringPidMaxPCoefficient = 1.0f; // Decl: 83
static const float SteeringPidMinPCoefficient = 0.4f; // Decl: 84

static const float SteeringPidMaxICoefficient = 0.1f;  // Decl: 86
static const float SteeringPidMinICoefficient = 0.01f; // Decl: 87

static const float SteeringPidMaxDCoefficient = 0.4f; // Decl: 89
static const float SteeringPidMinDCoefficient = 0.1f; // Decl: 90

static const float SteeringPidMinValueForTermToTuneCoefficient = 0.01f; // Decl: 97
static const float SteeringPidMinSpeedForAdaptation = 10.0f;            // Decl: 98

static const float PsychoThrottleBrakeMinSpeedForAdaptation = 10.0f; // Decl: 108
static const float PsychoThottleBrakePSeed = -1.0f;                  // Decl: 109
static const float PsychoThottleBrakeISeed = 0.0f;                   // Decl: 110
static const float PsychoThottleBrakeDSeed = 0.0f;                   // Decl: 111

// Decl: 114
bVector2 vVelocityErrorModelData[9] = {
    bVector2(-20.0f, 100.0f), bVector2(-10.0f, 10.0f), bVector2(-5.0f, 7.0f),   bVector2(-2.0f, 3.0f),    bVector2(0.0f, 0.0f),
    bVector2(2.0f, 0.0f),     bVector2(10.0f, -3.0f),  bVector2(20.0f, -10.0f), bVector2(30.0f, -100.0f),
};

// Decl: 128
Graph VelocityErrorModelGraph(vVelocityErrorModelData, NUM_ELEMENTS(vVelocityErrorModelData));

static const bool bAdaptiveGasBrake = false; // Decl: 130

static const float ThrottlePidAdaptationGainP = 5e-6f; // Decl: 132
static const float ThrottlePidAdaptationGainI = 5e-6f; // Decl: 133
static const float ThrottlePidAdaptationGainD = 5e-6f; // Decl: 134

static const float ThrottlePidMaxPCoefficient = 0.0f;  // Decl: 136
static const float ThrottlePidMinPCoefficient = -0.4f; // Decl: 137

static const float ThrottlePidMaxICoefficient = 0.0f;   // Decl: 139
static const float ThrottlePidMinICoefficient = -0.01f; // Decl: 140

static const float ThrottlePidMaxDCoefficient = 0.0f;  // Decl: 142
static const float ThrottlePidMinDCoefficient = -0.6f; // Decl: 143

static const float ThrottlePidMinValueForTermToTuneCoefficient = 0.01f; // Decl: 145
static const float ThrottlePidMinSpeedForAdaptation = 10.0f;            // Decl: 146

// Decl: 149
float PidProportionalData[10] = {
    0.328f, 0.22f, 0.148f, 0.115f, 0.09f, 0.074f, 0.057f, 0.049f, 0.043f, 0.04f,
};

// Decl: 164
float PidDerivativeData[10] = {
    0.0f, 0.075829f, 0.075829f, 0.073826f, 0.060403f, 0.04698f, 0.04698f, 0.040268f, 0.040268f, 0.040268f,
};

// Decl: 179
float PidIntegralData[10] = {
    0.21f, 0.244f, 0.267f, 0.29f, 0.305f, 0.321f, 0.328f, 0.336f, 0.341f, 0.344f,
};

// Decl: 194
Table PidProportionalTable(PidProportionalData, NUM_ELEMENTS(PidProportionalData), 0.0f, 160.0f);
// Decl: 195
Table PidDerivativeTable(PidDerivativeData, NUM_ELEMENTS(PidDerivativeData), 0.0f, 160.0f);
// Decl: 196
Table PidIntegralTable(PidIntegralData, NUM_ELEMENTS(PidIntegralData), 0.0f, 160.0f);

AIVehiclePid::AIVehiclePid(const BehaviorParams &bp, float update_rate, float stagger, Sim::TaskMode taskmode)
    : AIVehicle(bp, update_rate, stagger, taskmode), //
      mThrottleBrake(0.0f),                          //
      mPrevDesiredSpeed(0.0f) {
    this->pBodyError = new PidError(nPsychoPidIntegralTerms, nPsychoPidDerivativeTerms, 30.0f);
    this->pHeadingError = new PidError(nPsychoPidIntegralTerms, nPsychoPidDerivativeTerms, 30.0f);
    this->pVelocityError = new PidError(nThrottleIntegralTerms, nThrottleDerivativeTerms, 30.0f);

    this->pSteeringController = new AdaptivePIDControllerComplicated(eMIT_RULE, 0.1f);
    this->pThrottleBrakeController = new AdaptivePIDControllerSimple(eMIT_RULE, 0.1f, nThrottleIntegralTerms, nThrottleDerivativeTerms);

    this->pThrottleBrakeController->SetTimeSlice(fAdaptiveMRACSlice);

    this->pThrottleBrakeController->SetAdaptationGain(eP_TERM, ThrottlePidAdaptationGainP);
    this->pThrottleBrakeController->SetAdaptationGain(eI_TERM, ThrottlePidAdaptationGainI);
    this->pThrottleBrakeController->SetAdaptationGain(eD_TERM, ThrottlePidAdaptationGainD);

    this->pThrottleBrakeController->SetTuningThreshold(eP_TERM, ThrottlePidMinValueForTermToTuneCoefficient);
    this->pThrottleBrakeController->SetTuningThreshold(eI_TERM, ThrottlePidMinValueForTermToTuneCoefficient);
    this->pThrottleBrakeController->SetTuningThreshold(eD_TERM, ThrottlePidMinValueForTermToTuneCoefficient);

    this->pThrottleBrakeController->SetCoefficientClamp(eP_TERM, ThrottlePidMinPCoefficient, ThrottlePidMaxPCoefficient);
    this->pThrottleBrakeController->SetCoefficientClamp(eI_TERM, ThrottlePidMinDCoefficient, ThrottlePidMaxDCoefficient);
    this->pThrottleBrakeController->SetCoefficientClamp(eD_TERM, ThrottlePidMinICoefficient, ThrottlePidMaxICoefficient);

    this->pSteeringController->SetTimeSlice(fAdaptiveMRACSlice);

    this->pSteeringController->SetAdaptationGain(eP_TERM, SteeringPidAdaptationGainP);
    this->pSteeringController->SetAdaptationGain(eI_TERM, SteeringPidAdaptationGainI);
    this->pSteeringController->SetAdaptationGain(eD_TERM, SteeringPidAdaptationGainD);

    this->pSteeringController->SetTuningThreshold(eP_TERM, SteeringPidMinValueForTermToTuneCoefficient);
    this->pSteeringController->SetTuningThreshold(eI_TERM, SteeringPidMinValueForTermToTuneCoefficient);
    this->pSteeringController->SetTuningThreshold(eD_TERM, SteeringPidMinValueForTermToTuneCoefficient);

    IVehicle *vehicle = this->GetVehicle();
    bool drag_racing = false;
    if (vehicle != nullptr && vehicle->GetDriverStyle() == STYLE_DRAG) {
        drag_racing = vehicle->GetDriverClass() == DRIVER_HUMAN;
    }

    float min_p = drag_racing ? DragPidMinPCoefficient : SteeringPidMinPCoefficient;
    float max_p = drag_racing ? DragPidMaxPCoefficient : SteeringPidMaxPCoefficient;
    float min_d = drag_racing ? DragPidMinDCoefficient : SteeringPidMinDCoefficient;
    float max_d = drag_racing ? DragPidMaxDCoefficient : SteeringPidMaxDCoefficient;
    float min_i = drag_racing ? DragPidMinICoefficient : SteeringPidMinICoefficient;
    float max_i = drag_racing ? DragPidMaxICoefficient : SteeringPidMaxICoefficient;

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
        this->mThrottleBrake = fAiStagingThrottle;
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
            } else {
                if (bAdaptiveGasBrake) {
                    this->pThrottleBrakeController->SetTimeSlice(fAdaptiveMRACSlice);

                    this->pThrottleBrakeController->SetAdaptationGain(eP_TERM, ThrottlePidAdaptationGainP);
                    this->pThrottleBrakeController->SetAdaptationGain(eI_TERM, ThrottlePidAdaptationGainI);
                    this->pThrottleBrakeController->SetAdaptationGain(eD_TERM, ThrottlePidAdaptationGainD);

                    this->pThrottleBrakeController->SetTuningThreshold(eP_TERM, ThrottlePidMinValueForTermToTuneCoefficient);
                    this->pThrottleBrakeController->SetTuningThreshold(eI_TERM, ThrottlePidMinValueForTermToTuneCoefficient);
                    this->pThrottleBrakeController->SetTuningThreshold(eD_TERM, ThrottlePidMinValueForTermToTuneCoefficient);

                    this->pThrottleBrakeController->SetCoefficientClamp(eP_TERM, ThrottlePidMinPCoefficient, ThrottlePidMaxPCoefficient);
                    this->pThrottleBrakeController->SetCoefficientClamp(eI_TERM, ThrottlePidMinDCoefficient, ThrottlePidMaxDCoefficient);
                    this->pThrottleBrakeController->SetCoefficientClamp(eD_TERM, ThrottlePidMinICoefficient, ThrottlePidMaxICoefficient);

                    // TODO use these
                    float fThrottleBrake;
                    float fDelta;
                    float speed_difference;
                    eRecordingInstruction recording_instruction;

                    this->pThrottleBrakeController->Update(bAbs(desiredSpeed), currentSpeed, VelocityErrorModelGraph.GetValue(speed_error), dT,
                                                           recording_instruction);

                    if (currentSpeed < PsychoThrottleBrakeMinSpeedForAdaptation) {
                        this->pThrottleBrakeController->ForceCoefficient(eP_TERM, PsychoThottleBrakePSeed);
                        this->pThrottleBrakeController->ForceCoefficient(eI_TERM, PsychoThottleBrakeISeed);
                        this->pThrottleBrakeController->ForceCoefficient(eD_TERM, PsychoThottleBrakeDSeed);
                    }

                    this->mThrottleBrake += this->pThrottleBrakeController->GetOutput();
                } else {
                    float speed_error_integral = bClamp(this->pVelocityError->GetErrorIntegral(), -fThrottleIntegralClamp, fThrottleIntegralClamp);
                    float speed_error_derivative =
                        bClamp(this->pVelocityError->GetErrorDerivative(), -fThrottleDerivativeClamp, fThrottleDerivativeClamp);

                    float p = speed_error * -fThrottleP;
                    float i = speed_error_integral * -fThrottleI;
                    float d = speed_error_derivative * -fThrottleD;

                    this->mThrottleBrake += p + i + d;
                }
            }
        }
    }
    this->mPrevDesiredSpeed = this->mDriveSpeed;
    this->mThrottleBrake = UMath::Clamp(this->mThrottleBrake, -1.0f, 1.0f);
    input->SetControlGas(bClamp(this->mThrottleBrake, 0.0f, fAiMaxThrottle));
    input->SetControlBrake(bClamp(-this->mThrottleBrake, 0.0f, fAiMaxBrake));
}

void AIVehiclePid::OnSteering(float dT) {
    bool drag_racing = this->GetVehicle()->GetDriverStyle() == STYLE_DRAG;
    bool adaptive_pid = this->GetReverseOverride();

    if (adaptive_pid) {
        this->Base::OnSteering(dT);
        return;
    }

    if ((this->GetDriveFlags() & 1) == 0) {
        return;
    }

    ISuspension *suspension = this->GetSuspension();
    if (this->GetInput() == nullptr || suspension == nullptr) {
        return;
    }

    this->GetInput()->SetControlSteering(0.0f);
    this->GetInput()->SetControlSteeringVertical(0.0f);

    ISimable *simable = this->GetSimable();
    IRigidBody *rigid_body = simable->GetRigidBody();
    float currentSpeed = rigid_body->GetSpeedXZ();

    if (this->mDriveSpeed == 0.0f && currentSpeed < 1.0f) {
        return;
    }

    UMath::Vector3 dirVector;
    UMath::Sub(this->mDest, simable->GetPosition(), dirVector);
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

    float velocity_blend = UMath::Clamp(currentSpeed, 0.0f, 1.0f);
    float forward_blend = 1.0f - velocity_blend;

    UMath::Vector3 heading;
    UMath::Scale(forwardVector, forward_blend, heading);
    UMath::ScaleAdd(velocity, velocity_blend, heading, heading);
    UMath::Unit(heading, heading);

    UMath::Vector3 steerProd;
    UMath::Cross(forwardVector, dirVector, steerProd);
    this->pBodyError->Record(UMath::ASinr(UMath::Bound(steerProd.y, 1.0f)), dT, false, false);

    UMath::Vector3 headingProd;
    UMath::Cross(heading, dirVector, headingProd);
    this->pHeadingError->Record(UMath::ASinr(UMath::Bound(headingProd.y, 1.0f)), dT, false, false);

    float body_blend = 1.0f;
    float heading_blend = 0.0f;
    float body_error = this->pBodyError->GetError();
    float heading_error = this->pHeadingError->GetError();

    {
        float body_error_squared = body_error * body_error;
        float heading_error_squared = heading_error * heading_error;
        float total_error_squared = body_error_squared + heading_error_squared;
    }

    float angle_error = body_blend * body_error + heading_blend * heading_error;
    float angle_error_integral = bClamp(body_blend * this->pBodyError->GetErrorIntegral() + heading_blend * this->pHeadingError->GetErrorIntegral(),
                                        -fSteeringIntegralClamp, fSteeringIntegralClamp);
    float angle_error_derivative =
        bClamp(body_blend * this->pBodyError->GetErrorDerivative() + heading_blend * this->pHeadingError->GetErrorDerivative(),
               -fSteeringDerivativeClamp, fSteeringDerivativeClamp);

    bool error_growing = angle_error * angle_error_derivative >= 0.0f;
    float angle_error_abs_derivative = error_growing ? bAbs(angle_error_derivative) : -bAbs(angle_error_derivative);

    this->pSteeringController->SetTerm(eP_TERM, angle_error);
    this->pSteeringController->SetTerm(eI_TERM, angle_error_integral);
    this->pSteeringController->SetTerm(eD_TERM, angle_error_derivative);

    float model_behaviour_value = HeadingErrorModelGraph.GetValue(bAbs(bRadToDeg(angle_error)));
    float actual_behaviour_value = bRadToDeg(angle_error_abs_derivative);
    this->pSteeringController->Update(model_behaviour_value, actual_behaviour_value, dT, VALUE_NOT_SET);

    if (currentSpeed < SteeringPidMinSpeedForAdaptation) {
        float i_coefficient = PidIntegralTable.GetValue(currentSpeed);
        float p_coefficient = PidProportionalTable.GetValue(currentSpeed);
        float d_coefficient = PidDerivativeTable.GetValue(currentSpeed);

        this->pSteeringController->ForceCoefficient(eP_TERM, p_coefficient);
        this->pSteeringController->ForceCoefficient(eI_TERM, i_coefficient);
        this->pSteeringController->ForceCoefficient(eD_TERM, d_coefficient);
    }

    float steer = this->pSteeringController->GetOutput() / ANGLE2RAD(suspension->GetMaxSteering());
    float steerCorrection = this->GetOverSteerCorrection(steer);

    this->mSteeringBehind = false;
    if (this->GetTransmission() != nullptr && this->GetTransmission()->IsReversing()) {
        if (this->GetVehicle()->GetSpeed() < 0.0f) {
            steer = steer < 0.0f ? 1.0f : -1.0f;
        } else {
            steer = 0.0f;
        }
    }

    steer = UMath::Clamp(steer, -1.0f, 1.0f);
    this->GetInput()->SetControlSteering(steer);
}

bool bSpawnRacer = false;  // Decl: 13
HSIMABLE gRacer = nullptr; // Decl: 14

extern bool LOCK_TO_30;           // Decl: 16
extern bool Tweak_ForceICEReplay; // Decl: 17

bool bAiDriveReplay = false;    // Decl: 19
bool bAiDriveReplayOld = false; // Decl: 20

// STRIPPED
void MaybeSpawnRacer() {}

AIVehicleRacecar::AIVehicleRacecar(const BehaviorParams &bp)
    : AIPerpVehicle(bp), //
      IRacer(bp.fowner) {
    this->SetGoal(UCrc32("AIGoalRacer"));
}

AIVehicleRacecar::~AIVehicleRacecar() {}

static const float Tweak_DragsterStartLookAhead = 30.0f; // Decl: 108
static const float Tweak_RacerStartLookAhead = 60.0f;    // Decl: 109

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
                nav->IncNavPosition(Tweak_DragsterStartLookAhead, this->GetForwardVector(), 0.0f);
            } else {
                nav->IncNavPosition(Tweak_RacerStartLookAhead, this->GetForwardVector(), 0.0f);
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

BIND_BEHAVIOR_FACTORY(AIVehicleRacecar);

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

    AIPerpVehicle::Update(dT);
    this->UpdateSpawnTimer(dT);
    this->UpdateReverseOverride(dT);
    this->UpdateTargeting();

    if (this->GetGoal() != nullptr) {
        this->GetGoal()->Update(dT);
    }
}
