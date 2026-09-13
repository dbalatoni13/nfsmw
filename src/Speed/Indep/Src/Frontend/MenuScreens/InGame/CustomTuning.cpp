#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/CustomTuning.hpp"

#include "Speed/Indep/Src/FEng/FEngStandard.h"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Generated/Events/ETuneVehicle.hpp"
#include "Speed/Indep/Src/Generated/Events/EUnPause.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feWidget.hpp"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feWidget.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Physics/PhysicsTunings.h"
#include "Speed/Indep/Src/Physics/PhysicsUpgrades.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

TuningSlider::TuningSlider(Physics::Tunings::Path path, uint32 title, uint32 help_blurb, bool active)
    : FEToggleWidget(true), Title(title), HelpBlurb(help_blurb), bActive(active), TuningPath(path) {
    Update(0);
}

void TuningSlider::Act(const char *parent_pkg, uint32 data) {
    if (!bActive) {
        return;
    }

    if (data == __PAD_RIGHT__) {
        Current += Increment;
    } else if (data == __PAD_LEFT__) {
        Current -= Increment;
    }

    Current = bMin(bMax(Current, Min), Max);
    Negative.SetValue((Max + Min) * 0.5f + Min - Current);
    Positive.SetValue(Current);
    Update(data);
    Draw();
}

void TuningSlider::CheckMouse(const char *parent_pkg, const float mouse_x, const float mouse_y) {}

void TuningSlider::Draw() {
    FEngSetLanguageHash(GetTitleObject(), Title);
    float val = ((Current - Min) / (Max - Min)) * 10.0f;
    FEPrintf(GetDataObject(), "%2.1f", val);
    if (bActive) {
        const u32 FEObj_Init = FEHASH_INIT;
        FEngSetScript(GetTitleObject(), FEHASH_UNHIGHLIGHT, true);
        FEngSetScript(pSliderGroup, FEObj_Init, true);
    } else {
        const u32 FEObj_GREY = FEHASH_GREY;
        FEngSetScript(GetTitleObject(), FEObj_GREY, true);
        FEngSetScript(pSliderGroup, FEObj_GREY, true);
    }
    Negative.Draw();
    Positive.Draw();
}

void TuningSlider::Position() {}

void TuningSlider::SetFocus(const char *parent_pkg) {
    FEngSetCurrentButton(parent_pkg, GetTitleObject());
    if (bActive) {
        const u32 FEObj_Init = FEHASH_INIT;

        FEngSetScript(GetTitleObject(), FEHASH_UNHIGHLIGHT, true);
        FEngSetScript(pSliderGroup, FEObj_Init, true);
    }
}

void TuningSlider::UnsetFocus() {
    if (bActive) {
        const u32 FEObj_Init = FEHASH_INIT;
        FEngSetScript(GetTitleObject(), FEHASH_UNHIGHLIGHT, true);
        FEngSetScript(pSliderGroup, FEObj_Init, true);
    } else {
        const u32 FEObj_GREY = FEHASH_GREY;
        FEngSetScript(GetTitleObject(), FEObj_GREY, true);
        FEngSetScript(pSliderGroup, FEObj_GREY, true);
    }
}

void TuningSlider::SetSliderGroup(const char *pkg_name, unsigned int group_name) {
    pSliderGroup = FEngFindObject(pkg_name, group_name);
}

void TuningSlider::InitSliderObjects(const char *pkg_name, const char *name) {
    char sztemp[32];

    bSNPrintf(sztemp, sizeof(sztemp), "LEFT_%s", name);
    Negative.InitObjects(pkg_name, sztemp);
    bSNPrintf(sztemp, sizeof(sztemp), "RIGHT_%s", name);
    Positive.InitObjects(pkg_name, sztemp);
}

void TuningSlider::SetSliderValues(float min, float max, float inc, float cur) {
    Increment = inc;
    Min = min;
    Max = max;
    Current = cur;
    Negative.InitValues(min, (max + min) * 0.5f, 0.0f, (max + min) * 0.5f + min - cur, -82.0f);
    Positive.InitValues((Max + Min) * 0.5f, Max, 0.0f, Current, 82.0f);
}

CustomTuningScreen::CustomTuningScreen(ScreenConstructorData *sd)
    : UIWidgetMenu(sd),          //
      HelpTextScroller(nullptr), //
      HelpScrollBar(nullptr),    //
      TuningRecord(nullptr),     //
      TempTuningRecord(),        //
      CurrentTuningType(0),      //
      HelpVisible(false),        //
      ExitWithStart(false) {
    HelpScrollBar = new ("Tuning FEScrollBar", 0) FEScrollBar(GetPackageName(), "Scrollbar", true, true, false);
    bHasScrollBar = false;
    iMaxWidgetsOnScreen = 8;
    Setup();
}

// UNSOLVED
void CustomTuningScreen::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    if (HelpVisible) {
        HelpTextScroller->HandleNotificationMessage(msg);
    }

    if (msg == FEHASH_INITCOMPLETE) {
        for (TuningSlider *slider = reinterpret_cast<TuningSlider *>(Options.GetHead()); slider != Options.EndOfList();
             slider = reinterpret_cast<TuningSlider *>(slider->GetNext())) {
            slider->UnsetFocus();
        }
    }

    if (!HelpVisible || (msg != __PAD_LEFT__ && msg != __PAD_RIGHT__ && msg != __PAD_UP__ && msg != __PAD_DOWN__)) {
        UIWidgetMenu::NotificationMessage(msg, pobj, param1, param2);
    }

    switch (msg) {
        case __PAD_START__:
            if (!HelpVisible) {
                ExitWithStart = true;
            } else {
                return;
            }
        case __PAD_ACCEPT__:
            if (!HelpVisible) {
                StoreSettings();
                cFEng::Get()->QueuePackageMessage(0x587C018B, GetPackageName(), nullptr);
            }
            break;
        case __PAD_LEFT__:
        case __PAD_RIGHT__:
            if (!HelpVisible) {
                TuningSlider *slider = reinterpret_cast<TuningSlider *>(pCurrentOption);
                TempTuningRecord.SetTuning(slider->TuningPath, static_cast<eCustomTuningType>(CurrentTuningType), slider->Current);
            }
            break;
        case __PAD_LTRIGGER__:
            ScrollTypes(eSD_PREV);
            break;
        case __PAD_RTRIGGER__:
            ScrollTypes(eSD_NEXT);
            break;
        case __PAD_BUTTON5__:
            if (!HelpVisible) {
                ShowHelpBlurb();
            }
            break;
        case __PAD_BACK__:
            if (HelpVisible) {
                HideHelpBlurb();
            } else if (SettingsDidNotChange()) {
                cFEng::Get()->QueuePackageMessage(0x587C018B, GetPackageName(), nullptr);
            } else {
                DialogInterface::ShowTwoButtons(GetPackageName(), "InGameDialog.fng", dialog_alert, LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO,
                                                0x775DBA97, dialog_message_no, dialog_message_no, first_dialog_button2,
                                                GetLocalizedString(0xE9CB802F));
            }
            break;
        case 0x775DBA97:
            cFEng::Get()->QueuePackageMessage(0x587C018B, GetPackageName(), nullptr);
            break;
        case FEHASH_EXITCOMPLETE:
            if (ExitWithStart) {
                new EUnPause();
            } else {
                cFEng::Get()->QueuePackageSwitch("Pause_Main.fng", 0, 0, false);
            }
            break;
    }
}

void CustomTuningScreen::ScrollTypes(eScrollDir dir) {
    if (HelpVisible) {
        return;
    }

    int tuning_type = CurrentTuningType;

    if (dir == eSD_NEXT) {
        tuning_type++;
        if (tuning_type > 2) {
            tuning_type = 0;
        }
    } else if (dir == eSD_PREV) {
        tuning_type--;
        if (tuning_type < 0) {
            tuning_type = 2;
        }
    }

    if (tuning_type != CurrentTuningType) {
        CurrentTuningType = tuning_type;
        SetSlidersForType();
    }
}

void CustomTuningScreen::DrawSettingName(uint32 tuning_type) {
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

bool CustomTuningScreen::IsTuningAvailable(FEPlayerCarDB *stable, FECarRecord *record, Physics::Tunings::Path path) {
    if (record == nullptr) {
        return false;
    }

    FECustomizationRecord *custom_record = stable->GetCustomizationRecordByHandle(record->Customization);
    if (custom_record == nullptr) {
        return false;
    }

    switch (path) {
        case Physics::Tunings::STEERING:
        case Physics::Tunings::HANDLING:
        case Physics::Tunings::RIDEHEIGHT:
            return custom_record->GetInstalledPhysics()->Part[Physics::Upgrades::PUT_CHASSIS] > 0;
        case Physics::Tunings::BRAKES:
            return custom_record->GetInstalledPhysics()->Part[Physics::Upgrades::PUT_BRAKES] > 0;
        case Physics::Tunings::AERODYNAMICS: {
            RideInfo info;
            stable->BuildRideForPlayer(record->Handle, 0, &info);
            info.SetStockParts();

            CarPart *stock_part = info.GetPart(CARSLOTID_SPOILER);
            CarPart *part = custom_record->GetInstalledPart(record->GetType(), CARSLOTID_SPOILER);
            if (part != nullptr && part != stock_part) {
                return true;
            }

            stock_part = info.GetPart(CARSLOTID_BODY);
            part = custom_record->GetInstalledPart(record->GetType(), CARSLOTID_BODY);
            if (part != nullptr && part != stock_part) {
                return true;
            }
            return false;
        }
        case Physics::Tunings::NOS:
            return custom_record->GetInstalledPhysics()->Part[Physics::Upgrades::PUT_NOS] > 0;
        case Physics::Tunings::INDUCTION:
            return custom_record->GetInstalledPhysics()->Part[Physics::Upgrades::PUT_INDUCTION] > 0;
        default:
            return false;
    }
}

uint32 CustomTuningScreen::GetNameForPath(Physics::Tunings::Path path, bool turbo) {
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

uint32 CustomTuningScreen::GetHelpForPath(Physics::Tunings::Path path, bool active, bool turbo) {
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
        }
    } else {
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
        }
    }
    return 0;
}

uint32 CustomTuningScreen::AddTuningSlider(FEPlayerCarDB *stable, FECarRecord *record, Physics::Tunings::Path path, bool turbo) {
    bool active = IsTuningAvailable(stable, record, path);
    TuningSlider *slider = new ("TuningSlider", 0) TuningSlider(path, GetNameForPath(path, turbo), GetHelpForPath(path, active, turbo), active);
    char sztemp[64];

    slider->SetTitleObject(GetCurrentFEString(pTitleName));
    slider->SetDataObject(GetCurrentFEString(pDataName));
    slider->SetBacking(GetCurrentFEObject(pBackingName));
    FEngSNPrintf(sztemp, sizeof(sztemp), "%s%d", pSliderName, iIndexToAdd);
    slider->InitSliderObjects(GetPackageName(), sztemp);
    FEngSNPrintf(sztemp, sizeof(sztemp), "%sGROUP%d", pSliderName, iIndexToAdd);
    slider->SetSliderGroup(GetPackageName(), FEngHashString(sztemp));
    slider->SetLeftImage(GetCurrentFEImage(pLeftArrowName));
    slider->SetRightImage(GetCurrentFEImage(pRightArrowName));
    Options.AddTail(slider);
    iIndexToAdd++;
    IncrementStartPos();

    float img_left = FEngGetTopLeftX(slider->GetRightImage());
    float img_right = img_left + FEngGetSizeX(slider->GetRightImage());

    slider->SetWidth(bAbs(img_right - img_left));

    return iIndexToAdd - 1;
}

// UNSOLVED
void CustomTuningScreen::Setup() {
    uint32 player_car;

    if (FEDatabase->IsCareerMode()) {
        player_car = FEDatabase->GetCareerSettings()->GetCurrentCar();
    } else {
        player_car = FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->GetSelectedCar(0);
    }

    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECarRecord *record = stable->GetCarRecordByHandle(player_car);
    TuningRecord = stable->GetCustomizationRecordByHandle(record->Customization);
    if (TuningRecord != nullptr) {
        TempTuningRecord = *TuningRecord;
        CurrentTuningType = TuningRecord->GetActiveTuning();
    }

    AddTuningSlider(stable, record, Physics::Tunings::STEERING, false);
    AddTuningSlider(stable, record, Physics::Tunings::HANDLING, false);
    AddTuningSlider(stable, record, Physics::Tunings::BRAKES, false);
    AddTuningSlider(stable, record, Physics::Tunings::RIDEHEIGHT, false);
    AddTuningSlider(stable, record, Physics::Tunings::AERODYNAMICS, false);
    AddTuningSlider(stable, record, Physics::Tunings::NOS, false);

    Attrib::Gen::pvehicle vehicle(record->VehicleKey, 0, nullptr);

    Physics::Upgrades::SetLevel(vehicle, Physics::Upgrades::PUT_INDUCTION, 1);

    bool turbo = Physics::Info::InductionType(vehicle) == Physics::Info::INDUCTION_TURBO_CHARGER;

    if (turbo) {
        AddTuningSlider(stable, record, Physics::Tunings::INDUCTION, true);
    } else {
        AddTuningSlider(stable, record, Physics::Tunings::INDUCTION, false);
    }

    SetSlidersForType();
    SetInitialOption(1);
}

void CustomTuningScreen::SetSlidersForType() {
    DrawSettingName(CurrentTuningType);

    const Physics::Tunings *tunings = TempTuningRecord.GetTunings(CurrentTuningType);

    for (TuningSlider *slider = static_cast<TuningSlider *>(Options.GetHead()); slider != Options.EndOfList();
         slider = static_cast<TuningSlider *>(slider->GetNext())) {
        float lower_limit = Physics::Tunings::LowerLimit(slider->TuningPath);
        float upper_limit = Physics::Tunings::UpperLimit(slider->TuningPath);
        float inc = (upper_limit - lower_limit) * 0.1f;

        slider->SetSliderValues(lower_limit, upper_limit, inc, tunings->Value[slider->TuningPath]);
        slider->Draw();
    }
}

void CustomTuningScreen::ShowHelpBlurb() {
    TuningSlider *slider = static_cast<TuningSlider *>(pCurrentOption);

    const uint32 SHOWHELP = 0x89D332A9;
    cFEng::Get()->QueuePackageMessage(SHOWHELP, GetPackageName(), nullptr);
    pCurrentOption->UnsetFocus();
    if (HelpTextScroller == nullptr) {
        FEString *pString = FEngFindString(GetPackageName(), FEHashUpper("HELP_TEXT_LINE1"));

        HelpTextScroller = new CTextScroller();
        HelpTextScroller->Initialise(this, pString->MaxWidth, 7, "HELP_TEXT_LINE%d", FindFont(pString->Handle));
        HelpTextScroller->UseScrollBar(HelpScrollBar);
        HelpTextScroller->SetTextHash(slider->HelpBlurb);
    }
    HelpVisible = true;
}

void CustomTuningScreen::HideHelpBlurb() {
    const uint32 HIDEHELP = 0x950AD1C2;
    cFEng::Get()->QueuePackageMessage(HIDEHELP, GetPackageName(), nullptr);
    pCurrentOption->SetFocus(GetPackageName());
    if (HelpTextScroller != nullptr) {
        delete HelpTextScroller;
        HelpTextScroller = nullptr;
    }
    HelpVisible = false;
}

void CustomTuningScreen::StoreSettings() {
    for (int i = 0; i < NUM_CUSTOM_TUNINGS; i++) {
        for (int j = 0; j < Physics::Tunings::MAX_TUNINGS; j++) {
            TuningRecord->SetTuning(static_cast<Physics::Tunings::Path>(j), static_cast<eCustomTuningType>(i),
                                    TempTuningRecord.GetTunings(i)->Value[j]);
        }
    }

    TuningRecord->SetActiveTuning(static_cast<eCustomTuningType>(CurrentTuningType));
    new ETuneVehicle(0, TuningRecord->GetTunings());
}

bool CustomTuningScreen::SettingsDidNotChange() {
    for (int i = 0; i < NUM_CUSTOM_TUNINGS; i++) {
        for (int j = 0; j < Physics::Tunings::MAX_TUNINGS; j++) {
            float original_setting = TuningRecord->GetTunings(static_cast<eCustomTuningType>(i))->Value[j];
            float new_setting = TempTuningRecord.GetTunings(static_cast<eCustomTuningType>(i))->Value[j];

            if (original_setting != new_setting) {
                return false;
            }
        }
    }

    return CurrentTuningType == TuningRecord->GetActiveTuning();
}
