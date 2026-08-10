#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"
#include "Speed/Indep/Libs/Support/Utility/UBezierLite.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

void FillGraphFromSpline(const UMath::Matrix4 &matrix, bVector2 *points, int num_points, float XScale, float YScale);

static const float UP_SHIFTING_TRQ_ATTACH_INITIAL_PERCENT[4] = {1.0f, 1.0f, 1.0f, 1.0f};
static const int UP_SHIFTING_TRQ_ATTACK_TIME[4] = {0, 0, 0, 0};
extern float DOWN_SHIFTING_REV_PERCENT;
extern int DOWN_SHIFTING_REV_RAMP_TIME;

SndBase::TypeInfo *SFXCTL_Shifting::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFXCTL_Shifting::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXCTL_Shifting::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_Shifting::GetStaticTypeInfo()->typeName, false) SFXCTL_Shifting();
    }
    return new (SFXCTL_Shifting::GetStaticTypeInfo()->typeName, true) SFXCTL_Shifting();
}

SFXCTL_Shifting::SFXCTL_Shifting()
    : m_RPMGraph(m_RPMPoints, 7) {
    this->m_pShiftingPatternData = nullptr;
}

SFXCTL_Shifting::~SFXCTL_Shifting() {}

void SFXCTL_Shifting::UpdateMixerOutputs() {
    this->SetDMIX_Input(0, (this->eShiftState == SHFT_UP_DISENGAGE) ? 0x7FFF : 0);
    this->SetDMIX_Input(1, (this->eShiftStageChanged == SHFT_UP_ENGAGING) ? 0x7FFF : 0);
    this->SetDMIX_Input(2, this->IsDownShifting() ? 0x7FFF : 0);
}

int SFXCTL_Shifting::GetController(int Index) {
    if (Index != 0) {
        return -1;
    }
    return 4;
}

void SFXCTL_Shifting::AttachController(SFXCTL *psfxctl) {
    if ((psfxctl->GetObjectIndex()) == 4) {
        this->m_pEngineCtl = static_cast<SFXCTL_Engine *>(psfxctl);
    }
}

void SFXCTL_Shifting::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    this->m_UGL = this->m_pEAXCar->m_TransmissionUGL;
    this->m_pShiftingPatternData = &this->m_pEAXCar->GetShiftInfo();
    this->m_nPostShiftFXLevel = this->m_UGL % 2;
}

void SFXCTL_Shifting::InitSFX() {
    SFXCTL::InitSFX();
    this->ShiftType = static_cast<AEMS_SHIFTING_SAMPLES>(1);
    this->m_VOL_LFO_AMP = 0;
    this->m_VOL_LFO_FRQ = 0;
    this->m_TRQ_LFO_AMP = 0;
    this->m_TRQ_LFO_FRQ = 0;
    this->m_RPM_LFO_AMP = 0;
    this->m_RPM_LFO_FRQ = 0;
    this->eShiftState = SHFT_NONE;
    this->eShiftStageChanged = SHFT_NONE;
    this->eShift_LFO = SHIFT_LFO_NONE;
    this->m_bPendingNeedShiftSound = false;
    this->m_bShouldBeWhining = false;
    this->m_bBrakePedalMashed = false;
    this->tShiftDelay = 0.0f;
    this->m_nPostShiftFXLevel = 0;
    this->m_bNeed_DeccelSnd = false;
    this->t_Last_Shift = 0.0f;
    this->m_bNeed_AccelSnd = false;
    this->m_timeBrakeLastMashed = Timer(0);
}

void SFXCTL_Shifting::UpdateGearShiftState(float t) {
    if (SndBase::m_fRunningTime > this->tShiftDelay &&
        *static_cast<int *>(static_cast<void *>(&this->m_bPendingNeedShiftSound)) != 0) {
        *static_cast<int *>(static_cast<void *>(&this->m_bNeed_ShiftGearSnd)) = 1;
        *static_cast<int *>(static_cast<void *>(&this->m_bPendingNeedShiftSound)) = 0;
    }

    *static_cast<int *>(static_cast<void *>(&this->m_bShouldBeWhining)) = (static_cast<int>(this->GetCurGear()) == 0);

    EAX_CarState *carstate = this->m_pEAXCar->GetPhysCar();
    if (carstate->IsLocalPlayerCar()) {
        float t_last_mashed = (WorldTimer - this->m_timeBrakeLastMashed).GetSeconds();
        static float prevbrakestate = carstate->GetBrake();

        if (carstate->GetBrake() >= 1.0f &&
            *static_cast<int *>(static_cast<void *>(&this->m_bBrakePedalMashed)) == 0 &&
            t_last_mashed > 1.0f &&
            prevbrakestate == 0.0f &&
            carstate->GetVelocityMagnitudeMPH() > 5.0f) {
            *static_cast<int *>(static_cast<void *>(&this->m_bBrakePedalMashed)) = 1;
        }

        else if (*static_cast<int *>(static_cast<void *>(&this->m_bBrakePedalMashed)) != 0) {
            this->m_timeBrakeLastMashed = WorldTimer;
            if (carstate->GetBrake() == 0.0f) {
                *static_cast<int *>(static_cast<void *>(&this->m_bBrakePedalMashed)) = 0;
            }
        }

        prevbrakestate = carstate->GetBrake();
    }

    this->eShiftStageChanged = SHFT_NONE;
    if (this->eShiftState == SHFT_NONE) {
        return;
    }

    this->UpdateRPM(t);
    this->UpdateTorque(t);
    this->m_InterpShiftVol.Update(t);
    this->PostShiftFX_Update(t);
    this->t_CurStage = t * 1000.0f + this->t_CurStage;

    switch (this->eShiftState) {
    case SHFT_UP_DISENGAGE: {
        if (this->t_CurStage > this->m_RPMPoints[6].x) {
            this->m_CurStage++;
            if (this->m_CurStage < this->m_pShiftingPatternData->Num_Up_DisengageFall()) {
                const UMath::Matrix4 &curvespline = this->m_pShiftingPatternData->Up_DisengageFall_Curve(this->m_CurStage);

                FillGraphFromSpline(curvespline, this->m_RPMPoints, 7,
                                    static_cast<float>(this->m_pShiftingPatternData->Up_DisengageFall(this->m_CurStage).Time),
                                    static_cast<float>(this->m_pShiftingPatternData->Up_DisengageFall(this->m_CurStage).RPM));
                this->t_CurStage = t * 1000.0f;
                this->RPMOffset = static_cast<unsigned short>(static_cast<int>(this->m_InterpShiftRPM.GetValue()));
                float currpm = bClamp(static_cast<float>(this->RPMOffset) + this->m_RPMGraph.GetValue(this->t_CurStage), 1000.0f, 10000.0f);
                this->m_InterpShiftRPM.Initialize(currpm, currpm, 0, LINEAR);
                break;
            } else {
                this->eShiftState = SHFT_UP_ENGAGING;
                this->eShiftStageChanged = SHFT_UP_ENGAGING;

                FillGraphFromSpline(this->m_pShiftingPatternData->Up_Engage_Curve(), this->m_RPMPoints, 7,
                                    static_cast<float>(this->m_pShiftingPatternData->Up_Engage().Time),
                                    static_cast<float>(this->m_pShiftingPatternData->Up_Engage().RPM));
                this->t_CurStage = 0.0f;
                this->RPMOffset = static_cast<unsigned short>(static_cast<int>(this->m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM()));

                float currpm = bClamp(static_cast<float>(this->RPMOffset) + this->m_RPMGraph.GetValue(this->t_CurStage), 1000.0f, 10000.0f);
                this->m_InterpShiftRPM.Initialize(currpm, currpm, 0, LINEAR);

                int CurGear = bClamp(this->m_pEAXCar->GetCurGear() + Sound::SPORT_SHIFT, 0, 3);
                this->m_InterpShiftTorque.Initialize(
                    UP_SHIFTING_TRQ_ATTACH_INITIAL_PERCENT[CurGear] * this->m_pEngineCtl->m_pPhysicsCtl->GetPhysTRQ(),
                    this->m_pEngineCtl->m_pPhysicsCtl->GetPhysTRQ(), UP_SHIFTING_TRQ_ATTACK_TIME[CurGear], LINEAR);

                this->m_InterpShiftVol.Initialize(this->m_pShiftingPatternData->Up_Engaging_Attack_Vol(), 0.0f,
                                            static_cast<int>(this->m_pShiftingPatternData->Up_Engaging_Attack_T()), LINEAR);

                this->PostShiftFX_Init();
                this->PostShiftFX_Update(t);
                *static_cast<int *>(static_cast<void *>(&this->m_bNeed_EngageSnd)) = 1;
                return;
            }
        }

        float currpm = bClamp(static_cast<float>(this->RPMOffset) + this->m_RPMGraph.GetValue(this->t_CurStage), 1000.0f, 10000.0f);
        this->m_InterpShiftRPM.Initialize(currpm, currpm, 0, LINEAR);
        break;
    }
    case SHFT_UP_ENGAGING: {
        if (this->t_CurStage > this->m_RPMPoints[6].x) {
            this->eShiftState = SHFT_UP_LFO;
            this->eShiftStageChanged = SHFT_UP_LFO;
            return;
        }
        this->RPMOffset = static_cast<unsigned short>(static_cast<int>(this->m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM()));
        float currpm = bClamp(static_cast<float>(this->RPMOffset) + this->m_RPMGraph.GetValue(this->t_CurStage), 1000.0f, 10000.0f);
        this->m_InterpShiftRPM.Initialize(currpm, currpm, 0, LINEAR);
        break;
    }
    case SHFT_UP_LFO:
        if ((!this->m_Shift_RPM_AMP_DECAY.IsFinished() || !this->m_Shift_VOL_AMP_DECAY.IsFinished()) && this->eShift_LFO != SHIFT_LFO_NONE) {
            return;
        }
        this->CleanUpShiftFX();
        break;
    case SHFT_DOWN_DISENGAGE: {
        if (!this->m_InterpShiftRPM.IsFinished()) {
            return;
        }

        this->eShiftState = SHFT_DOWN_ENGAGING_RISE;
        this->eShiftStageChanged = SHFT_DOWN_ENGAGING_RISE;

        int Length = static_cast<int>(this->m_pShiftingPatternData->Down_Engaging_Rise_T());
        if (this->m_pEngineCtl->m_pPhysicsCtl->m_CurGear < Sound::SECOND_GEAR) {
            Length = static_cast<int>(static_cast<float>(Length) * 0.7f);
        }

        float LowRPMScale = 1.0f;
        if (this->m_pEngineCtl->GetEngRPM() < 1500.0f) {
            LowRPMScale = 0.0f;
        }
        float TargetRPM = bClamp(this->m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM() +
                                       static_cast<float>(this->m_pShiftingPatternData->Down_Engaging_Rise_RPM()) *
                                           LowRPMScale,
                                 1000.0f, 10000.0f);
        this->m_InterpShiftRPM.Initialize(this->m_pEngineCtl->GetEngRPM(), TargetRPM, Length, EQ_PWR_SQ);
        this->m_InterpShiftTorque.Initialize(this->m_pEngineCtl->GetEngTorque(), DOWN_SHIFTING_REV_PERCENT * LowRPMScale,
                                       DOWN_SHIFTING_REV_RAMP_TIME, LINEAR);
        break;
    }
    case SHFT_DOWN_ENGAGING_RISE: {
        if (!this->m_InterpShiftRPM.IsFinished()) {
            return;
        }

        this->eShiftState = SHFT_DOWN_ENGAGING_FALL;
        this->eShiftStageChanged = SHFT_DOWN_ENGAGING_FALL;

        int Length = static_cast<int>(this->m_pShiftingPatternData->Down_Engaging_Fall_T());
        if (this->GetCurGear() < Sound::SECOND_GEAR && this->m_UGL < AEMS_LEVEL2) {
            Length = static_cast<int>(static_cast<float>(Length) * 0.7f);
        }

        float LowRPMScale = 1.0f;
        if (this->m_pEngineCtl->GetEngRPM() < 1500.0f) {
            LowRPMScale = 0.0f;
        }
        this->m_InterpShiftRPM.Initialize(this->m_pEngineCtl->GetEngRPM(),
                                    this->m_pEngineCtl->GetEngRPM() -
                                        static_cast<float>(this->m_pShiftingPatternData->Down_Engaging_Fall_RPM()) * LowRPMScale,
                                    Length,
                                    LINEAR);
        this->m_InterpShiftTorque.Initialize(DOWN_SHIFTING_REV_PERCENT * LowRPMScale, 0.0f,
                                       DOWN_SHIFTING_REV_RAMP_TIME, LINEAR);
        break;
    }
    case SHFT_DOWN_ENGAGING_FALL: {
        if (!this->m_InterpShiftRPM.IsFinished()) {
            return;
        }

        this->eShiftState = SHFT_DOWN_ENGAGING_REATTACH;
        this->eShiftStageChanged = SHFT_DOWN_ENGAGING_REATTACH;

        float AttachTime =
            bClamp(bAbs((this->m_pEngineCtl->GetEngRPM() - this->m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM()) *
                        this->m_pShiftingPatternData->Down_Reattach_Scale()),
                   0.0f, 800.0f);
        this->m_InterpShiftRPM.Initialize(this->m_pEngineCtl->GetEngRPM(), this->m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM(),
                                    static_cast<int>(AttachTime), EQ_PWR_SQ);
        this->m_InterpShiftTorque.Initialize(0.0f, this->m_pEngineCtl->m_pPhysicsCtl->GetPhysTRQ(), 0x3C, LINEAR);
        break;
    }
    case SHFT_DOWN_ENGAGING_REATTACH:
        if (this->m_InterpShiftRPM.IsFinished()) {
            this->CleanUpShiftFX();
        }
        break;
    default:
        break;
    }
}

void SFXCTL_Shifting::UpdateParams(float t) {
    SFXCTL::UpdateParams(t);
    this->m_bNeed_DeccelSnd = false;
    this->m_bNeed_AccelSnd = false;
    this->m_bNeed_ShiftGearSnd = false;
    this->m_bNeed_DisengageSnd = false;
    this->m_bNeed_EngageSnd = false;

    if (this->GetCurGear() > this->GetLastGear()) {
        this->BeginUpShift();
    } else if (this->GetCurGear() < this->GetLastGear()) {
        this->BeginDownShift();
    }

    this->UpdateGearShiftState(t);
}

void SFXCTL_Shifting::UpdateTorque(float t) {
    switch (this->eShiftState) {
    case SHFT_UP_DISENGAGE:
    case SHFT_DOWN_DISENGAGE:
    case SHFT_DOWN_ENGAGING_RISE:
    case SHFT_DOWN_ENGAGING_FALL:
        this->m_InterpShiftTorque.Update(t);
        return;
    case SHFT_UP_ENGAGING:
    case SHFT_UP_LFO:
    case SHFT_DOWN_ENGAGING_REATTACH:
        this->m_InterpShiftTorque.Update(t, this->m_pEngineCtl->m_pPhysicsCtl->GetPhysTRQ());
        return;
    default:
        return;
    }
}

void SFXCTL_Shifting::UpdateRPM(float t) {
    if (!this->m_VisualRPM.IsFinished()) {
        this->m_VisualRPM.Update(t, this->m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM());
    }

    switch (this->eShiftState) {
    case SHFT_UP_DISENGAGE:
    case SHFT_UP_ENGAGING:
    case SHFT_DOWN_DISENGAGE:
    case SHFT_DOWN_ENGAGING_RISE:
    case SHFT_DOWN_ENGAGING_FALL:
        this->m_InterpShiftRPM.Update(t);
        return;
    case SHFT_UP_LFO:
    case SHFT_DOWN_ENGAGING_REATTACH:
        this->m_InterpShiftRPM.Update(t, this->m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM());
        return;
    default:
        return;
    }
}

float SFXCTL_Shifting::GetShiftingRPM() { return this->m_InterpShiftRPM.GetValue(); }

float SFXCTL_Shifting::GetShiftingTRQ() { return this->m_InterpShiftTorque.GetValue(); }

float SFXCTL_Shifting::GetShiftingVOL() { return this->m_InterpShiftVol.GetValue(); }

void SFXCTL_Shifting::BeginUpShift() {
    float TotalDuration;

    this->CleanUpShiftFX();

    if (3000.0f <= this->m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM()) {
        this->eShiftState = SHFT_UP_DISENGAGE;
        this->eShiftStageChanged = SHFT_UP_DISENGAGE;

        this->RPM_AtShift = this->m_pEngineCtl->m_fPrevRPM;
        this->m_pEngineCtl->m_fPrevRPM = this->m_pEngineCtl->m_fEng_RPM;
        this->m_pEngineCtl->SetEngRPM(this->RPM_AtShift);
        this->m_pEngineCtl->m_fSmoothedEng_RPM = this->m_pEngineCtl->m_fSmoothedEng_RPM * 0.95f + this->RPM_AtShift * 0.05f;
        this->t_Last_Shift = SndBase::m_fRunningTime;

        const UMath::Matrix4 &curvespline = this->m_pShiftingPatternData->Up_DisengageFall_Curve(0);
        FillGraphFromSpline(curvespline, this->m_RPMPoints, 7,
                            static_cast<float>(this->m_pShiftingPatternData->Up_DisengageFall(0).Time),
                            static_cast<float>(this->m_pShiftingPatternData->Up_DisengageFall(0).RPM));

        this->m_CurStage = 0;
        this->t_CurStage = 0.0f;
        this->RPMOffset = static_cast<unsigned short>(static_cast<int>(this->RPM_AtShift));

        this->m_InterpShiftRPM.Initialize(this->RPM_AtShift, this->RPM_AtShift, 0, LINEAR);
        this->m_InterpShiftVol.Initialize(0.0f, 0.0f, 1, LINEAR);
        this->m_InterpShiftTorque.Initialize(this->m_pEngineCtl->m_pPhysicsCtl->GetPhysTRQ(), 0.0f, 100, LINEAR);

        if (this->m_pShiftingPatternData->Num_Up_DisengageFall() == 1) {
            TotalDuration = static_cast<float>(this->m_pShiftingPatternData->Up_DisengageFall(0).Time);
        } else {
            TotalDuration = static_cast<float>(this->m_pShiftingPatternData->Up_DisengageFall(0).Time) +
                            static_cast<float>(this->m_pShiftingPatternData->Up_DisengageFall(1).Time);
        }

        TotalDuration += static_cast<float>(this->m_pShiftingPatternData->Up_Engage().Time);
        this->m_VisualRPM.Initialize(this->RPM_AtShift, this->RPM_AtShift - 500.0f, static_cast<int>(TotalDuration), EQ_PWR_SQ);

        *static_cast<int *>(static_cast<void *>(&this->m_bNeed_DisengageSnd)) = 1;
        *static_cast<int *>(static_cast<void *>(&this->m_bPendingNeedShiftSound)) = 1;
        this->tShiftDelay = SndBase::m_fRunningTime;
        this->tShiftDelay += this->m_pShiftingPatternData->Up_Shift_Sound_Delay();
        this->ShiftType = static_cast<AEMS_SHIFTING_SAMPLES>(1);
    }
}

void FillGraphFromSpline(const UMath::Matrix4 &matrix, bVector2 *points, int num_points, float XScale, float YScale) {
    if (num_points > 0) {
        float denom = static_cast<float>(num_points - 1);
        for (int n = 0; n < num_points; ++n) {
            UMath::Vector4 point;
            UBezierLite::Evaluate(matrix, static_cast<float>(n) / denom, point);
            points[n].x = point.x * XScale;
            points[n].y = point.y * YScale;
        }
    }
}

void SFXCTL_Shifting::BeginDownShift() {
    float TargetRPM;

    if (this->GetCurGear() != static_cast<Gear>(1)) {
        this->CleanUpShiftFX();
        this->eShiftState = SHFT_DOWN_DISENGAGE;
        this->eShiftStageChanged = SHFT_DOWN_DISENGAGE;

        this->RPM_AtShift = this->m_pEngineCtl->m_fPrevRPM;
        this->m_pEngineCtl->m_fPrevRPM = this->m_pEngineCtl->m_fEng_RPM;
        this->m_pEngineCtl->SetEngRPM(this->RPM_AtShift);
        this->m_pEngineCtl->m_fSmoothedEng_RPM = this->m_pEngineCtl->m_fSmoothedEng_RPM * 0.95f + this->RPM_AtShift * 0.05f;
        this->t_Last_Shift = SndBase::m_fRunningTime;

        this->m_InterpShiftTorque.Initialize(this->m_pEngineCtl->m_pPhysicsCtl->GetPhysTRQ(), 0.0f, 0x32, LINEAR);

        TargetRPM = this->RPM_AtShift - static_cast<float>(this->m_pShiftingPatternData->Down_Disengage_Fall_RPM());
        TargetRPM = bClamp(TargetRPM, 1000.0f, 10000.0f);
        this->m_InterpShiftRPM.Initialize(this->RPM_AtShift, TargetRPM,
                                    static_cast<int>(this->m_pShiftingPatternData->Down_Disengage_Fall_T()), LINEAR);
        this->m_InterpShiftVol.Initialize(0.0f, 0.0f, 1, LINEAR);

        this->tShiftDelay = SndBase::m_fRunningTime + this->m_pShiftingPatternData->Down_Shift_Sound_Delay();
        if (3000.0f < this->m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM()) {
            *static_cast<int *>(static_cast<void *>(&this->m_bPendingNeedShiftSound)) = 1;
        }

        this->ShiftType = static_cast<AEMS_SHIFTING_SAMPLES>(0);
    }
}

void SFXCTL_Shifting::PostShiftFX_Update(float t) {
    if (this->eShift_LFO != SHIFT_LFO_NONE) {
        this->m_Shift_RPM_AMP_DECAY.Update(t);
        this->m_Shift_VOL_AMP_DECAY.Update(t);
        if (this->m_Shift_RPM_AMP_DECAY.IsFinished()) {
        }
        if (this->m_Shift_RPM_AMP_DECAY.IsFinished()) {
            this->CleanUpShiftFX();
            return;
        }

        this->m_RPM_LFO_AMP = static_cast<int>(this->m_Shift_RPM_AMP_DECAY.GetValue());
        this->m_RPM_LFO_FRQ = static_cast<int>(this->m_pShiftingPatternData->LFO_RPM_Freq());
        this->m_VOL_LFO_AMP = static_cast<int>(this->m_Shift_VOL_AMP_DECAY.GetValue());
        this->m_VOL_LFO_FRQ = static_cast<int>(this->m_pShiftingPatternData->LFO_Vol_Freq());
    }
}

void SFXCTL_Shifting::PostShiftFX_End() {
    this->eShift_LFO = SHIFT_LFO_NONE;
    this->m_VOL_LFO_AMP = 0;
    this->m_VOL_LFO_FRQ = 0;
    this->m_TRQ_LFO_AMP = 0;
    this->m_TRQ_LFO_FRQ = 0;
    this->m_RPM_LFO_AMP = 0;
    this->m_RPM_LFO_FRQ = 0;
}

void SFXCTL_Shifting::PostShiftFX_Init() {
    float ScaleDown;

    if (this->GetCurGear() < Sound::SIXTH_GEAR) {
        ScaleDown = 1.0f;
        if (this->GetCurGear() == Sound::SECOND_GEAR) {
            ScaleDown = 0.85f;
        }
        if (this->GetCurGear() == Sound::THIRD_GEAR) {
            ScaleDown = 0.7f;
        }
        if (this->GetCurGear() == Sound::FOURTH_GEAR) {
            ScaleDown = 0.55f;
        }
        if (this->GetCurGear() == Sound::FIFTH_GEAR) {
            ScaleDown = 0.3f;
        }

        this->eShift_LFO = SHIFT_LFO_ON;
        this->m_Shift_RPM_AMP_DECAY.Initialize(static_cast<float>(this->m_pShiftingPatternData->LFO_RPM_Amp()) * ScaleDown, 0.0f,
                                         static_cast<int>(this->m_pShiftingPatternData->LFO_RPM_Decay_Time()), LINEAR);
        this->m_Shift_VOL_AMP_DECAY.Initialize(static_cast<float>(this->m_pShiftingPatternData->LFO_Vol_Amp()), 0.0f,
                                         static_cast<int>(this->m_pShiftingPatternData->LFO_Vol_Decay_Time()), LINEAR);
    }
}

void SFXCTL_Shifting::CleanUpShiftFX() {
    this->PostShiftFX_End();
    this->eShiftState = static_cast<SHIFT_STAGE>(0);
    this->eShiftStageChanged = static_cast<SHIFT_STAGE>(0);
}

Gear SFXCTL_Shifting::GetCurGear() { return this->m_pEngineCtl->m_pPhysicsCtl->m_CurGear; }

Gear SFXCTL_Shifting::GetLastGear() { return this->m_pEngineCtl->m_pPhysicsCtl->m_LastGear; }
