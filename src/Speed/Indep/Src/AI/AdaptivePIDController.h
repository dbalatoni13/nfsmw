#ifndef AI_ADAPTIVEPIDCONTROLLER_H
#define AI_ADAPTIVEPIDCONTROLLER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Table.hpp"
#include <cstddef>

enum eAdaptationRule {
    eMIT_RULE = 0,
    eNORMALIZED_MIT_RULE = 1,
    eSIGN_ERROR_RULE = 2,
    eSIGN_DATA_RULE = 3,
    eSIGN_SIGN_RULE = 4,
    eLYAPUNOV_RULE = 5,
    NUM_ADAPTATION_RULES = 6
};

enum ePIDTerm { eP_TERM = 0, eI_TERM = 1, eD_TERM = 2, NUM_PID_TERMS = 3 };

// total size: 0xAC
class AdaptivePIDControllerBase {
  public:
    // Functions
    // void *operator new(size_t size, void *ptr) {}

    // void operator delete(void *mem, void *ptr) {}

    void *operator new(size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    // void operator delete(void *mem, unsigned int size) {}

    // void *operator new(size_t size, const char *name) {}

    // void operator delete(void *mem, const char *name) {}

    // void operator delete(void *mem, size_t size, const char *name) {}

    AdaptivePIDControllerBase(eAdaptationRule adaptation_rule, float coefficient_derivative_window);
    virtual ~AdaptivePIDControllerBase();
    void UpdateBase(float model_error, float timestep, float desired_process_value);
    float GetOutput();

    void ForceCoefficient(ePIDTerm term, float new_coefficient) {}

    // float GetCoefficient(ePIDTerm term) {}

    // float GetClampedTerm(ePIDTerm term) {}

    void SetTimeSlice(float timeslice) {
        TimeSlice = timeslice;
    }

    void SetCoefficientClamp(ePIDTerm term, float min_value, float max_value) {
        CoefficientClamp[term][0] = min_value;
        CoefficientClamp[term][1] = max_value;
    }

    void SetTermClamp(ePIDTerm term, float min_value, float max_value) {}

    void SetTuningThreshold(ePIDTerm term, float min_value) {
        TuningThreshold[term] = min_value;
    }

    void SetAdaptationGain(ePIDTerm term, float adaptation_gain) {
        AdaptationGain[term] = adaptation_gain;
    }

    void SetAlpha(float alpha) {}

  protected:
    float GetNewCoefficientDerivative(ePIDTerm term, float model_error, float desired_process_value);

    // float GetSensitivityDerivative(ePIDTerm term) {}

    float GetSensitivityDerivative(float coefficient_derivative);

    float Sign(float v);

    float TermClamp[3][2]; // offset 0x0, size 0x18
  private:
    eAdaptationRule AdaptationRule;           // offset 0x18, size 0x4
    float TimeSlice;                          // offset 0x1C, size 0x4
    float CoefficientClamp[3][2];             // offset 0x20, size 0x18
    float TuningThreshold[3];                 // offset 0x38, size 0xC
    float AdaptationGain[3];                  // offset 0x44, size 0xC
    float Alpha;                              // offset 0x50, size 0x4
    float Coefficient[3];                     // offset 0x54, size 0xC
    AverageWindow *pCoefficientDerivative[3]; // offset 0x60, size 0xC
    float ModelError;                         // offset 0x6C, size 0x4
    AverageWindow ModelErrorDerivative;       // offset 0x70, size 0x38
};

enum eRecordingInstruction {
    eINSTRUCTION_NONE = 0,
    eINSTRUCTION_IGNORE_DERIVATIVE_TERM = 1,
    eINSTRUCTION_IGNORE_INTEGRAL_TERM = 2,
    eINSTRUCTION_IGNORE_INTEGRAL_AND_DERIVATIVE_TERMS = 3,
    NUM_RECORDING_INSTRUCTIONS = 4,
};

// total size: 0x130
class AdaptivePIDControllerSimple : public AdaptivePIDControllerBase {
  public:
    AdaptivePIDControllerSimple(eAdaptationRule adaptation_rule, float coefficient_derivative_window, int integral_history, int derivative_history);

    // Overrides: AdaptivePIDControllerBase
    ~AdaptivePIDControllerSimple() override {}

    // Overrides: AdaptivePIDControllerBase
    float GetTerm(ePIDTerm term) override;

    void Update(float desired_process_value, float actual_process_value, float model_behaviour_value, float timestep,
                eRecordingInstruction recording_instruction);

  private:
    PidError PIDController; // offset 0xAC, size 0x84
};

// total size: 0xB8
class AdaptivePIDControllerComplicated : public AdaptivePIDControllerBase {
  public:
    AdaptivePIDControllerComplicated(eAdaptationRule adaptation_rule, float coefficient_derivative_window);

    // Overrides: AdaptivePIDControllerBase
    ~AdaptivePIDControllerComplicated() override {}

    void Update(float model_behaviour_value, float actual_behaviour_value, float timestep, float desired_process_value);

    // Overrides: AdaptivePIDControllerBase
    // float GetTerm(ePIDTerm term) override {}

  private:
    float CurrentTermValue[3]; // offset 0xAC, size 0xC
};

#endif
