// OWNED BY zFeOverlay AGENT - DO NOT MODIFY OR EMPTY
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRMainMenu.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"

extern int QRMode;
extern int GetMikeMannBuild();
extern int FEngGetLastButton(const char *pkg_name);
void FEngSetLanguageHash(const char *pkg_name, unsigned int obj_hash, unsigned int lang_hash);
// _SetQRMode defined in uiQRModeSelect.cpp
static void _SetQRMode(int mode);

struct QuickPlay : public IconOption {
    QuickPlay(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}

    void React(const char *pkg_name, unsigned int data, FEObject *obj, unsigned int param1, unsigned int param2) override {
        if (data == 0xc407210) {
            _SetQRMode(0);
        }
    }
};

struct CustomRace : public IconOption {
    CustomRace(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}

    void React(const char *pkg_name, unsigned int data, FEObject *obj, unsigned int param1, unsigned int param2) override {
        if (data == 0xc407210) {
            _SetQRMode(1);
        }
    }
};

struct SplitScreenOption : public IconOption {
    SplitScreenOption(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}

    void React(const char *pkg_name, unsigned int data, FEObject *obj, unsigned int param1, unsigned int param2) override {
        if (data == 0xc407210) {
            _SetQRMode(2);
        }
    }
};

UIQRMainMenu::~UIQRMainMenu() {}

UIQRMainMenu::UIQRMainMenu(ScreenConstructorData *sd) : IconScrollerMenu(sd) {
    Setup();
    RefreshHeader();
}

void UIQRMainMenu::RefreshHeader() {
    IconScrollerMenu::RefreshHeader();
    FEngSetLanguageHash(PackageFilename, 0x3c458c1, 0xcdf0cc03);
    FEngSetLanguageHash(PackageFilename, 0xb5c74226, 0xcdf0cc03);
}

void UIQRMainMenu::Setup() {
    if (!GetMikeMannBuild()) {
        IconOption *qp = new QuickPlay(0xe6313967, 0xb5e8f82f, 0);
        AddOption(qp);
        IconOption *cr = new CustomRace(0x2a49b5e2, 0x25bbd4c3, 0);
        AddOption(cr);
        IconOption *ss = new SplitScreenOption(0xf365b5f5, 0x841d518a, 0);
        AddOption(ss);
    } else {
        IconOption *cr = new CustomRace(0x2a49b5e2, 0x25bbd4c3, 0);
        AddOption(cr);
    }
    int lastBtn = FEngGetLastButton(PackageFilename);
    if (bFadeInIconsImmediately) {
        Options.bFadingIn = true;
        Options.bFadingOut = false;
        Options.bDelayUpdate = false;
        Options.fCurFadeTime = 0.0f;
    }
    Options.SetInitialPos(lastBtn);
    FEDatabase->RefreshCurrentRide();
}

void UIQRMainMenu::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    IconScrollerMenu::NotificationMessage(msg, pobj, param1, param2);
    if (msg == 0xe1fde1d1) {
        switch (PrevButtonMessage) {
        case 0xc407210: {
            FEDatabase->iNumPlayers = 1;
            switch (QRMode) {
            case 1:
                FEDatabase->SetGameMode(static_cast<eFEGameModes>(FEDatabase->GetGameMode() | 0x400));
                cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
                break;
            case 0:
                cFEng::Get()->QueuePackageSwitch("Quick_Race_Brief.fng", 0, 0, false);
                break;
            case 2:
                FEDatabase->iNumPlayers = 2;
                FEDatabase->SetGameMode(static_cast<eFEGameModes>(FEDatabase->GetGameMode() | 0x400));
                cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
                break;
            }
            break;
        }
        case 0x911ab364:
            cFEng::Get()->QueuePackageSwitch("MainMenu.fng", 0, 0, false);
            break;
        }
    }
}
