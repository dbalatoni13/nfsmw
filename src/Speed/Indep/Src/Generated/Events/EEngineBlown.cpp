#include "Speed/Indep/Src/Generated/Events/EShowRaceOverMessage.hpp"
#include "EEngineBlown.hpp"

#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyEngineBlown.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

#include "Speed/Indep/Src/Generated/Messages/MEnterRaceOverFlow.h"

EEngineBlown::EEngineBlown(HSIMABLE phSimable) : Event(0x10), fhSimable(phSimable) {
}

EEngineBlown::~EEngineBlown() {
    ISimable *isimable = ISimable::FindInstance(fhSimable);

    if (isimable) {
        GRacerInfo *racerInfo = GRaceStatus::Get().GetRacerInfo(isimable);
        IPlayer *player;
        IInputPlayer *input;

        if (racerInfo) {
            racerInfo->BlowEngine();
        }

        // TODO hash
        MNotifyEngineBlown(isimable->GetInstanceHandle()).Post(UCrc32(0x20d60dbf));

        player = isimable->GetPlayer();

        if (player) {
            if (player->InGameBreaker()) {
                player->ToggleGameBreaker();
            }
        }

        if (isimable->QueryInterface(&input)) {
            input->BlockInput(true);
        }

        GRaceStatus::Get().UpdateAdaptiveDifficulty(GRaceStatus::kAdaptiveGain_FromEngineBlown, isimable);

        if (Sim::IsSplitScreen()) {
            if (isimable->IsPlayer()) {
                new EShowRaceOverMessage(isimable->GetPlayer());
            }
        }
    }
}

const char *EEngineBlown::GetEventName() const {
    return "EEngineBlown";
}

void EEngineBlown_MakeEvent_Callback(const void *staticData) {
    new EEngineBlown(((EEngineBlown::StaticData *) staticData)->fhSimable);
}
