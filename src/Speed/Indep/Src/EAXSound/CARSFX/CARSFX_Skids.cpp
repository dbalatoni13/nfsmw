#include "./CARSFX_Skids.hpp"

#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/OldSoundTemplates.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Wheel.hpp"
#include "Speed/Indep/Src/EAXSound/snd_gen/MAIN_AEMS.h"
#include "Speed/Indep/Src/World/RaceParameters.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

SndBase::TypeInfo SFXCTL_3DRightWheelPos::s_TypeInfo = {
    0x000200B0, "SFXCTL_3DRightWheelPos", &SFXCTL_3DObjPos::s_TypeInfo, SFXCTL_3DRightWheelPos::CreateObject};
SndBase::TypeInfo SFXCTL_3DLeftWheelPos::s_TypeInfo = {
    0x000200C0, "SFXCTL_3DLeftWheelPos", &SFXCTL_3DObjPos::s_TypeInfo, SFXCTL_3DLeftWheelPos::CreateObject};
SndBase::TypeInfo CARSFX_TrafficSkids::s_TypeInfo = {
    0x00050030, "CARSFX_TrafficSkids", &CARSFX_Skids::s_TypeInfo, CARSFX_TrafficSkids::CreateObject};
SndBase::TypeInfo CARSFX_Skids::s_TypeInfo = { 0, "CARSFX_Skids", nullptr, CARSFX_Skids::CreateObject };

SndBase::TypeInfo *CARSFX_Skids::GetTypeInfo() const { return &s_TypeInfo; }

const char *CARSFX_Skids::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *CARSFX_Skids::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (GetStaticTypeInfo()->typeName, false) CARSFX_Skids();
    }
    return new (GetStaticTypeInfo()->typeName, true) CARSFX_Skids();
}

CARSFX_Skids::CARSFX_Skids()
    : CARSFX() {
    m_pSkidControl = nullptr;
    m_SkidAzimuth[0] = 0;
    m_SkidAzimuth[1] = 0;
}

CARSFX_Skids::~CARSFX_Skids() {
    Destroy();
}

void CARSFX_Skids::Destroy() {
    if (m_pSkidControl) {
        delete m_pSkidControl;
        m_pSkidControl = nullptr;
    }
    Disable();
}

int CARSFX_Skids::GetController(int Index) {
    switch (Index) {
    case 0:
        return 1;
    case 1:
        return 0xB;
    case 2:
        return 0xC;
    default:
        return -1;
    }
}

void CARSFX_Skids::AttachController(SFXCTL *psfxctl) {
    unsigned int objIndex;

    objIndex = psfxctl->GetObjectIndex();
    switch (objIndex) {
    case 1:
        m_pWheelCtl = static_cast<SFXCTL_Wheel *>(psfxctl);
        break;
    case 0xB:
        m_pRightWheelPos = static_cast<SFXCTL_3DRightWheelPos *>(psfxctl);
        m_pRightWheelPos->AssignPositionVector(m_pWheelCtl->GetWheelPos(1, 2));
        m_pRightWheelPos->AssignVelocityVector(nullptr);
        break;
    case 0xC:
        m_pLeftWheelPos = static_cast<SFXCTL_3DLeftWheelPos *>(psfxctl);
        m_pLeftWheelPos->AssignPositionVector(m_pWheelCtl->GetWheelPos(0, 2));
        m_pLeftWheelPos->AssignVelocityVector(nullptr);
        break;
    default:
        break;
    }
}

void CARSFX_Skids::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
}

void CARSFX_Skids::InitSFX() {
    int numskids;

    if (!GetPhysCar()) {
        return;
    }

    SndBase::InitSFX();
    numskids = 0;
    if (TheRaceParameters.IsDriftRace()) {
        numskids = 1;
    }
    SkidType = numskids;
    if (m_pSkidControl) {
        delete m_pSkidControl;
    }

    g_pEAXSound->SetCsisName(this);
    numskids = SkidType;
    if (numskids < 0) {
        numskids = 0;
    } else if (numskids > 3) {
        numskids = 3;
    }
    m_pSkidControl = new FX_SKID(0, 0, 0, 0, numskids, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25000, 0, 0x7FFF, 0);
    numskids = m_pSkidControl->GetRefCount();
    Enable();
}

void CARSFX_Skids::Detach() {
    if (m_pSkidControl) {
        delete m_pSkidControl;
    }
    m_pSkidControl = nullptr;
}

void CARSFX_Skids::UpdateMixerOutputs() {
    int frontBackOutput;
    int leftRightOutput;
    int output;

    if (!m_pSkidControl) {
        return;
    }

    output = bAbs(m_pSkidControl->mData.front_FB);
    frontBackOutput = bAbs(m_pSkidControl->mData.back_FB);
    if (frontBackOutput > output) {
        output = frontBackOutput;
    }
    output <<= 5;
    frontBackOutput = 0;
    if (output > 0) {
        frontBackOutput = output;
    }
    if (frontBackOutput > 0x7FFF) {
        frontBackOutput = 0x7FFF;
    }
    SetDMIX_Input(0, frontBackOutput);
    SetDMIX_Input(2, frontBackOutput);

    output = bAbs(m_pSkidControl->mData.front_LR);
    leftRightOutput = bAbs(m_pSkidControl->mData.back_LR);
    if (leftRightOutput > output) {
        output = leftRightOutput;
    }
    output <<= 5;
    leftRightOutput = 0;
    if (output > 0) {
        leftRightOutput = output;
    }
    if (leftRightOutput > 0x7FFF) {
        leftRightOutput = 0x7FFF;
    }
    leftRightOutput = smooth(GetDMIX_InputValue(1), leftRightOutput, 500);
    SetDMIX_Input(1, leftRightOutput);

    output = leftRightOutput;
    if (frontBackOutput > leftRightOutput) {
        output = frontBackOutput;
    }
    output = smooth(GetDMIX_InputValue(3), output, 3000);
    SetDMIX_Input(3, output);
}

void CARSFX_Skids::ProcessUpdate() {
    int skid_type;
    float Speed;

    if (!IsEnabled() || !m_pSkidControl) {
        return;
    }

    m_pSkidControl->SetVOL_Fwd(GetDMixOutput(5, DMX_VOL));
    m_pSkidControl->SetVOL_Side(GetDMixOutput(7, DMX_VOL));
    m_pSkidControl->SetVOL_Back(GetDMixOutput(6, DMX_VOL));
    m_pSkidControl->SetFilter_Effects_Wet_FX(GetDMixOutput(9, DMX_VOL));
    m_pSkidControl->SetPITCH_OFFSET(GetDMixOutput(8, DMX_PITCH));
    m_pSkidControl->SetAzimuth(GetDMixOutput(1, DMX_AZIM));

    skid_type = static_cast<int>(m_pWheelCtl->LeftSideTerrain.Aud_Skid_Type());
    if (skid_type < static_cast<int>(m_pWheelCtl->RightSideTerrain.Aud_Skid_Type())) {
        skid_type = static_cast<int>(m_pWheelCtl->RightSideTerrain.Aud_Skid_Type());
    }
    m_pSkidControl->SetSurface(skid_type);

    Speed = m_pStateBase->GetPhysCar()->GetVelocityMagnitudeMPH();
    m_pSkidControl->SetSPEED(static_cast<int>(Speed));
    m_pSkidControl->SetUNDERSTEER(static_cast<int>(m_pStateBase->GetPhysCar()->mUndersteer));
    m_pSkidControl->SetOVERSTEER(static_cast<int>(m_pStateBase->GetPhysCar()->mOversteer));

    if (!m_pWheelCtl->RightSideTouchingGround && !m_pWheelCtl->LeftSideTouchingGround) {
        m_pSkidControl->SetFront_FB(0);
        m_pSkidControl->SetFront_LR(0);
        m_pSkidControl->SetFront_Load(0);
        m_pSkidControl->SetBack_FB(0);
        m_pSkidControl->SetBack_LR(0);
        m_pSkidControl->SetBack_Load(0);
    } else {
        m_pSkidControl->SetFront_FB(static_cast<int>((m_pWheelCtl->m_NormWheelSlip[1].x + m_pWheelCtl->m_NormWheelSlip[0].x) * 0.5f));
        m_pSkidControl->SetFront_LR(static_cast<int>((m_pWheelCtl->m_NormWheelSlip[1].y + m_pWheelCtl->m_NormWheelSlip[0].y) * 0.5f));
        m_pSkidControl->SetFront_Load(static_cast<int>((m_pWheelCtl->m_fLoad[1] + m_pWheelCtl->m_fLoad[0]) * 0.5f));
        m_pSkidControl->SetBack_FB(static_cast<int>((m_pWheelCtl->m_NormWheelSlip[3].x + m_pWheelCtl->m_NormWheelSlip[2].x) * 0.5f));
        m_pSkidControl->SetBack_LR(static_cast<int>((m_pWheelCtl->m_NormWheelSlip[3].y + m_pWheelCtl->m_NormWheelSlip[2].y) * 0.5f));
        m_pSkidControl->SetBack_Load(static_cast<int>((m_pWheelCtl->m_fLoad[3] + m_pWheelCtl->m_fLoad[2]) * 0.5f));
    }

    m_pSkidControl->CommitMemberData();
}

SndBase::TypeInfo *SFXCTL_3DRightWheelPos::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *SFXCTL_3DRightWheelPos::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *SFXCTL_3DRightWheelPos::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (GetStaticTypeInfo()->typeName, false) SFXCTL_3DRightWheelPos();
    }
    return new (GetStaticTypeInfo()->typeName, true) SFXCTL_3DRightWheelPos();
}

SndBase::TypeInfo *SFXCTL_3DLeftWheelPos::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *SFXCTL_3DLeftWheelPos::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *SFXCTL_3DLeftWheelPos::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (GetStaticTypeInfo()->typeName, false) SFXCTL_3DLeftWheelPos();
    }
    return new (GetStaticTypeInfo()->typeName, true) SFXCTL_3DLeftWheelPos();
}

SndBase::TypeInfo *CARSFX_TrafficSkids::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *CARSFX_TrafficSkids::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *CARSFX_TrafficSkids::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (GetStaticTypeInfo()->typeName, false) CARSFX_TrafficSkids();
    }
    return new (GetStaticTypeInfo()->typeName, true) CARSFX_TrafficSkids();
}

CARSFX_TrafficSkids::CARSFX_TrafficSkids()
    : CARSFX_Skids() {}

CARSFX_TrafficSkids::~CARSFX_TrafficSkids() {}

void CARSFX_TrafficSkids::Detach() {
    CARSFX_Skids::Detach();
}

int CARSFX_TrafficSkids::GetController(int Index) {
    if (Index == 0) {
        return 1;
    }
    return -1;
}

void CARSFX_Skids::SetupLoadData() {
    int nlvl;

    nlvl = 0;
    if (m_UGL > 0) {
        nlvl = m_UGL;
    }
    if (nlvl > 1) {
        nlvl = 1;
    }

    LoadAsset(g_pEAXSound->GetAttributes()->AEMS_SkidBanks(nlvl), SNDPATH_SKIDS, EAXSND_DT_AEMS_ASYNCSPUMEM, eBANK_SLOT_NONE, true);
}
