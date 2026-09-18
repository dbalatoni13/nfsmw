#include "ERestartRace.hpp"

#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FEPkg_Hud.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOn.hpp"
#include "Speed/Indep/Src/Generated/Messages/MRestartRace.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IGameState.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ITrafficMgr.h"
#include "Speed/Indep/Src/Misc/Rumble.hpp"
#include "Speed/Indep/Src/World/VisualTreatment.h"
#include "Speed/Indep/Src/World/World.hpp"

void CleanParticlesOnRaceRestart();
void KillSkidsOnRaceRestart();

ERestartRace::ERestartRace() : Event(0x10) {
    if (GRaceStatus::Exists() && GRaceStatus::Get().GetActivelyRacing()) {
        if (!GRaceStatus::IsChallengeRace()) {
            g_pEAXSound->ReStartRace(true);
        } else {
            g_pEAXSound->ReStartRace(false);
        }
    } else {
        g_pEAXSound->ReStartRace(false);
    }

    GRaceStatus::Get().UpdateAdaptiveDifficulty(GRaceStatus::kAdaptiveGain_FromRestart, NULL);

    FEngHud::bIsRestartingRace = true;

    MRestartRace().Send(UCrc32(0x20d60dbf));

    new EFadeScreenOn(false);
}

ERestartRace::~ERestartRace() {
    World_RestoreProps();
    CleanParticlesOnRaceRestart();
    KillSkidsOnRaceRestart();

    if (IVisualTreatment::Get()) {
        IVisualTreatment::Get()->Reset();
    }

    if (IGameState::Exists()) {
        IGameState::Get()->RaceReset();
    }

    if (ITrafficMgr::Exists()) {
        ITrafficMgr::Get()->FlushAllTraffic(true);
    }

    ResetCameraShakers();
    CameraMoverRestartRace();

    TheICEManager.SetGenericCameraToPlay("", "");
}

const char *ERestartRace::GetEventName() const {
    return "ERestartRace";
}

void ERestartRace_MakeEvent_Callback(const void *staticData) {
    new ERestartRace();
}
