#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"
#include "Speed/Indep/Libs/Support/Utility/UBezierLite.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

void FillGraphFromSpline(const UMath::Matrix4 &matrix, bVector2 *points, int num_points, float XScale, float YScale);

SFXCTL_Shifting::SFXCTL_Shifting()
    : m_pEngineCtl(nullptr) //
    , m_bNeed_ShiftGearSnd(false) //
    , m_bNeed_DisengageSnd(false) //
    , m_bNeed_EngageSnd(false) //
    , m_bNeed_AccelSnd(false) //
    , m_bNeed_DeccelSnd(false) //
    , m_bShouldBeWhining(false) //
    , m_bBrakePedalMashed(false) //
    , m_pShiftingPatternData(nullptr) //
    , eShiftState(SHFT_NONE) //
    , eShiftStageChanged(SHFT_NONE) //
    , m_VOL_LFO_AMP(0) //
    , m_VOL_LFO_FRQ(0) //
    , m_TRQ_LFO_AMP(0) //
    , m_TRQ_LFO_FRQ(0) //
    , m_RPM_LFO_AMP(0) //
    , m_RPM_LFO_FRQ(0) //
    , m_bPendingNeedShiftSound(false) //
    , ShiftType(static_cast< AEMS_SHIFTING_SAMPLES >(0)) //
    , tShiftDelay(0.0f) //
    , t_Last_Shift(0.0f) //
    , RPM_AtShift(0.0f) //
    , m_RPMGraph(m_RPMPoints, 7) //
    , t_CurStage(0.0f) //
    , m_CurStage(0) //
    , RPMOffset(0) //
    , m_timeBrakeLastMashed(0) //
    , m_nPostShiftFXLevel(0) //
    , eShift_LFO(SHIFT_LFO_NONE) {}

SndBase *SFXCTL_Shifting::CreateObject(unsigned int allocator) {
    if (allocator != 0) {
        return new (static_cast<SFXCTL_Shifting *>(
            gAudioMemoryManager.AllocateMemory(sizeof(SFXCTL_Shifting), SFXCTL_Shifting::s_TypeInfo.typeName, true)))
            SFXCTL_Shifting();
    } else {
        return new (static_cast<SFXCTL_Shifting *>(
            gAudioMemoryManager.AllocateMemory(sizeof(SFXCTL_Shifting), SFXCTL_Shifting::s_TypeInfo.typeName, false)))
            SFXCTL_Shifting();
    }
}

SFXCTL_Shifting::~SFXCTL_Shifting() {}

SndBase::TypeInfo *SFXCTL_Shifting::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_Shifting::GetTypeName() const { return s_TypeInfo.typeName; }

void SFXCTL_Shifting::InitSFX() {
    SFXCTL::InitSFX();
    ShiftType = static_cast< AEMS_SHIFTING_SAMPLES >(1);
    m_VOL_LFO_AMP = 0;
    m_VOL_LFO_FRQ = 0;
    m_TRQ_LFO_AMP = 0;
    m_TRQ_LFO_FRQ = 0;
    m_RPM_LFO_AMP = 0;
    m_RPM_LFO_FRQ = 0;
    eShiftState = SHFT_NONE;
    eShiftStageChanged = SHFT_NONE;
    eShift_LFO = SHIFT_LFO_NONE;
    *static_cast<int *>(static_cast<void *>(&m_bPendingNeedShiftSound)) = 0;
    *static_cast<int *>(static_cast<void *>(&m_bShouldBeWhining)) = 0;
    *static_cast<int *>(static_cast<void *>(&m_bBrakePedalMashed)) = 0;
    tShiftDelay = 0.0f;
    m_nPostShiftFXLevel = 0;
    *static_cast<int *>(static_cast<void *>(&m_bNeed_DeccelSnd)) = 0;
    t_Last_Shift = 0.0f;
    *static_cast<int *>(static_cast<void *>(&m_bNeed_AccelSnd)) = 0;
    m_timeBrakeLastMashed = Timer(0);
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

int SFXCTL_Shifting::GetController(int Index) {
    if (Index != 0) {
        return -1;
    }
    return 4;
}

float SFXCTL_Shifting::GetShiftingRPM() { return m_InterpShiftRPM.CurValue; }

float SFXCTL_Shifting::GetShiftingTRQ() { return m_InterpShiftTorque.CurValue; }

float SFXCTL_Shifting::GetShiftingVOL() { return m_InterpShiftVol.CurValue; }

Gear SFXCTL_Shifting::GetCurGear() { return m_pEngineCtl->m_pPhysicsCtl->m_CurGear; }

Gear SFXCTL_Shifting::GetLastGear() { return m_pEngineCtl->m_pPhysicsCtl->m_LastGear; }

void SFXCTL_Shifting::PostShiftFX_End() {
    eShift_LFO = SHIFT_LFO_NONE;
    m_VOL_LFO_AMP = 0;
    m_VOL_LFO_FRQ = 0;
    m_TRQ_LFO_AMP = 0;
    m_TRQ_LFO_FRQ = 0;
    m_RPM_LFO_AMP = 0;
    m_RPM_LFO_FRQ = 0;
}

void SFXCTL_Shifting::CleanUpShiftFX() {
    PostShiftFX_End();
    eShiftState = static_cast<SHIFT_STAGE>(0);
    eShiftStageChanged = static_cast<SHIFT_STAGE>(0);
}

void SFXCTL_Shifting::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    m_UGL = m_pEAXCar->m_TransmissionUGL;
    m_pShiftingPatternData = &m_pEAXCar->GetShiftInfo();
    m_nPostShiftFXLevel = m_UGL % 2;
}

void SFXCTL_Shifting::AttachController(SFXCTL *ctrl) {
    if (((ctrl->GetTypeInfo()->ObjectID >> 4) & 0xFFF) == 4) {
        m_pEngineCtl = static_cast<SFXCTL_Engine *>(ctrl);
    }
}

void SFXCTL_Shifting::UpdateMixerOutputs() {
    SetDMIX_Input(0, (eShiftState == SHFT_UP_DISENGAGE) ? 0x7FFF : 0);
    SetDMIX_Input(1, (eShiftStageChanged == SHFT_UP_ENGAGING) ? 0x7FFF : 0);
    SetDMIX_Input(2, IsDownShifting() ? 0x7FFF : 0);
}

void SFXCTL_Shifting::UpdateGearShiftState(float t) {
    static int _brakeInit = 0;
    static float _prevBrakeState = 0.0f;
    static const float kDownShiftingRevPercent = 0.5f;
    static const int kDownShiftingRevRampTime = 0x32;
    static const int kUpShiftTrqAttackTime[4] = {0x64, 0x64, 0x64, 0x64};
    static const float kUpShiftTrqAttachInitialPercent[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    static const int kSportShift = -1;

    if (SndBase::m_fRunningTime > tShiftDelay &&
        *static_cast<int *>(static_cast<void *>(&m_bPendingNeedShiftSound)) != 0) {
        *static_cast<int *>(static_cast<void *>(&m_bNeed_ShiftGearSnd)) = 1;
        *static_cast<int *>(static_cast<void *>(&m_bPendingNeedShiftSound)) = 0;
    }

    int isWhining = static_cast<int>(GetCurGear());
    *static_cast<int *>(static_cast<void *>(&m_bShouldBeWhining)) = (isWhining == 0);

    EAX_CarState *carstate = m_pStateBase->GetPhysCar();
    if (carstate->IsLocalPlayerCar()) {
        float t_last_mashed = (WorldTimer - m_timeBrakeLastMashed).GetSeconds();

        if (_brakeInit == 0) {
            _prevBrakeState = carstate->GetBrake();
            _brakeInit = 1;
        }

        if (carstate->GetBrake() >= 1.0f) {
            if (*static_cast<int *>(static_cast<void *>(&m_bBrakePedalMashed)) == 0) {
                if (t_last_mashed > 1.0f && _prevBrakeState == 0.0f) {
                    if (carstate->GetVelocityMagnitudeMPH() > 5.0f) {
                        *static_cast<int *>(static_cast<void *>(&m_bBrakePedalMashed)) = 1;
                    }
                }
            }
        }

        if (*static_cast<int *>(static_cast<void *>(&m_bBrakePedalMashed)) != 0) {
            m_timeBrakeLastMashed = WorldTimer;
            if (carstate->GetBrake() == 0.0f) {
                *static_cast<int *>(static_cast<void *>(&m_bBrakePedalMashed)) = 0;
            }
        }

        _prevBrakeState = carstate->GetBrake();
    }

    eShiftStageChanged = SHFT_NONE;
    if (eShiftState == SHFT_NONE) {
        return;
    }

    UpdateRPM(t);
    UpdateTorque(t);
    m_InterpShiftVol.Update(t);
    PostShiftFX_Update(t);
    t_CurStage += t * 1000.0f;

    switch (eShiftState) {
    case SHFT_UP_DISENGAGE: {
        float curTime = t_CurStage;
        if (m_RPMPoints[6].x < curTime) {
            m_CurStage++;
            unsigned int numDisengage = m_pShiftingPatternData->Num_Up_DisengageFall();
            if (m_CurStage < numDisengage) {
                const UMath::Matrix4 &curvespline = m_pShiftingPatternData->Up_DisengageFall_Curve(m_CurStage);
                const stShiftPair &pair = m_pShiftingPatternData->Up_DisengageFall(m_CurStage);
                const stShiftPair &pair2 = m_pShiftingPatternData->Up_DisengageFall(m_CurStage);

                FillGraphFromSpline(curvespline, m_RPMPoints, 7, static_cast<float>(pair.Time), static_cast<float>(pair2.RPM));
                t_CurStage = t * 1000.0f;
                RPMOffset = static_cast<unsigned short>(static_cast<int>(m_InterpShiftRPM.GetValue()));
                curTime = t_CurStage;
            } else {
                eShiftStageChanged = SHFT_UP_ENGAGING;
                eShiftState = SHFT_UP_ENGAGING;

                const UMath::Matrix4 &curvespline = m_pShiftingPatternData->Up_Engage_Curve();
                const stShiftPair &pair = m_pShiftingPatternData->Up_Engage();
                const stShiftPair &pair2 = m_pShiftingPatternData->Up_Engage();

                FillGraphFromSpline(curvespline, m_RPMPoints, 7, static_cast<float>(pair.Time), static_cast<float>(pair2.RPM));
                t_CurStage = 0.0f;
                RPMOffset = static_cast<unsigned short>(static_cast<int>(m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM()));

                float currpm = bClamp(static_cast<float>(RPMOffset) + m_RPMGraph.GetValue(t_CurStage), 1000.0f, 10000.0f);
                m_InterpShiftRPM.Initialize(currpm, currpm, 0, LINEAR);

                int length = bClamp(static_cast<int>(m_pEAXCar->GetCurGear()) + kSportShift, 0, 3);
                float physTRQ = m_pEngineCtl->m_pPhysicsCtl->GetPhysTRQ();
                m_InterpShiftTorque.Initialize(kUpShiftTrqAttachInitialPercent[length] * physTRQ, physTRQ,
                                               kUpShiftTrqAttackTime[length], LINEAR);

                m_InterpShiftVol.Initialize(m_pShiftingPatternData->Up_Engaging_Attack_Vol(), 0.0f,
                                            static_cast<int>(m_pShiftingPatternData->Up_Engaging_Attack_T()), LINEAR);

                PostShiftFX_Init();
                PostShiftFX_Update(t);
                *static_cast<int *>(static_cast<void *>(&m_bNeed_EngageSnd)) = 1;
                return;
            }
        }

        float currpm = bClamp(static_cast<float>(RPMOffset) + m_RPMGraph.GetValue(curTime), 1000.0f, 10000.0f);
        m_InterpShiftRPM.Initialize(currpm, currpm, 0, LINEAR);
        break;
    }
    case SHFT_UP_ENGAGING: {
        float curTime = t_CurStage;
        if (m_RPMPoints[6].x < curTime) {
            eShiftStageChanged = SHFT_UP_LFO;
            eShiftState = SHFT_UP_LFO;
            return;
        }
        RPMOffset = static_cast<unsigned short>(static_cast<int>(m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM()));
        float currpm = bClamp(static_cast<float>(RPMOffset) + m_RPMGraph.GetValue(curTime), 1000.0f, 10000.0f);
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
        if (!m_InterpShiftRPM.IsFinished()) {
            return;
        }

        eShiftStageChanged = SHFT_DOWN_ENGAGING_RISE;
        eShiftState = SHFT_DOWN_ENGAGING_RISE;

        int riseTime = static_cast<int>(m_pShiftingPatternData->Down_Engaging_Rise_T());
        if (m_pEngineCtl->m_pPhysicsCtl->m_CurGear < static_cast<Gear>(2)) {
            riseTime = static_cast<int>(static_cast<float>(riseTime) * 0.7f);
        }

        float lowRpmScale = m_pEngineCtl->GetEngRPM() < 1500.0f ? 0.0f : 1.0f;
        float targetRpm = bClamp(m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM() +
                                       static_cast<float>(m_pShiftingPatternData->Down_Engaging_Rise_RPM()) *
                                           lowRpmScale,
                                 1000.0f, 10000.0f);
        m_InterpShiftRPM.Initialize(m_pEngineCtl->GetEngRPM(), targetRpm, riseTime, EQ_PWR_SQ);
        m_InterpShiftTorque.Initialize(0.0f, m_pEngineCtl->GetEngTorque(), kDownShiftingRevRampTime, LINEAR);
        break;
    }
    case SHFT_DOWN_ENGAGING_RISE: {
        if (!m_InterpShiftRPM.IsFinished()) {
            return;
        }

        eShiftStageChanged = SHFT_DOWN_ENGAGING_FALL;
        eShiftState = SHFT_DOWN_ENGAGING_FALL;

        int fallTime = static_cast<int>(m_pShiftingPatternData->Down_Engaging_Fall_T());
        if (GetCurGear() < static_cast<Gear>(2) && m_UGL < AEMS_LEVEL2) {
            fallTime = static_cast<int>(static_cast<float>(fallTime) * 0.7f);
        }

        float lowRpmScale = m_pEngineCtl->GetEngRPM() < 1500.0f ? 0.0f : 1.0f;
        m_InterpShiftRPM.Initialize(m_pEngineCtl->GetEngRPM(),
                                    m_pEngineCtl->GetEngRPM() -
                                        static_cast<float>(m_pShiftingPatternData->Down_Engaging_Fall_RPM()) * lowRpmScale,
                                    fallTime,
                                    LINEAR);
        m_InterpShiftTorque.Initialize(kDownShiftingRevPercent * lowRpmScale, 0.0f, kDownShiftingRevRampTime, LINEAR);
        break;
    }
    case SHFT_DOWN_ENGAGING_FALL: {
        if (!m_InterpShiftRPM.IsFinished()) {
            return;
        }

        eShiftStageChanged = SHFT_DOWN_ENGAGING_REATTACH;
        eShiftState = SHFT_DOWN_ENGAGING_REATTACH;

        float attachTime =
            bClamp(bAbs((m_pEngineCtl->GetEngRPM() - m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM()) *
                        m_pShiftingPatternData->Down_Reattach_Scale()),
                   0.0f, 800.0f);
        m_InterpShiftRPM.Initialize(m_pEngineCtl->GetEngRPM(), m_pEngineCtl->m_pPhysicsCtl->GetPhysRPM(),
                                    static_cast<int>(attachTime), EQ_PWR_SQ);
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

void SFXCTL_Shifting::BeginUpShift() {
    CleanUpShiftFX();

    SFXCTL_Engine *engine = m_pEngineCtl;
    if (3000.0f <= engine->m_pPhysicsCtl->GetPhysRPM()) {
        eShiftStageChanged = SHFT_UP_DISENGAGE;
        eShiftState = SHFT_UP_DISENGAGE;

        float runningTime = SndBase::m_fRunningTime;
        float prevRpm = engine->m_fPrevRPM;
        RPM_AtShift = prevRpm;
        float curRpm = engine->m_fEng_RPM;
        engine->m_fPrevRPM = curRpm;
        engine->SetEngRPM(prevRpm);
        engine->m_fSmoothedEng_RPM = engine->m_fSmoothedEng_RPM * 0.95f + prevRpm * 0.05f;
        t_Last_Shift = runningTime;

        Attrib::Gen::shiftpattern *shiftPattern = m_pShiftingPatternData;
        const UMath::Matrix4 &curvespline = shiftPattern->Up_DisengageFall_Curve(0);
        const stShiftPair &pair = shiftPattern->Up_DisengageFall(0);
        const stShiftPair &pair2 = shiftPattern->Up_DisengageFall(0);

        FillGraphFromSpline(curvespline, m_RPMPoints, 7, static_cast<float>(pair.Time), static_cast<float>(pair2.RPM));

        float shiftRpm = RPM_AtShift;
        m_CurStage = 0;
        t_CurStage = 0.0f;
        RPMOffset = static_cast<unsigned short>(static_cast<int>(shiftRpm));

        m_InterpShiftRPM.Initialize(shiftRpm, shiftRpm, 0, LINEAR);
        m_InterpShiftVol.Initialize(0.0f, 0.0f, 1, LINEAR);
        float physTrq = engine->m_pPhysicsCtl->GetPhysTRQ();
        m_InterpShiftTorque.Initialize(physTrq, 0.0f, 100, LINEAR);

        float totalDuration;
        unsigned int numDisengage = shiftPattern->Num_Up_DisengageFall();
        if (numDisengage == 1) {
            const stShiftPair &first = shiftPattern->Up_DisengageFall(0);
            totalDuration = static_cast<float>(first.Time);
        } else {
            const stShiftPair &first = shiftPattern->Up_DisengageFall(0);
            const stShiftPair &second = shiftPattern->Up_DisengageFall(1);
            totalDuration = static_cast<float>(first.Time) + static_cast<float>(second.Time);
        }

        const stShiftPair &upEngage = shiftPattern->Up_Engage();
        int visualLength = static_cast<int>(totalDuration + static_cast<float>(upEngage.Time));
        m_VisualRPM.Initialize(RPM_AtShift, RPM_AtShift - 500.0f, visualLength, EQ_PWR_SQ);

        *static_cast<int *>(static_cast<void *>(&m_bNeed_DisengageSnd)) = 1;
        *static_cast<int *>(static_cast<void *>(&m_bPendingNeedShiftSound)) = 1;
        tShiftDelay = SndBase::m_fRunningTime + m_pShiftingPatternData->Up_Shift_Sound_Delay();
        ShiftType = static_cast< AEMS_SHIFTING_SAMPLES >(1);
    }
}

void SFXCTL_Shifting::BeginDownShift() {
    float TargetRPM;

    if (GetCurGear() != static_cast< Gear >(1)) {
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

        ShiftType = static_cast< AEMS_SHIFTING_SAMPLES >(0);
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
