#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_TruckFX.hpp"
#include "Speed/Indep/Src/Misc/Config.h"

DEFINE_CREATABLE(0x20130, SFXObj_TruckFX, SndBase);

SFXObj_TruckFX::SFXObj_TruckFX() : CARSFX() {
    m_pTruckFX = nullptr;
    m_fSpeed = 0.0f;
    m_bStopped = true;
}

SFXObj_TruckFX::~SFXObj_TruckFX() {
    this->Destroy();
}

void SFXObj_TruckFX::Destroy() {
    delete this->m_pTruckFX;
    this->m_pTruckFX = nullptr;
}

void SFXObj_TruckFX::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
}

void SFXObj_TruckFX::UpdateParams(float t) {
    if (this->m_pTruckFX == nullptr) {
        if (this->GetPhysCar()->IsShifting()) {
            int rand_puff_ID = bRandom(4) + 2;

            g_pEAXSound->SetCsisName("Csis::Tractor air puff SFX");
            this->m_pTruckFX = new Csis::FX_TRUCK_FX(rand_puff_ID, 0, 0, this->GetDMixOutput(0, DMX_AZIM), 25000, 0, 0x7FFF, 0);

            if (this->m_pTruckFX == nullptr || this->m_pTruckFX->GetRefCount() < 3) {
                delete this->m_pTruckFX;
                this->m_pTruckFX = nullptr;
                return;
            }
        } else if ((this->GetPhysCar()->GetBrake() > 0.5f || this->GetPhysCar()->GetEBrake() > 0.0f) &&
                   this->GetPhysCar()->GetVelocityMagnitudeMPH() < 1.0f && this->m_fSpeed >= 1.0f) {
            if (!this->m_bStopped) {
                int rand_brake_ID = bRandom(2);

                g_pEAXSound->SetCsisName("Csis::Tractor brake SFX");
                this->m_pTruckFX = new Csis::FX_TRUCK_FX(rand_brake_ID, 0, 0, this->GetDMixOutput(0, DMX_AZIM), 25000, 0, 0x7FFF, 0);

                if (this->m_pTruckFX == nullptr || this->m_pTruckFX->GetRefCount() < 3) {
                    delete this->m_pTruckFX;
                    this->m_pTruckFX = nullptr;
                    return;
                }

                this->m_bStopped = true;
            }
        }
    }

    if (this->m_bStopped) {
        if (this->GetPhysCar()->GetVelocityMagnitudeMPH() > this->m_fSpeed) {
            this->m_bStopped = false;
        }
    }

    this->m_fSpeed = this->GetPhysCar()->GetVelocityMagnitudeMPH();
}

void SFXObj_TruckFX::ProcessUpdate() {
    if (this->m_pTruckFX != nullptr) {
        if (this->m_pTruckFX->GetRefCount() == 1) {
            delete this->m_pTruckFX;
            this->m_pTruckFX = nullptr;
        } else {
            this->m_pTruckFX->SetVolume(this->GetDMixOutput(1, DMX_VOL));
            this->m_pTruckFX->SetAzimuth(this->GetDMixOutput(0, DMX_AZIM));
            this->m_pTruckFX->SetPitch_OFFSET(this->GetDMixOutput(2, DMX_PITCH) - 0x1000);
            this->m_pTruckFX->CommitMemberData();
        }
    }
}

int SFXObj_TruckFX::GetController(int Index) {
    return -1;
}

void SFXObj_TruckFX::AttachController(SFXCTL *psfxctl) {}

void SFXObj_TruckFX::InitSFX() {
    if (IsSoundEnabled != 0) {
        SndBase::InitSFX();
    }
}

void SFXObj_TruckFX::Detach() {
    Destroy();
}

DEFINE_CREATABLE(0xc00e0, CARSFX_TruckWoosh, CARSFX_TrafficWoosh);

CARSFX_TruckWoosh::CARSFX_TruckWoosh()
    : CARSFX_TrafficWoosh(), //
      m_TrailerRef(0) {
    this->m_p3DTrailerPos = nullptr;
    this->m_vTrailerPos = bVector3(0.0f, 0.0f, 0.0f);
    this->m_vTrailerVel = bVector3(0.0f, 0.0f, 0.0f);
}

CARSFX_TruckWoosh::~CARSFX_TruckWoosh() {}

int CARSFX_TruckWoosh::GetController(int Index) {
    if (Index == 0) {
        return 0xF;
    }
    return -1;
}

void CARSFX_TruckWoosh::AttachController(SFXCTL *psfxctl) {
    if (psfxctl->GetObjectIndex() == 0xF) {
        this->m_p3DTrailerPos = static_cast<SFXCTL_3DCarPos *>(psfxctl);
    }
}

void CARSFX_TruckWoosh::InitSFX() {
    SndBase::InitSFX();
    this->m_p3DTrailerPos->AssignPositionVector(&this->m_vTrailerPos);
    this->m_p3DTrailerPos->AssignVelocityVector(&this->m_vTrailerVel);
}

bool CARSFX_TruckWoosh::IsPlayerCarInRadius() {
    return GetPlayerCarInRadius(this->m_vTrailerPos, 10.0f) != nullptr;
}

void CARSFX_TruckWoosh::UpdateParams(float t) {
    if (this->GetPhysCar()->GetContext() == Sound::CONTEXT_TRACTOR) {
        if (this->GetPhysCar()->mTrailerID != 0) {
            this->m_TrailerRef.Set(GetPhysCar()->mTrailerID);
            if (this->m_TrailerRef.IsValid()) {
                this->m_vTrailerPos = *reinterpret_cast<const bVector3 *>(&this->m_TrailerRef.GetMatrix()->v3);
                this->m_vTrailerVel = *this->GetPhysCar()->GetVelocity();
            }
        }
    }
    CARSFX_TrafficWoosh::UpdateParams(t);
}

eDRIVE_BY_TYPE CARSFX_TruckWoosh::GetWooshSample() {
    EAX_CarState *pcar = GetClosestPlayerCar(this->GetPhysCar()->GetPosition());

    if (pcar == nullptr) {
        return DRIVE_BY_TREE;
    }

    bVector3 normalvel = bNormalize(this->m_vTrailerVel);
    float dotprod = bAbs(bDot(*pcar->GetForwardVector(), normalvel));

    if (dotprod < 0.2f) {
        this->SetDMIX_Input(6, 0x7FFF);
        return this->GetPhysCar()->GetAttributes()->WooshType();
    }

    this->SetDMIX_Input(6, 0);
    return DRIVE_BY_AI_CAR;
}

DEFINE_CREATABLE(0xc00f0, SFXCTL_3DTrailerPos, SFXCTL_3DCarPos);
