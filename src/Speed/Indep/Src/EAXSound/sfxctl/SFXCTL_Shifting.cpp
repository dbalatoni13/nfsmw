#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Physics.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Libs/Support/Utility/UBezierLite.hpp"

float DOWN_SHIFTING_REV_PERCENT = 100.0f; // size: 0x4, address: 0x80417958, Decl: 12

int DOWN_SHIFTING_REV_RAMP_TIME = 215; // size: 0x4, address: 0x8041795C, Decl: 13

static const float UP_SHIFTING_TRQ_ATTACH_INITIAL_PERCENT[4] = {1.0f, 1.0f, 1.0f, 1.0f}; // size: 0x10, address: 0x803D8AE8, Decl: 15

static const int UP_SHIFTING_TRQ_ATTACK_TIME[4] = {0, 0, 0, 0}; // size: 0x10, address: 0x803D8AF8, Decl: 23

static const float MIN_RPM_FOR_SHIFT_FX = 3000.0f; // size: 0x4, Decl: 31

DEFINE_CREATABLE(0x20020, SFXCTL_Shifting, SFXCTL);

SFXCTL_Shifting::SFXCTL_Shifting() : m_RPMGraph(m_RPMPoints, 7) {
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
    if (psfxctl->GetObjectIndex() == 4) {
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
    this->ShiftType = AEMS_SHIFTING_UP;
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

// STRIPPED
void SFXCTL_Shifting::SetupGraph(int rpm) {}

void FillGraphFromSpline(const UMath::Matrix4 &matrix, bVector2 *points, int num_points, float XScale, float YScale);

// TODO dwarf
void SFXCTL_Shifting::UpdateGearShiftState(float t) {
    if (SndBase::m_fRunningTime > this->tShiftDelay && this->m_bPendingNeedShiftSound) {
        this->m_bNeed_ShiftGearSnd = true;
        this->m_bPendingNeedShiftSound = false;
    }

    this->m_bShouldBeWhining = this->GetCurGear() == REVERSE;

    EAX_CarState *carstate = this->m_pEAXCar->GetPhysCar();
    if (carstate->IsLocalPlayerCar()) {
        float t_last_mashed = (WorldTimer - this->m_timeBrakeLastMashed).GetSeconds();
        static float prevbrakestate = carstate->GetBrake();

        if (carstate->GetBrake() >= 1.0f && !this->m_bBrakePedalMashed && t_last_mashed > 1.0f && prevbrakestate == 0.0f &&
            carstate->GetVelocityMagnitudeMPH() > 5.0f) {
            this->m_bBrakePedalMashed = true;
        } else if (this->m_bBrakePedalMashed) {
            this->m_timeBrakeLastMashed = WorldTimer;
            if (carstate->GetBrake() == 0.0f) {
                this->m_bBrakePedalMashed = false;
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
                }

                this->eShiftState = SHFT_UP_ENGAGING;
                this->eShiftStageChanged = SHFT_UP_ENGAGING;

                const UMath::Matrix4 &curvespline = this->m_pShiftingPatternData->Up_Engage_Curve();

                FillGraphFromSpline(curvespline, this->m_RPMPoints, 7, static_cast<float>(this->m_pShiftingPatternData->Up_Engage().Time),
                                    static_cast<float>(this->m_pShiftingPatternData->Up_Engage().RPM));

                this->t_CurStage = 0.0f;
                this->RPMOffset = static_cast<unsigned short>(static_cast<int>(this->m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM()));

                float currpm = bClamp(static_cast<float>(this->RPMOffset) + this->m_RPMGraph.GetValue(this->t_CurStage), 1000.0f, 10000.0f);

                this->m_InterpShiftRPM.Initialize(currpm, currpm, 0, LINEAR);

                int CurGear = bClamp(this->m_pEAXCar->GetCurGear() + SPORT_SHIFT, 0, 3);

                this->m_InterpShiftTorque.Initialize(UP_SHIFTING_TRQ_ATTACH_INITIAL_PERCENT[CurGear] *
                                                         this->m_pEngineCtl->m_pPhysicsCtl->GetPhysTRQ(),
                                                     this->m_pEngineCtl->m_pPhysicsCtl->GetPhysTRQ(), UP_SHIFTING_TRQ_ATTACK_TIME[CurGear], LINEAR);

                this->m_InterpShiftVol.Initialize(this->m_pShiftingPatternData->Up_Engaging_Attack_Vol(), 0.0f,
                                                  static_cast<int>(this->m_pShiftingPatternData->Up_Engaging_Attack_T()), LINEAR);

                this->PostShiftFX_Init();
                this->PostShiftFX_Update(t);
                this->m_bNeed_EngageSnd = true;
                return;
            }

            // TODO remove
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

        case SHFT_DOWN_DISENGAGE:
            if (this->m_InterpShiftRPM.IsFinished()) {
                this->eShiftState = SHFT_DOWN_ENGAGING_RISE;
                this->eShiftStageChanged = SHFT_DOWN_ENGAGING_RISE;

                int Length = static_cast<int>(this->m_pShiftingPatternData->Down_Engaging_Rise_T());
                if (this->m_pEngineCtl->m_pPhysicsCtl->m_CurGear < SECOND_GEAR) {
                    Length = static_cast<int>(Length * 0.7f);
                }

                float LowRPMScale = 1.0f;
                if (this->m_pEngineCtl->GetEngRPM() < 1500.0f) {
                    LowRPMScale = 0.0f;
                }

                float TargetRPM = bClamp(this->m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM() +
                                             static_cast<float>(this->m_pShiftingPatternData->Down_Engaging_Rise_RPM()) * LowRPMScale,
                                         1000.0f, 10000.0f);

                this->m_InterpShiftRPM.Initialize(this->m_pEngineCtl->GetEngRPM(), TargetRPM, Length, EQ_PWR_SQ);

                this->m_InterpShiftTorque.Initialize(this->m_pEngineCtl->GetEngTorque(), DOWN_SHIFTING_REV_PERCENT * LowRPMScale,
                                                     DOWN_SHIFTING_REV_RAMP_TIME, LINEAR);
            }
            break;

        case SHFT_DOWN_ENGAGING_RISE:
            if (this->m_InterpShiftRPM.IsFinished()) {
                this->eShiftState = SHFT_DOWN_ENGAGING_FALL;
                this->eShiftStageChanged = SHFT_DOWN_ENGAGING_FALL;

                int Length = static_cast<int>(this->m_pShiftingPatternData->Down_Engaging_Fall_T());
                if (this->GetCurGear() < SECOND_GEAR && this->m_UGL < AEMS_LEVEL2) {
                    Length = static_cast<int>(Length * 0.7f);
                }

                float LowRPMScale = 1.0f;
                if (this->m_pEngineCtl->GetEngRPM() < 1500.0f) {
                    LowRPMScale = 0.0f;
                }

                this->m_InterpShiftRPM.Initialize(this->m_pEngineCtl->GetEngRPM(),
                                                  this->m_pEngineCtl->GetEngRPM() -
                                                      static_cast<float>(this->m_pShiftingPatternData->Down_Engaging_Fall_RPM()) * LowRPMScale,
                                                  Length, LINEAR);

                this->m_InterpShiftTorque.Initialize(DOWN_SHIFTING_REV_PERCENT * LowRPMScale, 0.0f, DOWN_SHIFTING_REV_RAMP_TIME, LINEAR);
            }
            break;

        case SHFT_DOWN_ENGAGING_FALL:
            if (this->m_InterpShiftRPM.IsFinished()) {
                this->eShiftState = SHFT_DOWN_ENGAGING_REATTACH;
                this->eShiftStageChanged = SHFT_DOWN_ENGAGING_REATTACH;

                float AttachTime = bClamp(bAbs((this->m_pEngineCtl->GetEngRPM() - this->m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM()) *
                                               this->m_pShiftingPatternData->Down_Reattach_Scale()),
                                          0.0f, 800.0f);

                this->m_InterpShiftRPM.Initialize(this->m_pEngineCtl->GetEngRPM(), this->m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM(),
                                                  static_cast<int>(AttachTime), EQ_PWR_SQ);

                this->m_InterpShiftTorque.Initialize(0.0f, this->m_pEngineCtl->m_pPhysicsCtl->GetPhysTRQ(), 60, LINEAR);
            }
            break;

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
            break;

        case SHFT_UP_ENGAGING:
        case SHFT_UP_LFO:
        case SHFT_DOWN_ENGAGING_REATTACH:
            this->m_InterpShiftTorque.Update(t, this->m_pEngineCtl->m_pPhysicsCtl->GetPhysTRQ());
            break;

        default:
            break;
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
            break;

        case SHFT_UP_LFO:
        case SHFT_DOWN_ENGAGING_REATTACH:
            this->m_InterpShiftRPM.Update(t, this->m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM());
            break;

        default:
            break;
    }
}

float SFXCTL_Shifting::GetShiftingRPM() {
    return this->m_InterpShiftRPM.GetValue();
}

float SFXCTL_Shifting::GetShiftingTRQ() {
    return this->m_InterpShiftTorque.GetValue();
}

float SFXCTL_Shifting::GetShiftingVOL() {
    return this->m_InterpShiftVol.GetValue();
}

void SFXCTL_Shifting::BeginUpShift() {
    this->CleanUpShiftFX();

    if (3000.0f > this->m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM()) {
        return;
    }

    this->eShiftState = SHFT_UP_DISENGAGE;
    this->eShiftStageChanged = SHFT_UP_DISENGAGE;

    this->RPM_AtShift = this->m_pEngineCtl->m_fPrevRPM;
    this->m_pEngineCtl->SetEngRPM(this->RPM_AtShift);
    this->t_Last_Shift = SndBase::m_fRunningTime;

    const UMath::Matrix4 &curvespline = this->m_pShiftingPatternData->Up_DisengageFall_Curve(0);

    FillGraphFromSpline(curvespline, this->m_RPMPoints, 7, static_cast<float>(this->m_pShiftingPatternData->Up_DisengageFall(0).Time),
                        static_cast<float>(this->m_pShiftingPatternData->Up_DisengageFall(0).RPM));

    this->m_CurStage = 0;
    this->t_CurStage = 0.0f;
    this->RPMOffset = static_cast<unsigned short>(static_cast<int>(this->RPM_AtShift));

    this->m_InterpShiftRPM.Initialize(this->RPM_AtShift, this->RPM_AtShift, 0, LINEAR);
    this->m_InterpShiftVol.Initialize(0.0f, 0.0f, 1, LINEAR);
    this->m_InterpShiftTorque.Initialize(this->m_pEngineCtl->m_pPhysicsCtl->GetPhysTRQ(), 0.0f, 100, LINEAR);

    float TotalDuration;
    if (this->m_pShiftingPatternData->Num_Up_DisengageFall() == 1) {
        TotalDuration = static_cast<float>(this->m_pShiftingPatternData->Up_DisengageFall(0).Time);
    } else {
        TotalDuration = static_cast<float>(this->m_pShiftingPatternData->Up_DisengageFall(0).Time) +
                        static_cast<float>(this->m_pShiftingPatternData->Up_DisengageFall(1).Time);
    }

    TotalDuration += static_cast<float>(this->m_pShiftingPatternData->Up_Engage().Time);

    this->m_VisualRPM.Initialize(this->RPM_AtShift, this->RPM_AtShift - 1200.0f, static_cast<int>(TotalDuration), EQ_PWR_SQ);

    this->tShiftDelay = SndBase::m_fRunningTime + this->m_pShiftingPatternData->Up_Shift_Sound_Delay();

    this->m_bPendingNeedShiftSound = true;
    this->ShiftType = AEMS_SHIFTING_UP;
    this->m_bNeed_DisengageSnd = true;
}

void FillGraphFromSpline(const UMath::Matrix4 &matrix, bVector2 *points, int num_points, float XScale, float YScale) {
    for (int n = 0; n < num_points; ++n) {
        float t = static_cast<float>(n) / static_cast<float>(num_points - 1);

        UMath::Vector4 point;
        UBezierLite::Evaluate(matrix, t, point);
        points[n].x = point.x * XScale;
        points[n].y = point.y * YScale;
    }
}

void SFXCTL_Shifting::BeginDownShift() {
    if (this->GetCurGear() == NEUTRAL) {
        return;
    }

    this->CleanUpShiftFX();
    this->eShiftState = SHFT_DOWN_DISENGAGE;
    this->eShiftStageChanged = SHFT_DOWN_DISENGAGE;

    this->RPM_AtShift = this->m_pEngineCtl->m_fPrevRPM;
    this->m_pEngineCtl->SetEngRPM(this->RPM_AtShift);
    this->t_Last_Shift = SndBase::m_fRunningTime;

    this->m_InterpShiftTorque.Initialize(this->m_pEngineCtl->m_pPhysicsCtl->GetPhysTRQ(), 0.0f, 0x32, LINEAR);

    float TargetRPM = bClamp(this->RPM_AtShift - static_cast<float>(this->m_pShiftingPatternData->Down_Disengage_Fall_RPM()), 1000.0f, 10000.0f);
    this->m_InterpShiftRPM.Initialize(this->RPM_AtShift, TargetRPM, static_cast<int>(this->m_pShiftingPatternData->Down_Disengage_Fall_T()), LINEAR);
    this->m_InterpShiftVol.Initialize(0.0f, 0.0f, 1, LINEAR);

    this->tShiftDelay = SndBase::m_fRunningTime + this->m_pShiftingPatternData->Down_Shift_Sound_Delay();
    if (MIN_RPM_FOR_SHIFT_FX < this->m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM()) {
        this->m_bPendingNeedShiftSound = true;
    }

    this->ShiftType = static_cast<AEMS_SHIFTING_SAMPLES>(0);
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

static const float SECOND_GEAR_LFO_SCALE = 0.85f; // size: 0x4, Decl: 657
static const float THIRD_GEAR_LFO_SCALE = 0.7f;   // size: 0x4, Decl: 658
static const float FOURTH_GEAR_LFO_SCALE = 0.55f; // size: 0x4, Decl: 659
static const float FIFTH_GEAR_LFO_SCALE = 0.3f;   // size: 0x4, Decl: 660

void SFXCTL_Shifting::PostShiftFX_Init() {
    if (this->GetCurGear() >= SIXTH_GEAR) {
        return;
    }

    float ScaleDown = 1.0f;
    if (this->GetCurGear() == SECOND_GEAR) {
        ScaleDown = SECOND_GEAR_LFO_SCALE;
    }
    if (this->GetCurGear() == THIRD_GEAR) {
        ScaleDown = THIRD_GEAR_LFO_SCALE;
    }
    if (this->GetCurGear() == FOURTH_GEAR) {
        ScaleDown = FOURTH_GEAR_LFO_SCALE;
    }
    if (this->GetCurGear() == FIFTH_GEAR) {
        ScaleDown = FIFTH_GEAR_LFO_SCALE;
    }

    this->eShift_LFO = SHIFT_LFO_ON;
    this->m_Shift_RPM_AMP_DECAY.Initialize(static_cast<float>(this->m_pShiftingPatternData->LFO_RPM_Amp()) * ScaleDown, 0.0f,
                                           static_cast<int>(this->m_pShiftingPatternData->LFO_RPM_Decay_Time()), LINEAR);
    this->m_Shift_VOL_AMP_DECAY.Initialize(static_cast<float>(this->m_pShiftingPatternData->LFO_Vol_Amp()), 0.0f,
                                           static_cast<int>(this->m_pShiftingPatternData->LFO_Vol_Decay_Time()), LINEAR);
}

void SFXCTL_Shifting::CleanUpShiftFX() {
    this->PostShiftFX_End();
    this->eShiftState = SHFT_NONE;
    this->eShiftStageChanged = SHFT_NONE;
}

Gear SFXCTL_Shifting::GetCurGear() {
    return this->m_pEngineCtl->m_pPhysicsCtl->m_CurGear;
}

Gear SFXCTL_Shifting::GetLastGear() {
    return this->m_pEngineCtl->m_pPhysicsCtl->m_LastGear;
}

DEFINE_CREATABLE(0x30020, SFXCTL_AIShifting, SFXCTL_Shifting);
