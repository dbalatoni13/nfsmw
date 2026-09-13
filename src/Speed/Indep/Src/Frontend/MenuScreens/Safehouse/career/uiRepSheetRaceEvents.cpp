#include "uiRepSheetRaceEvents.hpp"

#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiTrackMapStreamer.hpp"
#include "Speed/Indep/Src/Frontend/RaceStarter.hpp"
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Generated/Events/ERaceSheetOff.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

extern int iCurrentViewBin; // TODO remove
GRaceParameters *theRace = nullptr;

void RaceDatum::NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) {
    if (msg == __BUTTON_PRESSED__) {
        if (!IsLocked()) {
            theRace = race;
        }
    }
}

UISafehouseRaceSheet::UISafehouseRaceSheet(ScreenConstructorData *sd) : ArrayScrollerMenu(sd, 3, 3, true) {
    bIsInGame = sd->Arg != 0;
    currentEvents = true;
#ifndef EA_BUILD_A124
    currentIndex = 0;
#endif
    if (!bIsInGame) {
        GarageMainScreen::GetInstance()->CancelCameraPush();
    }
    theRace = nullptr;

    for (int i = 0; i < GetWidth() * GetHeight(); i++) {
        FEImage *img = FEngFindImage(GetPackageName(), FEngHashString("EVENT_ICON_%d", i + 1));
        if (img != nullptr) {
            AddSlot(new ("ImageArraySlot", 0) ImageArraySlot(img));
        }
    }
    TrackMap = reinterpret_cast<FEMultiImage *>(FEngFindObject(GetPackageName(), FEngHashString("TRACK_MAP")));
    if (bIsInGame) {
        FEngSetLanguageHash(GetPackageName(), 0xbde82fcc, 0x2f32a021);
    } else {
        FEngSetLanguageHash(GetPackageName(), 0xbde82fcc, 0x84e4a54c);
    }
    Setup();
}

UISafehouseRaceSheet::~UISafehouseRaceSheet() {}

eMenuSoundTriggers UISafehouseRaceSheet::NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) {
    eMenuSoundTriggers result = ArrayScrollerMenu::NotifySoundMessage(msg, maybe);
    if (msg == 0x7b6b89d7 && (theRace == nullptr)) {
        return UISND_COMMON_WRONG;
    }
    return result;
}

void UISafehouseRaceSheet::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    ArrayScrollerMenu::NotificationMessage(msg, obj, param1, param2);
    // UNSOLVED
    switch (msg) {
        case FEMSG_SCREEN_TICK:
            TrackMapStreamer.UpdateAnimation();
            break;
        case __PAD_UP__:
        case __PAD_LEFT__:
        case __PAD_DOWN__:
        case __PAD_RIGHT__:
            RefreshHeader();
            break;
        case __PAD_LTRIGGER__:
        case __PAD_RTRIGGER__:
            ToggleList();
            break;
        case __BUTTON_PRESSED__: {
            if (theRace == nullptr) {
                break;
            }
            const char *dialog = "";
            if (!bIsInGame) {
                int joyPort = FEngMapJoyParamToJoyport(param1);
                FEDatabase->SetPlayersJoystickPort(0, joyPort);
            }
            if (bIsInGame) {
                dialog = "InGameDialog.fng";
            }
            DialogInterface::ShowTwoButtons(GetPackageName(), dialog, dialog_alert, LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO, dialog_message_yes,
                                            dialog_message_no, dialog_message_no, first_dialog_button2, LANGUAGE_REP_SHEET_RACE_CONFIRM);
            break;
        }
        case dialog_message_yes:
            if (bIsInGame) {
                new ERaceSheetOff();
                GManager::Get().StartRaceFromInGame(theRace->GetEventHash());
            } else {
                GRaceCustom *race = GRaceDatabase::Get().AllocCustomRace(theRace);
                GRaceDatabase::Get().SetStartupRace(race, GRace::kRaceContext_Career);
                GRaceDatabase::Get().FreeCustomRace(race);
                RaceStarter::StartRace();
            }
            break;
        case __PAD_BACK__:
            if (bIsInGame) {
                cFEng::Get()->QueuePackageSwitch("InGameReputationOverview.fng", 1, 0, false);
            } else {
                cFEng::Get()->QueuePackageSwitch("SafeHouseReputationOverview.fng", 0, 0, false);
            }
            break;
        case dialog_message_no:
            break;
    }
}

// UNSOLVED
void UISafehouseRaceSheet::RefreshHeader() {
    ArrayScrollerMenu::RefreshHeader();

    FEPrintf(GetPackageName(), 0x5a856a34, "%d", GetCurrentDatumNum());
    FEPrintf(GetPackageName(), 0x2d4d22c8, "%d", GetNumDatum());
    uint32 hash = 0x6475236d;
    if (currentEvents) {
        hash = 0xc948ef80;
    }
    FEngSetLanguageHash(GetPackageName(), 0x78008599, hash);
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FEPrintf(GetPackageName(), 0xb514e2d8, "%s %$d", GetLocalizedString(0xce6b99b1), stable->GetTotalBounty());
    FEPrintf(GetPackageName(), 0xf91a59f6, "%s %$d", GetLocalizedString(0x73b79e0), FEDatabase->GetCareerSettings()->GetCash());
    if (GetCurrentDatum() == nullptr) {
        return;
    }
    GRaceParameters *race = static_cast<RaceDatum *>(GetCurrentDatum())->race;
    FEPrintf(GetPackageName(), 0x13c45e, "%.0f", race->GetCashValue());
    const char *distUnits;
    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        distUnits = GetLocalizedString(0x8569a26a);
    } else {
        distUnits = GetLocalizedString(0x867dcfd9);
    }
    FEPrintf(GetPackageName(), 0x18b36f, "%d", race->GetNumLaps());
    FEPrintf(GetPackageName(), 0x80c9daa, "%$0.1f %s", race->GetRaceLengthMeters() * 0.001f, distUnits);
    FEngSetLanguageHash(GetPackageName(), 0xf2cd475, CalcLanguageHash("TRACKNAME_", race));
    FEngSetLanguageHash(GetPackageName(), 0x9b21, race->GetCopsEnabled() ? 0x61d1c5a5 : 0x73c615a3);
    FEngSetInvisible(FEngFindObject(GetPackageName(), 0x1c8fc866));
    FEngSetInvisible(FEngFindObject(GetPackageName(), 0x7af67920));
    FEngSetInvisible(FEngFindObject(GetPackageName(), 0xbbf970cd));

    GRaceSaveInfo *info = GRaceDatabase::Get().GetScoreInfo(race->GetEventHash());
    if (race->GetRaceType() == GRace::kRaceType_P2P || race->GetRaceType() == GRace::kRaceType_Circuit ||
        race->GetRaceType() == GRace::kRaceType_Drag || race->GetRaceType() == GRace::kRaceType_Knockout ||
        race->GetRaceType() == GRace::kRaceType_Tollbooth) {
        if (info->mHighScores.mBestTime == 0.0f) {
            FEPrintf(GetPackageName(), 0x8fd41bb4, GetLocalizedString(0x472aa00a));
        } else {
            char buf[64];
            Timer(info->mHighScores.mBestTime).PrintToString(buf, 0);
            FEPrintf(GetPackageName(), 0x8fd41bb4, "%s", buf);
        }
    } else {
        FEPrintf(GetPackageName(), 0x8fd41bb4, "%s", GetLocalizedString(0x472aa00a));
    }
    float avg_speed;
    float top_speed;
    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        distUnits = GetLocalizedString(0x8569a25f);
        avg_speed = MPS2KPH(info->mAverageSpeed);
        top_speed = MPS2KPH(info->mTopSpeed);
    } else {
        distUnits = GetLocalizedString(0x8569ab44);
        avg_speed = MPS2MPH(info->mAverageSpeed);
        top_speed = MPS2MPH(info->mTopSpeed);
    }
    FEPrintf(GetPackageName(), 0xebd7f926, "%$0.2f %s", top_speed, distUnits);
    FEPrintf(GetPackageName(), 0xde9145fb, "%$0.2f %s", avg_speed, distUnits);
    FEPrintf(GetPackageName(), 0x763f4b5b, "%$0.0f", race->GetCashValue());
    FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xf97ec5d5), FEDatabase->GetRaceIconHash(race->GetRaceType()));

    for (int i = 0; i < GetNumSlots(); i++) {
        RaceDatum *datum = static_cast<RaceDatum *>(GetDatumAt(i + GetStartDatumNum()));
        uint32 check_hash = FEngHashString("MEDAL_THUMB_%d", i + 1);
        FEngSetInvisible(FEngFindObject(GetPackageName(), check_hash));
        if (datum == nullptr) {
            continue;
        }
        if (datum->IsLocked()) {
            FEngSetVisible(FEngFindObject(GetPackageName(), check_hash));
            FEngSetTextureHash(FEngFindImage(GetPackageName(), check_hash), 0x18ed48);
        } else if (datum->IsChecked()) {
            FEngSetVisible(FEngFindObject(GetPackageName(), check_hash));
            FEngSetTextureHash(FEngFindImage(GetPackageName(), check_hash), 0x28feadd);
        }
    }
#ifndef EA_BUILD_A124
    if (currentIndex != GetCurrentDatumNum() - 1 && GetCurrentDatum() != nullptr) {
        TrackMapStreamer.Init(static_cast<RaceDatum *>(GetCurrentDatum())->race, TrackMap, 0, 0);
        currentIndex = GetCurrentDatumNum() - 1;
    }
#endif
}

bool UISafehouseRaceSheet::AddRace(GRaceParameters *race) {
    GRace::Type type = race->GetRaceType();
    switch (type) {
        case GRace::kRaceType_JumpToSpeedTrap:
        case GRace::kRaceType_JumpToMilestone:
            return false;
        default:
            AddDatum(new ("RaceDatum", 0)
                         RaceDatum(FEDatabase->GetRaceIconHash(race->GetRaceType()), FEDatabase->GetRaceNameHash(race->GetRaceType()), race));
            return true;
    }
}

void UISafehouseRaceSheet::Setup() {
    ClearData();
    if (currentEvents) {
        GRaceBin *bin = GRaceDatabase::Get().GetBinNumber(iCurrentViewBin);
        for (unsigned int i = 0; i < bin->GetWorldRaceCount(); i++) {
            unsigned int raceHash = bin->GetWorldRaceHash(i);
            GRaceParameters *race = GRaceDatabase::Get().GetRaceFromHash(raceHash);
            if (AddRace(race)) {
                GetDatumAt(GetNumDatum() - 1)->SetLocked(false);
                if (GRaceDatabase::Get().IsCareerRaceComplete(raceHash)) {
                    GetDatumAt(GetNumDatum() - 1)->SetChecked(true);
                }
            }
        }
    } else {
        unsigned int bindex = FEDatabase->GetCareerSettings()->GetCurrentBin();
        while (bindex <= GRaceDatabase::Get().GetBinCount()) {
            GRaceBin *bin = GRaceDatabase::Get().GetBinNumber(bindex);
            if (bin != nullptr) {
                for (unsigned int i = 0; i < bin->GetWorldRaceCount(); i++) {
                    GRaceParameters *race = GRaceDatabase::Get().GetRaceFromHash(bin->GetWorldRaceHash(i));
                    if (AddRace(race)) {
                        GetDatumAt(GetNumDatum() - 1)->SetLocked(false);
                    }
                }
            }
            bindex++;
        }
    }
    SetDescLabel(0x9ba78ba2);
    if (GetCurrentDatum() != nullptr) {
        TrackMapStreamer.Init(static_cast<RaceDatum *>(GetCurrentDatum())->race, TrackMap, 0, 0);
    }
    SetInitialPosition(0);
    RefreshHeader();
}

void UISafehouseRaceSheet::ToggleList() {}
