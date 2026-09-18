#ifndef FRONTEND_MENUSCREENS_INGAME_CUSTOMTUNING_H
#define FRONTEND_MENUSCREENS_INGAME_CUSTOMTUNING_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feUIWidgetMenu.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/CTextScroller.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"

class TuningSlider;

// File: speed/indep/src/frontend/menuscreens/ingame/CustomTuning.hpp
// total size: 0x2E8
class CustomTuningScreen : public UIWidgetMenu {
  public:
    CustomTuningScreen(ScreenConstructorData *sd);
    ~CustomTuningScreen() override;

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;
    void ScrollTypes(eScrollDir dir);
    void DrawSettingName(uint32 tuning_type);
    static bool IsTuningAvailable(FEPlayerCarDB *stable, FECarRecord *record, Physics::Tunings::Path path);
    uint32 GetNameForPath(Physics::Tunings::Path path, bool turbo);
    uint32 GetHelpForPath(Physics::Tunings::Path path, bool active, bool turbo);
    uint32 AddTuningSlider(FEPlayerCarDB *stable, FECarRecord *record, Physics::Tunings::Path path, bool turbo);
    void Setup() override;
    void SetSlidersForType();
    void ShowHelpBlurb();
    void HideHelpBlurb();
    void StoreSettings();
    bool SettingsDidNotChange();

    CTextScroller *HelpTextScroller;        // offset 0x138, size 0x4
    FEScrollBar *HelpScrollBar;             // offset 0x13C, size 0x4
    FECustomizationRecord *TuningRecord;    // offset 0x140, size 0x4
    FECustomizationRecord TempTuningRecord; // offset 0x144, size 0x198
    int CurrentTuningType;                  // offset 0x2DC, size 0x4
    int HelpVisible;                        // offset 0x2E0, size 0x4
    bool ExitWithStart;                     // offset 0x2E4, size 0x1
};

#endif
