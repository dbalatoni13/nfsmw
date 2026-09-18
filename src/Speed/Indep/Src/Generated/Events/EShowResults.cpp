#include "EShowResults.hpp"

#include "Speed/Indep/Src/AI/AIBasics.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

#include "Speed/Indep/Src/Generated/Messages/MEnterFreeRoam.h"
#include "Speed/Indep/Src/Generated/Messages/MFlowReadyForOutro.h"

EShowResults::EShowResults(FERESULTTYPE pResultType, bool pCalledWhileDriving) : Event(0x10), fResultType(pResultType), fCalledWhileDriving(pCalledWhileDriving) {
    if (FEManager::IsOkayToRequestPauseSimulation(0, true, false) || !fCalledWhileDriving) {
        const char *screen = NULL;
        int port;

        GPS_Disengage();

        if (fResultType == FERESULTTYPE_RACE) {
            screen = "PostRace_Results.fng";
        } else if (fResultType == FERESULTTYPE_PURSUIT) {
            screen = "PostRace_Pursuit.fng";
        }

        if (screen) {
            if (fResultType == FERESULTTYPE_PURSUIT) {
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

                            PostRacePursuitScreen::GetPursuitData().PopulateData(ipursuit, iperpvehicle, -1);
                        }
                    }
                }
            }

            port = FEngMapJoyportToJoyParam(FEDatabase->GetPlayersJoystickPort(0));

            bool splitscreen = FEDatabase->IsSplitScreenMode();

            if (Sim::IsSplitScreen() || splitscreen) {
                port |= FEngMapJoyportToJoyParam(FEDatabase->GetPlayersJoystickPort(1));
            }

            if (cFEng::Get()->IsPackageInControl("ControllerUnplugged.fng")) {
                cFEng::Get()->PopErrorPackage();
            }

            cFEng::Get()->QueuePackagePush(screen, 0, port, false);
        }
    }
}

EShowResults::~EShowResults() {
}

const char *EShowResults::GetEventName() const {
    return "EShowResults";
}

void EShowResults_MakeEvent_Callback(const void *staticData) {
    new EShowResults(((EShowResults::StaticData *) staticData)->fResultType, ((EShowResults::StaticData *) staticData)->fCalledWhileDriving);
}
