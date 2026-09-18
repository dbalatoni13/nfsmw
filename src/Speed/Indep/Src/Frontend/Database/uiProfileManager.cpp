#include "uiProfileManager.hpp"

#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"

void FEngSetColor(FEObject* obj, unsigned int color);
unsigned char FEngGetLastButton(const char* pkg_name);
// [tr] const char* GetLocalizedString(unsigned int hash);

MenuScreen* CreateUIProfileManager(ScreenConstructorData* sd) {
    return new ("CreateUIProfileManager", 0) UIProfileManager(sd);
}

UIProfileManager::UIProfileManager(ScreenConstructorData* sd)
    : IconScrollerMenu(sd) {
    Setup();
    FEPrintf(GetPackageName(), 0x42ADB44C, GetLocalizedString(0xBCB18F38));
}

void UIProfileManager::Refresh() {
    if (FEDatabase->bProfileLoaded) {
        mpSave->IsGreyOut = false;
    } else {
        mpSave->IsGreyOut = true;
    }

    if (FEDatabase->bProfileLoaded) {
        FEngSetVisible(GetPackageName(), FEHashUpper("NAME_GROUP"));
        FEPrintf(GetPackageName(), 0xEB406FEC, FEDatabase->GetUserProfile(0)->GetProfileName());
    } else {
        FEngSetInvisible(GetPackageName(), FEHashUpper("NAME_GROUP"));
    }

    FEngSetColor(mpSave->FEngObject, mpSave->OriginalColor);
    RefreshHeader();
}

void UIProfileManager::NotificationMessage(u32 msg, FEObject* obj, u32 param1,
                                           u32 param2) {
    IconScrollerMenu::NotificationMessage(msg, obj, param1, param2);

    switch (msg) {
    case 0x911AB364:
        cFEng::Get()->QueuePackageSwitch("MainMenu.fng", 0, 0, false);
        break;
    case 0x35F8620B:
        Refresh();
        break;
    case 0x7E998E5E:
        FEDatabase->RefreshCurrentRide();
        Refresh();
        break;
    }
}

void UIProfileManager::Setup() {
    mpSave = new ("PMSave", 0) PMSave(0x228B7E32, 0x1C8ACE, 0);
    mpSave->SetReactImmediately(true);

    PMCreateNew* createNew = new ("PMCreateNew", 0) PMCreateNew(0x43798644, 0x55423473, 0);
    createNew->SetReactImmediately(true);
    AddOption(createNew);

    PMLoad* load = new ("PMLoad", 0) PMLoad(0x2287E063, 0x18ECFF, 0);
    load->SetReactImmediately(true);
    AddOption(load);

    AddOption(mpSave);

    PMDelete* del = new ("PMDelete", 0) PMDelete(0x0D9035CE, 0x56B00632, 0);
    del->SetReactImmediately(true);
    AddOption(del);

    int lastButton = FEngGetLastButton(GetPackageName());
    if (bFadeInIconsImmediately) {
        Options.bDelayUpdate = false;
        Options.bFadingOut = false;
        Options.StartFadeIn();
    }
    Options.SetInitialPos(lastButton);

    Refresh();
}

MenuScreen* CreateUIDeleteProfile(ScreenConstructorData* sd) {
    return new ("CreateUIDeleteProfile", 0) UIDeleteProfile(sd);
}

UIDeleteProfile::UIDeleteProfile(ScreenConstructorData* sd)
    : IconScrollerMenu(sd) {
    Setup();
    FEPrintf(GetPackageName(), 0x42ADB44C, GetLocalizedString(0xE6F55DF0));
}

void UIDeleteProfile::Setup() {
    PMCreateNew* createNew = new ("PMCreateNew", 0) PMCreateNew(0x43798644, 0x55423473, 0);
    createNew->SetReactImmediately(true);
    AddOption(createNew);

    PMDelete* del = new ("PMDelete", 0) PMDelete(0x0D9035CE, 0x9F014666, 0);
    del->SetReactImmediately(true);
    AddOption(del);

    int lastButton = FEngGetLastButton(GetPackageName());
    SetInitialOption(lastButton);

    Refresh();
}

void UIDeleteProfile::Refresh() {
    if (FEDatabase->bProfileLoaded) {
        FEngSetVisible(FEngFindObject(GetPackageName(), FEHashUpper("NAME_GROUP")));
        FEPrintf(GetPackageName(), 0xEB406FEC,
                 FEDatabase->GetUserProfile(0)->GetProfileName());
    } else {
        FEngSetInvisible(FEngFindObject(GetPackageName(), FEHashUpper("NAME_GROUP")));
    }

    RefreshHeader();
    FEDatabase->RefreshCurrentRide();
}

void UIDeleteProfile::NotificationMessage(u32 msg, FEObject* obj, u32 param1,
                                          u32 param2) {
    IconScrollerMenu::NotificationMessage(msg, obj, param1, param2);

    switch (msg) {
    case 0x911AB364:
        cFEng::Get()->QueuePackageSwitch("MC_ProfileManager.fng", 0, 0, false);
        break;
    case 0x7E998E5E:
        Refresh();
        break;
    }
}
