#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingScreen.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Misc/BuildRegion.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Generated/Events/ESndGameState.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

extern void FEngSetScript(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash, bool start_at_beginning);
extern FEObject *FEngFindObject(const char *pkg_name, unsigned int hash);
extern void FEngSetVisible(FEObject *obj);
extern bool eIsWidescreen();
extern void SetSoundControlState(bool bON, eSNDCTLSTATE esndstate, const char *Reason);

static bool bSawLoadingScreen;

void *LoadingScreen::mLoadingScreenPtr;

LoadingScreen::LoadingScreen(ScreenConstructorData *sd) : MenuScreen(sd) {
    if (FEManager::Get()->IsFirstBoot()) {
        if (BuildRegion::ShowLanguageSelect()) {
            FEngSetScript(GetPackageName(), 0xcf281d29, 0x5d7c6a21, true);
        }
    }

    bSawLoadingScreen = true;

    FEngSetVisible(FEngFindObject(GetPackageName(), 0x06d91704));

    if (eIsWidescreen()) {
        cFEng::mInstance->QueuePackageMessage(bStringHash("CURRENT_GEN_WIDESCREEN"), GetPackageName(), nullptr);
    }

    new ESndGameState(10, true);
    SetSoundControlState(true, SNDSTATE_OFF, "FELoad");
}

void LoadingScreen::NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) {}

void LoadingScreen::InitLoadingScreen() {
    mLoadingScreenPtr = bMalloc(0x2C, 0);
}

MenuScreen *CreateLoadingScreen(ScreenConstructorData *sd) {
    return new (LoadingScreen::mLoadingScreenPtr) LoadingScreen(sd);
}