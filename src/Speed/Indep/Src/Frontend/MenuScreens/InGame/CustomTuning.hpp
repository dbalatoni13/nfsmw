#ifndef CUSTOMTUNING_HPP
#define CUSTOMTUNING_HPP

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feUIWidgetMenu.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"

class TuningSlider : public FEToggleWidget {
  public:
    TuningSlider(Physics::Tunings::Path path, uint32 title, uint32 help_blurb, bool active);
    ~TuningSlider() override {}
    void Act(const char *parent_pkg, uint32 data) override;
    void CheckMouse(const char *parent_pkg, float mouse_x, float mouse_y) override;
    void Draw() override;
    void Position() override;
    void SetFocus(const char *parent_pkg) override;
    void UnsetFocus() override;
    void SetSliderGroup(const char *pkg_name, uint32 group_name);
    void InitSliderObjects(const char *pkg_name, const char *name);
    void SetSliderValues(float min, float max, float inc, float cur);

    FEObject *pSliderGroup;
    cSlider Negative;
    cSlider Positive;
    uint32 Title;
    uint32 HelpBlurb;
    float Min;
    float Max;
    float Current;
    float Increment;
    bool bActive;
    Physics::Tunings::Path TuningPath;
};

class CustomTuningScreen : public UIWidgetMenu {
  public:
    CustomTuningScreen(ScreenConstructorData *sd);
    ~CustomTuningScreen() override {}

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

    static bool IsTuningAvailable(FEPlayerCarDB *stable, FECarRecord *record, Physics::Tunings::Path path);

  protected:
    void ScrollTypes(eScrollDir dir);

    void DrawSettingName(uint32 tuning_type);

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
    bool HelpVisible;                       // offset 0x2E0, size 0x1
    bool ExitWithStart;                     // offset 0x2E4, size 0x1
};

#endif
