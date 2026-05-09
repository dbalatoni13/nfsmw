#include "uiProfileManager.hpp"

#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/Frontend/FEPackageData.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"

inline void PMSave::React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) {
    if (data == 0x0C407210) {
        MemcardEnter(pkg_name, pkg_name, 0x2251, 0, 0, 0, 0);
    }
}

inline void PMLoad::React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) {
    if (data == 0x0C407210) {
        MemcardEnter(pkg_name, pkg_name, 0x411, 0, 0, 0x3A2BE557, 0x8867412D);
    }
}

inline void PMDelete::React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) {
    if (data == 0x0C407210) {
        MemcardEnter(pkg_name, pkg_name, 0x31, 0, 0, 0, 0);
    }
}

inline void PMCreateNew::React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) {
    if (data == 0x0C407210) {
        MemcardEnter(pkg_name, pkg_name, 0x61, 0, 0, 0, 0);
    }
}

inline void PMPopDelete::React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) {
    if (data == 0x0C407210) {
        MemcardEnter(pkg_name, pkg_name, 0x61, 0, 0, 0, 0);
    }
}

MenuScreen *CreateUIProfileManager(ScreenConstructorData *sd) {
    return new UIProfileManager(sd);
}

UIProfileManager::UIProfileManager(ScreenConstructorData *sd) : IconScrollerMenu(sd) {
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
        FEngSetVisible(FEngFindObject(GetPackageName(), FEHashUpper("PROFILE_NAME_GROUP")));
        FEPrintf(GetPackageName(), 0xEB406FEC, FEDatabase->GetUserProfile(0)->GetProfileName());
    } else {
        FEngSetInvisible(FEngFindObject(GetPackageName(), FEHashUpper("PROFILE_NAME_GROUP")));
    }

    FEngSetColor(mpSave->FEngObject, mpSave->OriginalColor);
    RefreshHeader();
}

void UIProfileManager::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
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
    mpSave = new PMSave(0x228B7E32, 0x1C8ACE, 0);
    mpSave->SetReactImmediately(true);

    PMPopDelete *popDelete = new PMPopDelete(0x43798644, 0x55423473, 0);
    popDelete->SetReactImmediately(true);
    AddOption(popDelete);

    PMLoad *load = new PMLoad(0x2287E063, 0x18ECFF, 0);
    load->SetReactImmediately(true);
    AddOption(load);

    AddOption(mpSave);

    PMDelete *del = new PMDelete(0x0D9035CE, 0x56B00632, 0);
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

MenuScreen *CreateUIDeleteProfile(ScreenConstructorData *sd) {
    return new UIDeleteProfile(sd);
}

UIDeleteProfile::UIDeleteProfile(ScreenConstructorData *sd) : IconScrollerMenu(sd) {
    Setup();
    FEPrintf(GetPackageName(), 0x42ADB44C, GetLocalizedString(0xE6F55DF0));
}

void UIDeleteProfile::Setup() {
    PMCreateNew *createNew = new PMCreateNew(0x43798644, 0x55423473, 0);
    createNew->SetReactImmediately(true);
    AddOption(createNew);

    PMDelete *del = new PMDelete(0x0D9035CE, 0x9F014666, 0);
    del->SetReactImmediately(true);
    AddOption(del);

    int lastButton = FEngGetLastButton(GetPackageName());
    SetInitialOption(lastButton);

    Refresh();
}

void UIDeleteProfile::Refresh() {
    if (FEDatabase->bProfileLoaded) {
        FEngSetVisible(FEngFindObject(GetPackageName(), FEHashUpper("PROFILE_NAME_GROUP")));
        FEPrintf(GetPackageName(), 0xEB406FEC, FEDatabase->GetUserProfile(0)->GetProfileName());
    } else {
        FEngSetInvisible(FEngFindObject(GetPackageName(), FEHashUpper("PROFILE_NAME_GROUP")));
    }

    RefreshHeader();
    FEDatabase->RefreshCurrentRide();
}

void UIDeleteProfile::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    IconScrollerMenu::NotificationMessage(msg, obj, param1, param2);

    switch (msg) {
        case 0x911AB364:
            cFEng::Get()->QueuePackageSwitch("ProfileManager.fng", 0, 0, false);
            break;
        case 0x7E998E5E:
            Refresh();
            break;
    }
}
