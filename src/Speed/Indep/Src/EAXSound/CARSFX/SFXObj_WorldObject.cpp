#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_WorldObject.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Enviro.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"
#include "Speed/Indep/Src/EAXSound/snd_gen/ENVIRO_AEMS.h"

#line 20 "src/eaxsound/States/Managers/STATEMGR_Enviro.hpp"
ISndAttachable::ISndAttachable()
    : UTL::Collections::Listable<ISndAttachable, 15>() {}
ISndAttachable::~ISndAttachable() {}

const bVector3 *WorldObject::GetPosition() {
    return &m_3DPosition;
}

int WorldObject::GetType() {
    return Types;
}
#line 8 "/home/kabiskac/Documents/clankers/eaxsound/nfsmw/src/Speed/Indep/Src/EAXSound/CARSFX/SFXObj_WorldObject.cpp"

SndBase::TypeInfo SFXObj_WorldObject::s_TypeInfo = {
    0x00060000,
    "SFXObj_WorldObject",
    &SndBase::s_TypeInfo,
    SFXObj_WorldObject::CreateObject,
};

SndBase::TypeInfo *SFXObj_WorldObject::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *SFXObj_WorldObject::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *SFXObj_WorldObject::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (s_TypeInfo.typeName, false) SFXObj_WorldObject();
    }
    return new (s_TypeInfo.typeName, true) SFXObj_WorldObject();
}

SFXObj_WorldObject::SFXObj_WorldObject()
    : CARSFX() {
    m_p3DObjPos = nullptr;
    mObjPos = bVector3(0.0f, 0.0f, 0.0f);
    m_pcsisSFX = nullptr;
}

SFXObj_WorldObject::~SFXObj_WorldObject() {}

void SFXObj_WorldObject::Destroy() {
    delete m_pcsisSFX;
    m_pcsisSFX = nullptr;
}

void SFXObj_WorldObject::ProcessUpdate() {
    if (m_pcsisSFX) {
        CSTATE_WorldObject *pWorldObjBase = static_cast<CSTATE_WorldObject *>(m_pStateBase);
        int volslot;
        int cutoffslot;
        int pitchslot;

        pWorldObjBase->mSndAttachment->GetPosition();
        volslot = GetDMixOutput(1, DMX_VOL);
        m_pcsisSFX->SetVOLUME(volslot);
        pitchslot = GetDMixOutput(2, DMX_PITCH);
        m_pcsisSFX->SetPITCH(pitchslot);
        m_pcsisSFX->SetAZIMUTH(GetDMixOutput(0, DMX_AZIM));
        cutoffslot = GetDMixOutput(3, DMX_FREQ);
        m_pcsisSFX->SetLoPass(cutoffslot);
        m_pcsisSFX->CommitMemberData();
    }
}

int SFXObj_WorldObject::GetController(int Index) {
    if (Index == 0) {
        return 0;
    }
    return -1;
}

void SFXObj_WorldObject::AttachController(SFXCTL *psfxctl) {
    if ((psfxctl->GetObjectIndex() & 0xFFF0) == 0) {
        m_p3DObjPos = static_cast<SFXCTL_3DObjPos *>(psfxctl);
    }
}

void SFXObj_WorldObject::InitSFX() {
    CSTATE_WorldObject *pWorldObjBase;

    SndBase::InitSFX();
    delete m_pcsisSFX;
    m_pcsisSFX = nullptr;
    pWorldObjBase = static_cast<CSTATE_WorldObject *>(m_pStateBase);
    m_p3DObjPos->AssignPositionVector(const_cast<bVector3 *>(pWorldObjBase->mSndAttachment->GetPosition()));
    m_p3DObjPos->AssignVelocityVector(nullptr);
    g_pEAXSound->SetCsisName(this);
    m_pcsisSFX = new ENV_STATIC(bClamp(pWorldObjBase->mSndAttachment->GetType(), 0, 0x32),
                                0,
                                0,
                                0,
                                ENVSTATICTYPETYPE_ENV_COMMON,
                                25000,
                                0,
                                0x7FFF,
                                0);
}

void SFXObj_WorldObject::Detach() {
    m_p3DObjPos->AssignPositionVector(nullptr);
    m_p3DObjPos->AssignVelocityVector(nullptr);
    delete m_pcsisSFX;
    m_pcsisSFX = nullptr;
}

template class UTL::Collections::Listable<ISndAttachable, 15>::List;
template class UTL::Vector<ISndAttachable *, 16>;
template class UTL::FixedVector<ISndAttachable *, 15, 16>;
