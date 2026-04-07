#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Helicopter.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/SimStates/EAX_HeliState.hpp"

SndBase::TypeInfo SFXObj_Helicopter::s_TypeInfo = {0x00000B00, "SFXObj_Helicopter", &SndBase::s_TypeInfo, SFXObj_Helicopter::CreateObject};

SndBase::TypeInfo *SFXObj_Helicopter::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFXObj_Helicopter::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXObj_Helicopter::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXObj_Helicopter::GetStaticTypeInfo()->typeName, false) SFXObj_Helicopter();
    }
    return new (SFXObj_Helicopter::GetStaticTypeInfo()->typeName, true) SFXObj_Helicopter();
}

SFXObj_Helicopter::SFXObj_Helicopter()
    : CARSFX()
    , m_pCsisHeli(nullptr) //
    , m_pHeliCtl(nullptr) {
    m_HeliAemsData.hELI_ID = 0;
    m_HeliAemsData.hELI_Pitch_Offset = 0;
    m_HeliAemsData.hELI_Stop = 0;
    m_HeliAemsData.hELI_Volume = 0;
    m_HeliAemsData.hELI_Azmuth = 0;
    m_HeliAemsData.hELI_Speed = 0;
    m_HeliAemsData.hELI_Distance = 0;
    m_HeliAemsData.hELI_LowPass = 16000;
    m_HeliAemsData.hELI_HiPass = 0;
    m_HeliAemsData.hELI_FX_Dry = 0;
    m_HeliAemsData.hELI_FX_Wet = 0;
    m_HeliAemsData.hELI_Rotation = 0;
}

SFXObj_Helicopter::~SFXObj_Helicopter() {
    Destroy();
}

void SFXObj_Helicopter::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
}

void SFXObj_Helicopter::InitSFX() {
    g_pEAXSound->SetCsisName(this);
    m_pCsisHeli = new FX_Helicopter(m_HeliAemsData.hELI_ID, m_HeliAemsData.hELI_Pitch_Offset, m_HeliAemsData.hELI_Stop, m_HeliAemsData.hELI_Volume,
                                    m_HeliAemsData.hELI_Azmuth, m_HeliAemsData.hELI_Speed, m_HeliAemsData.hELI_Distance, m_HeliAemsData.hELI_LowPass,
                                    m_HeliAemsData.hELI_HiPass, m_HeliAemsData.hELI_FX_Dry, m_HeliAemsData.hELI_FX_Wet, m_HeliAemsData.hELI_Rotation);
    SndBase::InitSFX();
}

void SFXObj_Helicopter::Detach() {
    Destroy();
}

int SFXObj_Helicopter::GetController(int Index) {
    if (Index == 0) {
        return 0;
    }
    return -1;
}

void SFXObj_Helicopter::AttachController(SFXCTL *psfxctl) {
    if ((psfxctl->GetTypeInfo()->ObjectID & 0xFFF0) == 0) {
        m_pHeliCtl = static_cast<SFXCTL_Helicopter *>(psfxctl);
    }
}

void SFXObj_Helicopter::Destroy() {
    if (m_pCsisHeli) {
        delete m_pCsisHeli;
    }
    m_pCsisHeli = nullptr;
}

void SFXObj_Helicopter::UpdateParams(float) {}

void SFXObj_Helicopter::ProcessUpdate() {
    if (m_pHeliCtl && m_pHeliCtl->m_pHeliState->IsSimUpdating()) {
        int nazim = GetDMixOutput(0, DMX_AZIM);

        m_HeliAemsData.hELI_Speed = static_cast<int>(m_pHeliCtl->m_fspeed);
        m_HeliAemsData.hELI_Distance = static_cast<int>(m_pHeliCtl->m_fdist);
        m_HeliAemsData.hELI_Rotation = m_pHeliCtl->m_Rotation;

        if (nazim < 0) {
            nazim = 0;
        } else if (nazim > 0xFFFF) {
            nazim = 0xFFFF;
        }

        if (m_HeliAemsData.hELI_Distance > 100) {
            m_HeliAemsData.hELI_Distance = 100;
        }

        if (nazim - 0x4001U < 0x7FFF) {
            m_HeliAemsData.hELI_Distance = -m_HeliAemsData.hELI_Distance;
        }

        if (!m_pCsisHeli) {
            return;
        }

        m_pCsisHeli->SetHELI_Azmuth(nazim);
        m_pCsisHeli->SetHELI_Distance(m_HeliAemsData.hELI_Distance);
        m_pCsisHeli->SetHELI_Volume(GetDMixOutput(1, DMX_VOL));
        m_pCsisHeli->SetHELI_FX_Dry(GetDMixOutput(1, DMX_VOL));
        m_pCsisHeli->SetHELI_Speed(m_HeliAemsData.hELI_Speed);
        m_pCsisHeli->SetHELI_Pitch_Offset(GetDMixOutput(2, DMX_PITCH));
        m_pCsisHeli->SetHELI_Rotation(m_HeliAemsData.hELI_Rotation);
        m_pCsisHeli->CommitMemberData();
    } else if (m_pCsisHeli) {
        m_pCsisHeli->SetHELI_Volume(0);
        m_pCsisHeli->SetHELI_FX_Dry(0);
        m_pCsisHeli->CommitMemberData();
    }
}
