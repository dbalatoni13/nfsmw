#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRPressStart.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"

uiQRPressStart::uiQRPressStart(ScreenConstructorData *sd) : MenuScreen(sd) {
    iPlayerNum = sd->Arg;
    param = 0;
    Setup();
}

uiQRPressStart::~uiQRPressStart() {}

void uiQRPressStart::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    switch (msg) {
        case 0xebfcda65: {
            int joyport = FEngMapJoyParamToJoyport(param1);
            if (iPlayerNum != 1 || joyport != FEDatabase->GetPlayersJoystickPort(0)) {
                FEDatabase->SetPlayersJoystickPort(iPlayerNum, static_cast<char>(joyport));
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
                cFEng::Get()->QueuePackageMessage(0x587c018b, GetPackageName(), nullptr);
            }
            break;
        }

        case 0x911ab364:
            if (iPlayerNum == 1) {
                uint32 control_mask = FEngMapJoyportToJoyParam(static_cast<int>(FEDatabase->GetPlayersJoystickPort(0)));
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

        case 0xe1fde1d1:
            cFEng::Get()->QueuePackageSwitch("Car_Select.fng", iPlayerNum, param, false);
            break;
    }
}

void uiQRPressStart::Setup() {
    const char *str = GetLocalizedString(0xcf538e1c);
    FEPrintf(GetPackageName(), 0xb244cf71, str, iPlayerNum + 1);
    str = GetLocalizedString(0xa065effe);
    FEPrintf(GetPackageName(), 0x545570c6, str);
}
