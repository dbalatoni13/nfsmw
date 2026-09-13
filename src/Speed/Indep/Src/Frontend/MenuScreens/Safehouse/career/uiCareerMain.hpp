#ifndef __UI_CAREER_MAIN_HPP__
#define __UI_CAREER_MAIN_HPP__

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"
#include "types.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"

class uiCareerCrib : public IconScrollerMenu {
  public:
    uiCareerCrib(ScreenConstructorData *sd);
    ~uiCareerCrib() override {};
    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

  private:
    void Setup() override;
};

class CResumeFreeRoam : public IconOption {
  public:
    CResumeFreeRoam(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {
        SetReactImmediately(true);
    }
    ~CResumeFreeRoam() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
        if (data == __BUTTON_PRESSED__) {
            FEDatabase->SetPlayersJoystickPort(0, FEngMapJoyParamToJoyport(param1));
            DialogInterface ::ShowTwoButtons(pkg_name, "", dialog_alert, LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO, dialog_message_yes,
                                             dialog_message_no, dialog_message_no, first_dialog_button2, GetLocalizedString(0xEB694C0C));
        }
    }
};

class CCarSelect : public IconOption {
  public:
    CCarSelect(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~CCarSelect() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
        if (data == __BUTTON_PRESSED__) {
            cFEng::Get()->QueuePackageSwitch("IG_CarLot.fng", 0, 0, false);
        }
    }
};

class CRapSheet : public IconOption {
  public:
    CRapSheet(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~CRapSheet() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
        if (data == __BUTTON_PRESSED__) {
            FEDatabase->SetGameMode(eFE_GAME_MODE_RAP_SHEET);
            cFEng::Get()->QueuePackageSwitch("RapSheetMain.fng", 0, 0, false);
        }
    };
};

class CTop15 : public IconOption {
  public:
    CTop15(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~CTop15() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
        if (data == __BUTTON_PRESSED__) {
            extern int iCurrentViewBin;
            iCurrentViewBin = FEDatabase->GetCareerSettings()->GetCurrentBin();
            cFEng::Get()->QueuePackageSwitch("WorldMap_Main.fng", 0, 0, false);
        }
    }
};

class CSave : public IconOption {
  public:
    CSave(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {
        SetReactImmediately(true);
    }
    ~CSave() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
        if (data == __BUTTON_PRESSED__) {
            MemcardEnter(pkg_name, pkg_name, 0x2251, nullptr, nullptr, 0, 0);
        }
    }
};

#endif
