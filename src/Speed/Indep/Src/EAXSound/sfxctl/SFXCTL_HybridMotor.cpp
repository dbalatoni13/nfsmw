#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_HybridMotor.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_AccelTrans.hpp"
#include "Speed/Indep/Src/EAXSound/OldSoundTemplates.hpp"

SFXCTL_HybridMotor::SFXCTL_HybridMotor()
    : m_pEngineCtl(nullptr) //
    , m_pShiftingCtl(nullptr) //
    , m_pAccelTranCtl(nullptr) //
    , tSteadyDuration(0.0f) //
    , m_EngVolAEMS(0) //
    , m_EngVolAccelGinsu(0) //
    , m_EngVolDecelGinsu(0) {
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
    float shiftVol = 0.0f;
    float accelVol = 0.0f;
    float engineTrq = 0.0f;

    if (m_pShiftingCtl != nullptr) {
        shiftVol = m_pShiftingCtl->GetShiftingVOL();
    }
    if (m_pAccelTranCtl != nullptr) {
        accelVol = m_pAccelTranCtl->m_InterpEngVol.CurValue;
    }
    if (m_pEngineCtl != nullptr) {
        engineTrq = m_pEngineCtl->m_fEng_Trq;
    }

    m_EngVolAEMS = static_cast<int>(shiftVol);
    m_EngVolAccelGinsu = static_cast<int>(accelVol);
    m_EngVolDecelGinsu = static_cast<int>(engineTrq);

    SetDMIX_Input(DMX_VOL, m_EngVolAEMS);
    SetDMIX_Input(DMX_FREQ, m_EngVolAccelGinsu);
    SetDMIX_Input(DMX_PITCH, m_EngVolDecelGinsu);
}

void SFXCTL_HybridMotor::UpdateSingleMixEng(float t) {
    float avgDeltaRpm = m_AvgDeltaRPM.GetValue() + 1.0f;
    if (avgDeltaRpm < 0.0f) {
        avgDeltaRpm = -avgDeltaRpm;
    }

    float accelThreshold = 0.0f;
    float decelThreshold = 0.0f;
    int targetCutoff = 0;
    float engineVolScale = 0.0f;

    if (m_pEngineCtl != nullptr) {
        float trqBase = m_pEngineCtl->m_fEng_Trq;
        if (trqBase < 0.01f) {
            trqBase = 0.01f;
        }
        float rpmBase = m_pEngineCtl->m_fSmoothedEng_RPM;
        if (rpmBase < 0.01f) {
            rpmBase = 0.01f;
        }

        accelThreshold = 1.0f - (trqBase - avgDeltaRpm) / trqBase;
        decelThreshold = 1.0f - (rpmBase - avgDeltaRpm) / rpmBase;
        if (accelThreshold < 0.0f) {
            accelThreshold = 0.0f;
        } else if (accelThreshold > 1.0f) {
            accelThreshold = 1.0f;
        }
        if (decelThreshold < 0.0f) {
            decelThreshold = 0.0f;
        } else if (decelThreshold > 1.0f) {
            decelThreshold = 1.0f;
        }

        targetCutoff = m_pEngineCtl->m_DistanceFltr;
        engineVolScale = static_cast<float>(m_pEngineCtl->m_iEngineVol) * (1.0f / 32767.0f);
    }

    bool smoothEngineMix = true;
    if (m_pShiftingCtl != nullptr) {
        SHIFT_STAGE shiftState = m_pShiftingCtl->eShiftState;
        if (shiftState != SHFT_NONE && shiftState != SHFT_UP_LFO) {
            smoothEngineMix = false;
        }
    }
    if (m_pAccelTranCtl != nullptr && m_pAccelTranCtl->eAccelTransFxState == 1) {
        smoothEngineMix = false;
    }

    float mixAems = accelThreshold;
    float mixAccel = accelThreshold;
    float mixDecel = decelThreshold;

    int targetAemsVol = static_cast<int>(mixAems * engineVolScale * 25000.0f);
    int targetAccelVol = static_cast<int>(mixAccel * engineVolScale * 25000.0f);
    int targetDecelVol = static_cast<int>(mixDecel * engineVolScale * 25000.0f);

    if (!smoothEngineMix) {
        m_EngVolAEMS = targetAemsVol;
        m_EngVolAccelGinsu = targetAccelVol;
        m_EngVolDecelGinsu = targetDecelVol;
    } else {
        m_EngVolAEMS = smooth(m_EngVolAEMS, targetAemsVol, 7000);
        m_EngVolAccelGinsu = smooth(m_EngVolAccelGinsu, targetAccelVol, 7000);
        m_EngVolDecelGinsu = smooth(m_EngVolDecelGinsu, targetDecelVol, 7000);
        targetCutoff = smooth(targetCutoff, targetCutoff, 6000);
    }

    SetDMIX_Input(DMX_VOL, m_EngVolAEMS);
    SetDMIX_Input(DMX_FREQ, m_EngVolAccelGinsu);
    SetDMIX_Input(DMX_PITCH, m_EngVolDecelGinsu);
    SetDMIX_Input(DMX_AZIM, targetCutoff);
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
    SetDMIX_Input(DMX_VOL, m_EngVolAEMS);
    SetDMIX_Input(DMX_FREQ, m_EngVolAccelGinsu);
    SetDMIX_Input(DMX_PITCH, m_EngVolDecelGinsu);
}
