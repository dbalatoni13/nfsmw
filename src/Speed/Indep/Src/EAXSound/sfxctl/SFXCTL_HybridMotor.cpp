#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_HybridMotor.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_AccelTrans.hpp"
#include "Speed/Indep/Src/EAXSound/OldSoundTemplates.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engineaudio.h"

SFXCTL_HybridMotor::SFXCTL_HybridMotor()
    : m_pEngineCtl(nullptr) //
    , m_pShiftingCtl(nullptr) //
    , m_pAccelTranCtl(nullptr) //
    , tSteadyDuration(0.0f) //
    , m_EngVolAEMS(0) //
    , m_EngVolAccelGinsu(0) //
    , m_EngVolDecelGinsu(0) //
    , DecelCrossfadeMix(CrossFadesPoints, 5) //
    , m_EngVolRedLine(0) //
    , mPrevDeltaRPM(0) //
    , m_bAEMSLPF(false) //
    , SteadyFrameCnt(0) //
    , m_AvgDeltaRPM(4) {
    m_EngineMix.Aems = 0.0f;
    m_EngineMix.AccelGinsu = 0.0f;
    m_EngineMix.DecelGinsu = 0.0f;
    m_EngineMix.Cutoff = 0;
    m_AvgDeltaRPM.Flush(0.0f);
    m_GinsuLPFVal = 24000.0f;
    bVector2 point0;
    bVector2 point1;
    bVector2 point2;
    bVector2 point3;
    bVector2 point4;

    point0 = bVector2(0.0f, 0.0f);
    point1 = bVector2(0.0f, 0.0f);
    point2 = bVector2(0.0f, 0.0f);
    point3 = bVector2(0.0f, 0.0f);
    point4 = bVector2(0.0f, 0.0f);

    CrossFadesPoints[0] = point0;
    CrossFadesPoints[1] = point1;
    CrossFadesPoints[2] = point2;
    CrossFadesPoints[3] = point3;
    CrossFadesPoints[4] = point4;
    m_GinsuScaledRPM = 0.0f;
    mPrevPhyDeltaRPM = 0.0f;
    m_CurPhyDeltaRPMVal = 0.0f;
    m_CurAudDeltaRPMVal = 0.0f;
    PrevRPM = 0.0f;
    PercentOfDecelThreshold = 0.0f;
    PercentOfAccelThreshold = 0.0f;
}

SndBase::TypeInfo *SFXCTL_HybridMotor::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFXCTL_HybridMotor::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXCTL_HybridMotor::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_HybridMotor::GetStaticTypeInfo()->typeName, false) SFXCTL_HybridMotor();
    }
    return new (SFXCTL_HybridMotor::GetStaticTypeInfo()->typeName, true) SFXCTL_HybridMotor();
}

SFXCTL_HybridMotor::~SFXCTL_HybridMotor() {}

void SFXCTL_HybridMotor::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    m_UGL = static_cast<eAemsUpgradeLevel>(m_pEAXCar->GetEngineUpgradeLevel());

    int MaxMinDif = static_cast<int>(m_pEAXCar->GetAttributes().GINSU_Decel_MaxRPM()) -
                    static_cast<int>(m_pEAXCar->GetAttributes().GINSU_Decel_MinRPM());

    CrossFadesPoints[1].y = 0.0f;
    CrossFadesPoints[1].x = static_cast<float>(m_pEAXCar->GetAttributes().GINSU_Decel_MinRPM());
    CrossFadesPoints[2].y = 1.0f;
    CrossFadesPoints[2].x = static_cast<float>(m_pEAXCar->GetAttributes().GINSU_Decel_MinRPM()) +
                            static_cast<float>(MaxMinDif) * m_pEAXCar->GetAttributes().GINSU_DECEL_FADE_OUT();
    CrossFadesPoints[3].y = 1.0f;
    CrossFadesPoints[3].x = static_cast<float>(m_pEAXCar->GetAttributes().GINSU_Decel_MaxRPM()) -
                            static_cast<float>(MaxMinDif) * m_pEAXCar->GetAttributes().GINSU_DECEL_FADE_IN();
    CrossFadesPoints[4].y = 0.0f;
    CrossFadesPoints[4].x = static_cast<float>(m_pEAXCar->GetAttributes().GINSU_Decel_MaxRPM());
}

void SFXCTL_HybridMotor::InitSFX() {
    SFXCTL::InitSFX();
    m_EngVolAEMS = 0;
    m_EngVolAccelGinsu = 0;
    m_EngVolDecelGinsu = 0;
}

int SFXCTL_HybridMotor::GetController(int Index) {
    switch (Index) {
    case 0:
        return 4;
    case 1:
        return 2;
    case 2:
        return 3;
    default:
        return -1;
    }
}

void SFXCTL_HybridMotor::AttachController(SFXCTL *psfxctl) {
    switch (psfxctl->GetObjectIndex()) {
    case 4:
        m_pEngineCtl = static_cast<SFXCTL_Engine *>(psfxctl);
        break;
    case 2:
        m_pShiftingCtl = static_cast<SFXCTL_Shifting *>(psfxctl);
        break;
    case 3:
        m_pAccelTranCtl = static_cast<SFXCTL_AccelTrans *>(psfxctl);
        break;
    }
}

void SFXCTL_HybridMotor::UpdateDeltaRPM() {
    mPrevPhyDeltaRPM = m_CurPhyDeltaRPMVal;

    float PhyDeltaRPM = GetPhysRPM() - PrevRPM;
    m_CurPhyDeltaRPMVal = PhyDeltaRPM;

    float AudDeltaRPM = m_pEngineCtl->m_fEng_RPM - m_pEngineCtl->m_fPrevRPM;
    m_CurAudDeltaRPMVal = AudDeltaRPM;
    PrevRPM = GetPhysRPM();

    if (m_pEAXCar->GetPhysicsCTL()->NISRevingEnabled) {
        m_AvgDeltaRPM.Record(AudDeltaRPM);
    } else if (m_pAccelTranCtl->IsActive()) {
        if (m_pAccelTranCtl->eAccelTransFxState == FX_ACCEL_STATE_ATTACK) {
            m_AvgDeltaRPM.Flush(PhyDeltaRPM);
        } else {
            m_AvgDeltaRPM.Flush(AudDeltaRPM);
        }
    } else if (m_pShiftingCtl->IsActive()) {
        SHIFT_STAGE ShiftState = m_pShiftingCtl->eShiftState;

        if (ShiftState == SHFT_DOWN_ENGAGING_RISE) {
            m_AvgDeltaRPM.Flush(AudDeltaRPM);
        } else if (m_pShiftingCtl->eShiftStageChanged == SHFT_UP_ENGAGING) {
            m_AvgDeltaRPM.Flush(bAbs(PhyDeltaRPM));
        } else {
            if (static_cast<unsigned int>(ShiftState - SHFT_UP_ENGAGING) < 2u) {
                m_AvgDeltaRPM.Record(bAbs(AudDeltaRPM));
            } else {
                m_AvgDeltaRPM.Record(AudDeltaRPM);
            }
        }
    } else {
        m_AvgDeltaRPM.Record(PhyDeltaRPM);
    }

    m_AvgDeltaRPM.Recalculate();
}

void SFXCTL_HybridMotor::UpdateParams(float t) {
    if (!m_pEAXCar->GetAttributes().IsValid()) {
        return;
    }
    SFXCTL::UpdateParams(t);
    UpdateDeltaRPM();
    float AemsRPM = m_pEngineCtl->GetEngRPM();
    AemsRPM = bClamp(AemsRPM, 1000.0f, 10000.0f);
    m_GinsuScaledRPM = (AemsRPM - 1000.0f) * (m_pEAXCar->GetAttributes().MaxRPM() - m_pEAXCar->GetAttributes().MinRPM()) * (1.0f / 9000.0f) + m_pEAXCar->GetAttributes().MinRPM();
    if (m_pEAXCar->m_EngineType == eGINSU_ENG_SINGLE) {
        UpdateSingleMixEng(t);
    } else {
        UpdateDualMixEng(t);
    }
    UpdateVolumeRedlining();
}

void SFXCTL_HybridMotor::UpdateSingleMixEng(float t) {
    (void)t;
    Slope TrqThreshold(0.0f, 1.0f, 0.0f, 35.0f);
    float AccelDecelMix = TrqThreshold.GetValue(m_pEngineCtl->GetEngTorque());
    bool USE_SMOOTHING = true;
    EngineMix AccelMix;
    EngineMix DecelMix;
    float DeltaRPM = bAbs(m_AvgDeltaRPM.GetValue() + 10.0f);
    float adt = m_pEAXCar->GetAttributes().AccelDeltaRPMThreshold();
    EngineMix newmix;
    float EngineCtlVolFactor;
    int VolAEMS;
    int VolAccelGinsu;
    int VolDecelGinsu;

    PercentOfAccelThreshold = bClamp(1.0f - (adt - DeltaRPM) / adt, 0.0f, 1.0f);
    adt = m_pEAXCar->GetAttributes().DecelDeltaRPMThreshold();
    PercentOfDecelThreshold = bClamp(1.0f - (adt - DeltaRPM) / adt, 0.0f, 1.0f);

    if (m_pStateBase->GetPhysCar()->IsShifting() || m_pShiftingCtl->IsActive()) {
        SHIFT_STAGE shiftState = m_pShiftingCtl->eShiftState;
        if (shiftState != SHFT_UP_LFO) {
            if (shiftState == SHFT_UP_ENGAGING) {
                USE_SMOOTHING = false;
            } else if (shiftState != SHFT_DOWN_ENGAGING_RISE) {
                if (shiftState == SHFT_DOWN_ENGAGING_REATTACH && m_pEAXCar->IsAccelerating()) {
                    USE_SMOOTHING = false;
                }
            }
        }
    } else if (m_pAccelTranCtl->IsActive()) {
        if (m_pAccelTranCtl->eAccelTransFxState == FX_ACCEL_STATE_ATTACK) {
            USE_SMOOTHING = false;
        }
    }
    AccelMix.Aems = PercentOfAccelThreshold *
                        (m_pEAXCar->GetAttributes().AEMSMix_L_RPM() - m_pEAXCar->GetAttributes().AEMSMix_S_RPM()) +
                    m_pEAXCar->GetAttributes().AEMSMix_S_RPM();
    AccelMix.AccelGinsu =
        PercentOfAccelThreshold *
            (m_pEAXCar->GetAttributes().GINSUMix_L_RPM() - m_pEAXCar->GetAttributes().GINSUMix_S_RPM()) +
        m_pEAXCar->GetAttributes().GINSUMix_S_RPM();
    AccelMix.DecelGinsu = 0.0f;
    AccelMix.Cutoff = m_pEngineCtl->m_DistanceFltr;
    DecelMix.Aems = PercentOfDecelThreshold * (m_pEAXCar->GetAttributes().DECEL_AEMSMix_L_RPM() -
                                               m_pEAXCar->GetAttributes().DECEL_AEMSMix_S_RPM()) +
                    m_pEAXCar->GetAttributes().DECEL_AEMSMix_S_RPM();
    DecelMix.AccelGinsu = m_pEAXCar->GetAttributes().Ginsu_ACL_Neg_S_RPM();
    DecelMix.DecelGinsu = 0.0f;
    DecelMix.Cutoff =
        bMin(static_cast<int>(m_pEAXCar->GetAttributes().GINSU_LowPassCutoff()), m_pEngineCtl->m_DistanceFltr);
    newmix.Aems = (AccelMix.Aems - DecelMix.Aems) * AccelDecelMix + DecelMix.Aems;
    newmix.AccelGinsu = (AccelMix.AccelGinsu - DecelMix.AccelGinsu) * AccelDecelMix + DecelMix.AccelGinsu;
    newmix.DecelGinsu = (AccelMix.DecelGinsu - DecelMix.DecelGinsu) * AccelDecelMix + DecelMix.DecelGinsu;
    newmix.Cutoff = static_cast<int>((static_cast<float>(AccelMix.Cutoff - DecelMix.Cutoff)) * AccelDecelMix +
                                     static_cast<float>(DecelMix.Cutoff));

    if (USE_SMOOTHING) {
        m_EngineMix.Aems = smooth(m_EngineMix.Aems, newmix.Aems, 0.2f);
        m_EngineMix.AccelGinsu = smooth(m_EngineMix.AccelGinsu, newmix.AccelGinsu, 0.2f);
        m_EngineMix.DecelGinsu = smooth(m_EngineMix.DecelGinsu, newmix.DecelGinsu, 0.2f);
        m_EngineMix.Cutoff = smooth(m_EngineMix.Cutoff, newmix.Cutoff, 6000);
    } else {
        m_EngineMix = newmix;
    }

    int curveOutput = NFSMixShape::GetCurveOutput(
        static_cast<NFSMixShape::eMIXTABLEID>(7), static_cast<int>(static_cast<float>(m_EngineMix.Cutoff) * 1.3106799f), true);
    m_GinsuLPFVal = static_cast<float>(25000 - curveOutput);

    EngineCtlVolFactor = static_cast<float>(m_pEngineCtl->m_iEngineVol) * 3.051851e-05f;
    VolAEMS = static_cast<int>(
        m_EngineMix.Aems * static_cast<float>(m_pEAXCar->GetAttributes().AEMSVol()) * EngineCtlVolFactor);
    VolAccelGinsu = static_cast<int>(
        m_EngineMix.AccelGinsu * static_cast<float>(m_pEAXCar->GetAttributes().GINSUAccelVol()) * EngineCtlVolFactor);
    VolDecelGinsu = static_cast<int>(
        m_EngineMix.DecelGinsu * static_cast<float>(m_pEAXCar->GetAttributes().GinsuDecelVol()) * EngineCtlVolFactor);

    if (USE_SMOOTHING) {
        m_EngVolAEMS = smooth(m_EngVolAEMS, VolAEMS, 7000);
        m_EngVolAccelGinsu = smooth(m_EngVolAccelGinsu, VolAccelGinsu, 7000);
        m_EngVolDecelGinsu = smooth(m_EngVolDecelGinsu, VolDecelGinsu, 7000);
    } else {
        m_EngVolAEMS = VolAEMS;
        m_EngVolAccelGinsu = VolAccelGinsu;
        m_EngVolDecelGinsu = VolDecelGinsu;
    }

    m_EngVolRedLine = static_cast<int>(static_cast<float>(m_pEAXCar->GetAttributes().AEMSVol()) * EngineCtlVolFactor);
}

void SFXCTL_HybridMotor::UpdateDualMixEng(float t) {
    (void)t;
    Slope TrqThreshold(0.0f, 1.0f, 0.0f, 35.0f);
    float AccelDecelMix = TrqThreshold.GetValue(m_pEngineCtl->GetEngTorque());
    bool USE_SMOOTHING = true;
    EngineMix AccelMix;
    EngineMix DecelMix;
    float DeltaRPM = bAbs(m_AvgDeltaRPM.GetValue() + 10.0f);
    float adt = m_pEAXCar->GetAttributes().AccelDeltaRPMThreshold();
    bool UseAccelMix;
    EngineMix newmix;
    float EngineCtlVolFactor;
    int VolAEMS;
    int VolAccelGinsu;
    int VolDecelGinsu;
    int tempAEMVol;

    PercentOfAccelThreshold = bClamp(1.0f - (adt - DeltaRPM) / adt, 0.0f, 1.0f);
    adt = m_pEAXCar->GetAttributes().DecelDeltaRPMThreshold();
    PercentOfDecelThreshold = bClamp(1.0f - (adt - DeltaRPM) / adt, 0.0f, 1.0f);

    if (!m_pEAXCar->GetPhysicsCTL()->NISRevingEnabled) {
        if (m_pStateBase->GetPhysCar()->IsShifting() || m_pShiftingCtl->IsActive()) {
            UseAccelMix = (m_pShiftingCtl->eShiftState == SHFT_UP_ENGAGING) ? true : false;
        } else {
            UseAccelMix = (m_pAccelTranCtl->eAccelTransFxState == FX_ACCEL_STATE_ATTACK) ? true : false;
        }
        if (UseAccelMix) {
            USE_SMOOTHING = false;
        }
    }

    AccelMix.Aems = PercentOfAccelThreshold *
                        (m_pEAXCar->GetAttributes().AEMSMix_L_RPM() - m_pEAXCar->GetAttributes().AEMSMix_S_RPM()) +
                    m_pEAXCar->GetAttributes().AEMSMix_S_RPM();
    AccelMix.AccelGinsu =
        PercentOfAccelThreshold *
            (m_pEAXCar->GetAttributes().GINSUMix_L_RPM() - m_pEAXCar->GetAttributes().GINSUMix_S_RPM()) +
        m_pEAXCar->GetAttributes().GINSUMix_S_RPM();
    AccelMix.DecelGinsu = 0.0f;
    AccelMix.Cutoff = 25000;
    DecelMix.Aems = PercentOfDecelThreshold * (m_pEAXCar->GetAttributes().DECEL_AEMSMix_L_RPM() -
                                               m_pEAXCar->GetAttributes().DECEL_AEMSMix_S_RPM()) +
                    m_pEAXCar->GetAttributes().DECEL_AEMSMix_S_RPM();
    DecelMix.DecelGinsu =
        PercentOfDecelThreshold *
            (m_pEAXCar->GetAttributes().DECEL_GINSUMix_L_RPM() - m_pEAXCar->GetAttributes().DECEL_GINSUMix_S_RPM()) +
        m_pEAXCar->GetAttributes().DECEL_GINSUMix_S_RPM();
    {
        float crossfade = DecelCrossfadeMix.GetValue(m_GinsuScaledRPM);
        float crossfadeAems = DecelCrossfadeMix.GetValue(m_GinsuScaledRPM);
        DecelMix.Aems = crossfadeAems * DecelMix.Aems + (1.0f - crossfade);
    }
    DecelMix.DecelGinsu = DecelCrossfadeMix.GetValue(m_GinsuScaledRPM) * DecelMix.DecelGinsu;
    DecelMix.AccelGinsu = m_pEAXCar->GetAttributes().Ginsu_ACL_Neg_S_RPM();
    DecelMix.Cutoff =
        bMin(static_cast<int>(m_pEAXCar->GetAttributes().GINSU_LowPassCutoff()), m_pEngineCtl->m_DistanceFltr);

    m_bAEMSLPF = false;
    newmix.Aems = (AccelMix.Aems - DecelMix.Aems) * AccelDecelMix + DecelMix.Aems;
    newmix.AccelGinsu = (AccelMix.AccelGinsu - DecelMix.AccelGinsu) * AccelDecelMix + DecelMix.AccelGinsu;
    newmix.DecelGinsu = (AccelMix.DecelGinsu - DecelMix.DecelGinsu) * AccelDecelMix + DecelMix.DecelGinsu;
    newmix.Cutoff = static_cast<int>((static_cast<float>(AccelMix.Cutoff - DecelMix.Cutoff)) * AccelDecelMix +
                                     static_cast<float>(DecelMix.Cutoff));

    if (USE_SMOOTHING) {
        m_EngineMix.Aems = smooth(m_EngineMix.Aems, newmix.Aems, 0.2f);
        m_EngineMix.AccelGinsu = smooth(m_EngineMix.AccelGinsu, newmix.AccelGinsu, 0.2f);
        m_EngineMix.DecelGinsu = smooth(m_EngineMix.DecelGinsu, newmix.DecelGinsu, 0.2f);
        m_EngineMix.Cutoff = smooth(m_EngineMix.Cutoff, newmix.Cutoff, 6000);
    } else {
        m_EngineMix = newmix;
    }

    int curveOutput = NFSMixShape::GetCurveOutput(static_cast<NFSMixShape::eMIXTABLEID>(7), m_EngineMix.Cutoff, true);
    m_GinsuLPFVal = static_cast<float>(curveOutput);

    unsigned int mixedCutoff = m_pEngineCtl->m_DistanceFltr;
    if (static_cast<unsigned int>(m_EngineMix.Cutoff) < mixedCutoff) {
        mixedCutoff = static_cast<unsigned int>(m_EngineMix.Cutoff);
    }
    m_GinsuLPFVal = static_cast<float>(mixedCutoff);

    EngineCtlVolFactor = static_cast<float>(m_pEngineCtl->m_iEngineVol) * 3.051851e-05f;
    tempAEMVol = static_cast<int>(
        (static_cast<float>(m_pEAXCar->GetAttributes().AEMSVol() - m_pEAXCar->GetAttributes().DECEL_AEMSVol())) *
            AccelDecelMix +
        static_cast<float>(m_pEAXCar->GetAttributes().DECEL_AEMSVol()));
    VolAEMS = static_cast<int>(m_EngineMix.Aems * static_cast<float>(tempAEMVol) * EngineCtlVolFactor);
    VolAccelGinsu = static_cast<int>(
        m_EngineMix.AccelGinsu * static_cast<float>(m_pEAXCar->GetAttributes().GINSUAccelVol()) * EngineCtlVolFactor);
    VolDecelGinsu = static_cast<int>(
        m_EngineMix.DecelGinsu * static_cast<float>(m_pEAXCar->GetAttributes().GinsuDecelVol()) * EngineCtlVolFactor);

    if (USE_SMOOTHING) {
        m_EngVolAEMS = smooth(m_EngVolAEMS, VolAEMS, 7000);
        m_EngVolAccelGinsu = smooth(m_EngVolAccelGinsu, VolAccelGinsu, 7000);
        m_EngVolDecelGinsu = smooth(m_EngVolDecelGinsu, VolDecelGinsu, 7000);
    } else {
        m_EngVolAEMS = VolAEMS;
        m_EngVolAccelGinsu = VolAccelGinsu;
        m_EngVolDecelGinsu = VolDecelGinsu;
    }

    m_EngVolRedLine = static_cast<int>(static_cast<float>(m_pEAXCar->GetAttributes().AEMSVol()) * EngineCtlVolFactor);
}

void SFXCTL_HybridMotor::UpdateVolumeRedlining() {
    m_EngVolRedLine =
        static_cast<int>(static_cast<float>(m_EngVolRedLine) * m_pEngineCtl->RedLineSampFactor.GetValue());
    m_EngVolAEMS =
        static_cast<int>(static_cast<float>(m_EngVolAEMS) * m_pEngineCtl->RedLineEngFactor.GetValue());
    m_EngVolAccelGinsu =
        static_cast<int>(static_cast<float>(m_EngVolAccelGinsu) * m_pEngineCtl->RedLineEngFactor.GetValue());
    m_EngVolDecelGinsu =
        static_cast<int>(static_cast<float>(m_EngVolDecelGinsu) * m_pEngineCtl->RedLineEngFactor.GetValue());
}

void SFXCTL_HybridMotor::UpdateMixerOutputs() {
    bool bOutputOn = false;
    float AvgDeltaRPM;
    float PercentOfThreshold;
    int output;
    float speedMph = m_pEAXCar->GetVelocityMagnitudeMPH();

    if (speedMph <= 30.0f || 30.0f <= bAbs(m_AvgDeltaRPM.GetValue())) {
        tSteadyDuration = SndBase::m_fRunningTime;
    } else {
        bOutputOn = tSteadyDuration + 3.0f < SndBase::m_fRunningTime;
    }

    if (!bOutputOn) {
        tSteadyDuration = SndBase::m_fRunningTime;
    } else {
        if (SteadyFrameCnt == 0) {
            SteadyFrameCnt = static_cast< unsigned short >(g_pEAXSound->Random(0x96) + 0x3C);
            m_pEngineCtl->bPlayCompression = true;
        }
        SteadyFrameCnt = static_cast< unsigned short >(SteadyFrameCnt - 1);
    }

    output = smooth(GetDMIX_InputValue(0), bOutputOn ? 0x7FFF : 0, 0x3D7, 0xC4);
    SetDMIX_Input(0, output);

    AvgDeltaRPM = m_AvgDeltaRPM.GetLastRecordedValue();
    float accelThreshold = m_pEAXCar->GetAttributes().AccelDeltaRPMThreshold();
    PercentOfThreshold = 1.0f - (accelThreshold - bAbs(AvgDeltaRPM)) / accelThreshold;
    PercentOfThreshold = bClamp(PercentOfThreshold, 0.0f, 1.0f);
    output = smooth(GetDMIX_InputValue(1), static_cast< int >(PercentOfThreshold * 32767.0f), 3000);

    if (!m_pEngineCtl->bWasRedlining &&
        m_pEngineCtl->bIsRedlining) {
        mPrevDeltaRPM = output / 2;
    }

    {
        int shiftActive = 1;

        if (m_pShiftingCtl->eShiftState == SHFT_NONE) {
            shiftActive = 0;
        }

        if (shiftActive == 0) {
        EAX_CarState *physCar = m_pStateBase->GetPhysCar();
        int wheelsOnGround = 0;

            for (int i = 0; i < 4; i++) {
                if (physCar->mWheel[i].mWheelOnGround != 0) {
                    wheelsOnGround++;
                }
            }

            if (0.0f < static_cast<float>(wheelsOnGround)) {
                SetDMIX_Input(1, output);
            }
        }
    }

    if (m_pEngineCtl->bIsRedlining) {
        SetDMIX_Input(1, mPrevDeltaRPM);
    }
}
