#ifndef FRONTEND_MENUSCREENS_INGAME_CUSTOMTUNING_H
#define FRONTEND_MENUSCREENS_INGAME_CUSTOMTUNING_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/UIWidgetMenu.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"

struct CTextScroller;
class FEScrollBar;

class CustomTuningScreen : public UIWidgetMenu {
  public:
    CustomTuningScreen(ScreenConstructorData *sd);
    ~CustomTuningScreen() override;
    void NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) override;
    static bool IsTuningAvailable(FEPlayerCarDB *stable, FECarRecord *record, Physics::Tunings::Path path);
    void ScrollTypes(eScrollDir dir);
    void DrawSettingName(unsigned int tuning_type);
    unsigned int GetNameForPath(Physics::Tunings::Path path, bool turbo);
    unsigned int GetHelpForPath(Physics::Tunings::Path path, bool active, bool turbo);
    unsigned int AddTuningSlider(FEPlayerCarDB *stable, FECarRecord *record, Physics::Tunings::Path path, bool turbo);
    void Setup() override;
    void SetSlidersForType();
    void ShowHelpBlurb();
    void HideHelpBlurb();
    void StoreSettings();
    bool SettingsDidNotChange();

  protected:
    CTextScroller *HelpTextScroller;
    FEScrollBar *HelpScrollBar;
    FECustomizationRecord *TuningRecord;
    FECustomizationRecord TempTuningRecord;
    int CurrentTuningType;
    bool HelpVisible;
    bool ExitWithStart;
};

#endif
