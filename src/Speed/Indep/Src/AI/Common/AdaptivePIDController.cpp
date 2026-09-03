#include "Speed/Indep/Src/AI/AdaptivePIDController.h"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

#define MAX_AI_FRAMERATE 45.0f      // Decl: 44
#define EXPECTED_AI_FRAMERATE 30.0f // Decl: 45

static const float MRACDefaultTimeSlice = 0.2f;           // Decl: 49
static const float MRACDefaultTuningThreshold = 0.01f;    // Decl: 50
static const float MRACDefaultCoefficientClampMin = 0.0f; // Decl: 51
static const float MRACDefaultCoefficientClampMax = 1.0f; // Decl: 52
static const float MRACDefaultTermClampMin = -99999.0f;   // Decl: 53
static const float MRACDefaultTermClampMax = 99999.0f;    // Decl: 54
static const float MRACDefaultAlpha = 1.0f;               // Decl: 55
static const float MRACDefaultAdaptationGain = 5e-5f;     // Decl: 56

static const float MRACMaxSensitivityDerivative = 1000.0f; // Decl: 58

static const bool AdaptivePIDControllerCompressAveragesTo16Bits = true; // Decl: 60

AdaptivePIDControllerBase::AdaptivePIDControllerBase(eAdaptationRule adaptation_rule, float coefficient_derivative_window)
    : ModelErrorDerivative(coefficient_derivative_window, MAX_AI_FRAMERATE) {
    this->AdaptationRule = adaptation_rule;
    this->TimeSlice = MRACDefaultTimeSlice;
    this->Alpha = MRACDefaultAlpha;
    for (int i = 0; i < NUM_PID_TERMS; i++) {
        this->pCoefficientDerivative[i] = new AverageWindow(coefficient_derivative_window, MAX_AI_FRAMERATE);

        this->CoefficientClamp[i][0] = MRACDefaultCoefficientClampMin;
        this->CoefficientClamp[i][1] = MRACDefaultCoefficientClampMax;

        this->TermClamp[i][0] = MRACDefaultTermClampMin;
        this->TermClamp[i][1] = MRACDefaultTermClampMax;

        this->TuningThreshold[i] = MRACDefaultTuningThreshold;
        this->AdaptationGain[i] = MRACDefaultAdaptationGain;
        this->Coefficient[i] = 0.0f;
    }
    this->ModelError = 0.0f;
}

AdaptivePIDControllerBase::~AdaptivePIDControllerBase() {
    for (int i = 0; i < NUM_PID_TERMS; i++) {
        delete this->pCoefficientDerivative[i];
    }
}

void AdaptivePIDControllerBase::UpdateBase(float model_error, float timestep, float desired_process_value) {
    ePIDTerm term_to_update = static_cast<ePIDTerm>(static_cast<int>(WorldTimer.GetSeconds() / this->TimeSlice) % NUM_PID_TERMS);
    float term_value = this->GetTerm(term_to_update);
    float coefficient_derivative = 0.0f;

    if (bAbs(term_value) >= this->TuningThreshold[term_to_update]) {
        coefficient_derivative = this->GetNewCoefficientDerivative(term_to_update, model_error, desired_process_value);
        this->Coefficient[term_to_update] = bClamp(coefficient_derivative * timestep + this->Coefficient[term_to_update],
                                                   this->CoefficientClamp[term_to_update][0], this->CoefficientClamp[term_to_update][1]);
    }

    float time_now = WorldTimer.GetSeconds();
    this->pCoefficientDerivative[term_to_update]->Record(coefficient_derivative, time_now);

    for (int i = 0; i < NUM_PID_TERMS; i++) {
        if (i != term_to_update) {
            this->pCoefficientDerivative[i]->Record(0.0f, time_now);
        }
    }

    float model_error_instantaneous_derivative = (model_error - this->ModelError) / bMax(timestep, 0.001f);
    this->ModelErrorDerivative.Record(model_error_instantaneous_derivative, time_now);
    this->ModelError = model_error;
}

float AdaptivePIDControllerBase::GetOutput() {
    float output = this->GetCoefficient(eP_TERM) * this->GetClampedTerm(eP_TERM) + this->GetCoefficient(eI_TERM) * this->GetClampedTerm(eI_TERM) +
                   this->GetCoefficient(eD_TERM) * this->GetClampedTerm(eD_TERM);

    return output;
}

float AdaptivePIDControllerBase::GetNewCoefficientDerivative(ePIDTerm term, float model_error, float desired_process_value) {
    float adaptation_gain = this->AdaptationGain[term];
    float sensitivity_derivative = this->GetSensitivityDerivative(term);
    switch (this->AdaptationRule) {
        case eMIT_RULE:
            return -adaptation_gain * model_error * sensitivity_derivative;
        case eNORMALIZED_MIT_RULE:
            return (-adaptation_gain * model_error * sensitivity_derivative) / (sensitivity_derivative * sensitivity_derivative + this->Alpha);
        case eSIGN_ERROR_RULE:
            return -adaptation_gain * this->Sign(model_error) * sensitivity_derivative;
        case eSIGN_SIGN_RULE:
            model_error = this->Sign(model_error);
            // fallthrough
        case eSIGN_DATA_RULE:
            return -adaptation_gain * model_error * this->Sign(sensitivity_derivative);
            break;
        case eLYAPUNOV_RULE:
            return -adaptation_gain * desired_process_value * model_error;
        default:
            break;
    }
    return 0.0f;
}

float AdaptivePIDControllerBase::GetSensitivityDerivative(float coefficient_derivative) {
    float model_error_derivative = this->ModelErrorDerivative.GetValue();

    if (!bEqual(coefficient_derivative, 0.0f, 1e-9f)) {
        return bClamp(model_error_derivative / coefficient_derivative, -MRACMaxSensitivityDerivative, MRACMaxSensitivityDerivative);
    }

    coefficient_derivative = 0.0f;

    if (bEqual(model_error_derivative, coefficient_derivative, 0.001f)) {
        return 0.0f;
    }

    if (model_error_derivative > coefficient_derivative) {
        return 1.0f;
    }

    return -1.0f;
}

float AdaptivePIDControllerBase::Sign(float v) {
    if (v == 0.0f) {
        return 0.0f;
    }
    if (v > 0.0f) {
        return 1.0f;
    }
    return -1.0f;
}

AdaptivePIDControllerSimple::AdaptivePIDControllerSimple(enum eAdaptationRule adaptation_rule, float coefficient_derivative_window,
                                                         int integral_history, int derivative_history)
    : AdaptivePIDControllerBase(adaptation_rule, coefficient_derivative_window), //
      PIDController(integral_history, derivative_history, EXPECTED_AI_FRAMERATE) {}

float AdaptivePIDControllerSimple::GetTerm(ePIDTerm term) {
    float term_value = 0.0f;
    switch (term) {
        case eP_TERM:
            term_value = this->PIDController.GetError();
            break;
        case eI_TERM:
            term_value = this->PIDController.GetErrorIntegral();
            break;
        case eD_TERM:
            term_value = this->PIDController.GetErrorDerivative();
            break;
        default:
            break;
    }
    return term_value;
}

void AdaptivePIDControllerSimple::Update(float desired_process_value, float actual_process_value, float model_behaviour_value, float timestep,
                                         eRecordingInstruction recording_instruction) {}

AdaptivePIDControllerComplicated::AdaptivePIDControllerComplicated(enum eAdaptationRule adaptation_rule, float coefficient_derivative_window)
    : AdaptivePIDControllerBase(adaptation_rule, coefficient_derivative_window) {
    for (int i = 0; i < NUM_PID_TERMS; i++) {
        this->CurrentTermValue[i] = 0.0f;
    }
}

void AdaptivePIDControllerComplicated::Update(float model_behaviour_value, float actual_behaviour_value, float timestep,
                                              float desired_process_value) {
    float model_error = actual_behaviour_value - model_behaviour_value;
    this->UpdateBase(model_error, timestep, desired_process_value);
}
