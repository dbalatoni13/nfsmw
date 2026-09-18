#include "ControllerUnplugged.hpp"

#include "Speed/Indep/Src/Frontend/Event.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FEBootFlowManager.hpp"

ControllerUnplugged::ControllerUnplugged(ScreenConstructorData *sd) : MenuScreen(sd) {
    port = (JoystickPort)sd->Arg;
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
    FEPrintf(GetPackageName(), 0xB244CF71, GetLocalizedString(0x54EEF4C5), port + 1);
}
