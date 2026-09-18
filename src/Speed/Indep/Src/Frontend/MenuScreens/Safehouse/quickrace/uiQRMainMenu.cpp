#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRMainMenu.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEButtons.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEPackageData.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"

int QRMode = 0;

static void _SetQRMode(int mode) {
    QRMode = mode;
}

class QuickPlay : public IconOption {
  public:
    QuickPlay(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}

    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
        if (data == 0xc407210) {
            _SetQRMode(0);
        }
    }
};

class CustomRace : public IconOption {
  public:
    CustomRace(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}

    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
        if (data == 0xc407210) {
            _SetQRMode(1);
        }
    }
};

class SplitScreen : public IconOption {
  public:
    SplitScreen(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}

    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
        if (data == 0xc407210) {
            _SetQRMode(2);
        }
    }
};

UIQRMainMenu::UIQRMainMenu(ScreenConstructorData *sd) : IconScrollerMenu(sd) {
    Setup();
    RefreshHeader();
}

void UIQRMainMenu::RefreshHeader() {
    IconScrollerMenu::RefreshHeader();
    FEngSetLanguageHash(GetPackageName(), 0x3c458c1, 0xcdf0cc03);
    FEngSetLanguageHash(GetPackageName(), 0xb5c74226, 0xcdf0cc03);
}

void UIQRMainMenu::Setup() {
    if (GetMikeMannBuild()) {
        AddOption(new ("CustomRace", 0) CustomRace(0x2a49b5e2, 0x25bbd4c3, 0));
    } else {
        AddOption(new ("QuickPlay", 0) QuickPlay(0xe6313967, 0xb5e8f82f, 0));
        AddOption(new ("CustomRace", 0) CustomRace(0x2a49b5e2, 0x25bbd4c3, 0));
        AddOption(new ("SplitScreen", 0) SplitScreen(0xf365b5f5, 0x841d518a, 0));
    }
    SetInitialOption(FEngGetLastButton(GetPackageName()));
    FEDatabase->RefreshCurrentRide();
}

void UIQRMainMenu::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    IconScrollerMenu::NotificationMessage(msg, pobj, param1, param2);
    switch (msg) {
        case 0xE1FDE1D1:
            switch (PrevButtonMessage) {
                case __BUTTON_PRESSED__:
                    FEDatabase->iNumPlayers = 1;
                    switch (QRMode) {
                        case 0:
                            cFEng::Get()->QueuePackageSwitch("Quick_Race_Brief.fng", 0, 0, false);
                            break;
                        case 2:
                            FEDatabase->SetGameMode(eFE_GAME_MODE_MODE_SELECT);
                            FEDatabase->iNumPlayers = 2;
                            cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
                            break;
                        case 1:
                            FEDatabase->SetGameMode(eFE_GAME_MODE_MODE_SELECT);
                            cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
                            break;
                    }
                    break;
                case __PAD_BACK__:
                    cFEng::Get()->QueuePackageSwitch("MainMenu.fng", 0, 0, false);
                    break;
            }
    }
}
