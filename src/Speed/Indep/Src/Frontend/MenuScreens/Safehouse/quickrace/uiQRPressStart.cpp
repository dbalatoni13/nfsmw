// OWNED BY zFeOverlay AGENT - DO NOT MODIFY OR EMPTY
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRPressStart.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"

extern int FEngMapJoyParamToJoyport(int param);
extern unsigned int FEngMapJoyportToJoyParam(int port);

uiQRPressStart::~uiQRPressStart() {}

uiQRPressStart::uiQRPressStart(ScreenConstructorData *sd) : MenuScreen(sd) {
    iPlayerNum = sd->Arg;
    param = 0;
    Setup();
}

void uiQRPressStart::Setup() {
    const char *str = GetLocalizedString(0xcf538e1c);
    FEPrintf(PackageFilename, 0xb244cf71, str, iPlayerNum + 1);
    str = GetLocalizedString(0xa065effe);
    FEPrintf(PackageFilename, 0x545570c6, str);
}

void uiQRPressStart::NotificationMessage(unsigned long msg, FEObject *obj, unsigned long param1, unsigned long param2) {
    if (msg == 0xe1fde1d1) {
        goto queue_car_select;
    }
    if (msg > 0xe1fde1d1) {
        goto handle_controller_change;
    }
    if (msg == 0x911ab364) {
        goto handle_back;
    }
    return;
handle_controller_change: {
        if (msg != 0xebfcda65) return;
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
            cFEng::Get()->QueuePackageMessage(0x587c018b, PackageFilename, nullptr);
        }
        return;
    }
handle_back: {
        if (iPlayerNum == 1) {
            unsigned int joyParam = FEngMapJoyportToJoyParam(static_cast<int>(FEDatabase->GetPlayersJoystickPort(0)));
            cFEng::Get()->QueuePackageSwitch("Car_Select.fng", 0, joyParam, false);
        } else {
            bool isSplitQR = false;
            if ((FEDatabase->GetGameMode() & 4) != 0) {
                isSplitQR = FEDatabase->iNumPlayers == 2;
            }
            if (isSplitQR && (FEDatabase->RaceMode == GRace::kRaceType_Drag || FEDatabase->RaceMode == GRace::kRaceType_P2P || FEDatabase->RaceMode == GRace::kRaceType_SpeedTrap)) {
                cFEng::Get()->QueuePackageSwitch("Track_Select.fng", 0, 0, false);
            } else {
                cFEng::Get()->QueuePackageSwitch("Track_Options.fng", 0, 0, false);
            }
        }
        return;
    }
queue_car_select:
    cFEng::Get()->QueuePackageSwitch("Car_Select.fng", iPlayerNum, param, false);
}
