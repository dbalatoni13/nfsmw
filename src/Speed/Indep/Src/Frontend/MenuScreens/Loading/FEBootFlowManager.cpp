#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FEBootFlowManager.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"

extern bool BuildRegion_IsPal();
extern char *bStrStr(const char *s1, const char *s2);

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
        CarViewer::ShowAllCars();
    }
    g_pEAXSound->PlayFEMusic(-1);
}

BootFlowManager::~BootFlowManager() {}

BootFlowManager::BootFlowManager() {
    if (!BuildRegion_IsPal()) {
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
