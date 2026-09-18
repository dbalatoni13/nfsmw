#include "ELoadingScreenOff.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalFlow.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOn.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"

ELoadingScreenOff::ELoadingScreenOff() : Event(0x10) {
    if (cFEng::Get()->IsPackageInControl(GetLoadingScreenPackageName())) {
        cFEng::Get()->QueuePackagePop(0);

        if (TheGameFlowManager.IsInGame()) {
            new EFadeScreenOn(false);
        }
    }
}

ELoadingScreenOff::~ELoadingScreenOff() {
    if (uiRepSheetRivalFlow::Get() && uiRepSheetRivalFlow::Get()->GetStage() == uiRepSheetRivalFlow::BACK_TO_FREE_ROAM) {
        uiRepSheetRivalFlow::Get()->Next();
    }
}

const char *ELoadingScreenOff::GetEventName() const {
    return "ELoadingScreenOff";
}

void ELoadingScreenOff_MakeEvent_Callback(const void *staticData) {
    new ELoadingScreenOff();
}
