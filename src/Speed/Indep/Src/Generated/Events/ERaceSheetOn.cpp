#include "ERaceSheetOn.hpp"

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FEPkg_PostRace.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetMain.hpp"

ERaceSheetOn::ERaceSheetOn(int pRivalFlow) : Event(0x10), fRivalFlow(pRivalFlow) {
    if (fRivalFlow != 2) {
        if (!FEManager::IsOkayToRequestPauseSimulation(0, true, false)) {
            return;
        }

        if (PostRacePursuitScreen::GetPursuitData().GetPursuitIsActive()) {
            return;
        }
    }

    iCurrentViewBin = FEDatabase->GetCareerSettings()->GetCurrentBin();

    int port = FEngMapJoyportToJoyParam(FEDatabase->GetPlayersJoystickPort(0));

    if (fRivalFlow > 0) {
        cFEng::Get()->QueuePackagePush("InGameRivalChallenge.fng", fRivalFlow, port, false);
    } else {
        cFEng::Get()->QueuePackagePush("InGameReputationOverview.fng", 1, port, false);
    }
}

ERaceSheetOn::~ERaceSheetOn() {
}

const char *ERaceSheetOn::GetEventName() const {
    return "ERaceSheetOn";
}

void ERaceSheetOn_MakeEvent_Callback(const void *staticData) {
    new ERaceSheetOn(((ERaceSheetOn::StaticData *) staticData)->fRivalFlow);
}
