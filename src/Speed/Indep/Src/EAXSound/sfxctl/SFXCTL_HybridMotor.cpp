#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_HybridMotor.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_AccelTrans.hpp"

SFXCTL_HybridMotor::~SFXCTL_HybridMotor() {}

SndBase::TypeInfo *SFXCTL_HybridMotor::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_HybridMotor::GetTypeName() const { return s_TypeInfo.typeName; }

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
