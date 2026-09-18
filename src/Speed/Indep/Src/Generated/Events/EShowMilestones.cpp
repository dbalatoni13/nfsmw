#include "EShowMilestones.hpp"

#include "Speed/Indep/Src/AI/AIBasics.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FEPkg_PostRace.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"

EShowMilestones::EShowMilestones(int pEnterSafehouseOnExit) : Event(0x10), fEnterSafehouseOnExit(pEnterSafehouseOnExit) {
    const char *screen = "PostRace_MilestoneRewards.fng";

    GPS_Disengage();

    ISimable *isimable = IPlayer::First(PLAYER_LOCAL)->GetSimable();

    if (isimable) {
        IVehicle *ivehicle;

        if (isimable->QueryInterface(&ivehicle)) {
            IPerpetrator *iperpvehicle;
            IVehicleAI *ivehicleai;

            ivehicle->QueryInterface(&iperpvehicle);
            ivehicleai = ivehicle->GetAIVehiclePtr();

            if (ivehicleai) {
                IPursuit *ipursuit = ivehicleai->GetPursuit();

                PostRacePursuitScreen::GetPursuitData().PopulateData(ipursuit, iperpvehicle, fEnterSafehouseOnExit);
            }
        }
    }

    cFEng::Get()->QueuePackagePush(screen, 0, 0, false);
}

EShowMilestones::~EShowMilestones() {
}

const char *EShowMilestones::GetEventName() const {
    return "EShowMilestones";
}

void EShowMilestones_MakeEvent_Callback(const void *staticData) {
    new EShowMilestones(((EShowMilestones::StaticData *) staticData)->fEnterSafehouseOnExit);
}
