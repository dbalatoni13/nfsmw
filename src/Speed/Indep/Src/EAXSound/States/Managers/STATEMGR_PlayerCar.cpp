#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_PlayerCar.hpp"

#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DCarPos.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

SndBase::TypeInfo SFXCTL_3DRearPos::s_TypeInfo = {
    0x00020080,
    "SFXCTL_3DRearPos",
    &SFXCTL_3DCarPos::s_TypeInfo,
    SFXCTL_3DRearPos::CreateObject,
};

CSTATEMGR_PlayerCar::CSTATEMGR_PlayerCar() {}

CSTATEMGR_PlayerCar::~CSTATEMGR_PlayerCar() {}

void CSTATEMGR_PlayerCar::EnterWorld(eSndGameMode esgm) {
    int SFXIDs = 0x53BFA;
    bool bIsTruck = false;
    typedef UTL::Collections::Listable<EAX_CarState, 10> CarList;
    const CarList::List &carlist = CarList::GetList();

    for (EAX_CarState *const *iter = carlist.begin(); iter != carlist.end(); ++iter) {
        if (bIsTruck) {
            break;
        }

        EAX_CarState *state = *iter;
        if (state->GetContext() == Sound::CONTEXT_PLAYER) {
            Attrib::Gen::pvehicle veh(state->GetAttributes()->GetCollection(), 0, nullptr);
            if (veh.TruckSndFX()) {
                bIsTruck = true;
            }
        }
    }

    if (bIsTruck) {
        IsTruck = true;
        SFXIDs = 0xD3BFA;
    } else {
        IsTruck = false;
    }

    for (int n = 0;; n++) {
        if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
            if (n > 1) {
                break;
            }
        } else if (n > 0) {
            break;
        }

        CSTATE_Base *NewPlayerCar = CreateState(0, SFXIDs);
        NewPlayerCar->Setup(SFXIDs);
        NewPlayerCar->ForceCreateSFXCtrls(0x100);
    }

    CSTATEMGR_Base::EnterWorld(esgm);
}

void CSTATEMGR_PlayerCar::UpdateParams(float t) {
    ProfileNode profile_node("TODO", 0);

    CSTATEMGR_Base::UpdateParams(t);
}

SndBase::TypeInfo *SFXCTL_3DRearPos::GetTypeInfo() const {
    return &s_TypeInfo;
}

bool CSTATEMGR_PlayerCar::IsTruck = false;

const char *SFXCTL_3DRearPos::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *SFXCTL_3DRearPos::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (s_TypeInfo.typeName, false) SFXCTL_3DRearPos();
    }
    return new (s_TypeInfo.typeName, true) SFXCTL_3DRearPos();
}

void SFXCTL_3DRearPos::InitSFX() {
    SFXCTL_3DCarPos::InitSFX();
}

void SFXCTL_3DRearPos::UpdateParams(float t) {
    bVector3 vfwrd;
    float OffsetToUse;

    vRearPos = *GetPhysCar()->GetPosition();
    vfwrd = *GetPhysCar()->GetForwardVector();
    OffsetToUse = 2.0f;
    if (m_pEAXCar->GetPOV() != 0 && m_pEAXCar->GetPOV() != 1) {
        int id;
        bVector3 cpos;
        bVector3 tempRearPos;
        float distbetween;

        id = bClamp(m_pStateBase->m_InstNum, 0, 1);
        cpos = *SndCamera::GetCamPos(id);
        tempRearPos = vRearPos;
        tempRearPos.z = 0.0f;
        cpos.z = 0.0f;
        distbetween = bDistBetween(&tempRearPos, &cpos);
        OffsetToUse = bMin(distbetween - 10.0f, 2.0f);
    }
    vRearPos = bAdd(vRearPos, bScale(vfwrd, -OffsetToUse));
    SFXCTL_3DObjPos::UpdateParams(t);
}
