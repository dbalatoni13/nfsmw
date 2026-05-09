#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingScreen.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Misc/BuildRegion.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Generated/Events/ESndGameState.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

static bool bSawLoadingScreen;

void *LoadingScreen::mLoadingScreenPtr;

LoadingScreen::LoadingScreen(ScreenConstructorData *sd) : MenuScreen(sd) {
    const u32 FEObj_LoadingBlinker = 0xCF281D29;

    if (FEManager::Get()->IsFirstBoot()) {
        if (BuildRegion::ShowLanguageSelect()) {
            FEngSetScript(GetPackageName(), FEObj_LoadingBlinker, 0x5D7C6A21, true);
        }
    }

    bSawLoadingScreen = true;

    {
        const unsigned long FEObj_LOADINGGROUP = 0x06D91704;

        FEngSetVisible(FEngFindObject(GetPackageName(), FEObj_LOADINGGROUP));
    }

    if (eIsWidescreen()) {
        cFEng::Get()->QueuePackageMessage(bStringHash("CURRENT_GEN_WIDESCREEN"), GetPackageName(), nullptr);
    }

    new ESndGameState(10, true);
    SetSoundControlState(true, SNDSTATE_OFF, "FELoad");
}
LoadingScreen::~LoadingScreen() {
    if (cFEng::Get()->IsPackageInControl("Loading_Tips.fng")) {
        cFEng::Get()->QueuePackagePop(1);
    }

    new ESndGameState(10, false);
    SetSoundControlState(false, SNDSTATE_OFF, "FELoad");
}

void LoadingScreen::InitLoadingScreen() {
    mLoadingScreenPtr = bMalloc(sizeof(LoadingScreen), nullptr, 0, 0);
}
