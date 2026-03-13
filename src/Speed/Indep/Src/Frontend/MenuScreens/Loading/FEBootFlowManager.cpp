#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FEBootFlowManager.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"

extern bool BuildRegion_IsPal();
extern bool eIsWidescreen();
extern char *bStrStr(const char *s1, const char *s2);
extern void ShowAllCars();
extern void PlayFEMusic(int);

extern const char *sBootFlowNTSC[];
extern const char *sBootFlowPAL[];
extern const char *sBootFlowWideScreen[];
extern const char *sBootFlowPALWidescreen[];

BootFlowManager *BootFlowManager::mInstance;

BootFlowManager *BootFlowManager::Get() {
    return mInstance;
}

void BootFlowManager::JumpToHead() {
    CurrentScreen = BootFlowScreens.GetHead();
    JumpToScreen(CurrentScreen->Name);
}

void BootFlowManager::Init() {
    if (mInstance == nullptr) {
        mInstance = new BootFlowManager();
    }
}

void BootFlowManager::Destroy() {
    if (mInstance != nullptr) {
        delete mInstance;
        mInstance = nullptr;
        ShowAllCars();
    }
    PlayFEMusic(-1);
}

BootFlowManager::BootFlowManager() {
    const char **bootFlow;
    if (BuildRegion_IsPal()) {
        if (eIsWidescreen()) {
            bootFlow = sBootFlowPALWidescreen;
        } else {
            bootFlow = sBootFlowPAL;
        }
    } else {
        if (eIsWidescreen()) {
            bootFlow = sBootFlowWideScreen;
        } else {
            bootFlow = sBootFlowNTSC;
        }
    }
    for (int i = 0; i < 7; i++) {
        if (*bootFlow[i] != '\0') {
            BootFlowScreen *screen = new BootFlowScreen();
            screen->Name = bootFlow[i];
            BootFlowScreens.AddTail(screen);
        }
    }
    CurrentScreen = BootFlowScreens.GetHead();
}

BootFlowScreen *BootFlowManager::FindScreen(const char *name) {
    BootFlowScreen *screen = BootFlowScreens.GetHead();
    while (screen != BootFlowScreens.EndOfList()) {
        if (bStrICmp(screen->Name, name) == 0) {
            return screen;
        }
        screen = screen->GetNext();
    }
    return nullptr;
}

BootFlowScreen *BootFlowManager::FindScreenSubStr(const char *name) {
    BootFlowScreen *screen = BootFlowScreens.GetHead();
    while (screen != BootFlowScreens.EndOfList()) {
        if (bStrStr(screen->Name, name) != nullptr) {
            return screen;
        }
        screen = screen->GetNext();
    }
    return nullptr;
}

bool BootFlowManager::JumpToScreen(const char *screen_name) {
    BootFlowScreen *screen = FindScreen(screen_name);
    if (screen == nullptr) {
        return false;
    }
    CurrentScreen = screen;
    cFEng::mInstance->QueuePackagePop(0);
    cFEng::mInstance->QueuePackagePush(CurrentScreen->Name, 0, 0, false);
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
    cFEng::mInstance->QueuePackageSwitch(CurrentScreen->Name, 0, mask, false);
    if (BootFlowScreens.GetTail() == CurrentScreen) {
        Destroy();
    }
}