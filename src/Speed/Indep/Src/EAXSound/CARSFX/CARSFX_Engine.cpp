#include "./CARSFX_Engine.hpp"

#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"

void CARSFX_GinsuEngine::SetEngineParams() {
    int nDMixOut;
    int TmpVol;
    int nFXDryVol;
    int nFXWetVol;
    int TmpRpmVol;

    if (!IsEnabled()) {
        return;
    }

    m_GinsuRPM = m_pHybridEngCtl->m_GinsuScaledRPM;
    nDMixOut = GetDMixOutput(4, DMX_PITCH);
    PitchMultipli = static_cast<float>(nDMixOut) * 0.000244140625f;
    SetGinsuParams();

    if (m_pcsisCarCtrl) {
        TmpVol = GetDMixOutput(1, DMX_VOL);
        TmpRpmVol = (m_pHybridEngCtl->m_EngVolAEMS * TmpVol >> 15) * 0x7FFF >> 15;
        m_pcsisCarCtrl->SetVOL_ENG(TmpRpmVol - 0x7FFF);
        m_pcsisCarCtrl->SetVOL_EXH(TmpRpmVol - 0x7FFF);
        m_pcsisCarCtrl->SetMAX_RPM(m_pHybridEngCtl->m_EngVolRedLine * TmpVol >> 15);
        m_pcsisCarCtrl->SetRPM(static_cast<int>(m_GinsuRPM));
        m_pcsisCarCtrl->SetCOMMON_PARAMETERS_PITCH_OFFSET(static_cast<int>(PitchMultipli * 16383.0f) - 0x3FFF);
        m_pcsisCarCtrl->SetTORQUE(static_cast<int>(m_pEngineCtl->GetEngTorque() * 10.24f));
        m_pcsisCarCtrl->SetCOMMON_PARAMETERS_ROTATION(m_pEngineCtl->m_Rotation);
        m_pcsisCarCtrl->SetCOMMON_PARAMETERS_AZIMUTH(GetDMixOutput(0, DMX_AZIM));
        m_pcsisCarCtrl->CommitMemberData();
    }

    if (!m_pTranny) {
        return;
    }

    m_pTranny->SetMagnitude(static_cast<int>(GetPhysCar()->GetForwardSpeed() * 15.0f));
    m_pTranny->SetVOL(GetDMixOutput(3, DMX_VOL));
    m_pTranny->SetAZIMUTH(GetDMixOutput(0, DMX_AZIM));
    m_pTranny->SetPITCH_OFFSET(GetDMixOutput(4, DMX_PITCH));
    m_pTranny->SetREVERB_AND_FILTERS_LoPass(GetDMixOutput(5, DMX_FREQ));
    nFXWetVol = m_pTunnelCtl->m_AEMSWetVol;
    nFXDryVol = m_pTunnelCtl->m_AEMSDryVol;
    m_pTranny->SetREVERB_AND_FILTERS_Wet(nFXWetVol);
    m_pTranny->SetREVERB_AND_FILTERS_Dry(nFXDryVol);
    if (m_pShiftingCtl && m_pShiftingCtl->IsActive()) {
        nDMixOut = 1;
    } else {
        nDMixOut = 0;
    }
    m_pTranny->SetSingle_Shot(nDMixOut);
    m_pTranny->CommitMemberData();
}
