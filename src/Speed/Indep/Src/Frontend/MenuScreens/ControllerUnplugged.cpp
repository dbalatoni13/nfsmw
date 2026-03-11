#include "ControllerUnplugged.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FEBootFlowManager.hpp"
#include "Speed/Indep/Src/Generated/Events/EPause.hpp"

const char* GetLocalizedString(unsigned int hash);
void FEPrintf(const char* pkg_name, unsigned int hash, const char* format, ...);
int FEngMapJoyParamToJoyport(int feng_param);
int GetPortsPlayer(int port);

ControllerUnplugged::ControllerUnplugged(ScreenConstructorData* sd)
    : MenuScreen(sd) //
    , port(static_cast<JoystickPort>(sd->Arg))
{
    Setup();
}

ControllerUnplugged::~ControllerUnplugged() {}

void ControllerUnplugged::Setup() {
    const char* pkg_name = GetPackageName();
    const char* text = GetLocalizedString(0x54EEF4C5);
    FEPrintf(pkg_name, 0xB244CF71, text, port + 1);
}

void ControllerUnplugged::NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1, unsigned long param2) {
    if (msg == 0xEBFCDA65) {
        if (port == -1) {
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
