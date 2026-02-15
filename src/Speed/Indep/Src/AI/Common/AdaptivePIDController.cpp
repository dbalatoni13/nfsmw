#include "Speed/Indep/Src/AI/AdaptivePIDController.h"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

AdaptivePIDControllerBase::AdaptivePIDControllerBase(eAdaptationRule adaptation_rule, float coefficient_derivative_window)
    : ModelErrorDerivative(coefficient_derivative_window, 45.0f) {
    AdaptationRule = adaptation_rule;
    TimeSlice = 0.2f;
    Alpha = 1.0f;
    for (int i = 0; i < NUM_PID_TERMS; i++) {
        pCoefficientDerivative[i] = new AverageWindow(coefficient_derivative_window, 45.0f);

        CoefficientClamp[i][0] = 0.0f;
        CoefficientClamp[i][1] = 1.0f;

        TermClamp[i][0] = -99999.0f;
        TermClamp[i][1] = 99999.0f;

        TuningThreshold[i] = 0.01f;
        AdaptationGain[i] = 5e-5f;
        Coefficient[i] = 0.0f;
    }
    ModelError = 0.0f;
}

AdaptivePIDControllerBase::~AdaptivePIDControllerBase() {
    for (int i = 0; i < NUM_PID_TERMS; i++) {
        delete pCoefficientDerivative[i];
    }
}

void AdaptivePIDControllerBase::UpdateBase(float model_error, float timestep, float desired_process_value) {
    ePIDTerm term_to_update = static_cast<ePIDTerm>(static_cast<int>(WorldTimer.GetSeconds() / TimeSlice) % NUM_PID_TERMS);
    float term_value = GetTerm(term_to_update);
    float coefficient_derivative = 0.0f;

    if (bAbs(term_value) >= TuningThreshold[term_to_update]) {
        coefficient_derivative = GetNewCoefficientDerivative(term_to_update, model_error, desired_process_value);
        Coefficient[term_to_update] = bClamp(coefficient_derivative * timestep + Coefficient[term_to_update], CoefficientClamp[term_to_update][0],
                                             CoefficientClamp[term_to_update][1]);
    }

    float time_now = WorldTimer.GetSeconds();
    pCoefficientDerivative[term_to_update]->Record(coefficient_derivative, time_now);

    for (int i = 0; i < NUM_PID_TERMS; i++) {
        if (i != term_to_update) {
            pCoefficientDerivative[i]->Record(0.0f, time_now);
        }
    }

    float model_error_instantaneous_derivative = (model_error - ModelError) / bMax(timestep, 0.001f);
    ModelErrorDerivative.Record(model_error_instantaneous_derivative, time_now);
    ModelError = model_error;
}

float AdaptivePIDControllerBase::GetOutput() {
    float output = GetCoefficient(eP_TERM) * GetClampedTerm(eP_TERM) + GetCoefficient(eI_TERM) * GetClampedTerm(eI_TERM) +
                   GetCoefficient(eD_TERM) * GetClampedTerm(eD_TERM);

    return output;
}

float AdaptivePIDControllerBase::GetNewCoefficientDerivative(ePIDTerm term, float model_error, float desired_process_value) {
    float adaptation_gain = AdaptationGain[term];
    float sensitivity_derivative = GetSensitivityDerivative(term);
    switch (AdaptationRule) {
        case eMIT_RULE:
            return -adaptation_gain * model_error * sensitivity_derivative;
        case eNORMALIZED_MIT_RULE:
            return (-adaptation_gain * model_error * sensitivity_derivative) / (sensitivity_derivative * sensitivity_derivative + Alpha);
        case eSIGN_ERROR_RULE:
            return -adaptation_gain * Sign(model_error) * sensitivity_derivative;
        case eSIGN_SIGN_RULE:
            model_error = Sign(model_error);
            // fallthrough
        case eSIGN_DATA_RULE:
            return -adaptation_gain * model_error * Sign(sensitivity_derivative);
            break;
        case eLYAPUNOV_RULE:
            return -adaptation_gain * desired_process_value * model_error;
        default:
            break;
    }
    return 0.0f;
}

// Functionally matching
float AdaptivePIDControllerBase::GetSensitivityDerivative(float coefficient_derivative) {
    float model_error_derivative = ModelErrorDerivative.GetValue();
    if (!bEqual(coefficient_derivative, 0.0f, 1e-9f)) {
        return bClamp(model_error_derivative / coefficient_derivative, -1000.0f, 1000.0f);
    } else if (bEqual(model_error_derivative, 0.0f, 0.001f)) {
        return 0.0f;
    } else if (model_error_derivative > 0.0f) {
        return 1.0f;
    } else {
        return -1.0f;
    }
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
      PIDController(integral_history, derivative_history, 30.0f) {}

float AdaptivePIDControllerSimple::GetTerm(ePIDTerm term) {
    float term_value = 0.0f;
    switch (term) {
        case eP_TERM:
            term_value = PIDController.GetError();
            break;
        case eI_TERM:
            term_value = PIDController.GetErrorIntegral();
            break;
        case eD_TERM:
            term_value = PIDController.GetErrorDerivative();
            break;
        default:
            break;
    }
    return term_value;
}

AdaptivePIDControllerComplicated::AdaptivePIDControllerComplicated(enum eAdaptationRule adaptation_rule, float coefficient_derivative_window)
    : AdaptivePIDControllerBase(adaptation_rule, coefficient_derivative_window) {
    for (int i = 0; i < NUM_PID_TERMS; i++) {
        CurrentTermValue[i] = 0.0f;
    }
}

void AdaptivePIDControllerComplicated::Update(float model_behaviour_value, float actual_behaviour_value, float timestep,
                                              float desired_process_value) {
    float model_error = actual_behaviour_value - model_behaviour_value;
    UpdateBase(model_error, timestep, desired_process_value);
}
