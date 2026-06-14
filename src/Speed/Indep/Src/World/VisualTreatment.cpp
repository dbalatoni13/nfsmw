#include "VisualTreatment.h"

#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/visuallookeffect.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IGameState.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "TimeOfDay.hpp"

float twkVisualTreatmentTargetHeat = -1.0f;
float twkVisualTreatmentRadialBlur = -1.0f;
float twkVisualTreatmentPulseBrightness = -1.0f;

// UNSOLVED
float GetValueFromSpline(float value, bMatrix4 *curve) {
    float tm1 = 1.0f - value;
    float tm13 = tm1 * tm1 * tm1;
    float t3 = value * value * value;

    return tm13 * curve->v0.y + value * (tm1 * 3.0f) * tm1 * curve->v1.y + value * (value * 3.0f) * tm1 * curve->v2.y + t3 * curve->v3.y;
}

void SetMiddleGrayValue(float val) {}

class VisualLookEffect {
    friend class IVisualTreatment;

  public:
    VisualLookEffect(Attrib::Gen::visuallookeffect *attribEffect)
        : AttribEffect(attribEffect), //
          StartTime(0.0f),            //
          PulseLength(0.0f) {
        this->StopIfHeatFalls = true;
        this->StopAfterLength = false;
        this->UseWorldTime = true;
    }

    float Update(float heatMeter) {
        if (this->IsActive()) {
            return this->UpdateActive(heatMeter);
        }

        return 0.0f;
    }

    void Reset() {
        this->StartTime = 0.0f;
        this->PulseLength = 0.0f;
    }

    Attrib::Gen::visuallookeffect *GetAttrib() {
        return this->AttribEffect;
    }

    void Trigger(float length, bool useWorldTime, bool stopIfHeatFalls, bool stopAfterLength) {
        if (length == 0.0f) {
            length = this->AttribEffect->length();
            if (length == 0.0f) {
                return;
            }
        }
        this->StartTime = useWorldTime ? WorldTimer.GetSeconds() : RealTimer.GetSeconds();
        this->PulseLength = length;
        this->UseWorldTime = useWorldTime;
        this->StopIfHeatFalls = stopIfHeatFalls;
        this->StopAfterLength = stopAfterLength;
    }

  protected:
    bool IsActive() {
        return this->StartTime != 0.0f;
    }

    // UNSOLVED float scheduling
    float UpdateActive(float heatMeter) {
        float secondsElapsed;
        if (this->UseWorldTime) {
            secondsElapsed = WorldTimer.GetSeconds() - this->StartTime;
        } else {
            secondsElapsed = RealTimer.GetSeconds() - this->StartTime;
        }

        if (this->StopIfHeatFalls && heatMeter < this->AttribEffect->heattrigger()) {
            this->StartTime = 0.0f;
        }

        if (this->StopAfterLength && secondsElapsed >= this->PulseLength) {
            this->StartTime = 0.0f;
        }

        bMatrix4 *curve = reinterpret_cast<bMatrix4 *>(&const_cast<UMath::Matrix4 &>(this->AttribEffect->graph()));
        if (secondsElapsed <= 0.0f) {
            return curve->v0.y * this->AttribEffect->magnitude();
        }

        if (secondsElapsed >= this->PulseLength) {
            return curve->v3.y * this->AttribEffect->magnitude();
        }

        float value = GetValueFromSpline(secondsElapsed / this->PulseLength, curve);
        return value * this->AttribEffect->magnitude();
    }

    Attrib::Gen::visuallookeffect *AttribEffect; // offset 0x0, size 0x4
    float StartTime;                             // offset 0x4, size 0x4
    float PulseLength;                           // offset 0x8, size 0x4
    bool UseWorldTime;                           // offset 0xC, size 0x4
    bool StopIfHeatFalls;                        // offset 0x10, size 0x4
    bool StopAfterLength;                        // offset 0x14, size 0x4
};

class VisualLookEffectTarget {
    friend class IVisualTreatment;

  public:
    VisualLookEffectTarget(Attrib::Gen::visuallookeffect *attribEffect) : AttribEffect(attribEffect) {
        this->Target = 0.0f;
        this->Current = 0.0f;
        this->StartWorldTime = 0.0f;
    }

    void Reset() {
        this->Target = 0.0f;
        this->Current = 0.0f;
        this->StartWorldTime = 0.0f;
    }

    float Update() {
        if (StartWorldTime != 0.0f) {
            float secondsElapsed = WorldTimeSeconds - StartWorldTime;

            if (secondsElapsed > this->AttribEffect->length()) {
                StartWorldTime = 0.0f;
                this->Current = this->Target;
            } else if (secondsElapsed >= 0.0f) {
                bMatrix4 *curve = (bMatrix4 *)&this->AttribEffect->graph();
                float value = secondsElapsed / this->AttribEffect->length();

                if (this->Current > this->Target) {
                    value = 1.0f - value;
                }

                this->Current = GetValueFromSpline(value, curve);
            }
        }

        return this->Current;
    }

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

// UNSOLVED float assignment order
IVisualTreatment::IVisualTreatment()
    : MiddayVisualLook(0xEEC2271A, 0, nullptr),                                                                                            //
      SunsetVisualLook(0xCEDA4E4F, 0, nullptr),                                                                                            //
      UvesVisualLook(0x681BEF75, 0, nullptr),                                                                                              //
      CopCamVisualLook(0xEE6074A3, 0, nullptr),                                                                                            //
      UvesPulse(new ("VisualLookEffect", 0) VisualLookEffect(new Attrib::Gen::visuallookeffect(0x334F1E4D, 0, nullptr))),                  //
      UvesRadialBlur(new ("VisualLookEffect", 0) VisualLookEffect(new Attrib::Gen::visuallookeffect(0xBFA7B6AC, 0, nullptr))),             //
      UvesTransition(new ("VisualLookEffect", 0) VisualLookEffect(new Attrib::Gen::visuallookeffect(0x15746132, 0, nullptr))),             //
      CameraFlash(new ("VisualLookEffect", 0) VisualLookEffect(new Attrib::Gen::visuallookeffect(0x30656612, 0, nullptr))),                //
      PursuitBreaker(new ("VisualLookEffectTarget", 0) VisualLookEffectTarget(new Attrib::Gen::visuallookeffect(0x90D06C71, 0, nullptr))), //
      NosRadialBlur(new ("VisualLookEffectTarget", 0) VisualLookEffectTarget(new Attrib::Gen::visuallookeffect(0x6B40EB80, 0, nullptr))) {
    this->State = HEAT_LOOK;
    this->HeatMeter = 0.0f;
    this->PulseBrightness = 1.0f;
    this->CurrentTarget = -1.0f;
    this->DesaturationTarget = -1.0f;
    this->IsBeingPursued = -1;

    this->NosRadialBlur->Current = 0.0f;
    this->NosRadialBlur->StartWorldTime = 0.0f;
    this->NosRadialBlur->Target = 0.0f;

    this->RadialBlur = 0.0f;
    this->PursuitBreakerBlend = 0.0f;
    this->NosRadialBlurAmount = 0.0f;
}

IVisualTreatment::~IVisualTreatment() {
    delete this->UvesPulse;
    delete this->UvesRadialBlur;
    delete this->UvesTransition;
    delete this->NosRadialBlur;
    delete this->CameraFlash;
    delete this->PursuitBreaker;
}

// UNSOLVED assignment order
void IVisualTreatment::Reset() {
    this->State = HEAT_LOOK;
    this->CurrentTarget = -1.0f;
    this->IsBeingPursued = -1;
    this->PulseBrightness = 1.0f;
    this->DesaturationTarget = -1.0f;
    this->RadialBlur = 0.0f;
    this->NosRadialBlurAmount = 0.0f;
    this->PursuitBreakerBlend = 0.0f;

    PursuitBreaker->Reset();
    UvesPulse->Reset();
    UvesRadialBlur->Reset();
    UvesTransition->Reset();
    CameraFlash->Reset();
    NosRadialBlur->Reset();
}

// STRIPPED
void IVisualTreatment::PrintValues() {}

void IVisualTreatment::TriggerPulse(float length) {
    CameraFlash->Trigger(length, false, false, true);
}

void IVisualTreatment::SetNosEngaged(bool isNosEngaged) {
    if (isNosEngaged) {
        NosRadialBlur->SetCurrent(1.0f);
    }

    NosRadialBlur->SetTarget(isNosEngaged ? 1.0f : 0.0f);
}

void IVisualTreatment::SetPursuitBreakerTarget(float blendTarget) {
    this->PursuitBreaker->SetTarget(blendTarget);
}

inline void AddBlend(bMatrix4 *result, bMatrix4 *m0, float scale) {
    result->v0.x += scale * m0->v0.x;
    result->v0.y += scale * m0->v0.y;

    result->v1.x += scale * m0->v1.x;
    result->v1.y += scale * m0->v1.y;

    result->v2.x += scale * m0->v2.x;
    result->v2.y += scale * m0->v2.y;

    result->v3.x += scale * m0->v3.x;
    result->v3.y += scale * m0->v3.y;
}

inline void AddBlend(bVector4 *result, bVector4 *v, float scale) {
    result->x += scale * v->x;
    result->y += scale * v->y;
    result->z += scale * v->z;
    result->w += scale * v->w;
}

// UNSOLVED missing AddBlend call causing issues
void IVisualTreatment::BlendVisualLookAttribute(bMatrix4 &result, float defaultUves, float uves,
                                                const UMath::Matrix4 &(Attrib::Gen::visuallook::*funcPtr)() const) {
    bMemSet(&result, 0, sizeof(bMatrix4));

    if (GetCurrentTimeOfDay() == eTOD_MIDDAY) {
        if (defaultUves != 0.0f) {
            AddBlend(&result, (bMatrix4 *)&(MiddayVisualLook.*funcPtr)(), defaultUves);
        }
    } else if (defaultUves != 0.0f) {
        AddBlend(&result, (bMatrix4 *)&(SunsetVisualLook.*funcPtr)(), defaultUves);
    }

    if (uves != 0.0f) {
        AddBlend(&result, (bMatrix4 *)&(UvesVisualLook.*funcPtr)(), uves);
    }
}

void IVisualTreatment::BlendVisualLookAttribute(float &result, float defaultUves, float uves,
                                                const float &(Attrib::Gen::visuallook::*funcPtr)() const) {
    result = 0.0f;

    if (defaultUves != 0.0f) {
        if (GetCurrentTimeOfDay() == eTOD_MIDDAY) {
            result += (this->MiddayVisualLook.*funcPtr)() * defaultUves;
        } else {
            result += (this->SunsetVisualLook.*funcPtr)() * defaultUves;
        }
    }

    if (uves != 0.0f) {
        result += (this->UvesVisualLook.*funcPtr)() * uves;
    }
}

void IVisualTreatment::BlendVisualLookAttribute(bVector4 &result, float defaultUves, float uves,
                                                const UMath::Vector4 &(Attrib::Gen::visuallook::*funcPtr)() const) {
    bMemSet(&result, 0, sizeof(bVector4));

    if (defaultUves != 0.0f) {
        if (GetCurrentTimeOfDay() == eTOD_MIDDAY) {
            AddBlend(&result, (bVector4 *)(&(MiddayVisualLook.*funcPtr)()), defaultUves);
        } else {
            AddBlend(&result, (bVector4 *)(&(SunsetVisualLook.*funcPtr)()), defaultUves);
        }
    }

    if (uves != 0.0f) {
        AddBlend(&result, (bVector4 *)(&(UvesVisualLook.*funcPtr)()), uves);
    }
}

void IVisualTreatment::UpdateVisualLook() {
    Attrib::Gen::visuallook *currVisualLook = nullptr;

    if (this->State == COPCAM_LOOK) {
        currVisualLook = &this->CopCamVisualLook;
        this->PulseBrightness = 0.0f;
        this->RadialBlur = 0.0f;
    } else if (this->State == FE_LOOK) {
        currVisualLook = &this->SunsetVisualLook;
        if (GetCurrentTimeOfDay() == eTOD_MIDDAY) {
            currVisualLook = &this->MiddayVisualLook;
        }

        this->PulseBrightness = 0.0f;
        this->RadialBlur = 0.0f;
        this->PursuitBreakerBlend = 0.0f;
    }

    this->BlackBloomCurve = reinterpret_cast<const bMatrix4 &>(currVisualLook->BlackBloomCurve());
    this->ColourBloomCurve = reinterpret_cast<const bMatrix4 &>(currVisualLook->ColourBloomCurve());
    this->DetailMapCurve = reinterpret_cast<const bMatrix4 &>(currVisualLook->DetailMapCurve());

    this->ColourBloomTint = reinterpret_cast<const bVector4 &>(currVisualLook->ColourBloomTint());
    this->BlackBloomIntensity = currVisualLook->BlackBloomIntensity();
    this->ColourBloomIntensity = currVisualLook->ColourBloomIntensity();
    this->Desaturation = currVisualLook->Desaturation();
    this->DetailMapIntensity = currVisualLook->DetailMapIntensity();
}

void IVisualTreatment::TriggerUves() {
    const float kUseAttribLength = UvesTransition->GetAttrib()->length();
    UvesTransition->Trigger(0.0f, true, true, false);
    UvesRadialBlur->Trigger(0.0f, true, true, false);
    UvesPulse->Trigger(0.0f, false, true, false);
}

void IVisualTreatment::UpdateHeat(eView *view, float targetHeat, bool isBeingPursued) {
    targetHeat = static_cast<float>(static_cast<int>(targetHeat));

    if (UTL::Collections::Singleton<INIS>::Exists()) {
        IsBeingPursued = -1;
        CurrentTarget = -1.0f;
        UvesPulse->Reset();
        UvesTransition->Reset();
    }

    if ((targetHeat > CurrentTarget && CurrentTarget != -1.0f) || (!IsBeingPursued && isBeingPursued)) {
        this->TriggerUves();
    }

    IsBeingPursued = static_cast<int>(isBeingPursued);
    CurrentTarget = targetHeat;

    float uves = UvesTransition->Update(targetHeat);
    float defaultUves = 1.0f - uves;

    BlendVisualLookAttribute(BlackBloomCurve, defaultUves, uves, &Attrib::Gen::visuallook::BlackBloomCurve);
    BlendVisualLookAttribute(ColourBloomCurve, defaultUves, uves, &Attrib::Gen::visuallook::ColourBloomCurve);
    BlendVisualLookAttribute(BlackBloomIntensity, defaultUves, uves, &Attrib::Gen::visuallook::BlackBloomIntensity);
    BlendVisualLookAttribute(ColourBloomIntensity, defaultUves, uves, &Attrib::Gen::visuallook::ColourBloomIntensity);
    BlendVisualLookAttribute(ColourBloomTint, defaultUves, uves, &Attrib::Gen::visuallook::ColourBloomTint);
    BlendVisualLookAttribute(Desaturation, defaultUves, uves, &Attrib::Gen::visuallook::Desaturation);
    BlendVisualLookAttribute(DetailMapCurve, defaultUves, uves, &Attrib::Gen::visuallook::DetailMapCurve);
    BlendVisualLookAttribute(DetailMapIntensity, defaultUves, uves, &Attrib::Gen::visuallook::DetailMapIntensity);

    PulseBrightness = UvesPulse->Update(CurrentTarget);
    PulseBrightness += CameraFlash->Update(CurrentTarget);

    float uvesBlur = UvesRadialBlur->Update(CurrentTarget);

    PursuitBreakerBlend = PursuitBreaker->Update();

    float nosBlurBlend = NosRadialBlur->Update();
    float pursuitBreakerBlur = 0.0f;
    float nosBlur = 0.0f;

    if (PursuitBreakerBlend > 0.0f) {
        pursuitBreakerBlur = bClamp(PursuitBreakerBlend * PursuitBreaker->GetAttrib()->radialblur_scale(), 0.0f, 1.0f);
    }

    NosRadialBlurAmount = 0.0f;

    if (nosBlurBlend > 0.0f) {
        nosBlur = bClamp(nosBlurBlend * NosRadialBlur->GetAttrib()->radialblur_scale(), 0.0f, 2.0f);
        NosRadialBlurAmount = nosBlur;
    }

    RadialBlur = bMax(uvesBlur, bMax(nosBlur, pursuitBreakerBlur));

    if (DesaturationTarget >= 0.0f) {
        Desaturation = DesaturationTarget;
    }

    if (ColourBloomIntensityTarget >= 0.0f) {
        ColourBloomIntensity = ColourBloomIntensityTarget;
    }
}

// UNSOLVED, just scheduling
void IVisualTreatment::Update(eView *view) {
    IGameState *gamestate = UTL::Collections::Singleton<IGameState>::Get();
    bool in_pursuit_breaker = false;

    if ((gamestate != nullptr) && gamestate->InGameBreaker()) {
        in_pursuit_breaker = true;
    }

    IPerpetrator *iperp = nullptr;
    IEngine *iengine = nullptr;
    const IPlayer::List &playerList = IPlayer::GetList(PLAYER_ALL);

    for (IPlayer::List::const_iterator iter = playerList.begin(); iter != playerList.end(); ++iter) {
        IPlayer *ip = *iter;

        if (ip->GetRenderPort() == view->GetID()) {
            ISimable *simable = ip->GetSimable();

            if (simable != nullptr) {
                simable->QueryInterface(&iperp);
                simable->QueryInterface(&iengine);
            }
            break;
        }
    }

    if (UTL::Collections::Singleton<INIS>::Exists()) {
        if (this->PursuitBreaker) {
            this->PursuitBreaker->SetCurrent(0.0f);
        }
    } else {
        if (in_pursuit_breaker || gCinematicMomementCamera) {
            SetPursuitBreakerTarget(1.0f);
        } else {
            SetPursuitBreakerTarget(0.0f);
        }
    }

    if (State == HEAT_LOOK) {
        float heatMeter = 0.0f;
        bool isBeingPursed = false;

        if (iperp != nullptr) {
            heatMeter = iperp->GetHeat();
            isBeingPursed = iperp->IsBeingPursued();
        }

        UpdateHeat(view, heatMeter, isBeingPursed);
        HeatMeter = heatMeter;
    } else {
        UpdateVisualLook();
    }

    if (iengine == nullptr) {
        return;
    }

    bool nos = iengine->IsNOSEngaged();
    SetNosEngaged(nos);
}
