#include "EFadeScreenOn.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Generated/Events/ESndGameState.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"

EFadeScreenOn::EFadeScreenOn(bool pLoadingBarOnly) : Event(0x10), fLoadingBarOnly(pLoadingBarOnly) {
    Main_SkipFrame(2);

    if (!cFEng::Get()->IsPackagePushed("FadeScreen.fng")) {
        cFEng::Get()->PushNoControlPackage("FadeScreen.fng", FE_PACKAGE_PRIORITY_SECOND_CLOSEST);
    }

    if (TheGameFlowManager.IsInFrontend()) {
        if (cFEngJoyInput::Get()->IsJoyEnabled(JOYSTICK_PORT_ALL)) {
            cFEngJoyInput::Get()->JoyDisable(JOYSTICK_PORT_ALL, true);
        }
    }

    FEManager::Get()->SuppressControllerError(true);

    if (!FEngIsScriptSet("FadeScreen.fng", 0x027FF2DC, FEHASH_APPEAR)) {
        if (fLoadingBarOnly) {
            FEngSetScript("FadeScreen.fng", 0x027FF2DC, FEHASH_APPEARINVISIBLE, true);
        } else {
            FEngSetScript("FadeScreen.fng", 0x027FF2DC, FEHASH_APPEAR, true);
        }
    }

    new ESndGameState(11, true);
    SetSoundControlState(true, SNDSTATE_MINILOAD, "EFadeScreenOn");
}

EFadeScreenOn::~EFadeScreenOn() {
}

const char *EFadeScreenOn::GetEventName() const {
    return "EFadeScreenOn";
}

void EFadeScreenOn_MakeEvent_Callback(const void *staticData) {
    new EFadeScreenOn(((EFadeScreenOn::StaticData *) staticData)->fLoadingBarOnly);
}
