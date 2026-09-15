#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FEBootFlowManager.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Misc/BuildRegion.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/FECarViewer.hpp"

const char *sBootFlowNTSC[] = {"MC_Bootup_GC.fng", "LS_EAlogo.fng",  "LS_PSA.fng",  "MW_LS_AttractFMV.fng",
                               "MW_LS_Splash.fng", "MC_Main_GC.fng", "MainMenu.fng"};
const char *sBootFlowPAL[] = {"MC_Bootup_GC.fng", "LS_EAlogo.fng",  "LS_PSA.fng",  "MW_LS_AttractFMV.fng",
                              "MW_LS_Splash.fng", "MC_Main_GC.fng", "MainMenu.fng"};
const char *sBootFlowWideScreen[] = {"MC_Bootup_GC.fng", "LS_EAlogo.fng",  "LS_PSA.fng",  "MW_LS_AttractFMV.fng",
                                     "MW_LS_Splash.fng", "MC_Main_GC.fng", "MainMenu.fng"};
const char *sBootFlowPALWidescreen[] = {"MC_Bootup_GC.fng", "LS_EAlogo.fng",  "LS_PSA.fng",  "MW_LS_AttractFMV.fng",
                                        "MW_LS_Splash.fng", "MC_Main_GC.fng", "MainMenu.fng"};

BootFlowManager *BootFlowManager::mInstance;

void BootFlowManager::Init() {
    if (mInstance == nullptr) {
        mInstance = new ("BootFlowManager", 0) BootFlowManager();
    }
}

void BootFlowManager::Destroy() {
    if (mInstance != nullptr) {
        delete mInstance;
        mInstance = nullptr;
        CarViewer::ShowAllCars();
    }
    g_pEAXSound->PlayFEMusic(-1);
}

BootFlowManager *BootFlowManager::Get() {
    return mInstance;
}

BootFlowManager::BootFlowManager() {
    if (!BuildRegion::IsPal()) {
        if (eIsWidescreen()) {
            for (int i = 0; i < 7; i++) {
                bool add_screen = true;

                if (bStrICmp(sBootFlowWideScreen[i], "WS_LS_EA_hidef.fng") == 0) {
                    // haha we don't do that here
                }

                if (add_screen && *sBootFlowWideScreen[i] != '\0') {
                    BootFlowScreens.AddTail(new ("BootFlowScreen", 0) BootFlowScreen(sBootFlowWideScreen[i]));
                }
            }
        } else {
            for (int i = 0; i < 7; i++) {
                if (*sBootFlowNTSC[i] != '\0') {
                    BootFlowScreens.AddTail(new ("BootFlowScreen", 0) BootFlowScreen(sBootFlowNTSC[i]));
                }
            }
        }
    } else {
        if (eIsWidescreen()) {
            for (int i = 0; i < 7; i++) {
                bool add_screen = true;

                if (bStrICmp(sBootFlowPALWidescreen[i], "LS_LangSelect.fng") == 0 && !BuildRegion::ShowLanguageSelect()) {
                    add_screen = false;
                }

                if (add_screen && *sBootFlowPALWidescreen[i] != '\0') {
                    BootFlowScreens.AddTail(new ("BootFlowScreen", 0) BootFlowScreen(sBootFlowPALWidescreen[i]));
                }
            }
        } else {
            for (int i = 0; i < 7; i++) {
                bool add_screen = true;

                if (bStrICmp(sBootFlowPAL[i], "LS_LangSelect.fng") == 0 && !BuildRegion::ShowLanguageSelect()) {
                    add_screen = false;
                }

                if (add_screen && *sBootFlowPAL[i] != '\0') {
                    BootFlowScreens.AddTail(new ("BootFlowScreen", 0) BootFlowScreen(sBootFlowPAL[i]));
                }
            }
        }
    }
    CurrentScreen = BootFlowScreens.GetHead();
    FEManager::Get()->SetFirstScreen(CurrentScreen->Name, 0, 0);
}

BootFlowScreen *BootFlowManager::FindScreen(const char *name) {
    for (BootFlowScreen *s = BootFlowScreens.GetHead(); s != BootFlowScreens.EndOfList(); s = s->GetNext()) {
        if (bStrICmp(s->Name, name) == 0) {
            return s;
        }
    }
    return nullptr;
}

BootFlowScreen *BootFlowManager::FindScreenSubStr(const char *name) {
    for (BootFlowScreen *s = BootFlowScreens.GetHead(); s != BootFlowScreens.EndOfList(); s = s->GetNext()) {
        if (bStrStr(s->Name, name) != nullptr) {
            return s;
        }
    }
    return nullptr;
}

void BootFlowManager::JumpToHead() {
    CurrentScreen = BootFlowScreens.GetHead();
    JumpToScreen(CurrentScreen->Name);
}

bool BootFlowManager::JumpToScreen(const char *screen_name) {
    BootFlowScreen *new_screen = FindScreen(screen_name);
    if (new_screen == nullptr) {
        return false;
    }
    CurrentScreen = new_screen;
    cFEng::Get()->QueuePackagePop(0);
    cFEng::Get()->QueuePackagePush(CurrentScreen->Name, 0, 0, false);
    if (BootFlowScreens.GetTail() == CurrentScreen) {
        Destroy();
    }
    return true;
}

bool BootFlowManager::DoAttract() {
    BootFlowScreen *new_screen = FindScreenSubStr("Attract");
    if (new_screen == nullptr) {
        return false;
    }
    return JumpToScreen(new_screen->Name);
}

void BootFlowManager::ChangeToNextBootFlowScreen(int mask) {
    CurrentScreen = CurrentScreen->GetNext();
    cFEng::Get()->QueuePackageSwitch(CurrentScreen->Name, 0, mask, false);
    if (BootFlowScreens.GetTail() == CurrentScreen) {
        Destroy();
    }
}
