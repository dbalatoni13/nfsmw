#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_PlayerCar.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"

bool CSTATEMGR_PlayerCar::IsTruck = false; // Decl: 14

CSTATEMGR_PlayerCar::CSTATEMGR_PlayerCar() {}

CSTATEMGR_PlayerCar::~CSTATEMGR_PlayerCar() {}

void CSTATEMGR_PlayerCar::EnterWorld(eSndGameMode esgm) {
    int SFXIDs = 0x53BFA;
    bool bIsTruck = false;
    const EAX_CarState::List &carlist = EAX_CarState::GetList();

    for (EAX_CarState *const *iter = carlist.begin(); iter != carlist.end(); ++iter) {
        if (bIsTruck) {
            break;
        }

        EAX_CarState *state = *iter;
        if (state->GetContext() != Sound::CONTEXT_PLAYER) {
            continue;
        }

        Attrib::Gen::pvehicle veh(state->GetAttributes()->GetCollection(), 0, nullptr);
        if (veh.TruckSndFX()) {
            bIsTruck = true;
        }
    }

    if (bIsTruck) {
        this->IsTruck = true;
        SFXIDs |= 0x80000;
    } else {
        this->IsTruck = false;
    }

    for (int n = 0; n < (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN ? 2 : 1); n++) {
        EAXCar *NewPlayerCar = static_cast<EAXCar *>(this->CreateState(0, SFXIDs));
        int SFXCTRLS = 0x100;

        NewPlayerCar->Setup(SFXIDs);
        NewPlayerCar->ForceCreateSFXCtrls(SFXCTRLS);
    }

    CSTATEMGR_Base::EnterWorld(esgm);
}

void CSTATEMGR_PlayerCar::UpdateParams(float t) {
    ProfileNode profile_node("TODO", 0);

    CSTATEMGR_Base::UpdateParams(t);
}

DEFINE_CREATABLE(0x20080, SFXCTL_3DRearPos, SFXCTL_3DCarPos);

void SFXCTL_3DRearPos::InitSFX() {
    SFXCTL::InitSFX();
    this->vRearPos.x = 0.0f;
    this->vRearPos.y = 0.0f;
    this->vRearPos.z = 0.0f;
    this->AssignPositionVector(&this->vRearPos);
    this->AssignDirectionVector(this->GetPhysCar()->GetForwardVector());
}

void SFXCTL_3DRearPos::UpdateParams(float t) {
    this->vRearPos = *this->GetPhysCar()->GetPosition();
    bVector3 vfwrd(*this->GetPhysCar()->GetForwardVector());
    float OffsetToUse = 2.0f;

    if (this->m_pEAXCar->GetPOV() != 0 && this->m_pEAXCar->GetPOV() != 1) {
        int id = bClamp(this->m_pStateBase->m_InstNum, 0, 1);
        bVector3 cpos = *SndCamera::GetCamPos(id);
        bVector3 tempRearPos = this->vRearPos;

        cpos.z = 0.0f;
        tempRearPos.z = 0.0f;

        float distbetween = bDistBetween(&tempRearPos, &cpos) - 1.0f;
        OffsetToUse = bMin(OffsetToUse, distbetween);
    }

    vfwrd = bScale(vfwrd, -OffsetToUse);
    this->vRearPos = bAdd(this->vRearPos, vfwrd);

    SFXCTL_3DObjPos::UpdateParams(t);
}
