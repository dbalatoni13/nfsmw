#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_WorldObject.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Enviro.hpp"

DEFINE_CREATABLE(0x60000, SFXCTL_3DFountainPos, SFXCTL_3DObjPos);

DEFINE_CREATABLE(0x60000, SFXObj_WorldObject, SndBase);

SFXObj_WorldObject::SFXObj_WorldObject() : CARSFX() {
    this->m_pcsisSFX = nullptr;
    this->m_p3DObjPos = nullptr;
    this->mObjPos = bVector3(0.0f, 0.0f, 0.0f);
}

SFXObj_WorldObject::~SFXObj_WorldObject() {}

void SFXObj_WorldObject::Destroy() {
    delete this->m_pcsisSFX;
    this->m_pcsisSFX = nullptr;
}

void SFXObj_WorldObject::ProcessUpdate() {
    if (this->m_pcsisSFX != nullptr) {
        CSTATE_WorldObject *pWorldObjBase = static_cast<CSTATE_WorldObject *>(this->m_pStateBase);
        int volslot = 1;
        int cutoffslot = 3;
        int pitchslot = 2;

        pWorldObjBase->mSndAttachment->GetType();
        this->m_pcsisSFX->SetVOLUME(this->GetDMixOutput(volslot, DMX_VOL));

        if (pitchslot >= 0) {
            this->m_pcsisSFX->SetPITCH(this->GetDMixOutput(pitchslot, DMX_PITCH));
        } else {
            this->m_pcsisSFX->SetPITCH(0x1000);
        }

        this->m_pcsisSFX->SetAZIMUTH(this->GetDMixOutput(0, DMX_AZIM));

        if (cutoffslot >= 0) {
            this->m_pcsisSFX->SetLoPass(this->GetDMixOutput(cutoffslot, DMX_FREQ));
        } else {
            this->m_pcsisSFX->SetLoPass(25000);
        }

        this->m_pcsisSFX->CommitMemberData();
    }
}

int SFXObj_WorldObject::GetController(int Index) {
    if (Index == 0) {
        return 0;
    }
    return -1;
}

// UNSOLVED
void SFXObj_WorldObject::AttachController(SFXCTL *psfxctl) {
    if ((psfxctl->GetObjectIndex() & 0xFFF0) == 0) {
        m_p3DObjPos = static_cast<SFXCTL_3DObjPos *>(psfxctl);
    }
}

void SFXObj_WorldObject::InitSFX() {
    SndBase::InitSFX();
    if (this->m_pcsisSFX != nullptr) {
        delete this->m_pcsisSFX;
        this->m_pcsisSFX = nullptr;
    }

    CSTATE_WorldObject *pWorldObjBase = static_cast<CSTATE_WorldObject *>(this->m_pStateBase);
    this->m_p3DObjPos->AssignPositionVector(const_cast<bVector3 *>(pWorldObjBase->mSndAttachment->GetPosition()));
    this->m_p3DObjPos->AssignVelocityVector(nullptr);
    g_pEAXSound->SetCsisName(this);
    this->m_pcsisSFX =
        new Csis::ENV_STATIC(pWorldObjBase->mSndAttachment->GetType(), 0, 0, 0, Csis::ENVSTATICTYPETYPE_ENV_COMMON, 25000, 0, 0x7FFF, 0);
}

void SFXObj_WorldObject::Detach() {
    this->m_p3DObjPos->AssignPositionVector(nullptr);
    this->m_p3DObjPos->AssignVelocityVector(nullptr);
    delete this->m_pcsisSFX;
    this->m_pcsisSFX = nullptr;
}
