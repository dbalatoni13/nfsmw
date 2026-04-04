#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"
#include "Speed/Indep/Libs/Support/Utility/UBezierLite.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

void FillGraphFromSpline(const UMath::Matrix4 &matrix, bVector2 *points, int num_points, float XScale, float YScale);

static const float kDownShiftingRevPercent = 0.5f;
static const int kDownShiftingRevRampTime = 0x32;
static const int kUpShiftTrqAttackTime[4] = {0x64, 0x64, 0x64, 0x64};
static const float kUpShiftTrqAttachInitialPercent[4] = {0.0f, 0.0f, 0.0f, 0.0f};

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
    m_pShiftingPatternData = nullptr;
}

SFXCTL_Shifting::~SFXCTL_Shifting() {}

void SFXCTL_Shifting::UpdateMixerOutputs() {
    SetDMIX_Input(0, (eShiftState == SHFT_UP_DISENGAGE) ? 0x7FFF : 0);
    SetDMIX_Input(1, (eShiftStageChanged == SHFT_UP_ENGAGING) ? 0x7FFF : 0);
    SetDMIX_Input(2, IsDownShifting() ? 0x7FFF : 0);
}

int SFXCTL_Shifting::GetController(int Index) {
    if (Index != 0) {
        return -1;
    }
    return 4;
}

void SFXCTL_Shifting::AttachController(SFXCTL *psfxctl) {
    if ((psfxctl->GetObjectIndex()) == 4) {
        m_pEngineCtl = static_cast<SFXCTL_Engine *>(psfxctl);
    }
}

void SFXCTL_Shifting::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    m_UGL = m_pEAXCar->m_TransmissionUGL;
    m_pShiftingPatternData = &m_pEAXCar->GetShiftInfo();
    m_nPostShiftFXLevel = m_UGL % 2;
}

void SFXCTL_Shifting::InitSFX() {
    SFXCTL::InitSFX();
    ShiftType = static_cast<AEMS_SHIFTING_SAMPLES>(1);
    m_VOL_LFO_AMP = 0;
    m_VOL_LFO_FRQ = 0;
    m_TRQ_LFO_AMP = 0;
    m_TRQ_LFO_FRQ = 0;
    m_RPM_LFO_AMP = 0;
    m_RPM_LFO_FRQ = 0;
    eShiftState = SHFT_NONE;
    eShiftStageChanged = SHFT_NONE;
    eShift_LFO = SHIFT_LFO_NONE;
    m_bPendingNeedShiftSound = false;
    m_bShouldBeWhining = false;
    m_bBrakePedalMashed = false;
    tShiftDelay = 0.0f;
    m_nPostShiftFXLevel = 0;
    m_bNeed_DeccelSnd = false;
    t_Last_Shift = 0.0f;
    m_bNeed_AccelSnd = false;
    m_timeBrakeLastMashed = Timer(0);
}

void SFXCTL_Shifting::UpdateGearShiftState(float t) {
    static int _brakeInit = 0;
    static float _prevBrakeState = 0.0f;

    if (SndBase::m_fRunningTime > tShiftDelay &&
        *static_cast<int *>(static_cast<void *>(&m_bPendingNeedShiftSound)) != 0) {
        *static_cast<int *>(static_cast<void *>(&m_bNeed_ShiftGearSnd)) = 1;
        *static_cast<int *>(static_cast<void *>(&m_bPendingNeedShiftSound)) = 0;
    }

    *static_cast<int *>(static_cast<void *>(&m_bShouldBeWhining)) = (static_cast<int>(GetCurGear()) == 0);

    EAX_CarState *carstate = m_pEAXCar->GetPhysCar();
    if (carstate->IsLocalPlayerCar()) {
        float t_last_mashed = (WorldTimer - m_timeBrakeLastMashed).GetSeconds();

        if (_brakeInit == 0) {
            _prevBrakeState = carstate->mBrake;
            _brakeInit = 1;
        }

        int BrakePedalMashed = *static_cast<int *>(static_cast<void *>(&m_bBrakePedalMashed));
        if (carstate->mBrake >= 1.0f &&
            BrakePedalMashed == 0 &&
            t_last_mashed > 1.0f &&
            _prevBrakeState == 0.0f &&
            carstate->GetVelocityMagnitudeMPH() > 5.0f) {
            BrakePedalMashed = 1;
            *static_cast<int *>(static_cast<void *>(&m_bBrakePedalMashed)) = 1;
        }

        if (BrakePedalMashed != 0) {
            m_timeBrakeLastMashed = WorldTimer;
            if (carstate->mBrake == 0.0f) {
                *static_cast<int *>(static_cast<void *>(&m_bBrakePedalMashed)) = 0;
            }
        }

        _prevBrakeState = carstate->mBrake;
    }

    eShiftStageChanged = SHFT_NONE;
    if (eShiftState == SHFT_NONE) {
        return;
    }

    UpdateRPM(t);
    UpdateTorque(t);
    m_InterpShiftVol.Update(t);
    PostShiftFX_Update(t);
    t_CurStage = t * 1000.0f + t_CurStage;

    switch (eShiftState) {
    case SHFT_UP_DISENGAGE: {
        if (t_CurStage > m_RPMPoints[6].x) {
            m_CurStage++;
            if (m_CurStage < m_pShiftingPatternData->Num_Up_DisengageFall()) {
                const UMath::Matrix4 &curvespline = m_pShiftingPatternData->Up_DisengageFall_Curve(m_CurStage);

                FillGraphFromSpline(curvespline, m_RPMPoints, 7,
                                    static_cast<float>(m_pShiftingPatternData->Up_DisengageFall(m_CurStage).Time),
                                    static_cast<float>(m_pShiftingPatternData->Up_DisengageFall(m_CurStage).RPM));
                t_CurStage = t * 1000.0f;
                RPMOffset = static_cast<unsigned short>(static_cast<int>(m_InterpShiftRPM.GetValue()));
                float currpm = bClamp(static_cast<float>(RPMOffset) + m_RPMGraph.GetValue(t_CurStage), 1000.0f, 10000.0f);
                m_InterpShiftRPM.Initialize(currpm, currpm, 0, LINEAR);
                break;
            } else {
                eShiftState = SHFT_UP_ENGAGING;
                eShiftStageChanged = SHFT_UP_ENGAGING;

                FillGraphFromSpline(m_pShiftingPatternData->Up_Engage_Curve(), m_RPMPoints, 7,
                                    static_cast<float>(m_pShiftingPatternData->Up_Engage().Time),
                                    static_cast<float>(m_pShiftingPatternData->Up_Engage().RPM));
                t_CurStage = 0.0f;
                RPMOffset = static_cast<unsigned short>(static_cast<int>(m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM()));

                float currpm = bClamp(static_cast<float>(RPMOffset) + m_RPMGraph.GetValue(t_CurStage), 1000.0f, 10000.0f);
                m_InterpShiftRPM.Initialize(currpm, currpm, 0, LINEAR);

                int CurGear = 0;
                int MaxCurGear = 3;
                int ShiftedGear = m_pEAXCar->CurGear + Sound::SPORT_SHIFT;
                if (ShiftedGear > 0) {
                    CurGear = ShiftedGear;
                }
                if (MaxCurGear < CurGear) {
                    CurGear = MaxCurGear;
                }
                m_InterpShiftTorque.Initialize(kUpShiftTrqAttachInitialPercent[CurGear] * m_pEngineCtl->m_pPhysicsCtl->GetPhysTRQ(),
                                               m_pEngineCtl->m_pPhysicsCtl->GetPhysTRQ(), kUpShiftTrqAttackTime[CurGear],
                                               LINEAR);

                m_InterpShiftVol.Initialize(m_pShiftingPatternData->Up_Engaging_Attack_Vol(), 0.0f,
                                            static_cast<int>(m_pShiftingPatternData->Up_Engaging_Attack_T()), LINEAR);

                PostShiftFX_Init();
                PostShiftFX_Update(t);
                *static_cast<int *>(static_cast<void *>(&m_bNeed_EngageSnd)) = 1;
                return;
            }
        }

        float currpm = bClamp(static_cast<float>(RPMOffset) + m_RPMGraph.GetValue(t_CurStage), 1000.0f, 10000.0f);
        m_InterpShiftRPM.Initialize(currpm, currpm, 0, LINEAR);
        break;
    }
    case SHFT_UP_ENGAGING: {
        if (t_CurStage > m_RPMPoints[6].x) {
            eShiftState = SHFT_UP_LFO;
            eShiftStageChanged = SHFT_UP_LFO;
            return;
        }
        RPMOffset = static_cast<unsigned short>(static_cast<int>(m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM()));
        float currpm = bClamp(static_cast<float>(RPMOffset) + m_RPMGraph.GetValue(t_CurStage), 1000.0f, 10000.0f);
        m_InterpShiftRPM.Initialize(currpm, currpm, 0, LINEAR);
        break;
    }
    case SHFT_UP_LFO:
        if ((!m_Shift_RPM_AMP_DECAY.IsFinished() || !m_Shift_VOL_AMP_DECAY.IsFinished()) && eShift_LFO != SHIFT_LFO_NONE) {
            return;
        }
        CleanUpShiftFX();
        break;
    case SHFT_DOWN_DISENGAGE: {
        cInterpLine *InterpShiftRPM = &m_InterpShiftRPM;
        if (!InterpShiftRPM->IsFinished()) {
            return;
        }

        eShiftState = SHFT_DOWN_ENGAGING_RISE;
        eShiftStageChanged = SHFT_DOWN_ENGAGING_RISE;

        int Length = static_cast<int>(m_pShiftingPatternData->Down_Engaging_Rise_T());
        if (m_pEngineCtl->m_pPhysicsCtl->m_CurGear < Sound::SECOND_GEAR) {
            Length = static_cast<int>(static_cast<float>(Length) * 0.7f);
        }

        float LowRPMScale = m_pEngineCtl->GetEngRPM() < 1500.0f ? 0.0f : 1.0f;
        float TargetRPM = bClamp(m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM() +
                                       static_cast<float>(m_pShiftingPatternData->Down_Engaging_Rise_RPM()) *
                                           LowRPMScale,
                                 1000.0f, 10000.0f);
        InterpShiftRPM->Initialize(m_pEngineCtl->GetEngRPM(), TargetRPM, Length, EQ_PWR_SQ);
        m_InterpShiftTorque.Initialize(0.0f, m_pEngineCtl->GetEngTorque(), kDownShiftingRevRampTime, LINEAR);
        break;
    }
    case SHFT_DOWN_ENGAGING_RISE: {
        if (!m_InterpShiftRPM.IsFinished()) {
            return;
        }

        eShiftState = SHFT_DOWN_ENGAGING_FALL;
        eShiftStageChanged = SHFT_DOWN_ENGAGING_FALL;

        int Length = static_cast<int>(m_pShiftingPatternData->Down_Engaging_Fall_T());
        if (GetCurGear() < Sound::SECOND_GEAR && m_UGL < AEMS_LEVEL2) {
            Length = static_cast<int>(static_cast<float>(Length) * 0.7f);
        }

        float LowRPMScale = m_pEngineCtl->GetEngRPM() < 1500.0f ? 0.0f : 1.0f;
        m_InterpShiftRPM.Initialize(m_pEngineCtl->GetEngRPM(),
                                    m_pEngineCtl->GetEngRPM() -
                                        static_cast<float>(m_pShiftingPatternData->Down_Engaging_Fall_RPM()) * LowRPMScale,
                                    Length,
                                    LINEAR);
        m_InterpShiftTorque.Initialize(kDownShiftingRevPercent * LowRPMScale, 0.0f, kDownShiftingRevRampTime, LINEAR);
        break;
    }
    case SHFT_DOWN_ENGAGING_FALL: {
        if (!m_InterpShiftRPM.IsFinished()) {
            return;
        }

        eShiftState = SHFT_DOWN_ENGAGING_REATTACH;
        eShiftStageChanged = SHFT_DOWN_ENGAGING_REATTACH;

        float AttachTime =
            bClamp(bAbs((m_pEngineCtl->GetEngRPM() - m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM()) *
                        m_pShiftingPatternData->Down_Reattach_Scale()),
                   0.0f, 800.0f);
        m_InterpShiftRPM.Initialize(m_pEngineCtl->GetEngRPM(), m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM(),
                                    static_cast<int>(AttachTime), EQ_PWR_SQ);
        m_InterpShiftTorque.Initialize(0.0f, m_pEngineCtl->m_pPhysicsCtl->GetPhysTRQ(), 0x3C, LINEAR);
        break;
    }
    case SHFT_DOWN_ENGAGING_REATTACH:
        if (m_InterpShiftRPM.IsFinished()) {
            CleanUpShiftFX();
        }
        break;
    default:
        break;
    }
}

void SFXCTL_Shifting::UpdateParams(float t) {
    SFXCTL::UpdateParams(t);
    m_bNeed_DeccelSnd = false;
    m_bNeed_AccelSnd = false;
    m_bNeed_ShiftGearSnd = false;
    m_bNeed_DisengageSnd = false;
    m_bNeed_EngageSnd = false;

    if (GetCurGear() > GetLastGear()) {
        BeginUpShift();
    } else if (GetCurGear() < GetLastGear()) {
        BeginDownShift();
    }

    UpdateGearShiftState(t);
}

void SFXCTL_Shifting::UpdateTorque(float t) {
    switch (eShiftState) {
    case SHFT_UP_DISENGAGE:
    case SHFT_DOWN_DISENGAGE:
    case SHFT_DOWN_ENGAGING_RISE:
    case SHFT_DOWN_ENGAGING_FALL:
        m_InterpShiftTorque.Update(t);
        return;
    case SHFT_UP_ENGAGING:
    case SHFT_UP_LFO:
    case SHFT_DOWN_ENGAGING_REATTACH:
        m_InterpShiftTorque.Update(t, m_pEngineCtl->m_pPhysicsCtl->GetPhysTRQ());
        return;
    default:
        return;
    }
}

void SFXCTL_Shifting::UpdateRPM(float t) {
    if (!m_VisualRPM.IsFinished()) {
        m_VisualRPM.Update(t, m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM());
    }

    switch (eShiftState) {
    case SHFT_UP_DISENGAGE:
    case SHFT_UP_ENGAGING:
    case SHFT_DOWN_DISENGAGE:
    case SHFT_DOWN_ENGAGING_RISE:
    case SHFT_DOWN_ENGAGING_FALL:
        m_InterpShiftRPM.Update(t);
        return;
    case SHFT_UP_LFO:
    case SHFT_DOWN_ENGAGING_REATTACH:
        m_InterpShiftRPM.Update(t, m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM());
        return;
    default:
        return;
    }
}

float SFXCTL_Shifting::GetShiftingRPM() { return m_InterpShiftRPM.GetValue(); }

float SFXCTL_Shifting::GetShiftingTRQ() { return m_InterpShiftTorque.GetValue(); }

float SFXCTL_Shifting::GetShiftingVOL() { return m_InterpShiftVol.GetValue(); }

void SFXCTL_Shifting::BeginUpShift() {
    float TotalDuration;

    CleanUpShiftFX();

    if (3000.0f <= m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM()) {
        eShiftState = SHFT_UP_DISENGAGE;
        eShiftStageChanged = SHFT_UP_DISENGAGE;

        RPM_AtShift = m_pEngineCtl->m_fPrevRPM;
        m_pEngineCtl->m_fPrevRPM = m_pEngineCtl->m_fEng_RPM;
        m_pEngineCtl->SetEngRPM(RPM_AtShift);
        m_pEngineCtl->m_fSmoothedEng_RPM = m_pEngineCtl->m_fSmoothedEng_RPM * 0.95f + RPM_AtShift * 0.05f;
        t_Last_Shift = SndBase::m_fRunningTime;

        const UMath::Matrix4 &curvespline = m_pShiftingPatternData->Up_DisengageFall_Curve(0);
        FillGraphFromSpline(curvespline, m_RPMPoints, 7,
                            static_cast<float>(m_pShiftingPatternData->Up_DisengageFall(0).Time),
                            static_cast<float>(m_pShiftingPatternData->Up_DisengageFall(0).RPM));

        m_CurStage = 0;
        t_CurStage = 0.0f;
        RPMOffset = static_cast<unsigned short>(static_cast<int>(RPM_AtShift));

        m_InterpShiftRPM.Initialize(RPM_AtShift, RPM_AtShift, 0, LINEAR);
        m_InterpShiftVol.Initialize(0.0f, 0.0f, 1, LINEAR);
        m_InterpShiftTorque.Initialize(m_pEngineCtl->m_pPhysicsCtl->GetPhysTRQ(), 0.0f, 100, LINEAR);

        if (m_pShiftingPatternData->Num_Up_DisengageFall() == 1) {
            TotalDuration = static_cast<float>(m_pShiftingPatternData->Up_DisengageFall(0).Time);
        } else {
            TotalDuration = static_cast<float>(m_pShiftingPatternData->Up_DisengageFall(0).Time) +
                            static_cast<float>(m_pShiftingPatternData->Up_DisengageFall(1).Time);
        }

        TotalDuration += static_cast<float>(m_pShiftingPatternData->Up_Engage().Time);
        m_VisualRPM.Initialize(RPM_AtShift, RPM_AtShift - 500.0f, static_cast<int>(TotalDuration), EQ_PWR_SQ);

        *static_cast<int *>(static_cast<void *>(&m_bNeed_DisengageSnd)) = 1;
        *static_cast<int *>(static_cast<void *>(&m_bPendingNeedShiftSound)) = 1;
        tShiftDelay = SndBase::m_fRunningTime;
        tShiftDelay += m_pShiftingPatternData->Up_Shift_Sound_Delay();
        ShiftType = static_cast<AEMS_SHIFTING_SAMPLES>(1);
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

    if (GetCurGear() != static_cast<Gear>(1)) {
        CleanUpShiftFX();
        eShiftState = SHFT_DOWN_DISENGAGE;
        eShiftStageChanged = SHFT_DOWN_DISENGAGE;

        RPM_AtShift = m_pEngineCtl->m_fPrevRPM;
        m_pEngineCtl->m_fPrevRPM = m_pEngineCtl->m_fEng_RPM;
        m_pEngineCtl->SetEngRPM(RPM_AtShift);
        m_pEngineCtl->m_fSmoothedEng_RPM = m_pEngineCtl->m_fSmoothedEng_RPM * 0.95f + RPM_AtShift * 0.05f;
        t_Last_Shift = SndBase::m_fRunningTime;

        m_InterpShiftTorque.Initialize(m_pEngineCtl->m_pPhysicsCtl->GetPhysTRQ(), 0.0f, 0x32, LINEAR);

        TargetRPM = RPM_AtShift - static_cast<float>(m_pShiftingPatternData->Down_Disengage_Fall_RPM());
        TargetRPM = bClamp(TargetRPM, 1000.0f, 10000.0f);
        m_InterpShiftRPM.Initialize(RPM_AtShift, TargetRPM,
                                    static_cast<int>(m_pShiftingPatternData->Down_Disengage_Fall_T()), LINEAR);
        m_InterpShiftVol.Initialize(0.0f, 0.0f, 1, LINEAR);

        tShiftDelay = SndBase::m_fRunningTime + m_pShiftingPatternData->Down_Shift_Sound_Delay();
        if (3000.0f < m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM()) {
            *static_cast<int *>(static_cast<void *>(&m_bPendingNeedShiftSound)) = 1;
        }

        ShiftType = static_cast<AEMS_SHIFTING_SAMPLES>(0);
    }
}

void SFXCTL_Shifting::PostShiftFX_Update(float t) {
    if (eShift_LFO != SHIFT_LFO_NONE) {
        m_Shift_RPM_AMP_DECAY.Update(t);
        m_Shift_VOL_AMP_DECAY.Update(t);
        if (m_Shift_RPM_AMP_DECAY.IsFinished()) {
        }
        if (m_Shift_RPM_AMP_DECAY.IsFinished()) {
            CleanUpShiftFX();
            return;
        }

        m_RPM_LFO_AMP = static_cast<int>(m_Shift_RPM_AMP_DECAY.GetValue());
        m_RPM_LFO_FRQ = static_cast<int>(m_pShiftingPatternData->LFO_RPM_Freq());
        m_VOL_LFO_AMP = static_cast<int>(m_Shift_VOL_AMP_DECAY.GetValue());
        m_VOL_LFO_FRQ = static_cast<int>(m_pShiftingPatternData->LFO_Vol_Freq());
    }
}

void SFXCTL_Shifting::PostShiftFX_End() {
    eShift_LFO = SHIFT_LFO_NONE;
    m_VOL_LFO_AMP = 0;
    m_VOL_LFO_FRQ = 0;
    m_TRQ_LFO_AMP = 0;
    m_TRQ_LFO_FRQ = 0;
    m_RPM_LFO_AMP = 0;
    m_RPM_LFO_FRQ = 0;
}

void SFXCTL_Shifting::PostShiftFX_Init() {
    float ScaleDown;

    if (GetCurGear() < Sound::SIXTH_GEAR) {
        ScaleDown = 1.0f;
        if (GetCurGear() == Sound::SECOND_GEAR) {
            ScaleDown = 0.85f;
        }
        if (GetCurGear() == Sound::THIRD_GEAR) {
            ScaleDown = 0.7f;
        }
        if (GetCurGear() == Sound::FOURTH_GEAR) {
            ScaleDown = 0.55f;
        }
        if (GetCurGear() == Sound::FIFTH_GEAR) {
            ScaleDown = 0.3f;
        }

        eShift_LFO = SHIFT_LFO_ON;
        m_Shift_RPM_AMP_DECAY.Initialize(static_cast<float>(m_pShiftingPatternData->LFO_RPM_Amp()) * ScaleDown, 0.0f,
                                         static_cast<int>(m_pShiftingPatternData->LFO_RPM_Decay_Time()), LINEAR);
        m_Shift_VOL_AMP_DECAY.Initialize(static_cast<float>(m_pShiftingPatternData->LFO_Vol_Amp()), 0.0f,
                                         static_cast<int>(m_pShiftingPatternData->LFO_Vol_Decay_Time()), LINEAR);
    }
}

void SFXCTL_Shifting::CleanUpShiftFX() {
    PostShiftFX_End();
    eShiftState = static_cast<SHIFT_STAGE>(0);
    eShiftStageChanged = static_cast<SHIFT_STAGE>(0);
}

Gear SFXCTL_Shifting::GetCurGear() { return m_pEngineCtl->m_pPhysicsCtl->m_CurGear; }

Gear SFXCTL_Shifting::GetLastGear() { return m_pEngineCtl->m_pPhysicsCtl->m_LastGear; }
