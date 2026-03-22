#include "VisualTreatment.h"

#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"

extern Timer RealTimer;
extern float WorldTimeSeconds;

void VisualLookEffect::Reset() {
    this->StartTime = 0.0f;
    this->PulseLength = 0.0f;
}

void VisualLookEffectTarget::Reset() {
    this->StartWorldTime = 0.0f;
    this->Current = 0.0f;
    this->Target = 0.0f;
}

IVisualTreatment::IVisualTreatment()
    : MiddayVisualLook(0xEEC2271A, 0, nullptr), //
      SunsetVisualLook(0xCEDA4E4F, 0, nullptr), //
      UvesVisualLook(0x681BEF75, 0, nullptr), //
      CopCamVisualLook(0xEE6074A3, 0, nullptr), //
      UvesPulse(::new (__FILE__, __LINE__) VisualLookEffect(new Attrib::Gen::visuallookeffect(0x334F1E4D, 0, nullptr))), //
      UvesRadialBlur(::new (__FILE__, __LINE__) VisualLookEffect(new Attrib::Gen::visuallookeffect(0xBFA7B6AC, 0, nullptr))), //
      UvesTransition(::new (__FILE__, __LINE__) VisualLookEffect(new Attrib::Gen::visuallookeffect(0x15746132, 0, nullptr))), //
      CameraFlash(::new (__FILE__, __LINE__) VisualLookEffect(new Attrib::Gen::visuallookeffect(0x30656612, 0, nullptr))), //
      PursuitBreaker(::new (__FILE__, __LINE__) VisualLookEffectTarget(new Attrib::Gen::visuallookeffect(0x90D06C71, 0, nullptr))), //
      NosRadialBlur(::new (__FILE__, __LINE__) VisualLookEffectTarget(new Attrib::Gen::visuallookeffect(0x6B40EB80, 0, nullptr)))
{
    this->PulseBrightness = 1.0f;
    this->DesaturationTarget = -1.0f;
    this->State = HEAT_LOOK;
    this->HeatMeter = 0.0f;
    this->IsBeingPursued = -1;
    this->NosRadialBlur->Target = 0.0f;
    this->NosRadialBlur->Current = 0.0f;
    this->NosRadialBlur->StartWorldTime = 0.0f;
    this->RadialBlur = 0.0f;
    this->NosRadialBlurAmount = 0.0f;
    this->PursuitBreakerBlend = 0.0f;
    this->CurrentTarget = -1.0f;
}

void IVisualTreatment::Reset() {
    VisualLookEffectTarget *pursuitBreaker = this->PursuitBreaker;
    VisualLookEffect *uvesPulse = this->UvesPulse;
    VisualLookEffect *uvesRadialBlur = this->UvesRadialBlur;
    VisualLookEffect *uvesTransition = this->UvesTransition;
    VisualLookEffect *cameraFlash = this->CameraFlash;
    VisualLookEffectTarget *nosRadialBlur = this->NosRadialBlur;

    this->State = HEAT_LOOK;
    this->PulseBrightness = 1.0f;
    this->DesaturationTarget = -1.0f;
    this->IsBeingPursued = -1;
    this->RadialBlur = 0.0f;
    this->NosRadialBlurAmount = 0.0f;
    this->PursuitBreakerBlend = 0.0f;
    this->CurrentTarget = -1.0f;

    pursuitBreaker->StartWorldTime = 0.0f;
    pursuitBreaker->Current = 0.0f;
    pursuitBreaker->Target = 0.0f;

    uvesPulse->PulseLength = 0.0f;
    uvesPulse->StartTime = 0.0f;

    uvesRadialBlur->PulseLength = 0.0f;
    uvesRadialBlur->StartTime = 0.0f;

    uvesTransition->PulseLength = 0.0f;
    uvesTransition->StartTime = 0.0f;

    cameraFlash->PulseLength = 0.0f;
    cameraFlash->StartTime = 0.0f;

    nosRadialBlur->StartWorldTime = 0.0f;
    nosRadialBlur->Target = 0.0f;
    nosRadialBlur->Current = 0.0f;
}

void IVisualTreatment::TriggerPulse(float length) {
    VisualLookEffect *cameraFlash = this->CameraFlash;

    if (length == 0.0f) {
        length = cameraFlash->GetAttrib()->length();
        if (length == 0.0f) {
            return;
        }
    }

    cameraFlash->StopAfterLength = 1;
    cameraFlash->PulseLength = length;
    cameraFlash->StopIfHeatFalls = 0;
    cameraFlash->UseWorldTime = 0;
    cameraFlash->StartTime = RealTimer.GetSeconds();
}

void IVisualTreatment::SetNosEngaged(bool isNosEngaged) {
    float target;

    if (isNosEngaged) {
        this->NosRadialBlur->Current = 1.0f;
        target = 1.0f;
        this->NosRadialBlur->Target = target;
        this->NosRadialBlur->StartWorldTime = 0.0f;
    } else {
        target = 0.0f;
    }

    if (this->NosRadialBlur->Target == target) {
        return;
    }

    this->NosRadialBlur->Target = target;
    this->NosRadialBlur->StartWorldTime = WorldTimeSeconds;
}

void IVisualTreatment::SetPursuitBreakerTarget(float blendTarget) {
    VisualLookEffectTarget *pursuitBreaker = this->PursuitBreaker;

    if (pursuitBreaker->Target == blendTarget) {
        return;
    }

    pursuitBreaker->Target = blendTarget;
    pursuitBreaker->StartWorldTime = WorldTimeSeconds;
}
