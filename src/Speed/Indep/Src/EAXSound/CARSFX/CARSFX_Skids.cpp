#include "./CARSFX_Skids.hpp"

#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/OldSoundTemplates.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Wheel.hpp"
#include "Speed/Indep/Src/EAXSound/snd_gen/MAIN_AEMS.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

struct SFXCTL_3DLeftWheelPos : public SFXCTL_3DObjPos {};
struct SFXCTL_3DRightWheelPos : public SFXCTL_3DObjPos {};

SndBase::TypeInfo CARSFX_Skids::s_TypeInfo = { 0, "CARSFX_Skids", nullptr, CARSFX_Skids::CreateObject };

SndBase::TypeInfo *CARSFX_Skids::GetTypeInfo() const { return &s_TypeInfo; }

const char *CARSFX_Skids::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *CARSFX_Skids::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (s_TypeInfo.typeName, false) CARSFX_Skids();
    }
    return new (s_TypeInfo.typeName, true) CARSFX_Skids();
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
    if (Index == 1) {
        return 0xB;
    }
    if (Index > 1) {
        if (Index == 2) {
            return 0xC;
        }
    } else if (Index == 0) {
        return 1;
    }
    return -1;
}

void CARSFX_Skids::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
}

void CARSFX_Skids::AttachController(SFXCTL *psfxctl) {
    unsigned int objIndex;

    objIndex = psfxctl->GetObjectIndex();
    if (objIndex == 0xB) {
        m_pRightWheelPos = static_cast<SFXCTL_3DRightWheelPos *>(psfxctl);
        m_pRightWheelPos->AssignPositionVector(m_pWheelCtl->GetWheelPos(1, 2));
        m_pRightWheelPos->AssignVelocityVector(nullptr);
    } else if (objIndex < 0xC) {
        if (objIndex == 1) {
            m_pWheelCtl = static_cast<SFXCTL_Wheel *>(psfxctl);
        }
    } else if (objIndex == 0xC) {
        m_pLeftWheelPos = static_cast<SFXCTL_3DLeftWheelPos *>(psfxctl);
        m_pLeftWheelPos->AssignPositionVector(m_pWheelCtl->GetWheelPos(0, 2));
        m_pLeftWheelPos->AssignVelocityVector(nullptr);
    }
}

void CARSFX_Skids::Detach() {
    if (m_pSkidControl) {
        delete m_pSkidControl;
    }
    m_pSkidControl = nullptr;
}

void CARSFX_Skids::UpdateMixerOutputs() {
    int output;

    if (!m_pSkidControl) {
        return;
    }

    output = bClamp(bMax(bAbs(m_pSkidControl->mData.front_FB), bAbs(m_pSkidControl->mData.back_FB)) << 5, 0, 0x7FFF);
    SetDMIX_Input(0, output);
    SetDMIX_Input(2, output);

    output = bClamp(bMax(bAbs(m_pSkidControl->mData.front_LR), bAbs(m_pSkidControl->mData.back_LR)) << 5, 0, 0x7FFF);
    output = smooth(GetDMIX_InputValue(1), output, 500);
    SetDMIX_Input(1, output);

    output = smooth(GetDMIX_InputValue(3), bMax(GetDMIX_InputValue(1), GetDMIX_InputValue(0)), 3000);
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

    Speed = GetPhysCar()->GetVelocityMagnitudeMPH();
    m_pSkidControl->SetSPEED(static_cast<int>(Speed));
    m_pSkidControl->SetUNDERSTEER(static_cast<int>(GetPhysCar()->mUndersteer));
    m_pSkidControl->SetOVERSTEER(static_cast<int>(GetPhysCar()->mOversteer));

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

void CARSFX_Skids::SetupLoadData() {
    int nlvl;
    Attrib::StringKey bank;

    nlvl = bClamp(static_cast<int>(m_UGL), 0, 1);
    bank = g_pEAXSound->GetAttributes()->AEMS_SkidBanks(nlvl);
    LoadAsset(bank, SNDPATH_SKIDS, EAXSND_DT_AEMS_ASYNCSPUMEM, eBANK_SLOT_NONE, true);
}
