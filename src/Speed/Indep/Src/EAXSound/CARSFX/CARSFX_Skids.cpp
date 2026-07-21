#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Skids.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/OldSoundTemplates.hpp"
#include "Speed/Indep/Src/EAXSound/SndBase.hpp"
#include "Speed/Indep/Src/World/RaceParameters.hpp"

DEFINE_CREATABLE(0x20070, CARSFX_Skids, SndBase);

CARSFX_Skids::CARSFX_Skids() : CARSFX() {
    this->m_pSkidControl = nullptr;
    this->m_SkidAzimuth[0] = 0;
    this->m_SkidAzimuth[1] = 0;
}

CARSFX_Skids::~CARSFX_Skids() {
    this->Destroy();
}

void CARSFX_Skids::Destroy() {
    if (this->m_pSkidControl != nullptr) {
        delete this->m_pSkidControl;
        this->m_pSkidControl = nullptr;
    }
    this->Disable();
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
    switch (psfxctl->GetObjectIndex()) {
        case 1:
            this->m_pWheelCtl = static_cast<SFXCTL_Wheel *>(psfxctl);
            break;
        case 11:
            this->m_pRightWheelPos = static_cast<SFXCTL_3DRightWheelPos *>(psfxctl);
            this->m_pRightWheelPos->AssignPositionVector(this->m_pWheelCtl->GetWheelPos(1, 2));
            this->m_pRightWheelPos->AssignVelocityVector(nullptr);
            break;
        case 12:
            this->m_pLeftWheelPos = static_cast<SFXCTL_3DLeftWheelPos *>(psfxctl);
            this->m_pLeftWheelPos->AssignPositionVector(this->m_pWheelCtl->GetWheelPos(0, 2));
            this->m_pLeftWheelPos->AssignVelocityVector(nullptr);
            break;
        default:
            break;
    }
}

void CARSFX_Skids::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
}

// UNSOLVED
void CARSFX_Skids::InitSFX() {
    if (this->GetPhysCar() == nullptr) {
        return;
    }

    SndBase::InitSFX();
    this->SkidType = TheRaceParameters.IsDriftRace() ? 1 : 0;
    delete this->m_pSkidControl;

    g_pEAXSound->SetCsisName(this);
    int numskids;
    this->m_pSkidControl = new Csis::FX_SKID(0, 0, 0, 0, this->SkidType, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25000, 0, 0x7FFF, 0);
    this->m_pSkidControl->GetRefCount();
    this->Enable();
}

void CARSFX_Skids::Detach() {
    delete this->m_pSkidControl;
    this->m_pSkidControl = nullptr;
}

void CARSFX_Skids::UpdateMixerOutputs() {
    if (this->m_pSkidControl == nullptr) {
        return;
    }

    int fwdval = bMax(bAbs(this->m_pSkidControl->GetFront_FB()), bAbs(this->m_pSkidControl->GetBack_FB()));
    fwdval = bClamp(fwdval << 5, 0, 0x7FFF);

    this->SetDMIX_Input(0, fwdval);
    this->SetDMIX_Input(2, fwdval);

    int sideval = bMax(bAbs(this->m_pSkidControl->GetFront_LR()), bAbs(this->m_pSkidControl->GetBack_LR()));
    sideval = bClamp(sideval << 5, 0, 0x7FFF);

    this->SetDMIX_Input(1, smooth(this->GetDMIX_InputValue(1), sideval, 500));

    int loadval = bMax(fwdval, sideval);

    this->SetDMIX_Input(3, smooth(this->GetDMIX_InputValue(3), loadval, 3000));
}

// UNSOLVED regswap
void CARSFX_Skids::ProcessUpdate() {
    if (!this->IsEnabled() || (this->m_pSkidControl == nullptr)) {
        return;
    }

    this->m_pSkidControl->SetVOL_Fwd(this->GetDMixOutput(5, DMX_VOL));
    this->m_pSkidControl->SetVOL_Side(this->GetDMixOutput(7, DMX_VOL));
    this->m_pSkidControl->SetVOL_Back(this->GetDMixOutput(6, DMX_VOL));
    this->m_pSkidControl->SetFilter_Effects_Wet_FX(this->GetDMixOutput(9, DMX_VOL));
    this->m_pSkidControl->SetPITCH_OFFSET(this->GetDMixOutput(8, DMX_PITCH));
    this->m_pSkidControl->SetAzimuth(this->GetDMixOutput(1, DMX_AZIM));

    int skid_type = UMath::Max(this->m_pWheelCtl->LeftSideTerrain.Aud_Skid_Type(), this->m_pWheelCtl->RightSideTerrain.Aud_Skid_Type());
    this->m_pSkidControl->SetSurface(skid_type);

    float RadYaw = bAbs(this->GetPhysCar()->GetState()->GetYaw());
    float DegYaw = bRadToDeg(RadYaw);
    float Speed = this->GetPhysCar()->GetVelocityMagnitudeMPH();

    this->m_pSkidControl->SetSPEED(static_cast<int>(Speed));
    this->m_pSkidControl->SetUNDERSTEER(static_cast<int>(this->GetPhysCar()->CalculateUndersteerFactor()));
    this->m_pSkidControl->SetOVERSTEER(static_cast<int>(this->GetPhysCar()->CalculateOversteerFactor()));

    if (this->m_pWheelCtl->RightSideTouchingGround || this->m_pWheelCtl->LeftSideTouchingGround) {
        this->m_pSkidControl->SetFront_FB(
            static_cast<int>((this->m_pWheelCtl->m_NormWheelSlip[1].x + this->m_pWheelCtl->m_NormWheelSlip[0].x) * 0.5f));

        this->m_pSkidControl->SetFront_LR(
            static_cast<int>((this->m_pWheelCtl->m_NormWheelSlip[1].y + this->m_pWheelCtl->m_NormWheelSlip[0].y) * 0.5f));

        this->m_pSkidControl->SetFront_Load(static_cast<int>((this->m_pWheelCtl->m_fLoad[1] + this->m_pWheelCtl->m_fLoad[0]) * 0.5f));

        this->m_pSkidControl->SetBack_FB(
            static_cast<int>((this->m_pWheelCtl->m_NormWheelSlip[3].x + this->m_pWheelCtl->m_NormWheelSlip[2].x) * 0.5f));

        this->m_pSkidControl->SetBack_LR(
            static_cast<int>((this->m_pWheelCtl->m_NormWheelSlip[3].y + this->m_pWheelCtl->m_NormWheelSlip[2].y) * 0.5f));

        this->m_pSkidControl->SetBack_Load(static_cast<int>((this->m_pWheelCtl->m_fLoad[3] + this->m_pWheelCtl->m_fLoad[2]) * 0.5f));
    } else {
        this->m_pSkidControl->SetFront_FB(0);
        this->m_pSkidControl->SetFront_LR(0);
        this->m_pSkidControl->SetFront_Load(0);
        this->m_pSkidControl->SetBack_FB(0);
        this->m_pSkidControl->SetBack_LR(0);
        this->m_pSkidControl->SetBack_Load(0);
    }

    this->m_pSkidControl->CommitMemberData();
}

DEFINE_CREATABLE(0x200b0, SFXCTL_3DRightWheelPos, SndBase);
DEFINE_CREATABLE(0x200c0, SFXCTL_3DLeftWheelPos, SndBase);

DEFINE_CREATABLE(0x50030, CARSFX_TrafficSkids, SndBase);

CARSFX_TrafficSkids::CARSFX_TrafficSkids() {}

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
