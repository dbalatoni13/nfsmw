#include "./CARSFX_Nitrous.hpp"

#include "Speed/Indep/Src/EAXSound/EAXAemsManager.h"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

enum eNFSSndNOSClass {
    AEMS_NOS_00 = 0,
    AEMS_NOS_01 = 1,
    MAX_AEMS_NOS = 2,
};

SndBase::TypeInfo CARSFX_Nitrous::s_TypeInfo = {0, "CARSFX_Nitrous", nullptr, CARSFX_Nitrous::CreateObject};

SndBase::TypeInfo *CARSFX_Nitrous::GetTypeInfo() const { return &s_TypeInfo; }

const char *CARSFX_Nitrous::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *CARSFX_Nitrous::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (s_TypeInfo.typeName, false) CARSFX_Nitrous();
    }
    return new (s_TypeInfo.typeName, true) CARSFX_Nitrous();
}

CARSFX_Nitrous::CARSFX_Nitrous()
    : CARSFX() {
    m_NitrousControl = nullptr;
    eNitrousState = SFX_NITROUS_NONE;
    m_NitrousPurge = nullptr;
    m_pShiftCtl = nullptr;
}

CARSFX_Nitrous::~CARSFX_Nitrous() {
    Destroy();
}

int CARSFX_Nitrous::GetController(int Index) {
    switch (Index) {
    case 0:
        return 7;
    case 1:
        return 2;
    default:
        return -1;
    }
}

void CARSFX_Nitrous::AttachController(SFXCTL *psfxctl) {
    switch (psfxctl->GetObjectIndex()) {
    case 7:
        m_p3DCarPosCtl = static_cast<SFXCTL_3DCarPos *>(psfxctl);
        break;
    case 2:
        m_pShiftCtl = static_cast<SFXCTL_Shifting *>(psfxctl);
        break;
    }
}

void CARSFX_Nitrous::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    m_UGL = m_pEAXCar->m_NOSUGL;
    m_NitrousPitchBoost.Initialize(0.0f, 0.0f, 1, LINEAR);
}

void CARSFX_Nitrous::InitSFX() {
    SndBase::InitSFX();
    Enable();
}

int CARSFX_Nitrous::Play(int type, int, int) {
    int nDMixOut;
    int m_NitrousControl_ref;

    if (IsEnabled() && IsSoundEnabled == 1) {
        nDMixOut = GetDMixOutput(1, DMX_VOL);
        if (m_NitrousControl) {
            delete m_NitrousControl;
            m_NitrousControl = nullptr;
        }

        g_pEAXSound->SetCsisName(this);
        m_NitrousControl = new FX_NITROUS(type, nDMixOut, GetDMixOutput(0, DMX_AZIM), 0, 0, 25000, 0, 0x7FFF, 0);

        m_NitrousControl_ref = 0;
        if (m_NitrousControl) {
            m_NitrousControl_ref = m_NitrousControl->GetRefCount();
        }

        if (INIS::Exists()) {
            SetDMIX_Input(1, 0);
        } else {
            SetDMIX_Input(1, 0x7FFF);
        }
    }

    return 0;
}

void CARSFX_Nitrous::Stop() {
    SetDMIX_Input(1, 0);

    if (m_NitrousControl) {
        m_NitrousControl->SetNIT_STOP(1);
        m_NitrousControl->CommitMemberData();
    }
}

void CARSFX_Nitrous::PlayPurge() {
    GetPhysCar()->mNosEmptyFlag = false;

    if (m_NitrousPurge) {
        delete m_NitrousPurge;
        m_NitrousPurge = nullptr;
    }

    g_pEAXSound->SetCsisName(this);
    m_NitrousPurge = new FX_PURGE(0, 0, 0, 0, 0x61A8, 0, 0x7FFF, 0);

    if (INIS::Exists()) {
        SetDMIX_Input(2, 0);
    } else {
        SetDMIX_Input(2, 0x7FFF);
    }
}

void CARSFX_Nitrous::Destroy() {
    if (m_NitrousControl) {
        delete m_NitrousControl;
    }
    m_NitrousControl = nullptr;

    if (m_NitrousPurge) {
        delete m_NitrousPurge;
    }
    m_NitrousPurge = nullptr;
}

void CARSFX_Nitrous::UpdateParams(float) {
    int nSTATE;
    unsigned int uNOSHash;

    if (reinterpret_cast<unsigned char *>(&objectID)[1] != 2 || !IsEnabled()) {
        return;
    }

    uNOSHash = bStringHash("GB_NOS");
    nSTATE = eNitrousState;
    switch (nSTATE) {
    case SFX_NITROUS_NONE:
        if (m_pEAXCar->GetPhysCar()->GetNitroFlag()) {
            eNitrousState = SFX_NITROUS_ON;
            Play(0, 0, m_pEAXCar->m_Rotation);
            m_NitrousPitchBoost.Initialize(0.0f, 1.0f, 400, LINEAR);
        }
        break;
    case SFX_NITROUS_ON:
        if (!m_pEAXCar->GetPhysCar()->GetNitroFlag()) {
            if (m_NitrousControl) {
                Stop();
            }
            m_NitrousPitchBoost.Initialize(m_NitrousPitchBoost.CurValue, 0.0f, 1000, LINEAR);
            eNitrousState = SFX_NITROUS_NONE;
        }
        break;
    }

    if (GetPhysCar()->mNosEmptyFlag && !m_NitrousPurge) {
        PlayPurge();
    }
}

void CARSFX_Nitrous::ProcessUpdate() {
    if (m_NitrousControl) {
        if (INIS::Exists()) {
            SetDMIX_Input(1, 0);
            SetDMIX_Input(2, 0);
        }

        if (m_NitrousControl->GetRefCount() < 2) {
            delete m_NitrousControl;
            m_NitrousControl = nullptr;
        } else {
            int nDMixOut;
            int Pitch;

            nDMixOut = GetDMixOutput(1, DMX_VOL);
            m_NitrousControl->SetNIT_volume(nDMixOut);
            nDMixOut = GetDMixOutput(0, DMX_AZIM);
            m_NitrousControl->SetNIT_azimuth(nDMixOut);
            Pitch = GetDMixOutput(3, DMX_PITCH);
            m_NitrousControl->SetPitch(Pitch);
            nDMixOut = GetDMixOutput(4, DMX_FREQ);
            m_NitrousControl->SetFilter_Effects_LoPass(nDMixOut);
            m_NitrousControl->CommitMemberData();
        }
    }

    if (m_NitrousPurge) {
        if (m_NitrousPurge->GetRefCount() < 2) {
            delete m_NitrousPurge;
            m_NitrousPurge = nullptr;
        } else {
            int Pitch;

            m_NitrousPurge->SetPURGE_volume(GetDMixOutput(2, DMX_VOL));
            m_NitrousPurge->SetPURGE_azimuth(GetDMixOutput(0, DMX_AZIM));
            Pitch = 0x1000;
            m_NitrousPurge->SetPURGE_pitch(Pitch);
            m_NitrousPurge->CommitMemberData();
        }
    }
}
void CARSFX_Nitrous::SetupLoadData() {
    eNFSSndNOSClass nbankindex;

    nbankindex = AEMS_NOS_00;
    if (m_UGL != AEMS_LEVEL1) {
        if (m_UGL > AEMS_LEVEL1) {
            if (m_UGL == AEMS_LEVEL2) {
                nbankindex = AEMS_NOS_01;
            } else if (m_UGL == AEMS_LEVEL3) {
                nbankindex = AEMS_NOS_01;
            }
        }
    }

    LoadAsset(
        g_pEAXSound->GetAttributes()->AEMS_NOSBanks(nbankindex), SNDPATH_NOS, EAXSND_DT_AEMS_ASYNCSPUMEM, eBANK_SLOT_NONE, true);
}

