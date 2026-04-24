#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/CustomTuning.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/FEng/FEImage.h"
#include "Speed/Indep/Src/Generated/Events/ETuneVehicle.hpp"
#include "Speed/Indep/Src/Generated/Events/EUnPause.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/DialogInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/CTextScroller.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feScrollerina.hpp"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feWidget.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"

extern int FEPrintf(FEString *, const char *, ...);
extern unsigned int FEngHashString(const char *, ...);
extern FEObject *FEngFindObject(const char *, unsigned int);
extern FEString *FEngFindString(const char *, int);
extern void FEngGetSize(FEObject *, float &, float &);
extern void FEngGetTopLeft(FEObject *, float &, float &);
extern FEngFont *FindFont(unsigned int);
extern const char *GetLocalizedString(unsigned int);
extern void FEngSetCurrentButton(const char *, unsigned int);
extern void FEngSetLanguageHash(FEString *, unsigned int);
extern void FEngSetLanguageHash(const char *, unsigned int, unsigned int);
extern void FEngSetScript(FEObject *, unsigned int, bool);
extern int FEngSNPrintf(char *, int, const char *, ...);
extern int bSNPrintf(char *, int, const char *, ...);

extern const char lbl_803E5088[];
extern const char lbl_803E89B8[];
extern const char lbl_803E89C4[];
extern const char lbl_803E89CC[];
extern const char lbl_803E89FC[];
extern const char lbl_803E8A28[];
extern const char lbl_803E8A38[];
extern const char lbl_803E8A18[];
extern const char lbl_803E4CFC[];
extern const char lbl_803E5EEC[];
extern const float lbl_803E89B4;
extern const float lbl_803E89C0;
extern const float lbl_803E89D8;
extern const float lbl_803E89DC;
extern const float lbl_803E89E0;
extern const float lbl_803E89E4;
extern const float lbl_803E8A24;

namespace Physics {
namespace Upgrades {
void SetLevel(Attrib::Gen::pvehicle &vehicle, int type, int level);
}

namespace Info {
eInductionType InductionType(const Attrib::Gen::pvehicle &vehicle);
}
}

struct TuningSlider : public FEToggleWidget {
    FEObject *pSliderGroup;
    cSlider Negative;
    cSlider Positive;
    unsigned int Title;
    unsigned int HelpBlurb;
    float Min;
    float Max;
    float Current;
    float Increment;
    bool bActive;
    Physics::Tunings::Path TuningPath;

    TuningSlider(Physics::Tunings::Path path, unsigned int title, unsigned int help_blurb, bool active);
    ~TuningSlider() override {}
    void Act(const char *parent_pkg, unsigned int data) override;
    void CheckMouse(const char *parent_pkg, float mouse_x, float mouse_y) override;
    void Draw() override;
    void Position() override;
    void SetFocus(const char *parent_pkg) override;
    void UnsetFocus() override;
    void SetSliderGroup(const char *pkg_name, unsigned int group_name);
    void InitSliderObjects(const char *pkg_name, const char *name);
    void SetSliderValues(float min, float max, float inc, float cur);
};

CustomTuningScreen::CustomTuningScreen(ScreenConstructorData *sd)
    : UIWidgetMenu(sd) //
    , HelpTextScroller(nullptr) //
    , HelpScrollBar(nullptr) //
    , TuningRecord(nullptr) //
    , TempTuningRecord() //
    , CurrentTuningType(0) //
    , HelpVisible(false) //
    , ExitWithStart(false) {
    HelpScrollBar = new (__FILE__, __LINE__) FEScrollBar(sd->PackageFilename, lbl_803E89FC, true, true, false);
    bHasScrollBar = false;
    iMaxWidgetsOnScreen = 8;
    Setup();
}

CustomTuningScreen::~CustomTuningScreen() {
    delete HelpScrollBar;
}

TuningSlider::TuningSlider(Physics::Tunings::Path path, unsigned int title, unsigned int help_blurb, bool active)
    : FEToggleWidget(true) //
{
    Title = title;
    HelpBlurb = help_blurb;
    bActive = active;
    TuningPath = path;
    bMovedLastUpdate = true;
    BlinkArrows(0);
}

void TuningSlider::Act(const char * /* parent_pkg */, unsigned int data) {
    if (!bActive) {
        return;
    }

    if (data == 0xB5971BF1) {
        Current += Increment;
    } else if (data == 0x9120409E) {
        Current -= Increment;
    }

    Current = Current < Min ? Min : Current;
    Current = Current > Max ? Max : Current;
    Negative.SetValue((Max + Min) * lbl_803E89B4 + Min - Current);
    Positive.SetValue(Current);
    Update(data);
}

void TuningSlider::CheckMouse(const char * /* parent_pkg */, float /* mouse_x */, float /* mouse_y */) {}

void TuningSlider::Draw() {
    FEngSetLanguageHash(GetTitleObject(), Title);
    FEPrintf(GetDataObject(), lbl_803E89B8, ((Current - Min) / (Max - Min)) * lbl_803E89C0);
    if (bActive) {
        FEngSetScript(GetTitleObject(), 0x7AB5521A, true);
        FEngSetScript(pSliderGroup, 0x001744B3, true);
    } else {
        FEngSetScript(GetTitleObject(), 0x00163C76, true);
        FEngSetScript(pSliderGroup, 0x00163C76, true);
    }
    Negative.Draw();
    Positive.Draw();
}

void TuningSlider::Position() {}

void TuningSlider::SetFocus(const char *parent_pkg) {
    FEngSetCurrentButton(parent_pkg, GetTitleObject());
    if (bActive) {
        const unsigned long FEObj_Init = 0x7AB5521A;

        FEngSetScript(GetTitleObject(), FEObj_Init, true);
        FEngSetScript(pSliderGroup, 0x001744B3, true);
    }
}

void TuningSlider::UnsetFocus() {
    if (bActive) {
        const unsigned long FEObj_Init = 0x7AB5521A;
        FEngSetScript(GetTitleObject(), FEObj_Init, true);
        FEngSetScript(pSliderGroup, 0x001744B3, true);
    } else {
        const unsigned long FEObj_GREY = 0x00163C76;
        FEngSetScript(GetTitleObject(), FEObj_GREY, true);
        FEngSetScript(pSliderGroup, FEObj_GREY, true);
    }
}

void TuningSlider::SetSliderGroup(const char *pkg_name, unsigned int group_name) {
    pSliderGroup = FEngFindObject(pkg_name, group_name);
}

void TuningSlider::InitSliderObjects(const char *pkg_name, const char *name) {
    char slider_name[32];

    bSNPrintf(slider_name, 32, lbl_803E89C4, name);
    Negative.InitObjects(pkg_name, slider_name);
    bSNPrintf(slider_name, 32, lbl_803E89CC, name);
    Positive.InitObjects(pkg_name, slider_name);
}

void TuningSlider::SetSliderValues(float min, float max, float inc, float cur) {
    float middle = (max + min) * lbl_803E89D8;

    Increment = inc;
    Min = min;
    Max = max;
    Current = cur;
    Negative.InitValues(min, middle, lbl_803E89DC, middle + min - cur, lbl_803E89E0);
    Positive.InitValues((Max + Min) * lbl_803E89D8, Max, lbl_803E89DC, Current, lbl_803E89E4);
}

void CustomTuningScreen::ScrollTypes(eScrollDir dir) {
    if (HelpVisible) {
        return;
    }

    int next_type = CurrentTuningType;

    if (dir == eSD_NEXT) {
        next_type++;
        if (next_type > 2) {
            next_type = 0;
        }
    } else if (dir == eSD_PREV) {
        next_type--;
        if (next_type < 0) {
            next_type = 2;
        }
    }

    if (next_type != CurrentTuningType) {
        CurrentTuningType = next_type;
        SetSlidersForType();
    }
}

void CustomTuningScreen::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    if (HelpVisible) {
        HelpTextScroller->HandleNotificationMessage(msg);
    }

    if (msg == 0x35F8620B) {
        for (FEWidget *option = Options.GetHead(); option != Options.EndOfList(); option = option->GetNext()) {
            option->UnsetFocus();
        }
    }

    if (!HelpVisible || (msg != 0x9120409E && msg != 0xB5971BF1 && msg != 0x72619778 && msg != 0x911C0A4B)) {
        UIWidgetMenu::NotificationMessage(msg, pobj, param1, param2);
    }

    switch (msg) {
    case 0xB5AF2461:
        if (!HelpVisible) {
            ExitWithStart = true;
        } else {
            return;
        }
    case 0x406415E3:
        if (!HelpVisible) {
            StoreSettings();
            cFEng::Get()->QueuePackageMessage(0x587C018B, GetPackageName(), nullptr);
        }
        break;
    case 0x9120409E:
    case 0xB5971BF1:
        if (!HelpVisible) {
            TempTuningRecord.Tunings[CurrentTuningType].Value[static_cast< TuningSlider * >(pCurrentOption)->TuningPath] =
                static_cast< TuningSlider * >(pCurrentOption)->Current;
        }
        break;
    case 0x5073EF13:
        ScrollTypes(eSD_PREV);
        break;
    case 0xD9FEEC59:
        ScrollTypes(eSD_NEXT);
        break;
    case 0xC519BFC4:
        if (!HelpVisible) {
            ShowHelpBlurb();
        }
        break;
    case 0x911AB364:
        if (HelpVisible) {
            HideHelpBlurb();
        } else if (SettingsDidNotChange()) {
            cFEng::Get()->QueuePackageMessage(0x587C018B, GetPackageName(), nullptr);
        } else {
            DialogInterface::ShowTwoButtons(
                GetPackageName(),
                lbl_803E5EEC,
                static_cast< eDialogTitle >(1),
                0x70E01038,
                0x417B25E4,
                0x775DBA97,
                0x34DC1BCF,
                0x34DC1BCF,
                static_cast< eDialogFirstButtons >(1),
                GetLocalizedString(0xE9CB802F));
        }
        break;
    case 0x775DBA97:
        cFEng::Get()->QueuePackageMessage(0x587C018B, GetPackageName(), nullptr);
        break;
    case 0xE1FDE1D1:
        if (ExitWithStart) {
            new EUnPause();
        } else {
            cFEng::Get()->QueuePackageSwitch(lbl_803E4CFC, 0, 0, false);
        }
        break;
    }
}

void CustomTuningScreen::DrawSettingName(unsigned int tuning_type) {
    switch (tuning_type) {
    case 0:
        FEngSetLanguageHash(GetPackageName(), 0x05CDDED4, 0x40230063);
        break;
    case 1:
        FEngSetLanguageHash(GetPackageName(), 0x05CDDED4, 0x40230064);
        break;
    case 2:
        FEngSetLanguageHash(GetPackageName(), 0x05CDDED4, 0x40230065);
        break;
    }
}

unsigned int CustomTuningScreen::AddTuningSlider(FEPlayerCarDB *stable, FECarRecord *record, Physics::Tunings::Path path, bool turbo) {
    char object_name[64];
    float button_width;
    float x;
    float y;
    float width;
    float height;
    bool active = IsTuningAvailable(stable, record, path);
    TuningSlider *slider = new TuningSlider(path, GetNameForPath(path, turbo), GetHelpForPath(path, active, turbo), active);

    slider->SetTitleObject(GetCurrentFEString(pTitleName));
    slider->SetDataObject(GetCurrentFEString(pDataName));
    slider->SetBacking(GetCurrentFEObject(pBackingName));
    FEngSNPrintf(object_name, 64, lbl_803E5088, pSliderName, iIndexToAdd);
    slider->InitSliderObjects(GetPackageName(), object_name);
    FEngSNPrintf(object_name, 64, lbl_803E8A18, pSliderName, iIndexToAdd);
    slider->SetSliderGroup(GetPackageName(), FEngHashString(object_name));
    slider->SetLeftImage(GetCurrentFEImage(pLeftArrowName));
    slider->SetRightImage(GetCurrentFEImage(pRightArrowName));
    Options.AddTail(slider);
    iIndexToAdd++;
    IncrementStartPos();
    FEngGetTopLeft(slider->GetRightImage(), x, y);
    FEngGetSize(slider->GetRightImage(), width, height);
    button_width = (x + width) - x;
    if (button_width < 0.0f) {
        button_width = -button_width;
    }
    slider->SetWidth(button_width);
    return iIndexToAdd - 1;
}

void CustomTuningScreen::Setup() {
    unsigned int current_car;
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECarRecord *record;

    if (FEDatabase->IsCareerMode()) {
        current_car = FEDatabase->GetCareerSettings()->GetCurrentCar();
    } else {
        current_car = FEDatabase->GetQuickRaceSettings(static_cast< GRace::Type >(0xB))->GetSelectedCar(0);
    }

    record = stable->GetCarRecordByHandle(current_car);
    TuningRecord = stable->GetCustomizationRecordByHandle(record->Customization);
    if (TuningRecord != nullptr) {
        TempTuningRecord = *TuningRecord;
        CurrentTuningType = TuningRecord->ActiveTuning;
    }

    AddTuningSlider(stable, record, Physics::Tunings::STEERING, false);
    AddTuningSlider(stable, record, Physics::Tunings::HANDLING, false);
    AddTuningSlider(stable, record, Physics::Tunings::BRAKES, false);
    AddTuningSlider(stable, record, Physics::Tunings::RIDEHEIGHT, false);
    AddTuningSlider(stable, record, Physics::Tunings::AERODYNAMICS, false);
    AddTuningSlider(stable, record, Physics::Tunings::NOS, false);

    Attrib::Gen::pvehicle vehicle(record->VehicleKey, 0, 0);

    Physics::Upgrades::SetLevel(vehicle, 5, 1);
    AddTuningSlider(stable, record, Physics::Tunings::INDUCTION, Physics::Info::InductionType(vehicle) == 1);
    SetSlidersForType();
    SetInitialOption(1);
}

void CustomTuningScreen::SetSlidersForType() {
    DrawSettingName(CurrentTuningType);

    for (FEWidget *option = Options.GetHead(); option != Options.EndOfList(); option = option->GetNext()) {
        TuningSlider *slider = static_cast< TuningSlider * >(option);
        float lower = Physics::Tunings::LowerLimit(slider->TuningPath);
        float upper = Physics::Tunings::UpperLimit(slider->TuningPath);

        slider->SetSliderValues(
            lower,
            upper,
            (upper - lower) * lbl_803E8A24,
            TempTuningRecord.Tunings[CurrentTuningType].Value[slider->TuningPath]);
        slider->Draw();
    }
}

void CustomTuningScreen::ShowHelpBlurb() {
    cFEng::Get()->QueuePackageMessage(0x89D332A9, GetPackageName(), nullptr);
    pCurrentOption->UnsetFocus();
    if (HelpTextScroller == nullptr) {
        FEString *help_text = FEngFindString(GetPackageName(), FEHashUpper(lbl_803E8A28));

        HelpTextScroller = new CTextScroller();
        HelpTextScroller->Initialise(this, help_text->MaxWidth, 7, const_cast< char * >(lbl_803E8A38), FindFont(help_text->Handle));
        HelpTextScroller->UseScrollBar(HelpScrollBar);
        HelpTextScroller->SetTextHash(static_cast< TuningSlider * >(pCurrentOption)->HelpBlurb);
    }
    HelpVisible = true;
}

void CustomTuningScreen::HideHelpBlurb() {
    cFEng::Get()->QueuePackageMessage(0x950AD1C2, GetPackageName(), nullptr);
    pCurrentOption->SetFocus(GetPackageName());
    if (HelpTextScroller != nullptr) {
        delete HelpTextScroller;
        HelpTextScroller = nullptr;
    }
    HelpVisible = false;
}

void CustomTuningScreen::StoreSettings() {
    for (int tuning = 0; tuning < Physics::NUM_CUSTOM_TUNINGS; tuning++) {
        for (int path = 0; path < Physics::Tunings::MAX_TUNINGS; path++) {
            TuningRecord->Tunings[tuning].Value[path] = TempTuningRecord.Tunings[tuning].Value[path];
        }
    }

    TuningRecord->ActiveTuning = static_cast< Physics::eCustomTuningType >(CurrentTuningType);
    new ETuneVehicle(0, reinterpret_cast< const Tunings * >(&TuningRecord->Tunings[TuningRecord->ActiveTuning]));
}

bool CustomTuningScreen::SettingsDidNotChange() {
    for (int tuning = 0; tuning < Physics::NUM_CUSTOM_TUNINGS; tuning++) {
        for (int path = 0; path < Physics::Tunings::MAX_TUNINGS; path++) {
            if (TuningRecord->Tunings[tuning].Value[path] != TempTuningRecord.Tunings[tuning].Value[path]) {
                return false;
            }
        }
    }

    return CurrentTuningType == TuningRecord->ActiveTuning;
}

bool CustomTuningScreen::IsTuningAvailable(FEPlayerCarDB *stable, FECarRecord *record, Physics::Tunings::Path path) {
    if (record == nullptr) {
        return false;
    }

    FECustomizationRecord *custom = stable->GetCustomizationRecordByHandle(record->Customization);
    if (custom == nullptr) {
        return false;
    }

    switch (path) {
    case Physics::Tunings::STEERING:
    case Physics::Tunings::HANDLING:
    case Physics::Tunings::RIDEHEIGHT:
        return custom->InstalledPhysics.Part[2] > 0;
    case Physics::Tunings::BRAKES:
        return custom->InstalledPhysics.Part[1] > 0;
    case Physics::Tunings::AERODYNAMICS: {
        RideInfo ride;

        ride.Init(static_cast< CarType >(-1), CarRenderUsage_Player, 0, 0);
        stable->BuildRideForPlayer(record->Handle, 0, &ride);
        ride.SetStockParts();

        struct CarPart *stock = ride.GetPart(0x2C);
        struct CarPart *installed = custom->GetInstalledPart(record->GetType(), 0x2C);
        if (installed != nullptr && installed != stock) {
            return true;
        }

        stock = ride.GetPart(0x17);
        installed = custom->GetInstalledPart(record->GetType(), 0x17);
        return installed != nullptr && installed != stock;
    }
    case Physics::Tunings::NOS:
        return custom->InstalledPhysics.Part[6] > 0;
    case Physics::Tunings::INDUCTION:
        return custom->InstalledPhysics.Part[5] > 0;
    default:
        return false;
    }
}

unsigned int CustomTuningScreen::GetNameForPath(Physics::Tunings::Path path, bool turbo) {
    switch (path) {
    case Physics::Tunings::STEERING:
        return 0xC56C5B36;
    case Physics::Tunings::HANDLING:
        return 0xFCEEBE1A;
    case Physics::Tunings::BRAKES:
        return 0x2EE2A74D;
    case Physics::Tunings::RIDEHEIGHT:
        return 0xCF6215D1;
    case Physics::Tunings::AERODYNAMICS:
        return 0x7196ACB4;
    case Physics::Tunings::NOS:
        return 0x934FCFA9;
    case Physics::Tunings::INDUCTION:
        return turbo ? 0x86945521 : 0xE3A932E0;
    default:
        return 0;
    }
}

unsigned int CustomTuningScreen::GetHelpForPath(Physics::Tunings::Path path, bool active, bool turbo) {
    if (active) {
        switch (path) {
        case Physics::Tunings::STEERING:
            return 0x4A1F18BE;
        case Physics::Tunings::HANDLING:
            return 0x81A17BA2;
        case Physics::Tunings::BRAKES:
            return 0x473862D5;
        case Physics::Tunings::RIDEHEIGHT:
            return 0xD6C24659;
        case Physics::Tunings::AERODYNAMICS:
            return 0x64FCEE3C;
        case Physics::Tunings::NOS:
            return 0xB65CFC31;
        case Physics::Tunings::INDUCTION:
            return turbo ? 0xB5DCBFA9 : 0xD70F7468;
        default:
            return 0;
        }
    }

    switch (path) {
    case Physics::Tunings::STEERING:
        return 0x221D7E85;
    case Physics::Tunings::HANDLING:
        return 0x18C12069;
    case Physics::Tunings::BRAKES:
        return 0xC213A6DC;
    case Physics::Tunings::RIDEHEIGHT:
        return 0xB6D02C60;
    case Physics::Tunings::AERODYNAMICS:
        return 0xC6A99483;
    case Physics::Tunings::NOS:
        return 0xB8124038;
    case Physics::Tunings::INDUCTION:
        return turbo ? 0xB7D6F7B0 : 0xE3F577AF;
    default:
        return 0;
    }
}

static MenuScreen *CreateCustomTuningScreen(ScreenConstructorData *sd) {
    return new (__FILE__, __LINE__) CustomTuningScreen(sd);
}
