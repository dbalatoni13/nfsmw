#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRPressStart.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"

uiQRPressStart::uiQRPressStart(ScreenConstructorData *sd) : MenuScreen(sd), iPlayerNum(sd->Arg), param(0) {
    Setup();
}

uiQRPressStart::~uiQRPressStart() {}

void uiQRPressStart::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    switch (msg) {
        case __PAD_START_RELEASED__: {
            int joyPort = FEngMapJoyParamToJoyport(param1);
            const u32 FEObj_leavescreen = 0x587c018b;
            if (iPlayerNum != 1 || joyPort != FEDatabase->GetPlayersJoystickPort(0)) {
                FEDatabase->SetPlayersJoystickPort(iPlayerNum, joyPort);
                this->param = param1;
                if ((static_cast<unsigned int>(this->param) & 1) != 0) {
                    this->param = 1;
                }
                if ((static_cast<unsigned int>(this->param) & 2) != 0) {
                    this->param = 2;
                }
                if ((static_cast<unsigned int>(this->param) & 4) != 0) {
                    this->param = 4;
                }
                if ((static_cast<unsigned int>(this->param) & 8) != 0) {
                    this->param = 8;
                }
                FEManager::Get()->AllowControllerError(true);
                cFEng::Get()->QueuePackageMessage(FEObj_leavescreen, GetPackageName(), nullptr);
            }
            break;
        }

        case __PAD_BACK__:
            if (iPlayerNum == 1) {
                int control_mask = FEngMapJoyportToJoyParam(static_cast<int>(FEDatabase->GetPlayersJoystickPort(0)));
                cFEng::Get()->QueuePackageSwitch("Car_Select.fng", 0, control_mask, false);
            } else {
                if (FEDatabase->IsSplitScreenMode() &&
                    (FEDatabase->RaceMode == GRace::kRaceType_Drag || FEDatabase->RaceMode == GRace::kRaceType_P2P ||
                     FEDatabase->RaceMode == GRace::kRaceType_SpeedTrap)) {
                    cFEng::Get()->QueuePackageSwitch("Track_Select.fng", 0, 0, false);
                } else {
                    cFEng::Get()->QueuePackageSwitch("Track_Options.fng", 0, 0, false);
                }
            }
            break;

        case FEHASH_EXITCOMPLETE:
            cFEng::Get()->QueuePackageSwitch("Car_Select.fng", iPlayerNum, param, false);
            break;
    }
}

void uiQRPressStart::Setup() {
    FEPrintf(GetPackageName(), 0xb244cf71, GetLocalizedString(0xcf538e1c), iPlayerNum + 1);
    FEPrintf(GetPackageName(), STRINGHASH_FONT_MW_BODY, GetLocalizedString(0xa065effe));
}
