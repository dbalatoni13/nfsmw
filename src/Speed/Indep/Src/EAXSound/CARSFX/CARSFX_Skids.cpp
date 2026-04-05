#include "./CARSFX_Skids.hpp"

#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Wheel.hpp"
#include "Speed/Indep/Src/EAXSound/snd_gen/MAIN_AEMS.h"

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
