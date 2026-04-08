#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_AICar.hpp"

#include "Speed/Indep/Src/EAXSound/EAXAemsManager.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"

int DEBUG_AI_CAR_CONNECTIONS = 0;
bool CSTATEMGR_AICar::bUsingGinsu = false;

CSTATEMGR_AICar::CSTATEMGR_AICar() {
    m_CarContext = Sound::CONTEXT_AIRACER;
    m_fConnectDistance = 135.0f;
}

CSTATEMGR_AICar::~CSTATEMGR_AICar() {}

void CSTATEMGR_AICar::UpdateParams(float t) {
    ProfileNode profile_node("TODO", 0);

    CSTATEMGR_CarState::UpdateParams(t);
    if (DEBUG_AI_CAR_CONNECTIONS != 0) {
        DebugDisplayAIConnections();
    }
}

void CSTATEMGR_AICar::DebugDisplayAIConnections() {}

void CSTATEMGR_AICar::EnterWorld(eSndGameMode esgm) {
    int SFXIDs = 0x91;
    int numopponents = 4;
    int n = UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::Count(VEHICLE_AIRACERS);

    n += UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::Count(VEHICLE_REMOTE);
    if (n >= 4) {
    } else {
        numopponents = n;
    }

    if (bUsingGinsu) {
        SFXIDs = 0x92;
    }

    for (int n = 0; n < numopponents; n++) {
        CSTATE_Base *NewAICar = CreateState(0, SFXIDs);
        NewAICar->Setup(SFXIDs);
        NewAICar->ForceCreateSFXCtrls(0xC0);
    }

    CSTATEMGR_Base::EnterWorld(esgm);
}

void CSTATEMGR_AICar::QueueSlots() {
    int numopponents;
    bool NoCops = FEDatabase->IsQuickRaceMode();
    GRaceParameters *race;

    bUsingGinsu = false;
    if (GRaceStatus::Exists()) {
        race = GRaceStatus::Get().GetRaceParameters();
    } else {
        race = GRaceDatabase::Get().GetStartupRace();
    }

    if (!race) {
        EAXAemsManager::QueueSlots(eBANK_SLOT_AI_AEMS_ENGINE, 4);
    } else {
        int engineslotsneeded =
            UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::Count(VEHICLE_AIRACERS) +
            UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::Count(VEHICLE_REMOTE);

        if (!NoCops) {
            engineslotsneeded++;
        }
        if (engineslotsneeded > 4) {
            engineslotsneeded = 4;
        }

        EAXAemsManager::QueueSlots(eBANK_SLOT_AI_AEMS_ENGINE, engineslotsneeded);
    }
}
