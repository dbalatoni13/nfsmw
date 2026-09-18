#include "Speed/Indep/Src/EAXSound/EAXAemsManager.h"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_AICar.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"

bool DEBUG_AI_CAR_CONNECTIONS = false; // size: 0x1, address: 0x80417F34, Decl: 16

CSTATEMGR_AICar::CSTATEMGR_AICar() {
    this->m_CarContext = Sound::CONTEXT_AIRACER;
    this->m_fConnectDistance = 135.0f;
}

CSTATEMGR_AICar::~CSTATEMGR_AICar() {}

void CSTATEMGR_AICar::UpdateParams(float t) {
    ProfileNode profile_node;

    CSTATEMGR_CarState::UpdateParams(t);
    if (DEBUG_AI_CAR_CONNECTIONS) {
        this->DebugDisplayAIConnections();
    }
}

void CSTATEMGR_AICar::DebugDisplayAIConnections() {}

void CSTATEMGR_AICar::EnterWorld(eSndGameMode esgm) {
    int SFXIDs = 0x91;
#ifdef EA_BUILD_A124
    int numopponents = IVehicle::Count(VEHICLE_AIRACERS) < 5 ? IVehicle::Count(VEHICLE_AICOPS) : VEHICLE_AICOPS;
#else
    // Upstream escribe bMin(suma, 4) con su bMin (`a < b`); con el nuestro (`b < a`)
    // el mismo codigo sale con los argumentos al reves.
    int numopponents = bMin(4, IVehicle::Count(VEHICLE_AIRACERS) + IVehicle::Count(VEHICLE_REMOTE));
#endif
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

void CSTATEMGR_AICar::QueueSlots() {
    bUsingGinsu = false;

    bool NoCops = FEDatabase->IsQuickRaceMode();
    GRaceParameters *race;
    if (GRaceStatus::Exists()) {
        race = GRaceStatus::Get().GetRaceParameters();
    } else {
        race = GRaceDatabase::Get().GetStartupRace();
    }

    if (race == nullptr) {
        EAXAemsManager::QueueSlots(eBANK_SLOT_AI_AEMS_ENGINE, 4);
        return;
    }

    int numopponents = IVehicle::Count(VEHICLE_AIRACERS) + IVehicle::Count(VEHICLE_REMOTE);
    if (!NoCops) {
        numopponents += 1;
    }
    if (numopponents > 4) {
        numopponents = 4;
    }
    EAXAemsManager::QueueSlots(eBANK_SLOT_AI_AEMS_ENGINE, numopponents);
}
