#include "ControllerUnplugged.hpp"

#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FEBootFlowManager.hpp"
#include "Speed/Indep/Src/Generated/Events/EPause.hpp"

ControllerUnplugged::ControllerUnplugged(ScreenConstructorData *sd) : MenuScreen(sd), port(static_cast<JoystickPort>(sd->Arg)) {
    Setup();
}

ControllerUnplugged::~ControllerUnplugged() {}

void ControllerUnplugged::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    if (msg == 0xEBFCDA65) {
        if (port == JOYSTICK_PORT_NONE) {
            BootFlowManager::Get()->JumpToHead();
        } else {
            int joyPort = FEngMapJoyParamToJoyport(param1);
            cFEng::Get()->PopErrorPackage(joyPort);
            if (FEManager::IsOkayToRequestPauseSimulation(GetPortsPlayer(joyPort), false, false)) {
                new EPause(GetPortsPlayer(joyPort), 0, 0);
            }
        }
    }
}

void ControllerUnplugged::Setup() {
    const char *pkg_name = GetPackageName();
    const char *text = GetLocalizedString(0x54EEF4C5);
    FEPrintf(pkg_name, 0xB244CF71, text, port + 1);
}
