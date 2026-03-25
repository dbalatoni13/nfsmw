#ifndef WORLD_VISUALTREATMENT_H
#define WORLD_VISUALTREATMENT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Ecstasy/EcstasyE.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/visuallook.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/visuallookeffect.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

extern float WorldTimeSeconds;

class VisualLookEffect {
    friend class IVisualTreatment;

  public:
    explicit VisualLookEffect(Attrib::Gen::visuallookeffect *attribEffect)
        : AttribEffect(attribEffect), //
          StartTime(0.0f), //
          PulseLength(0.0f), //
          UseWorldTime(1), //
          StopIfHeatFalls(1), //
          StopAfterLength(0)
    {}

    float Update(float heatMeter);
    void Reset();

    Attrib::Gen::visuallookeffect *GetAttrib() {
        return this->AttribEffect;
    }

    void Trigger(float length, bool useWorldTime, bool stopIfHeatFalls, bool stopAfterLength) {
        if (length == 0.0f) {
            length = AttribEffect->length();
            if (length == 0.0f) {
                return;
            }
        }
        this->StartTime = RealTimer.GetSeconds();
        this->PulseLength = length;
        this->UseWorldTime = useWorldTime;
        this->StopIfHeatFalls = stopIfHeatFalls;
        this->StopAfterLength = stopAfterLength;
    }

  protected:
    bool IsActive();
    float UpdateActive(float heatMeter);

    Attrib::Gen::visuallookeffect *AttribEffect; // offset 0x0, size 0x4
    float StartTime;                             // offset 0x4, size 0x4
    float PulseLength;                          // offset 0x8, size 0x4
    int UseWorldTime;                           // offset 0xC, size 0x4
    int StopIfHeatFalls;                        // offset 0x10, size 0x4
    int StopAfterLength;                        // offset 0x14, size 0x4
};

class VisualLookEffectTarget {
    friend class IVisualTreatment;

  public:
    explicit VisualLookEffectTarget(Attrib::Gen::visuallookeffect *attribEffect)
        : AttribEffect(attribEffect), //
          StartWorldTime(0.0f), //
          Current(0.0f), //
          Target(0.0f)
    {}

    void Reset();
    float Update();
    void SetTarget(float target) {
        if (this->Target == target) {
            return;
        }
        this->Target = target;
        this->StartWorldTime = WorldTimeSeconds;
    }
    void SetCurrent(float value) {
        this->Current = value;
        this->Target = value;
        this->StartWorldTime = 0.0f;
    }

    Attrib::Gen::visuallookeffect *GetAttrib() {
        return this->AttribEffect;
    }

  private:
    Attrib::Gen::visuallookeffect *AttribEffect; // offset 0x0, size 0x4
    float StartWorldTime;                        // offset 0x4, size 0x4
    float Current;                               // offset 0x8, size 0x4
    float Target;                                // offset 0xC, size 0x4
};

class IVisualTreatment {
  public:
    enum eVisualLookState {
        HEAT_LOOK = 0,
        COPCAM_LOOK = 1,
        FE_LOOK = 2,
    };

    IVisualTreatment();
    ~IVisualTreatment();

    static IVisualTreatment *Get();

    void SetState(eVisualLookState state) {
        this->State = state;
    }

    void Update(eView *view);
    void SetPursuitBreakerTarget(float blendTarget);
    void SetNosEngaged(bool isNosEngaged);

    void SetDesaturationTarget(float setTarget) {
        this->DesaturationTarget = setTarget;
    }

    void SetColourBloomIntensityTarget(float setTarget) {
        this->ColourBloomIntensityTarget = setTarget;
    }

    float GetRadialBlur() {
        return this->RadialBlur;
    }

    float GetNosRadialBlur() const {
        return this->NosRadialBlurAmount;
    }

    float GetPursuitBreakerBlend() {
        return this->PursuitBreakerBlend;
    }

    void TriggerPulse(float length);
    void Reset();
    void PrintValues();

  protected:
    void UpdateVisualLook();
    void BlendVisualLookAttribute(bMatrix4 &result, float defaultUves, float uves,
                                  const UMath::Matrix4 &(Attrib::Gen::visuallook::*funcPtr)() const);
    void BlendVisualLookAttribute(float &result, float defaultUves, float uves,
                                  const float &(Attrib::Gen::visuallook::*funcPtr)() const);
    void BlendVisualLookAttribute(bVector4 &result, float defaultUves, float uves,
                                  const UMath::Vector4 &(Attrib::Gen::visuallook::*funcPtr)() const);
    void TriggerUves();
    void UpdateHeat(eView *view, float targetHeat, bool isBeingPursued);

    eVisualLookState State;                    // offset 0x0, size 0x4
    bMatrix4 BlackBloomCurve;                 // offset 0x4, size 0x40
    bMatrix4 ColourBloomCurve;                // offset 0x44, size 0x40
    float BlackBloomIntensity;                // offset 0x84, size 0x4
    float ColourBloomIntensity;               // offset 0x88, size 0x4
    float Desaturation;                       // offset 0x8C, size 0x4
    float CombinedBrightness;                 // offset 0x90, size 0x4
    bVector4 ColourBloomTint;                 // offset 0x94, size 0x10
    bMatrix4 DetailMapCurve;                  // offset 0xA4, size 0x40
    bVector4 vCurveCoeffs0;                   // offset 0xE4, size 0x10
    bVector4 vCurveCoeffs1;                   // offset 0xF4, size 0x10
    bVector4 vCurveCoeffs2;                   // offset 0x104, size 0x10
    bVector4 vCurveCoeffs3;                   // offset 0x114, size 0x10
    bVector4 vCoeffs0;                        // offset 0x124, size 0x10
    bVector4 vCoeffs1;                        // offset 0x134, size 0x10
    bVector4 vCoeffs2;                        // offset 0x144, size 0x10
    bVector4 vCoeffs3;                        // offset 0x154, size 0x10
    float DetailMapIntensity;                 // offset 0x164, size 0x4
    float PulseBrightness;                    // offset 0x168, size 0x4
    float RadialBlur;                         // offset 0x16C, size 0x4
    float PursuitBreakerBlend;                // offset 0x170, size 0x4
    float NosRadialBlurAmount;                // offset 0x174, size 0x4
    Attrib::Gen::visuallook MiddayVisualLook; // offset 0x178, size 0x14
    Attrib::Gen::visuallook SunsetVisualLook; // offset 0x18C, size 0x14
    Attrib::Gen::visuallook UvesVisualLook;   // offset 0x1A0, size 0x14
    Attrib::Gen::visuallook CopCamVisualLook; // offset 0x1B4, size 0x14
    VisualLookEffect *UvesPulse;              // offset 0x1C8, size 0x4
    VisualLookEffect *UvesRadialBlur;         // offset 0x1CC, size 0x4
    VisualLookEffect *UvesTransition;         // offset 0x1D0, size 0x4
    VisualLookEffect *CameraFlash;            // offset 0x1D4, size 0x4
    VisualLookEffectTarget *PursuitBreaker;   // offset 0x1D8, size 0x4
    VisualLookEffectTarget *NosRadialBlur;    // offset 0x1DC, size 0x4
    float CurrentTarget;                      // offset 0x1E0, size 0x4
    float DesaturationTarget;                 // offset 0x1E4, size 0x4
    float ColourBloomIntensityTarget;         // offset 0x1E8, size 0x4
    float HeatMeter;                          // offset 0x1EC, size 0x4
    int IsBeingPursued;                       // offset 0x1F0, size 0x4
};

#endif
