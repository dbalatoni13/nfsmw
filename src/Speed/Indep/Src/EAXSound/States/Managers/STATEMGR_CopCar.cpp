#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_CopCar.hpp"

CSTATEMGR_CopCar::CSTATEMGR_CopCar() {
    this->mNumCopsInProximity = 0;
    this->m_CarContext = Sound::CONTEXT_COP;
    this->m_fConnectDistance = 135.0f;
}

CSTATEMGR_CopCar::~CSTATEMGR_CopCar() {}

void CSTATEMGR_CopCar::UpdateParams(float t) {
    CSTATEMGR_CarState::UpdateParams(t);
    this->mNumCopsInProximity = GetAttachedStateCount();
}

void CSTATEMGR_CopCar::EnterWorld(eSndGameMode esgm) {
    int SFXIDs = 0x20081;
    int SFXCTRLS = 0xC0;

    for (int n = 0; n < 4; n++) {
        CSTATE_Base *NewAICar = this->CreateState(0, SFXIDs);
        NewAICar->Setup(SFXIDs);
        NewAICar->ForceCreateSFXCtrls(SFXCTRLS);
    }

    CSTATEMGR_Base::EnterWorld(esgm);
}
