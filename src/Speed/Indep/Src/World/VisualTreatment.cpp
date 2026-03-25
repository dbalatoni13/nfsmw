#include "VisualTreatment.h"

#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Interfaces/SimActivities/IGameState.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/World/World.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"

extern Timer RealTimer;
extern float WorldTimeSeconds;
void SetMiddleGrayValue(float val) {}

float GetValueFromSpline(float t, bMatrix4 *curve) {
    float inv_t = 1.0f - t;

    return inv_t * inv_t * inv_t * curve->v0.y + t * 3.0f * inv_t * inv_t * curve->v1.y +
           t * t * 3.0f * inv_t * curve->v2.y + t * t * t * curve->v3.y;
}

inline void VisualLookEffect::Reset() {
    this->StartTime = 0.0f;
    this->PulseLength = 0.0f;
}

bool VisualLookEffect::IsActive() {
    return this->StartTime != 0.0f;
}

float VisualLookEffect::UpdateActive(float heatMeter) {
    int packedTime = RealTimer.GetPackedTime();
    if (this->UseWorldTime != 0) {
        packedTime = WorldTimer.GetPackedTime();
    }

    float currentTime = packedTime * 0.00025f - this->StartTime;
    if (this->StopIfHeatFalls != 0 && heatMeter < this->AttribEffect->heattrigger()) {
        this->StartTime = 0.0f;
    }

    if (this->StopAfterLength != 0 && this->PulseLength <= currentTime) {
        this->StartTime = 0.0f;
    }

    bMatrix4 *graph = reinterpret_cast<bMatrix4 *>(&const_cast<UMath::Matrix4 &>(this->AttribEffect->graph()));
    float graphValue;
    if (currentTime <= 0.0f) {
        graphValue = graph->v0.y;
    } else if (this->PulseLength <= currentTime) {
        graphValue = graph->v3.y;
    } else {
        graphValue = GetValueFromSpline(currentTime / this->PulseLength, graph);
    }

    return graphValue * this->AttribEffect->magnitude();
}

inline void VisualLookEffectTarget::Reset() {
    this->Target = 0.0f;
    this->Current = 0.0f;
    this->StartWorldTime = 0.0f;
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

IVisualTreatment::~IVisualTreatment() {
    delete this->UvesPulse;
    delete this->UvesRadialBlur;
    delete this->UvesTransition;
    delete this->NosRadialBlur;
    delete this->CameraFlash;
    delete this->PursuitBreaker;
}

void IVisualTreatment::Reset() {
    this->State = HEAT_LOOK;
    this->PulseBrightness = 1.0f;
    this->DesaturationTarget = -1.0f;
    this->IsBeingPursued = -1;
    this->RadialBlur = 0.0f;
    this->NosRadialBlurAmount = 0.0f;
    this->PursuitBreakerBlend = 0.0f;
    this->CurrentTarget = -1.0f;

    PursuitBreaker->Reset();
    UvesPulse->Reset();
    UvesRadialBlur->Reset();
    UvesTransition->Reset();
    CameraFlash->Reset();
    NosRadialBlur->Reset();
}

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
    VisualLookEffectTarget *pursuitBreaker = this->PursuitBreaker;

    if (pursuitBreaker->Target == blendTarget) {
        return;
    }

    pursuitBreaker->Target = blendTarget;
    pursuitBreaker->StartWorldTime = WorldTimeSeconds;
}

void IVisualTreatment::BlendVisualLookAttribute(
    bMatrix4 &result, float defaultUves, float uves,
    const UMath::Matrix4 &(Attrib::Gen::visuallook::*funcPtr)() const) {
    const Attrib::Gen::visuallook *currVisualLook;

    bMemSet(&result, 0, 0x40);

    if (GetCurrentTimeOfDay() == eTOD_MIDDAY) {
        if (defaultUves != 0.0f) {
            currVisualLook = &this->MiddayVisualLook;
            const bMatrix4 &currCurve = *reinterpret_cast<const bMatrix4 *>(&(currVisualLook->*funcPtr)());
            result.v0.x += defaultUves * currCurve.v0.x;
            result.v0.y += defaultUves * currCurve.v0.y;
            result.v1.x += defaultUves * currCurve.v1.x;
            result.v1.y += defaultUves * currCurve.v1.y;
            result.v2.x += defaultUves * currCurve.v2.x;
            result.v2.y += defaultUves * currCurve.v2.y;
            result.v3.x += defaultUves * currCurve.v3.x;
            result.v3.y += defaultUves * currCurve.v3.y;
        }
    } else if (defaultUves != 0.0f) {
        currVisualLook = &this->SunsetVisualLook;
        const bMatrix4 &currCurve = *reinterpret_cast<const bMatrix4 *>(&(currVisualLook->*funcPtr)());
        result.v0.x += defaultUves * currCurve.v0.x;
        result.v0.y += defaultUves * currCurve.v0.y;
        result.v1.x += defaultUves * currCurve.v1.x;
        result.v1.y += defaultUves * currCurve.v1.y;
        result.v2.x += defaultUves * currCurve.v2.x;
        result.v2.y += defaultUves * currCurve.v2.y;
        result.v3.x += defaultUves * currCurve.v3.x;
        result.v3.y += defaultUves * currCurve.v3.y;
    }

    if (uves != 0.0f) {
        const bMatrix4 &currCurve = *reinterpret_cast<const bMatrix4 *>(&(this->UvesVisualLook.*funcPtr)());
        result.v0.x += uves * currCurve.v0.x;
        result.v0.y += uves * currCurve.v0.y;
        result.v1.x += uves * currCurve.v1.x;
        result.v1.y += uves * currCurve.v1.y;
        result.v2.x += uves * currCurve.v2.x;
        result.v2.y += uves * currCurve.v2.y;
        result.v3.x += uves * currCurve.v3.x;
        result.v3.y += uves * currCurve.v3.y;
    }
}

void IVisualTreatment::BlendVisualLookAttribute(
    float &result, float defaultUves, float uves,
    const float &(Attrib::Gen::visuallook::*funcPtr)() const) {
    const Attrib::Gen::visuallook *currVisualLook;

    result = 0.0f;

    if (defaultUves != 0.0f) {
        currVisualLook = &this->SunsetVisualLook;
        if (GetCurrentTimeOfDay() == eTOD_MIDDAY) {
            currVisualLook = &this->MiddayVisualLook;
        }

        result += defaultUves * (currVisualLook->*funcPtr)();
    }

    if (uves != 0.0f) {
        result += uves * (this->UvesVisualLook.*funcPtr)();
    }
}

void IVisualTreatment::BlendVisualLookAttribute(
    bVector4 &result, float defaultUves, float uves,
    const UMath::Vector4 &(Attrib::Gen::visuallook::*funcPtr)() const) {
    const Attrib::Gen::visuallook *currVisualLook;

    bMemSet(&result, 0, 0x10);

    if (defaultUves != 0.0f) {
        currVisualLook = &this->SunsetVisualLook;
        if (GetCurrentTimeOfDay() == eTOD_MIDDAY) {
            currVisualLook = &this->MiddayVisualLook;
        }

        const bVector4 &currTint = *reinterpret_cast<const bVector4 *>(&(currVisualLook->*funcPtr)());
        result.x += defaultUves * currTint.x;
        result.y += defaultUves * currTint.y;
        result.z += defaultUves * currTint.z;
        result.w += defaultUves * currTint.w;
    }

    if (uves != 0.0f) {
        const bVector4 &currTint = *reinterpret_cast<const bVector4 *>(&(this->UvesVisualLook.*funcPtr)());
        result.x += uves * currTint.x;
        result.y += uves * currTint.y;
        result.z += uves * currTint.z;
        result.w += uves * currTint.w;
    }
}

void IVisualTreatment::UpdateVisualLook() {
    Attrib::Gen::visuallook *currVisualLook = 0;

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

    PSMTX44Copy(*reinterpret_cast<const Mtx44 *>(&currVisualLook->BlackBloomCurve()),
                *reinterpret_cast<Mtx44 *>(&this->BlackBloomCurve));
    PSMTX44Copy(*reinterpret_cast<const Mtx44 *>(&currVisualLook->ColourBloomCurve()),
                *reinterpret_cast<Mtx44 *>(&this->ColourBloomCurve));
    PSMTX44Copy(*reinterpret_cast<const Mtx44 *>(&currVisualLook->DetailMapCurve()),
                *reinterpret_cast<Mtx44 *>(&this->DetailMapCurve));

    this->ColourBloomTint = *reinterpret_cast<const bVector4 *>(&currVisualLook->ColourBloomTint());
    this->BlackBloomIntensity = currVisualLook->BlackBloomIntensity();
    this->ColourBloomIntensity = currVisualLook->ColourBloomIntensity();
    this->Desaturation = currVisualLook->Desaturation();
    this->DetailMapIntensity = currVisualLook->DetailMapIntensity();
}

void IVisualTreatment::TriggerUves() {
    VisualLookEffect *uvesTransition = this->UvesTransition;
    float length = uvesTransition->GetAttrib()->length();

    if (length != 0.0f) {
        uvesTransition->PulseLength = length;
        uvesTransition->StopAfterLength = 0;
        uvesTransition->UseWorldTime = 1;
        uvesTransition->StopIfHeatFalls = 1;
        uvesTransition->StartTime = WorldTimer.GetSeconds();
    }

    VisualLookEffect *uvesRadialBlur = this->UvesRadialBlur;
    length = uvesRadialBlur->GetAttrib()->length();
    if (length != 0.0f) {
        uvesRadialBlur->PulseLength = length;
        uvesRadialBlur->StopAfterLength = 0;
        uvesRadialBlur->UseWorldTime = 1;
        uvesRadialBlur->StopIfHeatFalls = 1;
        uvesRadialBlur->StartTime = WorldTimer.GetSeconds();
    }

    VisualLookEffect *uvesPulse = this->UvesPulse;
    length = uvesPulse->GetAttrib()->length();
    if (length != 0.0f) {
        uvesPulse->StopAfterLength = 0;
        uvesPulse->PulseLength = length;
        uvesPulse->StopIfHeatFalls = 1;
        uvesPulse->UseWorldTime = 0;
        uvesPulse->StartTime = RealTimer.GetSeconds();
    }
}

void IVisualTreatment::UpdateHeat(eView *view, float targetHeat, bool isBeingPursued) {
    targetHeat = static_cast< float >(static_cast< int >(targetHeat));

    bool hasIni = UTL::Collections::Singleton<INIS>::Get() != 0;
    if (hasIni) {
        this->IsBeingPursued = -1;
        this->CurrentTarget = -1.0f;
        this->UvesPulse->PulseLength = 0.0f;
        this->UvesPulse->StartTime = 0.0f;
        this->UvesTransition->PulseLength = 0.0f;
        this->UvesTransition->StartTime = 0.0f;
    }

    if (((targetHeat > this->CurrentTarget && this->CurrentTarget != -1.0f)) || (this->IsBeingPursued == 0 && isBeingPursued)) {
        this->TriggerUves();
    }

    this->IsBeingPursued = isBeingPursued;
    this->CurrentTarget = targetHeat;
    float currentTarget = this->CurrentTarget;

    VisualLookEffect *uvesTransition = this->UvesTransition;
    float uves = 0.0f;
    if (uvesTransition->IsActive()) {
        uves = uvesTransition->UpdateActive(targetHeat);
    }

    float defaultUves = 1.0f - uves;
    this->BlendVisualLookAttribute(this->BlackBloomCurve, defaultUves, uves, &Attrib::Gen::visuallook::BlackBloomCurve);
    this->BlendVisualLookAttribute(this->ColourBloomCurve, defaultUves, uves, &Attrib::Gen::visuallook::ColourBloomCurve);
    this->BlendVisualLookAttribute(this->BlackBloomIntensity, defaultUves, uves, &Attrib::Gen::visuallook::BlackBloomIntensity);
    this->BlendVisualLookAttribute(this->ColourBloomIntensity, defaultUves, uves, &Attrib::Gen::visuallook::ColourBloomIntensity);
    this->BlendVisualLookAttribute(this->ColourBloomTint, defaultUves, uves, &Attrib::Gen::visuallook::ColourBloomTint);
    this->BlendVisualLookAttribute(this->Desaturation, defaultUves, uves, &Attrib::Gen::visuallook::Desaturation);
    this->BlendVisualLookAttribute(this->DetailMapCurve, defaultUves, uves, &Attrib::Gen::visuallook::DetailMapCurve);
    this->BlendVisualLookAttribute(this->DetailMapIntensity, defaultUves, uves, &Attrib::Gen::visuallook::DetailMapIntensity);

    VisualLookEffect *uvesPulse = this->UvesPulse;
    float pulseBrightness = 0.0f;
    if (uvesPulse->IsActive()) {
        pulseBrightness = uvesPulse->UpdateActive(currentTarget);
    }
    this->PulseBrightness = pulseBrightness;

    VisualLookEffect *cameraFlash = this->CameraFlash;
    float cameraFlashValue;
    if (cameraFlash->IsActive()) {
        cameraFlashValue = cameraFlash->UpdateActive(currentTarget);
    } else {
        cameraFlashValue = 0.0f;
    }
    this->PulseBrightness += cameraFlashValue;

    VisualLookEffect *uvesRadialBlurEffect = this->UvesRadialBlur;
    float uvesRadialBlur;
    if (uvesRadialBlurEffect->IsActive()) {
        uvesRadialBlur = uvesRadialBlurEffect->UpdateActive(currentTarget);
    } else {
        uvesRadialBlur = 0.0f;
    }

    VisualLookEffectTarget *pursuitBreaker = this->PursuitBreaker;
    if (pursuitBreaker->StartWorldTime != 0.0f) {
        float elapsed = WorldTimeSeconds - pursuitBreaker->StartWorldTime;
        float length = pursuitBreaker->GetAttrib()->length();
        if (elapsed > length) {
            pursuitBreaker->StartWorldTime = 0.0f;
            pursuitBreaker->Current = pursuitBreaker->Target;
        } else if (elapsed >= 0.0f) {
            float normalized = elapsed / length;
            if (pursuitBreaker->Current > pursuitBreaker->Target) {
                normalized = 1.0f - normalized;
            }
            pursuitBreaker->Current =
                GetValueFromSpline(normalized, reinterpret_cast<bMatrix4 *>(&const_cast<UMath::Matrix4 &>(pursuitBreaker->GetAttrib()->graph())));
        }
    }

    VisualLookEffectTarget *nosRadialBlur = this->NosRadialBlur;
    this->PursuitBreakerBlend = pursuitBreaker->Current;
    if (nosRadialBlur->StartWorldTime != 0.0f) {
        float elapsed = WorldTimeSeconds - nosRadialBlur->StartWorldTime;
        float length = nosRadialBlur->GetAttrib()->length();
        if (elapsed > length) {
            nosRadialBlur->StartWorldTime = 0.0f;
            nosRadialBlur->Current = nosRadialBlur->Target;
        } else if (elapsed >= 0.0f) {
            float normalized = elapsed / length;
            if (nosRadialBlur->Current > nosRadialBlur->Target) {
                normalized = 1.0f - normalized;
            }
            nosRadialBlur->Current =
                GetValueFromSpline(normalized, reinterpret_cast<bMatrix4 *>(&const_cast<UMath::Matrix4 &>(nosRadialBlur->GetAttrib()->graph())));
        }
    }

    float pursuitBreakerRadialBlur = 0.0f;
    float nosRadialBlurAmount = nosRadialBlur->Current;
    float nosRadialBlurValue = 0.0f;
    if (this->PursuitBreakerBlend > 0.0f) {
        pursuitBreakerRadialBlur = this->PursuitBreakerBlend * this->PursuitBreaker->GetAttrib()->radialblur_scale();
        pursuitBreakerRadialBlur = bMax(pursuitBreakerRadialBlur, 0.0f);
        pursuitBreakerRadialBlur = bMin(pursuitBreakerRadialBlur, 1.0f);
    }

    this->NosRadialBlurAmount = 0.0f;
    if (nosRadialBlurAmount > 0.0f) {
        nosRadialBlurValue = nosRadialBlurAmount * this->NosRadialBlur->GetAttrib()->radialblur_scale();
        nosRadialBlurValue = bMax(nosRadialBlurValue, 0.0f);
        nosRadialBlurValue = bMin(nosRadialBlurValue, 0.5f);
        this->NosRadialBlurAmount = nosRadialBlurValue;
    }

    float radialBlur = bMax(pursuitBreakerRadialBlur, nosRadialBlurValue);
    radialBlur = bMax(uvesRadialBlur, radialBlur);
    this->RadialBlur = radialBlur;

    if (this->DesaturationTarget >= 0.0f) {
        this->Desaturation = this->DesaturationTarget;
    }
    if (this->ColourBloomIntensityTarget >= 0.0f) {
        this->ColourBloomIntensity = this->ColourBloomIntensityTarget;
    }

    (void)view;
}

void IVisualTreatment::Update(eView *view) {
    bool inGameBreaker = false;
    IGameState *gameState = IGameState::Get();
    if (gameState != 0 && gameState->InGameBreaker()) {
        inGameBreaker = true;
    }

    IPerpetrator *perpetrator = 0;
    IEngine *engine = 0;
    for (IPlayer::List::const_iterator iter = IPlayer::GetList(PLAYER_ALL).begin(); iter != IPlayer::GetList(PLAYER_ALL).end(); ++iter) {
        IPlayer *player = *iter;
        if (player->GetRenderPort() == view->GetID()) {
            ISimable *simable = player->GetSimable();
            if (simable != 0) {
                simable->QueryInterface(&perpetrator);
                simable->QueryInterface(&engine);
            }
            break;
        }
    }

    if (UTL::Collections::Singleton<INIS>::Get() == 0) {
        if (inGameBreaker || gCinematicMomementCamera) {
            this->SetPursuitBreakerTarget(1.0f);
        } else {
            this->SetPursuitBreakerTarget(0.0f);
        }
    } else if (this->PursuitBreaker != 0) {
        this->PursuitBreaker->StartWorldTime = 0.0f;
        this->PursuitBreaker->Current = 0.0f;
        this->PursuitBreaker->Target = 0.0f;
    }

    if (this->State == HEAT_LOOK) {
        float targetHeat = 0.0f;
        bool isBeingPursued = false;
        if (perpetrator != 0) {
            targetHeat = perpetrator->GetHeat();
            isBeingPursued = perpetrator->IsBeingPursued();
        }

        this->UpdateHeat(view, targetHeat, isBeingPursued);
        this->HeatMeter = targetHeat;
    } else {
        this->UpdateVisualLook();
    }

    if (engine != 0) {
        this->SetNosEngaged(engine->IsNOSEngaged());
    }
}
