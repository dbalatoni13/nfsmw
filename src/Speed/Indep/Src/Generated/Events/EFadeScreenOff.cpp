#include "EFadeScreenOff.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Generated/Events/ESndGameState.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"


EFadeScreenOff::EFadeScreenOff(int pDurationScript) : Event(0x10), fDurationScript(pDurationScript) {
    Main_SkipFrame(2);
}

EFadeScreenOff::~EFadeScreenOff() {
    if (cFEng::Get()->IsPackagePushed("FadeScreen.fng")) {
        if (FEngIsScriptSet("FadeScreen.fng", 0x027ff2dc, FEHASH_APPEARINVISIBLE) && fDurationScript == FEHASH_HIDE) {
            FEngSetScript("FadeScreen.fng", 0x027ff2dc, FEHASH_HIDE, true);
        } else if (fDurationScript == FEHASH_15_IN || fDurationScript == FEHASH_30_IN || fDurationScript == FEHASH_45_IN) {
            if (FEngIsScriptSet("FadeScreen.fng", 0x027ff2dc, FEHASH_APPEAR)) {
                FEngSetScript("FadeScreen.fng", 0x027ff2dc, fDurationScript, true);
            }
        }
    }

    if (TheGameFlowManager.IsInFrontend() && !cFEngJoyInput::Get()->IsJoyEnabled(JOYSTICK_PORT_ALL)) {
        cFEngJoyInput::Get()->JoyEnable(JOYSTICK_PORT_ALL, true);
    }

    new ESndGameState(11, false);
    SetSoundControlState(false, SNDSTATE_MINILOAD, "EFadeScreenOff");
}

const char *EFadeScreenOff::GetEventName() const {
    return "EFadeScreenOff";
}

void EFadeScreenOff_MakeEvent_Callback(const void *staticData) {
    new EFadeScreenOff(((EFadeScreenOff::StaticData *) staticData)->fDurationScript);
}
