#include "./CARSFX_Engine.hpp"

#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"

void CARSFX_EngineBase::UpdateParams(float) {}

void CARSFX_EngineBase::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
}

void CARSFX_EngineBase::InitSFX() {
    SndBase::InitSFX();
}

void CARSFX_EngineBase::Destroy() {}

void CARSFX_EngineBase::ProcessUpdate() {
    SetEngineParams();
}

void CARSFX_EngineBase::InitializeEngine() {}

void CARSFX_EngineBase::SetEngineParams() {}

int CARSFX_GinsuEngine::GetController(int Index) {
    switch (Index) {
    case 0:
        return 4;
    case 1:
        return 5;
    case 2:
        return 6;
    case 3:
        return 7;
    case 4:
        return 2;
    default:
        return -1;
    }
}

void CARSFX_GinsuEngine::AttachController(SFXCTL *psfxctl) {
    int nindex;

    nindex = psfxctl->GetObjectIndex();
    switch (nindex) {
    case 4:
        m_pEngineCtl = static_cast<SFXCTL_Engine *>(psfxctl);
        break;
    case 5:
        m_pHybridEngCtl = static_cast<SFXCTL_HybridMotor *>(psfxctl);
        break;
    case 6:
        m_pTunnelCtl = static_cast<SFXCTL_Tunnel *>(psfxctl);
        break;
    case 7:
        m_p3DCarPosCtl = static_cast<SFXCTL_3DCarPos *>(psfxctl);
        break;
    case 2:
        m_pShiftingCtl = static_cast<SFXCTL_Shifting *>(psfxctl);
        break;
    }
}

void CARSFX_GinsuEngine::SetupSFX(CSTATE_Base *_StateBase) {
    CARSFX_EngineBase::SetupSFX(_StateBase);
}

void CARSFX_GinsuEngine::InitSFX() {
    CARSFX_EngineBase::InitSFX();
    InitializeEngine();
    Enable();
}

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
        nDMixOut = GetDMixOutput(1, DMX_VOL);
        TmpVol = (m_pHybridEngCtl->m_EngVolAEMS * nDMixOut >> 15) * 0x7FFF >> 15;
        m_pcsisCarCtrl->SetVOL_ENG(TmpVol - 0x7FFF);
        m_pcsisCarCtrl->SetVOL_EXH(TmpVol - 0x7FFF);
        TmpRpmVol = m_pHybridEngCtl->m_EngVolRedLine * nDMixOut >> 15;
        m_pcsisCarCtrl->SetMAX_RPM(TmpRpmVol);
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
    m_pTranny->SetREVERB_AND_FILTERS_Wet(nFXWetVol);
    nFXDryVol = m_pTunnelCtl->m_AEMSDryVol;
    m_pTranny->SetREVERB_AND_FILTERS_Dry(nFXDryVol);
    if (m_pShiftingCtl && m_pShiftingCtl->IsActive()) {
        m_pTranny->SetSingle_Shot(1);
    } else {
        m_pTranny->SetSingle_Shot(0);
    }
    m_pTranny->CommitMemberData();
}
