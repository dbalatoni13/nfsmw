#ifndef WORLD_VISUALTREATMENT_H
#define WORLD_VISUALTREATMENT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/visuallook.h"

class IVisualTreatment {
  public:
    enum eVisualLookState {
        HEAT_LOOK = 0,
        COPCAM_LOOK = 1,
        FE_LOOK = 2,
    };

    static IVisualTreatment *Get();
    void Reset();

    void SetState(eVisualLookState state) {
        Reset();
        this->State = state;
    }

  protected:
    eVisualLookState State;                        // offset 0x0, size 0x4
    bMatrix4 BlackBloomCurve;                      // offset 0x4, size 0x40
    bMatrix4 ColourBloomCurve;                     // offset 0x44, size 0x40
    float BlackBloomIntensity;                     // offset 0x84, size 0x4
    float ColourBloomIntensity;                    // offset 0x88, size 0x4
    float Desaturation;                            // offset 0x8C, size 0x4
    float CombinedBrightness;                      // offset 0x90, size 0x4
    bVector4 ColourBloomTint;                      // offset 0x94, size 0x10
    bMatrix4 DetailMapCurve;                       // offset 0xA4, size 0x40
    bVector4 vCurveCoeffs0;                        // offset 0xE4, size 0x10
    bVector4 vCurveCoeffs1;                        // offset 0xF4, size 0x10
    bVector4 vCurveCoeffs2;                        // offset 0x104, size 0x10
    bVector4 vCurveCoeffs3;                        // offset 0x114, size 0x10
    bVector4 vCoeffs0;                             // offset 0x124, size 0x10
    bVector4 vCoeffs1;                             // offset 0x134, size 0x10
    bVector4 vCoeffs2;                             // offset 0x144, size 0x10
    bVector4 vCoeffs3;                             // offset 0x154, size 0x10
    float DetailMapIntensity;                      // offset 0x164, size 0x4
    float PulseBrightness;                         // offset 0x168, size 0x4
    float RadialBlur;                              // offset 0x16C, size 0x4
    float PursuitBreakerBlend;                     // offset 0x170, size 0x4
    float NosRadialBlurAmount;                     // offset 0x174, size 0x4
    Attrib::Gen::visuallook MiddayVisualLook;      // offset 0x178, size 0x14
    Attrib::Gen::visuallook SunsetVisualLook;      // offset 0x18C, size 0x14
    Attrib::Gen::visuallook UvesVisualLook;        // offset 0x1A0, size 0x14
    Attrib::Gen::visuallook CopCamVisualLook;      // offset 0x1B4, size 0x14
    struct VisualLookEffect *UvesPulse;            // offset 0x1C8, size 0x4
    struct VisualLookEffect *UvesRadialBlur;       // offset 0x1CC, size 0x4
    struct VisualLookEffect *UvesTransition;       // offset 0x1D0, size 0x4
    struct VisualLookEffect *CameraFlash;          // offset 0x1D4, size 0x4
    struct VisualLookEffectTarget *PursuitBreaker; // offset 0x1D8, size 0x4
    struct VisualLookEffectTarget *NosRadialBlur;  // offset 0x1DC, size 0x4
    float CurrentTarget;                           // offset 0x1E0, size 0x4
    float DesaturationTarget;                      // offset 0x1E4, size 0x4
    float ColourBloomIntensityTarget;              // offset 0x1E8, size 0x4
    float HeatMeter;                               // offset 0x1EC, size 0x4
    int IsBeingPursued;                            // offset 0x1F0, size 0x4
};

void OpenVisualTreatment();
void CloseVisualTreatment();

#endif
