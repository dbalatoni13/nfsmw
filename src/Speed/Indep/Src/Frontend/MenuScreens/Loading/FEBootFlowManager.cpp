#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FEBootFlowManager.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
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
        mInstance = new BootFlowManager();
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
                if (*sBootFlowWideScreen[i] != '\0') {
                    BootFlowScreen *screen = new BootFlowScreen();
                    screen->Name = sBootFlowWideScreen[i];
                    BootFlowScreens.AddTail(screen);
                }
            }
        } else {
            for (int i = 0; i < 7; i++) {
                if (*sBootFlowNTSC[i] != '\0') {
                    BootFlowScreen *screen = new BootFlowScreen();
                    screen->Name = sBootFlowNTSC[i];
                    BootFlowScreens.AddTail(screen);
                }
            }
        }
    } else {
        if (eIsWidescreen()) {
            for (int i = 0; i < 7; i++) {
                if (*sBootFlowPALWidescreen[i] != '\0') {
                    BootFlowScreen *screen = new BootFlowScreen();
                    screen->Name = sBootFlowPALWidescreen[i];
                    BootFlowScreens.AddTail(screen);
                }
            }
        } else {
            for (int i = 0; i < 7; i++) {
                if (*sBootFlowPAL[i] != '\0') {
                    BootFlowScreen *screen = new BootFlowScreen();
                    screen->Name = sBootFlowPAL[i];
                    BootFlowScreens.AddTail(screen);
                }
            }
        }
    }
    CurrentScreen = BootFlowScreens.GetHead();
}

BootFlowScreen *BootFlowManager::FindScreen(const char *name) {
    {
        BootFlowScreen *s = BootFlowScreens.GetHead();
        while (s != BootFlowScreens.EndOfList()) {
            if (bStrICmp(s->Name, name) == 0) {
                return s;
            }
            s = s->GetNext();
        }
    }
    return nullptr;
}

BootFlowScreen *BootFlowManager::FindScreenSubStr(const char *name) {
    {
        BootFlowScreen *s = BootFlowScreens.GetHead();
        while (s != BootFlowScreens.EndOfList()) {
            if (bStrStr(s->Name, name) != nullptr) {
                return s;
            }
            s = s->GetNext();
        }
    }
    return nullptr;
}

void BootFlowManager::JumpToHead() {
    CurrentScreen = BootFlowScreens.GetHead();
    JumpToScreen(CurrentScreen->Name);
}

bool BootFlowManager::JumpToScreen(const char *screen_name) {
    BootFlowScreen *screen = FindScreen(screen_name);
    if (screen == nullptr) {
        return false;
    }
    CurrentScreen = screen;
    cFEng::Get()->QueuePackagePop(0);
    cFEng::Get()->QueuePackagePush(CurrentScreen->Name, 0, 0, false);
    if (BootFlowScreens.GetTail() == CurrentScreen) {
        Destroy();
    }
    return true;
}

bool BootFlowManager::DoAttract() {
    BootFlowScreen *screen = FindScreenSubStr("Attract");
    if (screen == nullptr) {
        return false;
    }
    return JumpToScreen(screen->Name);
}

void BootFlowManager::ChangeToNextBootFlowScreen(int mask) {
    CurrentScreen = CurrentScreen->GetNext();
    cFEng::Get()->QueuePackageSwitch(CurrentScreen->Name, 0, mask, false);
    if (BootFlowScreens.GetTail() == CurrentScreen) {
        Destroy();
    }
}
