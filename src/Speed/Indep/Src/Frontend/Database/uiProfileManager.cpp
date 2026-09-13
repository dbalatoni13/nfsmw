#include "uiProfileManager.hpp"

#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/Frontend/FEPackageData.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"

inline void PMSave::React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) {
    if (data == __BUTTON_PRESSED__) {
        MemcardEnter(pkg_name, pkg_name, 0x2251, nullptr, nullptr, 0, 0);
    }
}

inline void PMLoad::React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) {
    if (data == __BUTTON_PRESSED__) {
        MemcardEnter(pkg_name, pkg_name, 0x411, nullptr, nullptr, 0x3A2BE557, 0x8867412D);
    }
}

inline void PMDelete::React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) {
    if (data == __BUTTON_PRESSED__) {
        MemcardEnter(pkg_name, pkg_name, 0x31, nullptr, nullptr, 0, 0);
    }
}

inline void PMCreateNew::React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) {
    if (data == __BUTTON_PRESSED__) {
        MemcardEnter(pkg_name, pkg_name, 0x61, nullptr, nullptr, 0, 0);
    }
}

inline void PMPopDelete::React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) {
    if (data == __BUTTON_PRESSED__) {
        MemcardEnter(pkg_name, pkg_name, 0x61, nullptr, nullptr, 0, 0);
    }
}

MenuScreen *CreateUIProfileManager(ScreenConstructorData *sd) {
    return new ("CreateUIProfileManager", 0) UIProfileManager(sd);
}

UIProfileManager::UIProfileManager(ScreenConstructorData *sd) : IconScrollerMenu(sd) {
    Setup();

    const u32 FEObj_TitleMaster = FEHASH_HEADERTEXT;
    FEPrintf(GetPackageName(), FEObj_TitleMaster, GetLocalizedString(0xBCB18F38));
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

void UIProfileManager::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    IconScrollerMenu::NotificationMessage(msg, obj, param1, param2);

    switch (msg) {
        case __PAD_BACK__:
            cFEng::Get()->QueuePackageSwitch("MainMenu.fng", 0, 0, false);
            break;
        case FEHASH_INITCOMPLETE:
            Refresh();
            break;
        case 0x7E998E5E:
            FEDatabase->RefreshCurrentRide();
            Refresh();
            break;
    }
}

void UIProfileManager::Setup() {
    this->mpSave = new ("PMSave", 0) PMSave(0x228B7E32, 0x1C8ACE, 0);

    if (false) {
        AddOption(new ("PMPopDelete", 0) PMPopDelete(0x6b303856, 0xe6f55df0, 0));
    }

    AddOption(new ("PMCreateNew", 0) PMCreateNew(0x43798644, 0x55423473, 0));

    AddOption(new ("PMLoad", 0) PMLoad(0x2287E063, 0x18ECFF, 0));

    AddOption(this->mpSave);

    AddOption(new ("PMDelete", 0) PMDelete(0x0D9035CE, 0x56B00632, 0));

    this->SetInitialOption(FEngGetLastButton(GetPackageName()));

    Refresh();
}

MenuScreen *CreateUIDeleteProfile(ScreenConstructorData *sd) {
    return new ("CreateUIDeleteProfile", 0) UIDeleteProfile(sd);
}

UIDeleteProfile::UIDeleteProfile(ScreenConstructorData *sd) : IconScrollerMenu(sd) {
    Setup();

    const u32 FEObj_TitleMaster = FEHASH_HEADERTEXT;
    FEPrintf(GetPackageName(), FEObj_TitleMaster, GetLocalizedString(0xE6F55DF0));
}

void UIDeleteProfile::Setup() {
    AddOption(new ("PMCreateNew", 0) PMCreateNew(0x43798644, 0x55423473, 0));

    AddOption(new ("PMDelete", 0) PMDelete(0x0D9035CE, 0x9F014666, 0));

    SetInitialOption(FEngGetLastButton(GetPackageName()));

    Refresh();
}

void UIDeleteProfile::Refresh() {
    if (FEDatabase->bProfileLoaded) {
        FEngSetVisible(GetPackageName(), FEHashUpper("NAME_GROUP"));
        FEPrintf(GetPackageName(), 0xEB406FEC, FEDatabase->GetUserProfile(0)->GetProfileName());
    } else {
        FEngSetInvisible(GetPackageName(), FEHashUpper("NAME_GROUP"));
    }

    RefreshHeader();
    FEDatabase->RefreshCurrentRide();
}

void UIDeleteProfile::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    IconScrollerMenu::NotificationMessage(msg, obj, param1, param2);

    switch (msg) {
        case __PAD_BACK__:
            cFEng::Get()->QueuePackageSwitch("MC_ProfileManager.fng", 0, 0, false);
            break;
        case 0x7E998E5E:
            Refresh();
            break;
    }
}
