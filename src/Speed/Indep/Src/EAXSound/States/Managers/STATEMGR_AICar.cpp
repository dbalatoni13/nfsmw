#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_AICar.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"

bool DEBUG_AI_CAR_CONNECTIONS = false; // size: 0x1, address: 0x80417F34, Decl: 16

CSTATEMGR_AICar::CSTATEMGR_AICar() {
    this->m_CarContext = Sound::CONTEXT_AIRACER;
    this->m_fConnectDistance = 135.0f;
}

CSTATEMGR_AICar::~CSTATEMGR_AICar() {}

void CSTATEMGR_AICar::UpdateParams(float t) {
    ProfileNode profile_node("TODO", 0);

    CSTATEMGR_CarState::UpdateParams(t);
    if (DEBUG_AI_CAR_CONNECTIONS) {
        this->DebugDisplayAIConnections();
    }
}

void CSTATEMGR_AICar::DebugDisplayAIConnections() {}

void CSTATEMGR_AICar::EnterWorld(eSndGameMode esgm) {
    int SFXIDs = 0x91;
    int numopponents = bMin(4, IVehicle::Count(VEHICLE_AIRACERS) + IVehicle::Count(VEHICLE_REMOTE));
    int SFXCTRLS = 0xC0;

    if (bUsingGinsu) {
        SFXIDs = 0x92;
    }

    for (int n = 0; n < numopponents; n++) {
        EAXCar *NewAICar = static_cast<EAXCar *>(this->CreateState(0, SFXIDs));
        NewAICar->Setup(SFXIDs);
        NewAICar->ForceCreateSFXCtrls(SFXCTRLS);
    }

    CSTATEMGR_Base::EnterWorld(esgm);
}

bool CSTATEMGR_AICar::bUsingGinsu = false; // Decl: 118

// TODO function here after FE is merged
