#ifndef __UI_CAREER_MANAGER_HPP__
#define __UI_CAREER_MANAGER_HPP__

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/Gameplay/GManager.h"

class uiCareerManager : public IconScrollerMenu {
  public:
    uiCareerManager(ScreenConstructorData *sd);
    ~uiCareerManager() override {};
    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;
    void Setup() override;
};

class CResumeCareer : public IconOption {
  public:
    CResumeCareer(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~CResumeCareer() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
        if (data == __BUTTON_PRESSED__) {
            bool should_go_into_epic_pursuit = false;
            FEDatabase->SetPlayersJoystickPort(0, FEngMapJoyParamToJoyport(param1));
            FEDatabase->GetCareerSettings()->ResumeCareer();

            if (!FEDatabase->GetCareerSettings()->HasBeatenCareer()) {
                if (GRaceDatabase::Get().IsCareerRaceComplete(
                        GRaceDatabase::Get().GetRaceFromName(GRaceDatabase::Get().GetFinalBossRace())->GetEventHash()))
                    should_go_into_epic_pursuit = true;
            }

            if (FEDatabase->GetCareerSettings()->GetCurrentBin() != 16 && !should_go_into_epic_pursuit) {
                GManager::Get().SetStartingFreeRoamFromSafeHouse();
                cFEng::Get()->QueuePackageSwitch("IG_SafehouseMain.fng", 0, 0, false);
            }
        }
    }
};

class CStartNewCareer : public IconOption {
  public:
    CStartNewCareer(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {
        SetReactImmediately(true);
    }
    ~CStartNewCareer() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
        if (data == __BUTTON_PRESSED__) {
            FEDatabase->SetPlayersJoystickPort(0, FEngMapJoyParamToJoyport(param1));

            if (FEDatabase->GetCareerSettings()->HasCareerStarted() || !FEDatabase->bProfileLoaded) {
                MemcardEnter(pkg_name, pkg_name, 0x80063, nullptr, nullptr, 0, 0);
            } else {
                FEDatabase->GetCareerSettings()->StartNewCareer(true);
                cFEng::Get()->QueuePackageSwitch(pkg_name, 0, 0, false);
            }
        }
    }
};

class CLoadCareer : public IconOption {
  public:
    CLoadCareer(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {
        SetReactImmediately(true);
    }
    ~CLoadCareer() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
        if (data == __BUTTON_PRESSED__) {
            FEDatabase->SetPlayersJoystickPort(0, FEngMapJoyParamToJoyport(param1));

            MemcardEnter(pkg_name, pkg_name, 0x413, nullptr, nullptr, 0x7E998E5E, 0x8867412D);
        }
    }
};

#endif
