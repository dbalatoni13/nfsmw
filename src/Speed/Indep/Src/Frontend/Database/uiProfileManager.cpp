#include "uiProfileManager.hpp"

#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"

FEObject* FEngFindObject(const char* pkg_name, unsigned int obj_hash);
void FEngSetVisible(FEObject* obj);
void FEngSetInvisible(FEObject* obj);
void FEngSetColor(FEObject* obj, unsigned int color);
unsigned char FEngGetLastButton(const char* pkg_name);
const char* GetLocalizedString(unsigned int hash);
void MemcardEnter(const char* from, const char* to, unsigned int op, void (*pTermFunc)(void*),
                  void* pTermFuncParam, unsigned int msgSuccess, unsigned int msgFailed);

MenuScreen* CreateUIProfileManager(ScreenConstructorData* sd) {
    return new UIProfileManager(sd);
}

UIProfileManager::UIProfileManager(ScreenConstructorData* sd)
    : IconScrollerMenu(sd) {
    Setup();
    FEPrintf(GetPackageName(), 0x42ADB44C, GetLocalizedString(0xBCB18F38));
}

UIProfileManager::~UIProfileManager() {}

void UIProfileManager::Refresh() {
    if (FEDatabase->bProfileLoaded) {
        mpSave->IsGreyOut = false;
    } else {
        mpSave->IsGreyOut = true;
    }

    if (FEDatabase->bProfileLoaded) {
        FEngSetVisible(FEngFindObject(GetPackageName(), FEHashUpper("PROFILE_NAME_GROUP")));
        FEPrintf(GetPackageName(), 0xEB406FEC,
                 FEDatabase->GetUserProfile(0)->GetProfileName());
    } else {
        FEngSetInvisible(FEngFindObject(GetPackageName(), FEHashUpper("PROFILE_NAME_GROUP")));
    }

    FEngSetColor(mpSave->FEngObject, mpSave->OriginalColor);
    RefreshHeader();
}

void UIProfileManager::NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1,
                                           unsigned long param2) {
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

    PMPopDelete* popDelete = new PMPopDelete(0x43798644, 0x55423473, 0);
    popDelete->SetReactImmediately(true);
    AddOption(popDelete);

    PMLoad* load = new PMLoad(0x2287E063, 0x18ECFF, 0);
    load->SetReactImmediately(true);
    AddOption(load);

    AddOption(mpSave);

    PMDelete* del = new PMDelete(0x0D9035CE, 0x56B00632, 0);
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
    return new UIDeleteProfile(sd);
}

UIDeleteProfile::UIDeleteProfile(ScreenConstructorData* sd)
    : IconScrollerMenu(sd) {
    Setup();
    FEPrintf(GetPackageName(), 0x42ADB44C, GetLocalizedString(0xE6F55DF0));
}

UIDeleteProfile::~UIDeleteProfile() {}

void UIDeleteProfile::Setup() {
    PMCreateNew* createNew = new PMCreateNew(0x43798644, 0x55423473, 0);
    createNew->SetReactImmediately(true);
    AddOption(createNew);

    PMDelete* del = new PMDelete(0x0D9035CE, 0x9F014666, 0);
    del->SetReactImmediately(true);
    AddOption(del);

    int lastButton = FEngGetLastButton(GetPackageName());
    SetInitialOption(lastButton);

    Refresh();
}

void UIDeleteProfile::Refresh() {
    if (FEDatabase->bProfileLoaded) {
        FEngSetVisible(FEngFindObject(GetPackageName(), FEHashUpper("PROFILE_NAME_GROUP")));
        FEPrintf(GetPackageName(), 0xEB406FEC,
                 FEDatabase->GetUserProfile(0)->GetProfileName());
    } else {
        FEngSetInvisible(FEngFindObject(GetPackageName(), FEHashUpper("PROFILE_NAME_GROUP")));
    }

    RefreshHeader();
    FEDatabase->RefreshCurrentRide();
}

void UIDeleteProfile::NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1,
                                          unsigned long param2) {
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

void PMSave::React(const char* pkg_name, unsigned int data, FEObject* obj,
                   unsigned int param1, unsigned int param2) {
    if (data == 0x0C407210) {
        MemcardEnter(pkg_name, pkg_name, 0x2251, 0, 0, 0, 0);
    }
}

void PMLoad::React(const char* pkg_name, unsigned int data, FEObject* obj,
                   unsigned int param1, unsigned int param2) {
    if (data == 0x0C407210) {
        MemcardEnter(pkg_name, pkg_name, 0x411, 0, 0, 0x3A2BE557, 0x8867412D);
    }
}

void PMDelete::React(const char* pkg_name, unsigned int data, FEObject* obj,
                     unsigned int param1, unsigned int param2) {
    if (data == 0x0C407210) {
        MemcardEnter(pkg_name, pkg_name, 0x31, 0, 0, 0, 0);
    }
}

void PMCreateNew::React(const char* pkg_name, unsigned int data, FEObject* obj,
                        unsigned int param1, unsigned int param2) {
    if (data == 0x0C407210) {
        MemcardEnter(pkg_name, pkg_name, 0x61, 0, 0, 0, 0);
    }
}

void PMPopDelete::React(const char* pkg_name, unsigned int data, FEObject* obj,
                        unsigned int param1, unsigned int param2) {
    if (data == 0x0C407210) {
        MemcardEnter(pkg_name, pkg_name, 0x61, 0, 0, 0, 0);
    }
}
