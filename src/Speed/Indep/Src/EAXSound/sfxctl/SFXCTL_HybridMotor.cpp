#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_HybridMotor.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_AccelTrans.hpp"
#include "Speed/Indep/Src/EAXSound/OldSoundTemplates.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engineaudio.h"

namespace {
static EAX_CarState *GetHybridStateCar(CSTATE_Base *stateBase) {
    return *static_cast<EAX_CarState **>(
        static_cast<void *>(static_cast<char *>(static_cast<void *>(stateBase)) + 0x34));
}
} // namespace

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
    , m_GinsuLPFVal(0.0f) //
    , m_bAEMSLPF(false) //
    , SteadyFrameCnt(0) //
    , m_CurPhyDeltaRPMVal(0.0f) //
    , m_CurAudDeltaRPMVal(0.0f) //
    , PrevRPM(0.0f) //
    , m_GinsuScaledRPM(0.0f) //
    , mPrevPhyDeltaRPM(0.0f) //
    , PercentOfAccelThreshold(0.0f) //
    , PercentOfDecelThreshold(0.0f) {
    m_EngineMix.Aems = 0.0f;
    m_EngineMix.Cutoff = 0;
    m_EngineMix.AccelGinsu = 0.0f;
    m_EngineMix.DecelGinsu = 0.0f;
    m_AvgDeltaRPM.Reset(0.0f);
}

SFXCTL_HybridMotor::~SFXCTL_HybridMotor() {}

SndBase::TypeInfo *SFXCTL_HybridMotor::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_HybridMotor::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXCTL_HybridMotor::CreateObject(unsigned int) { return new SFXCTL_HybridMotor(); }

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

void SFXCTL_HybridMotor::InitSFX() {
    SFXCTL::InitSFX();
    m_EngVolAEMS = 0;
    m_EngVolAccelGinsu = 0;
    m_EngVolDecelGinsu = 0;
}

void SFXCTL_HybridMotor::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    if (m_pEAXCar != nullptr) {
        m_UGL = static_cast<eAemsUpgradeLevel>(m_pEAXCar->GetEngineUpgradeLevel());
    }
    m_AvgDeltaRPM.Reset(0.0f);
}

void SFXCTL_HybridMotor::AttachController(SFXCTL *ctrl) {
    int id = (ctrl->GetTypeInfo()->ObjectID >> 4) & 0xFFF;
    switch (id) {
    case 4:
        m_pEngineCtl = static_cast<SFXCTL_Engine *>(ctrl);
        break;
    case 2:
        m_pShiftingCtl = static_cast<SFXCTL_Shifting *>(ctrl);
        break;
    case 3:
        m_pAccelTranCtl = static_cast<SFXCTL_AccelTrans *>(ctrl);
        break;
    }
}

void SFXCTL_HybridMotor::UpdateDeltaRPM() {
    if (m_pEngineCtl == nullptr) {
        m_AvgDeltaRPM.Reset(0.0f);
        return;
    }
    float deltaRPM = m_pEngineCtl->m_fEng_RPM - m_pEngineCtl->m_fPrevRPM;
    m_AvgDeltaRPM.Reset(deltaRPM);
}

void SFXCTL_HybridMotor::UpdateParams(float t) {
    UpdateDeltaRPM();
    UpdateSingleMixEng(t);
    UpdateDualMixEng(t);
    UpdateVolumeRedlining();
}

void SFXCTL_HybridMotor::UpdateDualMixEng(float t) {
    (void)t;
    bool bDoSmooth = true;
    Slope TrqThreshold(0.0f, 1.0f, 0.0f, 35.0f);
    float trqWeight = TrqThreshold.GetValue(m_pEngineCtl->Trq.GetValue());

    Attrib::Gen::engineaudio *engineInfo =
        static_cast<Attrib::Gen::engineaudio *>(static_cast<void *>(m_pEAXCar->mEngineInfo));

    float deltaRPM = bAbs(m_AvgDeltaRPM.GetValue() + 10.0f);
    float accelThresholdRange = engineInfo->AccelDeltaRPMThreshold();
    float accelPct = 1.0f - (accelThresholdRange - deltaRPM) / accelThresholdRange;
    if (accelPct < 0.0f) {
        accelPct = 0.0f;
    }
    PercentOfAccelThreshold = accelPct;
    if (PercentOfAccelThreshold > 1.0f) {
        PercentOfAccelThreshold = 1.0f;
    }

    float decelThresholdRange = engineInfo->DecelDeltaRPMThreshold();
    float decelPct = 1.0f - (decelThresholdRange - deltaRPM) / decelThresholdRange;
    if (decelPct < 0.0f) {
        decelPct = 0.0f;
    }
    PercentOfDecelThreshold = decelPct;
    if (PercentOfDecelThreshold > 1.0f) {
        PercentOfDecelThreshold = 1.0f;
    }

    SFXCTL_Physics *physicsCtl = m_pEAXCar->m_pPhysicsCTL;
    if (*static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(physicsCtl)) + 0xB4)) == 0) {
        bool bDisableSmooth = false;
        EAX_CarState *stateCar =
            m_pStateBase != nullptr
                ? GetHybridStateCar(m_pStateBase)
                : nullptr;
        int gearShiftFlag = stateCar != nullptr
                                ? stateCar->mDriveline.mGearShiftFlag
                                : 0;
        if (gearShiftFlag == 0 && m_pShiftingCtl->eShiftState == SHFT_NONE) {
            int accelState = m_pAccelTranCtl->eAccelTransFxState;
            if (accelState != 0) {
                bDisableSmooth = (accelState == 1);
            }
        } else {
            bDisableSmooth = (m_pShiftingCtl->eShiftState == SHFT_UP_ENGAGING);
        }
        if (bDisableSmooth) {
            bDoSmooth = false;
        }
    }

    float accelAemsMix =
        PercentOfAccelThreshold * (engineInfo->AEMSMix_L_RPM() - engineInfo->AEMSMix_S_RPM()) + engineInfo->AEMSMix_S_RPM();
    float accelGinsuMix =
        PercentOfAccelThreshold * (engineInfo->GINSUMix_L_RPM() - engineInfo->GINSUMix_S_RPM()) + engineInfo->GINSUMix_S_RPM();
    float decelAemsMix = PercentOfDecelThreshold * (engineInfo->DECEL_AEMSMix_L_RPM() - engineInfo->DECEL_AEMSMix_S_RPM()) +
                         engineInfo->DECEL_AEMSMix_S_RPM();
    float decelGinsuMix = PercentOfDecelThreshold * (engineInfo->DECEL_GINSUMix_L_RPM() - engineInfo->DECEL_GINSUMix_S_RPM()) +
                          engineInfo->DECEL_GINSUMix_S_RPM();

    float crossfade = DecelCrossfadeMix.GetValue(m_GinsuScaledRPM);
    decelAemsMix = crossfade * decelAemsMix + (1.0f - crossfade);
    decelGinsuMix = DecelCrossfadeMix.GetValue(m_GinsuScaledRPM) * decelGinsuMix;

    float accelGinsuNegMix = engineInfo->Ginsu_ACL_Neg_S_RPM();
    unsigned int lowPassCutoff = m_pEngineCtl->m_DistanceFltr;
    if (engineInfo->GINSU_LowPassCutoff() < lowPassCutoff) {
        lowPassCutoff = engineInfo->GINSU_LowPassCutoff();
    }

    *static_cast<int *>(static_cast<void *>(&m_bAEMSLPF)) = 0;
    int targetCutoff = static_cast<int>((static_cast<float>(25000 - lowPassCutoff)) * trqWeight + static_cast<float>(lowPassCutoff));
    float targetAems = (accelAemsMix - decelAemsMix) * trqWeight + decelAemsMix;
    float targetDecelGinsu = (0.0f - decelGinsuMix) * trqWeight + decelGinsuMix;
    float targetAccelGinsu = (accelGinsuMix - accelGinsuNegMix) * trqWeight + accelGinsuNegMix;

    if (!bDoSmooth) {
        m_EngineMix.Aems = targetAems;
        m_EngineMix.Cutoff = targetCutoff;
        m_EngineMix.AccelGinsu = targetAccelGinsu;
        m_EngineMix.DecelGinsu = targetDecelGinsu;
    } else {
        m_EngineMix.Aems = smooth(m_EngineMix.Aems, targetAems, 0.2f);
        m_EngineMix.AccelGinsu = smooth(m_EngineMix.AccelGinsu, targetAccelGinsu, 0.2f);
        m_EngineMix.DecelGinsu = smooth(m_EngineMix.DecelGinsu, targetDecelGinsu, 0.2f);
        m_EngineMix.Cutoff = smooth(m_EngineMix.Cutoff, targetCutoff, 6000);
    }

    int curveOutput = NFSMixShape::GetCurveOutput(static_cast<NFSMixShape::eMIXTABLEID>(7), m_EngineMix.Cutoff, true);
    m_GinsuLPFVal = static_cast<float>(curveOutput);

    unsigned int mixedCutoff = m_pEngineCtl->m_DistanceFltr;
    if (static_cast<unsigned int>(m_EngineMix.Cutoff) < mixedCutoff) {
        mixedCutoff = static_cast<unsigned int>(m_EngineMix.Cutoff);
    }
    m_GinsuLPFVal = static_cast<float>(mixedCutoff);

    float engineVolScale = static_cast<float>(m_pEngineCtl->m_iEngineVol) * 3.051851e-05f;
    int targetAemsVol = static_cast<int>(m_EngineMix.Aems * static_cast<float>(engineInfo->AEMSVol()) * engineVolScale);
    int targetAccelVol = static_cast<int>(m_EngineMix.AccelGinsu * static_cast<float>(engineInfo->GINSUAccelVol()) * engineVolScale);
    int targetDecelVol = static_cast<int>(m_EngineMix.DecelGinsu * static_cast<float>(engineInfo->GinsuDecelVol()) * engineVolScale);

    if (!bDoSmooth) {
        m_EngVolAEMS = targetAemsVol;
        m_EngVolAccelGinsu = targetAccelVol;
        m_EngVolDecelGinsu = targetDecelVol;
    } else {
        m_EngVolAEMS = smooth(m_EngVolAEMS, targetAemsVol, 7000);
        m_EngVolAccelGinsu = smooth(m_EngVolAccelGinsu, targetAccelVol, 7000);
        m_EngVolDecelGinsu = smooth(m_EngVolDecelGinsu, targetDecelVol, 7000);
    }

    m_EngVolRedLine = static_cast<int>(static_cast<float>(engineInfo->AEMSVol()) * engineVolScale);
}

void SFXCTL_HybridMotor::UpdateSingleMixEng(float t) {
    (void)t;
    bool bDoSmooth = true;
    Slope TrqThreshold(0.0f, 1.0f, 0.0f, 35.0f);

    float trqValue = m_pEngineCtl->Trq.GetValue();
    float trqWeight = TrqThreshold.GetValue(trqValue);

    Attrib::Gen::engineaudio *engineInfo =
        static_cast<Attrib::Gen::engineaudio *>(static_cast<void *>(m_pEAXCar->mEngineInfo));

    float deltaRPM = m_AvgDeltaRPM.GetValue() + 10.0f;
    float accelThresholdRange = engineInfo->AccelDeltaRPMThreshold();
    float accelPct = 1.0f - (accelThresholdRange - bAbs(deltaRPM)) / accelThresholdRange;
    if (accelPct < 0.0f) {
        accelPct = 0.0f;
    }
    if (accelPct > 1.0f) {
        accelPct = 1.0f;
    }
    PercentOfAccelThreshold = accelPct;

    float decelThresholdRange = engineInfo->DecelDeltaRPMThreshold();
    float decelPct = 1.0f - (decelThresholdRange - bAbs(deltaRPM)) / decelThresholdRange;
    if (decelPct < 0.0f) {
        decelPct = 0.0f;
    }
    if (decelPct > 1.0f) {
        decelPct = 1.0f;
    }
    PercentOfDecelThreshold = decelPct;

    EAX_CarState *stateCar =
        m_pStateBase != nullptr
            ? GetHybridStateCar(m_pStateBase)
            : nullptr;
    int gearShiftFlag = stateCar != nullptr
                            ? stateCar->mDriveline.mGearShiftFlag
                            : 0;
    SHIFT_STAGE shiftState = m_pShiftingCtl->eShiftState;

    if (gearShiftFlag == 0 && shiftState == SHFT_NONE) {
        int accelState = m_pAccelTranCtl->eAccelTransFxState;
        if (accelState != 0 && accelState == 1) {
            bDoSmooth = false;
        }
    } else {
        if (shiftState != SHFT_UP_LFO) {
            if (static_cast<int>(shiftState) < 4) {
                if (shiftState == SHFT_UP_ENGAGING) {
                    bDoSmooth = false;
                }
            } else if (shiftState != SHFT_DOWN_ENGAGING_RISE && shiftState == SHFT_DOWN_ENGAGING_REATTACH &&
                       m_pEAXCar->bIsAccelerating) {
                bDoSmooth = false;
            }
        }
    }

    float accelAemsMix =
        PercentOfAccelThreshold * (engineInfo->AEMSMix_L_RPM() - engineInfo->AEMSMix_S_RPM()) + engineInfo->AEMSMix_S_RPM();
    float accelGinsuMix =
        PercentOfAccelThreshold * (engineInfo->GINSUMix_L_RPM() - engineInfo->GINSUMix_S_RPM()) + engineInfo->GINSUMix_S_RPM();
    float decelAemsMix = PercentOfDecelThreshold * (engineInfo->DECEL_AEMSMix_L_RPM() - engineInfo->DECEL_AEMSMix_S_RPM()) +
                         engineInfo->DECEL_AEMSMix_S_RPM();
    float decelGinsuMix = PercentOfDecelThreshold * (engineInfo->Ginsu_ACL_Neg_S_RPM() - engineInfo->Ginsu_ACL_Neg_L_RPM()) +
                          engineInfo->Ginsu_ACL_Neg_S_RPM();

    unsigned int lowPassCutoff = m_pEngineCtl->m_DistanceFltr;
    if (engineInfo->GINSU_LowPassCutoff() < lowPassCutoff) {
        lowPassCutoff = engineInfo->GINSU_LowPassCutoff();
    }

    int targetCutoff = static_cast<int>((static_cast<float>(m_pEngineCtl->m_DistanceFltr) - static_cast<float>(lowPassCutoff)) *
                                            trqWeight +
                                        static_cast<float>(lowPassCutoff));
    float targetAems = (accelAemsMix - decelAemsMix) * trqWeight + decelAemsMix;
    float targetAccelGinsu = (accelGinsuMix - decelGinsuMix) * trqWeight + decelGinsuMix;
    float targetDecelGinsu = 0.0f;

    if (bDoSmooth) {
        m_EngineMix.Aems = smooth(m_EngineMix.Aems, targetAems, 0.2f);
        m_EngineMix.AccelGinsu = smooth(m_EngineMix.AccelGinsu, targetAccelGinsu, 0.2f);
        m_EngineMix.DecelGinsu = smooth(m_EngineMix.DecelGinsu, targetDecelGinsu, 0.2f);
        m_EngineMix.Cutoff = smooth(m_EngineMix.Cutoff, targetCutoff, 6000);
    } else {
        m_EngineMix.Aems = targetAems;
        m_EngineMix.AccelGinsu = targetAccelGinsu;
        m_EngineMix.DecelGinsu = targetDecelGinsu;
        m_EngineMix.Cutoff = targetCutoff;
    }

    int curveOutput = NFSMixShape::GetCurveOutput(
        static_cast<NFSMixShape::eMIXTABLEID>(7), static_cast<int>(static_cast<float>(m_EngineMix.Cutoff) * 1.3106799f), true);
    m_GinsuLPFVal = static_cast<float>(25000 - curveOutput);

    float engineVolScale = static_cast<float>(m_pEngineCtl->m_iEngineVol) * 3.051851e-05f;
    int targetAemsVol = static_cast<int>(m_EngineMix.Aems * static_cast<float>(engineInfo->AEMSVol()) * engineVolScale);
    int targetAccelVol = static_cast<int>(m_EngineMix.AccelGinsu * static_cast<float>(engineInfo->GINSUAccelVol()) * engineVolScale);
    int targetDecelVol = static_cast<int>(m_EngineMix.DecelGinsu * static_cast<float>(engineInfo->GinsuDecelVol()) * engineVolScale);

    if (bDoSmooth) {
        m_EngVolAEMS = smooth(m_EngVolAEMS, targetAemsVol, 7000);
        m_EngVolAccelGinsu = smooth(m_EngVolAccelGinsu, targetAccelVol, 7000);
        m_EngVolDecelGinsu = smooth(m_EngVolDecelGinsu, targetDecelVol, 7000);
    } else {
        m_EngVolAEMS = targetAemsVol;
        m_EngVolAccelGinsu = targetAccelVol;
        m_EngVolDecelGinsu = targetDecelVol;
    }

    m_EngVolRedLine = static_cast<int>(static_cast<float>(engineInfo->AEMSVol()) * engineVolScale);
}

void SFXCTL_HybridMotor::UpdateVolumeRedlining() {
    int redlineVol = 0;
    if (m_pEngineCtl != nullptr) {
        redlineVol = static_cast<int>(m_pEngineCtl->m_fEng_RPM * (1.0f / 16.0f));
        if (redlineVol < 0) {
            redlineVol = 0;
        } else if (redlineVol > 32767) {
            redlineVol = 32767;
        }
    }
    m_EngVolAEMS = smooth(m_EngVolAEMS, redlineVol, 4096);
}

void SFXCTL_HybridMotor::UpdateMixerOutputs() {
    bool bOutputOn = false;
    float AvgDeltaRPM;
    float PercentOfThreshold;
    int output;

    char *physCar = static_cast<char *>(static_cast<void *>(
        *static_cast<void **>(static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pStateBase)) + 0x34))));
    float speedMph =
        bSqrt(*static_cast<float *>(static_cast<void *>(physCar + 0x54)) *
                  *static_cast<float *>(static_cast<void *>(physCar + 0x54)) +
              *static_cast<float *>(static_cast<void *>(physCar + 0x58)) *
                  *static_cast<float *>(static_cast<void *>(physCar + 0x58)) +
              *static_cast<float *>(static_cast<void *>(physCar + 0x5C)) *
                  *static_cast<float *>(static_cast<void *>(physCar + 0x5C))) *
        2.23699f;

    if (speedMph <= 30.0f || 30.0f <= bAbs(m_AvgDeltaRPM.GetValue())) {
        tSteadyDuration = SndBase::m_fRunningTime;
    } else if (tSteadyDuration + 3.0f < SndBase::m_fRunningTime) {
        bOutputOn = true;
    }

    if (bOutputOn) {
        if (SteadyFrameCnt == 0) {
            SteadyFrameCnt = static_cast< unsigned short >(g_pEAXSound->Random(0x96) + 0x3C);
            *static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pEngineCtl)) + 0x14C)) = 1;
        }
        SteadyFrameCnt = static_cast< unsigned short >(SteadyFrameCnt - 1);
    } else {
        tSteadyDuration = SndBase::m_fRunningTime;
    }

    int *outputs = GetOutputBlockPtr();
    output = smooth(outputs[0], bOutputOn ? 0x7FFF : 0, 0x3D7, 0xC4);
    outputs[0] = output;

    AvgDeltaRPM = m_AvgDeltaRPM.GetLastRecordedValue();
    float accelThreshold =
        *static_cast<float *>(static_cast<void *>(
            *static_cast<char **>(static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pEAXCar)) + 0xCC)) + 0x78));
    PercentOfThreshold = 1.0f - (accelThreshold - bAbs(AvgDeltaRPM)) / accelThreshold;
    PercentOfThreshold = bClamp(PercentOfThreshold, 0.0f, 1.0f);
    output = smooth(outputs[1], static_cast< int >(PercentOfThreshold * 32767.0f), 3000);

    if (*static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pEngineCtl)) + 0xD0)) == 0 &&
        *static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pEngineCtl)) + 0xCC)) != 0) {
        mPrevDeltaRPM = output / 2;
    }

    int shiftActive = 0;
    if ((m_pShiftingCtl->eShiftState == SHFT_UP_LFO) || (m_pShiftingCtl->eShiftState != SHFT_NONE)) {
        shiftActive = 1;
    }
    if (shiftActive == 0) {
        int wheelsOnGround = 0;
        for (int i = 0; i < 4; i++) {
            if (*static_cast<int *>(static_cast<void *>(physCar + 0xB8 + i * 0x44)) != 0) {
                wheelsOnGround++;
            }
        }

        if (wheelsOnGround > 0) {
            outputs[1] = output;
        }
    }

    if (*static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pEngineCtl)) + 0xCC)) != 0) {
        outputs[1] = mPrevDeltaRPM;
    }
}
